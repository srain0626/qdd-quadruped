/**
 * QDD Motor Driver – Main
 *
 * ESP32 + SimpleFOC + AS5047P (SPI)
 * ESP-NOW 수신 → 토크/위치 제어 → 상태 송신
 */

#include <Arduino.h>
#include <SimpleFOC.h>
#include <esp_now.h>
#include <WiFi.h>

// ── 핀 설정 ──────────────────────────────────────────────
#define PIN_INH_A   25
#define PIN_INH_B   26
#define PIN_INH_C   27
#define PIN_IN_A    32
#define PIN_IN_B    33
#define PIN_IN_C    14

#define SPI_MISO    19
#define SPI_MOSI    23
#define SPI_SCK     18
#define SPI_CS      5

#define MOTOR_ID    0   // 0~11: 각 모터 고유 ID 설정

// ── 통신 구조체 ──────────────────────────────────────────
typedef struct {
    uint8_t  motor_id;
    uint8_t  control_mode;   // 0=TORQUE, 1=VELOCITY, 2=POSITION
    float    target;
    float    kp;
    float    kd;
} motor_cmd_t;

typedef struct {
    uint8_t  motor_id;
    float    position;       // rad
    float    velocity;       // rad/s
    float    torque;         // N·m (추정)
    uint8_t  temperature;
} motor_state_t;

// ── SimpleFOC 객체 ───────────────────────────────────────
MagneticSensorSPI encoder(SPI_CS, 14, 0x3FFF);  // AS5047P: 14bit
BLDCMotor motor(7);  // pole pairs – 모터에 맞게 수정
BLDCDriver3PWM driver(PIN_IN_A, PIN_IN_B, PIN_IN_C,
                      PIN_INH_A, PIN_INH_B, PIN_INH_C);

motor_cmd_t   rx_cmd;
motor_state_t tx_state;

// ── ESP-NOW 콜백 ─────────────────────────────────────────
void on_data_recv(const uint8_t *mac, const uint8_t *data, int len) {
    if (len != sizeof(motor_cmd_t)) return;
    memcpy(&rx_cmd, data, sizeof(motor_cmd_t));
    if (rx_cmd.motor_id != MOTOR_ID) return;

    motor.PID_velocity.P = rx_cmd.kp;
    motor.PID_velocity.D = rx_cmd.kd;
    motor.target = rx_cmd.target;
}

void setup() {
    Serial.begin(115200);

    // SPI 엔코더 초기화
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_CS);
    encoder.init();
    motor.linkSensor(&encoder);

    // 드라이버 초기화
    driver.voltage_power_supply = 24.0f;
    driver.init();
    motor.linkDriver(&driver);

    // FOC 설정
    motor.foc_modulation = FOCModulationType::SpaceVectorPWM;
    motor.controller = MotionControlType::torque;
    motor.voltage_limit = 12.0f;
    motor.init();
    motor.initFOC();

    // ESP-NOW 초기화
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_recv_cb(on_data_recv);

    Serial.printf("[Motor %d] Ready\n", MOTOR_ID);
}

void loop() {
    motor.loopFOC();   // ~20kHz FOC 루프
    motor.move();

    // 상태 브로드캐스트 (별도 타이머로 분리 권장)
    tx_state.motor_id  = MOTOR_ID;
    tx_state.position  = encoder.getAngle();
    tx_state.velocity  = encoder.getVelocity();
    tx_state.torque    = motor.current.q * 0.0f;  // kt 값 보정 필요
}
