#ifndef ROBOT_H
#define ROBOT_H

#include <pthread.h>

#define NUM_JOINTS 5

/* Safety limits */
#define MAX_JOINT_POSITION 180.0f
#define MIN_JOINT_POSITION 0.0f
#define MAX_TEMPERATURE 80.0f
#define MAX_CURRENT 5.0f


/* Joint structure */
typedef struct
{
    int id;
    float position;
    float target_position;
    float velocity;
    float temperature;
    float current;
    int enabled;
    int fault;

} Joint;


/* Robotic arm structure */
typedef struct
{
    Joint joints[NUM_JOINTS];

    int system_running;
    int emergency_stop;
    int safety_fault;

    pthread_mutex_t lock;

} RobotArm;


/* Robot functions */
void robot_init(RobotArm *robot);
void robot_print_status(RobotArm *robot);


/* Thread functions */
void *motor_control_thread(void *arg);
void *sensor_thread(void *arg);
void *motor_server_thread(void *arg);
void *safety_thread(void *arg);

#endif
