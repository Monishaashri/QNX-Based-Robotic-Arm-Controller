#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "robot.h"


/* Sensor monitoring thread */
void *sensor_thread(void *arg)
{
    RobotArm *robot = (RobotArm *)arg;

    printf("[SENSOR] Sensor thread started.\n");

    while (robot->system_running)
    {
        int i;

        /* Lock shared robot data */
        pthread_mutex_lock(&robot->lock);

        /* Simulate sensor readings */
        for (i = 0; i < NUM_JOINTS; i++)
        {
            robot->joints[i].temperature =
                30.0f + (robot->joints[i].current * 2.0f);
        }

        /* Unlock shared robot data */
        pthread_mutex_unlock(&robot->lock);

        /* Sensor sampling period = 200 ms */
        usleep(200000);
    }

    printf("[SENSOR] Sensor thread stopped.\n");

    return NULL;
}
