#ifndef __ALG_POWER_LIMIT_H
#define __ALG_POWER_LIMIT_H

#include "main.h"
#include "module_chassis.h"
#include "sys_const.h"
#include "arm_math.h"
typedef struct
{
	double powerCurrent;
	double Ct;
	double K1;
	double K2;
	double a;
}ChassisTyrePowerTypeDef;

typedef struct
{
	double powerLimit;
	double powerCurrent;
	ChassisTyrePowerTypeDef* chassisTyre[4];
	double K;
	double lastK;
}PowerLimitTypeDef;

extern ChassisTyrePowerTypeDef chassisTyre[4];
extern PowerLimitTypeDef chassisPowerLimit;
void PowerLimitInit(void);
void PowerCalculate(ChassisTyrePowerTypeDef* tyre, Motor_MotorTypeDef* motor);
float* powerConstPtr(void);
void ChassisPowerControl(void);
void ChassisPowerCalculate(void);
#endif

