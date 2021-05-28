# 3615 Queue d'âne

Some kind of *retro-futuristic smart home* for the [minitel](https://en.wikipedia.org/wiki/Minitel).

## Errors on build

If the build fails because of:

### expected ')' before numeric constant

```bash
~/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/lwip/lwip/ip4_addr.h:79:37: error: expected ')' before numeric constant
#define IPADDR_NONE         ((u32_t)0xffffffffUL)
```

edit file `~/.platformio/packages/framework-arduinoespressif32/cores/esp32/IPAddress.h` and comment out the line `const IPAddress INADDR_NONE(0, 0, 0, 0);`

### undefined reference to `ssh_mutex_lock`

```bash
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(init.c.o):(.literal._ssh_init+0xc): undefined reference to `ssh_mutex_lock'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(init.c.o):(.literal._ssh_init+0x10): undefined reference to `ssh_mutex_unlock'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(init.c.o): In function `_ssh_init':
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:155: undefined reference to `ssh_mutex_lock'
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:155: undefined reference to `ssh_mutex_unlock'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(init.c.o): In function `_ssh_finalize':
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:155: undefined reference to `ssh_mutex_lock'
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:155: undefined reference to `ssh_mutex_unlock'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(init.c.o): In function `is_ssh_initialized':
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:276: undefined reference to `ssh_mutex_lock'
.pio\libdeps\esp32dev\LibSSH-ESP32\src/init.c:278: undefined reference to `ssh_mutex_unlock'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(threads.c.o):(.literal.ssh_threads_init+0x8): undefined reference to `ssh_threads_get_default'
.pio\build\esp32dev\lib9d9\libLibSSH-ESP32.a(threads.c.o): In function `ssh_threads_init':
.pio\libdeps\esp32dev\LibSSH-ESP32\src/threads.c:85: undefined reference to `ssh_threads_get_default'
```

edit file `.pio\libdeps\esp32dev\LibSSH-ESP32\src\threads\noop.c` and change line

```C
#if !(HAVE_PTHREAD) && !(defined _WIN32 || defined _WIN64)
```

to

```C
#if !(HAVE_PTHREAD) && !(defined _WIN32 || defined _WIN64) || 1
```

## GETTING STARTED

On first run, you can set the default parameters by uncommenting the block

```C++
// preferences.putString("openWeatherKey", "xxxxxxxxxxxxxxxxxxxxxxxxx");
// preferences.putFloat("locationLat", 0.0000);
// preferences.putFloat("locationLon", 0.0000);
```

## TODO

- Add a smart home page (probably an MQTT client?)
- Add a settings page (for now, to set the API keys)
