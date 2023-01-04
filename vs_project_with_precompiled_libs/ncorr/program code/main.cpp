/*
* Using
* Ncorr: open-source 2D digital image correlation matlab software
* J Blaber, B Adair, A Antoniou
* Experimental Mechanics 55 (6), 1105-1122
*/

/**
* TODO:
*	- custom name of out folder
*		set the out folder name or path
*		the frolder should be created if does not exists
*		the folder should be cleaned before calculations
*	- make new cmd parsing system (like in perfetto)
*	- clean main
*		move steps in external functions
*		move some definitions to header
*		refactor _my_params.h
*	- add CMakes
*	- make external libs CMake dependencies or add repos' ink to git system
*	- make GUI
*/


#define _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
#define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING
#pragma warning (disable : 4996)


#include <Windows.h>
#include "conio.h"
#include <cstdlib>
#include <filesystem>
namespace fs = std::filesystem;

#include "..\external_libs\ncorr\include\ncorr.h"
#include "_my_parameters.h"
#include "external_methods.h"



#define END_OF_PROG(code) \
	std::cout << "\npress any button";	\
	_getch();							\
	return code;


#define BIN_DIR_NAME "BINARIES_"
#define AVI_DIR_NAME "OUTPUTS_"



const size_t countOfCmds = 13;
const char cmdNames[countOfCmds][18] = { "-p", "-n", "-c", "-f", "-s", "-m", "--fps", "--plot", "--alpha", "--mode", "--pointi", "--color", "--units-per-pixel" };
enum cmdComands { path = 0, name, count, format, skip, multi, fps, plot, alpha, mode, pointi, colormap, units_per_pixel };


typedef byte OUTPUT_MODE_TYPE;

namespace OUTPUT_MODE {
	const OUTPUT_MODE_TYPE LOGS  = 1;     // 00000001
	const OUTPUT_MODE_TYPE PLOTS = 2;     // 00000010
	const OUTPUT_MODE_TYPE ALL   = 255;   // 11111111
}


std::vector<size_t> CmdProc(int argc, char* argv[]) {

	std::vector<size_t> optCmds(countOfCmds);
	for (size_t k = 0; k < countOfCmds; k++) {
		optCmds[k] = 0;
		for (size_t i = 2; i < argc; i++) {
			if (strcmp(argv[i], cmdNames[k]) == 0) {
				optCmds[k] = i + 1;
				break;
			}
		}
	}

	return optCmds;

}



using namespace ncorr;

int main(int argc, char *argv[]) {

	std::cout << INTRO_MESSAGE << std::endl;

try {

	auto cmds = CmdProc(argc, argv);

	if (cmds[path] == 0) {
		throw std::invalid_argument("Must have command input of either 'calculate' or 'load' and then path to the folder");	
	}
	std::string pathToFiles = argv[cmds[path]];

	// Initialize DIC and strain information ---------------//
	DIC_analysis_input     DIC_input;
	DIC_analysis_output    DIC_output;
	strain_analysis_input  strain_input;
	strain_analysis_output strain_output;

	// lagrangian variables @note lagr declaration
	DIC_analysis_output    DIC_output_lagr;
	strain_analysis_input  strain_input_lagr;
	strain_analysis_output strain_output_lagr;

	const auto calculationsStartTime = GetTickCount64();
	char buf[30];

	// Determine whether or not to perform calculations or 
	// load data (only load data if analysis has already 
	// been done and saved or else throw an exception).
	std::string input(argv[1]);
	if (input == "load") {

		// Load inputs
		DIC_input     =     DIC_analysis_input::load(pathToFiles + "\\DIC_input.bin");
		DIC_output    =    DIC_analysis_output::load(pathToFiles + "\\DIC_output.bin");
		strain_input  =  strain_analysis_input::load(pathToFiles + "\\strain_input.bin");
		strain_output = strain_analysis_output::load(pathToFiles + "\\strain_output.bin");

		// @note load lagr inputs (only this will be used)
		DIC_output_lagr    =    DIC_analysis_output::load(pathToFiles + "\\DIC_output_lagr.bin");
		strain_output_lagr = strain_analysis_output::load(pathToFiles + "\\strain_output_lagr.bin");

	} else if (input == "calculate") {

		// Parameters
		raise(cmds[name] && cmds[count], std::invalid_argument("after path to the folder with picture, name of pictures and then count of pictures (format: path/name000.png .. path/name999.png)"));
		const std::string   picturesName   = argv[cmds[name]];
		const size_t		picturesCount  = atoi(argv[cmds[count]]);
		const size_t		fillSize       = CountOfSymbols(picturesCount - 1);
		const std::string   picturesFormat = (cmds[format]) ? argv[cmds[format]]     : "png";
		const size_t        iteratorStep   = (cmds[skip])   ? atoi(argv[cmds[skip]]) : 1;
		const float         unitsPerPixel  = (cmds[units_per_pixel]) ? atof(argv[cmds[units_per_pixel]]) : 0.2;

		// Set images
		std::vector<Image2D> imgs;
		for (size_t i = 0; i < picturesCount; i += iteratorStep) {
			std::ostringstream ostr;
			ostr << pathToFiles << "\\" << picturesName << std::setfill('0') << std::setw(fillSize) << i << "." << picturesFormat;
			imgs.push_back(ostr.str());
		}
		
		// Set DIC_input
		DIC_input = DIC_analysis_input(
			imgs, 										// Images
			ROI2D(Image2D(pathToFiles + "\\roi.png").get_gs() > 0.5),		// ROI
			3,                                         	// scalefactor
			INTERP::QUINTIC_BSPLINE_PRECOMPUTE,			// Interpolation
			SUBREGION::CIRCLE,							// Subregion shape
			20,                                        	// Subregion radius
			4,                                         	// # of threads
			DIC_analysis_config::NO_UPDATE,				// DIC configuration for reference image updates
			true										// Debugging enabled/disabled
		);

		// Perform DIC_analysis    
		DIC_output = DIC_analysis(DIC_input);
		// @note save lagr dic output
		DIC_output_lagr = DIC_output;
		// @remind Convert DIC_output to Eulerian perspective
		DIC_output = change_perspective(DIC_output, INTERP::QUINTIC_BSPLINE_PRECOMPUTE);
		// Set units of DIC_output (provide units/pixel)
		DIC_output = set_units(DIC_output, "mm", unitsPerPixel);
		// Set units of DIC_output (provide units/pixel)
		DIC_output_lagr = set_units(DIC_output_lagr, "mm", unitsPerPixel);

		// Set strain input
		strain_input = strain_analysis_input(
			DIC_input,
			DIC_output,
			SUBREGION::CIRCLE,     // Strain subregion shape
			5                      // Strain subregion radius
		);
		// Perform strain_analysis
		strain_output = strain_analysis(strain_input); 
		
		// @note lagr strain input
		strain_input_lagr = strain_analysis_input(
			DIC_input,
			DIC_output_lagr,
			SUBREGION::CIRCLE,     // Strain subregion shape
			5                      // Strain subregion radius
		);
		// @note lagr strain_analysis
		strain_output_lagr = strain_analysis(strain_input_lagr); 
		
		// Save outputs as binary
		std::string newDirPath = pathToFiles + "\\" + std::string(BIN_DIR_NAME) + itoa(calculationsStartTime, buf, 10);
		fs::create_directories(newDirPath);
		std::system((std::string("mkdir ") + newDirPath).c_str());
		save(DIC_input,     newDirPath + "\\DIC_input.bin");
		save(DIC_output,    newDirPath + "\\DIC_output.bin");
		save(strain_input,  newDirPath + "\\strain_input.bin");
		save(strain_output, newDirPath + "\\strain_output.bin");

		// @note save lagr outputs
		save(DIC_output_lagr,    newDirPath + "\\DIC_output_lagr.bin");
		save(strain_output_lagr, newDirPath + "\\strain_output_lagr.bin");

	} else {
		throw std::invalid_argument("Input of " + input + " is not recognized. Must be either 'calculate' or 'load'");	
	}		

	// @todo move default values to another place
	const int   plotFPS    = (cmds[fps])   ? atoi(argv[cmds[fps]])   : 15;
	const int   plotFormat = (cmds[plot])  ? atoi(argv[cmds[plot]])  : 0;
	const float plotAlpha  = (cmds[alpha]) ? atof(argv[cmds[alpha]]) : 0.5;
	const OUTPUT_MODE_TYPE outputMode = (cmds[mode]) ? atoi(argv[cmds[mode]]) : OUTPUT_MODE::ALL;
	cv::ColormapTypes colormapType = cv::COLORMAP_JET;
	if (cmds[colormap]) {
		switch (atoi(argv[cmds[colormap]]))
		{
		case 2: {
			colormapType = cv::COLORMAP_BONE;
		} break;
		case 1:
		default: {
			colormapType = cv::COLORMAP_JET;
		} break;
		}
	}

        
	// Create Videos ---------------------------------------//
	// Note that more inputs can be used to modify plots. 
	// If video is not saving correctly, try changing the 
	// input codec using cv::VideoWriter::fourcc(...)). Check 
	// the opencv documentation on video codecs. By default, 
	// ncorr uses cv::VideoWriter::fourcc('M','J','P','G')).

	std::string newDirPath = pathToFiles + "\\" + std::string(AVI_DIR_NAME) + itoa(calculationsStartTime, buf, 10);
	fs::create_directories(newDirPath);

	if (outputMode & OUTPUT_MODE::LOGS) {

		const int point_index = (cmds[pointi]) ? atoi(argv[cmds[pointi]]) : -1;

		try {

			// save eulerian strain logs
			save_strain_logs(
				newDirPath,
				strain_output,
				STRAIN::EXX,
				STRAIN_LOG::ALL,
				point_index
			);
			save_strain_logs(
				newDirPath,
				strain_output,
				STRAIN::EYY,
				STRAIN_LOG::ALL,
				point_index
			);
			save_strain_logs(
				newDirPath,
				strain_output,
				STRAIN::EXY,
				STRAIN_LOG::ALL,
				point_index
			);

			// @note save lagr strain logs
			save_strain_logs(
				newDirPath,
				strain_output_lagr,
				STRAIN::EXX,
				STRAIN_LOG::ALL,
				point_index,
				"lagr"
			);
			save_strain_logs(
				newDirPath,
				strain_output_lagr,
				STRAIN::EYY,
				STRAIN_LOG::ALL,
				point_index,
				"lagr"
			);
			save_strain_logs(
				newDirPath,
				strain_output_lagr,
				STRAIN::EXY,
				STRAIN_LOG::ALL,
				point_index,
				"lagr"
			);

			// save DIC logs
			save_DIC_logs(
				newDirPath,
				DIC_output_lagr,
				DISP::U,
				point_index
			);
			save_DIC_logs(
				newDirPath,
				DIC_output_lagr,
				DISP::V,
				point_index
			);

		}
		catch (const char* e) {
			std::cout << "Can't perform table data saving\n" << e << std::endl;
		}

	}

	if (outputMode & OUTPUT_MODE::PLOTS) {

		save_DIC_video(
			newDirPath + "\\plot_v_eulerian.avi",
			DIC_input,
			DIC_output,
			DISP::V,
			plotAlpha,		// Alpha		
			plotFPS,
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(),
			plotFormat >= 1,
			plotFormat >= 2,
			plotFormat >= 3,
			-1.0,
			1.0,
			11,
			colormapType
		);

		save_DIC_video(
			newDirPath + "\\plot_u_eulerian.avi",
			DIC_input,
			DIC_output,
			DISP::U,
			plotAlpha,		// Alpha
			plotFPS,
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(),
			plotFormat >= 1,
			plotFormat >= 2,
			plotFormat >= 3,
			- 1.0,
			1.0,
			11,
			colormapType
		);

		save_strain_video(
			newDirPath + "\\plot_eyy_eulerian.avi",
			strain_input,
			strain_output,
			STRAIN::EYY,
			plotAlpha,		// Alpha
			plotFPS,
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(),
			plotFormat >= 1,
			plotFormat >= 2,
			plotFormat >= 3,
			-1.0,
			1.0,
			11,
			colormapType
		);

		save_strain_video(
			newDirPath + "\\plot_exy_eulerian.avi",
			strain_input,
			strain_output,
			STRAIN::EXY,
			plotAlpha,		// Alpha
			plotFPS,
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(),
			plotFormat >= 1,
			plotFormat >= 2,
			plotFormat >= 3,
			-1.0,
			1.0,
			11,
			colormapType
		);

		save_strain_video(
			newDirPath + "\\plot_exx_eulerian.avi",
			strain_input,
			strain_output,
			STRAIN::EXX,
			plotAlpha,		// Alpha
			plotFPS,
			std::numeric_limits<double>::quiet_NaN(),
			std::numeric_limits<double>::quiet_NaN(),
			plotFormat >= 1,
			plotFormat >= 2,
			plotFormat >= 3,
			-1.0,
			1.0,
			11,
			colormapType
		);

	}

} catch (std::invalid_argument e) {
	std::cout << "___ERROR___\n" << e.what() << std::endl;
	END_OF_PROG(1);
}

	END_OF_PROG(0);
}
