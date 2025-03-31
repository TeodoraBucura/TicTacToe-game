/*^^^^^^^^^^^^^^^^^^^^^^^^^\
|     Tic Tac Toe Game     |
|       for ARDUINO        |
|                          |
| Autor: Bucura Teodora    |
|                          |
\^^^^^^^^^^^^^^^^^^^^^^^^^*/ 

// Biblioteci necesare pentru funcționarea ecranului LCD
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Inițializarea ecranului LCD la adresa I2C specifică (0x27) și cu dimensiunea 16x2
LiquidCrystal_I2C lcd(0x27,16,2); 

// Definim constante pentru identificarea jucătorilor
const int player1 = 1; // player1 - roșu
const int player2 = 2; // player2 - albastru

// Variabile de scor și număr al jocului
int scor1 = 0; // Scor pentru player1
int scor2 = 0; // Scor pentru player2
int nrjoc = 1; // Contor pentru numărul jocului
int mutare = 0;

// Vector care reține starea LED-urilor (0 = stins, 1 = roșu, 2 = albastru)
int lights[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
int flash[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Definirea pinilor pentru butoane și registrii de deplasare
int buttonPin = A0;     // Pinul analog pentru butoane
int button = 0;         // Variabilă pentru a reține butonul apăsat

int latchPin = 8;       // Pinul pentru blocarea (latch) registrului
int clockPin = 12;      // Pinul pentru semnalul de ceas
int dataPin = 11;       // Pinul pentru date

// Variabile pentru controlul fiecărui LED prin shift register
int byte1 = 0;
int byte2 = 0;
int byte3 = 0;

void setup() {
  // Configurarea pinilor ca OUTPUT
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);   
  pinMode(clockPin, OUTPUT);

  // Pornirea comunicației seriale
  Serial.begin(9600);

  // Inițializarea ecranului LCD și afișarea mesajului de start
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2,0);
  lcd.print("Tic Tac Toe");
  lcd.setCursor(4,1);
  lcd.print("Let's play!");
  delay(3000);
}

void loop() {
  // Afișăm scorul curent pe ecranul LCD
  lcd.setCursor(1,0);
  lcd.print("Score:        ");
  lcd.setCursor(8,0);
  lcd.print(scor1);
  lcd.print(" - ");
  lcd.print(scor2);

  // Citim butonul apăsat și pornim jocul
  button = getButtonPush();
  playTicTacToe(); // Funcția principală a jocului
  refreshDisplay(); // Actualizăm LED-urile
  delay(50); // Pauză pentru stabilitate
}

// Funcție pentru citirea butonului apăsat
int getButtonPush() {
  int pressed = -1; // Inițial, nicio apăsare
  int temp = analogRead(buttonPin); // Citirea valorii analogice a pinului pentru butoane

  // Verificăm intervalele pentru fiecare buton si reținem butonul apăsat
  if (temp > 690 && temp < 750) { pressed = 1; } 
    else if (temp > 600 && temp < 630) { pressed = 2; }
    else if (temp > 520 && temp < 580) { pressed = 3; } 
    else if (temp > 490 && temp < 530) { pressed = 4; }
    else if (temp > 420 && temp < 460) { pressed = 5; }
    else if (temp > 300 && temp < 350) { pressed = 6; }
    else if (temp > 280 && temp < 300) { pressed = 7; } 
    else if (temp > 240 && temp < 270) { pressed = 8; } 
    else if (temp > 200 && temp < 230) { pressed = 9; }
  
  return pressed; // Returnăm numărul butonului apăsat
}

// Funcție pentru actualizarea stării LED-urilor
void refreshDisplay() {
  // Resetăm stările byte-urilor folosite pentru shift register
  byte1 = 0;
  byte2 = 0;
  byte3 = 0;
  // Setăm LED-urile în funcție de starea lor: roșu (1) sau albastru (2)
  if (lights[1] == 2){byte1 = byte1 ^ B00000001;}
  if (lights[1] == 1){byte1 = byte1 ^ B00000010;}
  if (lights[2] == 2){byte1 = byte1 ^ B00000100;}
  if (lights[2] == 1){byte1 = byte1 ^ B00001000;}
  if (lights[3] == 2){byte1 = byte1 ^ B00010000;}
  if (lights[3] == 1){byte1 = byte1 ^ B00100000;}
  if (lights[4] == 2){byte1 = byte1 ^ B01000000;}
  if (lights[4] == 1){byte1 = byte1 ^ B10000000;}
  if (lights[5] == 2){byte2 = byte2 ^ B00000001;}
  if (lights[5] == 1){byte2 = byte2 ^ B00000010;}
  if (lights[6] == 2){byte2 = byte2 ^ B00000100;}
  if (lights[6] == 1){byte2 = byte2 ^ B00001000;}
  if (lights[7] == 2){byte2 = byte2 ^ B00010000;}
  if (lights[7] == 1){byte2 = byte2 ^ B00100000;}
  if (lights[8] == 2){byte2 = byte2 ^ B01000000;}
  if (lights[8] == 1){byte2 = byte2 ^ B10000000;}
  if (lights[9] == 2){byte3 = byte3 ^ B00000001;}
  if (lights[9] == 1){byte3 = byte3 ^ B00000010;}

  setLEDs(); // Apelăm funcția pentru a trimite datele către LED-uri
}

// Funcție pentru a trimite datele către registrul de deplasare
void setLEDs() {
  digitalWrite(latchPin, LOW); // Coborâm pinul latch
  shiftOut(dataPin, clockPin, MSBFIRST, byte3); // Transmitem byte3
  shiftOut(dataPin, clockPin, MSBFIRST, byte2); // Transmitem byte2
  shiftOut(dataPin, clockPin, MSBFIRST, byte1); // Transmitem byte1
  digitalWrite(latchPin, HIGH); // Ridicăm pinul latch pentru a actualiza LED-urile
}

// Funcția principală de joc
void playTicTacToe() {
  int gameState = 1; // Variabilă pentru starea jocului
  clearBoard();      // Curățăm tabla de joc la începutul fiecărui joc
      // Contor pentru mutările făcute
  // Vedem al catelea joc este in serial monitor
  Serial.print("Numarul jocului: ");
  Serial.println(nrjoc);
  // Determinăm cine începe primul în funcție de paritatea numărului de jocuri (nrjoc)
  
  
  if (nrjoc % 2 == 1) {
    lcd.setCursor(4, 1);
    lcd.print("Player 1.  ");
    Serial.println("Joc nou! Player 1 (rosu) începe jocul.");
  } else if (nrjoc%2==0) {
    lcd.setCursor(4, 1);
    lcd.print("Player 2.  ");
    Serial.println("Joc nou! Player 2 (albastru) începe jocul.");
  }

  // Loop principal de joc până la finalizarea meciului
  while (gameState == 1) {
    button = -1;
    refreshDisplay();   // Actualizăm starea LED-urilor
    button = getButtonPush(); // Citim butonul apăsat
    // Verificăm dacă butonul apăsat este valid și LED-ul corespunzător este stins
    if (button!=-1 && lights[button] == 0) {
      mutare++; // Incrementăm contorul mutărilor
      if (mutare % 2 == 1){
        lights[button] = 1;  // Player 1 (roșu)
        Serial.print("Player 1 (rosu) a apasat butonul ");
        Serial.print(button);
        Serial.print(" si a aprins LED-ul ");
        Serial.println(button);
      } else if (mutare % 2 == 0){
        lights[button] = 2;  // Player 2 (albastru)
        Serial.print("Player 2 (albastru) a apasat butonul ");
        Serial.print(button);
        Serial.print(" si a aprins LED-ul ");
        Serial.println(button);
      }
    }
      gameState = checkForWin(); // Verificăm dacă cineva a câștigat

      // Actualizăm scorul și nrjoc în funcție de câștigătorul jocului
      if (gameState == 11) { // Player 1 câștigă
        scor1++;
        nrjoc++;
        Serial.print("Player 1 (rosu) a castigat meciul! Scorul este acum ");
        Serial.print(scor1);
        Serial.print(" - ");
        Serial.println(scor2);
      } else if (gameState == 12) { // Player 2 câștigă
        scor2++;
        nrjoc++;
        Serial.print("Player 2 (albastru) a castigat meciul! Scorul este acum ");
        Serial.print(scor1);
        Serial.print(" - ");
        Serial.println(scor2);
      } else if (gameState == 13) { // Remiză
        nrjoc++;
        Serial.println("Jocul s-a terminat cu remiza!");
      }

      refreshDisplay(); // Actualizăm LED-urile după mutare
    
  }

  showWinner(gameState); // Afișăm câștigătorul
  clearBoard();          // Curățăm tabla pentru următorul joc
  refreshDisplay();      // Actualizăm LED-urile pentru a le stinge
  delay(1000);           // Pauză înainte de următorul joc
}

// Functie care afiseaza câștigătorul prin aprinderea și stroboscopia celulelor corespunzătoare pe tabelă
void showWinner(int gameState) {
  // Resetăm toate valorile din array-ul 'flash' la 0 (stins)
  flash[1]=0; flash[2]=0; flash[3]=0; flash[4]=0; flash[5]=0; flash[6]=0; flash[7]=0; flash[8]=0; flash[9]=0;

  // Verificăm dacă jucătorul 1 a câștigat pe fiecare linie, coloană sau diagonală
  if (lights[1] == player1 && lights[2] == player1 && lights[3] == player1) { flash[1] = 1; flash[2] = 1; flash[3] = 1; }
  if (lights[4] == player1 && lights[5] == player1 && lights[6] == player1) { flash[4] = 1; flash[5] = 1; flash[6] = 1; }
  if (lights[7] == player1 && lights[8] == player1 && lights[9] == player1) { flash[7] = 1; flash[8] = 1; flash[9] = 1; }
  if (lights[1] == player1 && lights[4] == player1 && lights[7] == player1) { flash[1] = 1; flash[4] = 1; flash[7] = 1; }
  if (lights[2] == player1 && lights[5] == player1 && lights[8] == player1) { flash[2] = 1; flash[5] = 1; flash[8] = 1; }
  if (lights[3] == player1 && lights[6] == player1 && lights[9] == player1) { flash[3] = 1; flash[6] = 1; flash[9] = 1; }
  if (lights[1] == player1 && lights[5] == player1 && lights[9] == player1) { flash[1] = 1; flash[5] = 1; flash[9] = 1; }
  if (lights[3] == player1 && lights[5] == player1 && lights[7] == player1) { flash[3] = 1; flash[5] = 1; flash[7] = 1; }

  // Verificăm dacă jucătorul 2 a câștigat pe fiecare linie, coloană sau diagonală
  if (lights[1] == player2 && lights[2] == player2 && lights[3] == player2) { flash[1] = 2; flash[2] = 2; flash[3] = 2; }
  if (lights[4] == player2 && lights[5] == player2 && lights[6] == player2) { flash[4] = 2; flash[5] = 2; flash[6] = 2; }
  if (lights[7] == player2 && lights[8] == player2 && lights[9] == player2) { flash[7] = 2; flash[8] = 2; flash[9] = 2; }
  if (lights[1] == player2 && lights[4] == player2 && lights[7] == player2) { flash[1] = 2; flash[4] = 2; flash[7] = 2; }
  if (lights[2] == player2 && lights[5] == player2 && lights[8] == player2) { flash[2] = 2; flash[5] = 2; flash[8] = 2; }
  if (lights[3] == player2 && lights[6] == player2 && lights[9] == player2) { flash[3] = 2; flash[6] = 2; flash[9] = 2; }
  if (lights[1] == player2 && lights[5] == player2 && lights[9] == player2) { flash[1] = 2; flash[5] = 2; flash[9] = 2; }
  if (lights[3] == player2 && lights[5] == player2 && lights[7] == player2) { flash[3] = 2; flash[5] = 2; flash[7] = 2; }

  // Afișăm câștigătorul printr-un efect de flash pe LED-uri
  for (int i = 1; i <= 9; i++) {
    delay(200); // Pauză de 200ms între fiecare actualizare
    swapLightsAndFlash(); // Apelăm funcția pentru a schimba valorile dintre 'lights' și 'flash'
    refreshDisplay(); // Actualizăm vizualizarea pe display
  }  
}

// Funcție pentru a schimba valorile din array-urile 'lights' și 'flash'
void swapLightsAndFlash() {
  // Copiem valorile din array-ul 'flash' în array-ul 'lights' pentru a actualiza starea tablei
  lights[1] = flash[1];
  lights[2] = flash[2];
  lights[3] = flash[3];
  lights[4] = flash[4];
  lights[5] = flash[5];
  lights[6] = flash[6];
  lights[7] = flash[7];
  lights[8] = flash[8];
  lights[9] = flash[9];
  delay(250); // Pauză de 250ms pentru a vizualiza schimbările
}

// Funcție care verifică dacă există un câștigător sau un egal pe tabla de joc
int checkForWin() {
  int winState = 1; // Inițial presupunem că nu există câștigător (valoare 1 înseamnă continuare joc)

  // Verificăm dacă jucătorul 1 a câștigat (toate cele 3 valori de pe linie, coloană sau diagonală sunt egale cu 'player1')
  if (lights[1] == player1 && lights[2] == player1 && lights[3] == player1) { winState = 11; }
  if (lights[4] == player1 && lights[5] == player1 && lights[6] == player1) { winState = 11; }
  if (lights[7] == player1 && lights[8] == player1 && lights[9] == player1) { winState = 11; }
  if (lights[1] == player1 && lights[4] == player1 && lights[7] == player1) { winState = 11; }
  if (lights[2] == player1 && lights[5] == player1 && lights[8] == player1) { winState = 11; }
  if (lights[3] == player1 && lights[6] == player1 && lights[9] == player1) { winState = 11; }
  if (lights[1] == player1 && lights[5] == player1 && lights[9] == player1) { winState = 11; }
  if (lights[3] == player1 && lights[5] == player1 && lights[7] == player1) { winState = 11; }

  // Verificăm dacă jucătorul 2 a câștigat (toate cele 3 valori de pe linie, coloană sau diagonală sunt egale cu 'player2')
  if (lights[1] == player2 && lights[2] == player2 && lights[3] == player2) { winState = 12; }
  if (lights[4] == player2 && lights[5] == player2 && lights[6] == player2) { winState = 12; }
  if (lights[7] == player2 && lights[8] == player2 && lights[9] == player2) { winState = 12; }
  if (lights[1] == player2 && lights[4] == player2 && lights[7] == player2) { winState = 12; }
  if (lights[2] == player2 && lights[5] == player2 && lights[8] == player2) { winState = 12; }
  if (lights[3] == player2 && lights[6] == player2 && lights[9] == player2) { winState = 12; }
  if (lights[1] == player2 && lights[5] == player2 && lights[9] == player2) { winState = 12; }
  if (lights[3] == player2 && lights[5] == player2 && lights[7] == player2) { winState = 12; }

  // Verificăm dacă jocul a ajuns la un egal (toate celulele sunt umplute)
  int draw = 1; // Inițial presupunem că este egalitate
  for (int i = 1; i < 10; i++) {
    if (lights[i] == 0) {
       draw = 0; // Dacă există o celulă necompletată, nu este egalitate
    }
  }
  if (draw == 1) {
    winState = 13; // Dacă toate celulele sunt umplute și nu există câștigător, declarăm egalitate
  }

  return winState; // Returnăm rezultatul: 11 - câștigă jucătorul 1, 12 - câștigă jucătorul 2, 13 - egalitate
}

// Funcție care curăță tabla de joc (resetează LED-urile la starea inițială)
void clearBoard() {
  for (int i = 1; i < 10; i++) {
    lights[i] = 0; // Resetăm fiecare LED la 0 (stins)
  }
}
