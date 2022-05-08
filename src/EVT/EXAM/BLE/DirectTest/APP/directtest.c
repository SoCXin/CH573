/********************************** (C) COPYRIGHT *******************************
 * File Name          : directtest.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2020/08/06
 * Description        : ֱ�Ӳ��Գ��򣬲���ָ��ͨ��Ƶ���������ݰ�
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

/******************************************************************************/
/* ͷ�ļ����� */
#include "CONFIG.h"
#include "HAL.h"
#include "directtest.h"

static tmosTaskID testTaskID;
#if BLE_DIRECT_TEST
static uint8_t TestEnalbe = FALSE;
#endif

static uint8_t payload = 0;

/*********************************************************************
 * @fn      TEST_ProcessEvent
 *
 * @brief   ������
 *
 * @param   task_id  - The TMOS assigned task ID.
 * @param   events - events to process.  This is a bit map and can
 *                   contain more than one event.
 *
 * @return  events not processed
 */
tmosEvents TEST_ProcessEvent(tmosTaskID task_id, tmosEvents events)
{
    uint8_t     *msgPtr;
    keyChange_t *msgKeyPtr;

    if(events & SYS_EVENT_MSG)
    { // ����HAL����Ϣ������tmos_msg_receive��ȡ��Ϣ��������ɺ�ɾ����Ϣ��
        msgPtr = tmos_msg_receive(task_id);
        if(msgPtr)
        {
            /* De-allocate */
            tmos_msg_deallocate(msgPtr);
        }
        return events ^ SYS_EVENT_MSG;
    }
    if(events & TEST_EVENT)
    {
        if(TestEnalbe == FALSE)
        {
            payload++;
            TestEnalbe = TRUE;
            HalLedBlink(HAL_LED_1, 0xff, 30, 4000);
            API_LE_TransmitterTestCmd(0, 20, payload & 7, 0x15 | 0x80);
            tmos_start_task(testTaskID, TEST_EVENT, MS1_TO_SYSTEM_TIME(20 * 1000)); // ����ʱ��20s
            PRINT("test start ...\n");
        }
        else
        {
            TestEnalbe = FALSE;
            HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
            API_LE_TestEndCmd();
            PRINT("   end!\n");
        }
        return events ^ TEST_EVENT;
    }
    return 0;
}

/*********************************************************************
 * @fn      key_Change
 *
 * @brief   �����ص�
 *
 * @param   keys    - ��������.
 *
 * @return  none
 */
void key_Change(uint8_t keys)
{
    if(keys & HAL_KEY_SW_1)
    {
        if(TestEnalbe == FALSE)
        {
            payload++;
            TestEnalbe = TRUE;
            HalLedBlink(HAL_LED_1, 0xff, 30, 4000);
            API_LE_TransmitterTestCmd(0, 20, payload & 7, 0x15 | 0x80);
            PRINT("(key)test start ...\n");
        }
        else
        {
            TestEnalbe = FALSE;
            HalLedSet(HAL_LED_1, HAL_LED_MODE_OFF);
            API_LE_TestEndCmd();
            PRINT("   (key)end!\n");
        }
        tmos_stop_task(testTaskID, TEST_EVENT);
    }
}

/*********************************************************************
 * @fn      TEST_Init
 *
 * @brief   ��ʼ��
 *
 * @return  none
 */
void TEST_Init(void)
{
    testTaskID = TMOS_ProcessEventRegister(TEST_ProcessEvent);
#if(defined HAL_KEY) && (HAL_KEY == TRUE)
    HalKeyConfig(key_Change);
#endif
#if BLE_DIRECT_TEST
    tmos_start_task(testTaskID, TEST_EVENT, 1000); // ���һ����������
#endif
}
/******************************** endfile @ mcu ******************************/
