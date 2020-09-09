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
* Description    : 配置系统运行时钟
* Input          : sc: 系统时钟源选择
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
* Description    : 获取当前系统时钟
* Input          : None
* Return         : Hz
*******************************************************************************/
UINT32 GetSysClock( void )
{
	UINT16  rev;
	
	rev = R16_CLK_SYS_CFG & 0xff;		
	if( (rev & 0x40) == (0<<6) ){				// 32M进行分频
	    return (32000000/(rev&0x1f));
	}
	else if( (rev & RB_CLK_SYS_MOD) == (1<<6) ){		// PLL进行分频
	    return (480000000/(rev&0x1f));		
	}
	else {												// 32K做主频
		return (32000);
	}	
}

/*******************************************************************************
* Function Name  : LClk32K_Select
* Description    : 32K 低频时钟来源
* Input          : hc: 
					Clk32K_LSI   -   选择内部32K
					Clk32K_LSE   -   选择外部32K
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
* Description    : HSE晶体 偏置电流配置
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
* Description    : HSE晶体 负载电容配置
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
* Description    : LSE晶体 偏置电流配置
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
* Description    : LSE晶体 负载电容配置
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
* Description    : 校准内部32K时钟
* Input          : None
* Return         : 误差：万分之（单位）
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

  /* 根据当前时钟获取标称值和斜率（T-step） */
  rev = R16_CLK_SYS_CFG & 0xff;
  if ( ( rev & RB_CLK_SYS_MOD ) == ( 2 << 6 ) )
  {       // 32M做主频
    calv = ( ( 5 * 32000000 + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -1.6;
  }
  else if ( ( rev & RB_CLK_SYS_MOD ) == ( 1 << 6 ) )
  {   // PLL进行分频
    calv = ( ( ( UINT32 ) 5 * 480000000 / ( rev & 0x1f ) + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -0.1 * ( rev & 0x1f );
  }
  else if ( ( rev & RB_CLK_SYS_MOD ) == ( 0 << 6 ) )
  {   // 32M进行分频
    calv = ( ( 5 * 32000000 / ( rev & 0x1f ) + ( CAB_LSIFQ >> 1 ) ) / CAB_LSIFQ );
    CNT_STEP_K = -1.6 * ( rev & 0x1f );
  }
  else
  {                       // 32K做主频
    calv = ( 5 );
    CNT_STEP_K = 0;
  }
  basev = calv;             // 获取校准标称值

  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;
  R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;
  loc = R16_INT32K_TUNE;
  R8_SAFE_ACCESS_SIG = 0;
//  if (loc == 2048)
//    loc = 1837; //10040.625-1.640625*5000;  //经验曲线

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

    /* 读取当前值 */
    while( !( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT ) )
      ;
    i = R16_OSC_CAL_CNT;      // 用于丢弃
    while( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT )
      ;
    while( !( R8_OSC_CAL_CTRL & RB_OSC_CNT_HALT ) )
      ;
    i = R16_OSC_CAL_CNT;      // 实时校准后采样值
    k++;
    diff_1 = i - basev;

    if ( diff_1 == 0 )
    {
      return 0;    // 校准正好
    }
    else if ( ( diff_1 * diff_2 ) < 0 )
    {         // 处于两点之间
      if ( ( diffc == 1 ) || ( diffc == -1 ) || ( diffc == 0 ) )
      {
        // 都变成正数
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

          return ( diff_2 * 10000 / basev );       // 返回误差值，万分之
        }
        else
          return ( diff_1 * 10000 / basev );
      }
    }

    if (((i > basev ? (i - basev) : (basev - i)) * 10000 / basev) < 11)
    {
      return ((i > basev ? (i - basev) : (basev - i)) * 10000 / basev);

    }
    // 保存上一次值
    diff_2 = diff_1;
    loc_t = loc;
    diffc = diff_1 *CNT_STEP_K;
    loc = loc - diffc;
    if ( loc == loc_t )
    {
      if ( diff_1 > 0 )
        loc = loc + 1;    // 当前频率偏小
      else
        loc = loc - 1;    // 当前频率偏大
    }
  } while( k < 20 );


  return ( (i>basev?(i-basev):(basev-i))* 10000 / basev );
}


/*******************************************************************************
* Function Name  : RTCInitTime
* Description    : RTC时钟初始化当前时间
* Input          : h: 配置时间 - 小时
					MAX_H = 393192
				   m: 配置时间 - 分钟
					MAX_M = 59
				   s: 配置时间 - 秒
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

    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG1;		// 进入安全模式
    R8_SAFE_ACCESS_SIG = SAFE_ACCESS_SIG2;		
    R32_RTC_TRIG = day;
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_HI;
    R32_RTC_TRIG = t;	
    R8_RTC_MODE_CTRL |= RB_RTC_LOAD_LO;
    R8_SAFE_ACCESS_SIG = 0;
}

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : 获取当前时间
* Input          : ph: 获取到的时间 - 小时
					MAX_H = 393192
				   pm: 获取到的时间 - 分钟
					MAX_M = 59
				   ps: 获取到的时间 - 秒
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
* Description    : 基于LSE/LSI时钟，配置当前RTC 周期数
* Input          : cyc: 配置周期计数初值 - cycle
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
* Description    : 基于LSE/LSI时钟，获取当前RTC 周期数
* Input          : None
* Return         : 返回当前周期数，MAX_CYC = 0xA8BFFFFF = 2831155199
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
* Description    : RTC定时模式配置
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
* Description    : RTC时间触发模式配置
* Input          : cyc: 相对当前时间的触发间隔时间，基于LSE/LSI时钟周期数
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
* Description    : RTC 模式功能关闭
* Input          : m: 需要关闭的当前模式
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
* Description    : 获取RTC中断标志
* Input          : f: 
					refer to RTC_EVENTTypeDef
* Return         : 中断标志状态:
					0     -  	未发生事件
				   (!0)   -  	发生事件
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
* Description    : 清除RTC中断标志
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


