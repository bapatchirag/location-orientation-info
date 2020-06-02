/* SIM900 GSM module related functionality
 * Includes UART initialisation and SMS sending functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char message_buff[160];
volatile int message_buff_pointer = 0;

void init_gsm(void);
void send_string(char*);
void start_gsm(void);
void send_message_gsm(char*, char*);

/* Initialises UART0 to be connected to the SIM900 GSM Module
 * @param: None
 * @return: <void> None
 */
void init_gsm() {
	PINSEL0 |= 0x05;							// Select TDX0 and RDX0
	U0LCR = 0x83;								// DLAB = 1, 1 stop bit, 8 bit character
	int reg_val = (15000000 / (16 * 9600));					// DLM:DLL in decimal, baud rate 9600
	U0DLL = reg_val & 0xFF;							// Lower byte 
	U0DLM = (reg_val >> 8) & 0xFF;						// Upper byte
	U0LCR = 0x03;								// DLAB = 0, 1 stop bit, 8 but character
	U0IER = 0x01;								// Enable RDA (Read Data Available) interrupts
}

/* Sends a string via UART module
 * @param: <char*> String to be sent
 * @return: <void> None
 */
void send_string(char* message) {
	U0IER = 0x00;												// Disabled RDA interrupt
	
	int str_index = 0;
	char message_char;											// Gets every character of message string
	
	while((message_char = message[str_index++]) != '\0') {							// Sends character one by one
		U0THR = message_char;
		while(!(U0LSR & (1 << 5)));									// Waits for all bits of character to be sent
	}
	
	U0IER = 0x01;												// Enables RDA interrupt
}

/* Activates the SIM900 GSM Module and checks if working by turning echo off
 * @param: None
 * @return: <void> None
 */
void start_gsm() {
	while(1) {
		send_string("ATE0\r");										// Turn off echo
		if(strstr(message_buff, "OK")) {								// If OK received...
			memset(message_buff, 0, 160);
			break;
		}
	}
	
	send_string("AT+CMGF=1\r");										// Select message format as text
}

/* Sends SMS via the SIM900 GSM module to the appropriate number
 * @param1: <char*> Mobile number to send SMS to
 * @param2: <char*> SMS text to be sent
 * @return: <void> None
 */
void send_message_gsm(char* number, char* message) {
	char sms_buff[35];
	message_buff_pointer = 0;
	sprintf(sms_buff, "AT+CMGS=\"%s\"\r", number);
	send_string(sms_buff);											// Send "send message" command
	
	while(1) {
		if(message_buff[message_buff_pointer] == 0x3E) {						// Wait for '>' character
			message_buff_pointer = 0;
			memset(message_buff, 0, strlen(message_buff));
			send_string(message);									// Send message to given number
			send_string((char*)0x1A);								// Send ^Z
			break;
		}
		message_buff_pointer++;
	}
	message_buff_pointer = 0;
	
	memset(message_buff, 0, strlen(message_buff));
	memset(sms_buff, 0, strlen(sms_buff));
}
