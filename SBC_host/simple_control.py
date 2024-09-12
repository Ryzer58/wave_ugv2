import serial
import json
import time
import keyboard

throttle_level = 100
throttle_min = 70
throttle_max = 255

# Initialize serial connection to SBC
ser = serial.Serial('/dev/ttyUSB0', 115200)

def send_motion(mota_speed, motb_speed):
    cmd = {
        "mota_speed": mota_speed,
        "motb_speed": motb_speed
    }
    json_command = json.dumps(cmd)
    ser.write(json_command.encode('utf-8'))


def main():
    global throttle_level, throttle_min, throttle_max
    while True:
        if keyboard.is_pressed('w'):
            send_motion(throttle_level, throttle_level)  # Forward
        elif keyboard.is_pressed('a'):
            send_motion(-150, 150)  # Left
        elif keyboard.is_pressed('s'):
            send_motion(-150, -150)  # Reverse
        elif keyboard.is_pressed('d'):
            send_motion(150, -150)  # Right
        elif keyboard.is_pressed(','):
            if throttle_level > throttle_min:
                throttle_level = throttle_level - 10
        elif keyboard.is_pressed('.'):
            if throttle_level < throttle_max:
                throttle_level = throttle_level + 10
        else:
            send_motion(0, 0)  # Stop

if __name__ == "__main__":
    main()
