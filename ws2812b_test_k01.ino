#include <Adafruit_NeoPixel.h>

// one digit is ledsForSegment*7+sum(gapsBetweenSegments) = 5*7+1 = 36 leds.
// 4 digits are 144 leds.
#define ledCount 100 

int demoShowIndex = -10;

const unsigned int ledsForSegment=5;

// before right top (=after middle), before top, before left top, left bottom, bottom, right bottom, after right bottom (before next number)
// I use a gap of 1 led between left top and left bottom segments.
const uint8_t gapsBetweenSegments[7] = { 0, 0, 0, 1, 0, 0, 0 };

// for now, it's unused.
const unsigned int ledsForDigit = ledsForSegment * 7 + gapsBetweenSegments[0]+gapsBetweenSegments[1]+gapsBetweenSegments[2]+gapsBetweenSegments[3]+
                                                       gapsBetweenSegments[4]+gapsBetweenSegments[5]+gapsBetweenSegments[6];

// segments lit up: middle -, (and counterclockwise:) right top, top, left top, left bottom, bottom, right bottom
const uint8_t asciiToSegments[128] = {
  // 0-15     NUL       SOH       STX       ETX       EOT       ENQ       ACK       BEL        BS        HT        LF        VT        FF        CR        SO        SI
         B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, 
  // 16-31    DLE       DC1       DC2       DC3       DC4       NAK       SYN       ETB       CAN        EM       SUB       ESC        FS        GS        RS        US
         B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, 
  // 32-47     SP         !         "         #         $         %         &         '         (         )         *         +         ,         -         .         /
         B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0000000, B0011110, B0110011, B0000000, B0000000, B0000000, B1000000, B0000000, B0000000, 
  // 48-63      0         1         2         3         4         5         6         7         8         9         :         ;         <         =         >         ?
         B0111111, B0100001, B1110110, B1110011, B1101001, B1011011, B1011111, B0110001, B1111111, B1111011, B0000000, B0000000, B0000000, B1000010, B0000000, B0000000, 
  // 64-79      @         A         B         C         D         E         F         G         H         I         J         K         L         M         N         O
         B1111110, B1111101, B1001111, B0011110, B1100111, B1011110, B1011100, B0011111, B1101101, B0100001, B0100011, B0000000, B0001110, B0000000, B0000000, B0111111, 
  // 80-95      P         Q         R         S         T         U         V         W         X         Y         Z         [         \         ]         ^         _
         B1111100, B0000000, B0000000, B1011011, B0000000, B0101111, B0000000, B0000000, B0000000, B1101100, B1110110, B0011110, B0000000, B0110011, B0000000, B0000010, 
  // 96-111     `         a         b         c         d         e         f         g         h         i         j         k         l         m         n         o
         B0000000, B1110111, B1001111, B1000110, B1100111, B1011110, B1011100, B0011111, B1001101, B0000001, B0000011, B0000000, B0001110, B0000000, B0000000, B1000111, 
  // 112-127    p         q         r         s         t         u         v         w         x         y         z         {         |         }         ~       DEL
         B1111100, B0000000, B0000000, B1011011, B0000000, B0000111, B0000000, B0000000, B0000000, B1101100, B1110110, B0000000, B0000000, B0000000, B0000000, B1010010
};

Adafruit_NeoPixel ledLine = Adafruit_NeoPixel(ledCount, A0, NEO_GRB + NEO_KHZ800);
const uint32_t onColor = ledLine.Color(255, 0, 0); // red
const uint32_t offColor = ledLine.Color(0, 0, 0); // off (black)

void lightLeds(unsigned int startOffset, String segmentsString) {
  boolean light;
  Serial.print("lightLeds");
  for (unsigned int segmentIndex=0; segmentIndex<segmentsString.length(); segmentIndex++) {
    light = (segmentsString.substring(segmentIndex,segmentIndex+1) == "1");
    ledLine.setPixelColor(segmentIndex + startOffset, light ? onColor : offColor);
    Serial.print(light ? "1" : "0");
  }
  Serial.println("");
  ledLine.show();
}

String getSegmentsForWholeString(String wholeString) {
  String segmentsForWholeString = "";
  for (unsigned int index=0; index<wholeString.length(); index++) {
    uint8_t asciiCode = min((uint8_t)wholeString[index], 127);  // skip 127-255 ASCII codes

    String segmentsForDigit = getSegmentsForDigit(asciiCode);
    segmentsForWholeString += segmentsForDigit;

    Serial.print((String)"wholeString = " + (String)wholeString + (String)"; index = " + (String)index + (String)"; wholeString[index] = " + (String)wholeString[index] + (String)"; asciiCode = " + (String)asciiCode + (String)": ");
    Serial.println(segmentsForDigit);
  }
  return segmentsForWholeString;
}

String getSegmentsForDigit(uint8_t asciiCode) {
  String segmentsString = "";
  String segmentOnOrOff = "";
  for(uint8_t segment = 0; segment<7; segment++) {
    segmentOnOrOff = ((asciiToSegments[asciiCode] >> (6-segment)) & 1) ? "1" : "0"; // (6-segment), because first (segment=0) is MSB
    segmentsString += repeatString(segmentOnOrOff, ledsForSegment);
    segmentsString += repeatString("0", gapsBetweenSegments[segment]);
  }
  return segmentsString;
}

String repeatString(String string, unsigned int count) {
  String repeatedString = "";
  for (unsigned int index = 0; index < count; index++) {
    repeatedString += string;
  }
  return repeatedString; // + ".";
}

void setup() {
  ledLine.begin();
  ledLine.clear();
  ledLine.show();

  Serial.begin(57600);
  Serial.setTimeout(50);
  Serial.println("Serial initialized!");

  //String segmentsForWholeString = getSegmentsForWholeString(" -1234");
  //Serial.println("Led count: " + (String)segmentsForWholeString.length());
  //Serial.println("LED MAP: " + segmentsForWholeString);
  //lightLeds(0, segmentsForWholeString);
}

void loop() {
  String segmentsForWholeString = getSegmentsForWholeString((String)demoShowIndex + " ");
  Serial.println("Led count: " + (String)segmentsForWholeString.length());
  Serial.println("LED MAP: " + segmentsForWholeString);
  lightLeds(0, segmentsForWholeString);
  
  demoShowIndex++;
  delay(500);
}
