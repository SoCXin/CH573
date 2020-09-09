/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_SYS.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"


/*******************************************************************************
* Function Name  : SYS_GetInfoSta
* Description    : 获取当前系统信息状态
* Input          : i: 
					refer to SYS_InfoStaTypeDef
* Return         : DISABLE  -  关闭
				   ENABLE   -  开启
*******************************************************************************/
UINT8 SYS_GetInfoSta( SYS_InfoStaTypeDef i )
{
  if (i == STA_SAFEACC_ACT)
    return (R8_SAFE_ACCESS_SIG & RB_SAFE_ACC_ACT);
  else
    return (R8_GLOB_CFG_INFO & (1 << i));
}

/*******************************************************************************
* Function Name  : SYS_ResetExecute
* Description    : 执行系统软件复位
* Input          : None
* Return         : None
*******************************************************************************/
void SYS_ResetExecute( void )
{
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R8_RST_WDOG_CTRL |= RB_SOFTWARE_RESET;
  R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : SYS_DisableAllIrq
* Description    : 关闭所有中断，并保留当前中断值
* Input          : pirqv：当前保留中断值
* Return         : None
*******************************************************************************/
void SYS_DisableAllIrq( PUINT32 pirqv )
{
  *pirqv = (PFIC->ISR[0] >> 8) | (PFIC->ISR[1] << 24);
  PFIC->IRER[0] = 0xffffffff;
  PFIC->IRER[1] = 0xffffffff;
}

/*******************************************************************************
 * Function Name  : SYS_RecoverIrq
 * Description    : 恢复之前关闭的中断值
 * Input          : irq_status：当前保留中断值
 * Return         : None
 *******************************************************************************/
void SYS_RecoverIrq( UINT32 irq_status )
{
  PFIC->IENR[0] = (irq_status << 8);
  PFIC->IENR[1] = (irq_status >> 24);
}

/*******************************************************************************
* Function Name  : SYS_GetSysTickCnt
* Description    : 获取当前系统(SYSTICK)计数值
* Input          : None
* Return         : 当前计数值
*******************************************************************************/
UINT32 SYS_GetSysTickCnt( void )
{
	UINT32 val;

	val = SysTick->CNT;
	return( val );
}

/*******************************************************************************
* Function Name  : WWDG_ITCfg
* Description    : 看门狗定时器溢出中断使能
* Input          : DISABLE-溢出不中断      ENABLE-溢出中断
* Return         : None
*******************************************************************************/
void  WWDG_ITCfg( FunctionalState s )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	if(s == DISABLE)		R8_RST_WDOG_CTRL&=~RB_WDOG_INT_EN;
	else 					R8_RST_WDOG_CTRL|=RB_WDOG_INT_EN;
	R8_SAFE_ACCESS_SIG = 0;	
}

/*******************************************************************************
* Function Name  : WWDG_ResetCfg
* Description    : 看门狗定时器复位功能
* Input          : DISABLE-溢出不复位      ENABLE-溢出系统复位
* Return         : None
*******************************************************************************/
void WWDG_ResetCfg( FunctionalState s )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	if(s == DISABLE)		R8_RST_WDOG_CTRL&=~RB_WDOG_RST_EN;
	else 					R8_RST_WDOG_CTRL|=RB_WDOG_RST_EN;
	R8_SAFE_ACCESS_SIG = 0;	
}

/*******************************************************************************
* Function Name  : WWDG_ClearFlag
* Description    : 清除看门狗中断标志，重新加载计数值也可清除
* Input          : None
* Return         : None
*******************************************************************************/
void WWDG_ClearFlag( void )
{
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
	R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
	R8_RST_WDOG_CTRL |= RB_WDOG_INT_FLAG;
	R8_SAFE_ACCESS_SIG = 0;	
}


/*******************************************************************************
* Function Name  : mDelayuS
* Description    : uS 延时
* Input          : t: 时间参数
* Return         : None
*******************************************************************************/
__attribute__((section(".highcode")))
void mDelayuS( UINT16 t )
{
    UINT32 i;

#if     (FREQ_SYS == 40000000)
    i = t*10;
#elif	(FREQ_SYS == 32000000)
    i = t<<3;
#elif	(FREQ_SYS == 24000000)
    i = t*6;
#elif	(FREQ_SYS == 16000000)
    i = t<<2;
#elif	(FREQ_SYS == 8000000)
    i = t<<1;
#elif	(FREQ_SYS == 4000000)
    i = t;
#elif	(FREQ_SYS == 2000000)
    i = t>>1;
#elif	(FREQ_SYS == 1000000)
    i = t>>2;
#endif
    do
    {
    	__nop();
    }while(--i);
}

/*******************************************************************************
* Function Name  : mDelaymS
* Description    : mS 延时
* Input          : t: 时间参数
* Return         : None
*******************************************************************************/
__attribute__((section(".highcode")))
void mDelaymS( UINT16 t )
{
    UINT16 i;

    for(i=0; i<t; i++)
        mDelayuS(1000);
}


#ifdef DEBUG
int _write(int fd, char *buf, int size)
{
	int i;
	for(i=0; i<size; i++)
	{
#if  DEBUG == Debug_UART0
		while( R8_UART0_TFC == UART_FIFO_SIZE );                        /* 等待数据发送 */
		R8_UART0_THR = *buf++;                                               /* 发送数据 */
#elif DEBUG == Debug_UART1       
		while( R8_UART1_TFC == UART_FIFO_SIZE );                        /* 等待数据发送 */
		R8_UART1_THR = *buf++;                                               /* 发送数据 */
#elif DEBUG == Debug_UART2       
		while( R8_UART2_TFC == UART_FIFO_SIZE );                        /* 等待数据发送 */
		R8_UART2_THR = *buf++;                                               /* 发送数据 */
#elif DEBUG == Debug_UART3       
		while( R8_UART3_TFC == UART_FIFO_SIZE );                        /* 等待数据发送 */
		R8_UART3_THR = *buf++;                                               /* 发送数据 */
#endif
	}
	return size;
}

#endif

