#include <DHT.h>

// Konfigurasi sensor
#define DHT_PIN 4
#define DHT_TYPE DHT22

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
  Serial.begin(115200);

  dht.begin();

  Serial.println("DHT22 Sensor Started!");
}

void loop() {
  float suhu = dht.readTemperature();
  float kelembapan = dht.readHumidity();

  // Periksa kegagalan pembacaan
  if (isnan(suhu) || isnan(kelembapan)) {
    Serial.println("Gagal membaca DHT22!");
    delay(2000);
    return;
  }

  // Tampilkan suhu
  Serial.print("Temperature: ");
  Serial.print(suhu);
  Serial.println(" °C");

  // Tampilkan kelembapan
  Serial.print("Humidity: ");
  Serial.print(kelembapan);
  Serial.println(" %");

  Serial.println("--------------------");

  delay(2000);
}
