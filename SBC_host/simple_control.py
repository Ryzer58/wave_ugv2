from time import sleep
import serial
import json
import time
import sys
import tty
import termios


throttle_level = 100
throttle_min = 70
throttle_max = 250

valid_keys = {'w', 'a', 's', 'd', ',','.'}

# Initialize serial connection to SBC using the corresponding serial device

# Raspberry PI (Bluetooth should be disabled on the Pi 3 and Pi4) remember to enable in raspi-config
ser = serial.Serial('/dev/ttyAMA0', 115200)

# Pcduino 2 or 3, enable overlay in armbian-config 
# ser = serial.Serial('/dev/ttyS2', 115200)


def send_motion(mota_speed, motb_speed):
    cmd = {
        "comd":"m",
        "mota": mota_speed,
        "motb": motb_speed
    }
    json_command = json.dumps(cmd)
    json_command = json_command + '\n'
    print(json_command)
    ser.write(json_command.encode('utf-8'))

def readchar():
    fd = sys.stdin.fileno()
    old_settings = termios.tcgetattr(fd)

    try:
        tty.setraw(sys.stdin.fileno())
        ch = sys.stdin.read(1)

    finally:
        termios.tcsetattr(fd, termios.TCSADRAIN, old_settings)
    if ch == '0x03':
        raise KeyboardInterrupt
    return ch


def readKey(getchar_fn=None):
    getchar = getchar_fn or readchar
    c1 = getchar()
    if ord(c1) != 0x1b:
        return c1
    c2 = getchar()
    if ord(c2) != 0x5b:
        return c2
    c3 = getchar()
    return chr(0x10 + ord(c3) - 65) #16=Up, 17=Down, 18=Right, 19=Left arrow


try:
    while True:
        
        throttle_inv = -throttle_level
        keyp = readKey()

        if keyp == 'w' or ord(keyp) ==16:
            send_motion(throttle_level, throttle_level)
            print('Forward: ' + str(throttle_level))

        elif keyp == 's' or ord(keyp) == 17:
            send_motion(throttle_inv, throttle_inv)
            print('Reverse: ' + str(throttle_level))
            run_time = 3
            
        elif keyp == 'd' or ord(keyp) == 18:
            print('Spinning right ', end=' ') # The IMU is not yet operation so have no point of reference to spin from therefore instead we be rotating for a given point of time
            send_motion(throttle_level, throttle_inv)
            print('at' + str(throttle_level))


        elif keyp == 'a' or ord(keyp) == 19:
            print('Spinning left', end=' ')
            send_motion(throttle_level, throttle_inv)
            print('at' + str(throttle_level))
            

        elif keyp == '.' or keyp == '>':
            if throttle_level < throttle_max and throttle_level >= throttle_min:
                print('Increasing power ', end=' ')
                throttle_level = throttle_level + 10
                print('output to ' + str(throttle_level))

            elif throttle_level == 0:
                 print('Currently idle')

            else:
                print('Set to max throttle')


        elif keyp == ',' or keyp == '<':
            if throttle_level > throttle_min:
                print('Decreasing power', end=' ')
                throttle_level = throttle_level - 10
                print(' output to ' + str(throttle_level))

            elif throttle_level == 0:
                 print('Currently idle')

            else:
                print('Set to min throttle')  

except KeyboardInterrupt:
    ser.close()
    sys.exit()
