import asyncio
from threading import Thread
import platform
from logging import getLogger
from datetime import datetime

from bleak import BleakClient
from bleak import BleakError

class BleakOperation:
	# set up logger
	LOGGER = getLogger("log").getChild("bleak_operation")
	# Notify Characteristic UUID. (Warnig : Must be all lowercase)
	A_CHARACTERISTIC_UUID = "7d46750b-443f-4de5-95be-7e86311acc1b".lower()
	B_CHARACTERISTIC_UUID = "7d46750b-443f-4de5-95be-7e86311acc1c".lower()
	# Write Characteristic UUID.
	WRITE_CHARACTERISTIC_UUID = "0663577d-1837-4e14-853b-a3478d2c7778".lower()
	# Variables for writing with BLE write and variables with write permission
	write_value = bytearray()
	write_eable = False
	# You want to continue the BLE receive loop.
	isBreak = False
	# for asyncio function
	loop = asyncio.get_event_loop()

	def __init__(self) -> None:
		self.MAC_ADDRESS = (
			# platform => windows or linux.
			# Write mac_address referring to the result of running address_check.py
			# (hex. ex: "D0:8B:7F:12:34:56")
			"FE:B2:FE:CD:EA:8A"
			if platform.system() != "Darwin"
			else "243E23AE-4A99-406C-B317-18F1BD7B4CBE"  # For mac OS (FIXME : only upper?)
		)

	async def run(self, address, loop) -> None:
		"""
			Use Bleak to connect to the BLE device.
			Set up the communication process and start operation.
			No external calls are made in the class. (Infinite loop).

			args:
				address(str) : mac address? checked by bleak
				loop(str)    : asyncio.get_event_loop()
			return:
				None
		"""
		try:
			# BLE connection
			async with BleakClient(address, loop=loop) as client:
				# FIXME: warning occurs in the version. Basically, it works.
				"""x = await client.is_connected()
				print("Connected: {0}".format(x))"""
				# Set up a callback for notify
				await client.start_notify(self.A_CHARACTERISTIC_UUID, self.a_notification_handler)
				await client.start_notify(self.B_CHARACTERISTIC_UUID, self.b_notification_handler)
				# Receiving is executed until a stop command is sent.
				while(not self.isBreak):
					await asyncio.sleep(0.01, loop=loop)
					# If there is a BLE write, do a BLE write.
					if self.write_eable:					
						await client.write_gatt_char(self.WRITE_CHARACTERISTIC_UUID, self.write_value)
						# Reset write permission
						self.disable_write()
				# End of notify process
				await client.stop_notify(self.A_CHARACTERISTIC_UUID)
				await client.stop_notify(self.B_CHARACTERISTIC_UUID)
		# When a try error occurs, such as a connection timeout. 
		# (FIXME :Exception may not be displayed).
		except BleakError as e:
			print("An error has occurred in bleak. Error Description: {}".format(e))
		except Exception as e:
			print("A fatal error has occurred. Error description : {}".format(e))

	def a_notification_handler(self, sender, data) -> None:
		# a notify callback handler.No external calls are made in the class.
		BleakOperation.LOGGER.debug("time : {2}, a notify {0}: {1}".format(sender, data, datetime.now()))

	def b_notification_handler(self, sender, data) -> None:
		# b notify callback handler. No external calls are made in the class.
		BleakOperation.LOGGER.debug("time : {2}, b notify {0}: {1}".format(sender, data, datetime.now()))

	def write_value(self, write_value:bytearray) -> None:
		"""
			Method for executing write operations via BLE communication.
			Writes in bytearray.

			args:
				write_value(bytearray) : Value to be sent via BLE communication
			return:
				None
		"""
		BleakOperation.LOGGER.debug("BLE write value : {}".format(write_value))
		self.write_eable = True
		self.write_value = bytearray(write_value)

	def disable_write(self) -> None:
		"""
			Resets the write permission of the BLE.
			(Make it write-protected and eliminate the call in the while statement)
			No external calls are made in the class. 

			args:
				None
			return:
				None
		"""
		self.write_eable = False
		self.write_value = bytearray()

	def asyncio_start(self) -> None:
		"""
			Start asynchronous processing for bleak.
			No external calls are made in the class.

			args:
				None
			return:
				None
		"""
		self.loop.run_until_complete(self.run(self.MAC_ADDRESS, self.loop))

	def start(self) -> None:
		"""
			Execute the start of the BLE process.
			args 	: None
			return 	: None
		"""
		self.worker = Thread(target=self.asyncio_start, args=(), daemon=True)
		self.worker.start()

	def end(self) -> None:
		"""
			Execute the program termination process.
			args 	: None
			return 	: None
		"""
		self.isBreak = True
		self.worker.join()