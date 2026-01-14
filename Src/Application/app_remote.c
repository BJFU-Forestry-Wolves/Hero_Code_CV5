/*
 *  Project      : Polaris
 * 
 *  file         : app_remote.c
 *  Description  : This file contains Remote control function
 *  LastEditors  : Polaris
 *  Date         : 2021-05-04 20:53:31
 *  LastEditTime : 2023-05-07 11:26:15
 */


#include "sys_const.h"
#include "protocol_common.h"
#include "app_remote.h"
#include "app_referee.h"
#include "module_shoot.h"
#include "module_gimbal.h"
#include "module_chassis.h"
#include "module_referee.h"
#include "periph_servo.h"
#include "cmsis_os.h"
#include "app_autoaim.h"
#define REMOTE_TASK_PERIOD  1

Remote_RemoteControlTypeDef Remote_remoteControlData;
Math_SlopeParamTypeDef Remote_ChassisFBSlope;


/**
  * @brief          遥控任务
  * @param          NULL
  * @retval         NULL
  */
void Remote_Task(void const * argument) {

    forever {
        Remote_ControlCom(); 						// 执行遥控控制通信操作
        osDelay(REMOTE_TASK_PERIOD); 		// 延时，等待下一个任务周期
    }
}


/**
  * @brief      遥控器控制初始化
  * @param      NULL
  * @retval     NULL
  */
void Remote_RemotrControlInit() {
    Remote_RemoteControlTypeDef *control_data = Remote_GetControlDataPtr();

    Math_InitSlopeParam(&Remote_ChassisFBSlope, MOUSE_CHASSIS_ACCELERATE, MOUSE_CHASSIS_ACCELERATE);
}


/**
  * @brief      Gets the pointer to the remote control data object
  * @param      NULL
  * @retval     Pointer to remote control data object
  */
Remote_RemoteControlTypeDef* Remote_GetControlDataPtr() {
    return &Remote_remoteControlData;
}


/**
* @brief      处理遥控命令
* @param      NULL
* @retval     NULL
*/
void Remote_ControlCom() {
		// 获取遥控器的控制数据指针
    Remote_RemoteControlTypeDef *control_data = Remote_GetControlDataPtr();
		Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
	
    control_data->pending = 1;// 标记遥控器控制命令正在处理中

    // 根据左侧开关的位置选择相应的控制模式
    switch (data->remote.s[0]) {
        case Remote_SWITCH_UP: {
            // 左侧开关向上时进入普通模式
            Remote_RemoteProcess();
            break;
        }
        case Remote_SWITCH_MIDDLE: {
            // 左侧开关中间时进入射击模式
            Remote_RemoteShooterModeSet();
            break;
        }
        case Remote_SWITCH_DOWN: {
            // 左侧开关向下时进入键鼠模式
            Remote_KeyMouseProcess();
            Remote_MouseShooterModeSet();
						shooter_control();
            break;
        }
        default:
            break;
    }

    control_data->pending = 0;// 任务处理完成，清除标记
}


void shooter_control()
{
			Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
//	Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();
	   switch (data->remote.s[1]) {
        case Remote_SWITCH_UP: {
            // 左侧开关向上时进入普通模式
            
            break;
        }
        case Remote_SWITCH_MIDDLE: {
            // 左侧开关中间时进入射击模式
            Shooter_ChangeShooterMode(Shoot_FAST);
            break;
        }
        case Remote_SWITCH_DOWN: {

						
            break;
        }
        default:
            break;
    }


}


/**
* @brief      鼠标射击模式设置
* @details    根据鼠标左键的触发状态控制供弹模式，实现单击/连发功能
*             包含摩擦轮转速安全检查，防止低速发射
* @param      NULL
* @retval     NULL
*/
void Remote_MouseShooterModeSet() {
    // 获取数据指针
    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();  // 遥控器原始数据
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr(); // 射击系统状态

    /*---------------- 摩擦轮安全检查 ----------------*/
  
    // 当前生效的安全检查：当左右摩擦轮转速均低于30时停止供弹
    if ((fabs(Motor_ShootLeftMotor.encoder.speed) <= 30) || 
        (fabs(Motor_ShootRightMotor.encoder.speed) <= 30)) 
    {
        Shooter_ChangeFeederMode(Feeder_NULL);  // 停止供弹
        return;  // 直接返回不执行后续逻辑
    }

    /*---------------- 鼠标按键状态处理 ----------------*/
    static int count_mouse_L = 0;  // 静态计数器，记录左键持续按下帧数
		static uint8_t last_mouse_state = 0;  // 上次鼠标状态 0:释放 1:按下

		// 检测上升沿：当前按下且上次未按下
		if (data->mouse.l == 1 && last_mouse_state == 0) 
		{
				// 执行单发射击逻辑
				Shooter_FeederReady();
				Shooter_SingleShootReset();         // 重置单发状态机
				Shooter_ChangeFeederMode(Feeder_SINGLE); // 触发单发模式
			
		}
		
		last_mouse_state = data->mouse.l;
	
}

/**
* @brief      遥控器射击
* @param      NULL
* @retval     NULL
*/
void Remote_RemoteShooterModeSet() {
		
		Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
		Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();

		// 根据右侧开关的位置选择射击模式
    switch (data->remote.s[1]) {
        case Remote_SWITCH_UP: {
            /*	右侧开关向上为停止射击模式	*/
            Shooter_ChangeShooterMode(Shoot_NULL);
            Shooter_ChangeFeederMode(Feeder_NULL);
            break;
        }
        case Remote_SWITCH_MIDDLE: {
            /*	右侧开关中间时进入准备射击模式	*/
						Remote_FeedStateJudge();
            Shooter_ChangeShooterMode(Shoot_FAST);
            Shooter_ChangeFeederMode(Feeder_READY);
            break;
        }
        case Remote_SWITCH_DOWN: {
            /*	右侧开关向下，进入单发射击模式	*/
            Shooter_ChangeShooterMode(Shoot_FAST);
            Shooter_ChangeFeederMode(Feeder_SINGLE);

            break;
        }
        default:
            break;
    }

		
//		Chassis_Follow();
		
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
		gimbalpitch->output_state = 1;
		gimbalyaw->output_state = 1;
    Protocol_DataTypeDef *buscomm = Protocol_GetBusDataPtr();
    buscomm->yaw_ref += Gimbal_LimitYaw((float)data->remote.ch[2] * -Const_WHEELLEG_REMOTE_YAW_GAIN 
													+ (float)visionDataGet.yaw_angle.yaw_predict *0.005f*0.004f);
		GimbalYaw_SetYawRef(buscomm->yaw_ref);

		float pitch_ref;
		pitch_ref = (float)data->remote.ch[3] * REMOTE_PITCH_ANGLE_TO_REF 
								+ (float)visionDataGet.pitch_angle.pitch_predict * 0.01f * 0.002f;
		GimbalPitch_SetPitchRef(Gimbal_LimitPitch(-pitch_ref));
		
}


/**
* @brief      遥控器控制流程
* @param      NULL
* @retval     NULL
*/
void Remote_RemoteProcess() {
    // 获取遥状态,数据指针
    Protocol_DataTypeDef *buscomm = Protocol_GetBusDataPtr();
    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();

    // 获取云台俯仰和偏航的控制结构体指针
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
    
    // 初始化云台俯仰和偏航的输出状态为0
    gimbalpitch->output_state = 0;
    gimbalyaw->output_state = 0;

    // 根据右侧开关的位置选择相应的控制模式
    switch (data->remote.s[1]) {
        case Remote_SWITCH_UP: {
            // 右侧开关向上时云台和底盘都不输出
            gimbalpitch->output_state = 0;
            gimbalyaw->output_state = 0;
            Chassis_SetChassisMode(Chassis_NULL);			//设置底盘模式为下电
						Chassis_SetChassisRef(NULL,NULL,NULL);		//底盘速度设置为0
					
						Shooter_ChangeShooterMode(Shoot_NULL);		//云台摩擦轮下电
            break;
        }
        case Remote_SWITCH_MIDDLE: {
            // 右侧开关中间时云台和底盘跟随遥控器控制
            gimbalpitch->output_state = 1;
            gimbalyaw->output_state = 1;
						Chassis_Follow();
					
						Remote_YawStateJudge();
						yaw_reset_offset();

            break;
        }
        case Remote_SWITCH_DOWN: {
            // 右侧开关向下时进入小陀螺模式
            gimbalpitch->output_state = 1;
            gimbalyaw->output_state = 1;
            Chassis_SetChassisYawAngle(Motor_YawMotor.encoder.limited_angle, CHASSIS_YAW_ANGLE_OFFSET);
            Chassis_SetChassisMode(Chassis_XTL);
            Chassis_SetChassisRef((float)data->remote.ch[1], (float)data->remote.ch[0], CHASSIS_XTL_WZ);
            break;
        }
        default:
            break;
    }
    
    // 计算并设置云台的偏航参考值，如果云台处于下电状态则不更改其期待值
		if (gimbalyaw->output_state != 0)
		{
			buscomm->yaw_ref += Gimbal_LimitYaw((float)data->remote.ch[2] * -Const_WHEELLEG_REMOTE_YAW_GAIN 
                                        + (float)visionDataGet.yaw_angle.yaw_predict * 0.01f * 0.004f);
			GimbalYaw_SetYawRef(buscomm->yaw_ref);
		}

    
    // 计算并设置云台的俯仰参考值
		if (gimbalpitch->output_state != 0)
		{
			float pitch_ref;
			pitch_ref = (float)data->remote.ch[3] * REMOTE_PITCH_ANGLE_TO_REF 
									+ (float)visionDataGet.pitch_angle.pitch_predict * 0.01f * 0.002f;
			GimbalPitch_SetPitchRef(Gimbal_LimitPitch(-pitch_ref));
		}
}

/**
* @brief      键鼠控制处理函数
* @details    处理来自遥控器的按键和鼠标输入，控制底盘运动、云台转动、射击系统等
* @param      NULL
* @retval     NULL
*/


void Remote_KeyMouseProcess() { 
    // 获取各模块数据指针
    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();        					// 遥控器原始数据
    Remote_RemoteControlTypeDef *control_data = Remote_GetControlDataPtr(); 		// 控制数据结构
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();     					// 射击系统状态
    GimbalPitch_GimbalPitchTypeDef *gimbal = GimbalPitch_GetGimbalPitchPtr(); 	// 云台俯仰数据
    Protocol_DataTypeDef *buscomm = Protocol_GetBusDataPtr();          					// 总线通信数据

    /*----------------------- 底盘控制 -----------------------*/
    static float chassis_vx;  // 底盘X轴速度（前后方向）
    static float chassis_vy;  // 底盘Y轴速度（左右方向）
    static float chassis_wz;
	
    // 左右平移固定200（A/D键控制）//
    float right_speed = 250.0f;    // 
		float right_speed_step = 13.0f;
    if(data->key.a == 1) {         // A键左移
        chassis_vy -= right_speed_step;
				if(chassis_vy <= -right_speed) chassis_vy = - right_speed;
    } else if(data->key.d == 1) {  // D键右移
				chassis_vy += right_speed_step;
				if(chassis_vy >= right_speed_step) chassis_vy = right_speed;
    } else {                       // 无操作
        chassis_vy = 0.0f;
    }

    // 前后移动根据Shift变化（假设W/S键控制chassis_vx）//
    float front_speed = 300.0f;    // 基础速度300（根据需求调整）
		float front_speed_step = 10.0f;

    if(data->key.w == 1) {         // W键前移
        chassis_vx += front_speed_step;
				if(chassis_vx >= front_speed) chassis_vx = front_speed;
    } else if(data->key.s == 1) {  // S键后移
				chassis_vx -= front_speed_step;
				if(chassis_vx <= -front_speed) chassis_vx = -front_speed;
    } else {                       // 无操作
        chassis_vx = 0.0f;
    }
		
    // Shift键切换底盘模式
    if(data->key.shift == 1) {
        // XTL模式（小陀螺模式）：保持底盘朝向固定角度
				chassis_wz += 8.0f;
				if(chassis_wz >= CHASSIS_XTL_WZ) chassis_wz = CHASSIS_XTL_WZ;
				
        Chassis_SetChassisYawAngle(Motor_YawMotor.encoder.limited_angle, CHASSIS_YAW_ANGLE_OFFSET);
        Chassis_SetChassisMode(Chassis_XTL);
        Chassis_SetChassisRef(chassis_vx, chassis_vy, CHASSIS_XTL_WZ); // 设置固定旋转速度
    } 
		else if(data->mouse.r != 1)
		{
        // 跟随模式：底盘跟随云台角度
        Chassis_SetChassisMode(Chassis_FOLLOW);
        // 设置底盘的参考值（前后、左右移动和偏航角度）
        float deg = Chassis_ResetYawAngle(Motor_YawMotor.encoder.limited_angle,
                                        CHASSIS_YAW_ANGLE_OFFSET);
        Chassis_SetChassisRef(chassis_vx, chassis_vy, deg);
    }
		
		static uint8_t last_key_q_state = 0;  // 上次鼠标状态 0:释放 1:按下
		
    // Q给英雄YAW轴零点校准
    if(data->key.q == 1  &&  last_key_q_state == 0) {
				GimbalYaw_GimbalYawTypeDef *yaw = GimbalYaw_GetGimbalYawPtr();
				
				CHASSIS_YAW_ANGLE_OFFSET -= 1;
			
				if(CHASSIS_YAW_ANGLE_OFFSET > 360)
						CHASSIS_YAW_ANGLE_OFFSET = 0;
				
				else if (CHASSIS_YAW_ANGLE_OFFSET < 0)
						CHASSIS_YAW_ANGLE_OFFSET = 360;	
    } 
		
		last_key_q_state = data->key.q;
		

    /*--------------------- 自动瞄准控制 ---------------------*/
    float autoaim_yaw;    // 自动瞄准yaw轴补偿
    float autoaim_pitch;  // 自动瞄准pitch轴补偿
    if(data->mouse.r == 1) {  // 鼠标右键按下时启用自动瞄准
        // 从视觉数据获取预测角度并进行单位转换
        autoaim_yaw = (float)visionDataGet.yaw_angle.yaw_predict *0.005f*0.004f;
        autoaim_pitch = (float)visionDataGet.pitch_angle.pitch_predict * 0.01f * 0.002f;
				
    } else {                   // 未启用自动瞄准
        autoaim_yaw = 0.0f;
        autoaim_pitch = 0.0f;
    }

    /*---------------------- 云台控制 -----------------------*/
    GimbalPitch_GimbalPitchTypeDef *gimbalpitch = GimbalPitch_GetGimbalPitchPtr();
    GimbalYaw_GimbalYawTypeDef *gimbalyaw = GimbalYaw_GetGimbalYawPtr();
    gimbalpitch->output_state = 1;  // 启用云台俯仰轴输出
    gimbalyaw->output_state = 1;    // 启用云台偏航轴输出

    // 计算偏航轴目标角度（鼠标移动 + 自动瞄准补偿）
    buscomm->yaw_ref += Gimbal_LimitYaw(
        (float)data->mouse.x * -MOUSE_YAW_ANGLE_TO_FACT  + autoaim_yaw
    );
    GimbalYaw_SetYawRef(buscomm->yaw_ref);

    // 计算俯仰轴目标角度（鼠标移动 + 自动瞄准补偿）
    float pitch_ref = ((float)data->mouse.y * -MOUSE_PITCH_ANGLE_TO_FACT + autoaim_pitch);
    GimbalPitch_SetPitchRef(Gimbal_LimitPitch(-pitch_ref));  // 设置俯仰角度并限制范围

    /*---------------------- 射击控制 -----------------------*/
    // F键启动高速射击模式，G键停止射击
    if (data->key.f == 1) 
		{ 
			Shooter_ChangeShooterMode(Shoot_FAST);		// 开启摩擦轮	
			Shooter_ChangeFeederMode(Feeder_READY);		// 设置供弹电机状态
		}	

    if (data->key.g == 1) 
		{
			Shooter_ChangeShooterMode(Shoot_NULL);   // 停止射击
			Shooter_ChangeFeederMode(Feeder_NULL);		// 设置供弹电机状态
		}
			
}

/**
 * @brief 控制底盘进入跟随模式，并根据遥控器输入设置底盘的运动参考值。
 * 
 * 本函数通过获取遥控器的输入数据，计算偏航角度并设置底盘的运动参考值， 
 * 使底盘按照遥控器的指令进行前后、左右移动以及调整偏航角度。
 * 
 * @note 偏航角度的计算时会考虑舵机的当前角度以及预定的偏航角度补偿值。
 * @note 如果偏航轴电机掉线，可以使用遥控器的第三通道作为替代的偏航角度参考值（此部分代码已被注释掉）。
 */
void Chassis_Follow()
{
	    Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
	
			Chassis_SetChassisMode(Chassis_FOLLOW);
			// 设置底盘的参考值（前后、左右移动和偏航角度）
			float deg = Chassis_ResetYawAngle(Motor_YawMotor.encoder.limited_angle,
																				CHASSIS_YAW_ANGLE_OFFSET);
			Chassis_SetChassisRef((float)data->remote.ch[1], (float)data->remote.ch[0], deg);
//			Chassis_SetChassisRef((float)data->remote.ch[1], (float)data->remote.ch[0], 
//																				(float)data->remote.ch[2] * 0.1);//yaw轴电机掉线用这个
}


/**
* @brief      供弹状态判断
* @param      NULL
* @retval     NULL
*/
void Remote_FeedStateJudge()
{
		Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
		Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();

		if(data->remote.ch[4] == 0)
		{
				shooter->feeder_state = Feeder_Nomal;
		}
		else if(data->remote.ch[4] > 0)
		{
				shooter->feeder_state = Feeder_Back;
		}
		else if(data->remote.ch[4] < 0)
		{
				shooter->feeder_state = Feeder_Add;
		}
}


/**
* @brief      云台补偿状态
* @param      NULL
* @retval     NULL
*/
void Remote_YawStateJudge()
{
		Remote_RemoteDataTypeDef *data = Remote_GetRemoteDataPtr();
		GimbalYaw_GimbalYawTypeDef *yaw = GimbalYaw_GetGimbalYawPtr();
	
		if(data->remote.ch[4] == 0)
		{
			yaw->yaw_offset = Offset_Nomal;
		}
		else if(data->remote.ch[4] > 0)
		{
			yaw->yaw_offset = Offset_Add;
		}
		else if(data->remote.ch[4] < 0)
		{
			yaw->yaw_offset = Offset_Back;
		}
}
