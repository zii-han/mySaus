/*
  Author : elNaoHolic
  Last update : 08 Mei 2026
  github : @zii_han

  note : kalau mau pakai/edit jangan lupa credit :)
*/

#include <WiFi.h>
#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

// Wifi network station credentials
#define WIFI_SSID "Esp32"
#define WIFI_PASSWORD "RFIDPROJ2026"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SS_PIN 5
#define RST_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
MFRC522 rfid(SS_PIN, RST_PIN);
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "8016503683:AAHNIwXsTsZQedmZMh8fYv3QbM8Nfqxd1Ns"
#define CHAT_ID "-5260583676"
#define owner "8315750679"

//pin
const char R = 12;
const char G = 13;
const char buzz = 14;
//data
struct Siswa {
  String id;
  String nama;
  String kelas;
  String tagName;
  bool sTatus;
};

Siswa data[] = {
  { "6D7CF8A1", "Farhan Fauzi", "XI.F-2", "farhan", 0 },
  { "47A8D55", "Septian Rama", "XII", "Rama", 0 },
  { "B73CE6", "Abdi Pratama", "XI", "abdi", 0 },
  { "B2AC91", "Rizqi Ramadhan", "XI", "rizqi", 0 }
};

int filter = sizeof(data) / sizeof(data[0]);

const unsigned long BOT_MTBS = 5000;  // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;  // last time messages' scan has been done
bool Start = false;
bool idKetemu;
String text;
String uid = "";
String sTaTus;
String namaID;
String kelasID;

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = bot.messages[i].chat_id;
    text = bot.messages[i].text;
    String oledKey = "/oled";
    String addKey = "/add";
    String infoGroup = "/infogroup";
    String infoGroupnOled = "/infoall";

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if (text == "Halo") {
      bot.sendChatAction(chat_id, "typing");
      delay(4000);
      bot.sendMessage(chat_id, "Hai");
    }

    if (text == "/menu") {
      String welcome = "Welcome to Universal Arduino Telegram Bot library, " + from_name + ".\n";
      welcome += "This is Chat Action Bot example.\n\n";
      welcome += "/send_test_action : to send test chat action message\n";
      bot.sendMessage(chat_id, welcome);
    }
    //my funct
    else if (text != NULL && chat_id == owner) {
      if (text.startsWith(oledKey)) {
        text.replace(oledKey, "");
        text.trim();
        benar();
        oledViaBot(text);

      } else if (text.startsWith(addKey)) {
        text.replace(addKey, "");
        text.trim();
        cariId(text);
      } else if (text.startsWith(infoGroup)) {
        text.replace(infoGroup, "");
        text.trim();
        bot.sendMessage(CHAT_ID, text);
      } else if (text.startsWith(infoGroupnOled)) {
        text.replace(infoGroupnOled, "");
        text.trim();
        benar();
        oledViaBot(text);
        bot.sendMessage(CHAT_ID, text);
      }
    } else if (text != NULL && chat_id != owner) {
      String denied = "Maaf, fitur ini hanya khusus admin dan operator saja";
      bot.sendMessage(chat_id, denied);
    }
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("MANSA PADPAR");
  Serial.println("RFID siap...");

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true)
      ;
  }
  Serial.println();

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);  // Add root certificate for api.telegram.org
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(10, 25);
    display.println("Menghubungkan ke ");
    display.setCursor(10, 35);
    display.println("Wi-fi mohon tunggu");
    box();
    delay(3000);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org");  // get UTC time via NTP
  time_t now = time(nullptr);
  while (now < 24 * 3600) {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  Serial.println("halo");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Wi-fi behasil ");
  display.setCursor(10, 35);
  display.println("Di hubungkan");
  box();
  delay(3000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 25);
  display.println("Silahkan tempelkan ");
  display.setCursor(10, 35);
  display.println("Kartu nya");
  box();
  bot.sendMessage(CHAT_ID, "Bot telah aktif", "");
  //pin
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(buzz, OUTPUT);
}

void loop() {
  if (millis() - bot_lasttime > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
  //RFID
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  Serial.print("UID: ");
  uid = "";

  for (byte i = 0; i < rfid.uid.size; i++) {
    uid += String(rfid.uid.uidByte[i], HEX);
  }

  uid.toUpperCase();
  Serial.println(uid);


  cariId(uid);
  rfid.PICC_HaltA();

  box();
}
void benar() {
  digitalWrite(R, 0);
  digitalWrite(G, 1);
  digitalWrite(buzz, 1);
  delay(500);
  digitalWrite(G, 0);
  digitalWrite(buzz, 0);
}
void salah() {
  digitalWrite(R, 1);
  digitalWrite(G, 0);
  digitalWrite(buzz, 1);
  delay(250);
  digitalWrite(R, 0);
  digitalWrite(buzz, 0);
  delay(250);
  digitalWrite(R, 1);
  digitalWrite(buzz, 1);
  delay(250);
  digitalWrite(R, 0);
  digitalWrite(buzz, 0);
}

//Olef via bot
void oledViaBot(String pesan) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println(pesan);
  box();
}

void cariId(String inputId) {
  idKetemu = false;
  for (int i = 0; i < filter; i++) {
    if (inputId == data[i].id) {
      namaID = data[i].nama;
      kelasID = data[i].kelas;
      if (data[i].sTatus == 0) {
        data[i].sTatus += 1;
        sTaTus = "Hadir";
        displayMyID(sTaTus, namaID, kelasID);
      } else if (data[i].sTatus == 1) {
        data[i].sTatus -= 1;
        sTaTus = "Pulang";
        displayMyID(sTaTus, namaID, kelasID);
      }

      //bot
      String absen = "Data di terima.\n\n";
      absen += " Id : " + data[i].id + "\n";
      absen += " Nama : " + data[i].nama + "\n";
      absen += " Kelas : " + data[i].kelas + "\n";
      absen += " Status : " + sTaTus + "\n";
      bot.sendMessage(CHAT_ID, absen);
      idKetemu = true;
      benar();
      break;
    }
  }

  if (!idKetemu) {
    displayMyID("-", "-", "-");
    salah();
  }
}

//tampilakn MyId
void displayMyID(String sTaTuss, String Nname, String Ckelas) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10, 10);
  display.println("ID dikenali");
  display.setCursor(10, 20);
  display.println("ID : " + uid);
  display.setCursor(10, 30);
  display.println("Nama : " + Nname);
  display.setCursor(10, 40);
  display.println("Kelas : " + Ckelas);
  display.setCursor(10, 50);
  display.println("Status : " + sTaTuss);
}
void box() {
  display.drawLine(0, 0, 0, 63, SSD1306_WHITE);
  display.drawLine(0, 63, 127, 63, SSD1306_WHITE);
  display.drawLine(127, 63, 127, 0, SSD1306_WHITE);
  display.drawLine(127, 0, 0, 0, SSD1306_WHITE);
  display.display();  // Update screen with each newly-drawn line
}