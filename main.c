#include <lpc214x.h>
#include "gsm.h"
#include "gps.h"
#include "mpu6050.h"
#include "timing.h"
#define IS_TOGGLE_ON (IO0PIN & (1 << 16))
#define MOBILE ("9876543210")
#define eos(s) ((s) + strlen(s))

int main() {
	// Initialise basic components
	init_gsm();
	init_gps();
	init_mpu();
	
	char all_details[150];
	memset(all_details, 0, 150);
	memset(final_data, 0, 14);
	
	// Main execution
	while(1) {
		while(IS_TOGGLE_ON) {
			get_latitude(gga_comma_ptrs[0]);
			sprintf(eos(all_details), "La%s", lat_buff);
			
			get_longitude(gga_comma_ptrs[2]);
			sprintf(eos(all_details), "Lo%s", long_buff);
			
			get_altitude(gga_comma_ptrs[7]);
			sprintf(eos(all_details), "Al%s", alt_buff);
			
			// Get complete MPU data
			get_data_mpu(final_data);
			
			// Get individual MPU data
			get_gyro(final_data);
			sprintf(eos(all_details), "Xg%lfYg%lfZg%lf", gyro_val[0], gyro_val[1], gyro_val[2]);
			
			get_acc(final_data);
			sprintf(eos(all_details), "Xa%lfYa%lfZa%lf", acc_val[0], acc_val[1], acc_val[2]);
			
			get_temp(final_data);
			sprintf(eos(all_details), "T%lf", temp_val);
			
			start_gsm();
			send_message_gsm(MOBILE, all_details);
			
			delay(30);
		}			
	}		
	
	return 0;
}
