#!/usr/bin/env python3

import sys
import time
import serial.tools.list_ports

def find_serial_port_by_vid_pid(vid, pid):
    pid_hex = pid.lower()
    vid_hex = vid.lower()
    ports = serial.tools.list_ports.comports()
    for port in ports:
        if pid_hex in port.hwid.lower() and vid_hex in port.hwid.lower():
            return port.device
    return None

pid = "c55c"
vid = "1209"

serial_port = find_serial_port_by_vid_pid(vid, pid)

if serial_port is None:
    print(f"Serial port with VID {vid} and PID {pid} not found.")
    sys.exit(1)

#open the serial port
try:
    ser = serial.Serial(serial_port, 115200, timeout=1)
    print(f"Connected to {serial_port}")
except serial.SerialException as e:
    print(f"Error opening serial port {serial_port}: {e}")
    sys.exit(1)

print("start in 3 seconds...")
time.sleep(3)
centerCommandClick = "MOVE 16383,16383,1\n"
print("Clicking at center...")
ser.write(centerCommandClick.encode('utf-8'))
time.sleep(1)
print("Dragging to lower right corner...")
dragUpperLeft = "MOVE 8191,8191,1\n"
ser.write(dragUpperLeft.encode('utf-8'))
time.sleep(1)
print("No Dragging to lower right corner...")
noDragLowerRight = "MOVE 24575,24575,0\n"
ser.write(noDragLowerRight.encode('utf-8'))
