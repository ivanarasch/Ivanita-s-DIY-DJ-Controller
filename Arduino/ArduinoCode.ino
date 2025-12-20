// Teensy 4.1 — Complete MIDI Controller
// AS5600 #1 on Wire  (SDA=18, SCL=19) -> Jog 1 (MIDI CC #9)
// AS5600 #2 on Wire2 (SDA2=24, SCL2=25) -> Jog 2 (MIDI CC #11)
// VL53L0X #1 on Wire2 -> MIDI CC #10
// VL53L0X #2 on Wire  -> MIDI CC #13
// Trill Bar on Wire2  -> MIDI CC #12
// 20 Buttons -> MIDI Notes
// 14 Potentiometers -> MIDI CCs

#include <Wire.h>
#include "AS5600.h"
#include "Adafruit_VL53L0X.h"
#include <Trill.h>
#include <StevesAwesomeButton.h>
#include <ResponsiveAnalogRead.h>
#include <math.h>

// -------------------- I2C --------------------
#define I2C_BUS_1 Wire    // AS5600 #1 + VL53L0X #2: SDA=18, SCL=19
#define I2C_BUS_2 Wire2   // AS5600 #2 + VL53L0X #1 + Trill : SDA2=24, SCL2=25

// -------------------- Devices --------------------
AS5600 jog1(&I2C_BUS_1);
AS5600 jog2(&I2C_BUS_2);
Adafruit_VL53L0X lox;   // VL53L0X on Wire2
Adafruit_VL53L0X lox2;  // VL53L0X on Wire
Trill trillSensor;

// -------------------- Jog Config --------------------
const int THRESHOLD = 15;
const int FAST_SPEED = 50;
const int JITTER1 = 5;   // jitter for jog1 (CC #9)
const int JITTER2 = 10;  // jitter for jog2 (CC #11) - higher because it's noisier

int last1 = -1, last2 = -1;
long accum1 = 0, accum2 = 0;

// -------------------- Buttons --------------------
StevesAwesomeButton button1(3, 0, INPUT_PULLUP);
StevesAwesomeButton button2(4, 1, INPUT_PULLUP);
StevesAwesomeButton button3(5, 2, INPUT_PULLUP);
StevesAwesomeButton button4(6, 3, INPUT_PULLUP);
StevesAwesomeButton button5(7, 4, INPUT_PULLUP);
StevesAwesomeButton button6(8, 5, INPUT_PULLUP);
StevesAwesomeButton button7(9, 6, INPUT_PULLUP);
StevesAwesomeButton button8(10, 7, INPUT_PULLUP);
StevesAwesomeButton button9(11, 8, INPUT_PULLUP);
StevesAwesomeButton button10(12, 9, INPUT_PULLUP);
StevesAwesomeButton button11(28, 10, INPUT_PULLUP);
StevesAwesomeButton button12(29, 11, INPUT_PULLUP);
StevesAwesomeButton button13(30, 12, INPUT_PULLUP);
StevesAwesomeButton button14(31, 13, INPUT_PULLUP);
StevesAwesomeButton button15(32, 14, INPUT_PULLUP);
StevesAwesomeButton button16(37, 15, INPUT_PULLUP);
StevesAwesomeButton button17(36, 16, INPUT_PULLUP);
StevesAwesomeButton button18(35, 17, INPUT_PULLUP);
StevesAwesomeButton button19(34, 18, INPUT_PULLUP);
StevesAwesomeButton button20(33, 19, INPUT_PULLUP);

StevesAwesomeButton* buttons[20] = {
  &button1, &button2, &button3, &button4,
  &button5, &button6, &button7, &button8,
  &button9, &button10, &button11, &button12,
  &button13, &button14, &button15, &button16,
  &button17, &button18, &button19, &button20
};

int midiNotes[20] = {
  55, 56, 57, 58, 59,
  60, 61, 62, 63, 64,
  65, 66, 67, 68, 69,
  70, 71, 72, 73, 74
};

// -------------------- Potentiometers --------------------
const int potPins[14] = {
  A12, A13, A9, A8, A7, A6, A3,
  A2, A1, A0, A17, A16, A15, A14
};

ResponsiveAnalogRead analogs[14] = {
  ResponsiveAnalogRead(potPins[0], true),
  ResponsiveAnalogRead(potPins[1], true),
  ResponsiveAnalogRead(potPins[2], true),
  ResponsiveAnalogRead(potPins[3], true),
  ResponsiveAnalogRead(potPins[4], true),
  ResponsiveAnalogRead(potPins[5], true),
  ResponsiveAnalogRead(potPins[6], true),
  ResponsiveAnalogRead(potPins[7], true),
  ResponsiveAnalogRead(potPins[8], true),
  ResponsiveAnalogRead(potPins[9], true),
  ResponsiveAnalogRead(potPins[10], true),
  ResponsiveAnalogRead(potPins[11], true),
  ResponsiveAnalogRead(potPins[12], true),
  ResponsiveAnalogRead(potPins[13], true),
};

const uint8_t MIDI_CHANNEL = 1;

int ccNumber[14] = {
  20, 21, 22, 23, 24, 25, 26,
  27, 28, 29, 30, 31, 32, 33
};

int lastCC[14] = {0};

// -------------------- VL53L0X CONFIG --------------------
const uint8_t VL1_CC_NUM = 10;
const int VL1_MIN_MM = 32;
const int VL1_MAX_MM = 300;
const bool VL1_INVERT = false;
const float VL1_ALPHA = 0.25f;
const uint16_t VL1_SEND_MS = 10;
const uint8_t VL1_DELTA_MIN = 1;

const uint8_t VL2_CC_NUM = 13;
const int VL2_MIN_MM = 32;
const int VL2_MAX_MM = 300;
const bool VL2_INVERT = false;
const float VL2_ALPHA = 0.25f;
const uint16_t VL2_SEND_MS = 10;
const uint8_t VL2_DELTA_MIN = 1;

int vl1_lastCC = -1;
elapsedMillis vl1_sinceSend;
float vl1_mmSmooth = NAN;

int vl2_lastCC = -1;
elapsedMillis vl2_sinceSend;
float vl2_mmSmooth = NAN;

// -------------------- TRILL BAR CONFIG --------------------
const uint8_t TRILL_CC_NUM = 12;
const int TRILL_MIN = 0;
const int TRILL_MAX = 3194;
const uint16_t TRILL_READ_MS = 50;

int trill_lastCC = -1;
boolean trill_touchActive = false;
elapsedMillis trill_sinceRead;

// -------------------- Helper Functions --------------------
static inline int clampi(int v, int lo, int hi) {
  return v < lo ? lo : (v > hi ? hi : v);
}

void onButtonPress(int _buttonNum) {
  usbMIDI.sendNoteOn(midiNotes[_buttonNum], 127, MIDI_CHANNEL);
}

void onButtonRelease(int _buttonNum) {
  usbMIDI.sendNoteOff(midiNotes[_buttonNum], 0, MIDI_CHANNEL);
}

uint8_t vl1_mm_to_cc(int mm) {
  mm = clampi(mm, VL1_MIN_MM, VL1_MAX_MM);
  float t = (float)(mm - VL1_MIN_MM) / (float)(VL1_MAX_MM - VL1_MIN_MM);
  int cc = (int)lroundf(t * 127.0f);
  if (VL1_INVERT) cc = 127 - cc;
  return (uint8_t)clampi(cc, 0, 127);
}

uint8_t vl2_mm_to_cc(int mm) {
  mm = clampi(mm, VL2_MIN_MM, VL2_MAX_MM);
  float t = (float)(mm - VL2_MIN_MM) / (float)(VL2_MAX_MM - VL2_MIN_MM);
  int cc = (int)lroundf(t * 127.0f);
  if (VL2_INVERT) cc = 127 - cc;
  return (uint8_t)clampi(cc, 0, 127);
}

void sendCC_VL1(uint8_t ccVal) {
  if (vl1_sinceSend < VL1_SEND_MS) return;
  if (vl1_lastCC < 0 || abs((int)ccVal - vl1_lastCC) >= VL1_DELTA_MIN) {
    usbMIDI.sendControlChange(VL1_CC_NUM, ccVal, MIDI_CHANNEL);
    vl1_lastCC = ccVal;
    vl1_sinceSend = 0;
  }
}

void sendCC_VL2(uint8_t ccVal) {
  if (vl2_sinceSend < VL2_SEND_MS) return;
  if (vl2_lastCC < 0 || abs((int)ccVal - vl2_lastCC) >= VL2_DELTA_MIN) {
    usbMIDI.sendControlChange(VL2_CC_NUM, ccVal, MIDI_CHANNEL);
    vl2_lastCC = ccVal;
    vl2_sinceSend = 0;
  }
}

void sendCC_Trill(uint8_t ccVal) {
  if (ccVal != trill_lastCC) {
    usbMIDI.sendControlChange(TRILL_CC_NUM, ccVal, MIDI_CHANNEL);
    trill_lastCC = ccVal;
  }
}

// -------------------- Jog Wheel Functions --------------------
void sendJog(int ccNum, long &accum) {
  uint8_t cc = 0;
  
  if (accum >= THRESHOLD) {
    cc = (accum > FAST_SPEED) ? 65 : 64;  // fast fwd : fwd
    accum = 0;
  } else if (accum <= -THRESHOLD) {
    cc = (accum < -FAST_SPEED) ? 61 : 62; // fast back : back
    accum = 0;
  }
  
  if (cc != 0) {
    usbMIDI.sendControlChange(ccNum, cc, MIDI_CHANNEL);
  }
}

void updateJog(AS5600 &sensor, int &last, long &accum, int ccNum, int jitter) {
  int raw = sensor.readAngle();
  
  if (last < 0) {
    last = raw;
    return;
  }
  
  int diff = raw - last;
  if (diff > 2048) diff -= 4096;
  if (diff < -2048) diff += 4096;
  
  last = raw;
  
  if (abs(diff) > jitter) {
    accum += diff;
    sendJog(ccNum, accum);
  }
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000) { delay(1); }

  // I2C
  I2C_BUS_1.begin();
  I2C_BUS_1.setClock(400000);
  I2C_BUS_2.begin();
  I2C_BUS_2.setClock(400000);

  // Buttons
  for (int i = 0; i < 20; i++) {
    buttons[i]->pressHandler(onButtonPress);
    buttons[i]->releaseHandler(onButtonRelease);
  }

  // Trill Bar
  int ret = trillSensor.setup(Trill::TRILL_BAR, 0x20, &I2C_BUS_2);
  if (ret != 0) {
    Serial.println("Failed to initialize Trill Bar");
  } else {
    Serial.println("Trill Bar initialized");
  }

  // VL53L0X #1 on Wire2
  if (!lox.begin(0x29, true, &I2C_BUS_2)) {
    Serial.println("Failed to boot VL53L0X #1 (Wire2)");
    while (1) {}
  }
  lox.startRangeContinuous();

  // VL53L0X #2 on Wire
  if (!lox2.begin(0x29, true, &I2C_BUS_1)) {
    Serial.println("Failed to boot VL53L0X #2 (Wire)");
    while (1) {}
  }
  lox2.startRangeContinuous();

}

// -------------------- Loop --------------------
void loop() {
  while (usbMIDI.read()) {}

  // Buttons
  for (int i = 0; i < 20; i++) {
    buttons[i]->process();
  }

  // Pots
  for (int i = 0; i < 14; i++) {
    analogs[i].update();
    if (analogs[i].hasChanged()) {
      int cc = map(analogs[i].getValue(), 0, 1023, 0, 127);
      if (cc != lastCC[i]) {
        usbMIDI.sendControlChange(ccNumber[i], cc, MIDI_CHANNEL);
        lastCC[i] = cc;
      }
    }
  }

  // Jog wheels
  updateJog(jog1, last1, accum1, 9, JITTER1);
  updateJog(jog2, last2, accum2, 11, JITTER2);

  // VL53L0X #1 (Wire2)
  if (lox.isRangeComplete()) {
    int mm = (int)lox.readRange();
    if (mm > 0 && mm < 4000 && mm != 8190) {
      if (isnan(vl1_mmSmooth)) vl1_mmSmooth = (float)mm;
      vl1_mmSmooth = VL1_ALPHA * (float)mm + (1.0f - VL1_ALPHA) * vl1_mmSmooth;
      uint8_t vl1_cc = vl1_mm_to_cc((int)lroundf(vl1_mmSmooth));
      sendCC_VL1(vl1_cc);
    }
  }

  // VL53L0X #2 (Wire)
  if (lox2.isRangeComplete()) {
    int mm = (int)lox2.readRange();
    if (mm > 0 && mm < 4000 && mm != 8190) {
      if (isnan(vl2_mmSmooth)) vl2_mmSmooth = (float)mm;
      vl2_mmSmooth = VL2_ALPHA * (float)mm + (1.0f - VL2_ALPHA) * vl2_mmSmooth;
      uint8_t vl2_cc = vl2_mm_to_cc((int)lroundf(vl2_mmSmooth));
      sendCC_VL2(vl2_cc);
    }
  }

  // Trill Bar
  if (trill_sinceRead >= TRILL_READ_MS) {
    trill_sinceRead = 0;
    trillSensor.read();

    if (trillSensor.getNumTouches() > 0) {
      int rawLocation = trillSensor.touchLocation(0);
      int cc = map(rawLocation, TRILL_MIN, TRILL_MAX, 0, 127);
      cc = constrain(cc, 0, 127);
      sendCC_Trill(cc);
      trill_touchActive = true;
    } else {
      trill_touchActive = false;
    }
  }

  delay(1);
}
