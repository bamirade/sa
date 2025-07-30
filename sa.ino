#include <ESP8266WebServer.h>
#include <DMDESP.h>
#include <fonts/Mono5x7.h>
#include "PageIndex.h"

const char* ssid = "Led Matrix";
const char* password = "1234567890";

ESP8266WebServer server(80);

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
DMDESP Disp(DISPLAYS_WIDE, DISPLAYS_HIGH);

#define NUM_ROWS 3

// Buffers for each row
char TextBuffers[NUM_ROWS][100] = {
  "WELCOME TO AMA SANTIAGO", "", ""
};

char* Text[] = {TextBuffers[0], TextBuffers[1], TextBuffers[2]};

// Scrolling state for each row
uint32_t scrollPos[NUM_ROWS] = {0, 0, 0};
uint32_t prevMillis[NUM_ROWS] = {0, 0, 0};
const uint8_t rowHeights[NUM_ROWS] = {0, 8, 16}; // Y positions for each row

void handleRoot() {
  server.send(200, "text/html", MAIN_page);
}

void handle_Incoming_Text() {
  if (server.hasArg("TextContents") && server.hasArg("row")) {
    String incoming = server.arg("TextContents");
    int row = server.arg("row").toInt();

    if (row >= 0 && row < NUM_ROWS) {
      Serial.print("Row ");
      Serial.print(row);
      Serial.print(" text: ");
      Serial.println(incoming);

      incoming.toCharArray(TextBuffers[row], sizeof(TextBuffers[row]));
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

  Disp.start();
  Disp.setBrightness(50);
  Disp.setFont(Mono5x7);

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
  Disp.loop();

  for (int i = 0; i < NUM_ROWS; i++) {
    scrollTextRow(i, rowHeights[i], 50); // Y-pos per row
  }
}

void scrollTextRow(int index, int y, uint8_t speed) {
  int width = Disp.width();
  Disp.setFont(Mono5x7);

  int fullScroll = Disp.textWidth(Text[index]) + width;
  if ((millis() - prevMillis[index]) > speed) {
    prevMillis[index] = millis();
    if (scrollPos[index] < fullScroll) {
      scrollPos[index]++;
    } else {
      scrollPos[index] = 0;
    }
    Disp.drawText(width - scrollPos[index], y, Text[index]);
  }
}
