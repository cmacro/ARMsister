/**
  ********************************************************************
  *
  * @file     serial.h
  * @author   fpack
  * @version  v1.0
  * @date     2014-9-1
  * @brief    小穆妹纸串口调试
  *
  ********************************************************************
  **/


#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <stdio.h>
#include <stdint.h>

#ifdef __cplusplus
  extern "C" {
#endif

void serial_init(void);
void serial_send(uint8_t d);


#ifdef __cplusplus    
  }
#endif

#endif   
