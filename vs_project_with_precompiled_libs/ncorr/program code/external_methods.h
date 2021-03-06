#pragma once


#ifndef __EXTERNAL_METHODS_H__
#define __EXTERNAL_METHODS_H__


#include "ncorr.h"
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>


namespace ncorr {



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
		__int64 point_index = -1 // use only if u also want to calculate in point (index of point: y*width + x) if -1 values will not be calculated
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

		Array2D<double> data_values_first = get_strain(strain_output.strains.front()).get_array()(strain_output.strains.front().get_roi().get_mask());
		Array2D<double> data_values_last = get_strain(strain_output.strains.back()).get_array()(strain_output.strains.back().get_roi().get_mask());
		double min_strain = std::min(prctile(data_values_first, 0.01), prctile(data_values_last, 0.01));
		double max_strain = std::max(prctile(data_values_first, 0.99), prctile(data_values_last, 0.99));

		// Get data plots
		std::vector<Data2D> data;
		for (difference_type strain_idx = 0; strain_idx < difference_type(strain_output.strains.size()); ++strain_idx) {
			data.push_back(get_strain(strain_output.strains[strain_idx]));
		}

		//get avg and abs max values on every picture
		std::stringstream maxstr, avgstr, dot_str;
		difference_type data_heigth = data[0].get_array().height();
		difference_type data_width  = data[0].get_array().width();
		std::cout << "data size: width=" << data_width << " heigth=" << data_heigth << std::endl;
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
				maxstr << std::setw(20) << frame_max_strain << "        " << std::setw(10) << IND2X(i, data_width) << "\t" << IND2Y(i, data_width) << std::endl;
			}
			if (strain_log_type & STRAIN_LOG::AVG) {
				avgstr << frame_avg_strain / array_size << std::endl;
			}
			if (point_index != -1) {
				dot_str << frame_data_array(point_index) << std::endl;
			}
		}
		if (strain_log_type & STRAIN_LOG::MAX) {
			std::ofstream f_max(filepath + "\\log_" + file_prefix + "_strain_max.txt");
			f_max << maxstr.str();
			f_max.close();
		}
		if (strain_log_type & STRAIN_LOG::AVG) {
			std::ofstream f_avg(filepath + "\\log_" + file_prefix + "_strain_avg.txt");
			f_avg << avgstr.str();
			f_avg.close();
		}
		if (point_index != -1) {
			char buf[16];
			std::ofstream f_point(filepath + "\\log_" + file_prefix + "_strain_point" + itoa(point_index, buf, 10) + ".txt");
			f_point << dot_str.str();
			f_point.close();
		}

	} // function save_strain_logs



} // namespace ncorr


#endif // !__EXTERNAL_METHODS_H__
