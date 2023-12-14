
/**
 * @file imu.ino
 * @author SeanKwok (shaoxiang@m5stack.com)
 * @brief M5AtomS3 IMU Test
 * @version 0.1
 * @date 2023-12-13
 *
 *
 * @Hardwares: M5AtomS3
 * @Platform Version: Arduino M5Stack Board Manager v2.0.9
 * @Dependent Library:
 * M5GFX: https://github.com/m5stack/M5GFX
 * M5Unified: https://github.com/m5stack/M5Unified
 * M5AtomS3: https://github.com/m5stack/M5AtomS3
 */

#include <M5Unified.h>

void setup(void) {
    auto cfg = M5.config();
    M5.begin(cfg);
}

void loop(void) {
    auto imu_update = M5.Imu.update();
    if (imu_update) {
        M5.Lcd.setCursor(0, 40);
        M5.Lcd.clear();  // Delay 100ms 延迟100ms

        auto data = M5.Imu.getImuData();

        // The data obtained by getImuData can be used as follows.
        data.accel.x;      // accel x-axis value.
        data.accel.y;      // accel y-axis value.
        data.accel.z;      // accel z-axis value.
        data.accel.value;  // accel 3values array [0]=x / [1]=y / [2]=z.

        data.gyro.x;      // gyro x-axis value.
        data.gyro.y;      // gyro y-axis value.
        data.gyro.z;      // gyro z-axis value.
        data.gyro.value;  // gyro 3values array [0]=x / [1]=y / [2]=z.

        data.value;  // all sensor 9values array [0~2]=accel / [3~5]=gyro /
                     // [6~8]=mag

        Serial.printf("ax:%f  ay:%f  az:%f\r\n", data.accel.x, data.accel.y,
                      data.accel.z);
        Serial.printf("gx:%f  gy:%f  gz:%f\r\n", data.gyro.x, data.gyro.y,
                      data.gyro.z);

        M5.Lcd.printf("IMU:\r\n");
        M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", data.accel.x, data.accel.y,
                      data.accel.z);
        M5.Lcd.printf("%0.2f %0.2f %0.2f\r\n", data.gyro.x, data.gyro.y,
                      data.gyro.z);
    }
    delay(100);
}
