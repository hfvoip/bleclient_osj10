/* ----------------------------------------------------------------------------
 * Copyright (c) 2015-2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * Copyright (C) RivieraWaves 2009-2016
 *
 * This module is derived in part from example code provided by RivieraWaves
 * and as such the underlying code is the property of RivieraWaves [a member
 * of the CEVA, Inc. group of companies], together with additional code which
 * is the property of ON Semiconductor. The code (in whole or any part) may not
 * be redistributed in any form without prior written permission from
 * ON Semiconductor.
 *
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * app.c
 * - Main application file
 * ----------------------------------------------------------------------------
 * $Revision: 1.28 $
 * $Date: 2017/12/05 15:12:10 $
 * ------------------------------------------------------------------------- */

#include "app.h"




int main(void)
{
    uint32_t length;
    uint8_t temp[BUFFER_SIZE];

    /* Initialize the system */
    App_Initialize();
    printf_init();
    PRINTF("\r\n BLE CLIENT START ...");

    APP_BASS_SetBatMonAlarm(0);
    uint8_t  sendcmd[] ={0xAA,0x00,0x00,0x00};

    /* Main application loop:
     * - Run the kernel scheduler
     * - Refresh the watchdog and wait for an interrupt before continuing.
     * - Send battery request and write command for a custom service attribute
     */
    while (1)
    {
        Kernel_Schedule();

        if (unhandled_packets != NULL)
        {
            if (UART_FillTXBuffer(unhandled_packets->length,
                                  unhandled_packets->data) !=
                UART_ERRNO_OVERFLOW)
            {
                unhandled_packets = removeNode(unhandled_packets);
            }
        }

#if 0
        if (arr_ble_env[0].state == APPM_CONNECTED)
        {
#if 0
            if ((basc_support_env.enable == true) && (app_env.send_batt_req >=
                                                      25))
            {
                app_env.send_batt_req = 0;
                Batt_SendReadInfoReq(arr_ble_env[0].conidx, 0, BASC_BATT_LVL_VAL);
            }
#endif

            J10_SendReadCmd(0,1);
            Sys_Watchdog_Refresh();
            Sys_Delay_ProgramROM(5 * SystemCoreClock);

            if (cs_env[0].sentSuccess)
            {
            	 sendcmd[3] ++;
				if (sendcmd[3] >3)  sendcmd[3] = 0;
				J10_SendWriteCmd(0,1,sendcmd,4);
				 Sys_Watchdog_Refresh();
				Sys_Delay_ProgramROM(5 * SystemCoreClock);

                /* Copy data from the UART RX buffer to the TX buffer */
                length = UART_EmptyRXBuffer(temp);
                if (length > 0 && cs_env[0].state == CS_ALL_ATTS_DISCOVERED)
                {
                    /* Split buffer into two packets when it's greater than
                     * packet size */
                    if (length > PACKET_SIZE)
                    {
                        CustomSrvice_SendWrite(arr_ble_env[0].conidx, temp,
                                               cs_env[0].disc_att[CS_IDX_RX_CHAR].
                                               pointer_hdl,
                                               0, PACKET_SIZE,
                                               GATTC_WRITE_NO_RESPONSE);
                        CustomSrvice_SendWrite(arr_ble_env[0].conidx,
                                               &temp[PACKET_SIZE],
                                               cs_env[0].disc_att[CS_IDX_RX_CHAR].
                                               pointer_hdl,
                                               0, length - PACKET_SIZE,
                                               GATTC_WRITE_NO_RESPONSE);
                    }
                    else
                    {
                        CustomSrvice_SendWrite(arr_ble_env[0].conidx, temp,
                                               cs_env[0].disc_att[CS_IDX_RX_CHAR].
                                               pointer_hdl,
                                               0, length,
                                               GATTC_WRITE_NO_RESPONSE);
                    }
                }
            }

        }
#endif
        /* Refresh the watchdog timer */
        Sys_Watchdog_Refresh();
        ADC_BUTTON_Handler();

        /* Wait for an event before executing the scheduler again */
        SYS_WAIT_FOR_EVENT;
    }
}
