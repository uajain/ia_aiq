/* main.c to see what all we can call from ia_imaging.
 * NULLs in input parameters are mostly optional parameters, skipped for now.
 */

/* Compile via:
 * g++ -L/home/uajain/Downloads/intel-3a-libs-bin-2019.08.26/usr/lib64 main.cpp -lia_aiq
 */
#include <iostream>

#include <ia_imaging/ia_aiq.h>

int show_err_if_any(ia_err iaErr)
{
	switch (iaErr) {
	case ia_err_general:
		std::cout << "UNKNOWN_ERROR\n";
		break;
	case ia_err_nomemory:
		std::cout << "NO_MEMORY\n";
		break;
	case ia_err_data:
		std::cout << "BAD_VALUE\n";
		break;
	case ia_err_internal:
		std::cout << "INVALID_OPERATION\n";
		break;
	case ia_err_argument:
		std::cout << "BAD_VALUE\n";
		break;
	}

	return 0;
}

int main() {
	ia_err err = ia_err_none;

	ia_binary_data *aiqbData = nullptr;
	ia_binary_data *nvmData = nullptr;
	ia_binary_data *aiqData = nullptr;

	ia_aiq * aiq = ia_aiq_init(aiqbData, nvmData, aiqData,
				   1920, 1080, 3, NULL, NULL);

	/**
	 * ==== AUTO FOCUS ====
	 */
	ia_aiq_af_input_params af_params = {
		ia_aiq_frame_use_still, 0, 1500,
		ia_aiq_af_operation_mode_auto,
		ia_aiq_af_range_normal,
		ia_aiq_af_metering_mode_auto,
		ia_aiq_flash_mode_auto,
		NULL, NULL, false
	};

	err = ia_err_none;
	ia_aiq_af_results *afResults = nullptr;
	err = ia_aiq_af_run(aiq, &af_params, &afResults);
	show_err_if_any(err);

	std::cout << "=== AUTO FOCUS ===" << "\n" <<
	"AutoFocus status: " <<  afResults->status << "\n" <<
	"Focal distance: " <<  afResults->current_focus_distance << "\n" <<
	"next_lens_position: " <<  afResults->next_lens_position << "\n" <<
	"lens_driver_action: " <<  afResults->lens_driver_action << "\n" <<
	"use_af_assist: " <<  afResults->use_af_assist << "\n" <<
	"Final lens pos: " <<  afResults->final_lens_position_reached << "\n\n";

	/**
	 * ==== AUTO FOCUS BRACKETING ====
	 */
	ia_aiq_af_bracket_input_params af_bracket_params = {
		2,
		*afResults,
		ia_aiq_af_bracket_mode_symmetric
	};

	ia_aiq_af_bracket_results *af_bracket_results = nullptr;
	err = ia_aiq_af_bracket (aiq, &af_bracket_params, &af_bracket_results);
	show_err_if_any(err);

	std::cout << "=== AUTO FOCUS BRACKET ===" << "\n" <<
	"distances_bracketing: " << *af_bracket_results->distances_bracketing << "\n" <<
	"lens_positions_bracketing: " << *af_bracket_results->lens_positions_bracketing << "\n\n";

	/**
	 * ==== AUTO EXPOSURE ====
	 * \todo: Look into AE bracketing structs returned as part of ae_results
	 */
	ia_aiq_ae_input_params ae_params = {
		2, /* Multiple exposure for exposure bracketing. */
		ia_aiq_frame_use_still, ia_aiq_flash_mode_auto,
		ia_aiq_ae_operation_mode_automatic,
		ia_aiq_ae_metering_mode_evaluative,
		ia_aiq_ae_priority_mode_normal,
		ia_aiq_ae_flicker_reduction_auto,
		NULL, /* AE descriptor sensor? param, mandatory */
		NULL, NULL, 0, NULL, NULL, NULL, NULL, NULL, -1,
		ia_aiq_aperture_control_dc_iris_auto,
		ia_aiq_ae_exposure_distribution_auto, -1
	};

	ia_aiq_ae_results *aeResults = nullptr;
	err = ia_aiq_ae_run(aiq, &ae_params, &aeResults);
	show_err_if_any(err);

	std::cout << "=== AUTO EXPOSURE ===" << "\n" <<
	"num_exposures: " << aeResults->num_exposures << "\n" <<
	"lux level estimate: " << aeResults->lux_level_estimate << "\n" <<
	"F-number: " << aeResults->aperture_control->aperture_fn << "\n\n";

	/**
	 * ==== GBCE ====
	 */
	ia_aiq_gbce_input_params gbce_params = {
		ia_aiq_gbce_level_use_tuning,
		ia_aiq_tone_map_level_use_tuning,
		ia_aiq_frame_use_still,
		0
	};

	ia_aiq_gbce_results *gbce_result = nullptr;
	err = ia_aiq_gbce_run (aiq, &gbce_params, &gbce_result);
	show_err_if_any(err);

	std::cout << "=== GBCE ===" << "\n"
	"r_gamma_lut: " << *gbce_result->r_gamma_lut << "\n" <<
	"g_gamma_lut: " << *gbce_result->g_gamma_lut << "\n" <<
	"b_gamma_lut: " << *gbce_result->b_gamma_lut << "\n" <<
	"gamma_lut_size: " << gbce_result->gamma_lut_size << "\n\n";

	/**
	 * ==== DSD (Determine Scene) ====
	 */
	ia_aiq_dsd_input_params dsd_params {
		afResults,
		ia_aiq_scene_mode_landscape
	};

	ia_aiq_scene_mode detected_scene_mode;
	err = ia_aiq_dsd_run (aiq, &dsd_params, &detected_scene_mode);
	show_err_if_any(err);

	std::cout << "=== Determine Scene DSD ===" << "\n"
	"Detected scene value: " << detected_scene_mode << "\n\n";

	/**
	 * ==== AUTO WHITE BALANCE (AWB)====
	 */
	ia_aiq_awb_input_params awb_params = {
		ia_aiq_frame_use_still, ia_aiq_awb_operation_mode_auto,
		nullptr, nullptr, 0.0
	};


	ia_aiq_awb_results awb_result_alloc;
	ia_aiq_awb_results *awb_result = &awb_result_alloc;
	err = ia_aiq_awb_run(aiq, &awb_params, &awb_result);
	show_err_if_any(err);

	/* Investigate why awb_result is still nullptr, causing segfault */
	std::cout << "=== AUTO WHITE BALANCE ===" << "\n";

	if (awb_result) {
		std::cout <<
		"Final R/G: " << awb_result->final_r_per_g << "\n" <<
		"Final B/G: " << awb_result->final_b_per_g << "\n" <<
		"Color temp est. : " << awb_result->distance_from_convergence << "\n\n";
	} else {
		std::cout << "Error: awb_result == NULL" << std::endl;
	}

	/**
	 * ==== Parameter adaptor ====
	 * Depends on AWB and Auto Exposure results.
	 * Compute generic parameters (Color Correction Matrix and Black Level
	 * Correction)which should be used to correct the next frame.
	 */

	/**
	 * ==== Shading adaptor ====
	 * Depends on AWB results.
	 */

	std::cout << "Main function ran" << std::endl;

	return 0;
}
