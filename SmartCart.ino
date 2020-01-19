#include<Wire.h>

const int MPU_addr = 0x68;  // I2C address of the MPU-6050
int16_t X[15], Y[15], Z[15];

int sum_x, sum_y, sum_z, avg_x, avg_y, avg_z, ang_x, ang_y;
int in1 = 4;
int in2 = 5;
int in3 = 6;
int in4 = 7;
int enA = 9;
int enB = 10;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B); 
  Wire.write(0);
  Wire.endTransmission(true);
  
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(enA, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  pinMode(enB, OUTPUT);
}

void loop(){
  readGyro();
  checkGo();
}

void readGyro() {
  sum_x = 0, sum_y = 0, sum_z = 0, avg_x = 0, avg_y = 0, avg_z = 0, ang_x= 0, ang_y=0;
  
  for (int i = 0; i < 15; i++) {
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x43  (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
    
    X[i]=((uint16_t)Wire.read())<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
    Y[i]=((uint16_t)Wire.read())<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
    Z[i]=((uint16_t)Wire.read())<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
    
    X[i] = int(X[i]/131);
    Y[i] = int(Y[i]/131);
    Z[i] = int(Z[i]/131);

    sum_x += X[i];
    sum_y += Y[i];
    sum_z += Z[i];
  }

  avg_x = int(sum_x/15)+3;
  avg_y = int(sum_y/15)-2;
  avg_z = int(sum_z/15);

  //y = -125 - +125, x= -114 - +135
  
  if(avg_x >=0){
    if(avg_x > 135){avg_x=135;}
    ang_x = map(avg_x,0,135,0,90);
    }
  else if(avg_x < 0){
    if(avg_x < -114){avg_x=-114;}
    ang_x = map(avg_x,-114,-1,-90,-1);
    }
  
  if(avg_y > 125){avg_x=125;}
  else if(avg_y < -125){avg_x=-125;}
  ang_y = map(avg_y,-125,125,90,-90);
  
  Serial.print(" | Avg_X = "); Serial.print(avg_x);
  Serial.print(" | AVG_Y = "); Serial.print(avg_y);
  Serial.print(" | AVG_Z = "); Serial.println(avg_z);
  Serial.print(" | Ang_X = "); Serial.print(ang_x);
  Serial.print(" | AnG_Y = "); Serial.println(ang_y);
  Serial.println();
  Serial.println();
}

void checkGo(){
  if(ang_y>=12 && ang_y <=55){
    Serial.println("going up");
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    int power = map(ang_y,12,55,70,100);
    analogWrite(enA,power);
    analogWrite(enB,power);
    }
  else if(ang_y<=-12 && ang_y >=-55){
    Serial.println("going down");
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
    digitalWrite(in3,LOW);
    digitalWrite(in4,HIGH);
    int power = map(ang_y,-55,-12, 40, 50);
    analogWrite(enA,power);
    analogWrite(enB,power+3);
    }
  else{
    Serial.println("stopping");
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
    digitalWrite(in3,LOW);
    digitalWrite(in4,LOW);
    int power = 0;
    analogWrite(enA,power);
    analogWrite(enB,power);
    }
  }
