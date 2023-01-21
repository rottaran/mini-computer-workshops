# hardware

* wemos C3 mini
  * https://www.wemos.cc/en/latest/c3/c3_mini.html 
  * oder https://www.wemos.cc/en/latest/c3/c3_mini_1_0_0.html (hat wohl kleinen Designfehler mit der Antenne)
* wemos D1 motor shield
  * https://www.wemos.cc/en/latest/d1_mini_shield/motor.html (v1 hat fehlerhafte Firmware)
  * oder https://www.wemos.cc/en/latest/d1_mini_shield/hr8833_motor.html
  * oder https://www.wemos.cc/en/latest/d1_mini_shield/at8870_motor.html
  * arduino: https://github.com/wemos/LOLIN_I2C_MOTOR_Library

# wemos D1 motor shield firmware

* arduino library: https://github.com/wemos/LOLIN_I2C_MOTOR_Library und https://github.com/wemos/WEMOS_Motor_Shield_Arduino_Library
* anleitung firmware: https://oberguru.net/elektronik/wemos/wemos-motor-shield.html
* anleitung firmware: https://espeasy.readthedocs.io/en/latest/Plugin/P079_WemosMotorshield.html
* hackaday anleitung: https://hackaday.io/project/18439-motor-shield-reprogramming
* anleitung firmware: https://medium.com/@shelladdicted/how-to-update-the-firmware-of-a-wemos-motor-shield-fadbb496fd01
* offizielle firmware: https://github.com/wemos/Motor_Shield_Firmware
* alternative firmware: https://github.com/pbugalski/wemos_motor_shield


Mit `arm-none-eabi-gcc` installiert:

```
git clone https://github.com/pbugalski/wemos_motor_shield
cd wemos_motor_shield
make
```

Mit einem USB zu Serial Adapter:
* GND ↔ GND
* 3V3 ↔ 3V3 (or VCC or whatever it is called on your USB2TTL)
* D2 ↔ TX
* D1 ↔ RX
* den RTS Pin mit dem 3.3V Pin daneben verbinden (this keeps the stm32 in update-mode)

```
stm32flash /dev/ttyUSB0
# unlock
stm32flash /dev/ttyUSB0 -k
# flash
stm32flash /dev/ttyUSB0 -v -w motor_shield.bin
```

# wemos C3 mini mit micropython

* https://micropython.org/download/LOLIN_C3_MINI/
* https://thonny.org/

```
pip install esptool
python -m esptool --chip esp32c3 --port COM5 erase_flash
python -m esptool --chip esp32c3 --port COM5 --baud 460800 write_flash -z 0x0 LOLIN_C3_MINI-20220618-v1.19.1.bin
```

# wemos D1 mit micropython ansteuern

```
from machine import I2C
i2c = I2C(0)
i2c.scan() # liefert 48 (0x30) für den wemos D1 motor treiber
i2c.writeto(48, b'\x00\x00\x03\xE8')
i2c.writeto(48, b'\x10\x01\x27\x10')
```

