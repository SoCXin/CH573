/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.1
 * Date               : 2019/11/05
 * Description        : 外设从机应用主函数及任务系统初始化
 *******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CONFIG.h"
#include "CH57x_common.h"
#include "HAL.h"
#include "GATTprofile.h"
#include "Peripheral.h"
#include "OTA.h"
#include "OTAprofile.h"

/* 记录当前的Image */
unsigned char CurrImageFlag = 0xff;

extern uint8 block_buf[4];
/*********************************************************************
 * GLOBAL TYPEDEFS
 */
__attribute__((aligned(4)))  u32 MEM_BUF[BLE_MEMHEAP_SIZE / 4];

#if (defined (BLE_MAC)) && (BLE_MAC == TRUE)
u8C MacAddr[6] = { 0x84, 0xC2, 0xE4, 0x03, 0x02, 0x02 };
#endif

/* 注意：关于程序升级后flash的操作必须先执行，不开启任何中断，防止操作中断和失败 */
/*******************************************************************************
 * Function Name  : ReadImageFlag
 * Description    : 读取当前的程序的Image标志，DataFlash如果为空，就默认是ImageA
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void ReadImageFlag( void )
{
  OTADataFlashInfo_t p_image_flash;

  EEPROM_READ( OTA_DATAFLASH_ADD, &p_image_flash, 4 );
  CurrImageFlag = p_image_flash.ImageFlag;

  /* 程序第一次执行，或者没有更新过，以后更新后在擦除DataFlash */
  if ( CurrImageFlag != IMAGE_A_FLAG && CurrImageFlag != IMAGE_B_FLAG )
  {
    CurrImageFlag = IMAGE_A_FLAG;
  }

  PRINT( "Image Flag %02x\n", CurrImageFlag );
}

/*******************************************************************************
 * Function Name  : ImageJumpCopy
 * Description    : 执行跳转指令的复制，仅当ImageA升级ImageB时执行
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void ImageJumpCopy( void )
{
  if ( ( CurrImageFlag == IMAGE_B_FLAG ) && ( (uint32)ImageJumpCopy < IMAGE_B_START_ADD ))
  {
    /* 擦除ImageA代码的第一块 */
    FLASH_ROM_ERASE( IMAGE_A_START_ADD, FLASH_BLOCK_SIZE );

    block_buf[0] = 0x6F;
    block_buf[1] = (IMAGE_SIZE>>(15-7)&(0xe0));
    block_buf[2] = (IMAGE_SIZE<<(7-3)&(0xe0)) | (IMAGE_SIZE>>(11-4)&(0x10)) | (IMAGE_SIZE>>(19-3)&(0x0f));
    block_buf[3] = ((IMAGE_SIZE>>(20-7))&(0x80)) | ((IMAGE_SIZE>>(10-6))&(0x3f));

    /* 将ImageB跳转代码写入第一块 */
    FLASH_ROM_WRITE( IMAGE_A_START_ADD, ( PUINT32 ) &block_buf[0], FLASH_BLOCK_SIZE );

    PRINT( "SYS_Reset\n" );
    mDelaymS(2);
    SYS_ResetExecute();
  }
}

/*******************************************************************************
 * Function Name  : main
 * Description    : 主函数
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
int main( void )
{
  SetSysClock( CLK_SOURCE_PLL_32MHz );
#ifdef DEBUG
  GPIOA_SetBits( bTXD1 );
  GPIOA_ModeCfg( bTXD1, GPIO_ModeOut_PP_5mA );
  UART1_DefInit();
#endif   
  PRINT( "%s\n", VER_LIB );
  ReadImageFlag();
  ImageJumpCopy();
  CH57X_BLEInit();
  HAL_Init();
  GAPRole_PeripheralInit();
  Peripheral_Init();
  while( 1 )
  {
    TMOS_SystemProcess();
  }
}

/******************************** endfile @ main ******************************/
