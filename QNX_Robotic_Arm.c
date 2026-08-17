#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <mqueue.h>
#include <sched.h>

#include "robot.h"
#include "command.h"

#define QUEUE_NAME "/robot_motor_queue"


int main(void)
{
    RobotArm robot;

    pthread_t motor_thread;
    pthread_t sensor_thread_id;
    pthread_t motor_server_thread_id;
    pthread_t safety_thread_id;

    mqd_t queue;

    MotorCommand cmd;

    /* Thread attributes */
    pthread_attr_t motor_attr;
    pthread_attr_t sensor_attr;
    pthread_attr_t safety_attr;

    /* Scheduling parameters */
    struct sched_param motor_param;
    struct sched_param sensor_param;
    struct sched_param safety_param;


    printf("\n");
    printf("========================================\n");
    printf("       QNX ROBOTIC ARM CONTROLLER\n");
    printf("========================================\n");


    /* =====================================
       INITIALIZE ROBOT
       ===================================== */

    robot_init(&robot);


    /* =====================================
       INITIALIZE THREAD ATTRIBUTES
       ===================================== */

    pthread_attr_init(&motor_attr);
    pthread_attr_init(&sensor_attr);
    pthread_attr_init(&safety_attr);


    /* =====================================
       SET THREAD PRIORITIES
       ===================================== */

    motor_param.sched_priority = 25;
    sensor_param.sched_priority = 20;
    safety_param.sched_priority = 30;


    pthread_attr_setschedparam(&motor_attr,
                               &motor_param);

    pthread_attr_setschedparam(&sensor_attr,
                               &sensor_param);

    pthread_attr_setschedparam(&safety_attr,
                               &safety_param);


    printf("\n");
    printf("========================================\n");
    printf("       THREAD PRIORITY CONFIGURATION\n");
    printf("========================================\n");

    printf("Safety Thread  : Priority 30\n");
    printf("Motor Thread   : Priority 25\n");
    printf("Sensor Thread  : Priority 20\n");
    printf("Motor Server   : Default Priority\n");


    /* =====================================
       START MOTOR CONTROL THREAD
       ===================================== */

    if (pthread_create(&motor_thread,
                       &motor_attr,
                       motor_control_thread,
                       &robot) != 0)
    {
        printf("ERROR: Motor thread creation failed.\n");

        return -1;
    }


    /* =====================================
       START SENSOR THREAD
       ===================================== */

    if (pthread_create(&sensor_thread_id,
                       &sensor_attr,
                       sensor_thread,
                       &robot) != 0)
    {
        printf("ERROR: Sensor thread creation failed.\n");

        robot.system_running = 0;

        pthread_join(motor_thread, NULL);

        return -1;
    }


    /* =====================================
       START MOTOR SERVER THREAD
       ===================================== */

    if (pthread_create(&motor_server_thread_id,
                       NULL,
                       motor_server_thread,
                       &robot) != 0)
    {
        printf("ERROR: Motor server thread creation failed.\n");

        robot.system_running = 0;

        pthread_join(motor_thread, NULL);
        pthread_join(sensor_thread_id, NULL);

        return -1;
    }


    /* =====================================
       START SAFETY THREAD
       ===================================== */

    if (pthread_create(&safety_thread_id,
                       &safety_attr,
                       safety_thread,
                       &robot) != 0)
    {
        printf("ERROR: Safety thread creation failed.\n");

        robot.system_running = 0;

        pthread_join(motor_thread, NULL);
        pthread_join(sensor_thread_id, NULL);
        pthread_join(motor_server_thread_id, NULL);

        return -1;
    }


    printf("\n");
    printf("========================================\n");
    printf("          THREAD STATUS\n");
    printf("========================================\n");

    printf("Motor thread        : CREATED\n");
    printf("Sensor thread       : CREATED\n");
    printf("Motor server        : CREATED\n");
    printf("Safety thread       : CREATED\n");


    /* =====================================
       WAIT FOR MESSAGE QUEUE
       ===================================== */

    sleep(1);


    /* =====================================
       OPEN MESSAGE QUEUE
       ===================================== */

    queue = mq_open(QUEUE_NAME, O_WRONLY);

    if (queue == (mqd_t)-1)
    {
        printf("\nERROR: Cannot open motor message queue.\n");

        robot.system_running = 0;

        pthread_join(motor_thread, NULL);
        pthread_join(sensor_thread_id, NULL);
        pthread_join(motor_server_thread_id, NULL);
        pthread_join(safety_thread_id, NULL);

        return -1;
    }


    printf("\n");
    printf("========================================\n");
    printf("       SENDING MOTOR COMMANDS\n");
    printf("========================================\n");


    /* =====================================
       MOVE J1
       ===================================== */

    cmd.command = CMD_MOVE;
    cmd.joint_id = 1;
    cmd.target_position = 30.0f;

    mq_send(queue,
            (const char *)&cmd,
            sizeof(MotorCommand),
            0);

    printf("\n[CONTROL] MOVE J1 -> 30 degrees\n");

    sleep(2);


    /* =====================================
       MOVE J2
       ===================================== */

    cmd.command = CMD_MOVE;
    cmd.joint_id = 2;
    cmd.target_position = 45.0f;

    mq_send(queue,
            (const char *)&cmd,
            sizeof(MotorCommand),
            0);

    printf("[CONTROL] MOVE J2 -> 45 degrees\n");

    sleep(2);


    /* =====================================
       MOVE J3
       ===================================== */

    cmd.command = CMD_MOVE;
    cmd.joint_id = 3;
    cmd.target_position = 60.0f;

    mq_send(queue,
            (const char *)&cmd,
            sizeof(MotorCommand),
            0);

    printf("[CONTROL] MOVE J3 -> 60 degrees\n");

    sleep(2);


    /* =====================================
       HOME ROBOT
       ===================================== */

    cmd.command = CMD_HOME;
    cmd.joint_id = 0;
    cmd.target_position = 0.0f;

    mq_send(queue,
            (const char *)&cmd,
            sizeof(MotorCommand),
            0);

    printf("[CONTROL] HOME command sent\n");

    sleep(5);


    /* =====================================
       EXIT MOTOR SERVER
       ===================================== */

    cmd.command = CMD_EXIT;
    cmd.joint_id = 0;
    cmd.target_position = 0.0f;

    mq_send(queue,
            (const char *)&cmd,
            sizeof(MotorCommand),
            0);

    printf("[CONTROL] EXIT command sent\n");


    /* Close message queue */
    mq_close(queue);


    /* =====================================
       STOP ROBOT
       ===================================== */

    robot.system_running = 0;


    /* =====================================
       WAIT FOR THREADS
       ===================================== */

    pthread_join(motor_thread, NULL);

    pthread_join(sensor_thread_id, NULL);

    pthread_join(motor_server_thread_id, NULL);

    pthread_join(safety_thread_id, NULL);


    /* =====================================
       FINAL ROBOT STATUS
       ===================================== */

    robot_print_status(&robot);


    /* Destroy mutex */
    pthread_mutex_destroy(&robot.lock);


    /* Destroy thread attributes */
    pthread_attr_destroy(&motor_attr);

    pthread_attr_destroy(&sensor_attr);

    pthread_attr_destroy(&safety_attr);


    printf("\n");
    printf("========================================\n");
    printf("     ROBOT CONTROLLER SHUTDOWN\n");
    printf("========================================\n");


    return 0;
}
