#pragma once

#include <cstdint>
#include <cstdio>
#include <cstring>
#include "witsdk_constants.h"

namespace witsdk {

inline const int WIT_HAL_OK = (0);     /**< There is no error */
inline const int WIT_HAL_BUSY = (-1);    /**< Busy */
inline const int WIT_HAL_TIMEOUT = (-2);    /**< Timed out */
inline const int WIT_HAL_ERROR = (-3);    /**< A generic error happens */
inline const int WIT_HAL_NOMEM = (-4);    /**< No memory */
inline const int WIT_HAL_EMPTY = (-5);    /**< The resource is empty */
inline const int WIT_HAL_INVAL = (-6);    /**< Invalid argument */

inline const int WIT_DATA_BUFF_SIZE = 256;

inline const int WIT_PROTOCOL_NORMAL = 0;
inline const int WIT_PROTOCOL_MODBUS = 1;
inline const int WIT_PROTOCOL_CAN = 2;
inline const int WIT_PROTOCOL_I2C = 3;
inline const int WIT_PROTOCOL_JY61 = 4;
inline const int WIT_PROTOCOL_905x_MODBUS = 5;
inline const int WIT_PROTOCOL_905x_CAN = 6;


/* serial function */
typedef void (*SerialWrite)(uint8_t *p_ucData, uint32_t uiLen);
int32_t WitSerialWriteRegister(SerialWrite write_func);
void WitSerialDataIn(uint8_t ucData);

/* iic function */

/*
    i2c write function example

    int32_t WitI2cWrite(uint8_t ucAddr, uint8_t ucReg, uint8_t *p_ucVal, uint32_t uiLen)
    {
        i2c_start();
        i2c_send(ucAddr);
        if(i2c_wait_ask() != SUCCESS)return 0;
        i2c_send(ucReg);
        if(i2c_wait_ask() != SUCCESS)return 0;
        for(uint32_t i = 0; i < uiLen; i++)
        {
            i2c_send(*p_ucVal++);
            if(i2c_wait_ask() != SUCCESS)return 0;
        }
        i2c_stop();
        return 1;
    }
*/
typedef int32_t (*WitI2cWrite)(uint8_t ucAddr, uint8_t ucReg, uint8_t *p_ucVal, uint32_t uiLen);
/*
    i2c read function example

    int32_t WitI2cRead(uint8_t ucAddr, uint8_t ucReg, uint8_t *p_ucVal, uint32_t uiLen)
    {
        i2c_start();
        i2c_send(ucAddr);
        if(i2c_wait_ask() != SUCCESS)return 0;
        i2c_send(ucReg);
        if(i2c_wait_ask() != SUCCESS)return 0;

        i2c_start();
        i2c_send(ucAddr+1);
        for(uint32_t i = 0; i < uiLen; i++)
        {
            if(i+1 == uiLen)*p_ucVal++ = i2c_read(0);  //last byte no ask
            else *p_ucVal++ = i2c_read(1);  //  ask
        }
        i2c_stop();
        return 1;
    }
*/
typedef int32_t (*WitI2cRead)(uint8_t ucAddr, uint8_t ucReg, uint8_t *p_ucVal, uint32_t uiLen);
int32_t WitI2cFuncRegister(WitI2cWrite write_func, WitI2cRead read_func);

/* can function */
typedef void (*CanWrite)(uint8_t ucStdId, uint8_t *p_ucData, uint32_t uiLen);
int32_t WitCanWriteRegister(CanWrite write_func);

/* Delayms function */
typedef void (*DelaymsCb)(uint16_t ucMs);
int32_t WitDelayMsRegister(DelaymsCb delayms_func);


void WitCanDataIn(uint8_t ucData[8], uint8_t ucLen);


typedef void (*RegUpdateCb)(uint32_t uiReg, uint32_t uiRegNum);
int32_t WitRegisterCallBack(RegUpdateCb update_func);
int32_t WitWriteReg(uint32_t uiReg, uint16_t usData);
int32_t WitReadReg(uint32_t uiReg, uint32_t uiReadNum);
int32_t WitInit(uint32_t uiProtocol, uint8_t ucAddr);
void WitDestroy();



/**
  ******************************************************************************
  * @file    wit_c_sdk.h
  * @author  Wit
  * @version V1.0
  * @date    05-May-2022
  * @brief   This file provides all Configure sensor function.
  ******************************************************************************
  * @attention
  *
  *        http://wit-motion.cn/
  *
  ******************************************************************************
  */
int32_t WitStartAccCali();
int32_t WitStopAccCali();
int32_t WitStartMagCali();
int32_t WitStopMagCali();
int32_t WitSetUartBaud(int32_t uiBaudIndex);
int32_t WitSetBandwidth(int32_t uiBaudWidth);
int32_t WitSetOutputRate(int32_t uiRate);
int32_t WitSetContent(int32_t uiRsw);
int32_t WitSetCanBaud(int32_t uiBaudIndex);
int32_t WitSaveParameter();
int32_t WitSetForReset();
int32_t WitCaliRefAngle();

char CheckRange(short sTemp,short sMin,short sMax);

extern int16_t sReg[REGSIZE];

}