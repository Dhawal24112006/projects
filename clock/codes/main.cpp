// 7447 pins
const int PIN_7447_A = 2;
const int PIN_7447_B = 3;
const int PIN_7447_C = 4;
const int PIN_7447_D = 5;

// Digit select pins
const int PIN_SEC_UNITS = 6;
const int PIN_SEC_TENS  = 7;
const int PIN_MIN_UNITS = 8;
const int PIN_MIN_TENS  = 9;

// Counters
int secUnits = 0, secTens = 0;
int minUnits = 0, minTens = 0;

unsigned long previousMillis = 0;

void setup() {
  // Segment pins
  for(int pin=PIN_7447_A; pin<=PIN_7447_D; pin++) pinMode(pin, OUTPUT);
  
  // Digit select pins
  pinMode(PIN_SEC_UNITS, OUTPUT);
  pinMode(PIN_SEC_TENS, OUTPUT);
  pinMode(PIN_MIN_UNITS, OUTPUT);
  pinMode(PIN_MIN_TENS, OUTPUT);
  
  // All digits OFF initially
  digitalWrite(PIN_SEC_UNITS, LOW);
  digitalWrite(PIN_SEC_TENS, LOW);
  digitalWrite(PIN_MIN_UNITS, LOW);
  digitalWrite(PIN_MIN_TENS, LOW);
  
  // Optional: start from arbitrary time
  minTens = 0; minUnits = 0;
  secTens = 0; secUnits = 0;
}

void loop() {
  unsigned long currentMillis = millis();

  // --- Update seconds every 1 sec ---
  if(currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    secUnits++;
    if(secUnits > 9) { secUnits = 0; secTens++; }
    if(secTens > 5) { secTens = 0; minUnits++; }
    if(minUnits > 9) { minUnits = 0; minTens++; }
    if(minTens > 5) { minTens = 0; } // max 59:59
  }

  // --- Multiplexing all 4 digits ---
  // Units of seconds
  setUnitsBCD(secUnits);
  digitalWrite(PIN_SEC_UNITS, HIGH);
  digitalWrite(PIN_SEC_TENS, LOW);
  digitalWrite(PIN_MIN_UNITS, LOW);
  digitalWrite(PIN_MIN_TENS, LOW);
  delay(3);

  // Tens of seconds
  setTensBCD(secTens);
  digitalWrite(PIN_SEC_UNITS, LOW);
  digitalWrite(PIN_SEC_TENS, HIGH);
  digitalWrite(PIN_MIN_UNITS, LOW);
  digitalWrite(PIN_MIN_TENS, LOW);
  delay(3);

  // Units of minutes
  setUnitsBCD(minUnits);
  digitalWrite(PIN_SEC_UNITS, LOW);
  digitalWrite(PIN_SEC_TENS, LOW);
  digitalWrite(PIN_MIN_UNITS, HIGH);
  digitalWrite(PIN_MIN_TENS, LOW);
  delay(3);

  // Tens of minutes
  setTensBCD(minTens);
  digitalWrite(PIN_SEC_UNITS, LOW);
  digitalWrite(PIN_SEC_TENS, LOW);
  digitalWrite(PIN_MIN_UNITS, LOW);
  digitalWrite(PIN_MIN_TENS, HIGH);
  delay(3);
}

// --- Units 0-9 Boolean logic ---
void setUnitsBCD(int val) {
  int W = (val >> 0) & 1;
  int X = (val >> 1) & 1;
  int Y = (val >> 2) & 1;
  int Z = (val >> 3) & 1;

  int A = !W;
  int B = (W && !X && !Z) || (!W && X);
  int C = (!X && Y)  (!W && Y)  (W && X && !Y);
  int D = (!W && Z) || (W && X && Y);

  digitalWrite(PIN_7447_A, A ? HIGH : LOW);
  digitalWrite(PIN_7447_B, B ? HIGH : LOW);
  digitalWrite(PIN_7447_C, C ? HIGH : LOW);
  digitalWrite(PIN_7447_D, D ? HIGH : LOW);
}

// --- Tens 0-5 Boolean logic ---
void setTensBCD(int val) {
  int W = (val >> 0) & 1; // 0-5 fits in 3 bits
  int X = (val >> 1) & 1;
  int Y = (val >> 2) & 1;

  int A = !W;
  int B = (W && !X && !Y) || (!W && X);
  int C = (W && X) || (!W && !X && Y);
  int D = 0;

  digitalWrite(PIN_7447_A, A ? HIGH : LOW);
  digitalWrite(PIN_7447_B, B ? HIGH : LOW);
  digitalWrite(PIN_7447_C, C ? HIGH : LOW);
  digitalWrite(PIN_7447_D, D ? HIGH : LOW);
}
