// Pins for 7447 inputs (shared for all digits)
const int PIN_A = 2;
const int PIN_B = 3;
const int PIN_C = 4;
const int PIN_D = 5;

// Enable pins for multiplexing
const int EN_SEC_ONES  = 6;
const int EN_SEC_TENS  = 7;
const int EN_MIN_ONES  = 8;
const int EN_MIN_TENS  = 9;
const int EN_HR_ONES   = 10;
const int EN_HR_TENS   = 11;

// ------------------ Current States ------------------
// Seconds
int W1=0, X1=0, Y1=0, Z1=0;        // ones 0-9
int W2=0, X2=0, Y2=0;              // tens 0-5

// Minutes
int W3=0, X3=0, Y3=0, Z3=0;        // ones 0-9
int W4=0, X4=0, Y4=0;              // tens 0-5

// Hours
int W5=0, X5=0, Y5=0, Z5=0;        // ones 0-9 (or 0-3 if tens = 2)
int W6=0, X6=0, Y6=0, Z6=0;        // tens 0-2 (we only use W6,X6,Y6, Boolean logic will ensure 0-2)

// Timer for 1-second increment
unsigned long lastSecUpdate = 0;

void setup() {
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(PIN_C, OUTPUT);
  pinMode(PIN_D, OUTPUT);

  pinMode(EN_SEC_ONES, OUTPUT);
  pinMode(EN_SEC_TENS, OUTPUT);
  pinMode(EN_MIN_ONES, OUTPUT);
  pinMode(EN_MIN_TENS, OUTPUT);
  pinMode(EN_HR_ONES, OUTPUT);
  pinMode(EN_HR_TENS, OUTPUT);

  // All disabled initially
  digitalWrite(EN_SEC_ONES, LOW);
  digitalWrite(EN_SEC_TENS, LOW);
  digitalWrite(EN_MIN_ONES, LOW);
  digitalWrite(EN_MIN_TENS, LOW);
  digitalWrite(EN_HR_ONES, LOW);
  digitalWrite(EN_HR_TENS, LOW);
}

void loop() {
  // ---------------- Multiplexing ----------------
  showSecOnes();  delay(3);
  showSecTens();  delay(3);
  showMinOnes();  delay(3);
  showMinTens();  delay(3);
  showHrOnes();   delay(3);
  showHrTens();   delay(3);

  // ---------------- Update Counter Every 1 Second ----------------
  if (millis() - lastSecUpdate >= 1000) {
    lastSecUpdate = millis();

    // ----- SECONDS ONES (0-9) -----
    int nextW1 = !W1;
    int nextX1 = (W1 && !X1 && !Z1) || (!W1 && X1);
    int nextY1 = (!X1 && Y1) || (!W1 && Y1) || (W1 && X1 && !Y1);
    int nextZ1 = (!W1 && Z1) || (W1 && X1 && Y1);
    W1=nextW1; X1=nextX1; Y1=nextY1; Z1=nextZ1;

    // ----- SECONDS TENS (0-5) -----
    if (W1==0 && X1==0 && Y1==0 && Z1==0) { // ones rolled over
      int nextW2 = !W2;
      int nextX2 = (W2 && !X2 && !Y2) || (!W2 && X2);
      int nextY2 = (W2 && X2) || (!W2 && !X2 && Y2);
      W2=nextW2; X2=nextX2; Y2=nextY2;
    }

    // ----- MINUTES ONES (0-9) -----
    if ((W1==0 && X1==0 && Y1==0 && Z1==0) && (W2==0 && X2==0 && Y2==0)) {
      int nextW3 = !W3;
      int nextX3 = (W3 && !X3 && !Z3) || (!W3 && X3);
      int nextY3 = (!X3 && Y3) || (!W3 && Y3) || (W3 && X3 && !Y3);
      int nextZ3 = (!W3 && Z3) || (W3 && X3 && Y3);
      W3=nextW3; X3=nextX3; Y3=nextY3; Z3=nextZ3;

      // ----- MINUTES TENS (0-5) -----
      if (W3==0 && X3==0 && Y3==0 && Z3==0) { // ones rolled over
        int nextW4 = !W4;
        int nextX4 = (W4 && !X4 && !Y4) || (!W4 && X4);
        int nextY4 = (W4 && X4) || (!W4 && !X4 && Y4);
        W4=nextW4; X4=nextX4; Y4=nextY4;
      }

      // ----- HOURS ONES (0-9 or 0-3 if tens=2) -----
      if ((W3==0 && X3==0 && Y3==0 && Z3==0) && (W4==0 && X4==0 && Y4==0)) {
        if (X6==0) { // tens=0 or 1, ones logic: 0-9
          int nextW5 = !W5;
          int nextX5 = (W5 && !X5 && !Z5) || (!W5 && X5);
          int nextY5 = (X5 && Y5) || (!W5 && Y5) || (W5 && X5 && !Y5);
          int nextZ5 = (!W5 && Z5) || (W5 && X5 && Y5);
          W5=nextW5; X5=nextX5; Y5=nextY5; Z5=nextZ5;
        } else { // tens=2, ones logic: 0-3
          int nextW5 = !W5;
          int nextX5 = (W5 && !X5) || (!W5 && X5);
          int nextY5 = 0;
          int nextZ5 = 0;
          W5=nextW5; X5=nextX5; Y5=nextY5; Z5=nextZ5;
        }

        // ----- HOURS TENS (0-2) -----
        if (W5==0 && X5==0 && Y5==0 && Z5==0) { // ones rolled over
          if (W6==0 && X6==0 && Y6==0) {
            int nextW6 = !W6;
            int nextX6 = (W6 && !X6) || (!W6 && X6);
            int nextY6 = 0;
            W6=nextW6; X6=nextX6; Y6=nextY6;
          }
        }
      }
    }
  }
}

// ---------------- Display Functions ----------------
void showSecOnes() { displayDigit(W1,X1,Y1,Z1,EN_SEC_ONES); }
void showSecTens() { displayDigit(W2,X2,Y2,0,EN_SEC_TENS); }
void showMinOnes() { displayDigit(W3,X3,Y3,Z3,EN_MIN_ONES); }
void showMinTens() { displayDigit(W4,X4,Y4,0,EN_MIN_TENS); }
void showHrOnes()  { displayDigit(W5,X5,Y5,Z5,EN_HR_ONES); }
void showHrTens()  { displayDigit(W6,X6,Y6,0,EN_HR_TENS); }

void displayDigit(int A,int B,int C,int D,int EN) {
  digitalWrite(PIN_A,A); digitalWrite(PIN_B,B); digitalWrite(PIN_C,C); digitalWrite(PIN_D,D);
  // Disable all others
  digitalWrite(EN_SEC_ONES,LOW); digitalWrite(EN_SEC_TENS,LOW);
  digitalWrite(EN_MIN_ONES,LOW); digitalWrite(EN_MIN_TENS,LOW);
  digitalWrite(EN_HR_ONES,LOW); digitalWrite(EN_HR_TENS,LOW);
  digitalWrite(EN,HIGH);
}
