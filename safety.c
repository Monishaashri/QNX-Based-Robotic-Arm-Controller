#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "robot.h"


void *safety_thread(void *arg)
{
    RobotArm *robot = (RobotArm *)arg;

    int fault_injected = 0;
    int cycle_count = 0;

    printf("[SAFETY] Safety monitoring thread started.\n");

    while (robot->system_running)
    {
        int i;

        pthread_mutex_lock(&robot->lock);

        cycle_count++;

        /*
         * TEST FAULT INJECTION
         *
         * After approximately 2 seconds,
         * simulate an over-current condition
         * on Joint 3.
         *
         * Safety monitoring period = 100 ms
         * 20 cycles = approximately 2 seconds.
         */
        if (!fault_injected && cycle_count >= 20)
        {
            printf("\n");
            printf("[SAFETY] *** TEST FAULT INJECTION ***\n");
            printf("[SAFETY] Simulating J3 motor over-current.\n");

            robot->joints[2].current = 6.0f;

            fault_injected = 1;
        }


        /* Check all joints */
        for (i = 0; i < NUM_JOINTS; i++)
        {
            Joint *joint = &robot->joints[i];


            /* Check joint position */
            if (joint->position > MAX_JOINT_POSITION ||
                joint->position < MIN_JOINT_POSITION)
            {
                joint->fault = 1;

                robot->safety_fault = 1;

                printf("[SAFETY] FAULT: J%d position limit exceeded.\n",
                       joint->id);
            }


            /* Check temperature */
            if (joint->temperature > MAX_TEMPERATURE)
            {
                joint->fault = 1;

                robot->safety_fault = 1;

                printf("[SAFETY] FAULT: J%d over-temperature.\n",
                       joint->id);
            }


            /* Check motor current */
            if (joint->current > MAX_CURRENT)
            {
                joint->fault = 1;

                robot->safety_fault = 1;

                printf("[SAFETY] FAULT: J%d over-current.\n",
                       joint->id);

                printf("[SAFETY] J%d current = %.2f A\n",
                       joint->id,
                       joint->current);
            }
        }


        /* Emergency shutdown */
        if (robot->safety_fault)
        {
            printf("\n");
            printf("[SAFETY] !!! SAFETY FAULT DETECTED !!!\n");
            printf("[SAFETY] Emergency stop activated.\n");

            robot->emergency_stop = 1;

            robot->system_running = 0;
        }


        pthread_mutex_unlock(&robot->lock);


        /* Safety monitoring period = 100 ms */
        usleep(100000);
    }


    printf("[SAFETY] Safety monitoring thread stopped.\n");

    return NULL;
}
