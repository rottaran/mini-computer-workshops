import ustruct
import time


class PCA9685:
    def __init__(self, i2c, address=0x40):
        self.i2c = i2c
        self.address = address
        self.reset()

    def _write(self, address, value):
        self.i2c.writeto_mem(self.address, address, bytearray([value]))

    def _read(self, address):
        return self.i2c.readfrom_mem(self.address, address, 1)[0]

    def reset(self):
        self._write(0x00, 0x00) # Mode1

    def freq(self, freq=None):
        if freq is None:
            return int(25000000.0 / 4096 / (self._read(0xfe) - 0.5))
        prescale = int(25000000.0 / 4096.0 / freq + 0.5)
        old_mode = self._read(0x00) # Mode 1
        self._write(0x00, (old_mode & 0x7F) | 0x10) # Mode 1, sleep
        self._write(0xfe, prescale) # Prescale
        self._write(0x00, old_mode) # Mode 1
        time.sleep_us(5)
        self._write(0x00, old_mode | 0xa1) # Mode 1, autoincrement on

    def pwm(self, index, on=None, off=None):
        if on is None or off is None:
            data = self.i2c.readfrom_mem(self.address, 0x06 + 4 * index, 4)
            return ustruct.unpack('<HH', data)
        data = ustruct.pack('<HH', on, off)
        self.i2c.writeto_mem(self.address, 0x06 + 4 * index,  data)

    def duty(self, index, value=None, invert=False):
        if value is None:
            pwm = self.pwm(index)
            if pwm == (0, 4096):
                value = 0
            elif pwm == (4096, 0):
                value = 4095
            value = pwm[1]
            if invert:
                value = 4095 - value
            return value
        if not 0 <= value <= 4095:
            raise ValueError("Out of range")
        if invert:
            value = 4095 - value
        if value == 0:
            self.pwm(index, 0, 4096)
        elif value == 4095:
            self.pwm(index, 4096, 0)
        else:
            self.pwm(index, 0, value)

import math


class Servo:
    def __init__(self, pca, index, min_us=600, max_us=2400, degrees=180):
        self.freq = 60
        self.period = 1000000 / self.freq
        self.min_duty = self._us2duty(min_us)
        self.max_duty = self._us2duty(max_us)
        self.degrees = degrees
        self.pca9685 = pca
        self.index = index
        self.pca9685.freq(self.freq)

    def _us2duty(self, value):
        return int(4095 * value / self.period)

    def position(self, degrees=None, radians=None, us=None, duty=None):
        span = self.max_duty - self.min_duty
        if degrees is not None:
            duty = self.min_duty + span * degrees / self.degrees
        elif radians is not None:
            duty = self.min_duty + span * radians / math.radians(self.degrees)
        elif us is not None:
            duty = self._us2duty(us)
        elif duty is not None:
            pass
        else:
            return self.pca9685.duty(self.index, 0)
        duty = min(self.max_duty, max(self.min_duty, int(duty)))
        self.pca9685.duty(self.index, duty)

    def release(self):
        self.pca9685.duty(self.index, 0)

class DRV8833:
    def __init__(self, pca, a1, a2):
        self.pca = pca
        self.a1 = a1
        self.a2 = a2
        self.pca.freq(1526)
        self.max = 4095
        self.min = 0

    def drive(self, power):
        span = self.max - self.min
        if power < 0:
            p = self.min + (-power)*span/100
            self.pca.duty(self.a1, 0)
            self.pca.duty(self.a2, int(p))            
        elif power > 0:
            p = self.min + power*span/100
            self.pca.duty(self.a1, int(p))
            self.pca.duty(self.a2, 0)
        else:
            self.pca.duty(self.a1, 4095)
            self.pca.duty(self.a2, 4095)

    def stop():
        self.pca.duty(self.a1, 4095)
        self.pca.duty(self.a2, 4095)
