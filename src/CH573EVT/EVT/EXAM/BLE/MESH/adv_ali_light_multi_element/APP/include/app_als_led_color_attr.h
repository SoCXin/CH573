/********************************** (C) COPYRIGHT *******************************
 * File Name          : app_als_led_color_attr.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/11/12
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#ifndef app_als_led_color_attr_H
#define app_als_led_color_attr_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MESH_LIB.h"

void read_led_color(int32_t *pcolor_R, int32_t *pcolor_G, int32_t *pcolor_B);

void set_led_color(int32_t color_R, int32_t color_G, int32_t color_B);

void gen_led_color_get(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf);

void gen_led_color_set(struct bt_mesh_model   *model,
                       struct bt_mesh_msg_ctx *ctx,
                       struct net_buf_simple  *buf);

void gen_led_color_set_unack(struct bt_mesh_model   *model,
                             struct bt_mesh_msg_ctx *ctx,
                             struct net_buf_simple  *buf);

#ifdef __cplusplus
}
#endif

#endif
