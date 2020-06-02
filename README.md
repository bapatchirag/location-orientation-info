# ORIENTATION AND POSITION NOTIFIER

Implementation of a object position and orientation SMS notifier using the ARM7 LPC2148 microcontroller

## MODULES INTERFACED
* GPS Module
* SIM900A GSM Module
* MPU6050 sensor

### OTHER PERIPHERALS
* Toggle switch

## WORKING
* Upon switching toggle switch on, current latitude, longitude and altitude are fetched from the GPS module
* Similarly, gyroscope, altitude (X, Y and Z coordinates for both) and current temperature is also read via the I2C channel through the MPU6050
* This data is strung together in a message string, and is sent to the SIM900A module via the UART0 interface
* GSM module then sends this message to the preset mobile number as a SMS
* Above process repeats every 30 seconds, as long as the toggle switch is turned on
