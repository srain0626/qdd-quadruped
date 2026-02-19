/**
 * AS5047P SPI 엔코더 기본 테스트
 * 각도(deg) 및 각속도(rad/s) 시리얼 출력
 */

#include <Arduino.h>
#include <SimpleFOC.h>

#define SPI_CS 5

MagneticSensorSPI encoder(SPI_CS, 14, 0x3FFF);

void setup() {
    Serial.begin(115200);
    SPI.begin(18, 19, 23, SPI_CS);  // SCK, MISO, MOSI, CS
    encoder.init();
    Serial.println("AS5047P encoder test start");
}

void loop() {
    encoder.update();
    Serial.printf("Angle: %.2f deg | Velocity: %.2f rad/s\n",
                  encoder.getAngle() * 180.0f / PI,
                  encoder.getVelocity());
    delay(10);
}
