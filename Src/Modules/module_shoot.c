/*
 *  Project      : Infantry_Neptune
 * 
 *  file         : gim_shoot_ctrl.c
 *  Description  : This file contains Shooter control function
 *  LastEditors  : Polaris
 *  Date         : 2021-05-04 20:53:31
 *  LastEditTime : 2023-05-07 09:29:25
 */

#include "module_shoot.h"
#include "sys_const.h"
#include "cmsis_os.h"

#include "periph_referee.h"
#include "util_gpio.h"

#include "util_debug.h"

Shoot_StatusTypeDef Shooter_ShooterControl;

/**
  * @brief      shooter control initialization
  * @param      NULL
  * @retval     NULL
  */
void Shooter_InitShooter() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    
    shooter->shooter_control = 1;

    shooter->feeder_mode = Feeder_NULL;
    shooter->heat_ctrl.shooter_42mm_cooling_heat = 0;
    shooter->heat_ctrl.shooter_42mm_cooling_rate = 0;

    shooter->shooter_mode = Shoot_NULL;
    shooter->shoot_speed.feeder_shoot_speed = 0;
    shooter->shoot_speed.left_shoot_speed = 0;
    shooter->shoot_speed.right_shoot_speed = 0;
    
    shooter->shooter_speed_15mpers = Const_Shooter15mpers;
    shooter->shooter_speed_18mpers = Const_Shooter18mpers;
    shooter->shooter_speed_22mpers = Const_Shooter22mpers;
    shooter->shooter_speed_30mpers = Const_Shooter30mpers;

    PID_InitPIDParam(&shooter->shootLeftPIDParam, Const_ShootLeftParam[0][0], Const_ShootLeftParam[0][1], Const_ShootLeftParam[0][2], Const_ShootLeftParam[0][3], 
                    Const_ShootLeftParam[0][4], Const_ShootLeftParam[1][0], Const_ShootLeftParam[1][1], Const_ShootLeftParam[2][0], Const_ShootLeftParam[2][1], 
                    Const_ShootLeftParam[3][0], Const_ShootLeftParam[3][1], PID_POSITION);      
    PID_InitPIDParam(&shooter->shootRightPIDParam, Const_ShootRightParam[0][0], Const_ShootRightParam[0][1], Const_ShootRightParam[0][2], Const_ShootRightParam[0][3], 
                    Const_ShootRightParam[0][4], Const_ShootRightParam[1][0], Const_ShootRightParam[1][1], Const_ShootRightParam[2][0], Const_ShootRightParam[2][1], 
                    Const_ShootRightParam[3][0], Const_ShootRightParam[3][1], PID_POSITION); 
    PID_InitPIDParam(&shooter->feedAngPIDParam, Const_FeedAngParam[0][0], Const_FeedAngParam[0][1], Const_FeedAngParam[0][2], Const_FeedAngParam[0][3], 
                    Const_FeedAngParam[0][4], Const_FeedAngParam[1][0], Const_FeedAngParam[1][1], Const_FeedAngParam[2][0], Const_FeedAngParam[2][1], 
                    Const_FeedAngParam[3][0], Const_FeedAngParam[3][1], PID_POSITION); 
    PID_InitPIDParam(&shooter->feedSpdPIDParam, Const_FeedSpdParam[0][0], Const_FeedSpdParam[0][1], Const_FeedSpdParam[0][2], Const_FeedSpdParam[0][3], 
                    Const_FeedSpdParam[0][4], Const_FeedSpdParam[1][0], Const_FeedSpdParam[1][1], Const_FeedSpdParam[2][0], Const_FeedSpdParam[2][1], 
                    Const_FeedSpdParam[3][0], Const_FeedSpdParam[3][1], PID_POSITION); 
}


/**
  * @brief      射击线程控制函数
  * @param      NULL  没有输入参数
  * @retval     NULL  没有返回值
  */
void Shooter_Control() {
		// 更新数据
    Shooter_UpdataControlData();

		// 控制摩擦轮
    Shooter_ShootControl();
    
		// 控制拨盘
//    Shooter_FeederControl();		

		// 电机输出控制
    Shooter_ShooterMotorOutput();
}


/**
  * @brief      Gets the pointer to the shooter control data object
  * @param      NULL
  * @retval     Pointer to shooter control data object
  */
Shoot_StatusTypeDef* Shooter_GetShooterControlPtr() {
    return &Shooter_ShooterControl;
}


/**
  * @brief      Get speed offset variable    (150 means offset is 0)
  * @param      NULL
  * @retval     NULL
  */
float Shooter_GetShootSpeedOffset() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    Referee_RefereeDataTypeDef *referee = Referee_GetRefereeDataPtr();
    float offset_speed;

    switch (referee->shooter_heat0_speed_limit) {
        case 15: 
            offset_speed = shooter->shoot_speed_offset.speed_15mm_offset;
            break;
        case 18:          
            offset_speed = shooter->shoot_speed_offset.speed_18mm_offset;
            break;
        case 22:
            offset_speed = shooter->shoot_speed_offset.speed_22mm_offset;
            break;
        case 30:
            offset_speed = shooter->shoot_speed_offset.speed_30mm_offset;
            break;
        default:
            offset_speed = 15.0f;
            break;
    }
    return offset_speed;
}


/**
  * @brief      更改射击器的射击模式
  * @param      mode: 射击模式的枚举值
  * @retval     NULL
  */
void Shooter_ChangeShooterMode(Shoot_ShooterModeEnum mode) {
		// 获取射击器状态结构体的指针
    Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();
//    Referee_RefereeDataTypeDef *referee = Referee_GetRefereeDataPtr();
//    
//    if (referee->mains_power_shooter_output == 1)
//        shooter->shooter_mode = mode;
//    else 
//        shooter->shooter_mode = Shoot_NULL;
	  shooter->shooter_mode = mode;
}


/**
  * @brief      更改射击器的供弹模式
  * @param      mode: 供弹模式的枚举值
  * @retval     NULL
  */
void Shooter_ChangeFeederMode(Shoot_FeederModeEnum mode) {
    // 获取射击器状态结构体的指针
    Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();

    // 如果当前供弹模式为锁定状态，则不进行更改
    if (shooter->feeder_mode == Feeder_LOCKED_ROTOR) return;

    // 保存当前供弹模式为上一次供弹模式
    shooter->last_feeder_mode = shooter->feeder_mode;

    // 更新当前供弹模式
    shooter->feeder_mode = mode;

    // 
    // 
//    if ((shooter->feeder_mode != shooter->last_feeder_mode) &&
//       ((shooter->last_feeder_mode == Feeder_SINGLE) ||
//        (shooter->last_feeder_mode == Feeder_REFEREE))) {
//        shooter->feeder_mode = Feeder_FINISH;
//        Shooter_AngleCorrect();
//    }
}



/**
  * @brief      Set referee shooter speed
  * @param      NULL
  * @retval     NULL
  */
float Shooter_GetRefereeSpeed() {
    Referee_RefereeDataTypeDef *referee = Referee_GetRefereeDataPtr();
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

    float speed;
    switch (referee->shooter_heat0_speed_limit) {
        case 15: 
            speed = shooter->shooter_speed_15mpers;
            break;
        case 18:
            speed = shooter->shooter_speed_18mpers;
            break;
        case 22:
            speed = shooter->shooter_speed_22mpers;
            break;
        case 30:
            speed = shooter->shooter_speed_30mpers;
            break;
        default:
            speed = shooter->shooter_speed_15mpers;
            break;
    }

    return speed;
}


/**
  * @brief      更新控制数据
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
Referee_RefereeDataTypeDef *test_referee;
void Shooter_UpdataControlData() {
		// 获取射手控制结构体与裁判系统数据结构体指针
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    Referee_RefereeDataTypeDef *referee = Referee_GetRefereeDataPtr();
	
		// 更新42mm弹丸冷却热量
    shooter->heat_ctrl.shooter_42mm_cooling_heat = (float)referee->shooter_heat0;
    // 更新42mm弹丸冷却速率
		shooter->heat_ctrl.shooter_42mm_cooling_rate = (float)referee->shooter_heat0_cooling_limit;
    
    Shooter_FeederMotorLockedJudge();
}


/**
  * @brief      设置供弹电机速度
  * @param      speed: 供弹电机速度参考值
  * @retval     NULL  无返回值
  */
void Shooter_SetFeederSpeed(float speed) {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

		// 设置供弹电机的速度
    shooter->shoot_speed.feeder_shoot_speed = speed;
}


/**
  * @brief      设置摩擦轮电机速度
  * @param      speed: 摩擦轮电机速度参考值
  * @retval     NULL  无返回值
  */
void Shooter_SetShooterSpeed(float speed) {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    
    shooter->shoot_speed.left_shoot_speed  = speed;
    shooter->shoot_speed.right_shoot_speed = -speed;
}


/**
  * @brief      Force change shooter mode
  * @param      mode: Feeder mode
  * @retval     NULL
  */
void Shooter_ForceChangeFeederMode(Shoot_FeederModeEnum mode) {
    Shoot_StatusTypeDef* shooter = Shooter_GetShooterControlPtr();

    shooter->feeder_mode = mode;
}


/**
  * @brief      供弹电机堵转判断
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_FeederMotorLockedJudge() {
  Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

  static int count = 0;// 用于计数判断电机堵转的时间
	// 如果当前供弹模式不是锁定模式
  if (shooter->feeder_mode != Feeder_LOCKED_ROTOR) {
		 // 如果电机电流大于等于设定的堵转电流且速度小于等于设定的堵转速度
    if ((fabs(Motor_FeedMotor.encoder.current) >= Const_ShooterLockedCurrent) &&
       (fabs(Motor_FeedMotor.encoder.speed) <= Const_ShooterLockedSpeed)) {
        count ++;// 增加计数器
				 // 如果计数器超过设定的堵转时间
        if (count > Const_ShooterLockedTime) {
						// 强制改变供弹模式为锁定模式
            Shooter_ForceChangeFeederMode(Feeder_LOCKED_ROTOR);
        }
    }
    else count = 0;// 如果条件不满足，则重置计数器
  }
}


/**
  * @brief      Motor locked handle
  * @param      NULL
  * @retval     NULL
  */
void Shooter_MotorLockedHandle() {
	  static int count_reverse = 0;
	  Shooter_SetFeederSpeed(Const_ShooterLockedReverseSpeed);
	  count_reverse++;
	  if(count_reverse >= Const_ShooterRelockedTime) {
		    count_reverse = 0;
        Shooter_ForceChangeFeederMode(Feeder_NULL);
	}
}


/**
  * @brief      停止电机转动
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_AngleCorrect() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    PID_SetPIDRef(&shooter->feedAngPID, PID_GetPIDFdb(&shooter->feedAngPID));
}

void Shooter_RealAngleCorrect() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    PID_SetPIDRef(&shooter->feedAngPID, ((int)(PID_GetPIDFdb(&shooter->feedAngPID) + 40.0f) / 45) * 45);
}


/**
  * @brief      Shooter heat control
  * @param      NULL
  * @retval     pid_num
  */
uint8_t Shooter_HeatCtrl() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

    if ((shooter->heat_ctrl.shooter_42mm_cooling_rate - shooter->heat_ctrl.shooter_42mm_cooling_heat)  >= Const_HeatCtrlFastLimit) {   // sufficient heat remain, fast shooting
        shooter->heat_ctrl.current_speed = Const_FeederFastSpeed;
        shooter->heat_ctrl.current_pidnum = 1;
        Shooter_SetFeederSpeed(shooter->heat_ctrl.current_speed);
        shooter->heat_ctrl.heat_tracking = 0;
    }
    else {
        if ((shooter->heat_ctrl.shooter_42mm_cooling_rate - shooter->heat_ctrl.shooter_42mm_cooling_heat)  >= Const_HeatCtrlSlowLimit) {
            shooter->heat_ctrl.current_speed = Const_FeederSlowSpeed;
            shooter->heat_ctrl.current_pidnum = 1;
            Shooter_SetFeederSpeed(shooter->heat_ctrl.current_speed);
            shooter->heat_ctrl.heat_tracking = 0;
        }
        else {
            if ((shooter->heat_ctrl.shooter_42mm_cooling_rate - shooter->heat_ctrl.shooter_42mm_cooling_heat) <= Const_HeatCtrlWaitLimit) {
            shooter->heat_ctrl.current_speed = Const_FeederWaitSpeed;
            shooter->heat_ctrl.current_pidnum = 1;
            Shooter_SetFeederSpeed(shooter->heat_ctrl.current_speed);
            shooter->heat_ctrl.heat_tracking = 0;
           }
           else {
                if ((shooter->heat_ctrl.shooter_42mm_cooling_rate - shooter->heat_ctrl.shooter_42mm_cooling_heat) <= Const_HeatCtrlStopLimit) {
                    shooter->heat_ctrl.heat_tracking = 0;
                    Shooter_AngleCorrect();
                    shooter->heat_ctrl.current_pidnum = 2;
                }
            }
        }
    }
    return shooter->heat_ctrl.current_pidnum;
}


/**
  * @brief      射击控制函数
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_ShootControl() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

		// 根据当前射击模式进行处理
    switch (shooter->shooter_mode) {
      case Shoot_NULL:									//停止射击模式
          GPIO_Reset(LASER);						//关闭激光
          Shooter_SetShooterSpeed(0);		//设置摩擦轮速度为0
          break;
      case Shoot_FAST:									//高速射击模式
          GPIO_Set(LASER);							//开启激光
          Shooter_SetShooterSpeed(Const_ShooterFastSpeed);//设置摩擦轮速度
          break;
      case Shoot_SLOW:									//低速射击模式
          GPIO_Set(LASER);							//开启激光
          Shooter_SetShooterSpeed(Const_ShooterSlowSpeed);
          break;
      case Shoot_REFEREE:								//裁判系统控制模式
          GPIO_Set(LASER);
          Shooter_SetShooterSpeed(Shooter_GetRefereeSpeed() + Shooter_GetShootSpeedOffset());
          break;
      default:
          break;
    }

    PID_SetPIDRef(&shooter->shootLeftPID, shooter->shoot_speed.left_shoot_speed);
    PID_SetPIDRef(&shooter->shootRightPID, shooter->shoot_speed.right_shoot_speed);
    PID_SetPIDFdb(&shooter->shootLeftPID, Motor_ShootLeftMotor.encoder.speed);
    PID_SetPIDFdb(&shooter->shootRightPID, Motor_ShootRightMotor.encoder.speed);
    PID_CalcPID(&shooter->shootLeftPID, &shooter->shootLeftPIDParam);
    PID_CalcPID(&shooter->shootRightPID, &shooter->shootRightPIDParam);
    Motor_SetMotorOutput(&Motor_ShootLeftMotor, PID_GetPIDOutput(&shooter->shootLeftPID));
    Motor_SetMotorOutput(&Motor_ShootRightMotor, PID_GetPIDOutput(&shooter->shootRightPID));
}


/**
  * @brief      供弹控制：单次射击
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_SingleShootCtrl() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    

		if (shooter->single_shoot_done == 0) {   // 检查是否已完成单次射击
					// 设置供弹电机的角度环，注意更改这里实现对转盘转动角度的控制
			PID_AddPIDRef(&shooter->feedAngPID, 60.0f);
				
			shooter->single_shoot_done = 1;

			PID_SetPIDFdb(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);	
		}	
		
		PID_SetPIDFdb(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);
		
		

}


/**
  * @brief      供弹控制：重置单次射击状态的标志位
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_SingleShootReset() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
    
    shooter->single_shoot_done = 0;
}


/**
  * @brief      供弹电机准备
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_FeederReady()
{
		Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
		
		//shooter->FeedAngle = Motor_FeedMotor.encoder.consequent_angle;
		PID_SetPIDRef(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);
		PID_SetPIDFdb(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);

}


/**
  * @brief      供弹控制
  * @param      NULL  无输入参数
  * @retval     NULL  无返回值
  */
void Shooter_FeederControl() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();
		static uint32_t feed_update_time;
    int current_pid_num = 0;
    switch (shooter->feeder_mode) {
      case Feeder_NULL:
          current_pid_num = 1;
          Shooter_SetFeederSpeed(0);
          break;
      case Feeder_READY:
					shooter->single_shoot_done = 0;
					switch	(shooter->feeder_state){
						case Feeder_Nomal:
								current_pid_num = 2;
								Shooter_SingleShootReset();		//重置标志位
								Shooter_FeederReady();				//本函数用于解决里程累计BUG
								break;
//						case Feeder_Add:
//								current_pid_num = 1;
//								Shooter_SetFeederSpeed(20);
//								break;
//						case Feeder_Back:
//								current_pid_num = 1;
//								Shooter_SetFeederSpeed(-10);
//								break;
						default:
								Shooter_FeederReady();				//本函数用于解决里程累计BUG
								break;
					}
					break;
      case Feeder_SINGLE:
          current_pid_num = 2;
					Shooter_SingleShootCtrl();
			
          break;
      case Feeder_REFEREE:
          current_pid_num = Shooter_HeatCtrl();
          break;
      default:
          break;
    }
		static uint32_t lock_time = 0;
		if(feed_update_time == Motor_FeedMotor.last_update_time)
		{
			lock_time++;
			if(lock_time >= 500)
			{
				PID_SetPIDRef(&shooter->feedAngPID, Motor_FeedMotor.encoder.consequent_angle);
				lock_time = 0;
			}
		}
		else lock_time = 0;
		feed_update_time = Motor_FeedMotor.last_update_time;
		// 根据当前 PID 模式进行 PID 计算和输出
    if (current_pid_num == 1) {
			Motor_SetMotorOutput(&Motor_FeedMotor, 0);
        //PID_ClearPID(&shooter->feedAngPID);// 将角度环PID参数重置
//        PID_SetPIDRef(&shooter->feedSpdPID, shooter->shoot_speed.feeder_shoot_speed);
//        PID_SetPIDFdb(&shooter->feedSpdPID, Motor_FeedMotor.encoder.speed);
//        PID_CalcPID(&shooter->feedSpdPID, &shooter->feedSpdPIDParam);
//        Motor_SetMotorOutput(&Motor_FeedMotor, PID_GetPIDOutput(&shooter->feedSpdPID));
    }
    if (current_pid_num == 2) {	
        PID_CalcPID(&shooter->feedAngPID, &shooter->feedAngPIDParam);     
        PID_SetPIDRef(&shooter->feedSpdPID, PID_GetPIDOutput(&shooter->feedAngPID));
        PID_SetPIDFdb(&shooter->feedSpdPID, Motor_FeedMotor.encoder.speed);
        PID_CalcPID(&shooter->feedSpdPID, &shooter->feedSpdPIDParam);   
				Motor_SetMotorOutput(&Motor_FeedMotor, PID_GetPIDOutput(&shooter->feedSpdPID)); 
    }   
}


/**
  * @brief      Output shooter motor
  * @param      NULL
  * @retval     NULL
  */
void Shooter_ShooterMotorOutput() {
    Shoot_StatusTypeDef *shooter = Shooter_GetShooterControlPtr();

    if (shooter->shooter_control == 1) {
        Motor_SendMotorGroupOutput(&Motor_ShootMotors);

//				Motor_feed_Shoot_MotorGroupOutput(&Motor_PitchMotors);//供弹电机的控制放在此处有BUG，移动到云台的控制线程
				
    }
}
