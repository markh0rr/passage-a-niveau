
/* ------------- PARTIE MODIFIABLE DU CODE ---------------*/

// vitesse de clignotement 
#define tempsAvantClignotement 1000
#define nombreClignotement 3
#define vitesseClignotementPortionLumineuse 2
#define vitesseClignotementPortionSombre 3

// pour supprimer les rebonds du scapteurs 
#define tempsMinimumEntreDeuxTrain 1000
#define tempsQuePrendLeTrainPourArriverAuPassageaNivau 3000

// angles des servomoteurs ouverts et fermés
#define angleOuvert 0
#define angleFerme 90

/* --------------------------------------------------------*/


#include "PinChangeInterrupt.h"
#define boutonEntreeGauche 8
#define boutonEntreeDroite 9
#define boutonSortieGauche 10
#define boutonSortieDroite 11

#include <Servo.h>
Servo servos_barrieres;
#define portArduino_Barrieres 3

bool voieOuverte = true;
bool unTrainArriveaGauche = false;
long int derniereArriveaGauche = -1;
bool unTrainArriveaDroite = false;
long int derniereArriveaDroite = -1;

void trainGaucheArrive(){
  if(derniereArriveaGauche == -1){
    derniereArriveaGauche = millis();
    unTrainArriveaGauche = true;
  }else{
    long int maintenant = millis();
    if(maintenant - derniereArriveaGauche > tempsMinimumEntreDeuxTrain){
       derniereArriveaGauche = maintenant;
       unTrainArriveaGauche = true;
    }
  }
}
void trainGaucheSort(){
  long int maintenant = millis();
  if(derniereArriveaGauche != -1 && (maintenant - derniereArriveaGauche > tempsQuePrendLeTrainPourArriverAuPassageaNivau)){
    unTrainArriveaGauche = false;
  }
}
void trainDroiteArrive(){
if(derniereArriveaDroite == -1){
    derniereArriveaDroite = millis();
    unTrainArriveaDroite = true;
  }else{
    long int maintenant = millis();
    if(maintenant - derniereArriveaDroite > tempsMinimumEntreDeuxTrain){
       derniereArriveaDroite = maintenant;
       unTrainArriveaDroite = true;
    }
  }
}
void trainDroiteSort(){
  long int maintenant = millis();
  if(derniereArriveaDroite != -1 && (maintenant - derniereArriveaDroite > tempsQuePrendLeTrainPourArriverAuPassageaNivau)){
    unTrainArriveaDroite = false;
  }
}  

#define led1_Pin 5
#define led2_Pin 6
int timer = 0;
void clignoterAccendant()
{
      for(int i = 0; i<=255;i+=1){
      if(i<155){
        timer = vitesseClignotementPortionSombre;
      }else{
        timer = vitesseClignotementPortionLumineuse;
      }
       analogWrite(led1_Pin, i);
       analogWrite(led2_Pin, i);
       delay(timer);
    }
}
void clignoterDescendant(){
  for(int i = 254; i>0;i-=1){
      if(i<155){
         timer = vitesseClignotementPortionSombre;
      }else{
         timer = vitesseClignotementPortionLumineuse;
      }
      analogWrite(led1_Pin, i);
      analogWrite(led2_Pin, i);
      delay(timer);
   }
}

void ouvrirBarriere(){
    // eteindre les leds 
    analogWrite(led1_Pin, 0);
    analogWrite(led2_Pin, 0);
  
   // ouvrir les servomoteurs 
   servos_barrieres.write(angleOuvert);
}
void fermerBarriere(){
    // attendre un peu
    delay(tempsAvantClignotement);
    
    // clignoter
    for(int x=1; x<=nombreClignotement; x++){
      clignoterAccendant();
      delay(100);
      if(x < nombreClignotement){
        clignoterDescendant();
      }
    }
    // fermer les servomoteurs
    servos_barrieres.write(angleFerme);
}

// configuration 
void setup(){
    pinMode(led2_Pin, OUTPUT);
    pinMode(led1_Pin, OUTPUT);
  
    pinMode(boutonEntreeGauche, INPUT);
    attachPCINT(digitalPinToPCINT(boutonEntreeGauche), trainGaucheArrive, RISING);
        
    pinMode(boutonSortieGauche, INPUT);
    attachPCINT(digitalPinToPCINT(boutonSortieGauche), trainGaucheSort, RISING);
        
    pinMode(boutonEntreeDroite, INPUT);
    attachPCINT(digitalPinToPCINT(boutonEntreeDroite), trainDroiteArrive, RISING);
    
    pinMode(boutonSortieDroite, INPUT);
    attachPCINT(digitalPinToPCINT(boutonSortieDroite), trainDroiteSort, RISING);

    servos_barrieres.attach(portArduino_Barrieres);
    ouvrirBarriere();
}


// lecture des états 
void loop(){

  /* Finite State Machine */
  if(voieOuverte && (unTrainArriveaGauche || unTrainArriveaDroite)){
    /* FERMER LES VOIES */
    fermerBarriere();
    voieOuverte = false;
  }else{
    if (!voieOuverte && !unTrainArriveaGauche && !unTrainArriveaDroite){
      /* OUVRIR LES VOIES */
      ouvrirBarriere();
      voieOuverte = true;
    }
  }
}
