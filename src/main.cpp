#include <Arduino.h>
#include <EEPROM.h>
/*
 * Define macros for input and output pin etc.
 */
#include "PinDefinitionsAndMore.h"

#include <IRremote.h>

#if defined(APPLICATION_PIN)
#define RELAY_PIN APPLICATION_PIN
#else
#define RELAY_PIN 5
#endif
#define EEPROM_SIZE 4
uint8_t load1_pin = 13, load2_pin = 13, load3_pin = 13, load4_pin = 13;
uint8_t load1 = 0, load2 = 0, load3 = 0, load4 = 0;
int cmd=0x0;
void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(RELAY_PIN, OUTPUT);
    pinMode(load1_pin, OUTPUT);
    pinMode(load2_pin, OUTPUT);
    pinMode(load3_pin, OUTPUT);
    pinMode(load4_pin, OUTPUT);

    Serial.begin(115200);
    
#if defined(__AVR_ATmega32U4__) || defined(SERIAL_USB) || defined(SERIAL_PORT_USBVIRTUAL) || defined(ARDUINO_attiny3217)
    delay(4000); // To be able to connect Serial monitor after reset or power up and before first print out. Do not wait for an attached Serial Monitor!
#endif
    // Just to know which program is running on my Arduino
    Serial.println(F("START " __FILE__ " from " __DATE__ "\r\nUsing library version " VERSION_IRREMOTE));

    IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver, enable feedback LED, take LED feedback pin from the internal boards definition

    Serial.print(F("Ready to receive IR signals at pin "));
    Serial.println(IR_RECEIVE_PIN);
    EEPROM.begin(EEPROM_SIZE);
    load1 = EEPROM.read(0);
    load2 = EEPROM.read(1);
    load3 = EEPROM.read(2);
    load4 = EEPROM.read(3);
}

int on = 0;
unsigned long last = millis();

void loop()
{
    if (IrReceiver.decode())
    {
        // If it's been at least 1/4 second since the last
        // IR received, toggle the relay
        if (millis() - last > 250)
        {
            on = !on;
            Serial.print(F("Switch relay "));
            if (on)
            {
                digitalWrite(RELAY_PIN, HIGH);
                Serial.println(F("on"));
            }
            else
            {
                digitalWrite(RELAY_PIN, LOW);
                Serial.println(F("off"));
            }
            IrReceiver.printIRResultShort(&Serial);
            Serial.println();
            if (IrReceiver.decodedIRData.protocol != NEC)
            {
                // We have an unknown protocol, print more info
                IrReceiver.printIRResultRawFormatted(&Serial, true);
            }
            else
            {
                cmd=IrReceiver.decodedIRData.command;
                if (cmd == 0x45)
                {
                    load1 = !load1;
                    EEPROM.write(0, load1);
                    EEPROM.commit();
                }
                if (cmd == 0x46)
                {
                    load2 = !load2;
                    EEPROM.write(1, load2);
                    EEPROM.commit();
                }
                if (cmd == 0x47)
                {
                    load3 = !load3;
                    EEPROM.write(2, load3);
                    EEPROM.commit();
                }
                if (cmd == 0x44)
                {
                    load4 = !load4;
                    EEPROM.write(3, load4);
                    EEPROM.commit();
                }
                Serial.println("******");
                for (int i=0;i<4;i++){
                    Serial.println(EEPROM.read(i));
                }
                Serial.println("******");

            }
        }
        last = millis();
        IrReceiver.resume(); // Enable receiving of the next value
    }
}
//  45,46,47
//  44,40,43
//  7, 15, 9
//  16,19, D
//    18
//  8,1C,5A
//    52