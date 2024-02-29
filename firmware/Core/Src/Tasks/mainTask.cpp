/*
 * mainTask.cpp
 *
 *  Created on: Feb 22, 2024
 *      Author: lenovo
 */

#include <tasksPrototypes.h>
#include <FreeRTOS.h>
#include <task.h>
#include <spi.h>

void mainTaskEntry(void *argument)
{
	while(true)
	{
		osDelay(10);

	}
}
