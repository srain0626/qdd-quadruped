# 시스템 아키텍처

## 전체 제어 흐름

```
┌─────────────────────────────────────┐
│         Raspberry Pi 5              │
│  ┌─────────────────────────────┐    │
│  │  ROS2 Humble                │    │
│  │  - motor_bridge_node        │    │
│  │  - robot_state_publisher    │    │
│  │  - gait_controller          │    │
│  │  - IMU / LiDAR / Camera     │    │
│  └───────────────┬─────────────┘    │
└──────────────────┼──────────────────┘
                   │ ESP-NOW (200~500Hz)
        ┌──────────┴──────────┐
        │                     │
   ┌────▼────┐           ┌────▼────┐
   │ ESP32 #1│    ...    │ESP32 #12│
   │ (HAA FL)│           │ (KFE RR)│
   └────┬────┘           └────┬────┘
        │ SimpleFOC            │
   ┌────▼────┐           ┌────▼────┐
   │  BLDC   │           │  BLDC   │
   │ + Belt  │           │ + Belt  │
   └─────────┘           └─────────┘
```

## 통신 프로토콜

### ESP-NOW 패킷 구조 (예시)
```c
typedef struct {
    uint8_t  motor_id;       // 0~11
    uint8_t  control_mode;   // TORQUE=0, VELOCITY=1, POSITION=2
    float    target;         // 제어 목표값
    float    kp;
    float    kd;
} motor_cmd_t;

typedef struct {
    uint8_t  motor_id;
    float    position;       // rad
    float    velocity;       // rad/s
    float    torque;         // N·m
    uint8_t  temperature;    // °C
} motor_state_t;
```

## 노드 그래프

```
/gait_controller
    → /joint_commands (sensor_msgs/JointState)

/motor_bridge_node
    ← /joint_commands
    → /joint_states (sensor_msgs/JointState)

/robot_state_publisher
    ← /joint_states
    → /tf, /tf_static
```
