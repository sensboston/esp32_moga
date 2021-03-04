#include "esp32_moga.h"

MogaGamepad gamepad;

void OnGamepadConnected()
{
    Serial.print("Gamepad connected\n");;
}

void OnGamepadDisconnected()
{
    Serial.print("Gamepad disconnected\n");
}

void setup() 
{
    Serial.begin(115200);

    gamepad.setOnConnected(OnGamepadConnected);
    gamepad.setOnDisconnected(OnGamepadDisconnected);
    gamepad.begin("Moga 2");
    //gamepad.begin("BD&A");
}

void loop() 
{
    gamepad.poll();
    if (gamepad.changed()) gamepad.printState();
}
