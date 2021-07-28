import drv8833
import time

motor1 = drv8833.Motor(32, 33)
motor2 = drv8833.Motor(27, 14)
motor3 = drv8833.Motor(12, 13)
motor4 = drv8833.Motor(15, 2)

while True:
    print("motor 1 vorwärts")
    motor1.drive(100)
    time.sleep(10)

    print("motor 1 stop")
    motor1.stop()
    time.sleep(10)

    print("motor 1 rückwärts")
    motor1.drive(-100)
    time.sleep(10)

    print("motor 1 stop")
    motor1.stop()
    time.sleep(10)


    print("motor 2 vorwärts")
    motor2.drive(100)
    time.sleep(10)

    print("motor 2 stop")
    motor2.stop()
    time.sleep(10)

    print("motor 2 rückwärts")
    motor2.drive(-100)
    time.sleep(10)

    print("motor 2 stop")
    motor2.stop()
    time.sleep(10)


    print("motor 3 vorwärts")
    motor3.drive(100)
    time.sleep(10)

    print("motor 3 stop")
    motor3.stop()
    time.sleep(10)

    print("motor 3 rückwärts")
    motor3.drive(-100)
    time.sleep(10)

    print("motor 3 stop")
    motor3.stop()
    time.sleep(10)


    print("motor 4 vorwärts")
    motor4.drive(100)
    time.sleep(10)

    print("motor 4 stop")
    motor4.drive(0)
    time.sleep(10)

    print("motor 4 rückwärts")
    motor4.drive(-100)
    time.sleep(10)

    print("motor 4 stop")
    motor4.drive(0)
    time.sleep(10)
