#pragma once

#ifndef __EXTERNAL_METHODS_H__
#define __EXTERNAL_METHODS_H__

#include "ncorr.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>


namespace ncorr {

#define my_assert(cond, err_str) \
	if (!(cond)) {				 \
		throw err_str;			 \
	}

typedef byte STRAIN_LOG_TYPE;

namespace STRAIN_LOG {
	const STRAIN_LOG_TYPE NONE = 0;   // 00000000
	const STRAIN_LOG_TYPE MAX  = 1;   // 00000001
	const STRAIN_LOG_TYPE AVG  = 2;   // 00000010
	const STRAIN_LOG_TYPE ALL  = 255; // 11111111
};


#define IND2Y(ind, width)   ind / width
#define IND2X(ind, width)   ind % width


void save_strain_logs(
	const std::string& filepath,
	const strain_analysis_output& strain_output,
	STRAIN strain_type,
	STRAIN_LOG_TYPE strain_log_type,
	__int64 point_index = -1, // use only if u also want to calculate in point (index of point: y*width + x) if -1 values will not be calculated,
	const char* file_suffix = nullptr
) {

	typedef ROI2D::difference_type difference_type;

	// Use get_strain function to obtain the specified strain field
	std::function<const Data2D& (const Strain2D&)> get_strain;
	std::string file_prefix;
	switch (strain_type) {
	case STRAIN::EYY: {
		get_strain = &Strain2D::get_eyy;
		file_prefix = "eyy";
	} break;
	case STRAIN::EXY: {
		get_strain = &Strain2D::get_exy;
		file_prefix = "exy";
	} break;
	case STRAIN::EXX: {
		get_strain = &Strain2D::get_exx;
		file_prefix = "exx";
	} break;
	}
	if (file_suffix) {
		file_prefix += file_suffix;
	}

	Array2D<double> data_values_first = get_strain(strain_output.strains.front()).get_array()(strain_output.strains.front().get_roi().get_mask());
	Array2D<double> data_values_last = get_strain(strain_output.strains.back()).get_array()(strain_output.strains.back().get_roi().get_mask());
	double min_strain = std::min(prctile(data_values_first, 0.01), prctile(data_values_last, 0.01));
	double max_strain = std::max(prctile(data_values_first, 0.99), prctile(data_values_last, 0.99));

	// Get data plots
	std::vector<Data2D> data;
	for (difference_type strain_idx = 0; strain_idx < difference_type(strain_output.strains.size()); ++strain_idx) {
		data.push_back(get_strain(strain_output.strains[strain_idx]));
	}
	my_assert(!(data.empty()), "Empty data");

	//get avg and abs max values on every picture
	std::stringstream maxss, avgss, dotss;
	difference_type data_heigth = data[0].get_array().height();
	difference_type data_width  = data[0].get_array().width();
	
	std::cout << "data size: width=" << data_width << " heigth=" << data_heigth << std::endl;
	my_assert(point_index >= 0, "Incorrect point index. Must be >= 0");
	my_assert(point_index < data_heigth * data_width, "Incorrect point index. Out of data size");
	
	for (difference_type strain_idx = 0; strain_idx < data.size(); ++strain_idx) {
		Array2D frame_data_array = data[strain_idx].get_array();
		difference_type array_size = frame_data_array.size();
		double frame_max_strain = (fabs(min_strain) < fabs(max_strain)) ? min_strain : max_strain;
		double frame_avg_strain = 0;
		difference_type i = 0;
		for (difference_type idx = 0; idx < array_size; ++idx) {
			auto value = frame_data_array(idx);
			if (fabs(value) > fabs(frame_max_strain)) {
				frame_max_strain = value;
				i = idx;
			}
			frame_avg_strain += value;
		}
		if (strain_log_type & STRAIN_LOG::MAX) {
			maxss << std::setw(20) << frame_max_strain << "        " << std::setw(10) << IND2X(i, data_width) << "\t" << IND2Y(i, data_width) << std::endl;
		}
		if (strain_log_type & STRAIN_LOG::AVG) {
			avgss << frame_avg_strain / array_size << std::endl;
		}
		if (point_index != -1) {
			dotss << frame_data_array(point_index) << std::endl;
		}
	}
	if (strain_log_type & STRAIN_LOG::MAX) {
		std::ofstream f_max(filepath + "\\log_" + file_prefix + "_strain_max.txt");
		f_max << maxss.str();
		f_max.close();
	}
	if (strain_log_type & STRAIN_LOG::AVG) {
		std::ofstream f_avg(filepath + "\\log_" + file_prefix + "_strain_avg.txt");
		f_avg << avgss.str();
		f_avg.close();
	}
	if (point_index != -1) {
		char buf[16];
		std::ofstream f_point(filepath + "\\log_" + file_prefix + "_strain_point" + itoa(point_index, buf, 10) + ".txt");
		f_point << dotss.str();
		f_point.close();
	}

} // function save_strain_logs

void save_DIC_logs(
	const std::string& filepath,
	const DIC_analysis_output& DIC_output,
	DISP disp_type,
	__int64 point_index, // use only if u also want to calculate in point (index of point: y*width + x) if -1 values will not be calculated
	const char* file_suffix = nullptr
) {

	typedef ROI2D::difference_type difference_type;

    // Use get_disp() function to obtain the specified displacement field
    std::function<const Data2D&(const Disp2D&)> get_disp;
	std::string file_prefix;
    switch (disp_type) {
        case DISP::V: {
            get_disp = &Disp2D::get_v;
			file_prefix = "v";
		} break;
        case DISP::U: {
            get_disp = &Disp2D::get_u;
			file_prefix = "u";
		} break;            
    }
	if (file_suffix) {
		file_prefix += "_";
		file_prefix += file_suffix;
	}

    // Get data plots
    std::vector<Data2D> data;
    for (difference_type disp_idx = 0; disp_idx < difference_type(DIC_output.disps.size()); ++disp_idx) {
        data.push_back(get_disp(DIC_output.disps[disp_idx]));
    }
	my_assert(!(data.empty()), "Empty data");

	//get avg and abs max values on every picture
	std::stringstream value_ss;
	difference_type data_heigth = data[0].get_array().height();
	difference_type data_width  = data[0].get_array().width();

	std::cout << "data size: width=" << data_width << " heigth=" << data_heigth << std::endl;
	my_assert(point_index >= 0, "Incorrect point index. Must be > 0");
	my_assert(point_index < data_heigth * data_width, "Incorrect point index. Out of data size");

	for (difference_type strain_idx = 0; strain_idx < data.size(); ++strain_idx) {
		Array2D frame_data_array = data[strain_idx].get_array();
		value_ss << frame_data_array(point_index) << std::endl;
	}

	char buf[16];
	std::ofstream f_point(filepath + "\\log_" + file_prefix + "_DIC_point" + itoa(point_index, buf, 10) + ".txt");
	f_point << value_ss.str();
	f_point.close();

} // function save_strain_logs

} // namespace ncorr

#endif // !__EXTERNAL_METHODS_H__
