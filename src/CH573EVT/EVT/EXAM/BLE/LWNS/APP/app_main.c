/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2021/11/17
 * Description        :
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "stdlib.h"
#include "lwns_adapter_csma_mac.h"
#include "lwns_adapter_blemesh_mac.h"
#include "lwns_adapter_no_mac.h"
#include "lwns_broadcast_example.h"
#include "lwns_ruc_example.h"
#include "lwns_rucft_example.h"
#include "lwns_unicast_example.h"
#include "lwns_multicast_example.h"
#include "lwns_netflood_example.h"
#include "lwns_uninetflood_example.h"
#include "lwns_multinetflood_example.h"
#include "lwns_mesh_example.h"

//ÿ���ļ�����debug��ӡ�Ŀ��أ���0���Խ�ֹ���ļ��ڲ���ӡ
#define DEBUG_PRINT_IN_THIS_FILE    1
#if DEBUG_PRINT_IN_THIS_FILE
  #define PRINTF(...)    PRINT(__VA_ARGS__)
#else
  #define PRINTF(...) \
    do                \
    {                 \
    } while(0)
#endif

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4))) uint32_t MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if(defined(BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02};
#endif

/*********************************************************************
 * @fn      Main_Circulation
 *
 * @brief   tmos��ѭ��.
 *
 * @param   None.
 *
 * @return  None.
 */
__attribute__((section(".highcode")))
void Main_Circulation()
{
    while(1)
    {
        TMOS_SystemProcess();
    }
}

/*********************************************************************
 * @fn      main
 *
 * @brief   ������.
 *
 * @param   None.
 *
 * @return  None.
 */
int main(void)
{
#if(defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#if(defined(HAL_SLEEP)) && (HAL_SLEEP == TRUE)
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU);
#endif
#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif
    PRINTF("start.\n");
    {
        PRINTF("%s\n", VER_LIB);
    }
    CH57X_BLEInit();
    HAL_Init();
    RF_RoleInit();
    RF_Init();
    lwns_init(); //��ʼlwnsЭ��ջ
    //lwns_broadcast_process_init();//�㲥���ӳ�ʼ��
    //lwns_multicast_process_init();//�鲥���ӳ�ʼ��
    //lwns_unicast_process_init();//�������ӳ�ʼ��
    //lwns_ruc_process_init();//�ɿ��������ӳ�ʼ��
    //lwns_rucft_process_init();//�ɿ������ļ��������ӳ�ʼ��
    lwns_netflood_process_init(); //���緺�����ӳ�ʼ��
    //lwns_uninetflood_process_init();//�������緺�����ӳ�ʼ��
    //lwns_multinetflood_process_init();//�鲥���緺�����ӳ�ʼ��
    //lwns_mesh_process_init();//mesh�������ӳ�ʼ��
    Main_Circulation();
}
/******************************** endfile @ main ******************************/
