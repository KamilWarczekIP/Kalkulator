/*
  Universal 8bit Graphics Library (https://github.com/olikraus/u8g2/)
*/

#include <Arduino.h>
#include <U8g2lib.h>
#include <String.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

/*
GPIO26 - > AC
GPIO13- > A
GPIO14- > B
GPIO19 -> C
GPIO12- > D
GPIO27- > E
GPIO4 - > F
GPIO2 -> G
GPIO33- > H
GPIO25- > I

*/
/*
  U8g2lib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
*/


U8G2_SSD1322_NHD_256X64_F_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/5, /* dc=*/17, /* reset=*/15);  // Enable U8G2_16BIT in u8g2.h

const char* KEYS[] = {
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9",
  "00",
  ".",
  "MU",
  "%",
  "=",
  "/",
  "*",
  "-",
  "+",
  "SQRT",
  "<-",
  "CE",
  "MRC",
  "M+",
  "M-",
  "GT",
  "NONE",
};
enum class Key {
  _0,
  _1,
  _2,
  _3,
  _4,
  _5,
  _6,
  _7,
  _8,
  _9,
  _00,
  _DOT,
  _MU,
  _PERCENT,
  _EQUAL,
  _DIVIDE,
  _MUTIPLY,
  _MINUS,
  _PLUS,
  _SQRT,
  _DELETE,
  _CE,
  _MRC,
  _MMINUS,
  _MPLUS,
  _GT,
  NONE,
};
enum InSeg {
  A = 13,
  B = 14,
  C = 19,
  D = 12,
  E = 27,
  F = 4,
  G = 2,
  H = 33,
  I = 25,
  AC = 26,
};

Key getKey() {

  pinMode(InSeg::B, INPUT_PULLDOWN);
  pinMode(InSeg::C, INPUT_PULLDOWN);
  pinMode(InSeg::D, INPUT_PULLDOWN);
  pinMode(InSeg::E, INPUT_PULLDOWN);
  pinMode(InSeg::F, INPUT_PULLDOWN);
  pinMode(InSeg::G, INPUT_PULLDOWN);
  pinMode(InSeg::H, INPUT_PULLDOWN);
  pinMode(InSeg::I, INPUT_PULLDOWN);

  pinMode(InSeg::A, OUTPUT);
  digitalWrite(InSeg::A, HIGH);


  if (digitalRead(InSeg::B) == HIGH) return Key::_5;
  if (digitalRead(InSeg::C) == HIGH) return Key::_4;
  if (digitalRead(InSeg::D) == HIGH) return Key::_3;
  if (digitalRead(InSeg::E) == HIGH) return Key::_2;
  if (digitalRead(InSeg::F) == HIGH) return Key::_1;
  if (digitalRead(InSeg::G) == HIGH) return Key::_0;
  if (digitalRead(InSeg::H) == HIGH) return Key::_00;
  if (digitalRead(InSeg::I) == HIGH) return Key::_DOT;

  digitalWrite(InSeg::A, LOW);
  pinMode(InSeg::A, INPUT_PULLDOWN);

  pinMode(InSeg::B, OUTPUT);
  digitalWrite(InSeg::B, HIGH);


  if (digitalRead(InSeg::C) == HIGH) return Key::_MU;
  if (digitalRead(InSeg::D) == HIGH) return Key::_PERCENT;
  if (digitalRead(InSeg::E) == HIGH) return Key::_EQUAL;
  if (digitalRead(InSeg::F) == HIGH) return Key::_DIVIDE;
  if (digitalRead(InSeg::G) == HIGH) return Key::_MUTIPLY;
  if (digitalRead(InSeg::H) == HIGH) return Key::_PLUS;
  if (digitalRead(InSeg::I) == HIGH) return Key::_MINUS;
 
  digitalWrite(InSeg::B, LOW);
  pinMode(InSeg::B, INPUT_PULLDOWN);

  pinMode(InSeg::C, OUTPUT);
  digitalWrite(InSeg::C, HIGH);

  if (digitalRead(InSeg::F) == HIGH) return Key::_SQRT;
  if (digitalRead(InSeg::H) == HIGH) return Key::_DELETE;
  if (digitalRead(InSeg::I) == HIGH) return Key::_CE;

  digitalWrite(InSeg::C, LOW);
  pinMode(InSeg::C, INPUT_PULLDOWN);

  pinMode(InSeg::D, OUTPUT);
  digitalWrite(InSeg::D, HIGH);


  if (digitalRead(InSeg::G) == HIGH) return Key::_MRC;
  if (digitalRead(InSeg::H) == HIGH) return Key::_MMINUS;
  if (digitalRead(InSeg::I) == HIGH) return Key::_MPLUS;

  digitalWrite(InSeg::D, LOW);
  pinMode(InSeg::D, INPUT_PULLDOWN);

  pinMode(InSeg::E, OUTPUT);
  digitalWrite(InSeg::E, HIGH);


  if (digitalRead(InSeg::F) == HIGH) return Key::_9;
  if (digitalRead(InSeg::G) == HIGH) return Key::_8;
  if (digitalRead(InSeg::H) == HIGH) return Key::_7;
  if (digitalRead(InSeg::I) == HIGH) return Key::_6;

  pinMode(InSeg::F, OUTPUT);
  digitalWrite(InSeg::F, HIGH);

  if (digitalRead(InSeg::G) == HIGH) return Key::_GT;

  digitalWrite(InSeg::E, LOW);
  digitalWrite(InSeg::F, LOW);

  return Key::NONE;
}
double result;
bool display_result = true;
bool error = false;
char operand[20] = "=";
char strn_display[22] = "";

void setup(void) {
  result = 0.0;
  u8g2.begin();
  Serial.begin(9600);
}
double operate(char * operand, double old_result)
{
  double opValue = atof(operand + 1);

  Serial.print(" OBL: ");
  Serial.print(old_result);
  Serial.print(operand[0]);
  Serial.print(opValue);
  switch(operand[0])
  {
    case '=':
      old_result = opValue;
    break;
    case '+':
      old_result += opValue;
    break;
    case '-':
      old_result -= opValue;
    break;
    case '/':
      
      if(opValue == 0.0)
        error = true;
      else
        old_result /= opValue;
    break;
    case '*':
      old_result *= opValue;
    break;
  }
  if(operand[0] != '=')
  {
    operand[1] = '\0';
  }
  Serial.print(" = ");
  Serial.println(old_result);
  dtostrf(old_result, sizeof(strn_display) - 1, 5, strn_display);
  return old_result;
}
void loop(void) {
  Key kwy = getKey();
  if(kwy != Key::NONE)
  {
    Serial.print(KEYS[(int)kwy]);
    switch(kwy)
    {
      case Key::_SQRT:
        result = sqrt(atof(operand));
        dtostrf(result, sizeof(operand) - 1, 5, operand);
      break;
      case Key::_CE:
        result = 0.0;
        operand[0] = '=';
        operand[1] = '\0';
        dtostrf(result, sizeof(strn_display) - 1, 5, strn_display);
      break;
      case Key::_DOT:
      case Key::_00:
      case Key::_0:
      case Key::_1:
      case Key::_2:
      case Key::_3:
      case Key::_4:
      case Key::_5:
      case Key::_6:
      case Key::_7:
      case Key::_8:
      case Key::_9:
        strcat(operand, KEYS[(int)kwy]);
        strcpy(strn_display, operand + 1);
      break;
      case Key::_PLUS:
        if(operand[1] != '\0')
          result = operate(operand, result);
        operand[0] = '+';
        operand[1] = '\0';
      break;
      case Key::_MINUS:
        if(operand[1] != '\0')
          result = operate(operand, result);
        operand[0] = '-';
        operand[1] = '\0';
      break;
      case Key::_MUTIPLY:
        if(operand[1] != '\0')
          result = operate(operand, result);
        operand[0] = '*';
        operand[1] = '\0';
      break;
      case Key::_DIVIDE:
        if(operand[1] != '\0')
          result = operate(operand, result);
        operand[0] = '/';
        operand[1] = '\0';
      break;
      case Key::_EQUAL:
        result = operate(operand, result);
      break;
    }
  }
  
  u8g2.clearBuffer();                         // clear the internal memory
  u8g2.setFont(u8g2_font_cardimon_pixel_tf);  // choose a suitable font
  if(error)
  {
    u8g2.drawStr(0, 48, "ERROR!");        // write something to the internal memory
    delay(1000);
    error = false;
    result = 0.0;
    operand[0] = '=';
    operand[1] = '\0';
  }
  else
  {
    u8g2.drawStr(0, 48, strn_display);        // write something to the internal memory
  }
  u8g2.sendBuffer();                          // transfer internal memory to the display
  delay(100);
}
