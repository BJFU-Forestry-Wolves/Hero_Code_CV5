/*
 *  Project      : Polaris
 * 
 *  file         : cha_gimbal_ctrl.c
 *  Description  : This file contains Gimbal Pitch control function
 *  LastEditors  : Polaris
 *  Date         : 2021-05-04 20:53:31
 *  LastEditTime : 2023-05-05 16:27:28
 */


#include "cmsis_os.h"
#include "sys_const.h"
#include "module_gimbal.h"
#include "protocol_common.h"
#include "app_ins.h"
#include "module_shoot.h"

GimbalPitch_GimbalPitchTypeDef GimbalPitch_gimbalPitchControlData;
GimbalYaw_GimbalYawTypeDef GimbalYaw_gimbalYawControlData;




/**
  * @brief      Gimbal pitch control initialization
  * @param      NULL
  * @retval     NULL
  */
void GimbalPitch_InitGimbalPitch() {
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();

    gimbalpitch->control_state = 1;
    gimbalpitch->output_state = 1;
    gimbalpitch->pitch_ref = 0;
    gimbalpitch->pitch_count = 0;

    PID_InitPIDParam(&gimbalpitch->spdPIDParam, Const_GimbalPitchSpdParam[0][0], Const_GimbalPitchSpdParam[0][1], Const_GimbalPitchSpdParam[0][2], Const_GimbalPitchSpdParam[0][3], 
                    Const_GimbalPitchSpdParam[0][4], Const_GimbalPitchSpdParam[1][0], Const_GimbalPitchSpdParam[1][1], Const_GimbalPitchSpdParam[2][0], Const_GimbalPitchSpdParam[2][1], 
                    Const_GimbalPitchSpdParam[3][0], Const_GimbalPitchSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&gimbalpitch->angPIDParam, Const_GimbalPitchAngParam[0][0], Const_GimbalPitchAngParam[0][1], Const_GimbalPitchAngParam[0][2], Const_GimbalPitchAngParam[0][3], 
                    Const_GimbalPitchAngParam[0][4], Const_GimbalPitchAngParam[1][0], Const_GimbalPitchAngParam[1][1], Const_GimbalPitchAngParam[2][0], Const_GimbalPitchAngParam[2][1], 
                    Const_GimbalPitchAngParam[3][0], Const_GimbalPitchAngParam[3][1], PID_POSITION);                      
}

/**
  * @brief      Gimbal yaw control initialization
  * @param      NULL
  * @retval     NULL
  */
void GimbalYaw_InitGimbalYaw() {
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();

    gimbalyaw->control_state = 1;
    gimbalyaw->output_state = 1;
    gimbalyaw->yaw_ref = 0;
    gimbalyaw->yaw_count = 0;

    PID_InitPIDParam(&gimbalyaw->spdPIDParam, Const_GimbalYawSpdParam[0][0], Const_GimbalYawSpdParam[0][1], Const_GimbalYawSpdParam[0][2], Const_GimbalYawSpdParam[0][3], 
                    Const_GimbalYawSpdParam[0][4], Const_GimbalYawSpdParam[1][0], Const_GimbalYawSpdParam[1][1], Const_GimbalYawSpdParam[2][0], Const_GimbalYawSpdParam[2][1], 
                    Const_GimbalYawSpdParam[3][0], Const_GimbalYawSpdParam[3][1], PID_POSITION);
    PID_InitPIDParam(&gimbalyaw->angPIDParam, Const_GimbalYawAngParam[0][0], Const_GimbalYawAngParam[0][1], Const_GimbalYawAngParam[0][2], Const_GimbalYawAngParam[0][3], 
                    Const_GimbalYawAngParam[0][4], Const_GimbalYawAngParam[1][0], Const_GimbalYawAngParam[1][1], Const_GimbalYawAngParam[2][0], Const_GimbalYawAngParam[2][1], 
                    Const_GimbalYawAngParam[3][0], Const_GimbalYawAngParam[3][1], PID_POSITION);                     
}

/**
  * @brief      Get the pointer of gimbal control object
  * @param      NULL
  * @retval     Pointer to gimbal control object
  */
GimbalPitch_GimbalPitchTypeDef* GimbalPitch_GetGimbalPitchPtr() {
    return &GimbalPitch_gimbalPitchControlData;
}
/**
  * @brief      Get the pointer of gimbal control object
  * @param      NULL
  * @retval     Pointer to gimbal control object
  */
GimbalYaw_GimbalYawTypeDef* GimbalYaw_GetGimbalYawPtr() {
    return &GimbalYaw_gimbalYawControlData;
}


/**
  * @brief      Set the gimbal control output calculation enabled state
  * @param      state: Enabled, 1 is enabled, 0 is disabled
  * @retval     NULL
  */
void GimbalPitch_SetGimbalPitchControlState(uint8_t state) {
    GimbalPitch_GimbalPitchTypeDef *gimbalPitch = GimbalPitch_GetGimbalPitchPtr();

    gimbalPitch->control_state = state;
}
/**
  * @brief      Set the gimbal control output calculation enabled state
  * @param      state: Enabled, 1 is enabled, 0 is disabled
  * @retval     NULL
  */
void GimbalYaw_SetGimbalYawControlState(uint8_t state) {
    GimbalYaw_GimbalYawTypeDef *gimbalYaw = GimbalYaw_GetGimbalYawPtr();

    gimbalYaw->control_state = state;
}


/**
  * @brief      Set gimbal control output enable status
  * @param      state: Enabled, 1 is enabled, 0 is disabled
  * @retval     NULL
  */
void GimbalPitch_SetGimbalPitchOutputState(uint8_t state) {
    GimbalPitch_GimbalPitchTypeDef *gimbalPitch = GimbalPitch_GetGimbalPitchPtr();

    gimbalPitch->output_state = state;
}
/**
  * @brief      Set gimbal control output enable status
  * @param      state: Enabled, 1 is enabled, 0 is disabled
  * @retval     NULL
  */
void GimbalYaw_SetGimbalYawOutputState(uint8_t state) {
    GimbalYaw_GimbalYawTypeDef *gimbalYaw = GimbalYaw_GetGimbalYawPtr();

    gimbalYaw->output_state = state;
}



/**
  * @brief      Set the target value of gimbal pitch
  * @param      pitch_ref: gimbal pitch target value
  * @retval     NULL
  */
void GimbalPitch_SetPitchRef(float pitch_ref) {
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    
    gimbalpitch->pitch_ref += pitch_ref;
}
/**
  * @brief      Set the target value of gimbal yaw
  * @param      yaw_ref: gimbal yaw target value
  * @retval     NULL
  */
void GimbalYaw_SetYawRef(float yaw_ref) {
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
    
    gimbalyaw->yaw_ref = yaw_ref;
}                                                             //Pitch加和，而Yaw直接赋值

/**
* @brief      俯仰角限制函数
* @param      ref: 俯仰角设定值
* @retval     限制后的俯仰角设定值
*/
float Gimbal_LimitPitch(float ref) {
    // 获取通信协议数据指针
    Protocol_DataTypeDef *buscomm = Protocol_GetBusDataPtr();
    // 获取云台俯仰数据指针
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();

    float pitch_umaxangle;
    // 根据通道模式选择俯仰最大角度
    if (buscomm->cha_mode == Cha_Gyro) {
        pitch_umaxangle = Const_PITCH_UMAXANGLE_GRYO;  // 陀螺仪模式的最大俯仰角
    } else {
        pitch_umaxangle = Const_PITCH_UMAXANGLE;       // 普通模式的最大俯仰角
    }
    
    // 检查是否超出俯仰角限制
    if (((PID_GetPIDRef(&gimbalpitch->angPID) > pitch_umaxangle) && (ref > 0)) ||  // 如果当前角度超过上限且设定值为正
        ((PID_GetPIDRef(&gimbalpitch->angPID) < Const_PITCH_DMAXANGLE) && (ref < 0))) { // 如果当前角度低于下限且设定值为负
        return 0.0f; // 返回0，限制设定值
    } else {
        return ref; // 否则返回原设定值
    }
}


/**
 * @brief      限制云台Yaw角
 * @param      ref: Yaw设定值
 * @retval     限制后的偏航角设定值
 */
float Gimbal_LimitYaw(float ref) {
    // 获取总线通信数据指针
    Protocol_DataTypeDef *buscomm = Protocol_GetBusDataPtr();
    // 获取惯性导航系统数据指针
    INS_INSTypeDef *ins = INS_GetINSPtr();

    // 如果当前模式为陀螺仪控制，直接返回设定值
	if (buscomm->cha_mode == Cha_Gyro)
        return ref;
    // 判断当前总偏航角度与设定参考值的差值是否超出最大允许范围
	else if (((ins->YawTotalAngle - buscomm->yaw_ref < -Const_YAW_MAXANGLE) && (ref > 0)) || 
             ((ins->YawTotalAngle - buscomm->yaw_ref >  Const_YAW_MAXANGLE) && (ref < 0))) 
        // 如果超出范围并且设定值的方向继续超出，则返回0.0f以限制偏航角
        return 0.0f;
    // 否则，返回原设定值
    else return ref;
}

/**
* @brief      Set pitch ref
* @param      ref: Yaw set ref
* @retval     NULL
*/
void Gimbal_SetPitchRef(float ref) {
    GimbalPitch_GimbalPitchTypeDef *gimbal = GimbalPitch_GetGimbalPitchPtr();

    gimbal->pitch_ref += ref;
}

/**
  * @brief      Setting IMU yaw position feedback
  * @param      imu_yaw_position_fdb: IMU Yaw Position feedback
  * @retval     NULL
  */
void GimbalYaw_SetIMUYawPositionFdb(float imu_yaw_position_fdb) {
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();

    gimbalyaw->yaw_position_fdb = imu_yaw_position_fdb;
}


/**
  * @brief      Setting IMU yaw speed feedback
  * @param      imu_yaw_speed_fdb: IMU Yaw Speed feedback
  * @retval     NULL
  */
void GimbalYaw_SetIMUYawSpeedFdb(float imu_yaw_speed_fdb) {
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();

    gimbalyaw->yaw_speed_fdb = imu_yaw_speed_fdb;
}


/**
  * @brief      云台俯仰轴控制函数
  * @param      NULL
  * @retval     NULL
  */
 void GimbalPitch_Control() {
    // 获取云台俯仰轴状态结构体的指针
		GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    // 获取陀螺仪数据结构体的指针
		INS_INSTypeDef *ins = INS_GetINSPtr();

		// 如果云台俯仰轴不处于控制状态，直接返回
    if (gimbalpitch->control_state != 1) return;
	
		//对Pitch轴的位置环进行PID计算
    PID_SetPIDRef(&gimbalpitch->angPID, gimbalpitch->pitch_ref);
    PID_SetPIDFdb(&gimbalpitch->angPID, ins->Roll + Const_PITCH_MOTOR_INIT_OFFSETf);
    PID_CalcPID(&gimbalpitch->angPID, &gimbalpitch->angPIDParam);

		//将位置环的输出结果作为输入放入速度环计算
    PID_SetPIDRef(&gimbalpitch->spdPID, PID_GetPIDOutput(&gimbalpitch->angPID));
    PID_SetPIDFdb(&gimbalpitch->spdPID, ins->Gyro[Y_INS]);
    PID_CalcPID(&gimbalpitch->spdPID, &gimbalpitch->spdPIDParam);   
	
		//将计算结果输出
//		Motor_SetMotorOutput(&Motor_PitchMotor, -PID_GetPIDOutput(&gimbalpitch->angPID));
    Motor_SetMotorOutput(&Motor_PitchMotor, -PID_GetPIDOutput(&gimbalpitch->spdPID));
}


/**
  * @brief      云台航向轴控制函数
  * @param      NULL
  * @retval     NULL
  */
void GimbalYaw_Control() {
		// 获取云台航向轴状态结构体的指针
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
    // 获取陀螺仪数据结构体的指针
		INS_INSTypeDef *ins = INS_GetINSPtr();
	
    if (gimbalyaw->control_state != 1) return;


    PID_SetPIDRef(&gimbalyaw->angPID, gimbalyaw->yaw_ref);
    PID_SetPIDFdb(&gimbalyaw->angPID, ins->YawTotalAngle);
    PID_CalcPID(&gimbalyaw->angPID, &gimbalyaw->angPIDParam);

    PID_SetPIDRef(&gimbalyaw->spdPID, PID_GetPIDOutput(&gimbalyaw->angPID));
    PID_SetPIDFdb(&gimbalyaw->spdPID, ins->Gyro[Z_INS]);
    PID_CalcPID(&gimbalyaw->spdPID, &gimbalyaw->spdPIDParam);   

    Motor_SetMotorOutput(&Motor_YawMotor, PID_GetPIDOutput(&gimbalyaw->spdPID));
//	  Motor_SetMotorOutput(&Motor_YawMotor, PID_GetPIDOutput(&gimbalyaw->angPID));
}


/**
  * @brief      Gimbal pitch output function
  * @param      NULL
  * @retval     NULL
  */
void GimbalPitch_Output() {
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
		Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
	
    if (gimbalpitch->output_state != 1) 
		{
				return;
//				PID_SetPIDFdb(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);
		};
	
	  if (gimbalyaw->output_state != 1) return;
	
    Motor_SendMotorGroupOutput(&Motor_PitchMotors);
}


void yaw_reset_offset()
{
		GimbalYaw_GimbalYawTypeDef *yaw = GimbalYaw_GetGimbalYawPtr();
	
		if (yaw->yaw_offset == Offset_Nomal)
			CHASSIS_YAW_ANGLE_OFFSET += 0;
		
		if(yaw->yaw_offset == Offset_Add)
			CHASSIS_YAW_ANGLE_OFFSET += 0.01;
		
		if(yaw->yaw_offset == Offset_Back)
			CHASSIS_YAW_ANGLE_OFFSET -= 0.01;
		
		if(CHASSIS_YAW_ANGLE_OFFSET > 360)
		{
				CHASSIS_YAW_ANGLE_OFFSET = 0;
		}
		else if (CHASSIS_YAW_ANGLE_OFFSET < 0)
		{
				CHASSIS_YAW_ANGLE_OFFSET = 360;
		}
}




///**
//  * @brief      Gimbal yaw output function
//  * @param      NULL
//  * @retval     NULL
//  */
//void GimbalYaw_Output() {
//    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();

//    if (gimbalyaw->output_state != 1) {
//		Motor_SetMotorOutput(&Motor_YawMotor, 0);
//	}
//    Motor_SendMotorGroupOutput(&Motor_YawMotors);
//}
