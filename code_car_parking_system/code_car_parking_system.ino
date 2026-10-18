#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const char* ssid = "Prateek";
const char* password = "justdoelectronics@#12345";

const int irDataPin1 = 12;
const int irSensorId1 = 1;

const int irDataPin2 = 14;
const int irSensorId2 = 2;

const int irDataPin3 = 27;
const int irSensorId3 = 3;

const int irDataPin4 = 26;
const int irSensorId4 = 4;

const int OCCUPIED_STATE = LOW;
const int TOTAL_SPOTS = 4;

const char* serverUrl = "https://justdoelectronics.com/test/carparking/update_status.php";

LiquidCrystal_I2C lcd(0x27, 20, 4);


void sendDataToServer(int id, int status) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    String data = "idsensor=" + String(id) + "&status=" + String(status);
    Serial.println("Sending: " + data);

    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    int httpResponseCode = http.POST(data);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("HTTP Code: " + String(httpResponseCode));
      Serial.println("Server Response: " + response);
    } else {
      Serial.println("Error sending data. HTTP Code: " + String(httpResponseCode));
      Serial.println("Error: " + http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected. Cannot send data.");
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(irDataPin1, INPUT);
  pinMode(irDataPin2, INPUT);
  pinMode(irDataPin3, INPUT);
  pinMode(irDataPin4, INPUT);

  lcd.init();
  lcd.backlight();
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Connecting to WiFi...");

  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    lcd.print(".");
    attempts++;
  }

  lcd.clear();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("Local IP: ");
    Serial.println(WiFi.localIP());

    lcd.setCursor(0, 0);
    lcd.print("Connected! IP:");
    lcd.setCursor(0, 1);
    lcd.print(WiFi.localIP());
    delay(2000);
    lcd.clear();

  } else {
    Serial.println("\nWiFi Connection Failed.");
    lcd.setCursor(0, 0);
    lcd.print("WiFi Failed!");
    lcd.setCursor(0, 1);
    lcd.print("Check credentials.");
  }
}

void loop() {
  int irReading1 = digitalRead(irDataPin1);
  int irReading2 = digitalRead(irDataPin2);
  int irReading3 = digitalRead(irDataPin3);
  int irReading4 = digitalRead(irDataPin4);

  int irStatus1 = (irReading1 == OCCUPIED_STATE) ? 1 : 0;
  int irStatus2 = (irReading2 == OCCUPIED_STATE) ? 1 : 0;
  int irStatus3 = (irReading3 == OCCUPIED_STATE) ? 1 : 0;
  int irStatus4 = (irReading4 == OCCUPIED_STATE) ? 1 : 0;

  int totalOccupied = irStatus1 + irStatus2 + irStatus3 + irStatus4;
  int totalFree = TOTAL_SPOTS - totalOccupied;

  if (totalOccupied == TOTAL_SPOTS) {

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("!!! PARKING FULL !!!");
    lcd.setCursor(0, 1);
    lcd.print("                    ");
    lcd.setCursor(0, 2);
    lcd.print(" NO SPACE AVAILABLE  ");

    lcd.setCursor(0, 3);
    lcd.print("                    ");

  } else {

    lcd.setCursor(0, 0);
    lcd.print("S1:");
    lcd.print((irStatus1 == 1) ? "FULL" : "FREE    ");

    lcd.setCursor(11, 0);
    lcd.print("S2:");
    lcd.print((irStatus2 == 1) ? "FULL" : "FREE    ");
    lcd.setCursor(0, 1);
    lcd.print("S3:");
    lcd.print((irStatus3 == 1) ? "FULL" : "FREE    ");

    lcd.setCursor(11, 1);
    lcd.print("S4:");
    lcd.print((irStatus4 == 1) ? "FULL" : "FREE    ");

    lcd.setCursor(0, 2);
    lcd.print("--------------------");

    lcd.setCursor(0, 3);
    lcd.print("FREE:");
    lcd.print(totalFree);
    lcd.print(" OF ");
    lcd.print(TOTAL_SPOTS);
    lcd.print("     ");

    lcd.setCursor(13, 3);
    lcd.print("OCC:");
    lcd.print(totalOccupied);
    lcd.print(" ");
  }

  sendDataToServer(irSensorId1, irStatus1);
  sendDataToServer(irSensorId2, irStatus2);
  sendDataToServer(irSensorId3, irStatus3);
  sendDataToServer(irSensorId4, irStatus4);

  delay(2000);
}