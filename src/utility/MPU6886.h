#ifndef __MPU6886_H__
#define __MPU6886_H__

#include <Wire.h>

#define MPU6886_DEFAULT_ADDRESS 0x68
#define MPU6886_GYRO_CONFIG     0x1B
#define MPU6886_ACCEL_CONFIG    0x1C

class MPU6886 {
   public:
    enum Ascale { AFS_2G = 0, AFS_4G, AFS_8G, AFS_16G };
    enum Gscale { GFS_250DPS = 0, GFS_500DPS, GFS_1000DPS, GFS_2000DPS };

    MPU6886(uint8_t deviceAddress = MPU6886_DEFAULT_ADDRESS,
            TwoWire& i2cPort      = Wire1);

    int begin(void);

    uint8_t whoAmI();

    void getAccel(float* ax, float* ay, float* az);
    void getGyro(float* gx, float* gy, float* gz);
    void getTemp(float* t);

    void setGyroFsr(Gscale scale);
    void setAccelFsr(Ascale scale);

   private:
    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t data);
    void bitOn(uint8_t address, uint8_t bit);
    void bitOff(uint8_t address, uint8_t bit);

    TwoWire* _i2cPort;
    int _deviceAddress;

    Gscale _gyroScale;
    float _gRes;
    Ascale _accelScale;
    float _aRes;
};

#endif
