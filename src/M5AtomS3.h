#ifndef _M5_ATOM_S3_H_
#define _M5_ATOM_S3_H_

#include "M5Unified.h"
#include "M5GFX.h"
#include "./utility/LedDisplay.h"

namespace m5 {
class M5AtomS3 {
   private:
    bool _led_enable;
    /* data */
   public:
    void begin(bool ledEnable = false);
    void begin(m5::M5Unified::config_t cfg, bool ledEnable = false);

    M5GFX &Display = M5.Display;
    M5GFX &Lcd     = Display;

    IMU_Class &Imu     = M5.Imu;
    Power_Class &Power = M5.Power;
    Button_Class &BtnA = M5.BtnA;

    /// for internal I2C device
    I2C_Class &In_I2C = m5::In_I2C;

    /// for external I2C device (Port.A)
    I2C_Class &Ex_I2C = m5::Ex_I2C;

    LedDisplay dis;

    void update(void);
};
}  // namespace m5

extern m5::M5AtomS3 AtomS3;

#endif
