#include "MPU6886.h"

MPU6886::MPU6886(uint8_t deviceAddress, TwoWire& i2cPort) :
        _gyroScale(GFS_250DPS), _gRes(250.0 / 32768.0),
        _accelScale(AFS_2G), _aRes(2.0 / 32768.0) {
    _deviceAddress = deviceAddress;
    _i2cPort       = &i2cPort;
}

uint8_t MPU6886::readByte(uint8_t address) {
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(address);
    _i2cPort->endTransmission();
    _i2cPort->requestFrom(_deviceAddress, 1);
    uint8_t val = _i2cPort->read();

    ESP_LOGD("MPU6886", "readByte(%02X) = %02X", address, val);
    return val;
}

void MPU6886::writeByte(uint8_t address, uint8_t data) {
    _i2cPort->beginTransmission(_deviceAddress);
    _i2cPort->write(address);
    _i2cPort->write(data);
    _i2cPort->endTransmission();
    ESP_LOGD("MPU6886", "writeByte(%02X) = %02X", address, data);
}

void MPU6886::bitOn(uint8_t address, uint8_t bit) {
    uint8_t add = address;
    uint8_t val = readByte(add) | bit;
    writeByte(add, val);
}

void MPU6886::bitOff(uint8_t address, uint8_t bit) {
    uint8_t add = address;
    uint8_t val = readByte(add) & ~bit;
    writeByte(add, val);
}

int MPU6886::begin(void) {
    // WHO_AM_I : IMU Check
    if (whoAmI() != 0x19) {
        return -1;
    }
    delay(1);

    // PWR_MGMT_1(0x6b)
    writeByte(0x6b, 0x00);
    delay(10);

    // PWR_MGMT_1(0x6b)
    writeByte(0x6b, 1 << 7);
    delay(10);

    // PWR_MGMT_1(0x6b)
    writeByte(0x6b, 1 << 0);
    delay(10);

    setAccelFsr(AFS_8G);
    setGyroFsr(GFS_2000DPS);

    // CONFIG(0x1a)
    writeByte(0x1a, 0x01);
    delay(1);

    // SMPLRT_DIV(0x19)
    writeByte(0x19, 0x05);
    delay(1);

    // INT_ENABLE(0x38)
    writeByte(0x38, 0x00);
    delay(1);

    // ACCEL_CONFIG 2(0x1d)
    writeByte(0x1d, 0x00);
    delay(1);

    // USER_CTRL(0x6a)
    writeByte(0x6a, 0x00);
    delay(1);

    // FIFO_EN(0x23)
    writeByte(0x23, 0x00);
    delay(1);

    // INT_PIN_CFG(0x37)
    writeByte(0x37, 0x22);
    delay(1);

    // INT_ENABLE(0x38)
    writeByte(0x38, 0x01);
    delay(100);

    return 0;
}

uint8_t MPU6886::whoAmI(void) {
    return readByte(0x75);
}

void MPU6886::setGyroFsr(Gscale scale) {
    unsigned char regdata;
    regdata = (scale << 3);
    writeByte(MPU6886_GYRO_CONFIG, regdata);
    delay(1);
    _gyroScale = scale;
    switch (_gyroScale) {
        case GFS_250DPS:
            _gRes = 250.0 / 32768.0;
            break;
        case GFS_500DPS:
            _gRes = 500.0 / 32768.0;
            break;
        case GFS_1000DPS:
            _gRes = 1000.0 / 32768.0;
            break;
        case GFS_2000DPS:
            _gRes = 2000.0 / 32768.0;
            break;
    }
}

void MPU6886::setAccelFsr(Ascale scale) {
    unsigned char regdata;
    regdata = (scale << 3);
    writeByte(MPU6886_ACCEL_CONFIG, regdata);
    delay(1);
    _accelScale = scale;
    switch (_accelScale) {
        case AFS_2G:
            _aRes = 2.0 / 32768.0;
            break;
        case AFS_4G:
            _aRes = 4.0 / 32768.0;
            break;
        case AFS_8G:
            _aRes = 8.0 / 32768.0;
            break;
        case AFS_16G:
            _aRes = 16.0 / 32768.0;
            break;
    }
}

void MPU6886::getAccel(float* ax, float* ay, float* az) {
    *ax        = (int16_t)((readByte(0x3b) << 8) | readByte(0x3c)) * _aRes;
    *ay        = (int16_t)((readByte(0x3d) << 8) | readByte(0x3e)) * _aRes;
    *az        = (int16_t)((readByte(0x3f) << 8) | readByte(0x40)) * _aRes;
}

void MPU6886::getGyro(float* gx, float* gy, float* gz) {
    *gx        = (int16_t)((readByte(0x43) << 8) | readByte(0x44)) * _gRes;
    *gy        = (int16_t)((readByte(0x45) << 8) | readByte(0x46)) * _gRes;
    *gz        = (int16_t)((readByte(0x47) << 8) | readByte(0x48)) * _gRes;
}

void MPU6886::getTemp(float* t) {
    *t = 25.0 + ((readByte(0x41) << 8) | readByte(0x42)) / 326.8;
}
