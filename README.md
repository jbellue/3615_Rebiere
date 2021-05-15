# 3615 Queue d'âne

Some kind of *retro-futuristic smart home* for the [minitel](https://en.wikipedia.org/wiki/Minitel).

## Errors on build

If the build fails because of:

```bash
~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/lwip/lwip/ip4_addr.h:79:37: error: expected ')' before numeric constant
#define IPADDR_NONE         ((u32_t)0xffffffffUL)
```

edit file `~/.platformio/packages/framework-arduinoespressif32/cores/esp32/IPAddress.h` and comment out the line `const IPAddress INADDR_NONE(0, 0, 0, 0);`

## TODO

- Add some error mechanism to have errors bubble up in clients (weatherClient and SSHClient so far)
- Allow setting latitude and longitude in weather page
- Add a smart home page (probably an MQTT client?)
