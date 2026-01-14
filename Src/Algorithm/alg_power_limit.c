#include "alg_power_limit.h"

ChassisTyrePowerTypeDef chassisTyre[4];

PowerLimitTypeDef chassisPowerLimit;

double myFabs(double num)
{
	if(num >= 0)
		return num;
	else
		return -num;
}

void PowerLimitInit(void)
{
	for(uint8_t i=0; i<4; i++)
	{
		chassisTyre[i].a = Const_PowerLimit[0];
		chassisTyre[i].Ct = Const_PowerLimit[1];
		chassisTyre[i].K1 = Const_PowerLimit[2];
		chassisTyre[i].K2 = Const_PowerLimit[3];
		chassisTyre[i].powerCurrent = 0.0f;
		chassisPowerLimit.lastK = 0.0f;
	}
}

float test;
void PowerCalculate(ChassisTyrePowerTypeDef* tyre, Motor_MotorTypeDef* motor)
{
  test = motor->encoder.current;
	tyre->powerCurrent = (tyre->K1 * motor->encoder.current * motor->encoder.current) 
												+ (motor->encoder.speed * motor->encoder.current)/9.55f 
												+ tyre->a;
}

float powerConst = 30000;

float* powerConstPtr()
{
	return &powerConst;
}


void ChassisPowerControl(void)
{
	double speedCmdSum;
	double TCmdSum;
	double speedSquaredSum;
	for(uint8_t i=0; i<4; i++)
	{
		speedCmdSum += Motor_ChassisMotors.motor_handle[i]->encoder.speed * Motor_ChassisMotors.motor_handle[i]->output;
		TCmdSum += Motor_ChassisMotors.motor_handle[i]->output * Motor_ChassisMotors.motor_handle[i]->output;
		speedSquaredSum += Motor_ChassisMotors.motor_handle[i]->encoder.speed * Motor_ChassisMotors.motor_handle[i]->encoder.speed;
	}
	double temp = speedCmdSum * speedCmdSum - 4 * Const_PowerLimit[2] * TCmdSum * (Const_PowerLimit[3] * speedSquaredSum - powerConst);
	chassisPowerLimit.K = 10.0f * (-speedCmdSum + sqrt(temp));

	if(TCmdSum != 0 && temp >= 0)
	{
		chassisPowerLimit.K /= 100.0f * 2.0f * Const_PowerLimit[2] * TCmdSum;
		chassisPowerLimit.K = myFabs(chassisPowerLimit.K);
	}
	else chassisPowerLimit.K = 0.0f;
	
	chassisPowerLimit.K = 0.8f * chassisPowerLimit.lastK + (1.0f - 0.8f) * chassisPowerLimit.K;
	chassisPowerLimit.lastK = chassisPowerLimit.K;
}

void ChassisPowerCalculate(void)
{
	PowerCalculate(&chassisTyre[0], &Motor_ChassisFontRightMotor);
	PowerCalculate(&chassisTyre[1], &Motor_ChassisFontLeftMotor);
	PowerCalculate(&chassisTyre[2], &Motor_ChassisBackLeftMotor);
	PowerCalculate(&chassisTyre[3], &Motor_ChassisBackRightMotor);	
}
