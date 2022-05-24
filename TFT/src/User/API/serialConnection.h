#ifndef _SERIAL_CONNECTION_H_
#define _SERIAL_CONNECTION_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BAUDRATE_COUNT 9

typedef enum
{
  ALL_PORTS = -1,
  PORT_1 = 0,
  #ifdef SERIAL_PORT_2
    PORT_2,
  #endif
  #ifdef SERIAL_PORT_3
    PORT_3,
  #endif
  #ifdef SERIAL_PORT_4
    PORT_4,
  #endif
  SERIAL_PORT_COUNT
} SERIAL_PORT_INDEX;  // serial port index for all enabled serial ports (This is not actual physical port number)

extern const uint32_t baudrateList[BAUDRATE_COUNT];

void Serial_ReSourceDeInit(void);
void Serial_ReSourceInit(void);

#ifdef __cplusplus
}
#endif

#endif
