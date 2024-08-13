/* ----------------------------------------------------------------------------
 * Copyright (c) 2015-2017 Semiconductor Components Industries, LLC (d/b/a
 * ON Semiconductor), All Rights Reserved
 *
 * This code is the property of ON Semiconductor and may not be redistributed
 * in any form without prior written permission from ON Semiconductor.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between ON Semiconductor and the licensee.
 *
 * This is Reusable Code.
 *
 * ----------------------------------------------------------------------------
 * ble_custom.c
 * - Bluetooth custom service functions
 * ----------------------------------------------------------------------------
 * $Revision: 1.16 $
 * $Date: 2018/02/27 15:03:59 $
 * ------------------------------------------------------------------------- */

#include "app.h"

/* Global variable definition */
struct cs_env_tag cs_env[APP_NB_PEERS];
overflow_packet_t *unhandled_packets = NULL;

/* ----------------------------------------------------------------------------
 * Function      : void CustomService_Env_Initialize(void)
 * ----------------------------------------------------------------------------
 * Description   : Initialize custom service environment
 * Inputs        : None
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void CustomService_Env_Initialize(void)
{
    /* Reset the application manager environment */
    memset(&cs_env, 0, sizeof(cs_env));

    cs_env[0].sentSuccess = 1;
    cs_env[1].sentSuccess = 1;

}

/* ----------------------------------------------------------------------------
 * Function      : void CustomService_ServiceEnable(uint8_t conidx)
 * ----------------------------------------------------------------------------
 * Description   : Send a command to use service discovery to look for a
 *                 specific service with a known UUID
 * Inputs        : - conidx       - connection index
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void CustomService_ServiceEnable(uint8_t conidx)
{
    struct gattc_disc_cmd *cmd;

    uint8_t svc_uuid[ATT_UUID_128_LEN] = CS_SVC_UUID;

    cmd = KE_MSG_ALLOC_DYN(GATTC_DISC_CMD, KE_BUILD_ID(TASK_GATTC, conidx),
                           TASK_APP, gattc_disc_cmd,
                           16 * sizeof(uint8_t));
    cmd->operation = GATTC_DISC_BY_UUID_SVC;
    cmd->uuid_len = ATT_UUID_128_LEN;
    cmd->seq_num = 0x00;
    cmd->start_hdl = 0x0001;
    cmd->end_hdl   = 0xffff;
    memcpy(cmd->uuid, svc_uuid, ATT_UUID_128_LEN);

    /* Send the message */
    ke_msg_send(cmd);
}

/* ----------------------------------------------------------------------------
 * Function      : int GATTC_DiscCharInd(ke_msg_id_t const msg_id,
 *                                       struct gattc_disc_char_ind
 *                                       const *param,
 *                                       ke_task_id_t const dest_id,
 *                                       ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Handle discovered characteristic indication message received
 *                 from GATT controller
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameters in format of
 *                                struct gattc_disc_char_ind
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int GATTC_DiscCharInd(ke_msg_id_t const msg_id,
                      struct gattc_disc_char_ind const *param,
                      ke_task_id_t const dest_id,
                      ke_task_id_t const src_id)
{
    uint8_t uuid[2][16] = CS_CHARACTERISTICS_LIST;
    uint8_t i;
    uint8_t conidx =  KE_IDX_GET(src_id);
	uint8_t ble_env_index  =  Find_Ble_Env(conidx);
	if (ble_env_index ==0xFF) return KE_MSG_CONSUMED;

    PRINTF("\r\n DiscCharInd: src_id=%0x,conidx=%0x ",src_id,conidx );
    /* Attr_hdl is for characteristic handle and pointer_hdl for value  */
    if (param->attr_hdl != 0 && cs_env[ble_env_index].disc_attnum < CS_IDX_NB)
    {
        for (i = 0; i < CS_IDX_NB; i++)
        {
            if (param->uuid_len == ATT_UUID_128_LEN &&
                !memcmp(param->uuid, &uuid[i][0], ATT_UUID_128_LEN))
            {
                memcpy(&cs_env[ble_env_index].disc_att[cs_env[ble_env_index].disc_attnum], param,
                       sizeof(struct discovered_char_att));
              PRINTF("\r\n attnum:%d,attr_hdl:%d, pointer_hdl:%d ",
            		  cs_env[ble_env_index].disc_attnum,
					  cs_env[ble_env_index].disc_att[cs_env[ble_env_index].disc_attnum].attr_hdl,
					  cs_env[ble_env_index].disc_att[cs_env[ble_env_index].disc_attnum].pointer_hdl);
                cs_env[ble_env_index].disc_attnum++;
                break;
            }
        }

        if (cs_env[ble_env_index].disc_attnum == CS_IDX_NB)
        {
            cs_env[ble_env_index].state = CS_ALL_ATTS_DISCOVERED;

            /* Enable pending client services to be enable */
            Service_Enable(conidx);
        }
    }

    return (KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : int GATTC_CmpEvt(ke_msg_id_t const msg_id,
 *                                  struct gattc_cmp_evt
 *                                  const *param,
 *                                  ke_task_id_t const dest_id,
 *                                  ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Handle received GATT controller complete event
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameters in format of
 *                                struct gattc_cmp_evt
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int GATTC_CmpEvt(ke_msg_id_t const msg_id, struct gattc_cmp_evt
                 const *param,
                 ke_task_id_t const dest_id,
                 ke_task_id_t const src_id)
{
	uint8_t conidx =  KE_IDX_GET(src_id);
	uint8_t ble_env_index  =  Find_Ble_Env(conidx);
	if (ble_env_index ==0xFF) return KE_MSG_CONSUMED;
    if (param->operation == GATTC_WRITE_NO_RESPONSE)
    {
        if (param->status == GAP_ERR_NO_ERROR)
        {
            cs_env[ble_env_index].sentSuccess = 1;
        }

        if (param->status == GAP_ERR_DISCONNECTED)
        {
            cs_env[ble_env_index].sentSuccess = 1;
        }
    }

    /* Check application state and status of service and characteristic
     * discovery for custom service and if it is unsuccessful we can disconnect
     * the link although it is possible to go to enable state and let the
     * battery service works */
    if (param->status != GAP_ERR_NO_ERROR)
    {
        if (param->operation == GATTC_DISC_BY_UUID_SVC &&
            param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND &&
            cs_env[ble_env_index].state != CS_SERVICE_DISCOVERD
            && arr_ble_env[ble_env_index].state != APPM_CONNECTED)
        {
            /* Enable pending client services to be enable */
            Service_Enable(conidx);
        }
        else if (param->operation == GATTC_DISC_ALL_CHAR &&
                 param->status == ATT_ERR_ATTRIBUTE_NOT_FOUND &&
                 cs_env[ble_env_index].state == CS_SERVICE_DISCOVERD)
        {
            /* Enable pending client services to be enable */
            Service_Enable(conidx);
        }
    }

    return (KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : int GATTC_DiscSvcInd(ke_msg_id_t const msg_id,
 *                                      struct gattc_disc_svc_ind
 *                                      const *param,
 *                                      ke_task_id_t const dest_id,
 *                                      ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Receive the result of a service discovery
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameters in format of
 *                                struct gattc_disc_svc_ind
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int GATTC_DiscSvcInd(ke_msg_id_t const msg_id,
                     struct gattc_disc_svc_ind const *param,
                     ke_task_id_t const dest_id,
                     ke_task_id_t const src_id)
{
    struct gattc_disc_cmd *cmd;

    uint8_t conidx =  KE_IDX_GET(src_id);
	uint8_t ble_env_index  =  Find_Ble_Env(conidx);
	if (ble_env_index ==0xFF) return KE_MSG_CONSUMED;
	PRINTF("\r\n DiscSvcInd: src_id=%0x,conidx=%0x,ble_env_index=%d",src_id,conidx,ble_env_index);

    /* We accepts only discovered attributes with 128-bit UUID according to the
     * defined
     * characteristics in this custom service */
    if (param->uuid_len == ATT_UUID_128_LEN)
    {
        cs_env[ble_env_index].state       = CS_SERVICE_DISCOVERD;

        cs_env[ble_env_index].start_hdl   = param->start_hdl;
        cs_env[ble_env_index].end_hdl     = param->end_hdl;

        cs_env[ble_env_index].disc_attnum = 0;

        /* Allocate and send GATTC discovery command to discover
         * characteristic declarations */
        cmd = KE_MSG_ALLOC_DYN(GATTC_DISC_CMD,
                               KE_BUILD_ID(TASK_GATTC,conidx),
                               TASK_APP, gattc_disc_cmd,
                               2 * sizeof(uint8_t));

        cmd->operation = GATTC_DISC_ALL_CHAR;
        cmd->uuid_len = 2;
        cmd->seq_num = 0x00;
        cmd->start_hdl = cs_env[ble_env_index].start_hdl;
        cmd->end_hdl   = cs_env[ble_env_index].end_hdl;
        cmd->uuid[0]   = 0;
        cmd->uuid[1]   = 0;

        /* Send the message */
        ke_msg_send(cmd);
    }

    return (KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : int GATTC_ReadInd(ke_msg_id_t const msg_id,
 *                                   struct gattc_read_ind
 *                                   const *param, ke_task_id_t const dest_id,
 *                                   ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Receive transmitted value from peripheral, assign to
 *                 tx_value
 *
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameters in format of
 *                                struct gattc_read_ind
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
int GATTC_ReadInd(ke_msg_id_t const msg_id, struct
                  gattc_read_ind *param,
                  ke_task_id_t const dest_id,
                  ke_task_id_t const src_id)
{

	  uint8_t conidx =  KE_IDX_GET(src_id);

	  PRINTF("\r\n Read Ind: src_id=%d,dest_id=%d,conidx=%d,hdl=%d,length=%d\r\n",src_id,dest_id,conidx,param->handle,param->length);
	  	for (int i=0;i<param->length;i++)
	  		PRINTF(" %0x",param->value[i]);
	  	PRINTF("  \r\n");

		uint8_t ble_env_index  =  Find_Ble_Env(conidx);
		if (ble_env_index ==0xFF) return KE_MSG_CONSUMED;

    if (cs_env[ble_env_index].disc_att[CS_IDX_TX_CHAR].pointer_hdl == param->handle)
    {
        memcpy(cs_env[ble_env_index].tx_value, param->value, param->length);
    }

    return (KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : int GATTC_EvtInd(ke_msg_id_t const msg_id,
 *                                  struct gattc_read_ind
 *                                  const *param, ke_task_id_t const dest_id,
 *                                  ke_task_id_t const src_id)
 * ----------------------------------------------------------------------------
 * Description   : Receive transmitted value from peripheral, assign to
 *                 tx_value - contains new value of peer attribute handle
 * Inputs        : - msg_id     - Kernel message ID number
 *                 - param      - Message parameters in format of
 *                                struct gattc_read_ind
 *                 - dest_id    - Destination task ID number
 *                 - src_id     - Source task ID number
 * Outputs       : return value - Indicate if the message was consumed;
 *                                compare with KE_MSG_CONSUMED
 * Assumptions   : None
 * ------------------------------------------------------------------------ */
int GATTC_EvtInd(ke_msg_id_t const msg_id, struct
                 gattc_event_ind *param,
                 ke_task_id_t const dest_id,
                 ke_task_id_t const src_id)
{
    overflow_packet_t *traverse = NULL;
    unsigned int flag;

    uint8_t conidx =  KE_IDX_GET(src_id);

    PRINTF("\r\n Notified:dst=%d,src_id=%d,conidx:%d,handle:=%d,length=%d\r\n",dest_id,src_id,conidx,param->handle, param->length);
    for (int i=0;i<param->length;i++)
    		PRINTF(" %0x",param->value[i]);
    PRINTF("  \r\n");

  		uint8_t ble_env_index  =  Find_Ble_Env(conidx);
  		if (ble_env_index ==0xFF) return KE_MSG_CONSUMED;

    if (param->length > 0 || unhandled_packets != NULL)
    {
        if (cs_env[ble_env_index].disc_att[CS_IDX_TX_CHAR].pointer_hdl == param->handle)
        {
            memcpy(cs_env[ble_env_index].tx_value, param->value, param->length);
            flag = 0;

            /* Start by trying to queue up any previously unhandled packets. If
             * we
             * can't queue them all, set a flag to indicate that we need to
             * queue
             * the new packet too. */
            while ((unhandled_packets != NULL) && (flag == 0))
            {
                if (UART_FillTXBuffer(unhandled_packets->length,
                                      unhandled_packets->data) !=
                    UART_ERRNO_OVERFLOW)
                {
                    /* Remove a successfully queued packet from the list of
                     * unqueued
                     * packets. */
                    unhandled_packets = removeNode(unhandled_packets);
                }
                else
                {
                    flag = 1;
                }
            }

            /* If we don't have any (more) outstanding packets, attempt to queue
             * the
             * current packet. If this packet is successfully queued, exit. */
            if (flag == 0)
            {
                if (UART_FillTXBuffer(param->length, cs_env[ble_env_index].tx_value) !=
                    UART_ERRNO_OVERFLOW)
                {
                    return (KE_MSG_CONSUMED);
                }
            }
            /* We couldn't empty the list or we couldn't queue the new packet.
             * In
             * both cases we need to save the current packet at the end of the
             * list,
             * then exit. */
            if (unhandled_packets != NULL)
            {
                traverse = unhandled_packets;
                while (traverse->next != NULL)
                {
                    traverse = traverse->next;
                }
                traverse->next = createNode(param->length, cs_env[ble_env_index].tx_value);
            }
            else
            {
                unhandled_packets = createNode(param->length, cs_env[ble_env_index].tx_value);
            }
        }
    }

    return (KE_MSG_CONSUMED);
}

/* ----------------------------------------------------------------------------
 * Function      : void CustomSrvice_SendWrite(uint8_t conidx, uint8_t *value,
 *                                             uint16_t handle, uint8_t offset,
 *                                             uint16_t length, uint8_t type)
 * ----------------------------------------------------------------------------
 * Description   : Send a write command or request to the client device
 * Inputs        : - conidx       - Connection index
 *                 - value        - Pointer to value
 *                 - hanlde       - Attribute handle
 *                 - length       - Length of value
 *                 - type         - Type of write message
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void CustomSrvice_SendWrite(uint8_t conidx, uint8_t *value, uint16_t handle,
                            uint8_t offset, uint16_t length, uint8_t type)
{
    struct gattc_write_cmd *cmd = KE_MSG_ALLOC_DYN(GATTC_WRITE_CMD,
                                                   KE_BUILD_ID(TASK_GATTC,
                                                               conidx),
                                                   TASK_APP, gattc_write_cmd,
                                                   length * sizeof(uint8_t));

    if (type == GATTC_WRITE)
    {
        /* Write request that needs a response from peer device */
        cmd->operation    = GATTC_WRITE;
        cmd->auto_execute = 1;
    }
    else if (type == GATTC_WRITE_NO_RESPONSE)
    {
        /* Write command that doesn't need a response from peer device */
        cmd->operation    = GATTC_WRITE_NO_RESPONSE;
        cmd->auto_execute = 0;
    }

    cmd->handle  = handle;
    cmd->seq_num = 0x00;
    cmd->offset  = offset;
    cmd->cursor  = 0;
    cmd->length  = length;
    memcpy(cmd->value, (uint8_t *)value, length);



   		uint8_t ble_env_index  =  Find_Ble_Env(conidx);
   		if (ble_env_index ==0xFF) return ;
    cs_env[ble_env_index].sentSuccess = 0;

    /* Send the message  */
    ke_msg_send(cmd);
}

/* ----------------------------------------------------------------------------
 * Function      : void CUSTOMSC_ReadLong(uint8_t conidx)
 * ----------------------------------------------------------------------------
 * Description   : Send a read command to the characteristic handle
 * Inputs        : - conidx       - Connection index
 *                 - handle       - Attribute handle
 *                 - length       - Length of value
 * Outputs       : None
 * Assumptions   : None
 * ------------------------------------------------------------------------- */
void CustomSrvice_SendReadCmd(uint8_t conidx, uint16_t handle,
                            uint8_t offset, uint16_t length)
{
    struct gattc_read_cmd *cmd = KE_MSG_ALLOC(GATTC_READ_CMD,
                                              KE_BUILD_ID(TASK_GATTC,
                                                          conidx),
                                              TASK_APP,
                                              gattc_read_cmd);
    cmd->operation = GATTC_READ_LONG;
    cmd->seq_num = 0;
    cmd->req.simple.handle = handle;
    cmd->req.simple.length = length;
    cmd->req.simple.offset = 0;
    ke_msg_send(cmd);
}


void J10_SendReadCmd(uint8_t ear_idx,uint16_t att_idx)
{
	if (cs_env[ear_idx].state !=CS_ALL_ATTS_DISCOVERED ) return ;

	uint8_t conidx = arr_ble_env[ear_idx].conidx;
	uint8_t uuid[2][16] = CS_CHARACTERISTICS_LIST;
	uint8_t param_uuid[16];
	memcpy(&param_uuid[0], &uuid[att_idx][0],16);

	for (int i = 0; i < CS_IDX_NB; i++)
	{

		if (0 ==memcmp(param_uuid, &cs_env[ear_idx].disc_att[i].uuid, ATT_UUID_128_LEN))
		{
			uint16_t  hdl = cs_env[ear_idx].disc_att[i].pointer_hdl;
			PRINTF("\r\n sendredcmd: hdl=%d", hdl);

			CustomSrvice_SendReadCmd(conidx,hdl,0,J10_PACKETLEN);

			break;
		}
	}


}


void J10_SendWriteCmd(uint8_t ear_idx,uint16_t att_idx, uint8_t* value, uint16_t length)
{
	if (cs_env[ear_idx].state !=CS_ALL_ATTS_DISCOVERED ) return ;

	uint8_t conidx = arr_ble_env[ear_idx].conidx;
	uint8_t uuid[2][16] = CS_CHARACTERISTICS_LIST;
	uint8_t param_uuid[16];
	memcpy(&param_uuid[0], &uuid[att_idx][0],16);

	for (int i = 0; i < CS_IDX_NB; i++)
	{

		if (0 ==memcmp(param_uuid, &cs_env[ear_idx].disc_att[i].uuid, ATT_UUID_128_LEN))
		{
			uint16_t  hdl = cs_env[ear_idx].disc_att[i].pointer_hdl;
			PRINTF("\r\n sendwritecmd: hdl=%d", hdl);

			CustomSrvice_SendWrite(  conidx,  value,   hdl, 0,  length,   GATTC_WRITE_NO_RESPONSE) ;


			break;
		}
	}


}

