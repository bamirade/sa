#include <DMD2.h>
#include <fonts/Arial14.h>
#include <SoftwareSerial.h>

#define DISPLAYS_WIDE 5
#define DISPLAYS_HIGH 1
SPIDMD dmd(DISPLAYS_WIDE, DISPLAYS_HIGH);

SoftwareSerial SerialUno2(2, 3);

char row1Text[100] = "Welcome_To_AMA_Santiago";
String buffer = "";

void setup()
{
    Serial.begin(9600);
    SerialUno2.begin(9600);
    delay(500);

    dmd.setBrightness(255);
    dmd.selectFont(Arial14);
    dmd.begin();
}

void loop()
{
    while (Serial.available())
    {
        char c = Serial.read();
        buffer += c;

        if (c == '\n')
        {
            buffer.trim();
            if (buffer.startsWith("ROW1:"))
            {
                buffer.remove(0, 5);
                buffer.trim();
                buffer.toCharArray(row1Text, sizeof(row1Text));
            }
            else if (buffer.startsWith("ROW2:"))
            {
                SerialUno2.println(buffer);
            }
            buffer = "";
        }
    }

    scrollTextRow(row1Text, 0, 50);
}

uint32_t scrollPos = 0;
uint32_t prevMillis = 0;

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
