/* main.c to see what all we can call from ia_imaging.
 *  NULLs in input parameters are mostly optional parameters, skipped for now.
 */

/* Compile via:
 * g++ -L/home/uajain/Downloads/intel-3a-libs-bin-2019.08.26/usr/lib64 main.cpp -lia_aiq
 */
#include <iostream>

#include <ia_imaging/ia_aiq.h>

int main() {
	ia_binary_data aiqbData = {nullptr, 0};
	ia_binary_data nvmData = {nullptr, 0};
	ia_binary_data aiqData = {nullptr, 0};

	ia_aiq * aiq = ia_aiq_init(&aiqbData, &nvmData, &aiqData,
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

	ia_aiq_af_results *afResults = nullptr;
	ia_aiq_af_run(aiq, &af_params, &afResults);

	std::cout <<
	"AutoFocus status: " <<  afResults->status << "\n" <<
	"Focal distanace: " <<  afResults->current_focus_distance << "\n" <<
	"next_lens_position: " <<  afResults->next_lens_position << "\n" <<
	"lens_driver_action: " <<  afResults->lens_driver_action << "\n" <<
	"use_af_assist: " <<  afResults->use_af_assist << "\n" <<
	"Final lens pos: " <<  afResults->final_lens_position_reached << "\n" <<
	"=========================================================\n\n";

	/**
	 * ==== AUTO EXPOSURE ====
	 */
	ia_aiq_ae_input_params ae_params = {
		1, ia_aiq_frame_use_still, ia_aiq_flash_mode_auto,
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
	ia_aiq_ae_run(aiq, &ae_params, &aeResults);

	std::cout <<
	"num_exposures: " << aeResults->num_exposures << "\n" <<
	"lux level estimate: " << aeResults->lux_level_estimate << "\n" <<
	"=========================================================\n\n";

	/**
	 * ==== AUTO WHITE BALANCE ====
	 */
	ia_aiq_awb_input_params awb_params = {
		ia_aiq_frame_use_still, ia_aiq_awb_operation_mode_auto,
		NULL, NULL, -1.0
	};

	ia_aiq_awb_results *awb_result = nullptr;
	ia_err err = ia_aiq_awb_run(aiq, &awb_params, &awb_result); //segfault

	std::cout <<
	"Final R/G: " << awb_result->final_r_per_g << "\n" <<
	"Final B/G: " << awb_result->final_b_per_g << "\n" <<
	"Color temp est. : " << awb_result->distance_from_convergence << "\n" <<
	"=========================================================\n\n";

	std::cout << "Main function ran" << std::endl;

	return 0;
}
