import mpr121
import machine
import time

i2c = machine.I2C(1, scl=machine.Pin(22), sda=machine.Pin(21), freq=400000)
touch = mpr121.MPR121(i2c)
touch.reset()

while True:
    print(touch.is_touched(0))
    time.sleep_ms(100)
