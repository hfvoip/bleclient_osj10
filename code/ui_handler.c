#include <app.h>
#include <math.h>

#include <printf.h>

void Debug_LED(int led_dio,int cnt) {
	for (uint8_t i=0;i<cnt;i++) {

	    Sys_GPIO_Set_High(led_dio);

	    /* Delay for LED_ON_DURATION seconds
	     * The number of cycles is calculated as follows:
	     * LED_ON_DURATION [s] * SystemCoreClock [cycle/s] */
	    Sys_Delay_ProgramROM(0.5 * SystemCoreClock);
	    Sys_Watchdog_Refresh();

	    Sys_GPIO_Set_Low(led_dio);
	    Sys_Delay_ProgramROM(0.5 * SystemCoreClock);
	       Sys_Watchdog_Refresh();
	}
}




void Dec_Volume() {
	 uint8_t  sendcmd[] ={0xAA,0x01,0x00,0x00};

			uint8_t btn_flag_7 = DIO_DATA->ALIAS[LEFT_BLE_BUTTON];
			uint8_t btn_flag_11 = DIO_DATA->ALIAS[RIGHT_BLE_BUTTON];

			if  (btn_flag_7 ==0)  {
				uint8_t vol_idx = cs_env[0].tx_value[1];
				//0..21
				vol_idx +=3;
				if (vol_idx>21)  vol_idx = 21;
				sendcmd[3] = vol_idx;
				J10_SendWriteCmd(0,1,sendcmd,4);
				Sys_Watchdog_Refresh();

			}
			if  (btn_flag_11 ==0)  {
				uint8_t vol_idx = cs_env[1].tx_value[1];
				//0..21
				vol_idx +=3;
				J10_SendWriteCmd(1,1,sendcmd,4);
				Sys_Watchdog_Refresh();

			}
}


void Inc_Volume() {
	 uint8_t  sendcmd[] ={0xAA,0x01,0x00,0x00};

		uint8_t btn_flag_7 = DIO_DATA->ALIAS[LEFT_BLE_BUTTON];
		uint8_t btn_flag_11 = DIO_DATA->ALIAS[RIGHT_BLE_BUTTON];

		if  (btn_flag_7 ==0)  {
			uint8_t vol_idx = cs_env[0].tx_value[1];
			//0..21
			vol_idx -=3;
			if (vol_idx <0) vol_idx =0;

			sendcmd[3] = vol_idx;
			J10_SendWriteCmd(0,1,sendcmd,4);
			Sys_Watchdog_Refresh();

		}
		if  (btn_flag_11 ==0)  {
			uint8_t vol_idx = cs_env[1].tx_value[1];
			//0..21
			vol_idx -=3;
			J10_SendWriteCmd(1,1,sendcmd,4);
			Sys_Watchdog_Refresh();

		}





}
//mode: 0..3
#define TOTAL_MEM_NUMS  4
#define BYTESIZE_PERMEM  128
void Change_Mode(uint8_t mem_idx) {

	 uint8_t  sendcmd[] ={0xAA,0x00,0x00,0x00};

		if (mem_idx >= TOTAL_MEM_NUMS) mem_idx = 0;


		if ((mem_idx >=0 ) && (mem_idx < TOTAL_MEM_NUMS)) {

			sendcmd[3] = mem_idx;
			uint8_t btn_flag_7 = DIO_DATA->ALIAS[LEFT_BLE_BUTTON];
			uint8_t btn_flag_11 = DIO_DATA->ALIAS[RIGHT_BLE_BUTTON];

			if  (btn_flag_7 ==0)  {
				J10_SendWriteCmd(0,1,sendcmd,4);
				Sys_Watchdog_Refresh();

			}
			if  (btn_flag_11 ==0)  {
				J10_SendWriteCmd(1,1,sendcmd,4);
				Sys_Watchdog_Refresh();

			}

		}


}

void Normal_BUTTON_Handler() {
	//检查DIO5 (T3), DIO11(RIGHT),DIO7(LEFT)

	uint8_t btn_flag_7 = DIO_DATA->ALIAS[LEFT_BLE_BUTTON];
	uint8_t btn_flag_11 = DIO_DATA->ALIAS[RIGHT_BLE_BUTTON];


	PRINTF("\r\n   7:%d 11:%d", btn_flag_7,btn_flag_11);
	 Sys_Delay_ProgramROM(0.5 * SystemCoreClock);


}
void  ADC_BUTTON_Handler() {

	int ad_val  = 0;

   //up: 16383
	//v- 15205
	//v+  13298
	//d 10598
	//c : 7337
	//b: 3650
	//A:501
	//PRINTF("\r\n ad val:%d",ad_val);
	ad_val = app_batt_read.batt_lvl_sum_mV ;
	ad_val = round(ad_val*0.01);

	if (ad_val >=160) {
		 Sys_Delay_ProgramROM(0.2 * SystemCoreClock);
		 return ;
	}else
	if  (ad_val >140) {
		// V-
		Dec_Volume();

	} else
	if  (ad_val >130) {
		// V +
		Inc_Volume();
	} else
	if (ad_val >100) {
		// D
		Change_Mode(3);
	} else
	if (ad_val >70) {
			// MODE C
		Change_Mode(2);
	} else
	if (ad_val >30) {
			// MODE B
		Change_Mode(1);
	} else
	if (ad_val >3) {
			// MODE A
		Change_Mode(0);
	}

    //We need
	 //Process_buttonevt();
	//处理完，等待用户松手，不然再次执行到了这里
	 Sys_Delay_ProgramROM(0.2 * SystemCoreClock);


}
