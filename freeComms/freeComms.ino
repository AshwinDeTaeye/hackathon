////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////
static const char PROGMEM _Base64AlphabetTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                                   "abcdefghijklmnopqrstuvwxyz"
                                                   "0123456789+/";

class Base64Class {
public:
  int encode(char *output, char *input, int inputLength) {
    int i = 0, j = 0;
    int encodedLength = 0;
    unsigned char A3[3];
    unsigned char A4[4];

    while (inputLength--) {
      A3[i++] = *(input++);
      if (i == 3) {
        fromA3ToA4(A4, A3);

        for (i = 0; i < 4; i++) {
          output[encodedLength++] = pgm_read_byte(&_Base64AlphabetTable[A4[i]]);
        }

        i = 0;
      }
    }

    if (i) {
      for (j = i; j < 3; j++) {
        A3[j] = '\0';
      }

      fromA3ToA4(A4, A3);

      for (j = 0; j < i + 1; j++) {
        output[encodedLength++] = pgm_read_byte(&_Base64AlphabetTable[A4[j]]);
      }

      while ((i++ < 3)) {
        output[encodedLength++] = '=';
      }
    }
    output[encodedLength] = '\0';
    return encodedLength;
  }

  int decode(char *output, char *input, int inputLength) {
    int i = 0, j = 0;
    int decodedLength = 0;
    unsigned char A3[3];
    unsigned char A4[4];


    while (inputLength--) {
      if (*input == '=') {
        break;
      }

      A4[i++] = *(input++);
      if (i == 4) {
        for (i = 0; i < 4; i++) {
          A4[i] = lookupTable(A4[i]);
        }

        fromA4ToA3(A3, A4);

        for (i = 0; i < 3; i++) {
          output[decodedLength++] = A3[i];
        }
        i = 0;
      }
    }

    if (i) {
      for (j = i; j < 4; j++) {
        A4[j] = '\0';
      }

      for (j = 0; j < 4; j++) {
        A4[j] = lookupTable(A4[j]);
      }

      fromA4ToA3(A3, A4);

      for (j = 0; j < i - 1; j++) {
        output[decodedLength++] = A3[j];
      }
    }
    output[decodedLength] = '\0';
    return decodedLength;
  }

  int encodedLength(int plainLength) {
    int n = plainLength;
    return (n + 2 - ((n + 2) % 3)) / 3 * 4;
  }

  int decodedLength(char *input, int inputLength) {
    int i = 0;
    int numEq = 0;
    for (i = inputLength - 1; input[i] == '='; i--) {
      numEq++;
    }

    return ((6 * inputLength) / 8) - numEq;
  }

  //Private utility functions
  inline void fromA3ToA4(unsigned char *A4, unsigned char *A3) {
    A4[0] = (A3[0] & 0xfc) >> 2;
    A4[1] = ((A3[0] & 0x03) << 4) + ((A3[1] & 0xf0) >> 4);
    A4[2] = ((A3[1] & 0x0f) << 2) + ((A3[2] & 0xc0) >> 6);
    A4[3] = (A3[2] & 0x3f);
  }

  inline void fromA4ToA3(unsigned char *A3, unsigned char *A4) {
    A3[0] = (A4[0] << 2) + ((A4[1] & 0x30) >> 4);
    A3[1] = ((A4[1] & 0xf) << 4) + ((A4[2] & 0x3c) >> 2);
    A3[2] = ((A4[2] & 0x3) << 6) + A4[3];
  }

  inline unsigned char lookupTable(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 71;
    if (c >= '0' && c <= '9') return c + 4;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
  }
};

static Base64Class Base64;

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

/**
 * kIsDebug turns debug prints on or off
 */
constexpr bool kIsDebug = true;
constexpr bool kActivateTempRead = false;

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////

//const String DEVICE_ID = "ALA2";
//const String DEVICE_ID = "MIC1";
const String DEVICE_ID = "REL";
//const String DEVICE_ID = "CAM";
// const String DEVICE_ID = "ASHWIN";
const String SERIAL_MESSAGE_KEYWORD_FOR_OUTPUT = "PAYLOAD==";
constexpr bool kHasMic = false;
constexpr bool kHasCamera = false;
constexpr bool kHasGPS = false;

constexpr bool kIsTracker = false;
const long kGpsCheckInterval = 2000;        // kGpsCheckInterval at which to blink (milliseconds)

// TODO
const String MSG_TYPE_GPS = "04";
const String MSG_TYPE_SEN = "07";
const String MSG_TYPE_TRK = "10";

String MSG_TYPE_TEST = "MT03";

////////////////////////////////////////////////
////////////////////////////////////////////////
////////////////////////////////////////////////


// TCD
#include <SoftwareSerial.h>
#include <TinyGPSPlus.h>
#include <Crypto.h>
#include <EAX.h>
#include <AES.h>
#include <string.h>

// Turns the 'PRG' button into the power button, long press is off
#define HELTEC_POWER_BUTTON  // must be before "#include <heltec.h>"
#include <heltec.h>

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

static const int RXPin = 19, TXPin = 20;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial ss(RXPin, TXPin);

const uint8_t kMessageLength = 60;
const uint8_t kAuthData = 1;

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
bool isTracker = false;

uint64_t msg_counter = 0;

unsigned long previousMillis = 0;  // will store last time GPS was updated

const int microphonePin = 1;
const int kCameraPin = 3;
int valMic = 0;
int micThreshold = 985;
// Add API
void getInfo() {
  both.printf("Frequency: %.2f MHz\n", FREQUENCY);
  both.printf("Bandwidth: %.1f kHz\n", BANDWIDTH);
  both.printf("TX power: %i dBm\n", TRANSMIT_POWER);
  float vbat = heltec_vbat();
  float temperature = kActivateTempRead ? temperatureRead() : 0;
  float vbatPerc = heltec_battery_percent();
  display.printf("BAT: %.0f%% %.2fV CPU:%.0f°C\n", vbatPerc, vbat, temperature);
}

void encryptData(uint8_t *plaintext, uint8_t *ciphertext) {
  uint8_t iv[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                     0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78 };
  uint8_t tag[16] = { 0x00 };
  uint8_t key[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                      0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F };
  uint8_t authdata[20] = { kAuthData };
  // Encrypt
  eax.setKey(key, sizeof(key));
  eax.setIV(iv, sizeof(iv));
  eax.addAuthData(authdata, sizeof(authdata));
  eax.encrypt(ciphertext, plaintext, kMessageLength);
  eax.computeTag(tag, sizeof(tag));

  if (kIsDebug) Serial.print("encryptData - ");
  if (kIsDebug) Serial.println((const char *)ciphertext);
}

void decryptData(uint8_t *plaintext, uint8_t *ciphertext) {
  uint8_t iv[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                     0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F, 0xF4, 0x78 };
  uint8_t tag[16] = { 0x00 };
  uint8_t key[16] = { 0x23, 0x39, 0x52, 0xDE, 0xE4, 0xD5, 0xED, 0x5F,
                      0x9B, 0x9C, 0x6D, 0x6F, 0xF8, 0x0F };
  uint8_t authdata[20] = { kAuthData };

  //Decrypt
  eax.setKey(key, sizeof(key));
  eax.setIV(iv, sizeof(iv));
  eax.addAuthData(authdata, sizeof(authdata));
  eax.decrypt(plaintext, ciphertext, kMessageLength);
  /*
  if (!eax.checkTag(tag, sizeof(tag))) {
    Serial.println("!! data is invalid");
    // The data was invalid - do not use it.
  }
  */
  if (kIsDebug) Serial.print("decryptData: ");
  if (kIsDebug) Serial.println((const char *)plaintext);
}


void sendMessage(const String &messageInClear) {
  if (kIsDebug) Serial.print("sendMessage - ");
  if (kIsDebug) Serial.println(messageInClear);
  if (kIsDebug) Serial.println("strlen(messageInClear)");
  if (kIsDebug) Serial.println(strlen(messageInClear.c_str()));


  uint8_t encryptedData[kMessageLength] = "";

  encryptData((uint8_t *)messageInClear.c_str(), encryptedData);
  const int inputStringLength = kMessageLength;
  const int encodedLength = Base64.encodedLength(inputStringLength) + 1;
  char base64EncodedString[encodedLength];

  if (kIsDebug) Serial.println("before encode");


  Base64.encode(base64EncodedString, (char *)encryptedData, inputStringLength);

  if (kIsDebug) Serial.print("base64EncodedString=");
  if (kIsDebug) Serial.println(base64EncodedString);

  radio.clearDio1Action();

  RADIOLIB(radio.transmit(base64EncodedString, encodedLength)); // this is blocking

  radio.setDio1Action(rx);
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}

void receiveMessage(String messageEncrypted) {
  if (kIsDebug) Serial.print("receiveMessage - ");
  if (kIsDebug) Serial.println(messageEncrypted);

  uint8_t plainText[kMessageLength] = "";
  uint8_t encryptedData[kMessageLength] = "";

  char encryptedDataStr[kMessageLength + 1];  // one extra byte for the null terminator
  memcpy(encryptedDataStr, messageEncrypted.c_str(), kMessageLength);

  encryptedDataStr[kMessageLength] = '\0';  // null terminator

  decryptData(plainText, (unsigned char *)encryptedDataStr);
  display.printf("RX [%s]\n", plainText);
  display.printf(" RSSI: %.2f dBm\n", radio.getRSSI());
  display.printf(" SNR: %.2f dB\n", radio.getSNR());

  Serial.print(SERIAL_MESSAGE_KEYWORD_FOR_OUTPUT);
  Serial.println((const char *)plainText);

  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
}


String makePayload(String payloadType, String channel, String dest_id, String payload) {
  
  return "[" + payloadType + "," + DEVICE_ID + "," + msg_counter + "," + channel + "," + dest_id + "," + payload + "]\0";
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

void sendGpsData() {
  if (kIsDebug) {
    Serial.println("In function SendGPSData()");
    Serial.print("microphone level: ");
    Serial.println(valMic);
  }

  if (kHasGPS) {
    Serial.print(F("Location: "));
    if (gps.location.isValid()) {
      Serial.print(gps.location.lat(), 6);
      Serial.print(F(","));
      Serial.print(gps.location.lng(), 6);
    } else {
      Serial.println(F("INVALID"));
    }
    smartDelay(200);
  }

  // Get the data from IC
  // String assembledMessage = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6) + "," + String(gps.date.month()) + "/" + String(gps.date.day()) + "," + String(gps.date.year()) + "," + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second());

  String assembledMessage = String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6) + "," + valMic;

  if (!kHasGPS) {
    assembledMessage = "51.155469,5.363265,0";
  }

  if (kIsTracker) {
    if (kIsDebug) Serial.println("we are a tracker sending pos");
    if (kIsDebug) Serial.println(assembledMessage);

    sendMessage(makePayload(MSG_TYPE_GPS, "", "", assembledMessage));
  }

  if (kHasMic) {
    // Interval time elapsed, we check the sound
    valMic = analogRead(microphonePin);

    if (valMic > micThreshold) {
      if (kIsDebug) Serial.println("sound detected sending pos");
      if (kIsDebug) Serial.println(assembledMessage);

      sendMessage(makePayload("07", "", "", assembledMessage));
    }
  }

  if (kHasCamera) {
    if (digitalRead(kCameraPin)) { 
        if (kIsDebug) Serial.println("person detected sending pos");
        if (kIsDebug) Serial.println(assembledMessage);
        
        sendMessage(makePayload("07", "", "", assembledMessage));
    }
  }

  /*
      Message type: 
      0: System
        00: alive
        01: ACK ( crc20 message )
        02: Freq hop
        03: Text
        04: GPS
        05: Temp
        06: Person recog
        07: Microphone
      meta
        message type 3ch
        timestamp
        id sender
        channel
      data
        payload type
        payload data
        retry
    */
}

void displayInfo() {
  Serial.print(F("Location: "));
  if (gps.location.isValid()) {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid()) {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  if (gps.time.isValid()) {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  } else {
    Serial.print(F("INVALID"));
  }

  Serial.println();
}

void setup() {
  heltec_setup();
  ss.begin(GPSBaud);

  pinMode(microphonePin, INPUT);
  pinMode(kCameraPin, INPUT);

  RADIOLIB_OR_HALT(radio.begin(434.0, 125.0, 9, 7, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 10, 8 /* default is 8 */
                               ,
                               1.6, false));

  // Set the callback function for received packets
  radio.setDio1Action(rx);
  // Set radio parameters
  RADIOLIB_OR_HALT(radio.setFrequency(FREQUENCY));
  RADIOLIB_OR_HALT(radio.setBandwidth(BANDWIDTH));
  RADIOLIB_OR_HALT(radio.setOutputPower(TRANSMIT_POWER));
  // Start receiving
  RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  float vbat = heltec_vbat();
  float temperature = kActivateTempRead ? temperatureRead() : 0;
  float vbatPerc = heltec_battery_percent();
  display.printf("BAT: %.0f%% %.2fV CPU:%.0f°C\n", vbatPerc, vbat, temperature);
  /*
  while (hs.available()) {
      Serial.println("HS not available");
  }
  */
}

void loop() {
  heltec_loop();

  if (menu == 1 && button.isDoubleClick()) {
    menu = 0;
  } else {
  }
  // Transmit a packet every PAUSE seconds or when the button is pressed
  if ((PAUSE && millis() - last_tx > (PAUSE * 200)) || button.isDoubleClick()) {
    if (menu == 0) {
      menu = 1;
      //display.print("\n\n\n\nMenu\n");
      display.clear();
      display.drawString(0, 0, "-> Send message");
      display.drawString(0, 15, "Received messages");
      display.drawString(0, 30, "Config");
      display.drawString(0, 45, "Tools");
      display.display();
    }
  }
  if (menu == 1 && button.isSingleClick()) {
    display.clear();
    display.drawString(0, 0, " Send message:");
    display.drawString(0, 8, "----------------------------");
    display.drawString(0, 20, "Hello!");
    display.drawString(0, 50, "-> SEND");
    display.display();
  }
  if (menu == 1 && submenu == 1 && button.isSingleClick()) {
    display.clear();
    heltec_led(30);  // 50% brightness is plenty for this LED
    tx_time = millis();

    uint8_t plainText[kMessageLength] = "Hello!";
    uint8_t encryptedData[kMessageLength] = "";
    encryptData(plainText, encryptedData);
    char encryptedDataStr[kMessageLength + 1];  // one extra byte for the null terminator
    memcpy(encryptedDataStr, encryptedData, kMessageLength);
    encryptedDataStr[kMessageLength] = '\0';  // null terminator
    RADIOLIB(radio.transmit(String(counter++).c_str()));
    tx_time = millis() - tx_time;
    display.drawString(0, 20, "Message send!");
    display.display();
    delay(2000);
    button.update();
    display.clear();
    display.drawString(0, 20, "        ");
    display.display();
    menu = 0;
    submenu = 0;
  }
  if (submenu == 0 && button.isSingleClick()) {
    submenu = 1;
  }

  if ((PAUSE && millis() - last_tx > (PAUSE * 200)) || button.isSingleClick() && menu != 1) {
    // In case of button click, tell user to wait
    both.printf("\nTX [%s] ", String(counter).c_str());
    radio.clearDio1Action();
    heltec_led(30);  // 50% brightness is plenty for this LED
    tx_time = millis();
    uint8_t plainText[kMessageLength] = "Test";
    uint8_t encryptedData[kMessageLength] = "";
    String dataToEncrypt = makePayload("payloadType", "channel", "dest_id", "payload");

    uint8_t *plaintext = (uint8_t *)dataToEncrypt.c_str();
    encryptData(plaintext, encryptedData);

    int inputStringLength = sizeof(encryptedData);
    int encodedLength = Base64.encodedLength(inputStringLength);
    char encodedString[encodedLength];

    Base64.encode(encodedString, (char *)encryptedData, inputStringLength);

    RADIOLIB(radio.transmit(encodedString, kMessageLength));
    tx_time = millis() - tx_time;
    heltec_led(0);
    if (_radiolib_status == RADIOLIB_ERR_NONE) {
      display.normalDisplay();
      both.printf("OK (%i ms)\n", tx_time);
      float vbat = heltec_vbat();
      both.printf("Battery: %.2f V\n\n", vbat);
    } else {
      both.printf("fail (%i)\n", _radiolib_status);
    }
    // Maximum 1% duty cycle
    minimum_pause = tx_time * 10;
    last_tx = millis();

    if (kHasGPS) {
      smartDelay(200);
    }

    radio.setDio1Action(rx);
    RADIOLIB_OR_HALT(radio.startReceive(RADIOLIB_SX126X_RX_TIMEOUT_INF));
  }

  // If a packet was received, display it and the RSSI and SNR
  if (rxFlag) {
    heltec_led(30);  // 50% brightness is plenty for this LED
    rxFlag = false;
    radio.readData(rxdata, kMessageLength);

    const int decodedLength = Base64.decodedLength((char *)rxdata.c_str(), rxdata.length());

    if (kIsDebug) Serial.print("decodedLength=");
    if (kIsDebug) Serial.println(decodedLength);

    if (decodedLength > 0) {
      char decodedString[decodedLength];
      Base64.decode(decodedString, (char *)rxdata.c_str(), rxdata.length());
      receiveMessage(decodedString);
    }

    if (kHasGPS) {
      smartDelay(100);
    }

    heltec_led(0);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= kGpsCheckInterval) {
   
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    // Interval time elapsed, we check the GPS
    sendGpsData();
  }
}

// Can't do Serial or display things here, takes too much time for the interrupt
void rx() {
  rxFlag = true;
}
