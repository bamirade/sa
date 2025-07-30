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

String Text[NUM_ROWS] = {"Welcome_To_AMA_Santiago", "Please_proceed_quietly_classes_are_in_session"};

uint32_t scrollPos[NUM_ROWS] = {0, 0};
uint32_t prevMillis[NUM_ROWS] = {0, 0};
const uint8_t rowHeights[NUM_ROWS] = {0, 8};

unsigned long lastSerialSend = 0;

void handleRoot()
{
  server.send(200, "text/html", MAIN_page);
}

void handle_Incoming_Text()
{
  if (server.hasArg("TextContents") && server.hasArg("row"))
  {
    String incoming = server.arg("TextContents");
    int row = server.arg("row").toInt();

    if (row >= 0 && row < NUM_ROWS)
    {
      Text[row] = incoming;

      Serial.printf("Row %d text: %s\n", row, Text[row].c_str());

      if (row == 1)
      {
        Serial.print("ROW2:");
        Serial.println(Text[1]);
      }

      server.send(200, "text/plain", "Text received for row " + String(row));
    }
    else
    {
      server.send(400, "text/plain", "Invalid row index");
    }
  }
  else
  {
    server.send(400, "text/plain", "Missing TextContents or row");
  }
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  dmd.setBrightness(255);
  dmd.selectFont(Arial14);
  dmd.begin();

  WiFi.softAP(ssid, password);
  IPAddress apip = WiFi.softAPIP();

  Serial.println();
  Serial.println("Reset reason:");
  Serial.println(ESP.getResetReason());

  Serial.print("Connect to: ");
  Serial.println(ssid);
  Serial.print("IP Address: ");
  Serial.println(apip);

  server.on("/", handleRoot);
  server.on("/setText", handle_Incoming_Text);
  server.begin();
  Serial.println("HTTP server started");
}

void loop()
{
  server.handleClient();

  scrollTextRow(0, rowHeights[0], 50);

  if (millis() - lastSerialSend > 500)
  {
    Serial.print("ROW2:");
    Serial.println(Text[1]);
    lastSerialSend = millis();
  }

  delay(30);
}

void scrollTextRow(int index, int y, uint8_t speed)
{
  dmd.selectFont(Arial14);
  int screenWidth = dmd.width;

  if ((millis() - prevMillis[index]) > speed)
  {
    prevMillis[index] = millis();

    String text = Text[index];
    int totalWidth = dmd.stringWidth(text.c_str());

    dmd.drawFilledBox(0, y, screenWidth, y + 14, GRAPHICS_OFF);

    dmd.drawString(screenWidth - scrollPos[index], y, text.c_str());

    scrollPos[index]++;
    if (scrollPos[index] > totalWidth + screenWidth)
    {
      scrollPos[index] = 0;
    }
  }
}
