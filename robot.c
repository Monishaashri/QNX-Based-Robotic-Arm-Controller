#include <stdio.h>
#include <pthread.h>

#include "robot.h"


/* ==========================================
   Initialize Robotic Arm
   ========================================== */

void robot_init(RobotArm *robot)
{
    int i;

    /* Initialize mutex */
    pthread_mutex_init(&robot->lock, NULL);

    /* Initialize all joints */
    for (i = 0; i < NUM_JOINTS; i++)
    {
        robot->joints[i].id = i + 1;

        robot->joints[i].position = 0.0f;

        robot->joints[i].target_position = 0.0f;

        robot->joints[i].velocity = 0.0f;

        robot->joints[i].temperature = 30.0f;

        robot->joints[i].current = 0.0f;

        robot->joints[i].enabled = 1;

        robot->joints[i].fault = 0;
    }

    /* Initialize system status */
    robot->system_running = 1;

    robot->emergency_stop = 0;

    robot->safety_fault = 0;
}


/* ==========================================
   Display Robotic Arm Status
   ========================================== */

void robot_print_status(RobotArm *robot)
{
    int i;

    printf("\n");
    printf("============================================\n");
    printf("             ROBOT ARM STATUS\n");
    printf("============================================\n");

    printf("Joint   Position   Target   Velocity   Temp   Current\n");

    printf("------------------------------------------------------\n");


    /* Protect shared robot data */
    pthread_mutex_lock(&robot->lock);


    /* Display all joint information */
    for (i = 0; i < NUM_JOINTS; i++)
    {
        printf("J%d      %7.2f   %7.2f   %7.2f   %5.1f   %5.2f\n",
               robot->joints[i].id,
               robot->joints[i].position,
               robot->joints[i].target_position,
               robot->joints[i].velocity,
               robot->joints[i].temperature,
               robot->joints[i].current);
    }


    printf("------------------------------------------------------\n");


    /* Display system status */
    printf("System          : %s\n",
           robot->system_running ? "RUNNING" : "STOPPED");


    /* Display emergency stop status */
    printf("Emergency Stop  : %s\n",
           robot->emergency_stop ? "ACTIVE" : "INACTIVE");


    /* Display safety status */
    printf("Safety Fault    : %s\n",
           robot->safety_fault ? "DETECTED" : "NONE");


    /* Release mutex */
    pthread_mutex_unlock(&robot->lock);
}
