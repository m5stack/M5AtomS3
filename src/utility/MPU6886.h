#ifndef __MPU6886_H__
#define __MPU6886_H__

#include <Wire.h>

#define MPU6886_DEFAULT_ADDRESS 0x68

class MPU6886 {
   public:
    MPU6886(uint8_t deviceAddress = MPU6886_DEFAULT_ADDRESS,
            TwoWire& i2cPort      = Wire1);

    int begin(void);

    uint8_t whoAmI();

    void getAccel(float* ax, float* ay, float* az);
    void getGyro(float* gx, float* gy, float* gz);
    void getTemp(float* t);

   private:
    uint8_t readByte(uint8_t address);
    void writeByte(uint8_t address, uint8_t data);
    void bitOn(uint8_t address, uint8_t bit);
    void bitOff(uint8_t address, uint8_t bit);

    TwoWire* _i2cPort;
    int _deviceAddress;
};

#endif
