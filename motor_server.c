#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <pthread.h>

#include "robot.h"
#include "command.h"

#define QUEUE_NAME "/robot_motor_queue"
#define MAX_MESSAGES 10


void *motor_server_thread(void *arg)
{
    RobotArm *robot = (RobotArm *)arg;

    mqd_t queue;

    MotorCommand cmd;

    struct mq_attr attr;

    printf("[MOTOR SERVER] Starting...\n");


    /* Remove any old queue from a previous run */
    mq_unlink(QUEUE_NAME);


    /* Configure message queue */
    attr.mq_flags = 0;

    attr.mq_maxmsg = MAX_MESSAGES;

    attr.mq_msgsize = sizeof(MotorCommand);

    attr.mq_curmsgs = 0;


    /* Create message queue */
    queue = mq_open(QUEUE_NAME,
                    O_CREAT | O_RDONLY,
                    0666,
                    &attr);


    if (queue == (mqd_t)-1)
    {
        printf("[MOTOR SERVER] ERROR: Cannot create message queue.\n");

        return NULL;
    }


    printf("[MOTOR SERVER] Message queue ready.\n");


    while (1)
    {
        /*
         * Wait for a command from
         * the main controller.
         */
        ssize_t received;

        received = mq_receive(queue,
                              (char *)&cmd,
                              sizeof(MotorCommand),
                              NULL);


        if (received == -1)
        {
            printf("[MOTOR SERVER] Receive error.\n");

            break;
        }


        printf("\n[MOTOR SERVER] Command received: %d\n",
               cmd.command);


        /* =====================================
           MOVE COMMAND
           ===================================== */

        if (cmd.command == CMD_MOVE)
        {
            printf("[MOTOR SERVER] MOVE command\n");

            printf("  Joint  : J%d\n",
                   cmd.joint_id);

            printf("  Target : %.2f degrees\n",
                   cmd.target_position);


            if (cmd.joint_id >= 1 &&
                cmd.joint_id <= NUM_JOINTS)
            {
                pthread_mutex_lock(&robot->lock);


                robot->joints[cmd.joint_id - 1]
                    .target_position =
                    cmd.target_position;


                pthread_mutex_unlock(&robot->lock);
            }
        }


        /* =====================================
           STOP COMMAND
           ===================================== */

        else if (cmd.command == CMD_STOP)
        {
            printf("[MOTOR SERVER] STOP command received.\n");


            pthread_mutex_lock(&robot->lock);

            robot->system_running = 0;

            pthread_mutex_unlock(&robot->lock);


            break;
        }


        /* =====================================
           HOME COMMAND
           ===================================== */

        else if (cmd.command == CMD_HOME)
        {
            int i;


            printf("[MOTOR SERVER] HOME command received.\n");


            pthread_mutex_lock(&robot->lock);


            for (i = 0; i < NUM_JOINTS; i++)
            {
                robot->joints[i].target_position = 0.0f;
            }


            pthread_mutex_unlock(&robot->lock);
        }


        /* =====================================
           EXIT COMMAND
           ===================================== */

        else if (cmd.command == CMD_EXIT)
        {
            printf("[MOTOR SERVER] EXIT command received.\n");

            break;
        }
    }


    /* Close queue */
    mq_close(queue);


    /* Remove queue */
    mq_unlink(QUEUE_NAME);


    printf("[MOTOR SERVER] Stopped.\n");


    return NULL;
}
