// TCD
#include <Crypto.h>
#include <EAX.h>
#include <AES.h>
#include <string.h>

// Turns the 'PRG' button into the power button, long press is off
#define HELTEC_POWER_BUTTON  // must be before "#include <heltec.h>"
#include <heltec.h>
#include <esp32-hal.h>
#include <EAX.h>

// Pause between transmited packets in seconds.
// Set to zero to only transmit a packet when pressing the user button
// Will not exceed 1% duty cycle, even if you set a lower value.
#define PAUSE 0

// Frequency in MHz. Keep the decimal point to designate float.
// Check your own rules and regulations to see what is legal where you are.
#define FREQUENCY 866.3  // for Europe
// #define FREQUENCY           905.2       // for US

// LoRa bandwidth. Keep the decimal point to designate float.
// Allowed values are 7.8, 10.4, 15.6, 20.8, 31.25, 41.7, 62.5, 125.0, 250.0 and 500.0 kHz.
#define BANDWIDTH 250.0

// Number from 5 to 12. Higher means slower but higher "processor gain",
// meaning (in nutshell) longer range and more robust against interference.
#define SPREADING_FACTOR 10

// Transmit power in dBm. 0 dBm = 1 mW, enough for tabletop-testing. This value can be
// set anywhere between -9 dBm (0.125 mW) to 22 dBm (158 mW). Note that the maximum ERP
// (which is what your antenna maximally radiates) on the EU ISM band is 25 mW, and that
// transmissting without an antenna can damage your hardware.
#define TRANSMIT_POWER 22


#define DEVICE_ID "ZIC5154"

#define MSG_TYPE_POSITION "MT01"
#define MSG_TYPE_PING "MT02"
#define MSG_TYPE_TEST "MT03"


#define DSP_HOME 0
#define DSP_SEND_MESSAGE 1




EAX<AES256> eax;

String rxdata;
volatile bool rxFlag = false;
long counter = 0;
uint64_t last_tx = 0;
uint64_t tx_time;
uint64_t minimum_pause = 10;
bool flashLight = 0;
bool menu = 0;
bool submenu = 0;

string  channel = "";
long    msg_counter = 0;


void setup() {
  heltec_setup();
  RADIOLIB_OR_HALT(radio.begin());
  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  float vbat = heltec_vbat();
  float temperature = temperatureRead();
  float vbatPerc = heltec_battery_percent();
  display.printf("BAT: %.0f%% %.2fV CPU:%.0f°C\n", vbatPerc, vbat, temperature);
}

void loop() {
  heltec_loop();

  handleButtons();

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {

  }

}

// Can't do Serial or display things here, takes too much time for the interrupt
void rx() {
  rxFlag = true;
}


