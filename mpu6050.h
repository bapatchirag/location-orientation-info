/* MPU6050 interfacing functionality
 * Includes gyroscope, acceleratometer and temperature sensor retrieval
 */

#include <string.h>
#include "i2c.h"
#define MPU_WRITE_ADDR 0xD0
#define MPU_READ_ADDR 0xD1

double gyro_val[3], acc_val[3], temp_val;
char final_data[14];

void mpu_i2c(int, int);
void init_mpu(void);
void get_data_mpu(char[]);
void get_gyro(char[]);
void get_acc(char[]);
void get_temp(char[]);

/* MPU6050 handler function for initializing MPU
 * @param1: <int> Register location to access
 * @param2: <int> Register value to put in @param1
 * @return: <void> None
 */
void mpu_i2c(int reg, int reg_val) {
	start_i2c();
	write_i2c(MPU_WRITE_ADDR);
	write_i2c(reg);
	write_i2c(reg_val);
	stop_i2c();
}

/* Initialises MPU6050 component
 * @param: None
 * @return: <void> None
 */
void init_mpu() {
	init_i2c();
	
	mpu_i2c(0x6B, 0x02); 		// PWR_MGMT_1 PLL with Y-axis gyroscope reference
	mpu_i2c(0x19, 0x07); 		// SMPLRT_DIV Reg Select, SMPLRT = 1K 	
	mpu_i2c(0x1A, 0x00); 		// CONFIG Reg Select 
	mpu_i2c(0x1B, 0x00); 		// CONFIG Reg Select, Full scale range +/- 250 degree/C 
	mpu_i2c(0x1C, 0x00); 		// ACCEL Reg Select, Full scale range +/- 2g 	
	mpu_i2c(0x23, 0x00); 		// FIFO disabled 	
	mpu_i2c(0x24, 0x00); 		// I2C Master Control, I2C Freq 348KHz  	
	mpu_i2c(0x37, 0x00); 		// Interrupt pin configuration 	
	mpu_i2c(0x38, 0x01); 		// Interrupt Enable, Data Ready Enable 	
	mpu_i2c(0x67, 0x00); 		// Master delay reg 	
	mpu_i2c(0x68, 0x00); 		// Signal path reset 	
	mpu_i2c(0x6A, 0x00); 		// User Control 	
	mpu_i2c(0x6C, 0x00); 		// PWR_MGMT_2 	
	mpu_i2c(0x74, 0x00); 		// FIFO R/W 
}

/* Gets all data from the MPU6050
 * @param: <char[]> String to be written in
 * @return: <void> None
 */
void get_data_mpu(char data[]) {
	start_i2c();
	write_i2c(MPU_WRITE_ADDR);
	write_i2c(0x3B);
	start_i2c();
	write_i2c(MPU_READ_ADDR);
	read_multichar_i2c(data, 14);
	stop_i2c();
}

/* Gets gyroscope data from the data string
 * @param: <char[]> String to be written in
 * @return: <void> None
 */
void get_gyro(char data[]) {
	gyro_val[0] = (double)(((int)(data[8] << 8)) | ((int)data[9])) / 131.0;
	gyro_val[1] = (double)(((int)(data[10] << 8)) | ((int)data[11])) / 131.0;
	gyro_val[2] = (double)(((int)(data[12] << 8)) | ((int)data[13])) / 131.0;
}

/* Gets accelerometer data from the data string
 * @param: <char[]> String to be written in
 * @return: <void> None
 */
void get_acc(char data[]) {
	acc_val[0] = (double)(((int)(data[0] << 8)) | ((int)data[1])) / 16384.0;
	acc_val[1] = (double)(((int)(data[2] << 8)) | ((int)data[3])) / 16384.0;
	acc_val[2] = (double)(((int)(data[4] << 8)) | ((int)data[5])) / 16384.0;
}

/* Gets temperature from the data string
 * @param: <char[]> String to be written in
 * @return: <void> None
 */
void get_temp(char data[]) {
	temp_val = ((double)(((int)(data[6] << 8)) | ((int)data[7])) / 340.0) + 36.53;
}
