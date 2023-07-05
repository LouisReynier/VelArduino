// VELARDUINO
// Version du 07 02 2023 
// MAE avec menus
// Affichage de la vitesse et de la distance
// affichage toutes les 2 secondes
// Si pas d'impulsion sur les 2 secondes v = 0
// Flash de la led à chaque \_ de l'ILS
// debounce sur IT
// bouton gauche : reglage - bouton droit : menu suiavnt

#include <LiquidCrystal.h>
#include <EEPROM.h>

#define  BP1 8
#define  BP2 9
#define LED     13
#define d 65  //diamètre de la roue en cm
#define pi 3.141592653

unsigned long delta_t, compteur, compteur_old, dernierPassage = 0 ;
unsigned long temps_courant, temps_avant = 0 ;

float p, vitesse, vitesse_old, vitesse_max, distance = 0 ;
const int delaiAntiRebond = 10;
const int delaiAffich = 2000 ;
const int delaiAppuiLong = 1000 ; 
int etat, etat_old = 0 ;

LiquidCrystal lcd(12, 11, 5, 4, 3, 7); // broche de l'afficheur

void setup()                       
{
  // broche d'entrée pour les interruptions => ILS
  pinMode(2, INPUT);
  pinMode(LED, OUTPUT);
  pinMode(BP1, INPUT);
  pinMode(BP2, INPUT);
  lcd.begin(16, 2); // specifie les dimensions de l'afficheur : 16 car et 2 lignes
  p = EEPROM.read(0);     // lecture perimetre de la roue à l'@0
  if (p==0) {p = 200 ; } // 200 cm si p = 0 
  compteur_old = 0;
  compteur=0;
  // activer interruptions
  attachInterrupt(digitalPinToInterrupt(2), comptage, FALLING); // initialisation de l'interruption externe sur le port 2 (front descendant) 
  interrupts();
  // initialisation liaison série
  Serial.begin(115200);
  etat = 1 ;
}

void loop()  // Programme principal
{ int etat_bp1 ;
  int etat_bp2 ;
  switch (etat) 
  {
    case (1): 
    // ***********  affichage distance et vitesse **************
    // calcul et affichage périodique de vitesse et distance (2s)
    temps_courant = millis();
    if ((temps_courant - temps_avant) > delaiAffich )
    {
    // determination vitesse
      if (delta_t !=0) // delta_t = ecart de temps entre deux impulsions
      {  vitesse = 36* p / delta_t;   // vitesse en km/h
        vitesse_old = vitesse ; }    // on memorise la derniere valeur valable
      else
      { vitesse = vitesse_old ; } // si division par zero on garde l'ancienne valeur !
    
      if (compteur == compteur_old) // Si le compteur d'impulsion n'a pas incrémenté depuis la derniere loop , la vitesse = 0 !
      {   vitesse =0;  }
      // vitesse maxi
      
      if (vitesse > vitesse_max)
      { vitesse_max = vitesse;}
      
    // determination distance 
    distance = (compteur*p)/100; 
    
    // affichage lcd + serial
    affichValeurs(distance, vitesse);
    Serial.println("d =" + String(d, DEC) + "-- v =" + String(vitesse, DEC) + "-- delta_t =" + String(delta_t, DEC) + "-- compteur =" + String(compteur, DEC) + "-- compteur_old =" + String(compteur_old, DEC));
    compteur_old = compteur; // mémorisation de l'ancienne valeur de compteur
     temps_avant = temps_courant ;
    } // 
  // lecture des BPs
  etat_bp1=digitalRead(BP1); 
  etat_bp2 = digitalRead(BP2);
  
  if (etat_bp1==0)// si bouton relaché 
  {
    etat = 2 ;
    lcd.clear();
  }
  if (etat_bp2 ==0) 
  {
     lcd.clear();
    do { etat_bp2 = digitalRead(BP2);} while (etat_bp2==0);
    etat = 3 ;
   
  }
  break ; // fin etat1
  
   case (2):  
   // **************  RAZ d et v   ********************
    digitalWrite(LED,HIGH);
    lcd.setCursor(0, 0);
    lcd.print("***   RAZ   ***");
    lcd.setCursor(0, 1);
    lcd.print("compteur d et v  ");
    delta_t = 0 ;
    vitesse = 0 ;
    vitesse_old = 0 ;
    distance = 0 ;
    compteur = 0 ;
    delay(delaiAffich);
    etat = 1 ;
    lcd.clear();
    digitalWrite(LED,LOW);
    break ; // fin etat 2
    
    case(3): 
    // *****    affichage v max    *****
    lcd.setCursor(0, 0);
    lcd.print(" vitesse max = ");
    lcd.setCursor(2, 1);
    lcd.print(vitesse_max);
    lcd.print(" km/h");
    etat_bp1=digitalRead(BP1); 
    etat_bp2 = digitalRead(BP2);
    if (etat_bp1==0)// si bouton appuyé 
    { etat = 4 ; // RAZ v max
      lcd.clear();
    }
    if (etat_bp2==0)// si bouton appuyé
    {lcd.clear();
     do { etat_bp2 = digitalRead(BP2);} while (etat_bp2==0);
    etat = 5 ;
    }
    break ;

  case (4):
  //*****    RAZ vitesse max     *****
    digitalWrite(LED,HIGH);
    lcd.setCursor(0, 0);
    lcd.print("***   RAZ   ***");
    lcd.setCursor(0, 1);
    lcd.print("   vitesse max   ");
    vitesse_max = 0 ;
    delay(delaiAffich);
    etat = 3 ;
    lcd.clear();
    digitalWrite(LED,LOW);
    break ; // fin etat 4

    case(5): 
    //*****     affichage perimetre     *****
    lcd.setCursor(0, 0);
    lcd.print("1 tour de roue =");
    lcd.setCursor(2, 1);
    lcd.print(p/100);
    lcd.print(" m");
    etat_bp1=digitalRead(BP1); 
    etat_bp2 = digitalRead(BP2);
    if (etat_bp1==0)// si bouton appuyé 
    {lcd.clear();
    do { etat_bp1 = digitalRead(BP1);} while (etat_bp1==0);
    etat = 6 ; // RAZ v max
     }
    if (etat_bp2==0)// si bouton appuyé
    { lcd.clear();
     do { etat_bp2 = digitalRead(BP2);} while (etat_bp2==0);
    etat = 1 ;
    }
    break ;

     case(6): // 
     //*****     Reglage perimetre     *****
    digitalWrite(LED,HIGH);
    lcd.setCursor(0, 0);
    lcd.print("1 tour de roue =");
    lcd.setCursor(0, 1);
    lcd.print("* ");
    lcd.print(p/100);
    lcd.print(" m");
    etat_bp1=digitalRead(BP1); 
 
    int press_bp2 = detectPress(BP2, delaiAntiRebond, delaiAppuiLong);
    if (press_bp2 == 1 ){ p++ ;} // appui court
    if (press_bp2 == 2 ) { p = p+10 ;} // appui long
    if (p>230){p = 90 ; }
    
    if (etat_bp1==0)// si bouton appuyé 
    {lcd.clear();
    do { etat_bp1 = digitalRead(BP1);} while (etat_bp1==0);
    etat = 5 ; // RAZ v max
    digitalWrite(LED,LOW);
    //EEPROM.write(0,p);
    EEPROM.update(0, p); // update n'écrit que si la data est differente de celle deja présente !
     }
    break ;
}// fin switch case
if (etat != etat_old )
  { Serial.print("Etat = "); Serial.println(etat);
  etat_old = etat ;
  }
delay(10); // stabilité/inertie boucle
} // fin loop 
// *****  Comptage - routine d'interruptions *****

void comptage () // routine IT compte les \_ et mesure l'ecart entre deux \_ et 
{  digitalWrite(LED, HIGH) ;
   unsigned long temps = millis() ;
   delta_t = temps - dernierPassage ;  // intervalle de temps entre 2 passages du 
  if (delta_t > delaiAntiRebond) // on ne valide que les impulsions assez grande (on vire les rebonds)
  {
    compteur++;  // Incrémente le compteur seulement si le temps depuis le dernier appui est supérieur au délai anti-rebond
    // debug
    Serial.println("t : " + String(temps) + "  ms : dernierPassage : " + String(dernierPassage) + " ms delta_t : " + String(delta_t) + " ms : cpmtr " + String(compteur, DEC)) ;
    dernierPassage = millis();
  }
   digitalWrite(LED, LOW) ;
} // fin void comptage

// **** affichValeurs (sur LCD)  ****
void affichValeurs(float dist, float vit) // affichage v et d sur LCD
{
  lcd.setCursor(0, 0);
  lcd.print("d : ");
  lcd.print(dist,1);
  lcd.print(" m   ");
  lcd.setCursor(1, 1);
  lcd.print("v : ");
  lcd.print(vit,1);
  lcd.print(" km/h   ");
} // fin void affichVal

// ****  detectPress (appui long/court sur BP)  ****
int detectPress(int buttonPin, int debounceDelay, int longPressDuration) {
  // cette fonction retourne 0 si pas d'appui sur BP
  // 1 si appui court et 2 si appui long
  
  int buttonState = digitalRead(buttonPin);
  int lastButtonState = buttonState;
  unsigned long lastDebounceTime = 0;

  if (buttonState != lastButtonState) 
  { lastDebounceTime = millis(); }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (buttonState == LOW) {
      unsigned long currentTime = millis();
      while (digitalRead(buttonPin) == LOW) {
       if ((millis() - currentTime) > longPressDuration) {
          return 2;
        }
      }
      return 1;
    }
  }
  return 0;
}
