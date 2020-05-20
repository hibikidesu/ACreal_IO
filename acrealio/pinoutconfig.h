#ifndef CONFIG_H
#define CONFIG_H

#define RFID_BAUD 115200		//Baud rate for RFID Module

#define RFID_MODULE1 1			//Rfid module used for reader1, 1:SL015M 2:RR10

#define SDVX_VOL_SENS 7        //Sensitivity for SDVX Volume buttons


//input pins for sdvx controls
#define BT_FXR A0
#define BT_FXL A1
#define BT_A A2
#define BT_B A3
#define BT_C A4
#define BT_D A5
#define BT_START 50
#define BT_TEST 52
#define BT_SVC A8

//input pins for volume encoders (phase A and phase B for each)
#define VOL_R A6
#define VOL_L A7

//pins for sdvx lights (outputs)
#define LT_START 53
#define LT_A 51
#define LT_B 49
#define LT_C 47
#define LT_D 45
#define LT_FXL 43
#define LT_FXR 41

//pins for sdvx/jubeat RGB LEDs (use PWM pins)
#define LED1_R 7
#define LED1_G 6
#define LED1_B 5

#endif

