#include <Servo.h>
Servo S;

//define inputs
#define LS A2
#define MS A1
#define RS A0
#define R A3    //extreme right sensor
#define L A4    //extreme left sensor
#define echoPin 11

//define outputs
#define enL 3
#define LMA 7
#define LMB 8
#define enR 6
#define RMA 2
#define RMB 4
#define trigPin 12

void setup()
{
  pinMode(LS,INPUT);
  pinMode(MS,INPUT);
  pinMode(RS,INPUT);


  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
 
  S.attach(13);
  S.write(0);
 
  pinMode(enL,OUTPUT);
  pinMode(LMA,OUTPUT);
  pinMode(LMB,OUTPUT);
  pinMode(enR,OUTPUT);
  pinMode(RMA,OUTPUT);
  pinMode(RMB,OUTPUT);

  /*analogWrite(enL, 200);
  analogWrite(enR, 200);*/
 
  Serial.begin(9600);
}

//possible improvements: 1. combine all movement functions, 2. use PWM even w/o enable pins
void forward()
{
   digitalWrite(LMA, HIGH);
   digitalWrite(LMB, LOW);
   digitalWrite(RMA, HIGH);
   digitalWrite(RMB, LOW);
}

void turnLeft()
{
 
  digitalWrite(LMA, LOW);
  digitalWrite(LMB, LOW);
  digitalWrite(RMA, HIGH);
  digitalWrite(RMB, LOW);
}

void turnRight()
{
   digitalWrite(LMA, HIGH);
   digitalWrite(LMB, LOW);
   digitalWrite(RMA, LOW);
   digitalWrite(RMB, LOW);
}

void reverse()
{
   digitalWrite(LMA, LOW);
   digitalWrite(LMB, HIGH);
   digitalWrite(RMA, LOW);
   digitalWrite(RMB, HIGH);
}

void sharpRight()
{
   digitalWrite(LMA, HIGH);
   digitalWrite(LMB, LOW);
   digitalWrite(RMA, LOW);
   digitalWrite(RMB, HIGH);
}

void sharpLeft()
{
   digitalWrite(LMA, LOW);
   digitalWrite(LMB, HIGH);
   digitalWrite(RMA, HIGH);
   digitalWrite(RMB, LOW);
}

void halt()
{
  digitalWrite(LMA, LOW);
  digitalWrite(LMB, LOW);
  digitalWrite(RMA, LOW);
  digitalWrite(RMB, LOW);
}

long duration;
int distance;
int angle=0;
bool box=false;
int dMin=4;
int dMax=7;
int turned=0;
char prev;

int findDistance()
{ 
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  distance = duration*0.034/2;
  return distance;  
}

void drop()
{
  for (; angle>=0; )
  {
    angle=angle-15;
    S.write(angle);
    delay(50);
  }
}

void grab()
{
  while(angle<=180)
  {
     angle=angle+15;
     S.write(angle);
     delay(50);
  } 
}

int allWhite()
{return digitalRead(L) && digitalRead(LS) && digitalRead(MS) && digitalRead(RS) && digitalRead(R); }

void reacquire()
{
  switch(prev)
  {
    case 'R': do
              {
                sharpLeft(); delay(50);
                halt(); delay(100);
              } while(allWhite());
              break;
              
    case 'L': do
              {
                sharpRight(); delay(50);
                halt(); delay(100);
              } while(allWhite());
              break;
  }
}

//digitalRead() returns true when IR sensor detects white line

void loop() {

  //grab box
  if((findDistance()>=dMin && findDistance()<=dMax) && box==false)
  {
      halt(); delay(200);
      grab();
      box=true; 
  }
  
  //navigation
  else if (digitalRead(LS) && !digitalRead(MS) && digitalRead(RS))
    {forward();}
   else if (digitalRead(LS) && (!digitalRead(RS) || !(digitalRead(R))))
    {sharpRight(); prev='R';}
  else if ((!digitalRead(LS) || !digitalRead(L))&& digitalRead(RS))
    {sharpLeft(); prev='L';}
  
  //base reached
  else if (!digitalRead(LS) && digitalRead(MS) && !digitalRead(RS)) 
    {forward(); delay(25);}
    
  //correction
  else if(allWhite())
    {reacquire();}

   else if (!digitalRead(LS) && !digitalRead(MS) && !digitalRead(RS))
   {
      forward(); delay(25);
      
      if(box==true && turned==0)
        {
          sharpLeft(); delay(800);
          turned=1;      
        }
        
       
       //for crooked tracks: direction depends on track
       else if (box==false && turned==0)
       {
          reverse(); delay(450);
          
         do
          { 
            sharpLeft(); delay(50);
            halt(); delay(200);
           } while(findDistance()>20);
          
          sharpLeft(); delay(25);
          
           do
            {
               forward(); delay(25);
               halt(); delay(50);
            } while(findDistance()>6);

            halt(); delay(200);
            grab();
            box=true;

            sharpLeft(); delay(550);
            turned=1;       
        }

        else if (box==true && turned==1 && (!digitalRead(LS) && digitalRead(MS) && !digitalRead(RS)))
        {
          reverse(); delay(500);
          halt(); delay(100);
          drop();
          while(1){;}
        }              
   }
 
  delay(100);
  halt();
  delay(100);
}
