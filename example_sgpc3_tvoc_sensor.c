#include <stdlib.h>
#include "example_sgpc3_tvoc_sensor.h"

/********* Constants ***********/
#define SGPC3_I2C_ADDRESS (0x58)
#define HW_SLEEP(ms) //TODO platform specific

/********* App Variables ****************/

/********* Handler Functions ****************/
static uint8_t getCRC8(uint8_t *data, uint8_t len)
{
    /**
     * SPEC 6.6:
     * Width 8 bit
     * Polynomial 0x31 (x8 + x5 + x4 + 1)
     * Initialization 0xff
     * Final XOR 0x00
    **/
    const uint8_t polynomial = 0x31;
    const uint8_t initialization = 0xff;
    uint8_t crc = initialization;
    for (uint8_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
            crc = (crc & 0x80) ? (crc << 1) ^ polynomial : crc << 1;
    }
    return crc;
}

/********* Public Functions ****************/
SPR_Status startSensor(void)
{
    Debug.printline("startSensor: waiting for sensor ...");
    while(SPR_OK != probSensor()) HW_SLEEP(100);

    Debug.printline("startSensor: configuring sensor ...");
    while(SPR_OK != configSensor(SGPC3_ULTRA_LOW_POWER, NULL, NULL)) HW_SLEEP(100);

    Debug.printline("startSensor: sensor started");
    return SPR_OK;
}

SPR_Status probSensor(void)
{
    /**
    * SPEC 6.5: Get Serial ID
    **/
    uint16_t serialId = 0;
    SPR_Status status = readSensor(SGPC3_GET_SERIAL_ID, &serialId);
#if defined(SGPC3_SERIAL_ID)
    if (SPR_OK == status && serialId != SGPC3_SERIAL_ID) {
        Debug.printline("Error: SGPC3_SERIAL_ID(%d) not found", SGPC3_SERIAL_ID);
        status = SPR_FAIL;
    }
#endif
    Debug.printline("probSensor status=%x, serialId=%d", status, serialId);

#if defined(EXTRA_FEATURES)
    // SPEC 6.3 Feature Set
    if (SPR_OK == status) {
        SGPC3_Feature_Set_t featureSet;
        status = readSensor(SGPC3_GET_FEATURE_SET_VERSION, (uint16_t*) &featureSet);
        Debug.printline("probSensor status=%x, productType=%d, productVersion=%d",
            status, featureSet.productType, featureSet.productVersion);
    }
#endif
    return status;
}

SPR_Status configSensor(SGPC3_Power_Mode_t mode, uint16_t *baseline, uint16_t *humidity)
{
    /**
    * SPEC 6.3 Set Power Mode
    **/
    Debug.printline("configSensor: power mode (%d)", mode);
    SPR_Status status = writeSensor(SGPC3_SET_POWER_MODE, mode);

    /**
    * SPEC 6.3 Air Quality Signal
    **/
    if (SPR_OK == status) {
        uint8_t writeBuffer[2];
        Util.pack_uint16_msb(writeBuffer, 0, SGPC3_INIT_AIR_QUALITY);
        status = I2C.write(SGPC3_I2C_ADDRESS, writeBuffer, sizeof(writeBuffer));
        Debug.printline("configSensor: status=%x Init_air_quality", status);
    }

#if defined(EXTRA_FEATURES)
    /**
    * SPEC 6.3 Set and Get Baseline
    **/
    if (SPR_OK == status && baseline) {
        status = writeSensor(SGPC3_SET_BASELINE, *baseline);
        Debug.printline("configSensor: status=%x baseline (%d)", status, *baseline);
    }

    /**
    * SPEC 6.3 Humidity Compensation
    **/
    if (SPR_OK == status && humidity) {
        status = writeSensor(SGPC3_SET_HUMIDITY, *humidity);
        Debug.printline("configSensor: status=%x humidity (%d)", status, *humidity);
    }
#endif
    return status;
}

SPR_Status writeSensor(SGPC3_Cmd_t writeCmd, uint16_t data)
{
    Debug.printline("writeSensor!");

    switch(writeCmd) {
        case SGPC3_SET_POWER_MODE:
        case SGPC3_SET_BASELINE:
        case SGPC3_SET_HUMIDITY:
            break;
        default:
            Debug.printf("Error: Unsupported write command (%d)", writeCmd);
            return SPR_INVALID_PARAMETER;
    }

    uint8_t writeBuffer[5];
    Util.pack_uint16_msb(writeBuffer, 0, writeCmd);
    Util.pack_uint16_msb(writeBuffer, 2, data);
    writeBuffer[4] = getCRC8((uint8_t*) &data, sizeof(data));
    return I2C.write(SGPC3_I2C_ADDRESS, writeBuffer, sizeof(writeBuffer));
}

SPR_Status readSensor(SGPC3_Cmd_t readCmd, uint16_t *data)
{
    Debug.printline("readSensor!");

    switch(readCmd) {
        case SGPC3_MEASURE_AIR_QUALITY:
        case SGPC3_GET_BASELINE:
        case SGPC3_MEASURE_TEST:
        case SGPC3_GET_FEATURE_SET_VERSION:
        case SGPC3_MEASURE_RAW_SIGNAL:
            break;
        case SGPC3_MEASURE_RAW_SIGNAL_AND_AIR_QUALITY:
            Debug.printf("Error: SGPC3_MEASURE_RAW_SIGNAL_AND_AIR_QUALITY is not ready");
            return SPR_INVALID_PARAMETER;
        default:
            Debug.printf("Error: Unsupported read command (%d)", readCmd);
            return SPR_INVALID_PARAMETER;
    }

    if (data) {
        uint8_t writeBuffer[2];
        Util.pack_uint16_msb(writeBuffer, 0, readCmd);
        SPR_Status status = I2C.write(SGPC3_I2C_ADDRESS, writeBuffer, sizeof(writeBuffer));
        if (SPR_OK == status) {
            /**
             * SPEC 6.5:
             * After issuing the measurement command and sending the ACK Bit
             * the sensor needs the time tIDLE = 0.5ms to respond to the I2C
             * read header with an ACK Bit. Hence, it is recommended to wait
             * tIDLE =0.5ms before issuing the read header. 
            **/
            HW_SLEEP(50);

            uint8_t readBuffer[3];
            status = I2C.read(SGPC3_I2C_ADDRESS, readBuffer, sizeof(readBuffer));
            if (SPR_OK == status) {
                if (readBuffer[SGPC3_DATA_CRC] == getCRC8(readBuffer, SGPC3_DATA_SIZE)) {
                    *data = Util.unpack_uint16_msb(readBuffer, 0);
                    return SPR_OK;
                }
                Debug.printf("Error: CRC is invalid");
                return SPR_FAIL;
            }
        }
        return status;
    } else {
        Debug.printf("Error: data parameter is invalid");
        return SPR_INVALID_PARAMETER;
    }
}

#if defined(EXTRA_FEATURES)
SPR_Status resetSensor(void)
{
    /**
    * SPEC 6.4: Soft Reset
    **/
    uint8_t addressByte = 0x00;
    uint8_t writeByte = 0x06;
    // Use write command (keep bit 8 to low) to generate the "General Call" waveform
    SPR_Status status = I2C.write(addressByte, &writeByte, sizeof(writeByte));
    Debug.printline("resetSensor: status=%x", status);
    return status;
}
#endif
