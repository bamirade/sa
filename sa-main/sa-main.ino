#include <ESP8266WebServer.h>
#include <DMD2.h>
#include <fonts/Arial14.h>
#include "PageIndex.h"

const char* ssid = "Led Matrix";
const char* password = "1234567890";

ESP8266WebServer server(80);

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);

#define NUM_ROWS 2

char TextBuffers[NUM_ROWS][100] = {
  "WELCOME TO AMA SANTIAGO", "Row 2"
};
char* Text[] = {TextBuffers[0], TextBuffers[1]};

uint32_t scrollPos[NUM_ROWS] = {0, 0};
uint32_t prevMillis[NUM_ROWS] = {0, 0};
const uint8_t rowHeights[NUM_ROWS] = {0, 8};

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handle_Incoming_Text() {
  if (server.hasArg("TextContents") && server.hasArg("row")) {
    String incoming = server.arg("TextContents");
    int row = server.arg("row").toInt();

    if (row >= 0 && row < NUM_ROWS) {
      Serial.printf("Row %d text: %s\n", row, incoming.c_str());
      incoming.toCharArray(TextBuffers[row], sizeof(TextBuffers[row]));

      if (row == 1) {
        // Send row 2 data to second ESP
        Serial.println("ROW2:" + incoming);
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

  Serial.printf("Connect to: %s\nIP Address: %s\n", ssid, apip.toString().c_str());

  server.on("/", handleRoot);
  server.on("/setText", handle_Incoming_Text);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
  dmd.clearScreen();

  scrollTextRow(0, rowHeights[0], 50);

  delay(30);
}

void scrollTextRow(int index, int y, uint8_t speed) {
  int width = dmd.width;
  dmd.selectFont(Arial14);

  int fullScroll = dmd.stringWidth(Text[index]) + width;
  if ((millis() - prevMillis[index]) > speed) {
    prevMillis[index] = millis();
    scrollPos[index] = (scrollPos[index] + 1) % fullScroll;
  }

  dmd.drawString(width - scrollPos[index], y, Text[index]);
}
