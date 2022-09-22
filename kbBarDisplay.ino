const char PROGRAMNAME[]    = "midi display";
const char PROGRAMVERSION[] = "// kb 20220922";
const char PROGRAMMANUAL[]  = "  controls: none\n";

#include <SoftwareSerial.h>
#include <MIDI.h>

String timecodeBar = "...|..|..|...";

const unsigned int kbUpdateInterval = 100;
unsigned long kbPreviousMillis = 0;

// MIDI In goes to GPIO12
// old: (D7 on NodeMCU v3). We do not need to send anything, so TX=-1.
#define MYPORT_TX -1
#define MYPORT_RX 12
SoftwareSerial myPort(MYPORT_RX, MYPORT_TX);
MIDI_CREATE_INSTANCE(SoftwareSerial, myPort, MIDI);

void setup() {
  initSerial();
  initMIDI();
  showAbout();
  Serial.println("Let's start!\n");
}

void initSerial() {
  Serial.begin(57600);
  Serial.setTimeout(50);
  Serial.println("Serial initialized!");
}

void initMIDI() {
  MIDI.turnThruOff();
  MIDI.begin(MIDI_CHANNEL_OMNI);
}

void showAbout() {
  Serial.println("\n\n");
  Serial.println(PROGRAMNAME);
  Serial.println(PROGRAMVERSION);
  Serial.println(PROGRAMMANUAL);
}

void loop() {
  if (MIDI.read()) {               // Is there a MIDI message incoming ?
    unsigned char midiType = MIDI.getType(); // + MIDI.getChannel();
    unsigned char midiChannel = MIDI.getChannel();
    unsigned char midiData1 = MIDI.getData1();
    unsigned char midiData2 = MIDI.getData2();
    
    if (midiType == 0xb0) {
      // Serial.println((String)timecodeBar + (String)" <#" + (String)midiType + " " + (String)midiChannel + " " + (String)midiData1 + " " + (String)midiData2);
      if ((midiData1 >= 0x40) && (midiData1 <= 0x49)) {   // timecode data
        setTimecodeBar(midiData1, midiData2);
        Serial.println(timecodeBar);
      }
    }
  }
}

void setTimecodeBar(unsigned char midiData1, unsigned char midiData2) {
  if (midiData1 & 0x40) {
    if ((midiData2 == 0x20) || (midiData2 == 0x40)) { // space or "@"
      midiData2 = 0x20;
    } else {
      midiData2 = (midiData2 & 0x0f) + 0x30; // "0"-"9" only (even if it allows 0x0a-0x0f which goes to ":,<=>?" also)
    };
    
    int newDigitPosition = -1;
    switch (midiData1) {
      case 0x4b:  // left assign char, not used
        // disp2_in[0] = midiData2a2;
        // disp2.value(disp2_in);
        break;
      case 0x4a:  // left assign char, not used
        // disp2_in[1] = midiData2;
        // disp2.value(disp2_in);
        break;
      case 0x49:  // time digit 10 (msb)
        newDigitPosition = 0;
        break;
      case 0x48:  // time digit 9
        newDigitPosition = 1;
        break;
      case 0x47:  // time digit 8
        newDigitPosition = 2;
        break;
      case 0x46:  // time digit 7
        newDigitPosition = 4;
        break;
      case 0x45:  // time digit 6
        newDigitPosition = 5;
        break;
      case 0x44:  // time digit 5
        newDigitPosition = 7;
        break;
      case 0x43:  // time digit 4
        newDigitPosition = 8;
        break;
      case 0x42:  // time digit 3
        newDigitPosition = 10;
        break;
      case 0x41:  // time digit 2
        newDigitPosition = 11;
        break;
      case 0x40:  // time digit 1 (lsb)
        newDigitPosition = 12;
        break;
      default:
        break;
    }
    if (newDigitPosition == -1 ) { return; }
    timecodeBar[newDigitPosition] = midiData2;
  }
}
