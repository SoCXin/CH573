/********************************** (C) COPYRIGHT *******************************
 * File Name          : app.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2022/01/18
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#include "CONFIG.h"
#include "MESH_LIB.h"
#include "app_vendor_model_srv.h"
#include "app.h"
#include "peripheral.h"
#include "HAL.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
#define ADV_TIMEOUT       K_MINUTES(10)

#define SELENCE_ADV_ON    0x01
#define SELENCE_ADV_OF    0x00

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

static uint8_t MESH_MEM[1024 * 2] = {0};

extern const ble_mesh_cfg_t app_mesh_cfg;
extern const struct device  app_dev;

static uint8_t App_TaskID = 0; // Task ID for internal task/event processing

static uint16_t App_ProcessEvent(uint8_t task_id, uint16_t events);

static uint8_t dev_uuid[16] = {0}; // ���豸��UUID
uint8_t        MACAddr[6];         // ���豸��mac

#if(!CONFIG_BLE_MESH_PB_GATT)
NET_BUF_SIMPLE_DEFINE_STATIC(rx_buf, 65);
#endif /* !PB_GATT */

/*********************************************************************
 * LOCAL FUNCION
 */

static void cfg_srv_rsp_handler( const cfg_srv_status_t *val );
static void link_open(bt_mesh_prov_bearer_t bearer);
static void link_close(bt_mesh_prov_bearer_t bearer, uint8_t reason);
static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index);
static void vendor_model_srv_rsp_handler(const vendor_model_srv_status_t *val);
static void prov_reset(void);

static struct bt_mesh_cfg_srv cfg_srv = {
    .relay = BLE_MESH_RELAY_ENABLED,
    .beacon = BLE_MESH_BEACON_DISABLED,
#if(CONFIG_BLE_MESH_FRIEND)
    .frnd = BLE_MESH_FRIEND_ENABLED,
#endif
#if(CONFIG_BLE_MESH_PROXY)
    .gatt_proxy = BLE_MESH_GATT_PROXY_ENABLED,
#endif
    /* Ĭ��TTLΪ3 */
    .default_ttl = 3,
    /* �ײ㷢����������7�Σ�ÿ�μ��10ms�������ڲ�������� */
    .net_transmit = BLE_MESH_TRANSMIT(7, 10),
    /* �ײ�ת����������7�Σ�ÿ�μ��10ms�������ڲ�������� */
    .relay_retransmit = BLE_MESH_TRANSMIT(7, 10),
    .handler = cfg_srv_rsp_handler,
};

/* Attention on */
void app_prov_attn_on(struct bt_mesh_model *model)
{
    APP_DBG("app_prov_attn_on");
}

/* Attention off */
void app_prov_attn_off(struct bt_mesh_model *model)
{
    APP_DBG("app_prov_attn_off");
}

const struct bt_mesh_health_srv_cb health_srv_cb = {
    .attn_on = app_prov_attn_on,
    .attn_off = app_prov_attn_off,
};

static struct bt_mesh_health_srv health_srv = {
    .cb = &health_srv_cb,
};

BLE_MESH_HEALTH_PUB_DEFINE(health_pub, 8);

uint16_t cfg_srv_keys[CONFIG_MESH_MOD_KEY_COUNT_DEF] = {BLE_MESH_KEY_UNUSED};
uint16_t cfg_srv_groups[CONFIG_MESH_MOD_GROUP_COUNT_DEF] = {BLE_MESH_ADDR_UNASSIGNED};

uint16_t health_srv_keys[CONFIG_MESH_MOD_KEY_COUNT_DEF] = {BLE_MESH_KEY_UNUSED};
uint16_t health_srv_groups[CONFIG_MESH_MOD_GROUP_COUNT_DEF] = {BLE_MESH_ADDR_UNASSIGNED};

// rootģ�ͼ���
static struct bt_mesh_model root_models[] = {
    BLE_MESH_MODEL_CFG_SRV(cfg_srv_keys, cfg_srv_groups, &cfg_srv),
    BLE_MESH_MODEL_HEALTH_SRV(health_srv_keys, health_srv_groups, &health_srv, &health_pub),
};

struct bt_mesh_vendor_model_srv vendor_model_srv = {
    .srv_tid.trans_tid = 0xFF,
    .handler = vendor_model_srv_rsp_handler,
};

uint16_t vnd_model_srv_keys[CONFIG_MESH_MOD_KEY_COUNT_DEF] = {BLE_MESH_KEY_UNUSED};
uint16_t vnd_model_srv_groups[CONFIG_MESH_MOD_GROUP_COUNT_DEF] = {BLE_MESH_ADDR_UNASSIGNED};

// �Զ���ģ�ͼ���
struct bt_mesh_model vnd_models[] = {
    BLE_MESH_MODEL_VND_CB(CID_WCH, BLE_MESH_MODEL_ID_WCH_SRV, vnd_model_srv_op, NULL, vnd_model_srv_keys,
                          vnd_model_srv_groups, &vendor_model_srv, &bt_mesh_vendor_model_srv_cb),
};

// ģ����� elements
static struct bt_mesh_elem elements[] = {
    {
        /* Location Descriptor (GATT Bluetooth Namespace Descriptors) */
        .loc = (0),
        .model_count = ARRAY_SIZE(root_models),
        .models = (root_models),
        .vnd_model_count = ARRAY_SIZE(vnd_models),
        .vnd_models = (vnd_models),
    }
};

// elements ���� Node Composition
const struct bt_mesh_comp app_comp = {
    .cid = 0x07D7, // WCH ��˾id
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};

// ���������ͻص�
static const struct bt_mesh_prov app_prov = {
    .uuid = dev_uuid,
    .link_open = link_open,
    .link_close = link_close,
    .complete = prov_complete,
    .reset = prov_reset,
};

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

/*********************************************************************
 * @fn      prov_enable
 *
 * @brief   ʹ����������
 *
 * @return  none
 */
static void prov_enable(void)
{
    if(bt_mesh_is_provisioned())
    {
        return;
    }

    // Make sure we're scanning for provisioning inviations
    bt_mesh_scan_enable();
    // Enable unprovisioned beacon sending
    bt_mesh_beacon_enable();

    if(CONFIG_BLE_MESH_PB_GATT)
    {
        bt_mesh_proxy_prov_enable();
    }
}

/*********************************************************************
 * @fn      link_open
 *
 * @brief   ����ʱ���link�򿪻ص�
 *
 * @param   bearer  - ��ǰlink��PB_ADV����PB_GATT
 *
 * @return  none
 */
static void link_open(bt_mesh_prov_bearer_t bearer)
{
    APP_DBG("");
}

/*********************************************************************
 * @fn      link_close
 *
 * @brief   �������link�رջص�
 *
 * @param   bearer  - ��ǰlink��PB_ADV����PB_GATT
 * @param   reason  - link�ر�ԭ��
 *
 * @return  none
 */
static void link_close(bt_mesh_prov_bearer_t bearer, uint8_t reason)
{
    if(reason != CLOSE_REASON_SUCCESS)
        APP_DBG("reason %x", reason);
}

/*********************************************************************
 * @fn      prov_complete
 *
 * @brief   ������ɻص������¿�ʼ�㲥
 *
 * @param   net_idx     - ����key��index
 * @param   addr        - link�ر�ԭ�������ַ
 * @param   flags       - �Ƿ���key refresh״̬
 * @param   iv_index    - ��ǰ����iv��index
 *
 * @return  none
 */
static void prov_complete(uint16_t net_idx, uint16_t addr, uint8_t flags, uint32_t iv_index)
{
    APP_DBG("");
}

/*********************************************************************
 * @fn      prov_reset
 *
 * @brief   ��λ�������ܻص�
 *
 * @param   none
 *
 * @return  none
 */
static void prov_reset(void)
{
    APP_DBG("");

    prov_enable();
}

/*********************************************************************
 * @fn      cfg_srv_rsp_handler
 *
 * @brief   config ģ�ͷ���ص�
 *
 * @param   val     - �ص������������������͡���������ִ��״̬
 *
 * @return  none
 */
static void cfg_srv_rsp_handler( const cfg_srv_status_t *val )
{
    if(val->cfgHdr.status)
    {
        // ��������ִ�в��ɹ�
        APP_DBG("warning opcode 0x%02x", val->cfgHdr.opcode);
        return;
    }
    if(val->cfgHdr.opcode == OP_APP_KEY_ADD)
    {
        APP_DBG("App Key Added");
    }
    else if(val->cfgHdr.opcode == OP_MOD_APP_BIND)
    {
        APP_DBG("Vendor Model Binded");
    }
    else if(val->cfgHdr.opcode == OP_MOD_SUB_ADD)
    {
        APP_DBG("Vendor Model Subscription Set");
    }
    else
    {
        APP_DBG("Unknow opcode 0x%02x", val->cfgHdr.opcode);
    }
}

/*********************************************************************
 * @fn      vendor_model_srv_rsp_handler
 *
 * @brief   �Զ���ģ�ͷ���ص�
 *
 * @param   val     - �ص�������������Ϣ���͡��������ݡ����ȡ���Դ��ַ
 *
 * @return  none
 */
static void vendor_model_srv_rsp_handler(const vendor_model_srv_status_t *val)
{
    if(val->vendor_model_srv_Hdr.status)
    {
        // ��Ӧ�����ݴ��� ��ʱδ�յ�Ӧ��
        APP_DBG("Timeout opcode 0x%02x", val->vendor_model_srv_Hdr.opcode);
        return;
    }
    if(val->vendor_model_srv_Hdr.opcode == OP_VENDOR_MESSAGE_TRANSPARENT_MSG)
    {
        // �յ�͸������
        APP_DBG("len %d, data 0x%02x from 0x%04x", val->vendor_model_srv_Event.trans.len,
                val->vendor_model_srv_Event.trans.pdata[0],
                val->vendor_model_srv_Event.trans.addr);
        // ת��������(���������)
        peripheralChar4Notify(val->vendor_model_srv_Event.trans.pdata, val->vendor_model_srv_Event.trans.len);
    }
    else if(val->vendor_model_srv_Hdr.opcode == OP_VENDOR_MESSAGE_TRANSPARENT_WRT)
    {
        // �յ�write����
        APP_DBG("len %d, data 0x%02x from 0x%04x", val->vendor_model_srv_Event.write.len,
                val->vendor_model_srv_Event.write.pdata[0],
                val->vendor_model_srv_Event.write.addr);
        // ת��������(���������)
        peripheralChar4Notify(val->vendor_model_srv_Event.write.pdata, val->vendor_model_srv_Event.write.len);
    }
    else if(val->vendor_model_srv_Hdr.opcode == OP_VENDOR_MESSAGE_TRANSPARENT_IND)
    {
        // ���͵�indicate���յ�Ӧ��
    }
    else
    {
        APP_DBG("Unknow opcode 0x%02x", val->vendor_model_srv_Hdr.opcode);
    }
}

/*********************************************************************
 * @fn      keyPress
 *
 * @brief   �����ص�
 *
 * @param   keys    - ��������
 *
 * @return  none
 */
void keyPress(uint8_t keys)
{
    APP_DBG("%d", keys);

    switch(keys)
    {
        default:
        {
            uint8_t           status;
            struct send_param param = {
                .app_idx = vnd_models[0].keys[0], // ����Ϣʹ�õ�app key�������ض���ʹ�õ�0��key
                .addr = 0x01,                     // ����Ϣ������Ŀ�ĵص�ַ������Ϊ�������������ߣ�Ĭ�ϵ�ַΪ0x0001
                .trans_cnt = 0x01,                // ����Ϣ���û��㷢�ʹ���
                .period = K_MSEC(400),            // ����Ϣ�ش��ļ�������鲻С��(200+50*TTL)ms�������ݽϴ�����ӳ�
                .rand = (0),                      // ����Ϣ���͵�����ӳ�
                .tid = vendor_srv_tid_get(),      // tid��ÿ��������Ϣ����ѭ����srvʹ��128~191
                .send_ttl = BLE_MESH_TTL_DEFAULT, // ttl�����ض���ʹ��Ĭ��ֵ
            };
            uint8_t data[8] = {0, 1, 2, 3, 4, 5, 6, 7};
            //			status = vendor_message_srv_indicate(&param, data, 8);	// �����Զ���ģ�ͷ������Ӧ��ָʾ�����������ݣ�Ĭ�ϳ�ʱ2s
            status = vendor_message_srv_send_trans(&param, data, 8); // ���ߵ����Զ���ģ�ͷ����͸�������������ݣ�ֻ���ͣ���Ӧ�����
            if(status)
                APP_DBG("indicate failed %d", status);
            break;
        }
    }
}

/*********************************************************************
 * @fn      blemesh_on_sync
 *
 * @brief   ͬ��mesh���������ö�Ӧ���ܣ��������޸�
 *
 * @return  none
 */
void blemesh_on_sync(void)
{
    int        err;
    mem_info_t info;

    if(tmos_memcmp(VER_MESH_LIB, VER_MESH_FILE, strlen(VER_MESH_FILE)) == FALSE)
    {
        PRINT("head file error...\n");
        while(1);
    }

    info.base_addr = MESH_MEM;
    info.mem_len = ARRAY_SIZE(MESH_MEM);

#if(CONFIG_BLE_MESH_FRIEND)
    friend_init_register(bt_mesh_friend_init, friend_state);
#endif /* FRIEND */
#if(CONFIG_BLE_MESH_LOW_POWER)
    lpn_init_register(bt_mesh_lpn_init, lpn_state);
#endif /* LPN */

    GetMACAddress(MACAddr);
    tmos_memcpy(dev_uuid, MACAddr, 6);
    err = bt_mesh_cfg_set(&app_mesh_cfg, &app_dev, MACAddr, &info);
    if(err)
    {
        APP_DBG("Unable set configuration (err:%d)", err);
        return;
    }
    hal_rf_init();
    err = bt_mesh_comp_register(&app_comp);

#if(CONFIG_BLE_MESH_RELAY)
    bt_mesh_relay_init();
#endif /* RELAY  */
#if(CONFIG_BLE_MESH_PROXY || CONFIG_BLE_MESH_PB_GATT)
  #if(CONFIG_BLE_MESH_PROXY)
    bt_mesh_proxy_beacon_init_register((void *)bt_mesh_proxy_beacon_init);
    gatts_notify_register(bt_mesh_gatts_notify);
    proxy_gatt_enable_register(bt_mesh_proxy_gatt_enable);
  #endif /* PROXY  */
  #if(CONFIG_BLE_MESH_PB_GATT)
    proxy_prov_enable_register(bt_mesh_proxy_prov_enable);
  #endif /* PB_GATT  */

    bt_mesh_proxy_init();
#endif /* PROXY || PB-GATT */

#if(CONFIG_BLE_MESH_PROXY_CLI)
    bt_mesh_proxy_client_init(cli); //������
#endif                              /* PROXY_CLI */

    bt_mesh_prov_retransmit_init();
#if(!CONFIG_BLE_MESH_PB_GATT)
    adv_link_rx_buf_register(&rx_buf);
#endif /* !PB_GATT */
    err = bt_mesh_prov_init(&app_prov);

    bt_mesh_mod_init();
    bt_mesh_net_init();
    bt_mesh_trans_init();
    bt_mesh_beacon_init();

    bt_mesh_adv_init();

#if((CONFIG_BLE_MESH_PB_GATT) || (CONFIG_BLE_MESH_PROXY) || (CONFIG_BLE_MESH_OTA))
    bt_mesh_conn_adv_init();
#endif /* PROXY || PB-GATT || OTA */

#if(CONFIG_BLE_MESH_SETTINGS)
    bt_mesh_settings_init();
#endif /* SETTINGS */

#if(CONFIG_BLE_MESH_PROXY_CLI)
    bt_mesh_proxy_cli_adapt_init();
#endif /* PROXY_CLI */

#if((CONFIG_BLE_MESH_PROXY) || (CONFIG_BLE_MESH_PB_GATT) || \
    (CONFIG_BLE_MESH_PROXY_CLI) || (CONFIG_BLE_MESH_OTA))
    bt_mesh_adapt_init();
#endif /* PROXY || PB-GATT || PROXY_CLI || OTA */

    if(err)
    {
        APP_DBG("Initializing mesh failed (err %d)", err);
        return;
    }

    APP_DBG("Bluetooth initialized");

#if(CONFIG_BLE_MESH_SETTINGS)
    settings_load();
#endif /* SETTINGS */

    if(bt_mesh_is_provisioned())
    {
        APP_DBG("Mesh network restored from flash");
    }
    else
    {
        prov_enable();
    }

    APP_DBG("Mesh initialized");
}

/*********************************************************************
 * @fn      App_Init
 *
 * @brief   Ӧ�ò��ʼ��
 *
 * @return  none
 */
void App_Init()
{
    GAPRole_PeripheralInit();
    Peripheral_Init();

    App_TaskID = TMOS_ProcessEventRegister(App_ProcessEvent);

    blemesh_on_sync();
    HAL_KeyInit();
    HalKeyConfig(keyPress);
    tmos_start_task(App_TaskID, APP_NODE_TEST_EVT, 1600);
}

/*********************************************************************
 * @fn      App_ProcessEvent
 *
 * @brief   Ӧ�ò��¼���������
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
static uint16_t App_ProcessEvent(uint8_t task_id, uint16_t events)
{
    if(events & APP_NODE_TEST_EVT)
    {
        tmos_start_task(App_TaskID, APP_NODE_TEST_EVT, 2400);
        return (events ^ APP_NODE_TEST_EVT);
    }

    // Discard unknown events
    return 0;
}

/******************************** endfile @ main ******************************/