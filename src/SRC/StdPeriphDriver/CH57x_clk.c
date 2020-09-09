/********************************** (C) COPYRIGHT *******************************
* File Name          : CH57x_clk.c
* Author             : WCH
* Version            : V1.0
* Date               : 2018/12/15
* Description 
*******************************************************************************/

#include "CH57x_common.h"

/*******************************************************************************
* Function Name  : SetSysClock
* Description    : ����ϵͳ����ʱ��
* Input          : sc: ϵͳʱ��Դѡ��
					refer to SYS_CLKTypeDef
* Return         : None
*******************************************************************************/
void SetSysClock( SYS_CLKTypeDef sc)
{
  UINT32 i;
  if ( sc & 0x20 ){    // HSE div
    if ( !( R8_HFCK_PWR_CTRL & RB_CLK_XT32M_PON ) ){
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
      R8_HFCK_PWR_CTRL |= RB_CLK_XT32M_PON;    // HSE power on
      for(i=0;i<1200;i++){  __nop();__nop();  }
    }

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
      R16_CLK_SYS_CFG = ( 0 << 6 ) | ( sc & 0x1f );
      __nop();__nop();__nop();__nop();
      R8_SAFE_ACCESS_SIG = 0;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
      R8_FLASH_CFG = 0X01;
      R8_SAFE_ACCESS_SIG = 0;
  }

  else if ( sc & 0x40 ){    // PLL div
    if ( !( R8_HFCK_PWR_CTRL & RB_CLK_PLL_PON ) ){
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
        R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
        R8_HFCK_PWR_CTRL |= RB_CLK_PLL_PON;    // PLL power on
        for(i=0;i<2000;i++){  __nop();__nop();  }
    }

    if((0<(sc&0x1f))&&((sc&0x1f)<=24)){
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_CLK_SYS_CFG = ( 1 << 6 ) | ( sc & 0x1f );
    __nop();__nop();__nop();__nop();
    R8_SAFE_ACCESS_SIG = 0;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_FLASH_CFG = 0X02;
    R8_SAFE_ACCESS_SIG = 0;
    }
    else{
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
      R16_CLK_SYS_CFG = ( 1 << 6 ) | ( sc & 0x1f );
      __nop();__nop();__nop();__nop();
      R8_SAFE_ACCESS_SIG = 0;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
      R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
      R8_FLASH_CFG = 0X03;
      R8_SAFE_ACCESS_SIG = 0;
      }
  }
}

/*******************************************************************************
* Function Name  : GetSysClock
* Description    : ��ȡ��ǰϵͳʱ��
* Input          : None
* Return         : Hz
*******************************************************************************/
UINT32 GetSysClock( void )
{
	UINT16  rev;
	
	rev = R16_CLK_SYS_CFG & 0xff;		
	if( (rev & 0x40) == (0<<6) ){				// 32M���з�Ƶ
	    return (32000000/(rev&0x1f));
	}
	else if( (rev & RB_CLK_SYS_MOD) == (1<<6) ){		// PLL���з�Ƶ
	    return (480000000/(rev&0x1f));		
	}
	else {												// 32K����Ƶ
		return (32000);
	}	
}

/*******************************************************************************
* Function Name  : LClk32K_Select
* Description    : 32K ��Ƶʱ����Դ
* Input          : hc: 
					Clk32K_LSI   -   ѡ���ڲ�32K
					Clk32K_LSE   -   ѡ���ⲿ32K
* Return         : None
*******************************************************************************/
void LClk32K_Select( LClk32KTypeDef hc)
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;	
    if( hc == Clk32K_LSI)
        R8_CK32K_CONFIG &= ~RB_CLK_OSC32K_XT;
    else
        R8_CK32K_CONFIG |= RB_CLK_OSC32K_XT;
    R8_SAFE_ACCESS_SIG = 0;
}


/*******************************************************************************
* Function Name  : HSECFG_Current
* Description    : HSE���� ƫ�õ�������
* Input          : c: 75%,100%,125%,150%
* Return         : None
*******************************************************************************/
void HSECFG_Current( HSECurrentTypeDef c )
{
    UINT8  x32M_c;
    
    x32M_c = R8_XT32M_TUNE;
    x32M_c = (x32M_c&0xfc)|(c&0x03);
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;	
    R8_XT32M_TUNE = x32M_c;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : HSECFG_Capacitance
* Description    : HSE���� ���ص�������
* Input          : c: refer to HSECapTypeDef
* Return         : None
*******************************************************************************/
void HSECFG_Capacitance( HSECapTypeDef c )
{
    UINT8  x32M_c;
    
    x32M_c = R8_XT32M_TUNE;
    x32M_c = (x32M_c&0x8f)|(c<<4);
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;	
    R8_XT32M_TUNE = x32M_c;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : LSECFG_Current
* Description    : LSE���� ƫ�õ�������
* Input          : c: 70%,100%,140%,200%
* Return         : None
*******************************************************************************/
void LSECFG_Current( LSECurrentTypeDef c )
{
    UINT8  x32K_c;
    
    x32K_c = R8_XT32K_TUNE;
    x32K_c = (x32K_c&0xfc)|(c&0x03);
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;	
    R8_XT32K_TUNE = x32K_c;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : LSECFG_Capacitance
* Description    : LSE���� ���ص�������
* Input          : c: refer to LSECapTypeDef
* Return         : None
*******************************************************************************/
void LSECFG_Capacitance( LSECapTypeDef c )
{
    UINT8  x32K_c;
    
    x32K_c = R8_XT32K_TUNE;
    x32K_c = (x32K_c&0x0f)|(c<<4);
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;	
    R8_XT32K_TUNE = x32K_c;
    R8_SAFE_ACCESS_SIG = 0;
}
/*******************************************************************************
* Function Name  : Calibration_LSI
* Description    : У׼�ڲ�32Kʱ��
* Input          : None
* Return         : �����֮����λ��
*******************************************************************************/
UINT16 Calibration_LSI( void )
{
  UINT16 rev, basev;
  UINT32 calv;
  UINT16 i;
  UINT16 loc, loc_t;
  float CNT_STEP_K;
  signed short diff_1, diff_2, diffc;
  UINT8 k = 0;

  /* ���ݵ�ǰʱ�ӻ�ȡ���ֵ��б�ʣ�T-step�� */
  rev = R16_CLK_SYS_CFG & 0xff;
  if ( ( rev & RB_CLK_SYS_MOD ) == ( 2 << 6 ) )
  {       // 32M����Ƶ
    calv = ( ( 5 * 32000000 + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -1.6;
  }
  else if ( ( rev & RB_CLK_SYS_MOD ) == ( 1 << 6 ) )
  {   // PLL���з�Ƶ
    calv = ( ( ( UINT32 ) 5 * 480000000 / ( rev & 0x1f ) + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -0.1 * ( rev & 0x1f );
  }
  else if ( ( rev & RB_CLK_SYS_MOD ) == ( 0 << 6 ) )
  {   // 32M���з�Ƶ
    calv = ( ( 5 * 32000000 / ( rev & 0x1f ) + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -1.6 * ( rev & 0x1f );
  }
  else
  {                       // 32K����Ƶ
    calv = ( 5 );
    CNT_STEP_K = 0;
  }
  basev = calv;             // ��ȡУ׼���ֵ

  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  loc = R16_INT32K_TUNE;
  R8_SAFE_ACCESS_SIG = 0;
//  if (loc == 2048)
//    loc = 1837; //10040.625-1.640625*5000;  //��������

  diff_2 = 0;
  diffc = 0;

  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  R8_OSC_CAL_CTRL = RB_OSC_CNT_EN;
  do
  {
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R16_INT32K_TUNE = loc;
    R8_SAFE_ACCESS_SIG = 0;

    /* ��ȡ��ǰֵ */
    while( !( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT ) )
      ;
    i = R16_OSC_CAL_CNT;      // ���ڶ���
    while( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT )
      ;
    while( !( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT ) )
      ;
    i = R16_OSC_CAL_CNT;      // ʵʱУ׼�����ֵ
    k++;
    diff_1 = i - basev;

    if ( diff_1 == 0 )
    {
      return 0;    // У׼����
    }
    else if ( ( diff_1 * diff_2 ) < 0 )
    {         // ��������֮��
      if ( ( diffc == 1 ) || ( diffc == -1 ) || ( diffc == 0 ) )
      {
        // ���������
        if ( diff_2 < 0 )
          diff_2 = ~( diff_2 - 1 );
        else
          diff_1 = ~( diff_1 - 1 );

        if ( diff_1 > diff_2 )
        {
          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
          R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
          R16_INT32K_TUNE = loc_t;
          R8_SAFE_ACCESS_SIG = 0;

          return ( diff_2 * 10000 / basev );       // �������ֵ�����֮
        }
        else
          return ( diff_1 * 10000 / basev );
      }
    }

    if (((i > basev ? (i - basev) : (basev - i)) * 10000 / basev) < 11)
    {
      return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);

    }
    // ������һ��ֵ
    diff_2 = diff_1;
    loc_t = loc;
    diffc = diff_1 *CNT_STEP_K;
    loc = loc - diffc;
    if ( loc == loc_t )
    {
      if ( diff_1 > 0 )
        loc = loc + 1;    // ��ǰƵ��ƫС
      else
        loc = loc - 1;    // ��ǰƵ��ƫ��
    }
  } while( k < 20 );


  return ( (i>basev?(i-basev):(basev-i))* 10000 / basev );
}


/*******************************************************************************
* Function Name  : RTCInitTime
* Description    : RTCʱ�ӳ�ʼ����ǰʱ��
* Input          : h: ����ʱ�� - Сʱ
					MAX_H = 393192
				   m: ����ʱ�� - ����
					MAX_M = 59
				   s: ����ʱ�� - ��
				    MAX_S = 59
* Return         : None
*******************************************************************************/
void RTC_InitTime( UINT32 h, UINT16 m, UINT16 s )
{
    UINT32  t;
    UINT16  day, sec2, t32k;

    day = h/24;
    sec2 = (h%24)*1800+m*30+s/2;
    t32k = (s&1)?(0x8000):(0);
    t = sec2;
    t = t<<16 | t32k;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		// ���밲ȫģʽ
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;		
    R32_RTC_TRIG = day;
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
    R32_RTC_TRIG = t;	
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : ��ȡ��ǰʱ��
* Input          : ph: ��ȡ����ʱ�� - Сʱ
					MAX_H = 393192
				   pm: ��ȡ����ʱ�� - ����
					MAX_M = 59
				   ps: ��ȡ����ʱ�� - ��
				    MAX_S = 59
* Return         : None
*******************************************************************************/
void RTC_GetTime( PUINT32 ph, PUINT16 pm, PUINT16 ps )
{
    UINT32  t;
    UINT16  day, sec2, t32k;

    day = R32_RTC_CNT_DAY & 0x3FFF;
    sec2 = R16_RTC_CNT_2S; 
    t32k = R16_RTC_CNT_32K;

    t = sec2*2 + ((t32k<0x8000)?0:1);		// 
    *ph = day*24 + t/3600;
    *pm = t%3600/60;
    *ps = t%60;
}

/*******************************************************************************
* Function Name  : RTC_SetCycle32k
* Description    : ����LSE/LSIʱ�ӣ����õ�ǰRTC ������
* Input          : cyc: �������ڼ�����ֵ - cycle
					MAX_CYC = 0xA8BFFFFF = 2831155199
* Return         : None
*******************************************************************************/
void RTC_SetCycle32k( UINT32 cyc )
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;   
    R32_RTC_TRIG = cyc;
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_GetCycle32k
* Description    : ����LSE/LSIʱ�ӣ���ȡ��ǰRTC ������
* Input          : None
* Return         : ���ص�ǰ��������MAX_CYC = 0xA8BFFFFF = 2831155199
*******************************************************************************/
UINT32 RTC_GetCycle32k( void )
{
    UINT32 i;
    
    do{
    	i = R32_RTC_CNT_32K;
    }while( i != R32_RTC_CNT_32K );
    
    return (i);
}

/*******************************************************************************
* Function Name  : RTC_TMRFunCfg
* Description    : RTC��ʱģʽ����
* Input          : t: 
					refer to RTC_TMRCycTypeDef
* Return         : None
*******************************************************************************/
void RTC_TMRFunCfg( RTC_TMRCycTypeDef t )
{
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RTC_MODE_CTRL &= ~(RB_RTC_TMR_EN|RB_RTC_TMR_MODE);
    R8_RTC_MODE_CTRL |= RB_RTC_TMR_EN | (t);
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_TRIGFunCfg
* Description    : RTCʱ�䴥��ģʽ����
* Input          : cyc: ��Ե�ǰʱ��Ĵ������ʱ�䣬����LSE/LSIʱ��������
* Return         : None
*******************************************************************************/
void RTC_TRIGFunCfg( UINT32 cyc )
{
    UINT32 t;

    t = RTC_GetCycle32k() + cyc;

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R32_RTC_TRIG = t;
    R8_RTC_MODE_CTRL |= RB_RTC_TRIG_EN;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_ModeFunDisable
* Description    : RTC ģʽ���ܹر�
* Input          : m: ��Ҫ�رյĵ�ǰģʽ
* Return         : None
*******************************************************************************/
void RTC_ModeFunDisable( RTC_MODETypeDef m )
{
    UINT8  i=0;
    
    if( m == RTC_TRIG_MODE )    i |= RB_RTC_TRIG_EN;
    else if( m == RTC_TMR_MODE )     i |= RB_RTC_TMR_EN;
    
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
    R8_RTC_MODE_CTRL &= ~(i);
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_GetITFlag
* Description    : ��ȡRTC�жϱ�־
* Input          : f: 
					refer to RTC_EVENTTypeDef
* Return         : �жϱ�־״̬:
					0     -  	δ�����¼�
				   (!0)   -  	�����¼�
*******************************************************************************/
UINT8 RTC_GetITFlag( RTC_EVENTTypeDef f )
{
    if( f == RTC_TRIG_EVENT )
        return ( R8_RTC_FLAG_CTRL & RB_RTC_TRIG_FLAG );
    else 
        return ( R8_RTC_FLAG_CTRL & RB_RTC_TMR_FLAG );
}

/*******************************************************************************
* Function Name  : RTC_ClearITFlag
* Description    : ���RTC�жϱ�־
* Input          : f: 
					refer to RTC_EVENTTypeDef
* Return         : None
*******************************************************************************/
void RTC_ClearITFlag( RTC_EVENTTypeDef f )
{
    switch( f ) 
    {
        case RTC_TRIG_EVENT:
            R8_RTC_FLAG_CTRL = RB_RTC_TRIG_CLR;
            break;
        case RTC_TMR_EVENT:
            R8_RTC_FLAG_CTRL = RB_RTC_TMR_CLR;
            break;
        default :
            break;
    }
}


