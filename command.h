#ifndef COMMAND_H
#define COMMAND_H

#define CMD_MOVE 1
#define CMD_STOP 2
#define CMD_HOME 3
#define CMD_EXIT 4

typedef struct
{
    int command;
    int joint_id;
    float target_position;
} MotorCommand;

#endif
