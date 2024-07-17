#include <ArduinoBLE.h>

BLEService sensorService("00000002-0000-0000-FDFD-FDFDFDFDFDFD");

// BLE characteristics for temperature and humidity
BLEFloatCharacteristic temperatureCharacteristic("2A6E", BLERead | BLENotify);
BLEFloatCharacteristic humidityCharacteristic("2A6F", BLERead | BLENotify);

float generateTemperature() {
  // Mimic sensor behavior by generating a random temperature value between 20.0 and 30.0
  return random(200, 300) / 10.0;
}

float generateHumidity() {
  // Mimic sensor behavior by generating a random humidity value between 40.0 and 60.0
  return random(400, 600) / 10.0;
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("ESP32 Sensor");
  BLE.setAdvertisedService(sensorService);

  // Add characteristics to the service
  sensorService.addCharacteristic(temperatureCharacteristic);
  sensorService.addCharacteristic(humidityCharacteristic);

  // Add service
  BLE.addService(sensorService);

  // Set initial value for the characteristics
  temperatureCharacteristic.writeValue(generateTemperature());
  humidityCharacteristic.writeValue(generateHumidity());

  // Start advertising
  BLE.advertise();

  Serial.println("BLE device is now advertising");
}

void loop() {
  // Listen for BLE peripherals to connect
  BLEDevice central = BLE.central();

  // If a central is connected
  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    // Keep updating the temperature and humidity values
    while (central.connected()) {
      float temp = generateTemperature();
      float humidity = generateHumidity();

      Serial.print("Temperature: ");
      Serial.print(temp);
      Serial.print(" C, Humidity: ");
      Serial.print(humidity);
      Serial.println(" %");

      temperatureCharacteristic.writeValue(temp);
      humidityCharacteristic.writeValue(humidity);

      // Notify connected devices about the updated values
      temperatureCharacteristic.notify();
      humidityCharacteristic.notify();

      // Update values every 2 seconds
      delay(2000);
    }

    Serial.print("Disconnected from central: ");
    Serial.println(central.address());
  }
}
