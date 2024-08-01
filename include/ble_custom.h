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
 * ble_custom.h
 * - Bluetooth custom service header
 * ----------------------------------------------------------------------------
 * $Revision: 1.11 $
 * $Date: 2018/02/27 15:03:59 $
 * ------------------------------------------------------------------------- */

#ifndef BLE_CUSTOM_H
#define BLE_CUSTOM_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Custom service UUIDs */
#define CS_SVC_UUID                     { 0x24, 0xdc, 0x0e, 0x6e, 0x01, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_TX_UUID       { 0x24, 0xdc, 0x0e, 0x6e, 0x02, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHARACTERISTIC_RX_UUID       { 0x24, 0xdc, 0x0e, 0x6e, 0x03, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }



#define ATT_DECL_CHAR() \
    { ATT_DECL_CHARACTERISTIC_128, PERM(RD, ENABLE), 0, 0 }
#define ATT_DECL_CHAR_UUID_16(uuid, perm, max_length) \
    { uuid, perm, max_length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_16) }
#define ATT_DECL_CHAR_UUID_32(uuid, perm, max_length) \
    { uuid, perm, max_length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_32) }
#define ATT_DECL_CHAR_UUID_128(uuid, perm, max_length) \
    { uuid, perm, max_length, PERM(RI, ENABLE) | PERM(UUID_LEN, UUID_128) }
#define ATT_DECL_CHAR_CCC()                                                     \
    { ATT_DESC_CLIENT_CHAR_CFG_128, PERM(RD, ENABLE) | PERM(WRITE_REQ, ENABLE), \
      0, PERM(RI, ENABLE) }
#define ATT_DECL_CHAR_USER_DESC(max_length)                      \
    { ATT_DESC_CHAR_USER_DESC_128, PERM(RD, ENABLE), max_length, \
      PERM(RI, ENABLE) }

/* Define the available custom service states */
enum cs_state
{
    CS_INIT,
    CS_SERVICE_DISCOVERD,
    CS_ALL_ATTS_DISCOVERED,
};

#define CS_TX_VALUE_MAX_LENGTH          MTU_SIZE - 3
#define CS_RX_VALUE_MAX_LENGTH          MTU_SIZE - 3

#define J10_PACKETLEN 128

#define CS_CHARACTERISTICS_LIST         { CS_CHARACTERISTIC_TX_UUID, \
                                          CS_CHARACTERISTIC_RX_UUID };

enum cs_idx_att
{
    /* TX Characteristic */
    CS_IDX_TX_CHAR,

    /* RX Characteristic */
    CS_IDX_RX_CHAR,

    /* Max number of characteristics */
    CS_IDX_NB,
};

/* List of message handlers that are used by the custom service application manager */
#define CS_MESSAGE_HANDLER_LIST                                            \
    { GATTC_DISC_SVC_IND, (ke_msg_func_t)GATTC_DiscSvcInd },              \
    DEFINE_MESSAGE_HANDLER(GATTC_CMP_EVT, (ke_msg_func_t)GATTC_CmpEvt),   \
    DEFINE_MESSAGE_HANDLER(GATTC_DISC_CHAR_IND,                            \
                           (ke_msg_func_t)GATTC_DiscCharInd),             \
    DEFINE_MESSAGE_HANDLER(GATTC_READ_IND, (ke_msg_func_t)GATTC_ReadInd), \
    DEFINE_MESSAGE_HANDLER(GATTC_EVENT_IND, (ke_msg_func_t)GATTC_EvtInd)

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

struct discovered_char_att
{
    /* Database element handle */
    uint16_t attr_hdl;

    /* Pointer attribute handle to UUID */
    uint16_t pointer_hdl;

    /* Properties */
    uint8_t prop;

    /* UUID length */
    uint8_t uuid_len;

    /* Characteristic UUID */
    uint8_t uuid[16];
};

struct cs_env_tag
{
    /* The service start and end handle values in
     * the database of attributes in the stack */
    uint16_t start_hdl;
    uint16_t end_hdl;

    /* The value of TX characteristic value */
    uint8_t tx_value[CS_TX_VALUE_MAX_LENGTH];

    /* The value of RX characteristic value */
    uint8_t rx_value[CS_RX_VALUE_MAX_LENGTH];

    /* A flag that indicates that PDU has been sent over the air */
    bool sentSuccess;

    /* Custom service */
    uint16_t cnt_notifc;
    uint8_t val_notif;

    /* The state machine for service discovery, it is not used for server role */
    uint8_t state;

    uint8_t disc_attnum;
    struct discovered_char_att disc_att[CS_IDX_NB];
};

extern struct cs_env_tag cs_env[];

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/
extern void CustomService_Env_Initialize(void);

extern void CustomService_ServiceEnable(uint8_t conidx);

extern void CustomService_SendNotification(uint8_t conidx, uint8_t attidx,
                                           uint8_t *value, uint8_t length);

extern void CustomSrvice_SendWrite(uint8_t conidx, uint8_t *value,
                                   uint16_t handle,
                                   uint8_t offset, uint16_t length,
                                   uint8_t type);

extern void CustomSrvice_SendReadCmd(uint8_t conidx,
                                     uint16_t handle,
                                     uint8_t offset, uint16_t length);

extern int GATTC_DiscCharInd(ke_msg_id_t const msgid,
                             struct gattc_disc_char_ind const *param,
                             ke_task_id_t const dest_id,
                             ke_task_id_t const src_id);

extern int GATTC_ReadInd(ke_msg_id_t const msg_id,
                         struct gattc_read_ind *param,
                         ke_task_id_t const dest_id,
                         ke_task_id_t const src_id);

extern int GATTC_EvtInd(ke_msg_id_t const msg_id,
                        struct gattc_event_ind *param,
                        ke_task_id_t const dest_id,
                        ke_task_id_t const src_id);

extern int GATTC_DiscSvcInd(ke_msg_id_t const msgid,
                            struct gattc_disc_svc_ind const *param,
                            ke_task_id_t const dest_id,
                            ke_task_id_t const src_id);

extern int GATTC_CmpEvt(ke_msg_id_t const msgid,
                        struct gattc_cmp_evt const *param,
                        ke_task_id_t const dest_id,
                        ke_task_id_t const src_id);

extern void J10_SendReadCmd(uint8_t ear_idx,uint16_t att_idx);
extern void J10_SendWriteCmd(uint8_t ear_idx,uint16_t att_idx, uint8_t* value, uint16_t length);

/* ----------------------------------------------------------------------------
 *
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_CUSTOM_H */
