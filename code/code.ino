
/* ------------- PARTIE MODIFIABLE DU CODE ---------------*/

// vitesse de clignotement
#define tempsAvantClignotement 1000
#define nombreClignotementAvantAction 2
#define vitesseClignotementPortionLumineuse 2
#define vitesseClignotementPortionSombre 3

// pour supprimer les rebonds du scapteurs
#define tempsMinimumEntreDeuxTrain 1000
#define tempsQuePrendLeTrainPourArriverAuPassageaNivau 3000

/* --------------------------------------------------------*/

#include "PinChangeInterrupt.h"
#define boutonEntreeGauche 11
#define boutonSortieGauche 10
#define boutonEntreeDroite 9
#define boutonSortieDroite 8

#define relais_Open 4
#define relais_Close 5
#define timeDelta 500
#define actionBarriereDelta 3000

bool voieOuverte = true;
bool unTrainArriveaGauche = false;
long int derniereArriveaGauche = -1;
bool unTrainArriveaDroite = false;
long int derniereArriveaDroite = -1;

void trainGaucheArrive()
{
  if (derniereArriveaGauche == -1)
  {
    derniereArriveaGauche = millis();
    unTrainArriveaGauche = true;
  }
  else
  {
    long int maintenant = millis();
    if (maintenant - derniereArriveaGauche > tempsMinimumEntreDeuxTrain)
    {
      derniereArriveaGauche = maintenant;
      unTrainArriveaGauche = true;
    }
  }
}
void trainGaucheSort()
{
  long int maintenant = millis();
  if (derniereArriveaGauche != -1 && (maintenant - derniereArriveaGauche > tempsQuePrendLeTrainPourArriverAuPassageaNivau))
  {
    unTrainArriveaGauche = false;
  }
}
void trainDroiteArrive()
{
  if (derniereArriveaDroite == -1)
  {
    derniereArriveaDroite = millis();
    unTrainArriveaDroite = true;
  }
  else
  {
    long int maintenant = millis();
    if (maintenant - derniereArriveaDroite > tempsMinimumEntreDeuxTrain)
    {
      derniereArriveaDroite = maintenant;
      unTrainArriveaDroite = true;
    }
  }
}
void trainDroiteSort()
{
  long int maintenant = millis();
  if (derniereArriveaDroite != -1 && (maintenant - derniereArriveaDroite > tempsQuePrendLeTrainPourArriverAuPassageaNivau))
  {
    unTrainArriveaDroite = false;
  }
}

#define led1_Pin 3
#define led2_Pin 2
int timer = 0;
void clignoterAccendant()
{
  for (int i = 0; i <= 255; i += 1)
  {
    if (i < 155)
    {
      timer = vitesseClignotementPortionSombre;
    }
    else
    {
      timer = vitesseClignotementPortionLumineuse;
    }
    analogWrite(led1_Pin, i);
    analogWrite(led2_Pin, i);
    delay(timer);
  }
}
void clignoterDescendant()
{
  for (int i = 254; i > 0; i -= 1)
  {
    if (i < 155)
    {
      timer = vitesseClignotementPortionSombre;
    }
    else
    {
      timer = vitesseClignotementPortionLumineuse;
    }
    analogWrite(led1_Pin, i);
    analogWrite(led2_Pin, i);
    delay(timer);
  }
}

void ouvrirBarriere()
{
  // eteindre les leds
  analogWrite(led1_Pin, 0);
  analogWrite(led2_Pin, 0);

  // ouvrir les barrieres
  digitalWrite(relais_Open, HIGH);
  delay(actionBarriereDelta);
  digitalWrite(relais_Open, LOW);
}
void fermerBarriere()
{
  // attendre un peu
  delay(tempsAvantClignotement);

  // clignoter
  for (int x = 0; x < nombreClignotementAvantAction; x++)
  {
    clignoterAccendant();
    delay(100);
    clignoterDescendant();
  }

  // clignoter + fermer les barrieres
  digitalWrite(relais_Close, HIGH);
  int start = millis();
  while (millis() - start < actionBarriereDelta)
  {
    clignoterAccendant();
    delay(100);
    clignoterDescendant();
  }
  digitalWrite(relais_Close, LOW);
}

// configuration
void setup()
{
  /*------- define the inputs -------*/
  pinMode(boutonEntreeGauche, INPUT);
  attachPCINT(digitalPinToPCINT(boutonEntreeGauche), trainGaucheArrive, RISING);

  pinMode(boutonSortieGauche, INPUT);
  attachPCINT(digitalPinToPCINT(boutonSortieGauche), trainGaucheSort, RISING);

  pinMode(boutonEntreeDroite, INPUT);
  attachPCINT(digitalPinToPCINT(boutonEntreeDroite), trainDroiteArrive, RISING);

  pinMode(boutonSortieDroite, INPUT);
  attachPCINT(digitalPinToPCINT(boutonSortieDroite), trainDroiteSort, RISING);

  /*------- define the outputs -------*/
  pinMode(led2_Pin, OUTPUT);
  pinMode(led1_Pin, OUTPUT);

  pinMode(relais_Close, OUTPUT);
  digitalWrite(relais_Close, LOW);

  pinMode(relais_Open, OUTPUT);
  digitalWrite(relais_Open, LOW);

  ouvrirBarriere();
}

// lecture des états
void loop()
{
  /* Finite State Machine */
  if (voieOuverte && (unTrainArriveaGauche || unTrainArriveaDroite))
  {
    /* FERMER LES VOIES */
    fermerBarriere();
    voieOuverte = false;
  }
  else
  {
    if (!voieOuverte && !unTrainArriveaGauche && !unTrainArriveaDroite)
    {
      /* OUVRIR LES VOIES */
      ouvrirBarriere();
      voieOuverte = true;
    }
    else
    {
      /* CLIGNOTER */
      clignoterAccendant();
      delay(100);
      clignoterDescendant();
    }
  }
}
