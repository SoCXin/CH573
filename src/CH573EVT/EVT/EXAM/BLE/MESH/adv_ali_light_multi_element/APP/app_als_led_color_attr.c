/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_als_led_color_attr.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/03/24
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
#include "CONFIG.h"
#include "MESH_LIB.h"
#include "app_mesh_config.h"
#include "app_als_led_color_attr.h"
#include "app_vendor_model.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */

#define ALI_DEF_TTL    (10)

// 模拟led_color值
int32_t device_led_color_R = 0;
int32_t device_led_color_G = 0;
int32_t device_led_color_B = 0;

/*******************************************************************************
 * Function Name  : read_led_color
 * Description    : 获取当前led_color
 * Input          : None
 * Return         : None
 *******************************************************************************/
void read_led_color(int32_t *pcolor_R, int32_t *pcolor_G, int32_t *pcolor_B)
{
    APP_DBG("device_led_color_R: %d ", (int32_t)device_led_color_R);
    APP_DBG("device_led_color_G: %d ", (int32_t)device_led_color_G);
    APP_DBG("device_led_color_B: %d ", (int32_t)device_led_color_B);
    *pcolor_R = device_led_color_R;
    *pcolor_G = device_led_color_G;
    *pcolor_B = device_led_color_B;
}

/*******************************************************************************
 * Function Name  : set_led_colors
 * Description    : 设置当前led_color
 * Input          : None
 * Return         : None
 *******************************************************************************/
void set_led_color(int32_t color_R, int32_t color_G, int32_t color_B)
{
    device_led_color_R = color_R;
    device_led_color_G = color_G;
    device_led_color_B = color_B;
}

/*******************************************************************************
 * Function Name  : gen_led_color_status
 * Description    : 回复天猫精灵led_color
 * Input          : model: 模型参数
 *										ctx：数据参数
 * Return         : None
 *******************************************************************************/
static void gen_led_color_status(struct bt_mesh_model   *model,
                                 struct bt_mesh_msg_ctx *ctx)
{
    NET_BUF_SIMPLE_DEFINE(msg, 32);
    int     err;
    int32_t color_R;
    int32_t color_G;
    int32_t color_B;
    ////////////////////////////////////////////////////////////////////////
    //  0xD3  0xA8  0x01  |  0x##   |  0x##  0x##       |  0x##  0x## ....//
    //      Opcode        |  TID    | Attribute Type    | Attribute Value //
    ////////////////////////////////////////////////////////////////////////
    bt_mesh_model_msg_init(&msg, OP_VENDOR_MESSAGE_ATTR_STATUS);
    net_buf_simple_add_u8(&msg, als_avail_tid_get());
    net_buf_simple_add_le16(&msg, ALI_GEN_ATTR_TYPE_RGBCOLOR);
    read_led_color(&color_R, &color_G, &color_B);
    net_buf_simple_add_le32(&msg, color_R);
    net_buf_simple_add_le32(&msg, color_G);
    net_buf_simple_add_le32(&msg, color_B);

    APP_DBG("ttl: 0x%02x dst: 0x%04x", ctx->recv_ttl, ctx->recv_dst);

    if(ctx->recv_ttl != ALI_DEF_TTL)
    {
        ctx->send_ttl = BLE_MESH_TTL_DEFAULT;
    }
    else
    {
        ctx->send_ttl = 0;
    }

    err = bt_mesh_model_send(model, ctx, &msg, NULL, NULL);
    if(err)
    {
        APP_DBG("send status failed: %d", err);
    }
}

/*******************************************************************************
 * Function Name  : gen_led_color_get
 * Description    : 天猫精灵下发的获取led_color命令
 * Input          : model: 模型参数
 *										ctx：数据参数
 *										buf: 数据内容
 * Return         : None
 *******************************************************************************/
void gen_led_color_get(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf)
{
    APP_DBG(" ");
    gen_led_color_status(model, ctx);
}

/*******************************************************************************
* Function Name  : gen_led_color_set
* Description    : 天猫精灵下发的设置led_color命令
                                        如果与当前led_color不同,还需要发送ind给天猫
* Input          : model: 模型参数
*										ctx：数据参数
*										buf: 数据内容
* Return         : None
*******************************************************************************/
void gen_led_color_set(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf)
{
    struct indicate_param param = {
        .trans_cnt = 0x09,
        .period = K_MSEC(300),
        .rand = (tmos_rand() % 50),
        .tid = als_avail_tid_get(),
    };

    APP_DBG("ttl: 0x%02x dst: 0x%04x rssi: %d len %d",
            ctx->recv_ttl, ctx->recv_dst, ctx->recv_rssi, buf->len);

    if((buf->data[1] | (buf->data[2] << 8)) == ALI_GEN_ATTR_TYPE_RGBCOLOR)
    {
        APP_DBG("%x %x %x %x %x %x ",
                buf->data[0], buf->data[1], buf->data[2], buf->data[3], buf->data[4], buf->data[5]);
        int32_t color_R = (buf->data[3] | (buf->data[4] << 8) | (buf->data[5] << 16) | (buf->data[6] << 24));
        int32_t color_G = (buf->data[7] | (buf->data[8] << 8) | (buf->data[9] << 16) | (buf->data[10] << 24));
        int32_t color_B = (buf->data[11] | (buf->data[12] << 8) | (buf->data[13] << 16) | (buf->data[14] << 24));
        //        int32_t color_R = (buf->data[3]);
        //        int32_t color_G = (buf->data[4]);
        //        int32_t color_B = (buf->data[5]);
        // 命令为设定值
        set_led_color(color_R, color_G, color_B);
    }

    if(ctx->recv_ttl != ALI_DEF_TTL)
    {
        param.send_ttl = BLE_MESH_TTL_DEFAULT;
    }

    /* Overwrite default configuration */
    if(BLE_MESH_ADDR_IS_UNICAST(ctx->recv_dst))
    {
        param.rand = 0;
        param.send_ttl = BLE_MESH_TTL_DEFAULT;
        param.period = K_MSEC(100);
    }

    send_led_color_indicate(&param);

    gen_led_color_status(model, ctx);
}

/*******************************************************************************
 * Function Name  : gen_led_color_set_unack
 * Description    : 天猫精灵下发的设置led_color命令(无应答)
 * Input          : model: 模型参数
 *										ctx：数据参数
 *										buf: 数据内容
 * Return         : None
 *******************************************************************************/
void gen_led_color_set_unack(struct bt_mesh_model   *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple  *buf)
{
    APP_DBG(" ");

    if((buf->data[1] | (buf->data[2] << 8)) == ALI_GEN_ATTR_TYPE_RGBCOLOR)
    {
        uint32_t color_R = (buf->data[3] | (buf->data[4] << 8) | (buf->data[5] << 16) | (buf->data[6] << 24));
        uint32_t color_G = (buf->data[7] | (buf->data[8] << 8) | (buf->data[9] << 16) | (buf->data[10] << 24));
        uint32_t color_B = (buf->data[11] | (buf->data[12] << 8) | (buf->data[13] << 16) | (buf->data[14] << 24));
        // 命令为设定值
        set_led_color(color_R, color_G, color_B);
    }
}

/******************************** endfile @ main ******************************/
