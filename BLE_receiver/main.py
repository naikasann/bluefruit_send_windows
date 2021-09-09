from bleak_operation.bleak_operation import BleakOperation
from logging import getLogger, StreamHandler, DEBUG, INFO, Formatter
import time

# Set up logger
HANDLER = StreamHandler()
FORMATTER = Formatter('[%(levelname)s] %(filename)s - %(lineno)d - %(message)s')
HANDLER.setFormatter(FORMATTER)
LOGGER = getLogger("log")
LOGGER.addHandler(HANDLER)
LOGGER.setLevel(DEBUG)

def main():
    # init bleak operation
    bleak = BleakOperation()
    # ble connection start
    bleak.start()
    time.sleep(10)
    # do BLE write
    print("write")
    bleak.write_value(bytearray(b"\x06\x07"))
    time.sleep(5)

    print("end")
    # disconnection BLE
    bleak.end()

if __name__ == "__main__":
    main()