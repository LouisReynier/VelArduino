/*
 Pragramme TEST du compteur velo Arduino
 Test de l'ecran LCD, de la led
 Lecture des 3 entrées BP1 BP2 Capteur
*/

// include the library code:
#include <LiquidCrystal.h>

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 7, ledR = 13, bp1 = 8, bp2 = 9 , capt = 2 ;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup() {
  // set up the LCD's number of columns and rows:
  pinMode(ledR, OUTPUT);
  pinMode(capt, INPUT_PULLUP);
  // ecran d'acceuil 3 secondes
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("hello, world!");
  lcd.setCursor(0, 1);
  lcd.print("Bonjour, monde!");
  lcd.blink();
  digitalWrite(ledR, HIGH);
  delay(3000);
  lcd.clear();
}

void loop() {
  // lecture des entrées
   int etatBP1 =  digitalRead(bp1);
   int etatBP2 =  digitalRead(bp2);
   int etatCapteur = digitalRead(capt);
   // affichage etat des entrées
   lcd.setCursor(0, 0);
   lcd.print("BP1 :");
   lcd.print(etatBP1);
   lcd.print("    BP2 :");
   lcd.print(etatBP2);
   lcd.setCursor(3, 1);
   lcd.print("Capteur :");
   lcd.print(etatCapteur);
   // la led n'est allumé que si toutes les entrées sont à 1
   bool etatLED = (etatBP1 &&  etatBP2 && etatCapteur) ;
   digitalWrite(ledR,etatLED);
}
