


#ifndef __CH57x_CLK_H__
#define __CH57x_CLK_H__

#ifdef __cplusplus
 extern "C" {
#endif


typedef enum
{
  CLK_SOURCE_LSI = 0x00,
  CLK_SOURCE_LSE,

  CLK_SOURCE_HSE_16MHz = 0x22,
  CLK_SOURCE_HSE_8MHz = 0x24,
  CLK_SOURCE_HSE_6_4MHz = 0x25,
  CLK_SOURCE_HSE_4MHz = 0x28,
  CLK_SOURCE_HSE_2MHz = (0x20|16),
  CLK_SOURCE_HSE_1MHz = (0x20|0),

  CLK_SOURCE_PLL_60MHz = 0x48,
  CLK_SOURCE_PLL_48MHz = (0x40|10),
  CLK_SOURCE_PLL_40MHz = (0x40|12),
  CLK_SOURCE_PLL_36_9MHz = (0x40|13),
  CLK_SOURCE_PLL_32MHz = (0x40|15),
  CLK_SOURCE_PLL_30MHz = (0x40|16),
  CLK_SOURCE_PLL_24MHz = (0x40|20),
  CLK_SOURCE_PLL_20MHz = (0x40|24),
  CLK_SOURCE_PLL_15MHz = (0x40|0),
}SYS_CLKTypeDef;

typedef enum
{
	Clk32K_LSI = 0,
	Clk32K_LSE,
	
}LClk32KTypeDef;

typedef enum
{
	HSE_RCur_75 = 0,
	HSE_RCur_100,
    HSE_RCur_125,
    HSE_RCur_150
	
}HSECurrentTypeDef;

typedef enum
{
	HSECap_10p = 0,
	HSECap_12p,  HSECap_14p,  HSECap_16p,  HSECap_18p,  
    HSECap_20p,  HSECap_22p,  HSECap_24p
	
}HSECapTypeDef;

typedef enum
{
	LSE_RCur_70 = 0,
	LSE_RCur_100,
    LSE_RCur_140,
    LSE_RCur_200
	
}LSECurrentTypeDef;

typedef enum
{
	LSECap_2p = 0,
	LSECap_13p,  LSECap_14p,  LSECap_15p,  LSECap_16p,  
    LSECap_17p,  LSECap_18p,  LSECap_19p,  LSECap_20p,
    LSECap_21p,  LSECap_22p,  LSECap_23p,  LSECap_24p,
    LSECap_25p,  LSECap_26p,  LSECap_27p
	
}LSECapTypeDef;

#define  MAX_DAY		0x00004000 
#define	 MAX_2_SEC		0x0000A8C0
//#define	 MAX_SEC		0x545FFFFF	


/**
  * @brief  rtc timer mode period define
  */
typedef enum
{
	Period_0_125_S = 0,			// 0.125s ����
	Period_0_25_S,				// 0.25s ����
	Period_0_5_S,				// 0.5s ����
	Period_1_S,					// 1s ����
	Period_2_S,					// 2s ����
	Period_4_S,					// 4s ����
	Period_8_S,					// 8s ����
	Period_16_S,				// 16s ����
}RTC_TMRCycTypeDef;	 
	 

/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	RTC_TRIG_EVENT = 0,			// RTC �����¼�
	RTC_TMR_EVENT,				// RTC ���ڶ�ʱ�¼�

}RTC_EVENTTypeDef;	 

/**
  * @brief  rtc interrupt event define
  */
typedef enum
{
	RTC_TRIG_MODE = 0,			// RTC ����ģʽ
	RTC_TMR_MODE,				// RTC ���ڶ�ʱģʽ

}RTC_MODETypeDef;

UINT16 Int32K_Tune_FLASH;
UINT16 Int32K_Tune_RAM;

void LClk32K_Select( LClk32KTypeDef hc);		/* 32K ��Ƶʱ����Դ */

void HSECFG_Current( HSECurrentTypeDef c );     /* HSE���� ƫ�õ������� */
void HSECFG_Capacitance( HSECapTypeDef c );     /* HSE���� ���ص������� */
void LSECFG_Current( LSECurrentTypeDef c );     /* LSE���� ƫ�õ������� */
void LSECFG_Capacitance( LSECapTypeDef c );     /* LSE���� ���ص������� */

UINT16 Calibration_LSI_FLASH( void );				/* �ⲿ32Mʱ��У׼FLASH����ʱ�ڲ�32Kʱ�� */
UINT16 Calibration_LSI_RAM( void );         /* �ⲿ32Mʱ��У׼RAM����ʱ�ڲ�32Kʱ�� */
void LSI_SetTune_FLASH( void );           /* ������FLASH������ʱ���ڲ�32kУ׼ֵ */
void LSI_SetTune_RAM( void );             /* ������RAM������ʱ���ڲ�32kУ׼ֵ */

	 
void RTC_InitTime( UINT32 h, UINT16 m, UINT16 s );			/* RTCʱ�ӳ�ʼ����ǰʱ�� */
void RTC_GetTime( PUINT32 ph, PUINT16 pm, PUINT16 ps );		/* ��ȡ��ǰʱ�� */
	 
void RTC_SetCycle32k( UINT32 cyc );							/* ����LSE/LSIʱ�ӣ����õ�ǰRTC ������ */	 
UINT32 RTC_GetCycle32k( void );				                /* ����LSE/LSIʱ�ӣ���ȡ��ǰRTC ������ */

void RTC_TRIGFunCfg( UINT32 cyc );							/* RTC����ģʽ���ü��ʱ��,����LSE/LSIʱ�ӣ�ƥ�������� */
void RTC_TMRFunCfg( RTC_TMRCycTypeDef t );					/* RTC��ʱģʽ���� */
void RTC_ModeFunDisable( RTC_MODETypeDef m );               /* RTC ģʽ���ܹر� */

UINT8 RTC_GetITFlag( RTC_EVENTTypeDef f );					/* ��ȡRTC�жϱ�־ */	 
void RTC_ClearITFlag( RTC_EVENTTypeDef f );					/* ���RTC�жϱ�־ */ 

	 

	 
#ifdef __cplusplus
}
#endif

#endif  // __CH57x_CLK_H__	
