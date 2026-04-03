/*
 *  Project      : Polaris Robot 
 *  
 *  FilePath     : periph_motor.c
 *  Description  : This file contains motor control function
 *  LastEditors  : Polaris
 *  Date         : 2023-01-23 00:43:48
 *  LastEditTime : 2023-05-05 17:59:23
 */


#include "periph_motor.h"
#include "sys_const.h"
#include "stdio.h"
#include "stdlib.h"

#include "alg_power_limit.h"
#include "module_supercap.h"
/********** VOLATILE USER CODE **********/

const uint32_t Const_Motor_MOTOR_OFFLINE_TIME = 200;
const uint32_t Const_Motor_MOTOR_TX_EXTID = 0x01;
const uint32_t Const_Motor_MOTOR_TX_DLC = 8;
const uint32_t Const_Motor_MOTOR_RX_DLC = 8;

Motor_MotorGroupTypeDef *Motor_groupHandle[MOTOR_GROUP_NUM];

Motor_MotorGroupTypeDef Motor_ChassisMotors;
Motor_MotorGroupTypeDef Motor_PitchMotors;        
Motor_MotorGroupTypeDef Motor_ShootMotors;
//Motor_MotorGroupTypeDef Motor_FeedMotors;

Motor_MotorTypeDef Motor_ChassisFontRightMotor;
Motor_MotorTypeDef Motor_ChassisFontLeftMotor;
Motor_MotorTypeDef Motor_ChassisBackLeftMotor;
Motor_MotorTypeDef Motor_ChassisBackRightMotor;
Motor_MotorTypeDef Motor_PitchMotor;
Motor_MotorTypeDef Motor_YawMotor;          //yaw
Motor_MotorTypeDef Motor_ShootLeftMotor;
Motor_MotorTypeDef Motor_ShootRightMotor;
Motor_MotorTypeDef Motor_FeedMotor;


/**
  * @brief      Motor encoder decoding callback function
  * @param      canid: CAN Handle number
  * @param      stdid: CAN identifier
  * @param      rxdata: CAN rx data buff
  * @retval     NULL
  */
void Motor_EncoderDecodeCallback(CAN_HandleTypeDef* phcan, uint32_t stdid, uint8_t rxdata[], uint32_t len) 
	{
		if (stdid == 0x211) {
        Parse_SuperCap_Feedback(rxdata,&SuperCap);
    }
		
    for (int i = 0; i < MOTOR_GROUP_NUM; i++) 
		{
        for (int j = 0; j < Motor_groupHandle[i]->motor_num; j++) 
			{
					if ((phcan == Motor_groupHandle[i]->can_handle) 
								&& (stdid == Motor_groupHandle[i]->motor_handle[j]->id) 
								&& phcan != NULL) 
						{
							Motor_groupHandle[i]->motor_handle[j]->callback(Motor_groupHandle[i]->motor_handle[j], rxdata, len);
						}
      }
    }
}


/********** VOLATILE USER CODE END **********/


/**
  * @brief      初始化全部电机
  * @param      NULL
  * @retval     NULL
  */
void Motor_InitAllMotors() {
//		Motor_groupHandle[0] = &Motor_FeedMotors;
//		Motor_InitMotorGroup(&Motor_FeedMotors, Motor_TYPE_RM3508, 1, &hcan1, 0x1FF);
//		Motor_InitMotor(&Motor_FeedMotor, Motor_TYPE_RM3508, 0X207, 0.1, Hero_FeedMotor_rm3508_encoder_callback);// 初始化供弹电机
//		Motor_PitchMotors.motor_handle[0] = &Motor_FeedMotor;				//	将拨弹电机句柄赋值给电机组
	
		// 初始化俯仰电机组，注意：英雄机械结构特殊，拨弹电机需要挂在CAN1上
    Motor_groupHandle[0] = &Motor_PitchMotors;
    Motor_InitMotorGroup(&Motor_PitchMotors, Motor_TYPE_RM6020, 3, &hcan1, 0x1FF); // 初始化电机组，包含两个电机，使用 CAN 总线 1，ID 为 0x1FF
		Motor_InitMotor(&Motor_YawMotor,Motor_TYPE_RM6020,0x205,0.1,gm6020_encoder_callback);// 初始化 Yaw 电机
    Motor_InitMotor(&Motor_PitchMotor, Motor_TYPE_RM6020, 0x206, 0.1, gm6020_encoder_callback);// 初始化 Pitch 电机
	  Motor_InitMotor(&Motor_FeedMotor, Motor_TYPE_RM3508, 0X207, 0.1, Hero_FeedMotor_rm3508_encoder_callback);// 初始化供弹电机
		Motor_PitchMotors.motor_handle[0] = &Motor_YawMotor;				// 	将 Yaw 电机句柄赋值给电机组
    Motor_PitchMotors.motor_handle[1] = &Motor_PitchMotor;			// 	将 Pitch 电机句柄赋值给电机组
    Motor_PitchMotors.motor_handle[2] = &Motor_FeedMotor;				//	将拨弹电机句柄赋值给电机组
	
    Motor_groupHandle[1] = &Motor_ShootMotors;
    Motor_InitMotorGroup(&Motor_ShootMotors, Motor_TYPE_RM3508, 2, &hcan2, 0x200);// 初始化电机组，包含两个电机，使用 CAN 总线 2，ID 为 0x200
    Motor_InitMotor(&Motor_ShootRightMotor, Motor_TYPE_RM3508, 0x201, 0.1, rm3508_encoder_callback);// 初始化右射击电机
    Motor_InitMotor(&Motor_ShootLeftMotor, Motor_TYPE_RM3508, 0x202, 0.1, rm3508_encoder_callback);// 初始化左射击电机
		Motor_ShootMotors.motor_handle[0] = &Motor_ShootRightMotor;
    Motor_ShootMotors.motor_handle[1] = &Motor_ShootLeftMotor;  
	
		Motor_groupHandle[2] = &Motor_ChassisMotors;
    Motor_InitMotorGroup(&Motor_ChassisMotors, Motor_TYPE_RM3508, 4, &hcan1, 0x200);
	  Motor_InitMotor(&Motor_ChassisFontRightMotor,Motor_TYPE_RM3508,0x201,0.1,rm3508_encoder_callback);
	  Motor_InitMotor(&Motor_ChassisFontLeftMotor,Motor_TYPE_RM3508,0x202,0.1,rm3508_encoder_callback);	 
	  Motor_InitMotor(&Motor_ChassisBackLeftMotor,Motor_TYPE_RM3508,0x203,0.1,rm3508_encoder_callback);
	  Motor_InitMotor(&Motor_ChassisBackRightMotor,Motor_TYPE_RM3508,0x204,0.1,rm3508_encoder_callback);	
	  Motor_ChassisMotors.motor_handle[0] = &Motor_ChassisFontRightMotor;
    Motor_ChassisMotors.motor_handle[1] = &Motor_ChassisFontLeftMotor;
    Motor_ChassisMotors.motor_handle[2] = &Motor_ChassisBackLeftMotor;
    Motor_ChassisMotors.motor_handle[3] = &Motor_ChassisBackRightMotor;

		
}


/**
  * @brief      Initialize the motor
  * @param      pmotor: Pointer to motor object
  * @param      type: Type of motor (pwm or can)
  * @param      callback: Motor callback function
  * @retval     NULL
  */
void Motor_InitMotor(Motor_MotorTypeDef* pmotor, Motor_MotorTypeEnum type, uint16_t id, float fil_param, 
                     Motor_EncoderCallbackFuncTypeDef callback) {
    if (pmotor == NULL) return;
    pmotor->last_update_time = 0;
    pmotor->type = type;
    pmotor->id = id;			
    pmotor->init = 0;			
    pmotor->is_online = 0;
    pmotor->output = 0;
    pmotor->callback = callback;
    Filter_LowPassInit(fil_param, &pmotor->fdb_fil_param);
}


/**
  * @brief      Initialization of motor group
  * @param      pgroup: Pointer to motor group
  * @param      type: Type of motor (pwm or can)
  * @param      motor_num: Number of motor group
  * @param      phcan: Pointer of can handle
  * @param      stdid: Motor id
  * @retval     NULL
  */
void Motor_InitMotorGroup(Motor_MotorGroupTypeDef* pgroup, Motor_MotorTypeEnum type, uint8_t motor_num, CAN_HandleTypeDef* phcan, uint16_t stdid) {
    if (pgroup == NULL) return;
    pgroup->motor_num = motor_num;
    pgroup->type = type;

    if (phcan == NULL) return;
    pgroup->can_handle = phcan;
    Can_InitTxHeader(&(pgroup->can_header), stdid, Const_Motor_MOTOR_TX_EXTID, Const_Motor_MOTOR_TX_DLC);

    for (int i = 0; i < 4; ++i) {
        pgroup->motor_handle[i] = NULL;
    }
}


/**
  * @brief      Set output
  * @param      pmotor: Pointer to motor object
  * @param      pparam: Pointer to motor parameter object
  * @retval     NULL
  */
void Motor_SetMotorOutput(Motor_MotorTypeDef* pmotor, float output) {
    pmotor->output = output;
}

void Motor_SetMotorOutputPowerLimit(Motor_MotorTypeDef* pmotor, float output)
{
	if(chassisPowerLimit.K != 0)	pmotor->output = output * chassisPowerLimit.K;
	else 													pmotor->output = output;
}
///**
//  * @brief      设置带功率限制的电机输出
//  * @details    根据底盘功率限制系数对输出值进行动态调整
//  *             当功率限制系数K有效时（K≠0），输出值会按比例缩放
//  * @param      pmotor: 电机对象指针
//  * @param      output: 原始输出值（预限制值）
//  * @retval     NULL
//  */
//void Motor_SetMotorOutputPowerLimit(Motor_MotorTypeDef* pmotor, float output)
//{

//    if(Chassic_pl.K !=0) {
//        pmotor->output = output * Chassic_pl.K;  // 应用功率限制
//    } else {
//        pmotor->output = output;  // 直通模式
//    }
//}

/**
  * @brief      Get motor output value
  * @param      pmotor: Pointer to motor object
  * @retval     Output value
  */
uint16_t Motor_GetMotorOutput(Motor_MotorTypeDef* pmotor) {
    if (pmotor == NULL) return 0;
    if (pmotor->type == Motor_TYPE_NOT_CONNECTED) return 0;

    int16_t ret = 0;
    if (pmotor->type == Motor_TYPE_RM3508 || pmotor->type == Motor_TYPE_RM2006 || pmotor->type ==Motor_TYPE_RM6020) {
        ret = (int16_t)(pmotor->output * 1000.0f);               // output / 10.0f * 10000.0f
        return (uint16_t)ret;
    }
    return 0;
}


/**
  * @brief      Transmitter output
  * @param      pgroup: Pointer to the motor group to send
  * @retval     NULL
  */
void Motor_SendMotorGroupOutput(Motor_MotorGroupTypeDef *pgroup) {
    if (pgroup == NULL) return;
    uint8_t txdata[64];
	memset(txdata, 0, 64);
    txdata[0] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[0]) >> 8);
    txdata[1] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[0]);
    txdata[2] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[1]) >> 8);
    txdata[3] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[1]);
    txdata[4] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[2]) >> 8);
    txdata[5] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[2]);
    txdata[6] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[3]) >> 8);
    txdata[7] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[3]);
				
    Can_SendMessage(pgroup->can_handle, &(pgroup->can_header), txdata);
}


/**
  * @brief      拨弹电机专用函数
  * @param      pgroup: Pointer to the motor group to send
  * @retval     NULL
	*	@note				由于英雄特殊的滑环构造导致拨弹电机必须要挂在CAN1上，因此和
	*							云台电机共用一组，拨弹电机ID是7，在组内对应的位置就是第三个电机
  */
void Motor_feed_Shoot_MotorGroupOutput(Motor_MotorGroupTypeDef *pgroup) {
    if (pgroup == NULL) return;
    uint8_t txdata[64];
	memset(txdata, 0, 64);
    txdata[4] = (uint8_t)(Motor_GetMotorOutput(pgroup->motor_handle[2]) >> 8);
    txdata[5] = (uint8_t)Motor_GetMotorOutput(pgroup->motor_handle[2]);

    Can_SendMessage(pgroup->can_handle, &(pgroup->can_header), txdata);
}


void Motor_SendMotorGroupsOutput() {
    for (int i = 0; i < MOTOR_GROUP_NUM; i++) {
        Motor_SendMotorGroupOutput(Motor_groupHandle[i]);
    }
}


/**
  * @brief      Judge whether any motor is offline
  * @param      NULL
  * @retval     Offline or not (1 is yes, 0 is no)
  */
uint8_t Motor_IsAnyMotorOffline() {
    for (int i = 0; i < MOTOR_GROUP_NUM; ++i) {
        for (int j = 0; j < 4; ++j) {
            if (!Motor_IsMotorOffline(Motor_groupHandle[i]->motor_handle[j])) {
                return 0;
            }
        }
    }
    return 1;
}


/**
  * @brief      Judge whether the motor is offline
  * @param      pmotor: Pointer to motor object
  * @retval     Offline or not (1 is yes, 0 is no)
  */
uint8_t Motor_IsMotorOffline(Motor_MotorTypeDef* pmotor) {
    if (pmotor == NULL) return 0;
    if (pmotor->type == Motor_TYPE_NOT_CONNECTED) return 0;
    uint32_t now = HAL_GetTick();
    return (now - pmotor->last_update_time) < Const_Motor_MOTOR_OFFLINE_TIME;
}


/********** ENCODER CALLBACK FUNCTION **********/

/**
  * @brief      rm2006 motor encoder callback
  * @param      pmotor: Pointer to motor object
  * @retval     NULL
  */
void rm2006_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    if (pmotor == NULL) return;
    if (len != 8) return;

    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3])) / 36.0f;
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp = 0; 
    pmotor->init = 1; 

    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
    // Calculate the shaft angle because the reduction ratio needs to be divided by 36
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 10.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 10.0f;
    if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.limited_angle < pmotor->encoder.init_offset - 180 && pmotor->encoder.init_offset >= 180)
        pmotor->encoder.limited_angle += 360;
    else if (pmotor->encoder.limited_angle > pmotor->encoder.init_offset + 180 && pmotor->encoder.init_offset < 180)
        pmotor->encoder.limited_angle -= 360;
    pmotor->last_update_time = HAL_GetTick(); 
}


/**
  * @brief      Gimbal motor encoder callback
  * @param      pmotor: Pointer to motor object
  * @retval     NULL
  */
void gm6020_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    // Calculate angle difference and number of cycles
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      //The increase of mechanical angle is positive
    if (diff < -5500)           // Make a positive turn
        pmotor->encoder.round_count++;
    else if (diff > 5500)       // Turn around in the opposite direction
        pmotor->encoder.round_count--;

    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3]));
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp = 0; 
    pmotor->init = 1; 

    // Calculate continuous angle
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f;
    pmotor->encoder.limited_angle = (float)pmotor->encoder.angle / 8192.0f * 360.0f;
}


/**
  * @brief      3508电机数据处理回调函数
  * @param      pmotor: Pointer to motor object
  * @retval     NULL
	* @note				官方3508减速比为19，如果改了减速箱不能使用本函数
  */
void rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    if (pmotor == NULL) return;
    

    // 更新角度数据
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3])) / 19.0f;
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp    = (float)((int16_t)((uint16_t)rxbuff[6]));
		pmotor->init = 1;
	
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
    // 计算轮子转动的轴角，电机转速比为19故除以19
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f / 19.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f / 19.0f;
    if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.limited_angle < pmotor->encoder.init_offset - 180 && pmotor->encoder.init_offset >= 180)
        pmotor->encoder.limited_angle += 360;
    else if (pmotor->encoder.limited_angle > pmotor->encoder.init_offset + 180 && pmotor->encoder.init_offset < 180)
        pmotor->encoder.limited_angle -= 360;	
	  
    pmotor->last_update_time = HAL_GetTick(); 
}


/**
  * @brief      3508电机,去除减速箱
  * @param      pmotor: Pointer to motor object
  * @retval     NULL
	* @note				去除减速箱
  */
void rm3508_shooter_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    if (pmotor == NULL) return;
    

    // 更新角度数据
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3]));
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp    = (float)((int16_t)((uint16_t)rxbuff[6]));
    
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
    // 计算轮子转动的轴角，电机转速比为19故除以19
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f  + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f ;
    if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.limited_angle < pmotor->encoder.init_offset - 180 && pmotor->encoder.init_offset >= 180)
        pmotor->encoder.limited_angle += 360;
    else if (pmotor->encoder.limited_angle > pmotor->encoder.init_offset + 180 && pmotor->encoder.init_offset < 180)
        pmotor->encoder.limited_angle -= 360;	
	
    pmotor->last_update_time = HAL_GetTick(); 
}



/**
  * @brief      英雄专用，供弹电机
  * @param      pmotor: Pointer to motor object
  * @retval     NULL
	* @note				本电机改了减速比，加速比为51
  */
void Hero_FeedMotor_rm3508_encoder_callback(Motor_MotorTypeDef *pmotor, uint8_t rxbuff[], uint32_t len) {
    if (pmotor == NULL) return;
    

    // 更新角度数据
    pmotor->encoder.last_angle = pmotor->init == 1 ? pmotor->encoder.angle : (rxbuff[0] << 8 | rxbuff[1]);
    pmotor->encoder.angle   = (float)((int16_t)((uint16_t)rxbuff[0] << 8 | (uint16_t)rxbuff[1]));
    pmotor->encoder.speed   = (float)((int16_t)((uint16_t)rxbuff[2] << 8 | (uint16_t)rxbuff[3])) / 51.0f;
    pmotor->encoder.current = (float)((int16_t)((uint16_t)rxbuff[4] << 8 | (uint16_t)rxbuff[5]));
    pmotor->encoder.temp    = (float)((int16_t)((uint16_t)rxbuff[6]));
    
    int diff = pmotor->encoder.angle - pmotor->encoder.last_angle;      
    if (diff < -4096)           
        pmotor->encoder.round_count++;
    else if (diff > 4096)       
        pmotor->encoder.round_count--;
    // 计算轮子转动的轴角，电机转速比为51故除以51
    pmotor->encoder.consequent_angle = (float)pmotor->encoder.round_count * 360.0f / 51.0f + 
                                       (float)pmotor->encoder.angle / 8192.0f * 360.0f / 51.0f;
		pmotor->init = 1;
    if (pmotor->encoder.round_count > 10000) {
        pmotor->encoder.consequent_angle -= 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }
    if (pmotor->encoder.round_count < -10000) {
        pmotor->encoder.consequent_angle += 10 * pmotor->encoder.round_count; 
        pmotor->encoder.round_count = 0;
    }

    pmotor->last_update_time = HAL_GetTick(); 
}

