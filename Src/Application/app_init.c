/*
 *  Project      : Polaris Robot 
 *  
 *  FilePath     : app_init.c
 *  Description  : All initialization threads
 *  LastEditors  : Polaris
 *  Date         : 2023-01-23 03:42:52
 *  LastEditTime : 2023-05-05 15:17:43
 */


#include "app_init.h"
#include "sys_softTimer.h"
#include "sys_dwt.h"
#include "alg_pid.h"
#include "util_can.h"
#include "periph_motor.h"
#include "periph_bmi088.h"
#include "periph_remote.h"
#include "periph_referee.h"
#include "periph_servo.h"
#include "module_gimbal.h"
#include "module_shoot.h"
#include "module_chassis.h"
#include "module_referee.h"
#include "app_ins.h"
#include "app_remote.h"
#include "protocol_common.h"
#include "periph_autoaim.h"
#include "app_autoaim.h"
#include "module_supercap.h"



void Init_InitAll() {
    DWT_Init(168); // 初始化DWT计数器，设置系统时钟频率为168 MHz

    BMI088_Init(0); // 初始化BMI088传感器，传入参数0表示使用默认配置
    Remote_InitRemote(); // 初始化遥控器模块

    // 初始化惯性导航系统
    INS_Init();

    // 初始化电机组
    Can_InitFilterAndStart(&hcan1); 		// 初始化CAN1的过滤器并启动
    Can_InitFilterAndStart(&hcan2); 		// 初始化CAN2的过滤器并启动
    Motor_InitAllMotors(); 							// 初始化所有电机

    Referee_InitReferee(); 						// 初始化裁判系统
    Servo_InitAllServos(); 						// 初始化所有舵机
	
//  	Referee_Setup(); // 设置裁判系统

		GimbalPitch_InitGimbalPitch(); 		// 初始化云台Pitch轴
		GimbalYaw_InitGimbalYaw(); 				// 初始化云台Yaw轴
    Shooter_InitShooter(); 							// 初始化射击系统
		Chassis_InitChassis(); 						// 初始化底盘
		
		// 自瞄串口初始化
		usart1_tx_dma_init();
		usart1_rx_dma_init();

//    Remote_RemotrControlInit(); 				// 初始化遥控器控制
    Protocol_InitProtocol(); 					// 初始化通信协议
		
		SuperCap_InitHeader(&CapHeader);
		SuperCap_SetPower(&hcan1,&CapHeader,70);
}



void Init_Task(void const * argument) {
    SoftTimer_StartAll();
    forever {
        vTaskSuspend(Init_TaskHandleHandle);
      osDelay(25);
    }
}
