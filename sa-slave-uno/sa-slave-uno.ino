#include <DMD2.h>
#include <fonts/Arial14.h>

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);

char row2Text[100] = "Waiting for text...";
uint32_t scrollPos = 0;
uint32_t prevMillis = 0;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(100);
  delay(500);

  dmd.setBrightness(255);
  dmd.selectFont(Arial14);
  dmd.begin();
}

void loop()
{
  if (Serial.available())
  {
    String incoming = Serial.readStringUntil('\n');

    if (incoming.startsWith("ROW2:"))
    {
      incoming.remove(0, 5);
      incoming.trim();
      incoming.toCharArray(row2Text, sizeof(row2Text));
    }
  }

  scrollTextRow(row2Text, 0, 50);
}

void scrollTextRow(const char *text, int y, uint8_t speed)
{
  dmd.selectFont(Arial14);
  int width = dmd.width;
  int fullScroll = dmd.stringWidth(text) + width;

  if ((millis() - prevMillis) > speed)
  {
    prevMillis = millis();
    scrollPos = (scrollPos + 1) % fullScroll;

    dmd.clearScreen();
    dmd.drawString(width - scrollPos, y, text);
  }
}
