#!/usr/bin/env python3
"""Hyprland global right-click listener for the local Qt app."""
import select
import time
import urllib.request

from evdev import InputDevice, ecodes, list_devices

TRIGGER_URL = "http://127.0.0.1:8766/capture"


def devices():
    result = []
    for path in list_devices():
        try:
            device = InputDevice(path)
            if ecodes.EV_KEY in device.capabilities() and ecodes.BTN_RIGHT in device.capabilities()[ecodes.EV_KEY]:
                result.append(device)
        except OSError:
            pass
    return result


def main():
    last = 0.0
    while True:
        current = devices()
        if not current:
            print("No mouse input device. Add user to input group or install python-evdev.", flush=True)
            time.sleep(3)
            continue
        ready, _, _ = select.select(current, [], [], 1.0)
        for device in ready:
            for event in device.read():
                if event.type == ecodes.EV_KEY and event.code == ecodes.BTN_RIGHT and event.value == 1 and time.monotonic() - last > 0.25:
                    last = time.monotonic()
                    try:
                        urllib.request.urlopen(urllib.request.Request(TRIGGER_URL, method="POST"), timeout=2).close()
                    except OSError:
                        pass


if __name__ == "__main__":
    main()
