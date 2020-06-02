/* GPS Module interfacing functionality
 * Gets latitude, longitude and altitude from received GGA string
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char gga_string[150], lat_buff[15], long_buff[15], alt_buff[8];
char iir_value[10];
char gga[3];
int gga_comma_ptrs[20];
int is_gga_string = 0;
volatile int gga_index, comma_ctr;

void reset_vals(void);
void init_gps(void);
__irq void interrupt_uart1(void);
void get_latitude(int);
void get_longitude(int);
void get_altitude(int);


/* Resets all GPS parameters received
 * @param: None
 * @return: <void> None
 */
void reset_vals() {
	memset(gga_string, 0, 150);
	memset(lat_buff, 0, 15);
	memset(long_buff, 0, 15);
	memset(alt_buff, 0, 8);	
}

/* Initialises UART1 to be connected to a GPS Module
 * @param: None
 * @return: <void> None
 */
void init_gps() {
	reset_vals();
	
	VICVectAddr0 = (unsigned)interrupt_uart1;						// Select priority of interrupt
	VICVectCntl0 = 0x27;									// Interrupt #7 - UART1, D5: Enable interrupt
	VICIntEnable = 0x80;									// Select interrupt #7 - UART1
	VICIntSelect = 0x00;									// Interrupt type - IRQ
	
	PINSEL0 |= 0x50000;									// Select TDX0 and RDX0
	U1LCR = 0x83;										// DLAB = 1, 1 stop bit, 8 bit character
	U1DLL = 0x61;										// Calculated from formula - baud rate 9600
	U1DLM = 0x00;										// Calculated from formula - baud rate 9600
	U1LCR = 0x03;										// DLAB = 0, 1 stop bit, 8 but character
	U1IER = 0x01;										// Enable RDA (Read Data Available) interrupts
}

/* Checks if string is GGA string upon interrupt
 * @param: None
 * @return: <void> None
 */
__irq void interrupt_uart1() {
	int iir_value = U1IIR;
	char received_char = U1RBR;
	
	if(received_char == '$') {															// Reset comma counter, GGA string index on '$'
		gga_index = 0;
		comma_ctr = 0;
		is_gga_string = 0;
	}
	else if(is_gga_string) {															// If string is GGA, record comma positions
		if(received_char == ',')
			gga_comma_ptrs[comma_ctr++] = gga_index;
		
		gga_string[gga_index++] = received_char;
	}
	else if(gga[0] == 'G' && gga[1] == 'G' && gga[2] == 'A') {				// Check if string is a GGA string
		is_gga_string = 1;
		gga[0] = gga[1] = gga[2] = 0;													// Reset "GGA" to "\0\0\0"
	}
	else {																				// Shift characters until GGA received
		gga[0] = gga[1];															
		gga[1] = gga[2];
		gga[2] = received_char;
	}
	
	
	VICVectAddr = 0x00;
}

/* Gets latitude parameter from NMEA GGA string
 * @param: <int> Current latitude pointer
 * @return: <void> None
 */
void get_latitude(int lat_ptr) {
	U1IER = 0x00;																		// Disable RDA interrupts
	
	int gga_index = (lat_ptr + 1);
	int lat_index;
	for(lat_index = 0; gga_string[gga_index] != ','; lat_index++, gga_index++) {		// Parse latitude in GGA string
		lat_buff[lat_index] = gga_string[gga_index];
	}
	
	double temp_lat, lat_degrees;
	temp_lat = atof(lat_buff);
	temp_lat /= 100;
	
	lat_degrees = (int)(temp_lat);
	temp_lat = (temp_lat - lat_degrees) / 0.6;
	lat_degrees = (double)(lat_degrees + temp_lat);						// Latitude in ddmm.mmmmmm format
	
	sprintf(lat_buff, "%lf", lat_degrees);
	
	U1IER = 0x01;																		// Enable RDA interrupts
}

/* Gets longitude parameter from NMEA GGA string
 * @param: <int> Current longitude pointer
 * @return: <void> None
 */
void get_longitude(int long_ptr) {
	U1IER = 0x00;																		// Disable RDA interrupts
	
	int gga_index = (long_ptr + 1);
	int long_index;
	for(long_index = 0; gga_string[gga_index] != ','; long_index++, gga_index++) {		// Parse longitude in GGA string
		long_buff[long_index] = gga_string[gga_index];
	}
	
	double temp_long, long_degrees;
	temp_long = atof(long_buff);
	temp_long /= 100;
	
	long_degrees = (int)(temp_long);
	temp_long = (temp_long - long_degrees) / 0.6;
	long_degrees = (double)(long_degrees + temp_long);					// longitude in ddmm.mmmmmm format
	
	sprintf(long_buff, "%lf", long_degrees);
	
	U1IER = 0x01;																		// Enable RDA interrupts
}

/* Gets altitude parameter from NMEA GGA string
 * @param: <int> Current altitude pointer
 * @return: <void> None
 */
void get_altitude(int alt_ptr) {
	U1IER = 0x00;																		// Disable RDA interrupts
	
	int gga_index = (alt_ptr + 1);
	int alt_index;
	for(alt_index = 0; gga_string[gga_index] != ','; alt_index++, gga_index++) {		// Parse altitude in GGA string
		alt_buff[alt_index] = gga_string[gga_index];
	}
	
	U1IER = 0x01;																		// Enable RDA interrupts
}
