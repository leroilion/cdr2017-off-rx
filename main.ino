#include <Arduino.h>
#include <VirtualWire.h>

#define RESTART_PIN 5

#define BUFFER_SIZE 4

void setup()
{
    /// Init serial and send version number
    Serial.begin(115200);
    Serial.print(F("VERSION : "));
    Serial.println(F(VERSION));

    pinMode(RESTART_PIN, INPUT);
    digitalWrite(RESTART_PIN, HIGH);

    vw_setup(2000);
    vw_rx_start();
}

void loop()
{
    static unsigned long lastUpdate = 0;
    uint16_t power = 0;
    uint16_t direction = 0;
    boolean received = false;
    boolean activateRobot = true;
    uint8_t buf[BUFFER_SIZE];
    uint8_t buflen = BUFFER_SIZE;
    if (vw_wait_rx_max(100)) // Si un message est reçu dans les 100ms qui viennent
    {
        if (vw_get_message(buf, &buflen)) // On copie le message, qu'il soit corrompu ou non
        {
            power = buf[1];
            power = power << 8;
            power += buf[0];
            direction = buf[3];
            direction = (direction << 8);
            direction += buf[2];
            received = true;
        }
    }

    if( millis() - lastUpdate > 500 )
    {
        // Stop robot
        activateRobot = false;
        Serial.println(F("Stop robot"));
    }

    if( activateRobot && received )
    {
        lastUpdate = millis();
        Serial.println(F("Set command"));
        Serial.print(power);
        Serial.print(F("|"));
        Serial.println(direction);
    }

    if( digitalRead(RESTART_PIN) == LOW )
    {
        Serial.println(F("Restart robot"));
        lastUpdate = millis();
    }
}
