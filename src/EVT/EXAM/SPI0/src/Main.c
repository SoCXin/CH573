/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : SPI0��ʾ Master/Slave ģʽ�����շ�
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH57x_common.h"

__attribute__((aligned(4))) uint8_t spiBuff[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2, 3, 4, 5, 6};
__attribute__((aligned(4))) uint8_t spiBuffrev[16];

/*********************************************************************
 * @fn      DebugInit
 *
 * @brief   ���Գ�ʼ��
 *
 * @return  none
 */
void DebugInit(void)
{
    GPIOA_SetBits(GPIO_Pin_9);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
}

/*********************************************************************
 * @fn      main
 *
 * @brief   ������
 *
 * @return  none
 */
int main()
{
    uint8_t i;

    SetSysClock(CLK_SOURCE_PLL_60MHz);

    /* ���ô��ڵ��� */
    DebugInit();
    PRINT("Start @ChipID=%02X\n", R8_CHIP_ID);

#if 1
    /* ����ģʽ */
    PRINT("1.spi0 mul master mode send data ...\n");
    DelayMs(100);

    GPIOA_SetBits(GPIO_Pin_12);
    GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14, GPIO_ModeOut_PP_5mA);
    SPI0_MasterDefInit();

    // ���ֽڷ���
    GPIOA_ResetBits(GPIO_Pin_12);
    SPI0_MasterSendByte(0x55);
    GPIOA_SetBits(GPIO_Pin_12);
    DelayMs(1);
    GPIOA_ResetBits(GPIO_Pin_12);
    i = SPI0_MasterRecvByte();
    GPIOA_SetBits(GPIO_Pin_12);
    DelayMs(2);

    // FIFO ��������
    GPIOA_ResetBits(GPIO_Pin_12);
    SPI0_MasterTrans(spiBuff, 9);
    GPIOA_SetBits(GPIO_Pin_12);
    DelayMs(1);
    GPIOA_ResetBits(GPIO_Pin_12);
    SPI0_MasterRecv(spiBuffrev, 12);
    GPIOA_SetBits(GPIO_Pin_12);
    DelayMs(1);

    // DMA ��������
    GPIOA_ResetBits(GPIO_Pin_12);
    SPI0_MasterDMATrans(spiBuff, 12);
    GPIOA_SetBits(GPIO_Pin_12);
    DelayMs(1);
    GPIOA_ResetBits(GPIO_Pin_12);
    SPI0_MasterDMARecv(spiBuffrev, 12);
    GPIOA_SetBits(GPIO_Pin_12);

    PRINT("END ...\n");
    while(1);
#endif

#if 0
    /* �豸ģʽ */
    PRINT("1.spi0 mul slave mode \n");
    GPIOA_ModeCfg(GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15, GPIO_ModeIN_PU);
    SPI0_SlaveInit();
    i = SPI0_SlaveRecvByte();
    SPI0_SlaveSendByte(~i);

    SPI0_SlaveRecv(spiBuffrev, 9);
    SPI0_SlaveTrans(spiBuffrev, 12);
    SPI0_SlaveDMARecv(spiBuffrev, 12);
    SPI0_SlaveDMATrans(spiBuffrev, 12);

    while(1);
#endif

    while(1);
}
