// ESP32 MOGA gamepad library
#include "esp32_moga.h"

static BluetoothSerial bts;
static bool bConnected, bChanged = false;
static uint8_t data[MOGABUF_LEN], oldData[MOGABUF_LEN];
static SPP_CALLBACK *pConnected = NULL, *pDisconnected = NULL;

static void sendMessage(uint8_t code)
{
    uint8_t msg[SENDMSG_LEN];
    uint8_t chksum = 0;

    msg[0] = 0x5a;            // identifier
    msg[1] = SENDMSG_LEN;     // message length - always 5
    msg[2] = code;            // command to send
    msg[3] = 1;               // controller id

    // calculate a check sum
    for (uint8_t i = 0; i < SENDMSG_LEN-1; i++) chksum = (byte)(msg[i] ^ chksum);
    msg[4] = chksum;
    bts.write(msg, SENDMSG_LEN);
}

static bool receiveMessage()
{
	uint8_t i, chksum = 0;
    uint8_t recvBuf[RECVBUF_LEN];
	memset(recvBuf, 0, RECVBUF_LEN);

	uint8_t len = bts.available();
	if (len >= RECVMSG_LEN)
	{
		for (i=0; i < len && i < RECVBUF_LEN - 1; i++) recvBuf[i]=bts.read();
		// Calculate checksum
        for (i=0; i < RECVMSG_LEN - 1; i++) chksum = (byte)(recvBuf[i] ^ chksum);
        // Check for bad data
        if (recvBuf[0] != 0x7a || recvBuf[RECVMSG_LEN - 1] != chksum) return false;
		memcpy(data, &recvBuf[4], MOGABUF_LEN);
		for (i=0; i<6; i++) bChanged |= (oldData[i] != data[i]);
		memcpy(oldData, data, MOGABUF_LEN);
		return true;
	}
	return false;
}

static void spp_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
  	if (event == ESP_SPP_CLOSE_EVT) 
	{
		bConnected = false;
    	if (pDisconnected) (*pDisconnected)();
	}
  	else if (event == ESP_SPP_OPEN_EVT)
	{
		bConnected = true;
		if (pConnected) (*pConnected)();
		sendMessage(CMD_INIT);
		sendMessage(CMD_RESET);
	}
}

void MogaGamepad::begin(String remoteName, String localName)
{
	bConnected = false;
	// Start Bleutooth Serial in SPP mode
	bts.register_callback(spp_callback);
    bts.enableSSP();
    bts.begin(localName, true);
	bts.connect(remoteName);
}

bool MogaGamepad::isConnected() { return bConnected; }

void MogaGamepad::setOnConnected(SPP_CALLBACK *callback) { pConnected = callback; }

void MogaGamepad::setOnDisconnected(SPP_CALLBACK *callback) { pDisconnected = callback; }

void MogaGamepad::poll()
{
	bChanged = false;
	if (bConnected && millis()-prevMillis > pollInterval)
	{
		prevMillis = millis();
		sendMessage(CMD_FLUSH);
		if (!receiveMessage()) sendMessage(CMD_RESET);
	}
}

bool MogaGamepad::changed() { return bChanged; }

bool MogaGamepad::A() { return (bool)((data[0] >> 2) & 1); }
bool MogaGamepad::B() { return (bool)((data[0] >> 1) & 1); }
bool MogaGamepad::X() { return (bool)((data[0] >> 3) & 1); }
bool MogaGamepad::Y() { return (bool)((data[0] >> 0) & 1); }
bool MogaGamepad::L1() { return (bool)((data[0] >> 6) & 1); }
bool MogaGamepad::R1() { return (bool)((data[0] >> 7) & 1); }
bool MogaGamepad::L2() { return (bool)((data[1] >> 4) & 1); }
bool MogaGamepad::R2() { return (bool)((data[1] >> 5) & 1); }
bool MogaGamepad::Select() { return (bool)((data[0] >> 5) & 1); }
bool MogaGamepad::Start() { return (bool)((data[0] >> 4) & 1); }
bool MogaGamepad::DPadUp() { return (bool)(data[1]  & 1); }
bool MogaGamepad::DPadDown() { return (bool)((data[1] >> 1) & 1); }
bool MogaGamepad::DPadLeft() { return (bool)((data[1] >> 2) & 1); }
bool MogaGamepad::DPadRight() { return (bool)((data[1] >> 3) & 1); }
int MogaGamepad::X1() { return (int8_t) data[2]; };
int MogaGamepad::Y1() { return (int8_t) data[3]; };
int MogaGamepad::X2() { return (int8_t) data[4]; };
int MogaGamepad::Y2() { return (int8_t) data[5]; };

void MogaGamepad::printState()
{
	Serial.printf("A:%x B:%x X:%x Y:%x L1:%x R1:%x L2:%x R2:%x Sel:%x Start:%x dUp:%x dDown:%x dLeft:%x dRight:%x X1:%d Y1:%d X2:%d Y2:%d\n", 
	     A(), B(), X(), Y(), L1(), R1(), L2(), R2(), Select(), Start(), DPadUp(), DPadDown(), DPadLeft(), DPadRight(), X1(), Y1(), X2(), Y2());
}
