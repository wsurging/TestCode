// ---------------------------------------------------------------------------
// Example NewPing library sketch that does a ping about 20 times per second.
// ---------------------------------------------------------------------------
#include <NewPing.h>
#define TRIG 3
#define ECHO 2
#define MAXDISTANCE 400
NewPing sonar(TRIG,ECHO,MAXDISTANCE);

int middleDistanceTable[5];
void readSr04()
{
   for(int i = 0;i<5;i++)
   {
     middleDistanceTable[i]=sonar.ping()/US_ROUNDTRIP_CM;
      delay(10);
   }
}
int bubbleSort(int a[],int n)
{
   int i,j,temp;
   for(j=0;j<n-1;j++)
   {
      for(i=0;i<n-1;i++)
      {
         if(a[i]>a[i+1])
         {
            temp = a[i];
            a[i] = a[i+1];
            a[i+1] = temp;
         }
      }
   }
}
int countDistance()
{
   int i;
   int dataTemp = 0;
   readSr04();
   bubbleSort(middleDistanceTable,5);
   for(i=1;i<5;i++)
   {
      dataTemp+=middleDistanceTable[i];
 //     Serial.print(middleDistanceTable[i]);
 //     Serial.print(';');
   } 
 //  Serial.println("");
   return dataTemp/=4;
}
/*void judgeDistance()
{
   int leftDistance,rightDistance;
   int motorSpeed;
   if(countDistance()<150)
     motorSpeed = countDistance();
   else
     motorSpeed = 150;
   Serial.println(countDistance());

   if(countDistance()<40)
   {
     setMotor(0,0);
     distanceLR(leftDistance,rightDistance);
     turnServo('M');
     Serial.println(countDistance());
     if(((leftDistance+rightDistance)<80)&&(leftDistance<30)&&(rightDistance<30))
     {
        setMotor(-60,-70);
        delay(300);
        if(leftDistance<rightDistance)
        {
           setMotor(45,-52);
           delay(300);
        }
        else
        {
           setMotor(-45,52);
           delay(300);
        }

     }
     else
     {
        if((leftDistance>rightDistance)&&(leftDistance>20))
        {
           setMotor(-45,50);
           delay(300);
        }
        if((leftDistance<rightDistance)&&(rightDistance>20))
        {
           setMotor(45,-50);
           delay(300);
        }
     }
   }
   else 
      setMotor(motorSpeed,motorSpeed+3);
}*/

