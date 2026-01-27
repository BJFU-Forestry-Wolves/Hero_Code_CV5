/*
 *  Project      : Infantry_Neptune
 * 
 *  file         : gim_shoot_ctrl.h
 *  Description  : This file contains Shooter control function
 *  LastEditors  : Lee
 *  Date         : 
 *  LastEditTime : 
 */

#ifndef GIM_SHOOT_CTRL_H
#define GIM_SHOOT_CTRL_H

#ifdef __cplusplus
extern "C" {
#endif


#include "alg_pid.h"
#include "alg_math.h"
#include "periph_motor.h"

typedef enum {
	Feeder_NULL             = 0u,
	Feeder_SINGLE           = 1u,
	Feeder_FAST_CONTINUE    = 2u,
  Feeder_LOW_CONTINUE     = 3u,
	Feeder_LOCKED_ROTOR     = 4u,
  Feeder_REFEREE          = 5u,
  Feeder_FINISH           = 6u, 
	Feeder_READY							=7u
}Shoot_FeederModeEnum;

typedef enum {
	Shoot_NULL		= 0u,
	Shoot_FAST  	= 1u,
	Shoot_SLOW 	    = 2u,
	Shoot_REFEREE 	= 3u 
}Shoot_ShooterModeEnum;

typedef struct {
	float left_shoot_speed;
  	float right_shoot_speed;
  	float feeder_shoot_speed;
}Shoot_ShootSpeedTypeDef; 

typedef struct {
	float speed_15mm_offset;
  	float speed_18mm_offset;
    float speed_22mm_offset;
  	float speed_30mm_offset;
}Shoot_ShootSpeedOffsetTypeDef; 

typedef struct {
    float shooter_42mm_cooling_heat;
    float shooter_42mm_cooling_rate;
    float shooter_42mm_heat_remain;
    
    float current_speed;
    uint8_t current_pidnum;
    
    uint16_t heat_tracking;
}Shooter_HeatCtrlTypeDef;

typedef enum{
		Feeder_Nomal							=	0,			//不执行任何操作
		Feeder_Add								= 1,			
		Feeder_Back								=	2
}	Shoot_FeederState;			//供弹电机连续供弹/退弹状态

typedef struct {
		uint8_t shooter_control;											//射击标志位
		Shoot_ShooterModeEnum 	shooter_mode;					//射击模式
		Shoot_FeederModeEnum 		feeder_mode;					//供弹模式
		Shoot_FeederState				feeder_state;					//供弹电机连续供弹/退弹状态
		Shoot_FeederModeEnum 		last_feeder_mode;			//上一次供弹模式

		uint8_t single_shoot_done;										//单次射击模式

		Shoot_ShootSpeedTypeDef shoot_speed;					//射击速度
		Shoot_ShootSpeedOffsetTypeDef shoot_speed_offset;	//射击速度偏差

		float shooter_speed_15mpers;
		float shooter_speed_18mpers;
		float shooter_speed_22mpers;
		float shooter_speed_30mpers;

		Shooter_HeatCtrlTypeDef heat_ctrl;

		PID_PIDTypeDef shootLeftPID;
		PID_PIDParamTypeDef shootLeftPIDParam;
		PID_PIDTypeDef shootRightPID;
		PID_PIDParamTypeDef shootRightPIDParam;

		PID_PIDTypeDef feedSpdPID;
		PID_PIDParamTypeDef feedSpdPIDParam;
		PID_PIDTypeDef feedAngPID;
		PID_PIDParamTypeDef feedAngPIDParam;
}Shoot_StatusTypeDef;


extern Shoot_StatusTypeDef Shooter_ShooterControl;

void Shooter_InitShooter(void);
Shoot_StatusTypeDef* Shooter_GetShooterControlPtr(void);
void Shooter_ChangeShooterMode(Shoot_ShooterModeEnum mode);
void Shooter_ChangeFeederMode(Shoot_FeederModeEnum mode);
float Shooter_GetShootSpeedOffset(void);
void Shooter_Control(void);
float Shooter_GetRefereeSpeed(void);
void Shooter_UpdataControlData(void);
void Shooter_SetFeederSpeed(float speed);
void Shooter_SetShooterSpeed(float speed);
void Shooter_ForceChangeFeederMode(Shoot_FeederModeEnum mode);
void Shooter_FeederMotorLockedJudge(void);
void Shooter_MotorLockedHandle(void);
void Shooter_AngleCorrect(void);
void Shooter_RealAngleCorrect(void);
uint8_t Shooter_HeatCtrl(void);
void Shooter_ShootControl(void);
void Shooter_SingleShootCtrl(void);
void Shooter_SingleShootReset(void);
void Shooter_FeederControl(void);
void Shooter_ShooterMotorOutput(void);
float* Shooter_GetRefereeSpeedPtr(void);

void Shooter_FeederReady();

#endif

#ifdef __cplusplus
}
#endif
