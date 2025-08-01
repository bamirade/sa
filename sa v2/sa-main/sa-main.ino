#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "PageIndex.h"

const char *ssid = "Led Matrix";
const char *password = "1234567890";

ESP8266WebServer server(80);

#define NUM_ROWS 2
String Text[NUM_ROWS] = {
    "Welcome_To_AMA_Santiago",
    "Please_proceed_quietly_classes_are_in_session"};

unsigned long lastSendTime = 0;

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
      Serial.printf("ROW%d:%s\n", row + 1, Text[row].c_str());
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
  Serial.begin(9600);
  delay(1000);

  WiFi.softAP(ssid, password);
  IPAddress apip = WiFi.softAPIP();

  Serial.println("ESP8266 ready");
  Serial.println("Connect to: " + String(ssid));
  Serial.println("IP Address: " + apip.toString());

  server.on("/", handleRoot);
  server.on("/setText", handle_Incoming_Text);
  server.begin();
}

void loop()
{
  server.handleClient();

  if (millis() - lastSendTime > 1000)
  {
    Serial.print("ROW1:");
    Serial.println(Text[0]);

    delay(50);
    Serial.print("ROW2:");
    Serial.println(Text[1]);

    lastSendTime = millis();
  }
}
