// ESP32 MOGA gamepad library
#ifndef __ESP32_MOGA__
#define __ESP32_MOGA__

#include "BluetoothSerial.h"

#define RECVMSG_LEN 12
#define SENDMSG_LEN 5
#define RECVBUF_LEN 256
#define MOGABUF_LEN 8

#define CMD_POLL_DIGITAL 65    // poll controller state, digital triggers  (12b response)
#define CMD_INIT 67			   // change controller ID
#define CMD_LISTEN_DIGITAL 68  // listen mode, digital triggers  (12b response)
#define CMD_POLL_ANALOG 69     // poll controller state, analog triggers  (14b response)
#define CMD_LISTEN_ANALOG 70   // listen mode, analog triggers  (14b response)
#define CMD_RESET 65
#define CMD_FLUSH 68

// Callback function prototype
typedef void (SPP_CALLBACK)();

class MogaGamepad
{
    public:
		int pollInterval = 50;

		void begin(String remoteName, String localName = "ESP32");
		void begin(uint8_t remoteAddress[], String localName = "ESP32");
        void poll();
		bool changed();
		bool isConnected();
		void setOnConnected(SPP_CALLBACK *callback);
		void setOnDisconnected(SPP_CALLBACK *callback);
		void printState();

		bool A();
        bool B();
        bool X();
        bool Y();
        bool L1();
        bool R1();
        bool L2();
        bool R2();
        bool Select();
        bool Start();
        bool DPadUp();
        bool DPadDown();
        bool DPadLeft();
        bool DPadRight();
		bool AnyButton() { return A() || B() || X() || Y() || L1() || R1() || L2() || R2() || Select() || Start(); }
		bool Fire() { return A() || L1() || L2(); }
		int X1();
		int Y1();
		int X2();
		int Y2();

    private:
		unsigned long prevMillis = 0;
};

#endif // __ESP32_MOGA__