# 🐾 QDD 기반 3DOF 사족보행 로봇

> MIT Mini Cheetah 구조를 참고한 BLDC 기반 QDD 사족보행 로봇 개발 프로젝트  
> ROS2 + SimpleFOC + ESP-NOW 기반 분산 제어 시스템

---

## 1. 프로젝트 개요

### 🎯 목표

BLDC 모터 기반 QDD(Quasi Direct Drive) 액추에이터를 직접 설계하여 3DOF 다리를 가진 사족보행 로봇을 제작한다.

### 주요 목표

- QDD 액추에이터 직접 설계 및 제작
- ESP32 기반 FOC 제어 구현 (SimpleFOC 사용)
- ROS2 기반 상위 제어 시스템 구성
- IMU / LiDAR / Camera 통합
- 무선 분산 제어 아키텍처 구현 (ESP-NOW)
- 1차 프로토타입 예산: 약 50만원

---

## 2. 시스템 아키텍처

```
[ ROS2 – Raspberry Pi 5 ]
        ↓ (Broadcast 200~500Hz)
        ESP-NOW
        ↓
[ ESP32 Motor Driver ]
    – SimpleFOC
    – SPI Magnetic Encoder (AS5047/5048)
    – Current Sensing
        ↓
  BLDC Motor + Belt Reduction (QDD)
```

---

## 3. 기계 설계

### 다리 구조 (3 DOF per leg)

각 다리는 다음 3개 관절로 구성:

| 관절 | 이름 |
|------|------|
| HAA | Hip Abduction/Adduction |
| HFE | Hip Flexion/Extension |
| KFE | Knee Flexion/Extension |

총 4개 다리 → **12개 액추에이터**

---

## 4. QDD 액추에이터 설계

### 구성 요소

- BLDC Motor
- 자기식 SPI 엔코더 (AS5047/5048 계열)
- 벨트 감속기 (6~9:1)
- ESP32 기반 FOC 드라이버

### 목표 스펙

| 항목 | 목표값 |
|------|--------|
| 연속 토크 | 3~6 N·m |
| 피크 토크 | 10~15 N·m |
| 출력 속도 | 20~40 rad/s |
| 감속비 | 6~9:1 |

---

## 5. 전기 및 제어 설계

### 제어 주기 목표

| 루프 | 주기 |
|------|------|
| FOC 루프 | ~20kHz |
| 토크/위치 루프 | 1kHz |
| 통신 주기 | 200~500Hz |

---

## 6. ROS2 소프트웨어 구조

### 주요 노드

1. `motor_bridge_node` – ESP32 ↔ ROS2 브리지 (ESP-NOW)
2. `robot_state_publisher` – 관절 상태 퍼블리시
3. `gait_controller` – 보행 패턴 생성 (초기: 패턴 기반 트로트)

### 디렉토리 구조

```
ros2_ws/
└── src/
    ├── motor_bridge_pkg/
    ├── gait_controller_pkg/
    └── robot_description_pkg/
```

---

## 7. 센서 구성

### IMU
- 자세 안정화
- 보행 균형 제어

### LiDAR
- 2D SLAM 기반 자율 이동

### Camera
- 시각 인식 및 확장 기능

---

## 8. 개발 로드맵

### 1단계 – 액추에이터 1개 완성
- [ ] SPI 엔코더 안정화
- [ ] FOC 제어 성공
- [ ] 위치 제어 안정화
- [ ] 백드라이브 특성 확인

### 2단계 – 단일 다리
- [ ] 3DOF 통합 테스트

### 3단계 – 4족 통합
- [ ] 정적 자세 유지
- [ ] 제자리 트로트 구현

### 4단계 – 센서 통합
- [ ] IMU 기반 안정화
- [ ] SLAM 실험

---

## 9. 향후 확장

- 임피던스 제어 고도화
- 모델 기반 제어 (MPC)
- 3D 지형 인식
- 강화학습 기반 보행

---

## 디렉토리 구조

```
qdd-quadruped/
├── README.md
├── docs/                        # 설계 문서
├── firmware/
│   └── esp32/
│       ├── motor_driver/        # FOC 메인 드라이버
│       ├── encoder_test/        # AS5047/5048 테스트
│       └── comm_test/           # ESP-NOW 통신 테스트
├── ros2_ws/
│   └── src/
│       ├── motor_bridge_pkg/
│       ├── gait_controller_pkg/
│       └── robot_description_pkg/
├── hardware/
│   ├── cad/                     # 기구 설계 파일
│   ├── pcb/                     # PCB 설계 파일
│   └── bom/                     # BOM (부품 목록)
└── scripts/                     # 유틸리티 스크립트
```

---

## Author

Mechanical Engineering Student  
QDD Quadruped Project
