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
__attribute__((aligned(4))) u32 MEM_BUF[BLE_MEMHEAP_SIZE/4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = {0x84,0xC2,0xE4,0x33,0x32,0x32};
#endif

/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
  SetSysClock( CLK_SOURCE_PLL_32MHz );
#ifdef DEBUG
  GPIOA_SetBits(bTXD1);
  GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
  UART1_DefInit( );
#endif   
  PRINT("%s\n",VER_LIB);
  CH57X_BLEInit( );
	HAL_Init( );
	GAPRole_PeripheralInit( );
	Peripheral_Init( ); 
	while(1){
		TMOS_SystemProcess( );
	}
}

/******************************** endfile @ main ******************************/
