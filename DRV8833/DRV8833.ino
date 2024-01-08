
int E1 = 5;  //PWMA
int M1 = 4;  //DIRA
int E2 = 6;  //PWMB             
int M2 = 7;  //DIRB
int Val = 100; //0-255

void setup(){
      pinMode(M1, OUTPUT); 
      pinMode(M2, OUTPUT);
      pinMode(E1, OUTPUT); 
      pinMode(E2, OUTPUT);

}
void forward(){
      digitalWrite(M1,LOW);        
      analogWrite(E1, Val);   
      digitalWrite(M2,LOW);
      analogWrite(E2, Val);   
}

void back(){
     digitalWrite(E1,LOW);
     analogWrite(M1,Val);   //PWM调速
     digitalWrite(E2,LOW);
     analogWrite(M2,Val);   //PWM调速
}

void stop(){
     digitalWrite(E1,LOW);
     digitalWrite(M1,LOW);   //PWM调速
     digitalWrite(E2,LOW);
     digitalWrite(M2,LOW);   //PWM调速
}

void loop(){
     forward();
     delay(1000);
     stop();
     delay(100);
     back();
     delay(1000);
     stop();
     delay(100);
}
