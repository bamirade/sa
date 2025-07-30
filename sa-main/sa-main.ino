#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include "PageIndex.h"

const char *ssid = "Led Matrix";
const char *password = "1234567890";

ESP8266WebServer server(80);

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);

#define NUM_ROWS 2

char TextBuffers[NUM_ROWS][100] = {
  "Flames Hope", "Row 2"
};
char *Text[] = {TextBuffers[0], TextBuffers[1]};

uint32_t scrollPos[NUM_ROWS] = {0, 0};
uint32_t prevMillis[NUM_ROWS] = {0, 0};
const uint8_t rowHeights[NUM_ROWS] = {0, 8};

unsigned long lastSerialSend = 0;

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handle_Incoming_Text() {
  if (server.hasArg("TextContents") && server.hasArg("row")) {
    String incoming = server.arg("TextContents");
    int row = server.arg("row").toInt();

    if (row >= 0 && row < NUM_ROWS) {
      incoming.toCharArray(TextBuffers[row], sizeof(TextBuffers[row]));
      Serial.printf("Row %d text: %s\n", row, incoming.c_str());

      if (row == 1) {
        Serial.print("ROW2:");
        Serial.println(TextBuffers[1]);
      }

      server.send(200, "text/plain", "Text received for row " + String(row));
    } else {
      server.send(400, "text/plain", "Invalid row index");
    }
  } else {
    server.send(400, "text/plain", "Missing TextContents or row");
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  dmd.setBrightness(255);
  dmd.selectFont(Arial14);
  dmd.begin();

  WiFi.softAP(ssid, password);
  IPAddress apip = WiFi.softAPIP();

  Serial.print("Connect to: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(apip);

  server.on("/", handleRoot);
  server.on("/setText", handle_Incoming_Text);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();

  scrollTextRow(0, rowHeights[0], 50);

  if (millis() - lastSerialSend > 500) {
    Serial.print("ROW2:");
    Serial.println(Text[1]);
    lastSerialSend = millis();
  }

  delay(30);
}

void scrollTextRow(int index, int y, uint8_t speed) {
  dmd.selectFont(Arial14);
  int width = dmd.width;
  int fullScroll = dmd.stringWidth(Text[index]) + width;

  if ((millis() - prevMillis[index]) > speed) {
    prevMillis[index] = millis();
    scrollPos[index] = (scrollPos[index] + 1) % fullScroll;

    dmd.clearScreen();
    dmd.drawString(width - scrollPos[index], y, Text[index]);
  }
}
