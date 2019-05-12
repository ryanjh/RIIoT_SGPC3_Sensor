/**
 * @file example_sgpc3_tvoc_sensor.h
 * SGPC3 Sensor Module
 */ 
#ifndef EXAMPLE_SGPC3_TVOC_SENSOR_H
#define EXAMPLE_SGPC3_TVOC_SENSOR_H

#include "spr_api.h"
#include "api_modules.h"

/********* Type Definitions ****************/
// SPEC 6.3 Table 10 Measurement commands
typedef enum {
    SGPC3_INIT_AIR_QUALITY                   = 0x20ae,
    SGPC3_MEASURE_AIR_QUALITY                = 0x2008,
    SGPC3_SET_POWER_MODE                     = 0x209f,
    SGPC3_GET_BASELINE                       = 0x2015,
    SGPC3_SET_BASELINE                       = 0x201e,
    SGPC3_SET_HUMIDITY                       = 0x2061,
    SGPC3_MEASURE_TEST                       = 0x2032,
    SGPC3_GET_FEATURE_SET_VERSION            = 0x202f,
    SGPC3_MEASURE_RAW_SIGNAL                 = 0x204d,
    SGPC3_MEASURE_RAW_SIGNAL_AND_AIR_QUALITY = 0x2046,
    SGPC3_GET_SERIAL_ID                      = 0x3682
} SGPC3_Cmd_t;

typedef enum {
    SGPC3_DATA_MSB = 0,
    SGPC3_DATA_LSB,
    SGPC3_DATA_CRC,
    SGPC3_DATA_SIZE = SGPC3_DATA_CRC
} SGPC3_Data_t;

// SPEC 6.3 Set Power Mode
typedef enum {
    SGPC3_ULTRA_LOW_POWER = 0x0000,
    SGPC3_LOW_POWER       = 0x0001
} SGPC3_Power_Mode_t;

// SPEC 6.3 Feature Set
typedef struct
{
    uint16_t productType    :4; // Product type
    uint16_t resv           :3; // Reserved for future use
    uint16_t zero           :1; // 0
    uint16_t productVersion :8; // Product version
} SGPC3_Feature_Set_t;

/********* Public Function Declarations ****************/
SPR_Status startSensor(void);
SPR_Status probSensor(void);
SPR_Status configSensor(SGPC3_Power_Mode_t mode, uint16_t *baseline, uint16_t *humidity);
SPR_Status writeSensor(SGPC3_Cmd_t writeCmd, uint16_t data);
SPR_Status readSensor(SGPC3_Cmd_t readCmd, uint16_t *data);

#if defined(EXTRA_FEATURES)
SPR_Status resetSensor(void);
#endif

#endif // EXAMPLE_SGPC3_TVOC_SENSOR_H
