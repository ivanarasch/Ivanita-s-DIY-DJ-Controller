// Teensy 4.1 MIDI Controller
#include <Wire.h>
#include "AS5600.h"
#include "Adafruit_VL53L0X.h"
#include <Trill.h>
#include <StevesAwesomeButton.h>
#include <ResponsiveAnalogRead.h>

AS5600 jog1(&Wire), jog2(&Wire2);
Adafruit_VL53L0X lox, lox2;
Trill trillSensor;

StevesAwesomeButton btn1(3,0,INPUT_PULLUP), btn2(4,1,INPUT_PULLUP), btn3(5,2,INPUT_PULLUP), btn4(6,3,INPUT_PULLUP);
StevesAwesomeButton btn5(7,4,INPUT_PULLUP), btn6(8,5,INPUT_PULLUP), btn7(9,6,INPUT_PULLUP), btn8(10,7,INPUT_PULLUP);
StevesAwesomeButton btn9(11,8,INPUT_PULLUP), btn10(12,9,INPUT_PULLUP), btn11(28,10,INPUT_PULLUP), btn12(29,11,INPUT_PULLUP);
StevesAwesomeButton btn13(30,12,INPUT_PULLUP), btn14(31,13,INPUT_PULLUP), btn15(32,14,INPUT_PULLUP), btn16(37,15,INPUT_PULLUP);
StevesAwesomeButton btn17(36,16,INPUT_PULLUP), btn18(35,17,INPUT_PULLUP), btn19(34,18,INPUT_PULLUP), btn20(33,19,INPUT_PULLUP);
StevesAwesomeButton* btns[] = {&btn1,&btn2,&btn3,&btn4,&btn5,&btn6,&btn7,&btn8,&btn9,&btn10,&btn11,&btn12,&btn13,&btn14,&btn15,&btn16,&btn17,&btn18,&btn19,&btn20};

ResponsiveAnalogRead pots[] = {
  ResponsiveAnalogRead(A12,true), ResponsiveAnalogRead(A13,true), ResponsiveAnalogRead(A9,true), ResponsiveAnalogRead(A8,true),
  ResponsiveAnalogRead(A7,true), ResponsiveAnalogRead(A6,true), ResponsiveAnalogRead(A3,true), ResponsiveAnalogRead(A2,true),
  ResponsiveAnalogRead(A1,true), ResponsiveAnalogRead(A0,true), ResponsiveAnalogRead(A17,true), ResponsiveAnalogRead(A16,true),
  ResponsiveAnalogRead(A15,true), ResponsiveAnalogRead(A14,true)
};

int notes[] = {55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74};
int lastPot[14], lastJ1=-1, lastJ2=-1, lastVL1=-1, lastVL2=-1, lastTrill=-1;
long acc1=0, acc2=0;
float smooth1=0, smooth2=0;
elapsedMillis t1,t2,t3;

void press(int n) { usbMIDI.sendNoteOn(notes[n],127,1); }
void release(int n) { usbMIDI.sendNoteOff(notes[n],0,1); }

void setup() {
  Wire.begin(); Wire.setClock(400000);
  Wire2.begin(); Wire2.setClock(400000);
  for(int i=0; i<20; i++) { btns[i]->pressHandler(press); btns[i]->releaseHandler(release); }
  trillSensor.setup(Trill::TRILL_BAR, 0x20, &Wire2);
  lox.begin(0x29, true, &Wire2); lox.startRangeContinuous();
  lox2.begin(0x29, true, &Wire); lox2.startRangeContinuous();
}

void loop() {
  while(usbMIDI.read()){}
  
  for(int i=0; i<20; i++) btns[i]->process();
  
  for(int i=0; i<14; i++) {
    pots[i].update();
    if(pots[i].hasChanged()) {
      int cc = map(pots[i].getValue(),0,1023,0,127);
      if(cc != lastPot[i]) { usbMIDI.sendControlChange(20+i, cc, 1); lastPot[i] = cc; }
    }
  }
  
  int r1 = jog1.readAngle();
  if(lastJ1 >= 0) {
    int d = r1 - lastJ1;
    if(d > 2048) d -= 4096;
    if(d < -2048) d += 4096;
    if(abs(d) > 5) {
      acc1 += d;
      if(acc1 >= 15) { usbMIDI.sendControlChange(9, acc1>50?65:64, 1); acc1=0; }
      else if(acc1 <= -15) { usbMIDI.sendControlChange(9, acc1<-50?61:62, 1); acc1=0; }
    }
  }
  lastJ1 = r1;
  
  int r2 = jog2.readAngle();
  if(lastJ2 >= 0) {
    int d = r2 - lastJ2;
    if(d > 2048) d -= 4096;
    if(d < -2048) d += 4096;
    if(abs(d) > 10) {
      acc2 += d;
      if(acc2 >= 15) { usbMIDI.sendControlChange(11, acc2>50?65:64, 1); acc2=0; }
      else if(acc2 <= -15) { usbMIDI.sendControlChange(11, acc2<-50?61:62, 1); acc2=0; }
    }
  }
  lastJ2 = r2;
  
  if(t1>=10 && lox.isRangeComplete()) {
    t1=0;
    int mm = lox.readRange();
    if(mm>0 && mm<4000 && mm!=8190) {
      smooth1 = smooth1*0.75 + mm*0.25;
      int cc = map(constrain(smooth1,32,300), 32,300, 0,127);
      if(lastVL1<0 || abs(cc-lastVL1)>=1) { usbMIDI.sendControlChange(10, cc, 1); lastVL1=cc; }
    }
  }
  
  if(t2>=10 && lox2.isRangeComplete()) {
    t2=0;
    int mm = lox2.readRange();
    if(mm>0 && mm<4000 && mm!=8190) {
      smooth2 = smooth2*0.75 + mm*0.25;
      int cc = map(constrain(smooth2,32,300), 32,300, 0,127);
      if(lastVL2<0 || abs(cc-lastVL2)>=1) { usbMIDI.sendControlChange(13, cc, 1); lastVL2=cc; }
    }
  }
  
  if(t3>=50) {
    t3=0;
    trillSensor.read();
    if(trillSensor.getNumTouches()>0) {
      int cc = map(constrain(trillSensor.touchLocation(0),0,3194),0,3194,0,127);
      if(cc != lastTrill) { usbMIDI.sendControlChange(12, cc, 1); lastTrill=cc; }
    }
  }
  
  delay(1);
}
