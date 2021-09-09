#include <Arduino.h>
#include <bluefruit.h>
#include "UUIDConverter.h"
// Define UUID
#define USER_SERVICE_UUID "5d65dfcc-996b-4376-868e-82196f0962e1"
#define WRITE_CHARACTERISTIC_UUID "0663577d-1837-4e14-853b-a3478d2c7778"
#define NOTIFY_CHARACTERISTIC_UUID "7d46750b-443f-4de5-95be-7e86311acc1b"
#define NOTIFY_CHARACTERISTIC_UUID2 "7d46750b-443f-4de5-95be-7e86311acc1c"

//Service & Characteristic
BLEService        testService;
BLECharacteristic testNotifyCharacteristic;
BLECharacteristic testNotifyCharacteristic2;
BLECharacteristic testWriteCharacteristic;

uint8_t  num = 0;

void setup()
{
  Serial.begin(115200);
  while ( !Serial ) delay(10);   // for nrf52840 with native usb

  Serial.println("Bluefruit52 Test");
  Serial.println("--------------------------\n");

  // Initialise the Bluefruit module
  Bluefruit.begin();
  Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

  // Set the advertised device name (keep it short!)
  Bluefruit.setName("Yamazaki_Arm_mycon_test");

  // Set the connect/disconnect callback handlers
  Bluefruit.Periph.setConnectCallback(connect_callback);
  Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

  // Setup the Heart Rate Monitor service using
  // BLEService and BLECharacteristic classes
  Serial.println("Configuring the Heart Rate Monitor Service");
  setupTestService();

  // Set up and start advertising
  startAdv();

}

void startAdv(void)
{
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addName();

  // Include HRM Service UUID
  Bluefruit.Advertising.addService(testService);

  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  

  // Create loop2() using Scheduler to run in 'parallel' with loop()
  Scheduler.startLoop(loop2);
}

void setupTestService(void)
{
  uint8_t userServiceUUID[16];
  uint8_t notifyCharacteristicUUID[16];
  uint8_t writeCharacteristicUUID[16];
  uint8_t notify2CharacteristicUUID[16];

  strUUID2Bytes(USER_SERVICE_UUID, userServiceUUID);
  strUUID2Bytes(NOTIFY_CHARACTERISTIC_UUID, notifyCharacteristicUUID);
  strUUID2Bytes(NOTIFY_CHARACTERISTIC_UUID2, notify2CharacteristicUUID);
  strUUID2Bytes(WRITE_CHARACTERISTIC_UUID, writeCharacteristicUUID);

  testService = BLEService(userServiceUUID);
  testService.begin();

  testNotifyCharacteristic = BLECharacteristic(notifyCharacteristicUUID);
  testNotifyCharacteristic.setProperties(CHR_PROPS_NOTIFY);
  testNotifyCharacteristic.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  testNotifyCharacteristic.setMaxLen(5);
  //testNotifyCharacteristic.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  testNotifyCharacteristic.begin();

  testNotifyCharacteristic2 = BLECharacteristic(notify2CharacteristicUUID);
  testNotifyCharacteristic2.setProperties(CHR_PROPS_NOTIFY);
  testNotifyCharacteristic2.setPermission(SECMODE_OPEN, SECMODE_NO_ACCESS);
  testNotifyCharacteristic2.setMaxLen(5);
  //testNotifyCharacteristic.setCccdWriteCallback(cccd_callback);  // Optionally capture CCCD updates
  testNotifyCharacteristic2.begin();

  testWriteCharacteristic = BLECharacteristic(writeCharacteristicUUID);
  testWriteCharacteristic.setProperties(CHR_PROPS_WRITE);
  testWriteCharacteristic.setPermission(SECMODE_OPEN, SECMODE_OPEN);
  testWriteCharacteristic.setFixedLen(5);
  testWriteCharacteristic.setWriteCallback(write_callback);
  testWriteCharacteristic.begin();
}

void connect_callback(uint16_t conn_handle)
{
  // Get the reference to current connection
  BLEConnection* connection = Bluefruit.Connection(conn_handle);

  char central_name[32] = { 0 };
  connection->getPeerName(central_name, sizeof(central_name));

  Serial.print("Connected to ");
  Serial.println(central_name);
}

void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
  (void) conn_handle;
  (void) reason;

  Serial.print("Disconnected, reason = 0x"); Serial.println(reason, HEX);
  Serial.println("Advertising!");
}

void cccd_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint16_t cccd_value)
{
    // Display the raw request packet
    Serial.print("CCCD Updated: ");
    //Serial.printBuffer(request->data, request->len);
    Serial.print(cccd_value);
    Serial.println("");

    // Check the characteristic this CCCD update is associated with in case
    // this handler is used for multiple CCCD records.
    if (chr->uuid == testNotifyCharacteristic.uuid) {
        if (chr->notifyEnabled(conn_hdl)) {
            Serial.println("'Notify' enabled");
        } else {
            Serial.println("'Notify' disabled");
        }
    }
}

void write_callback(uint16_t conn_hdl, BLECharacteristic* chr, uint8_t* data, uint16_t len)
{
  Serial.println(data[0]);
  digitalToggle(LED_RED); // Toggle LED
}

void loop()
{
  uint8_t testdata[5] = { 0x06, 0x07, 0x08, 0x09, 0x10 };

  if ( Bluefruit.connected() ) {

    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent

    for(int i = 0; i < 1000;i++){
        
    }
    
    if ( testNotifyCharacteristic.notify(testdata, num%5+1) ){
      ++num;
      //Serial.println("SUCCESS");
    }else{
      //Serial.println("ERROR: Notify not set in the CCCD or not connected!");
    }
    if ( testNotifyCharacteristic2.notify(testdata, num%5+2) ){
      ++num;
      //Serial.println("SUCCESS");
    }else{
      //Serial.println("ERROR: Notify not set in the CCCD or not connected!");
    }
  }

  vTaskDelay(10);              // wait for a half second
}
void loop2(){

}
/*
void loop2(){
  uint8_t testdata[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

  if ( Bluefruit.connected() ) {

    // Note: We use .notify instead of .write!
    // If it is connected but CCCD is not enabled
    // The characteristic's value is still updated although notification is not sent
    if ( testNotifyCharacteristic.notify(testdata, num%5+1) ){
      ++num;
      Serial.println("SUCCESS");
    }else{
      Serial.println("ERROR: Notify not set in the CCCD or not connected!");
    }
  }

  vTaskDelay(10);
}
*/
