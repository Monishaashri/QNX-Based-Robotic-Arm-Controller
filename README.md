# QNX-Based Robotic Arm Controller

## 📌 Overview

The **QNX-Based Robotic Arm Controller** is a software-based robotic arm simulation developed using **QNX Neutrino RTOS and C**.

The project demonstrates key embedded and QNX concepts including **multithreading, mutex synchronization, POSIX message queues, thread priorities, sensor monitoring, fault detection, and emergency-stop handling**.

The robotic arm consists of five software-simulated joints with position, velocity, temperature, current, and fault parameters.

---

## 🎯 Objectives

* Develop a robotic arm controller using QNX RTOS.
* Implement multiple concurrent threads.
* Synchronize shared data using mutexes.
* Implement IPC using POSIX message queues.
* Configure thread priorities.
* Simulate motor and sensor behavior.
* Implement safety monitoring.
* Detect simulated motor faults.
* Activate an emergency stop during unsafe conditions.

---

## 🏗️ System Architecture

```text
                         QNX RTOS
                            |
        +-------------------+-------------------+
        |                   |                   |
        ↓                   ↓                   ↓
 Motor Control         Sensor Thread       Safety Thread
    Thread                  |                   |
        |                   |                   |
        +-------------------+-------------------+
                            |
                     Shared Robot State
                            |
                          Mutex
                            |
                            ↓
                    Motor Server Thread
                            ↑
                            |
                   POSIX Message Queue
                            ↑
                            |
                     Main Controller
```

---

## ⚙️ Features

### 1. Five-Joint Robotic Arm

The software model contains:

| Joint | Function |
| ----- | -------- |
| J1    | Base     |
| J2    | Shoulder |
| J3    | Elbow    |
| J4    | Wrist    |
| J5    | Gripper  |

Each joint contains:

* Position
* Target position
* Velocity
* Temperature
* Current
* Enable status
* Fault status

### 2. Multithreading

The application uses separate threads for:

* Motor control
* Sensor monitoring
* Motor command processing
* Safety monitoring

### 3. Mutex Synchronization

A POSIX mutex protects the shared robotic-arm data when multiple threads access or modify it.

### 4. Message Queue IPC

Motor commands are transferred using a POSIX message queue.

Supported commands:

```text
MOVE
HOME
STOP
EXIT
```

### 5. Thread Priorities

The following priorities are configured:

| Thread        | Priority |
| ------------- | -------: |
| Safety Thread |       30 |
| Motor Thread  |       25 |
| Sensor Thread |       20 |
| Motor Server  |  Default |

### 6. Safety Monitoring

The safety thread monitors:

* Joint position
* Motor current
* Temperature
* Fault status

Configured limits:

```text
Maximum Position : 180°
Minimum Position : 0°
Maximum Current  : 5 A
Maximum Temperature : 80°C
```

---

## 🚨 Fault Detection and Emergency Stop

A software-based fault is injected into Joint 3 by simulating an over-current condition:

```text
J3 Current = 6.00 A
Maximum Allowed = 5.00 A
```

The safety thread detects the fault and activates the emergency stop.

```text
Over-Current
     ↓
Fault Detection
     ↓
Safety Fault
     ↓
Emergency Stop
     ↓
Robot Operation Stopped
```

Example output:

```text
[SAFETY] *** TEST FAULT INJECTION ***
[SAFETY] Simulating J3 motor over-current.
[SAFETY] FAULT: J3 over-current.
[SAFETY] J3 current = 6.00 A
[SAFETY] !!! SAFETY FAULT DETECTED !!!
[SAFETY] Emergency stop activated.
```

Final status:

```text
Emergency Stop  : ACTIVE
Safety Fault    : DETECTED
```

---

## 📂 Project Structure

```text
QNX_Robotic_Arm/
│
├── src/
│   ├── QNX_Robotic_Arm.c
│   ├── robot.c
│   ├── robot.h
│   ├── motor.c
│   ├── sensor.c
│   ├── motor_server.c
│   ├── safety.c
│   └── command.h
│
└── common.mk
```

### File Description

| File                | Purpose                                    |
| ------------------- | ------------------------------------------ |
| `QNX_Robotic_Arm.c` | Main controller and thread creation        |
| `robot.c`           | Robot initialization and status display    |
| `robot.h`           | Robot structures and function declarations |
| `motor.c`           | Simulated motor control                    |
| `sensor.c`          | Simulated sensor monitoring                |
| `motor_server.c`    | Message queue command processing           |
| `safety.c`          | Safety monitoring and fault detection      |
| `command.h`         | Motor command definitions                  |
| `common.mk`         | QNX project build configuration            |

---

## 🛠️ Technologies Used

* **QNX Neutrino RTOS**
* **C**
* **QNX Momentics IDE**
* **POSIX Threads**
* **POSIX Mutex**
* **POSIX Message Queues**
* **Thread Scheduling**
* **Software Motor Simulation**
* **Software Sensor Simulation**

---

## ▶️ Build and Run

1. Open the project in **QNX Momentics IDE**.
2. Select the configured QNX target.
3. Build the project:

```text
Ctrl + B
```

4. Run the application on the QNX target.
5. Observe the terminal output for:

   * Thread creation
   * Motor commands
   * Message queue communication
   * Sensor monitoring
   * Safety monitoring
   * Fault detection
   * Emergency-stop activation

---

## 📊 Demonstrated QNX Concepts

This project demonstrates practical implementation of:

* QNX RTOS application development
* POSIX multithreading
* Mutex synchronization
* Message queue IPC
* Thread priority configuration
* Shared resource protection
* Periodic monitoring
* Fault detection
* Emergency-stop handling

---

## 🔮 Future Enhancements

* Interactive terminal-based robot control
* Graphical robotic-arm visualization
* CAN communication
* Physical motor integration
* Real sensor integration
* Data logging
* Watchdog monitoring
* Additional fault conditions
* Hardware-in-the-loop testing

---

## 📌 Project Status

**Current Status: Completed up to safety monitoring and fault-injection implementation.**

This project is a **software-based robotic arm simulation** developed to demonstrate QNX RTOS and embedded-system programming concepts.

---

## 👩‍💻 Author

**Monishaa Shri S**

**Domain:** Embedded Systems | QNX RTOS | C | Electronics
