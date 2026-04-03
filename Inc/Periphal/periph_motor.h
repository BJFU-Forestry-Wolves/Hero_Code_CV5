/*
 *  Project      : Polaris Robot 
 *  
 *  FilePath     : periph_motor.h
 *  Description  : This file contains motor control function
 *  LastEditors  : Polaris
 *  Date         : 2023-01-23 00:44:03
 *  LastEditTime : 2023-05-05 10:41:51
 */
#ifndef MOTOR_PERIPH_H
#define MOTOR_PERIPH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "util_can.h"
#include "util_uart.h"
#include "alg_math.h"
#include "alg_filter.h"


#define MOTOR_GROUP_NUM 4

typedef enum {      
    Motor_TYPE_NOT_CONNECTED    = 0,
    Motor_TYPE_RM3508           = 1,
    Motor_TYPE_RM2006           = 2,
    Motor_TYPE_RM6020           = 3,
} Motor_MotorTypeEnum;  // Motor drive type

// 定义一个结构体 Motor_EncoderTypeDef，用于保存电机编码器的反馈信息
typedef struct {
    // 电机反馈
    float angle;              // 当前角度
    float speed;              // 当前速度
    float current;            // 当前电流
    float temp;               // 温度（单位：℃）
    uint8_t error_code;       // 错误代码

    // 用于计算连续角度
    float last_angle;         // 上一个角度
    int32_t round_count;      // 旋转圈数
    float init_offset;        // 初始偏移量
    float limited_angle;      // 限制角度
    float consequent_angle;    // 结果角度
} Motor_EncoderTypeDef;

typedef struct _motor_type {
    Motor_EncoderTypeDef encoder;
    Motor_MotorTypeEnum type;

    float output;
    uint32_t id;
    uint8_t is_online;
    uint32_t last_update_time;
    uint8_t init;
    void (*callback)(struct _motor_type*, uint8_t rxbuff[], uint32_t len);
    Filter_LowPassParamTypeDef fdb_fil_param;
    Filter_LowPassTypeDef fdb_fil;
} Motor_MotorTypeDef;

typedef struct {
    uint8_t motor_num;
    Motor_MotorTypeDef* motor_handle[4];
    Motor_MotorTypeEnum type;
    CAN_HandleTypeDef* can_handle;
    CAN_TxHeaderTypeDef can_header;
} Motor_MotorGroupTypeDef;

typedef void (*Motor_EncoderCallbackFuncTypeDef)(Motor_MotorTypeDef*, uint8_t[], uint32_t);

extern Motor_MotorGroupTypeDef Motor_ChassisMotors;
extern Motor_MotorGroupTypeDef Motor_PitchMotors;        
extern Motor_MotorGroupTypeDef Motor_ShootMotors;
//extern Motor_MotorGroupTypeDef Motor_FeedMotors;

extern Motor_MotorTypeDef Motor_ChassisFontRightMotor;
extern Motor_MotorTypeDef Motor_ChassisFontLeftMotor;
extern Motor_MotorTypeDef Motor_ChassisBackLeftMotor;
extern Motor_MotorTypeDef Motor_ChassisBackRightMotor;
extern Motor_MotorTypeDef Motor_PitchMotor;
extern Motor_MotorTypeDef Motor_YawMotor;            //yaw轴
extern Motor_MotorTypeDef Motor_ShootLeftMotor;
extern Motor_MotorTypeDef Motor_ShootRightMotor;
extern Motor_MotorTypeDef Motor_FeedMotor;

/********** VOLATILE USER CODE END **********/


void Motor_EncoderDecodeCallback(CAN_HandleTypeDef* phcan, uint32_t stdid, uint8_t rxdata[], uint32_t len);
void Motor_InitAllMotors(void);
void Motor_InitMotor(Motor_MotorTypeDef* pmotor, Motor_MotorTypeEnum type, uint16_t id, float fil_param, 
                     Motor_EncoderCallbackFuncTypeDef callback);
void Motor_InitMotorGroup(Motor_MotorGroupTypeDef* pgroup, Motor_MotorTypeEnum type, uint8_t motor_num, CAN_HandleTypeDef* phcan, uint16_t stdid);
void Motor_SetMotorOutput(Motor_MotorTypeDef* pmotor, float output);
void Motor_SetMotorOutputPowerLimit(Motor_MotorTypeDef* pmotor, float output);
uint16_t Motor_GetMotorOutput(Motor_MotorTypeDef* pmotor);
void Motor_SendMotorGroupOutput(Motor_MotorGroupTypeDef *pgroup);
void Motor_SetMotorOutputPowerLimit(Motor_MotorTypeDef* pmotor, float output);
void Motor_feed_Shoot_MotorGroupOutput(Motor_MotorGroupTypeDef *pgroup);
void Motor_SendMotorGroupsOutput(void);
uint8_t Motor_IsAnyMotorOffline(void);
uint8_t Motor_IsMotorOffline(Motor_MotorTypeDef* pmotor);
void rm2006_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);
void gm6020_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);
void rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);

void Hero_FeedMotor_rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);
void rm3508_shooter_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len);
	
#endif

#ifdef __cplusplus
}
#endif
