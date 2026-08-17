#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "robot.h"


/* Motor control thread */
void *motor_control_thread(void *arg)
{
    RobotArm *robot = (RobotArm *)arg;

    printf("[MOTOR] Motor control thread started.\n");

    while (robot->system_running)
    {
        int i;

        /* Lock shared robot data */
        pthread_mutex_lock(&robot->lock);

        /* Check emergency stop */
        if (robot->emergency_stop)
        {
            pthread_mutex_unlock(&robot->lock);

            printf("[MOTOR] Emergency stop active. Motors stopped.\n");

            break;
        }

        /* Control all joints */
        for (i = 0; i < NUM_JOINTS; i++)
        {
            Joint *joint = &robot->joints[i];

            /* Skip disabled or faulty joints */
            if (!joint->enabled || joint->fault)
            {
                continue;
            }

            /* Move joint toward target position */
            if (joint->position < joint->target_position)
            {
                joint->position += 1.0f;

                if (joint->position > joint->target_position)
                {
                    joint->position = joint->target_position;
                }
            }
            else if (joint->position > joint->target_position)
            {
                joint->position -= 1.0f;

                if (joint->position < joint->target_position)
                {
                    joint->position = joint->target_position;
                }
            }

            /* Simulated motor parameters */
            joint->velocity = 1.0f;

            joint->current =
                0.5f + (joint->position * 0.01f);
        }

        /* Unlock shared robot data */
        pthread_mutex_unlock(&robot->lock);

        /* Motor control period = 100 ms */
        usleep(100000);
    }

    printf("[MOTOR] Motor control thread stopped.\n");

    return NULL;
}
