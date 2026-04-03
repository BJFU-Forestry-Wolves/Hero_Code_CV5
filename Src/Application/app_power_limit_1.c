#include "app_power_limit.h"

void PowerlimitTask(void const * argument)
{
	PowerLimitInit();
	for(;;)
	{
		ChassisPowerCalculate();
		ChassisPowerControl();	
		osDelay(3);
	}
}
