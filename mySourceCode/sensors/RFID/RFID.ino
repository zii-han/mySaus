
#include <SPI.h>
#include <MFRC522.h>

// ================================
// KONFIGURASI PIN RFID RC522 - ESP32
// ================================

// RC522  -> ESP32
// SDA    -> GPIO 5
// SCK    -> GPIO 18
// MOSI   -> GPIO 23
// MISO   -> GPIO 19
// IRQ    -> Tidak digunakan
// GND    -> GND
// RST    -> GPIO 22
// 3.3V   -> 3V3

#define SS_PIN 5
#define RST_PIN 22

// Inisialisasi RFID
MFRC522 rfid(SS_PIN, RST_PIN);

// Variabel global UID
String uidA = "";

void setup() {
  Serial.begin(115200);

  // Konfigurasi SPI ESP32
  SPI.begin(18, 19, 23, 5);

  // Inisialisasi RFID
  rfid.PCD_Init();

  Serial.println("RFID Ready!");
  Serial.println("Tempelkan kartu RFID...");
}

void loop() {
  // Deteksi kartu baru
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Membaca UID kartu
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Reset UID sebelumnya
  uidA = "";

  // Mengambil UID dalam format HEX
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uidA += "0";
    }

    uidA += String(rfid.uid.uidByte[i], HEX);
  }

  // Mengubah UID menjadi kapital
  uidA.toUpperCase();

  // Menampilkan UID
  Serial.print("UID: ");
  Serial.println(uidA);

  // Menghentikan komunikasi dengan kartu
  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}
