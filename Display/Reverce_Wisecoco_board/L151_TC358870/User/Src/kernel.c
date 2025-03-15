/*
 *
 *  Created on: Mar 16, 2022
 *      Author: Heorenmaru
 */

#include "kernel.h"

#define DEVICE_CODE 0x0000U
#define DEVICE_VER 	0x00U

/////////////////////////////////////////////////////////////////////////////////
// HANDLERS and VARS
/////////////////////////////////////////////////////////////////////////////////

extern I2C_HandleTypeDef hi2c1;
extern TIM_HandleTypeDef htim4;
//uint16_t adc_data[9 * 8] = { 0 };

#define T_TIMEOUT  1
#define T_OK 0

uint8_t devaddr = 0x0F;

uint8_t DisplayStarted = 0;
uint8_t LostSignal = 0;


//see VESA-EEDID-A2.pdf
uint8_t edid_data[]={
						0x00,   		//	0b0
						0xFF,   		//	0b11111111
						0xFF,   		//	0b11111111
						0xFF,   		//	0b11111111
						0xFF,   		//	0b11111111
						0xFF,   		//	0b11111111
						0xFF,   		//	0b11111111
						0x00,   		//	0b0
					//		ID Manufacturer Name
						0x22,			//	0b00110011
						0xD2,			//	0b01010100			0b0 01100 11010 10100  - 12 26 20 - "LZT"
					//
					//	A B C D E F G H I J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z
					//	1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26
					//
					//
					//		ID Product Code
						0x01,			//	0b1
						0xAA,			//	0b0						- 1
					//		ID Serial Number                                           	
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0						- 0
					//		Week of Manufacture or Model Year Flag
						0x0C,			//	0b1100					- 12 week
						0x1B,			//	0b11011					- 17 year
					//		EDID Structure Version and Revision Numbers                
						0x01,			//	0b1						- !
						0x03,			//	0b11					- !!! (needed 0x04)
					//		Video Input Definition                                     
						0x80,			//	0b10000000				- Color Bit Depth is undefined ; Digital Interface is not defined
					//		Horizontal and Vertical Screen Size or Aspect Ratio        
						0x00,			//	0b0
						0x00,			//	0b0						- unknown or undefined
					//		Display Transfer Characteristics (GAMMA)                   
						0x78,			//	0b1111000				- 120 (2.2)
					//		Feature Support                                            
						0x02,			//	0b00001010				- multi-mode freq; Preferred Timing Mode ; sRGB default color space; RGB color display
					//		Chromaticity and Default White Point                       
						0x07,			//	0b111					- Rx1=0 Rx0=0 Ry1=0 Ry0=0 Gx1=0 Gx0=1 Gy1=1 Gy0=1
						0xF5,			//	0b11110101				- Bx1=1 Bx0=1 By1=1 By0=1 Wx1=0 Wx0=1 Wy1=0 Wy0=1
						0x9A,			//	0b10011010				- Red_x
						0x56,			//	0b1010110				- Red_y
						0x4E,			//	0b1001110				- Green_x
						0x86,			//							- Green_y
						0x26,			//							- Blue_x
						0x1E,			//							- Blue_y
						0x50,			//							- White_x
						0x54,			//							- White_y
					//		Established Timings I & II                                 
						0x00,			//	0b0						- 800 x 600 @ 60Hz
						0x00,			//	0b0						- 1280 x 1024 @ 75Hz
						0x00,			//	0b0						- Reserved for Manufacturer Specified Timings
					//		Standard Timings                                           
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						0x00,			//	0b0
						//[ 18 Byte Data Blocks ]
					//		Preferred Timing Mode
						0xC5,			//	////////////////
						0x64,			//	////////////////
						0x40,			//	////////////////
						0x49,			//	////////////////
						0xB0,			//	////////////////
						0xA0,			//	////////////////
						0x10,			//	////////////////
						0x50,			//	////////////////
						0x32,			//	////////////////
						0x08,			//	////////////////
						0x82,			//	////////////////
						0x00,			//	
						0x5A,			//	 
						0xA0,			//	 
						0x00,			//	
						0x00,			//	
						0x00,			//	
						0x18,			//	 
						//	Detailed Timing # 2 or Display Descriptor
						0x00,			//	//
						0x00,			//	//
						0x00,			//	//
						0xFC,			//	////Display Product Name (ASCII)
						0x00,			//	
						0x48,			//	////////////////
						0x54,			//	////////////////
						0x43,			//	////////////////
						0x2d,			//	////////////////
						0x56,			//	////////////////
						0x49,			//	////////////////
						0x56,			//	////////////////
						0x45,			//	////////////////
						0x0a,			//	////////////////
						0x20,			//	////////////////
						0x20,			//	////////////////
						0x20,			//	////////////////
						0x20,			//	////////////////
						// Detailed Timing # 3 or Display Descriptor
						0x00,			//	//
						0x00,			//	//
						0x00,			//	//
						0xFF,			//	//
						0x00,			//	//Display Product Serial Number Descriptor Definition
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x30,			//	 
						0x20,			//	 
						0x20,			//	 
						0x20,			//	 
						0x20,			//	 
						0x20,			//	 
						// Detailed Timing # 4 or Display Descriptor
						0x00,			// //
						0x00,			// //
						0x00,			// //
						0xFD,			// //Display Range Limits & Timing Descriptor Block Definition
						0x00,			// // V/H offset zero
						0x17,			// // Minimum Vertical Rate 23 Hz
						0x4B,			// // Maximum Vertical Rate 75 Hz
						0x0F,			// // Minimum Horizontal Rate 15 kHz
						0xF0,			// // Maximum Horizontal Rate 240 kHz
						0x22,			// // Maximum Pixel Clock 340 MHz
						0x00,			// 
						0x0A,			// 
						0x20,			// 
						0x20,			// 
						0x20,			// 
						0x20,			// 
						0x20,			// 
						0x20,			// 
						//[ Extension Block Count N ]
						0x01,
						//[ checksumm ]
						0xCC,
					
					
						0x02, 
						0x03, 
						0x17, 
						0x74, 
						0x47, 
						0x00, 
						0x00, 
						0x00, 
						0x00,
						0x00, 
						0x00, 
						0x00, 
						0x23, 
						0x09, 
						0x7F, 
						0x07, 
						0x66, 
						0x03, 
						0x0C, 
						0x00, 
						0x30, 
						0x00, 
						0x80, 
						0xC5, 
						0x64, 
						0x40, 
						0x49, 
						0xB0, 
						0xA0, 
						0x10, 
						0x50, 
						0x32, 
						0x08, 
						0x82, 
						0x00, 
						0x5A, 
						0xA0, 
						0x00, 
						0x00, 
						0x00, 
						0x18, 
						0xC5, 
						0x64, 
						0x40, 
						0x49, 
						0xB0, 
						0xA0, 
						0x10, 
						0x50, 
						0x32, 
						0x08, 
						0x82, 
						0x00, 
						0x5A, 
						0xA0, 
						0x00, 
						0x00, 
						0x00, 
						0x18, 
						0xC5, 
						0x64, 
						0x40, 
						0x49, 
						0xB0, 
						0xA0, 
						0x10, 
						0x50, 
						0x32, 
						0x08, 
						0x82, 
						0x00, 
						0x5A, 
						0xA0, 
						0x00, 
						0x00, 
						0x00, 
						0x18, 
						0xC5, 
						0x64, 
						0x40, 
						0x49, 
						0xB0, 
						0xA0, 
						0x10, 
						0x50, 
						0x32, 
						0x08, 
						0x82, 
						0x00, 
						0x5A, 
						0xA0, 
						0x00, 
						0x00, 
						0x00, 
						0x18, 
						0xC5, 
						0x64, 
						0x40, 
						0x49, 
						0xB0, 
						0xA0, 
						0x10, 
						0x50, 
						0x32, 
						0x08, 
						0x82, 
						0x00, 
						0x5A, 
						0xA0, 
						0x00, 
						0x00, 
						0x00, 
						0x18, 
						0x00, 
						0x00,                         
						0x00,             
						0x00,             
						0x00,             
						0x00,             
						0x00,    
						0x00,    
						0x00,    
						0x00,    
						0x00,
						0x00,
						0x00,
						0x00,
						0x62
};					
						/// END EEDID


/////////////////////////////////////////////////////////////////////////////////
// USB Receive logic
/////////////////////////////////////////////////////////////////////////////////



void usb_callback(uint8_t *arr, uint16_t len){



	////////////
	// DEV INFO
	// (STANDART COMMAND)
	if(arr[0] == 0 ){

		uint16_t *idBase0 = (uint16_t*)(UID_BASE);
		uint16_t *idBase1 = (uint16_t*)(UID_BASE + 0x02);
		uint32_t *idBase2 = (uint32_t*)(UID_BASE + 0x04);
		uint32_t *idBase3 = (uint32_t*)(UID_BASE + 0x08);

		usb_rst_cursor();
		usb_add_uint8(0x00);
		usb_add_uint16(idBase0);
		usb_add_uint16(idBase1);
		usb_add_uint32(idBase2);
		usb_add_uint32(idBase3);

		usb_add_uint16((uint8_t)DEVICE_CODE);
		usb_add_uint8((uint8_t)DEVICE_VER);

		usb_send_buff();

	}


	////////////////RFM SETTINGS
	if(arr[0] == 1 ){



//
//		HAL_GPIO_WritePin(LED13_GPIO_Port, LED13_Pin, 0);
//		HAL_ADC_Start_DMA(&hadc, (uint32_t*)&adc_data, 9*8);
//
		usb_rst_cursor();
		usb_add_uint8(0x01);
		usb_add_uint8(0);

		usb_send_buff();
	}


}


/////////////////////////////////////////////////////////////////////////////////
// Func
/////////////////////////////////////////////////////////////////////////////////


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{

    if(hadc->Instance == ADC1)
    {
        HAL_ADC_Stop_DMA(hadc); // это необязательно
        calc_voltage();

    }
}


void err_handle(){
	while(1){
		HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 1);
		HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 1);
		HAL_Delay(50);
		HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 0);
		HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 0);
		HAL_Delay(300);
		HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 1);
		HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 1);
		HAL_Delay(50);
		HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 0);
		HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 0);
		HAL_Delay(1000);

	}
}


void i2c_write(uint8_t addr, uint16_t reg, uint32_t dat, uint8_t len){

	//HAL_I2C_Mem_Write(&hi2c1,  (addr<<1) | 0b1, reg, 2, dat, 2, 100);
	if(HAL_I2C_Mem_Write(&hi2c1,  (addr<<1) | 0b0, reg, 2, &dat, len, 100) != HAL_OK){
		err_handle();
	}
}


void eedid_write(){
	uint16_t i = 0;
	uint8_t tmp[256] = {0};
	memcpy( &tmp, &edid_data, 0x100);
	tmp[127] = 0;
	tmp[255] = 0;

	while ( i < 256 ){
		if(i < 127)
			tmp[127] += tmp[i];
		if(i == 127)
			tmp[127] = 0 - tmp[127];

		if(i > 0x127)
			tmp[255] += tmp[i];
		if(i == 255)
			tmp[255] = 0 - tmp[255];


		i2c_write(devaddr, 0x8C00 + i, tmp[i], 1);

		i++;
	}


}
/////////////////////////////////////////////////////////////////////////////////
// INIT
/////////////////////////////////////////////////////////////////////////////////
void tcs_init() {

//	HAL_GPIO_WritePin(A_IO1v8_GPIO_Port, A_IO1v8_Pin, 1);
	HAL_GPIO_WritePin(B_IO1v8_GPIO_Port, B_IO1v8_Pin, 1);
//	HAL_GPIO_WritePin(C_IO3v3_GPIO_Port, C_IO3v3_Pin, 1);
//	HAL_GPIO_WritePin(D_IO3v3_GPIO_Port, D_IO3v3_Pin, 1);
	HAL_GPIO_WritePin(VDD3v3d_GPIO_Port, VDD3v3d_Pin, 1);
	HAL_GPIO_WritePin(VDD1v2_GPIO_Port, VDD1v2_Pin, 0);
	HAL_GPIO_WritePin(VDD1v8d_GPIO_Port, VDD1v8d_Pin, 0);
	HAL_GPIO_WritePin(VDD1v1_GPIO_Port, VDD1v1_Pin, 1);
	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 0);
	HAL_GPIO_WritePin(INT_GPIO_Port, INT_Pin, 0);

	HAL_Delay(2000);


	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 1);

	HAL_Delay(200);
	HAL_GPIO_WritePin(INT_GPIO_Port, INT_Pin, 1);




	//i2c:  [address + rw] [regaddr_h][regaddr_l] [data_l]..[data_h]

	//datasheet page 62

	i2c_write(devaddr, 0x0004, 0x0004, 2); //W 0x0004 0x0400		0b100					- AutoIndex = 1
	i2c_write(devaddr, 0x0002, 0x3F01, 2); //W 0x0002 0x013F		0b11111100000001		- SLEEP + HdmiRst + CTxRst + CecRst + IRRst + SPLRst + SLMBRst
	HAL_Delay(10);//1ms
	i2c_write(devaddr, 0x0002, 0x0000, 2); //W 0x0002 0x0000		0b0						- Software needs to clear
	i2c_write(devaddr, 0x0006, 0x0008, 2); //W 0x0006 0x0800		0b1000					- Refclk = 1 ; RGB888
	HAL_Delay(1);//0.25ms
	i2c_write(devaddr, 0x8410, 0x03, 1); //W 0x8410 0x03			0b00000011				- PHYCtl = 0 ; ??
	i2c_write(devaddr, 0x8413, 0x3F, 1); //W 0x8413 0x3F			0b00111111				- PHYEnb = 1
	i2c_write(devaddr, 0x84F0, 0x31, 1); //W 0x84F0 0x31			0b00110001				- APPL_ON = 1 ; APLL_CPCTL = 11
	i2c_write(devaddr, 0x84F4, 0x01, 1); //W 0x84F4 0x01			0b1						- DDCPWR
	i2c_write(devaddr, 0x8540, 0x12C0, 2); //W 0x8540 0xC012		0b1001011000000	(4800)	- SYS_FREQ0 = 48MHz
	i2c_write(devaddr, 0x8630, 0x00, 1); //W 0x8630 0x00			0b0
	i2c_write(devaddr, 0x8631, 0x0753, 2); //W 0x8631 0x5307		0b11101010011 (480000)	- LOCK_REF_FREQ = 48MHz
	i2c_write(devaddr, 0x8670, 0x02, 1); //W 0x8670 0x02			0b10					- NCO_F0_MOD = 10
	i2c_write(devaddr, 0x8A0C, 0x12C0, 2); //W 0x8A0C 0xC012		0b1001011000000 (4800)	- SYS_FREQ = 48MHz
	i2c_write(devaddr, 0x8502, 0xFF, 1); //W 0x8502 0xFF			0b11111111				- ALL system interrupt
	i2c_write(devaddr, 0x8512, 0xFE, 1); //W 0x8512 0xFE			0b11111110				- DDC mask OFF
	i2c_write(devaddr, 0x8514, 0x00, 1); //W 0x8514 0x00			0b0						- PACKET INTERRUPT MASK OFF
	i2c_write(devaddr, 0x8515, 0x00, 1); //W 0x8515 0x00			0b0						- CBIT INTERRUPT MASK OFF
	i2c_write(devaddr, 0x8516, 0x00, 1); //W 0x8516 0x00			0b0						- AUDIO INTERRUPT MASK OFF
	i2c_write(devaddr, 0x0014, 0x0FBF, 2); //W 0x0014 0xBF0F		0b111110111111			- clear Interrupt
	i2c_write(devaddr, 0x0016, 0x0FFF, 2); //W 0x0016 0xFF0F		0b111111111111			- Interrupt Mask  - all disabled?
	i2c_write(devaddr, 0x85E0, 0x01, 1); //W 0x85E0 0x01			0b1						- EDID_MODE = Internal EDID-RAM & DDC2B mode
	i2c_write(devaddr, 0x85E3, 0x0100, 2); //W 0x85E3 0x0001		0b1						- EDID data size stored in RAM = 1
//
//
	eedid_write();
//	0x8C00 - EDID-RAM ADDRESS
//	see VESA-EEDID-A2.pdf
//		 EDID Header

	/// END EEDID

//
//	i2c_write(devaddr, 0x85D1, 0x01, 1);//	W 0x85D1 0x01 					-	What o_O??
//	i2c_write(devaddr, 0x8560, 0x24, 1);//	W 0x8560 0x24 					-	HDCP MODE		(Default) Automatic authentication mode 0
//	i2c_write(devaddr, 0x8563, 0x11, 1);//	W 0x8563 0x11 					-	What o_O??
	i2c_write(devaddr, 0x8564, 0x0F, 1);//	W 0x8564 0x0F 					-	What o_O??
	//// audio
	i2c_write(devaddr, 0x8A02, 0x42, 1);//	W 0x8A02 0x42 					-	OUT_SYNC0 		(Default) Mode2
	i2c_write(devaddr, 0x8543, 0x02, 1);//	W 0x8543 0x02 					-	DDC_CTL			(Default)
	i2c_write(devaddr, 0x8544, 0x11, 1);//	W 0x8544 0x11 					-	HPD_CTL			HOTPLUG = “H”   DDC5V detection interlock
	i2c_write(devaddr, 0x8600, 0x00, 1);//	W 0x8600 0x00 					-	FORCE_MUTE		AMUTEOUT = Mute OFF   data MUTE control = Mute OFF
	i2c_write(devaddr, 0x8602, 0xF3, 1);//	W 0x8602 0xF3 					-	Mute_Auto		(Default)
	i2c_write(devaddr, 0x8603, 0x02, 1);//	W 0x8603 0x02 					-	UTO_CMD1		(Default)
	i2c_write(devaddr, 0x8604, 0x0C, 1);//	W 0x8604 0x0C 					-	UTO_CMD2		(Default)
	i2c_write(devaddr, 0x8606, 0x05, 1);//	W 0x8606 0x05 					-	BUFINIT_START	(Default)
	i2c_write(devaddr, 0x8607, 0x00, 1);//	W 0x8607 0x00 					-	FS_MUTE			Do not Mute (ALL)
	i2c_write(devaddr, 0x8652, 0x02, 1);//	W 0x8652 0x02 					-	SDO_MODE1		16bit  LRCK = 768 kHz standard I2S
	i2c_write(devaddr, 0x8671, 0x020C49BA, 4);//	W 0x8671 0xBA490C02		-	AUDIO PLL Setting Register
	i2c_write(devaddr, 0x8675, 0x01E1B089, 4);//	W 0x8675 0x89B0E101		-	AUDIO PLL Setting Register

	i2c_write(devaddr, 0x854A, 0x01, 1);//	W 0x854A 0x01					-	INIT_END


}


uint8_t tcs_HDMIStatus(){ // check connect
	uint8_t result = 0;
	// W 0x8520 R 0x00
	uint16_t addr = 0x8520;
	uint8_t data = 0;
	if(HAL_I2C_Mem_Read(&hi2c1, devaddr<<1, addr, 2, &data, 1, 100)!= HAL_OK){
		err_handle();
	}

	// 8f -> 1
	return data;
}

void tcs_initDisplay(){



	HAL_GPIO_WritePin(VDD3v3d_GPIO_Port, VDD3v3d_Pin, 0);

	HAL_Delay(200);
	HAL_GPIO_WritePin(VDD1v8d_GPIO_Port, VDD1v8d_Pin, 1);
	HAL_GPIO_WritePin(VDD1v2_GPIO_Port, VDD1v2_Pin, 1);

	HAL_GPIO_WritePin(VN5_GPIO_Port, VN5_Pin, 1);

	HAL_Delay(200);
//	HAL_GPIO_WritePin(A_IO1v8_GPIO_Port, A_IO1v8_Pin, 0);
	HAL_GPIO_WritePin(B_IO1v8_GPIO_Port, B_IO1v8_Pin, 0);
//	HAL_GPIO_WritePin(C_IO3v3_GPIO_Port, C_IO3v3_Pin, 1);
//	HAL_GPIO_WritePin(D_IO3v3_GPIO_Port, D_IO3v3_Pin, 1);
	HAL_Delay(200);

	i2c_write(devaddr, 0x0004, 0x0004, 2);		//AutoIndex
	i2c_write(devaddr, 0x0002, 0x1200, 2);		//DSI-TX Software Reset    Audio Block Software Reset
	HAL_Delay(10);
	i2c_write(devaddr, 0x0002, 0x0000, 2);		//disable rst bit
	i2c_write(devaddr, 0x0006, 0x0008, 2);		//Refclk = 1
	i2c_write(devaddr, 0x0108, 0x00000001, 4);	//DSITXEn = 1
	i2c_write(devaddr, 0x010C, 0x00000001, 4);	//PPIHsTxClkEn = 1
	i2c_write(devaddr, 0x02A0, 0x00000001, 4);	//
	i2c_write(devaddr, 0x02AC, 0x000090AC, 4);	//-//MIPI_PLL_CONF
	HAL_Delay(20);
	////////// DSI 0
	i2c_write(devaddr, 0x02A0, 0x00000003, 4);	//
	i2c_write(devaddr, 0x0118, 0x00000014, 4);	// Data Lane 0, 1, 2 and 3 are enabled.   CLaneEn = 1
	i2c_write(devaddr, 0x0120, 0x00001665, 4);	//-// LINEINITCNT = 0x178D
	i2c_write(devaddr, 0x0124, 0x00000000, 4);	// HSToCnt = 0
	i2c_write(devaddr, 0x0128, 0x00000101, 4);	// VHDelayEn = 1   IndModeSel = 1
	i2c_write(devaddr, 0x0130, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0134, 0x00005000, 4);	//
	i2c_write(devaddr, 0x0138, 0x00010000, 4);	//
	i2c_write(devaddr, 0x013C, 0x00010000, 4);	// DSI_PRESP_LPW_COUNT
	i2c_write(devaddr, 0x0140, 0x00010000, 4);	// DSI_PRESP_HSR_COUNT
	i2c_write(devaddr, 0x0144, 0x00010000, 4);	// DSI_PRESP_HSW_COUNT
	i2c_write(devaddr, 0x0148, 0x00001000, 4);	//
	i2c_write(devaddr, 0x014C, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0150, 0x00000161, 4);	// EoTpEn = 1 HsCkMd = 1 CrcDis = 1 ECCDis = 1
	i2c_write(devaddr, 0x0154, 0x00000001, 4);	// RXVC0_EN = 1
	i2c_write(devaddr, 0x0158, 0x000000C8, 4);	// IND_TO_COUNT
	i2c_write(devaddr, 0x0168, 0x0000002A, 4);	//
	i2c_write(devaddr, 0x0170, 0x0000049F, 4);	// APF_VDELAYCNT
	i2c_write(devaddr, 0x017C, 0x00000081, 4);	// DSITXMd = 1   BlankPkt_En = 1
	i2c_write(devaddr, 0x018C, 0x0000000D, 4);	// DSI_HSYNC_WIDTH    5?
	i2c_write(devaddr, 0x0190, 0x00000026, 4);	// DSI_HBPR 	90?
	i2c_write(devaddr, 0x01A4, 0x00000000, 4);	// DSI_RX_STATE_INT_MASK  = NoMask
	i2c_write(devaddr, 0x01C0, 0x00000015, 4);	// DSI_LPRX_THRESH_COUNT
	i2c_write(devaddr, 0x0214, 0x00000000, 4);	// APP_SIDE_ERR_INT_MASK  = NoMask
	i2c_write(devaddr, 0x021C, 0x00000080, 4);	// DSI_RX_ERR_INT_MASK  = Reserved (1)
	i2c_write(devaddr, 0x0224, 0x00000000, 4);	// DSI_LPTX_INT_MASK    = NoMask
	i2c_write(devaddr, 0x0254, 0x00000005, 4);	// PPI_DPHY_LPTXTIMECNT
	i2c_write(devaddr, 0x0258, 0x00250305, 4);	// PPI_DPHY_TCLK_HEADERCNT
	i2c_write(devaddr, 0x025C, 0x000C000A, 4);	// PPI_DPHY_TCLK_TRAILCNT
	i2c_write(devaddr, 0x0260, 0x00110008, 4);	// PPI_DPHY_THS_HEADERCNT
	i2c_write(devaddr, 0x0264, 0x00004AA9, 4);	// PPI_DPHY_TWAKEUPCNT
	i2c_write(devaddr, 0x0268, 0x0000000E, 4);	// PPI_DPHY_TCLK_POSTCNT
	i2c_write(devaddr, 0x026C, 0x000C0009, 4);	// PPI_DPHY_THSTRAILCNT
	i2c_write(devaddr, 0x0270, 0x00000020, 4);	// PPI_DPHY_HSTXVREGCNT (TX Voltage Regulator setup Wait Counter)
	i2c_write(devaddr, 0x0274, 0x0000001F, 4);	// PPI_DPHY_HSTXVREGEN
	i2c_write(devaddr, 0x0278, 0x00050006, 4);	// PPI_DSI_BTA_COUNT
	i2c_write(devaddr, 0x027C, 0x00000002, 4);	// PPI_DPHYTX_ADJUST  D0S_LPTXCURREN = 25% (default)
	i2c_write(devaddr, 0x0288, 0x000002AA, 4);	// PPI_DPHY_CAP (default)
	i2c_write(devaddr, 0x011C, 0x00000001, 4);	// DSITX_START
	////////// DSI 1
	i2c_write(devaddr, 0x0308, 0x00000001, 4);	//
	i2c_write(devaddr, 0x030C, 0x00000001, 4);	//
	i2c_write(devaddr, 0x04A0, 0x00000001, 4);	//
	i2c_write(devaddr, 0x04AC, 0x000090AC, 4);	//
	HAL_Delay(20);
	i2c_write(devaddr, 0x04A0, 0x00000003, 4);	//
	i2c_write(devaddr, 0x0318, 0x00000014, 4);	//
	i2c_write(devaddr, 0x0320, 0x00001665, 4);	//
	i2c_write(devaddr, 0x0324, 0x00000000, 4);	//
	i2c_write(devaddr, 0x0328, 0x00000101, 4);	//
	i2c_write(devaddr, 0x0330, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0334, 0x00005000, 4);	//
	i2c_write(devaddr, 0x0338, 0x00010000, 4);	//
	i2c_write(devaddr, 0x033C, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0340, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0344, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0348, 0x00001000, 4);	//
	i2c_write(devaddr, 0x034C, 0x00010000, 4);	//
	i2c_write(devaddr, 0x0350, 0x00000161, 4);	//
	i2c_write(devaddr, 0x0354, 0x00000001, 4);	//
	i2c_write(devaddr, 0x0358, 0x000000C8, 4);	//
	i2c_write(devaddr, 0x0368, 0x0000002A, 4);	//
	i2c_write(devaddr, 0x0370, 0x0000049F, 4);	//
	i2c_write(devaddr, 0x037C, 0x00000081, 4);	//
	i2c_write(devaddr, 0x038C, 0x0000000D, 4);	//
	i2c_write(devaddr, 0x0390, 0x00000026, 4);	//
	i2c_write(devaddr, 0x03A4, 0x00000000, 4);	//
	i2c_write(devaddr, 0x03C0, 0x00000015, 4);	//
	i2c_write(devaddr, 0x0414, 0x00000000, 4);	//
	i2c_write(devaddr, 0x041C, 0x00000080, 4);	//
	i2c_write(devaddr, 0x0424, 0x00000000, 4);	//
	i2c_write(devaddr, 0x0454, 0x00000005, 4);	//
	i2c_write(devaddr, 0x0458, 0x00250305, 4);	//
	i2c_write(devaddr, 0x045C, 0x000C000A, 4);	//
	i2c_write(devaddr, 0x0460, 0x00110008, 4);	//
	i2c_write(devaddr, 0x0464, 0x00004AA9, 4);	//
	i2c_write(devaddr, 0x0468, 0x0000000E, 4);	//
	i2c_write(devaddr, 0x046C, 0x000C0009, 4);	//
	i2c_write(devaddr, 0x0470, 0x00000020, 4);	//
	i2c_write(devaddr, 0x0474, 0x0000001F, 4);	//
	i2c_write(devaddr, 0x0478, 0x00050006, 4);	//
	i2c_write(devaddr, 0x047C, 0x00000002, 4);	//
	i2c_write(devaddr, 0x0488, 0x000002AA, 4);	//
	i2c_write(devaddr, 0x031C, 0x00000001, 4);	//

	i2c_write(devaddr, 0x0110, 0x00000016, 4);	// IndMode  HSYNC_POL_SW  VSYNC_POL_SW
	i2c_write(devaddr, 0x0310, 0x00000016, 4);	// IndMode  HSYNC_POL_SW  VSYNC_POL_SW
	HAL_Delay(10);

	///// display init
	i2c_write(devaddr, 0x0500, 0x0004, 2);	//CMD_SEL  (PAGE 30) Select both DSITX0 and DSITX1 I/F
	i2c_write(devaddr, 0x0504, 0x0015, 2);	//
	i2c_write(devaddr, 0x0504, 0xC036, 2);	//
	HAL_Delay(20);
	i2c_write(devaddr, 0x0504, 0x0005, 2);	//
	i2c_write(devaddr, 0x0504, 0x0029, 2);	// 0x11?
	HAL_Delay(110);
	i2c_write(devaddr, 0x0504, 0x0005, 2);	//
	i2c_write(devaddr, 0x0504, 0x0011, 2);	// 0x29?
	HAL_Delay(40);


//	HAL_Delay(400);

	i2c_write(devaddr, 0x5000, 0x0000, 2);	//
	i2c_write(devaddr, 0x5008, 0x10E0, 2);	//
	i2c_write(devaddr, 0x500C, 0x8000, 2);	//
	i2c_write(devaddr, 0x5080, 0x0000, 2);	//
	i2c_write(devaddr, 0x5088, 0x10E0, 2);	//

	i2c_write(devaddr, 0x0004, 0x0C37, 2);	//0b110000110111
	i2c_write(devaddr, 0x0006, 0x0000, 2);	//

	i2c_write(devaddr, 0x0110, 0x00000006, 4);	//
	i2c_write(devaddr, 0x0310, 0x00000006, 4);	//

	TIM4->CCR1 = 50;

	HAL_Delay(11000);

	DisplayStarted = 1;


}

void tcs_DisplayOff(){

	HAL_GPIO_WritePin(VDD3_3_GPIO_Port, VDD3_3_Pin, 0);  //+

	HAL_GPIO_WritePin(VDD3v3d_GPIO_Port, VDD3v3d_Pin, 0);  //+

	HAL_GPIO_WritePin(VDD1v1_GPIO_Port, VDD1v1_Pin, 1);  //+

	HAL_GPIO_WritePin(VDD1v2_GPIO_Port, VDD1v2_Pin, 0);  //+

//	HAL_GPIO_WritePin(PWR_OLED_GPIO_Port, PWR_OLED_Pin, 1);  //+

//	HAL_GPIO_WritePin(OLED_EN_PWR_GPIO_Port, OLED_EN_PWR_Pin, 0); //+

//	HAL_GPIO_WritePin(VDD1v8d_GPIO_Port, VDD1v8d_Pin, 0);  //+

//	HAL_GPIO_WritePin(BL_PWR_GPIO_Port, BL_PWR_Pin, 0);   //+

//	HAL_GPIO_WritePin(A_IO1v8_GPIO_Port, A_IO1v8_Pin, 1);   //+
	HAL_GPIO_WritePin(B_IO1v8_GPIO_Port, B_IO1v8_Pin, 1);   //+
//	HAL_GPIO_WritePin(C_IO3v3_GPIO_Port, C_IO3v3_Pin, 1);   //+
//	HAL_GPIO_WritePin(D_IO3v3_GPIO_Port, D_IO3v3_Pin, 1);   //+

	i2c_write(devaddr, 0x0504, 0x0005, 2);
	i2c_write(devaddr, 0x0504, 0x0017, 2);
	HAL_Delay(100);
	i2c_write(devaddr, 0x0504, 0x0005, 2);
	i2c_write(devaddr, 0x0504, 0x001C, 2);
	HAL_Delay(50);
	i2c_write(devaddr, 0x0504, 0x0005, 2);
	i2c_write(devaddr, 0x0504, 0x000A, 2);
	HAL_Delay(200);

	HAL_GPIO_WritePin(VDD3v3d_GPIO_Port, VDD3v3d_Pin, 1);
	HAL_GPIO_WritePin(VDD1v8d_GPIO_Port, VDD1v8d_Pin, 0);

	TIM4->CCR1 = 0;
//////////////////

	DisplayStarted = 0;
}

void kernel_init() {
	HAL_TIM_Base_Start(&htim4);
	TIM4->CCR1 = 0;
    //////////////////////////////////
    // Hardware Check
//	hi2c1->ErrorCode = HAL_I2C_ERROR_NONE;
//	hi2c1->State = HAL_I2C_STATE_READY;
//	hi2c1->PreviousState = I2C_STATE_NONE;
//	hi2c1->Mode = HAL_I2C_MODE_NONE;


//	HAL_GPIO_WritePin(A_IO1v8_GPIO_Port, A_IO1v8_Pin, 1);
	HAL_GPIO_WritePin(B_IO1v8_GPIO_Port, B_IO1v8_Pin, 1);
//	HAL_GPIO_WritePin(C_IO3v3_GPIO_Port, C_IO3v3_Pin, 1);
//	HAL_GPIO_WritePin(D_IO3v3_GPIO_Port, D_IO3v3_Pin, 1);
	HAL_GPIO_WritePin(VDD3v3d_GPIO_Port, VDD3v3d_Pin, 1);
	HAL_GPIO_WritePin(VDD1v2_GPIO_Port, VDD1v2_Pin, 0);
	HAL_GPIO_WritePin(VDD1v8d_GPIO_Port, VDD1v8d_Pin, 0);
	HAL_GPIO_WritePin(VDD1v1_GPIO_Port, VDD1v1_Pin, 1);

	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 0);
	HAL_GPIO_WritePin(INT_GPIO_Port, INT_Pin, 0);

	HAL_Delay(2000);


	HAL_GPIO_WritePin(RST_GPIO_Port, RST_Pin, 1);

	HAL_Delay(200);
	HAL_GPIO_WritePin(INT_GPIO_Port, INT_Pin, 1);
	uint16_t addr = 0x0004;
	uint16_t data = 0;
	for(devaddr = 0; devaddr<127; devaddr++){
		if(HAL_I2C_Mem_Read(&hi2c1, (devaddr<<1) | 0b1, addr, 2, &data, 1, 100) == HAL_OK){
			HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 1);
			break;
		}
	}
	if(devaddr==127) {
		err_handle();
	}



	tcs_init();
//	HAL_I2C_Mem_Read(&hi2c1, devaddr<<1, addr, 2, &data, 2, 100);

//	HAL_I2C_Mem_Read(&hi2c1, devaddr<<1, addr, 2, &data, 1, 1000);









}


/////////////////////////////////////////////////////////////////////////////////
// MAIN
/////////////////////////////////////////////////////////////////////////////////

void kernel_main() {
	TIM4->CCR1 = 0;
	HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
	HAL_GPIO_WritePin(L13_GPIO_Port, L13_Pin, 1);
	kernel_init();
	HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 0);
	HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 0);
    while(1)
    {
    	if (DisplayStarted == 0){
    		if((tcs_HDMIStatus() == 0x8f)|(tcs_HDMIStatus() == 0x9f)){
    			HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 1);
//    			tcs_init();
//    			HAL_Delay(1000);
    			tcs_initDisplay();
    		}
//    		HAL_Delay(1);
    	}

    	if((tcs_HDMIStatus() & 0b10000000) >0){
    	    HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 1);
    	}else{
    		HAL_GPIO_WritePin(L1_GPIO_Port, L1_Pin, 0);
    	}

    	if(tcs_HDMIStatus() == 0x9f){
    		HAL_GPIO_WritePin(L13_GPIO_Port, L13_Pin, 0);
    	}else{
    		HAL_GPIO_WritePin(L13_GPIO_Port, L13_Pin, 1);
    	}


    	if (DisplayStarted == 1){
    		// 8f		0b10001111 	[S_SYNC][--][------][------][S_PHY_SCDT][S_PHY_PLL][S_TMDS][S_DDC5V]
    		// 9f		0b10011111 	[S_SYNC][--][------][S_HDMI][S_PHY_SCDT][S_PHY_PLL][S_TMDS][S_DDC5V]
    		// 1f		0b00011111	[------][--][------][S_HDMI][S_PHY_SCDT][S_PHY_PLL][S_TMDS][S_DDC5V]
    		// 0f		0b00001111	[------][--][------][------][S_PHY_SCDT][S_PHY_PLL][S_TMDS][S_DDC5V]
    		// 2f		0b00101111	[------][--][S_HDCP][------][S_PHY_SCDT][S_PHY_PLL][S_TMDS][S_DDC5V]
    		// 29		0b00101001	[------][--][S_HDCP][------][S_PHY_SCDT][---------][------][S_DDC5V]
    		// 28		0b00101000	[------][--][S_HDCP][------][S_PHY_SCDT][---------][------][-------]
    		if((tcs_HDMIStatus() & 0b00000110) == 0){
    			HAL_GPIO_WritePin(L2_GPIO_Port, L2_Pin, 0);
    			tcs_DisplayOff();
//    			HAL_Delay(600);

    	    }
//    	    HAL_Delay(1);
    	}



    }
}



