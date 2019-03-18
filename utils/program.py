#!/usr/bin/env python3

import sys
import subprocess
import os
from glob import glob


def get_serial_port():
    serial_ports = glob("/dev/cu.usb*")

    if len(serial_ports) == 0:
        raise Exception("No usb serial ports found")
    elif len(serial_ports) == 1:
        return serial_ports[0]
    else:
        print("Multiple USB serial ports found.\n")
        for i, port in enumerate(serial_ports):
            print("[%02d] %s" % (i, port))

        print()
        port_num = int(input("Select port #: "))
        return serial_ports[0]


def program_avr(hex_file, port, mmcu, baud=115200, programmer="wiring"):
    args = ["avrdude", "-b", str(baud), "-c", programmer, "-p", mmcu,
            "-P", port, "-U", "flash:w:%s" % hex_file] 
    print("> ", " ".join(args))
    subprocess.check_call(args)


if __name__ == "__main__":
    if len(sys.argv) == 1:
        print("Usage: python program.py [hex file]")
        sys.exit(-1)

    serial_port = get_serial_port()
    mmcu = os.environ.get("MMCU") or "atmega328p"
    
    program_avr(sys.argv[1], serial_port, mmcu)
