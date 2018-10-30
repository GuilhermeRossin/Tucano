#include<Wire.h>

//Endereco I2C do MPU6050
const int MPU=0x68;  
//Variaveis para armazenar valores dos sensores
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 double AX;
 double AY;
 double AZ;

 double GX;
 double GY;
 double GZ;
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  
  //Inicializa o MPU-6050
  Wire.write(0); 
  Wire.endTransmission(true);

}
double ConverteScale(double xmax, double xmin,double x,double ymax, double ymin){
  double y;
  y = ymax - (xmax - x)*(ymax-ymin)/(xmax-xmin);
  return y;
} 

  void loop()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  //Solicita os dados do sensor
  Wire.requestFrom(MPU,14,true);  
  //Armazena o valor dos sensores nas variaveis correspondentes
  AcX=Wire.read()<<8|Wire.read();  //0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  //0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  //0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  //0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  //0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  //0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  //0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  //Envia valor X do acelerometro para a serial e o LCD
  
    AX = ConverteScale(32768.00, -32768.00, (double) AcX, 2.0,-2.0);
    AY = ConverteScale(32768.00, -32768.00, (double) AcY, 2.0,-2.0);
    AZ = ConverteScale(32768.00, -32768.00, (double) AcZ, 2.0,-2.0);

    GX = ConverteScale(32768.00, -32768.00, (double) GyX, 2000,-2000);
    GY = ConverteScale(32768.00, -32768.00, (double) GyY, 2000,-2000);
    GZ = ConverteScale(32768.00, -32768.00, (double) GyZ, 2000,-2000);

  Serial.print("2\t-2\t");Serial.print("\t");
  Serial.print(AX);Serial.print("\t");
  Serial.print(AY);Serial.print("\t");
  Serial.println(AZ);
/*
  Serial.print("2000\t-2000\t");Serial.print("\t");
  Serial.print(GX);Serial.print("\t");
  Serial.print(GY);Serial.print("\t");
  Serial.println(GZ);
*/
 //Serial.print(" | Tmp = "); Serial.println(Tmp/340.00+36.53);

  delay(50);
}
