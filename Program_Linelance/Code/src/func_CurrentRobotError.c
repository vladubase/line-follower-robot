/****
	*	@name		Linelance_linefollower
	*	@file 		func_CurrentRobotError.c
	*
	*	@author 	Uladzislau 'vladubase' Dubatouka
	*				<vladubase@gmail.com>
	*	@version	V1.0
	*	@date 		01-February-2021
	*	@link		https://github.com/vladubase/Linelance
	*
*****/


/************************************** Includes **************************************/

#include "func_CurrentRobotError.h"


/************************************** Function **************************************/

float CurrentRobotError (void) {
   /*
	*	@brief	This function calculate robot error relatively sensor line.
	*/
	
	// DEFINITION OF VARIABLES
		extern bool line_data[];
		uint8_t i = 0;
		float current_error = 0;
		int8_t power_of_sensor[8] = {-80, -35, -10, -2, 2, 10, 35, 80};
	
	// FUNCTION
		for (i = 0; i < 8; i++) {
			/*	If the data on the i-th sensor is zero,				*
			 *	then the sensor is located above the black line.	*/
			if (line_data[i] != 0) {
				current_error += power_of_sensor[i];
			}
			
//			if (line_data[i] != 0) {
//				/*	If the data on the i-th sensor is zero,
//				 *	then the sensor is located above the black line.
//				 *	Odd degree to preserve the sign '-'. */
//				current_error += pow (8 / 2 - i - 0.5, 3); 	// (8 sensors / 2 - 0.5 - i)^3.
//			}
		}
	
	return current_error;
}

//float pow (float base, uint8_t power) {
//	uint32_t result = 1;
//	
//	while (power > 0) {
//		result *= base;		
//		power--;
//	}
//	
//	return result;
//}
