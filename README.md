If the build fails because of
~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/lwip/lwip/ip4_addr.h:79:37: error: expected ')' before numeric constant
 #define IPADDR_NONE         ((u32_t)0xffffffffUL)
edit file ~/.platformio/packages/framework-arduinoespressif32/cores/esp32/IPAddress.h and comment out the line
const IPAddress INADDR_NONE(0, 0, 0, 0)