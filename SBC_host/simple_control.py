#import serial
import json
import time
import keyboard

throttle_level = 100
throttle_min = 70
throttle_max = 250

valid_keys = {'w', 'a', 's', 'd', ',','.'}

# Initialize serial connection to SBC using the corresponding serial device

# Raspberry PI (Bluetooth should be disabled on the Pi 3 and Pi4) remember to enable in raspi-config
#ser = serial.Serial('/dev/ttyAMA0', 115200)

# Intialize serial communication on Pcduino, enable overlay in armbian-config 
# ser = serial.Serial('/dev/ttyS2', 115200)


def send_motion(mota_speed, motb_speed):
    cmd = {
        "comd":"m",
        "mota": mota_speed,
        "motb": motb_speed
    }
    json_command = json.dumps(cmd)
    json_command = json_command + '\0'
    print(json_command)
    #ser.write(json_command.encode('utf-8'))


def key_handler(key):
    global throttle_level, throttle_min, throttle_max
    throttle_inv = -throttle_level

    if key in valid_keys:
        if key == 'w':
            send_motion(throttle_level, throttle_level)  # Forward
            print("Moving foward")
        elif key == 'a':
            send_motion(throttle_inv, throttle_level)  # Left
            print("Turning left")
        elif key == 's':
            send_motion(throttle_inv, throttle_inv)  # Reverse
            print("Moving in reverse")
        elif key == 'd':
            send_motion(throttle_level, throttle_inv)  # Right
            print("Turning right")
        elif key == ',':
            if throttle_level > throttle_min:
                throttle_level = throttle_level - 10
                print("Slowing down")
            else:
                print("At crawl speed")
        elif key == '.':
            if throttle_level < throttle_max:
                throttle_level = throttle_level + 10
                print("Speeding up")
            else:
                print("At max speed")
    else:    
        print("Invalid key,stopping")
        send_motion(0, 0)  # Stop


def main():


    # Listen for key presses
    keyboard.on_press(lambda e: key_handler(e.name))
    time.sleep(0.2)

    # Keep the program running
    keyboard.wait('esc')
            

if __name__ == "__main__":
    main()
