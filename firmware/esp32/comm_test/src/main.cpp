/**
 * ESP-NOW 통신 테스트 – Sender (Raspberry Pi 역할 대역)
 * motor_cmd_t 브로드캐스트 후 echo 수신 확인
 */

#include <Arduino.h>
#include <esp_now.h>
#include <WiFi.h>

typedef struct {
    uint8_t  motor_id;
    uint8_t  control_mode;
    float    target;
    float    kp;
    float    kd;
} motor_cmd_t;

uint8_t broadcast_addr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void on_sent(const uint8_t *mac, esp_now_send_status_t status) {
    Serial.printf("Send status: %s\n",
                  status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    esp_now_init();
    esp_now_register_send_cb(on_sent);

    esp_now_peer_info_t peer = {};
    memcpy(peer.peer_addr, broadcast_addr, 6);
    peer.channel = 0;
    peer.encrypt = false;
    esp_now_add_peer(&peer);

    Serial.println("ESP-NOW comm test ready");
}

void loop() {
    motor_cmd_t cmd = {0, 2, 1.57f, 10.0f, 0.1f};  // POSITION 90deg
    esp_now_send(broadcast_addr, (uint8_t *)&cmd, sizeof(cmd));
    delay(10);  // ~100Hz
}
