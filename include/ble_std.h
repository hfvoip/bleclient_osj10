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
 * ble_std.h
 * - Bluetooth standard header
 * ----------------------------------------------------------------------------
 * $Revision: 1.17 $
 * $Date: 2018/02/27 15:03:59 $
 * ------------------------------------------------------------------------- */

#ifndef BLE_STD_H
#define BLE_STD_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Number of APP Task Instances */
#define APP_IDX_MAX                     1

/* Bluetooth device address type */
#define BD_TYPE_PUBLIC                  0
#define BD_TYPE_PRIVATE                 1
#define BD_ADDRESS_TYPE                 BD_TYPE_PUBLIC

/* Length of Bluetooth address (in octets) */
#define BDADDR_LENGTH                   6

/* Static private Bluetooth device address */
#define PRIVATE_BDADDR                  { 0x94, 0x11, 0x11, 0xff, 0xff, 0xE3 }

/* Public Bluetooth device address */
#define APP_PUBLIC_BDADDR               { 0x11, 0x22, 0x22, 0x44, 0x55, 0x33 }

/* RF Oscillator accuracy in ppm */
#define RADIO_CLOCK_ACCURACY            20
#define APP_NB_PEERS                     2 /* 1-4 */

/* Bluetooth address of the peer device */
#define DIRECT_PEER_BD_ADDRESS1          { 0x61, 0x73, 0xAA, 0xBF, 0xC0, 0x60 }
#define DIRECT_PEER_BD_ADDRESS_TYPE1     BD_TYPE_PUBLIC

#define DIRECT_PEER_BD_ADDRESS2          { 0x94, 0x11, 0x22, 0xff, 0xff, 0xE6 }
#define DIRECT_PEER_BD_ADDRESS_TYPE2     BD_TYPE_PRIVATE


/* ON SEMICONDUCTOR Company ID */
#define APP_COMPANY_ID_DATA             { 0x4, 0xff, 0x62, 0x3, 0x3 }
#define APP_COMPANY_ID_DATA_LEN         (0x4 + 1)

/* Set scan interval to 62.5ms and scan window to 50% of the interval */
#define SCAN_INTERVAL                   100
#define SCAN_WINDOW                     50

/* Set the connection interval to 7.5ms and slave latency to zero */
//这个CON_INTERVAL_MAX 原来是6,导致disconnected ，现在改为60
#define CON_INTERVAL_MIN                6
#define CON_INTERVAL_MAX                60
#define CON_SLAVE_LATENCY               0

/* Set supervisory timeout to 3s */
#define CON_SUP_TIMEOUT                 300

#define PACKET_SIZE                     244
#define MTU_SIZE                        PACKET_SIZE + 7

/* GAPM configuration definitions */
#define RENEW_DUR                       15000
#define MTU_MAX                         0x200
#define MPS_MAX                         0x200
#define ATT_CFG                         0x80
#define TX_OCT_MAX                      0xfb
#define TX_TIME_MAX                     (14 * 8 + TX_OCT_MAX * 8)

/* Define the available application states */
enum appm_state
{
    /* Initialization state */
    APPM_INIT,

    /* Database create state */
    APPM_CREATE_DB,

    /* Ready State */
    APPM_READY,

    /* Connecting state */
    APPM_CONNECTING,

    /* Connected state */
    APPM_CONNECTED,

    /* Number of defined states */
    APPM_STATE_MAX
};

/* List of message handlers that are used by the Bluetooth application manager */
#define BLE_MESSAGE_HANDLER_LIST                                            \
    DEFINE_MESSAGE_HANDLER(GAPM_CMP_EVT, GAPM_CmpEvt),                      \
    DEFINE_MESSAGE_HANDLER(GAPM_PROFILE_ADDED_IND, GAPM_ProfileAddedInd),   \
    DEFINE_MESSAGE_HANDLER(GAPC_CONNECTION_REQ_IND, GAPC_ConnectionReqInd), \
    DEFINE_MESSAGE_HANDLER(GAPC_CMP_EVT, GAPC_CmpEvt),                      \
    DEFINE_MESSAGE_HANDLER(GAPC_DISCONNECT_IND, GAPC_DisconnectInd),        \
    DEFINE_MESSAGE_HANDLER(GAPC_PARAM_UPDATED_IND, GAPC_ParamUpdatedInd),   \
    DEFINE_MESSAGE_HANDLER(GAPC_PARAM_UPDATE_REQ_IND, GAPC_ParamUpdateReqInd)

/* Standard declaration/description UUIDs in 16-byte format */
#define ATT_DECL_CHARACTERISTIC_128     { 0x03, 0x28, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }
#define ATT_DESC_CLIENT_CHAR_CFG_128    { 0x02, 0x29, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }
#define ATT_DESC_CHAR_USER_DESC_128     { 0x01, 0x29, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
                                          0x00, 0x00, 0x00, 0x00 }

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

/* Support for the application manager and the application environment */
extern const struct ke_state_handler appm_default_handler;
extern ke_state_t appm_state[APP_IDX_MAX];

struct ble_env_tag
{
    /* Connection handle */
    uint16_t conhdl;

    /* Connection index */
    uint8_t conidx;

    /* Next service to initialize */
    uint8_t next_svc;

    /* Next service to enable */
    uint8_t next_svc_enable;

    /* Application state */
    uint8_t state;

    /* Connection parameters */
    uint16_t con_interval;
    uint16_t time_out;
    uint16_t con_latency;

    uint16_t updated_con_interval;
    uint16_t updated_latency;
    uint16_t updated_suo_to;
};

/* Support for the application manager and the application environment */
extern struct ble_env_tag arr_ble_env[APP_NB_PEERS];

/* Bluetooth Device Address */
extern uint8_t bdaddr[BDADDR_LENGTH];

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/
extern uint8_t Find_Ble_Env(uint8_t conidx) ;
/* Bluetooth baseband application support functions */
extern void BLE_Initialize(void);

extern bool Service_Add(void);

extern void Connection_SendStartCmd(void);

extern void BLE_SetServiceState(bool enable, uint8_t conidx);

extern bool Service_Enable(uint8_t conidx);

/* MTU and data-length extension support functions */
extern void Connection_SetPktLength(uint8_t conidx, uint16_t length);

extern void Connection_ExchangeMTUCmd(uint8_t conidx);

extern void Connection_SetPHY(uint8_t conidx,uint8_t rx_rate, uint8_t tx_rate);

/* Bluetooth event and message handlers */
extern int GAPM_ProfileAddedInd(ke_msg_id_t const msgid,
                                struct gapm_profile_added_ind const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);

extern int GAPM_CmpEvt(ke_msg_id_t const msgid,
                       struct gapm_cmp_evt const *param,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);

extern int GAPC_CmpEvt(ke_msg_id_t const msgid,
                       struct gapc_cmp_evt const *param,
                       ke_task_id_t const dest_id,
                       ke_task_id_t const src_id);

extern int GAPC_DisconnectInd(ke_msg_id_t const msgid,
                              struct gapc_disconnect_ind const *param,
                              ke_task_id_t const dest_id,
                              ke_task_id_t const src_id);

extern int GAPC_ParamUpdatedInd(ke_msg_id_t const msgid,
                                struct gapc_param_updated_ind const *param,
                                ke_task_id_t const dest_id,
                                ke_task_id_t const src_id);

extern int GAPC_ParamUpdateReqInd(ke_msg_id_t const msg_id,
                                  struct gapc_param_update_req_ind const *param,
                                  ke_task_id_t const dest_id,
                                  ke_task_id_t const src_id);

extern int GAPC_ConnectionReqInd(ke_msg_id_t const msgid,
                                 struct gapc_connection_req_ind const *param,
                                 ke_task_id_t const dest_id,
                                 ke_task_id_t const src_id);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BLE_STD_H */
