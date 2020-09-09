/********************************** (C) COPYRIGHT *******************************
 * File Name          : Main.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description 		    : ��ʱ������
 *******************************************************************************/

#include "CH57x_common.h"

__attribute__((aligned(4)))   UINT32 CapBuf[100];
UINT8V capFlag = 0;

void DebugInit( void )
{
  GPIOA_SetBits( GPIO_Pin_9 );
  GPIOA_ModeCfg( GPIO_Pin_8, GPIO_ModeIN_PU );
  GPIOA_ModeCfg( GPIO_Pin_9, GPIO_ModeOut_PP_5mA );
  UART1_DefInit();
}

int main()
{
  UINT8 i;

  SetSysClock( CLK_SOURCE_PLL_32MHz );

  /* ���ô��ڵ��� */
  DebugInit();
  PRINT( "Start @ChipID=%02X\n", R8_CHIP_ID );
  
#if 1       /* ��ʱ��0���趨100ms��ʱ������IO�����ƣ� PB15-LED */

  GPIOB_SetBits( GPIO_Pin_15 );
  GPIOB_ModeCfg( GPIO_Pin_15, GPIO_ModeOut_PP_5mA );

  TMR0_TimerInit( FREQ_SYS / 10 );                  // ���ö�ʱʱ�� 100ms
  TMR0_ITCfg( ENABLE, TMR0_3_IT_CYC_END );          // �����ж�
  PFIC_EnableIRQ( TMR0_IRQn );

#endif 

#if 1       /* ��ʱ��3��PWM��� */

  GPIOB_ResetBits( GPIO_Pin_22 );            // ����PWM�� PB22
  GPIOB_ModeCfg( GPIO_Pin_22, GPIO_ModeOut_PP_5mA );

  TMR3_PWMInit( High_Level, PWM_Times_1 );
  TMR3_PWMCycleCfg( 3200 );        // ���� 100ms
  TMR3_PWMActDataWidth( 1600 );              // ռ�ձ� 50%

#endif   

#if 1       /* ��ʱ��1��CAP��׽�� */
  PWR_UnitModCfg( DISABLE, UNIT_SYS_LSE );     // ע���������LSE�������ţ�Ҫ��֤�رղ���ʹ����������
  GPIOA_ResetBits( GPIO_Pin_10 );             // ����PWM�� PA10
  GPIOA_ModeCfg( GPIO_Pin_10, GPIO_ModeIN_PU );

  TMR1_CapInit( Edge_To_Edge );
  TMR1_CAPTimeoutCfg( 0xFFFFFFFF );    // ���ò�׽��ʱʱ��
  TMR1_DMACfg( ENABLE, ( UINT16 ) ( UINT32 ) &CapBuf[0], ( UINT16 ) ( UINT32 ) &CapBuf[100], Mode_Single );
  TMR1_ITCfg( ENABLE, TMR1_2_IT_DMA_END );          // ����DMA����ж�
  PFIC_EnableIRQ( TMR1_IRQn );

  while( capFlag == 0 )
    ;
  capFlag = 0;
  for ( i = 0; i < 100; i++ )
  {
    printf( "%08ld ", CapBuf[i] & 0x1ffffff );      // bit26 ���λ��ʾ �ߵ�ƽ���ǵ͵�ƽ
  }
  printf( "\n" );

#endif

  while( 1 )
    ;
}

__attribute__((interrupt("WCH-Interrupt-fast")))
void TMR0_IRQHandler( void )        // TMR0 ��ʱ�ж�
{
  if ( TMR0_GetITFlag( TMR0_3_IT_CYC_END ) )
  {
    TMR0_ClearITFlag( TMR0_3_IT_CYC_END );      // ����жϱ�־
    GPIOB_InverseBits( GPIO_Pin_15 );
  }
}

__attribute__((interrupt("WCH-Interrupt-fast")))
void TMR1_IRQHandler( void )        // TMR1 ��ʱ�ж�
{
  if ( TMR1_GetITFlag( TMR1_2_IT_DMA_END ) )
  {
    TMR1_ITCfg( DISABLE, TMR1_2_IT_DMA_END );       // ʹ�õ���DMA����+�жϣ�ע����ɺ�رմ��ж�ʹ�ܣ������һֱ�ϱ��жϡ�
    TMR1_ClearITFlag( TMR1_2_IT_DMA_END );      // ����жϱ�־
    capFlag = 1;
    printf( "*" );
  }
}

