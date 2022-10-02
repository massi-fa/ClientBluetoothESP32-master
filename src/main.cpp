#include "BLEDevice.h"
#include  <Arduino.h>
#include "wifi32.h"
#include "mqtt.h"

/* Specify the Service UUID of Server */
static BLEUUID service1UUID("3778499a-3453-11ed-a261-0242ac120002"); //DHT
static BLEUUID service2UUID("3778516a-3453-11ed-a261-0242ac120002"); //LUM
/* Specify the Characteristic UUID of Server */
static BLEUUID    charUUID("37785020-3453-11ed-a261-0242ac120002");

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLERemoteCharacteristic* pRemoteCharacteristic;
static BLEAdvertisedDevice* myDevice;
static BLEClient* myClient;

int connectedDevice = 2;
int countSend = 0;

int lum = 0;
float temp = 0;
float hum = 0;

boolean readLum = false;
boolean readDht = false;

static void notifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic,
                            uint8_t* pData, size_t length, bool isNotify)
{
  Serial.print("Notify callback for characteristic ");
  Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
  Serial.print(" of data length ");
  Serial.println(length);
  Serial.print("data: ");
  Serial.println((char*)pData);
}

class MyClientCallback : public BLEClientCallbacks
{
  void onConnect(BLEClient* pclient)
  {
    
  }

  void onDisconnect(BLEClient* pclient)
  {
    connected = false;
    Serial.println("onDisconnect");
  }
};

/* Start connection to the BLE Server */
bool connectToServer()
{
  if (connectedDevice == 1){
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
      
    BLEClient*  pClient  = BLEDevice::createClient();
    
    //TEST
    myClient = pClient;

    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

      /* Connect to the remote BLE Server */
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

      /* Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(service1UUID);
    if (pRemoteService == nullptr)
    {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(service1UUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    /* Obtain a reference to the characteristic in the service of the remote BLE server */
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    /* Read the value of the characteristic */
    /* Initial value is 'Hello, World!' */
    if(pRemoteCharacteristic->canRead())
    {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
      if (value.c_str() != "DHTValue"){ 
        Serial.println("Save dati");
        hum = std::stof(value.substr(1,5));
        temp = std::stof(value.substr(13).substr(0,5));
        readDht = true;
      }
    }

    if(pRemoteCharacteristic->canNotify())
    {
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    }

    connected = true;
    return true;
  }else if (connectedDevice == 2){
    
    Serial.print("Forming a connection to ");
    Serial.println(myDevice->getAddress().toString().c_str());
      
    BLEClient*  pClient  = BLEDevice::createClient();
    
    //TEST
    myClient = pClient;

    Serial.println(" - Created client");

    pClient->setClientCallbacks(new MyClientCallback());

      /* Connect to the remote BLE Server */
    pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(" - Connected to server");

      /* Obtain a reference to the service we are after in the remote BLE server */
    BLERemoteService* pRemoteService = pClient->getService(service2UUID);
    if (pRemoteService == nullptr)
    {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(service2UUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our service");


    /* Obtain a reference to the characteristic in the service of the remote BLE server */
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr)
    {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      pClient->disconnect();
      return false;
    }
    Serial.println(" - Found our characteristic");

    /* Read the value of the characteristic */
    /* Initial value is 'Hello, World!' */
    if(pRemoteCharacteristic->canRead())
    {
      std::string value = pRemoteCharacteristic->readValue();
      Serial.print("The characteristic value was: ");
      Serial.println(value.c_str());
      if (value.c_str() != "LightServer") {
        lum = std::stoi(value);
        readLum = true;
      }
      
    }

    if(pRemoteCharacteristic->canNotify())
    {
      pRemoteCharacteristic->registerForNotify(notifyCallback);

    }

    connected = true;
    return true;
  }else{
    return false;
  }
  
}
/* Scan for BLE servers and find the first one that advertises the service we are looking for. */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks
{
 /* Called for each advertising BLE server. */
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    Serial.print("BLE Advertised Device found: ");
    Serial.println(advertisedDevice.toString().c_str());

    /* We have found a device, let us now see if it contains the service we are looking for. */
    if (connectedDevice == 1){
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service1UUID)){
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
      }
    }else if (connectedDevice == 2){
      if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(service2UUID)){
        BLEDevice::getScan()->stop();
        myDevice = new BLEAdvertisedDevice(advertisedDevice);
        doConnect = true;
        doScan = true;
      }
    }
  }
};


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("ESP32-BLE-Client");
  /* Retrieve a Scanner and set the callback we want to use to be informed when we
     have detected a new device.  Specify that we want active scanning and start the
     scan to run for 5 seconds. */
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);

  connect::setup();
  awsMqtt::setup();
}


void loop()
{
  awsMqtt::loop();
  /* If the flag "doConnect" is true, then we have scanned for and found the desired
     BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
     connected we set the connected flag to be true. */
  if (doConnect == true)
  {
    if (connectToServer())
    {
      Serial.println("We are now connected to the BLE Server.");
    } 
    else
    {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }

  /* If we are connected to a peer BLE Server, update the characteristic each time we are reached
     with the current time since boot */
  if (connected)
  {
    Serial.println("tring to disconnect");
    if ( connectedDevice == 1){
      connectedDevice = 2;
    }else{
      connectedDevice = 1;
    }
    
    myClient->disconnect();

    myClient = NULL;
    pRemoteCharacteristic = NULL;
    myDevice = NULL;
    delay(1000);
    //doConnect = true;
    //String newValue = "Shutdown";
    //pRemoteCharacteristic->writeValue(newValue.c_str(), newValue.length());
  }
  else
  {
    BLEDevice::getScan()->start(0);  // this is just example to start scan after disconnect, most likely there is better way to do it in arduino
  }
  
  if (readDht && readLum){
    awsMqtt::publishMessage(temp, hum, lum);
    readDht = false;
    readLum = false;
    countSend += 1;
    Serial.println(countSend);
    //
  }

  if (countSend == 4){
    ESP.restart();
  }
  delay(10000);
}

