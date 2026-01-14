/*
 *  Project      : Polaris Robot 
 *  
 *  FilePath     : protocol_common.h
 *  Description  : This file contains Bus communication control function
 *  LastEditors  : Polaris
 *  Date         : 2023-01-23 03:03:07
 *  LastEditTime : 2023-05-05 14:43:21
 */


#ifndef PROTOCOL_COMMON_H
#define PROTOCOL_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif 

#include "stm32f4xx_hal.h"

typedef enum {
    Protocol_STATE_NULL      = 0,
    Protocol_STATE_CONNECTED = 1,
    Protocol_STATE_LOST      = 2,
    Protocol_STATE_ERROR     = 3,
    Protocol_STATE_PENDING   = 4
} Protocol_CommStateEnum;

typedef enum {
    Balance_STOP    = 0,
    Balance_STAND   = 1,
    Balance_RUN     = 3,
    Balance_FALL    = 4
} WheelLeg_BalanceModeEnum;

typedef enum {
    Cha_NULL        = 0,
    Cha_Gyro        = 1,
    Cha_Servo       = 2,
    Cha_Servo_90    = 3,
    Cha_Servo_270   = 4   
} WheelLeg_ChassisModeEnum;

typedef struct {
    Protocol_CommStateEnum state;           // 通信状态
    uint32_t last_update_time;              // 最后更新时间

    float leg_len;                          // 数据长度
    float dx;                               // 位移差值
    float yaw_ref;                          // 偏航角参考值

    uint8_t yaw_control;                    // yaw控制标志
    uint8_t yaw_output;                     // yaw输出标志
    WheelLeg_BalanceModeEnum bal_mode;      // 平衡模式
    WheelLeg_ChassisModeEnum cha_mode;      // 底盘模式

    WheelLeg_BalanceModeEnum bal_real_mode; // 实际平衡模式
} Protocol_DataTypeDef;



extern Protocol_DataTypeDef Protocol_Data;


void Protocol_InitProtocol(void);
Protocol_DataTypeDef* Protocol_GetBusDataPtr(void);
uint8_t Protocol_IsProtocolOffline(void);
void Protocol_SendBlockError(void);
void Protocol_ResetProtocolData(void);
void Protocol_SendProtocolData(void);
void Protocol_DecodeData(uint32_t stdid, uint8_t buff[], uint32_t rxdatalen);
void Protocol_SetChaMode(WheelLeg_ChassisModeEnum cha_mode);
void Protocol_SetChassisRef(float len, float dx, float yaw_ref);
void Protocol_SetYawState(uint8_t ctrl, uint8_t out);
void Protocol_SetBalMode(WheelLeg_BalanceModeEnum bal_mode);

#endif

#ifdef __cplusplus
}
#endif
