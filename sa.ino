#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DMD2.h>
#include <fonts/ElektronMart6x8.h>
#include "PageIndex.h"

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 2
#define panelWidth 32
#define panelHeight 16
#define NUM_ROWS 3

const char* ssid = "Led Matrix";
const char* password = "1234567890";

ESP8266WebServer server(80);
SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);
SoftDMD softDmd(&dmd);
DMD_TextBox box(&softDmd, 0, 0, panelWidth * DISPLAYS_WIDE, panelHeight * DISPLAYS_HIGH);

char TextBuffers[NUM_ROWS][256] = {"", "", ""};

void setPixelZigzag(int x, int y, bool on) {
  int panelIndex = x / panelWidth;
  int localX = x % panelWidth;
  int physX;

  DMDGraphicsMode mode = on ? GRAPHICS_ON : GRAPHICS_OFF;

  if (panelIndex < DISPLAYS_WIDE) {
    physX = (DISPLAYS_WIDE - 1 - panelIndex) * panelWidth + localX;
    dmd.setPixel(physX, y, mode);
  } else {
    int bottomPanel = panelIndex - DISPLAYS_WIDE;
    physX = (DISPLAYS_WIDE - 1 - bottomPanel) * panelWidth + localX;
    dmd.setPixel(physX, y + panelHeight, mode);
  }
}

void scrollTextRow(int row, int yOffset, uint8_t speed) {
  softDmd.setBrightness(255);
  softDmd.selectFont(ElektronMart6x8);
  softDmd.begin();

  int textWidth = softDmd.stringWidth(TextBuffers[row]);
  int screenWidth = panelWidth * DISPLAYS_WIDE;

  for (int x = screenWidth; x >= -textWidth; x--) {
    softDmd.clearScreen();
    softDmd.drawString(x, yOffset, TextBuffers[row]);
    delay(speed);
  }
}

void handleRoot() {
  server.send_P(200, "text/html", MAIN_page); // from PageIndex.h
}

void handle_Incoming_Text() {
  if (server.hasArg("TextContents") && server.hasArg("row")) {
    int row = server.arg("row").toInt();
    String content = server.arg("TextContents");
    if (row >= 0 && row < NUM_ROWS) {
      content.toCharArray(TextBuffers[row], sizeof(TextBuffers[row]));
      server.send(200, "text/plain", "OK");
    } else {
      server.send(400, "text/plain", "Invalid row index");
    }
  } else {
    server.send(400, "text/plain", "Missing parameters");
  }
}

void setup() {
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/setText", handle_Incoming_Text);
  server.begin();

  dmd.begin();
  dmd.setBrightness(255);
}

void loop() {
  server.handleClient();

  scrollTextRow(0, 0, 30);         // Top third
  scrollTextRow(1, 11, 30);        // Middle third
  scrollTextRow(2, 22, 30);        // Bottom third
}
