/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2020/08/06
 * Description        : ����ӻ�Ӧ��������������ϵͳ��ʼ��
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "gattprofile.h"
#include "peripheral.h"

/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4)))  u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x03,0x02,0x02};
#endif

extern void app_uart_process(void);
extern void app_uart_init(void);

/*******************************************************************************
 * Function Name  : Main_Circulation
 * Description    : ��ѭ��
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
__attribute__((section(".highcode")))
void Main_Circulation() {
    while (1) {
        TMOS_SystemProcess();
        app_uart_process();
    }
}

/*******************************************************************************
 * Function Name  : main
 * Description    : ������
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
int main(void) {
    //PWR_DCDCCfg( ENABLE );
    SetSysClock(CLK_SOURCE_PLL_60MHz);
#ifdef DEBUG
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();
#endif   
    PRINT("%s\n", VER_LIB);
    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();
    //RF_Init();
    app_uart_init();
    Main_Circulation();
}

/******************************** endfile @ main ******************************/