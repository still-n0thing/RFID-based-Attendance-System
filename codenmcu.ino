// Install all the listed library on Arduino
#include <SPI.h>

#include <MFRC522.h>

#include "ThingSpeak.h"

#include <ESP8266WiFi.h>

// All password here
#define SECRET_SSID "Wifi Name"
#define SECRET_PASS "Wifi Password"
#define SECRET_CH_ID 0 // Channel ID here
#define SECRET_WRITE_APIKEY "Write API Key"

char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password
int keyIndex = 0; // your network key Index number (needed only for WEP)
WiFiClient client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

//--------------------------------------------------------

constexpr uint8_t RST_PIN = D3; // Configurable, see typical pin layout above
constexpr uint8_t SS_PIN = D4; // Configurable, see typical pin layout above

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class
MFRC522::MIFARE_Key key;

String tag;
String myStatus = "";

// Use some video to get rfid card number
const int len = 4;
String students[len] = {
  "RFID card no.",
  "RFID card no.",
  "RFID card no.",
  "RFID card no."
};

int ledred = D2;
int ledgreen = D1;

void setup() {
  Serial.begin(9600);
  // Leds
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);

  // Rfid
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  // WiFi
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client); // Initialize ThingSpeak
}
void loop() {
  // Connect or reconnect to WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while (WiFi.status() != WL_CONNECTED) {
      WiFi.begin(ssid, pass); // Connect to WPA/WPA2 network. Change this line if using open or WEP network
      Serial.print(".");
      delay(5000);
    }
    Serial.println("\nConnected.");
  }

  if (!rfid.PICC_IsNewCardPresent())
    return;
  if (rfid.PICC_ReadCardSerial()) {
    for (byte i = 0; i < 4; i++) {
      tag += rfid.uid.uidByte[i];
    }

    bool flag = false;
    int id = 0;
    for (int i = 0; i < len; i++) {
      if (tag == students[i]) {
        flag = true;
        id = i + 1;
      }
    }

    if (flag) {
      ThingSpeak.setField(1, id);
      // set the status
      ThingSpeak.setStatus(myStatus);

      // write to the ThingSpeak channel
      int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
      if (x == 200) {
        digitalWrite(ledgreen, HIGH);
        delay(1000); // 1000
        digitalWrite(ledgreen, LOW);
        Serial.println("Channel update successful.");
      } else {
        digitalWrite(ledred, HIGH);
        delay(1000); // 1000
        digitalWrite(ledred, LOW);
        Serial.println("Problem updating channel. HTTP error code " + String(x));
      }
    }
    //    digitalWrite(ledred, HIGH);
    //    digitalWrite(ledgreen, HIGH);
    //    delay(1000);
    //    digitalWrite(ledgreen, LOW);
    //    digitalWrite(ledred, LOW);

    Serial.println(tag);
    tag = "";
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    delay(20000); // Wait 20 seconds to update the channel again 20000
  }
}