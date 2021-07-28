from machine import Pin, PWM

class Motor:
    def __init__(self, pin1, pin2, pmin=0, pmax=1023):
        self.pin1 = PWM(Pin(pin1), freq=10000)
        self.pin2 = PWM(Pin(pin2), freq=10000)
        self.min = pmin
        self.max = pmax

    def deinit(self):
        self.pin1.deinit()
        self.pin2.deinit()

    def drive(self, power):
        span = self.max - self.min
        if power < 0:
            self.pin1.duty(0)
            self.pin2.duty(int(self.min + (-power)*span/100))
        elif power > 0:
            self.pin1.duty(int(self.min + power*span/100))
            self.pin2.duty(0)
        else:
            self.pin1.duty(1023)
            self.pin2.duty(1023)

    def stop(self):
        self.drive(0)
