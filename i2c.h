/* I2C functionality
 * Initialising, starting, stopping I2c block, along with reading and writing from and to the slave from the master covered
 */

void init_i2c(void);
void start_i2c(void);
void stop_i2c(void);
void write_i2c(char);
char read_ack_i2c(void);
char read_nack_i2c(void);
void read_multichar_i2c(char*, int);

/* Initialises I2C block
 * @param: None
 * @return: <void> None
 */
void init_i2c() {
	PINSEL0 |= 0x50;					// Select options 1 for P0.2 and P0.3
	I2C0CONSET = 0x40;					// Bit 6: Enable I2C
	I2C0SCLL = 0x32;					// Data rate = 300 kHz, duty cycle = 50%
	I2C0SCLH = 0x32;					// Data rate = 300 kHz, duty cycle = 50%
}

/* Starts I2C transmission
 * @param: None
 * @return: <void> None
 */
void start_i2c() {
	I2C0CONSET = 0x20;					// Bit 5: Start I2C
	while(!(I2C0CONSET & 0x08));				// Wait till start condition transmission is complete
	I2C0CONCLR = 0x28;					// Clear start and SI bits
}

/* Stops I2C block
 * @param: None
 * @return: <void> None
 */
void stop_i2c() {
	I2C0CONSET = 0x50;					// Bit 4: Stop I2C, Bit 6: Enable I2C (for further transmission, if required)
}

/* Writes to slave from master
 * @param: <char> Single byte
 * @return: <void> None
 */
void write_i2c(char data) {
	I2C0DAT = data;						
	I2C0CONSET = 0x40;					// Bit 6: Enable I2C
 	while(!(I2C0CONSET & 0x08));				// Wait till start condition transmission is complete
	I2C0CONCLR = 0x08;					// Clear SI bit
}

/* Reads from slave with an acknowledgement
 * @param: None
 * @return: <char> Single byte
 */
char read_ack_i2c() {
	I2C0CONSET = 0x44;					// Bit 6: Enable I2C, Bit 2: Enable ACK bit
	while(!(I2C0CONSET & 0x08));				// Wait till start condition transmission is complete
	I2C0CONCLR = 0x0C;					// Clear ACK and SI bits
	return I2C0DAT;
}

/* Reads from slave without an acknowledgement
 * @param: None
 * @return: <char> Single byte
 */
char read_nack_i2c() {
	I2C0CONSET = 0x40;					// Bit 6: Enable I2C
	while(!(I2C0CONSET & 0x08));				// Wait till start condition transmission is complete
	I2C0CONCLR = 0x08;					// Clear SI bit
	return I2C0DAT;
}

/* Reads multiple characters from slave
 * @param1: <char*> Message read from slave
 * @param2: <int> Size of message read
 * @return: <void> None
 */
void read_multichar_i2c(char* message, int size) {
	int message_index = 0;
	
	while(size-- != 1)
		*(message + message_index++) = read_ack_i2c();
	
	*(message + message_index) = read_nack_i2c();
}
