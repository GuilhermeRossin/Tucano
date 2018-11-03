//------- CÓDIGO LEITURA MPU-6052, BMP-180 E SALVAMENTO EM CARTÃO SD -------------
//Autores: Guilherme Rossin e Paulo Ricardo Bueno

//----------------------------------------------------------------------------------

//Carrega Bibliotecas
#include <Wire.h>
#include <SD.h>

//Endereco I2C do MPU6050
const int MPU=0x68; 
// Endereço I2C do BMP180
#define Addr 0x77 

//Variaveis para armazenar valores para o MPU-6050
int AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
 double AX;
 double AY;
 double AZ;

 double GX;
 double GY;
 double GZ;

//Variaveis utilizadas para o BMP180
int coff[11] = {0};
unsigned int AC4 = 0, AC5 = 0, AC6 = 0;
unsigned int data[3] = {0};

//Variaveis utlizadas no Cartão SD
Sd2Card SDcard;
SdVolume volume;

//Pino do arduino conectado ao pino CS do módulo
const int chipSelect = 4;

//----------------------------------------------------------------------------------

void setup()
{
  //Inicializa a comunicação I2C
  Serial.begin(9600);
  Wire.begin();
  
  //Inicializa o MPU-6050
  Wire.beginTransmission(MPU);
  Wire.write(0x6B); 
  Wire.write(0); 
  Wire.endTransmission(true);

  //Inicializa o BMP180
   for (int i = 0; i < 11; i++)
  {
    // Inicializa a  I2C Transmissão
    Wire.beginTransmission(Addr);
    // Seleciona registro de dados
    Wire.write((170 + (2 * i)));
    // Para I2C Transmissão
    Wire.endTransmission(true);

    // Requisita 2 bytes de dados
    Wire.requestFrom(Addr, 2);

    // Leitura 2 bytes de dados
    if (Wire.available() == 2)
    {
      data[0] = Wire.read();
      data[1] = Wire.read();
    }

    delay(50);
    coff[i] = data[0] * 256 + data[1];
    if ((coff[i] > 32767) && (i != 3) && (i != 4) && (i != 5))
    {
      coff[i] -= 65536;
    }
  }
  AC4 = coff[3];
  AC5 = coff[4];
  AC6 = coff[5];
  delay(300);

  //Inicia a comunicação com o módulo SD
  if (!SD.begin(chipSelect)) 
  {
    Serial.println("Falha ao acessar o cartao !");
    Serial.println("Verifique o cartao/conexoes e reinicie o Arduino...");
    return;
  }
  Serial.println("Cartao iniciado corretamente !");
  Serial.println("Aguardando o inicio das medicoes...");
  Serial.println();
}

//----------------------------------------------------------------------------------

double ConverteScale(double xmax, double xmin,double x,double ymax, double ymin){
  double y;
  y = ymax - (xmax - x)*(ymax-ymin)/(xmax-xmin);
  return y;
  }
  
//----------------------------------------------------------------------------------
  void loop()
{
 
  //Aquisição dados do MPU-6050
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

  Serial.print("2000\t-2000\t");Serial.print("\t");
  Serial.print(GX);Serial.print("\t");
  Serial.print(GY);Serial.print("\t");
  Serial.println(GZ);


  //Aquisição dados BMP180
  //Inicio da transmissão I2C 
  Wire.beginTransmission(Addr);
  // Seleção do registro de controle de medição
  Wire.write(0xF4);
  // Ativar medição de temperatura
  Wire.write(0x2E);
  // Para transmissão I2C 
  Wire.endTransmission();
  delay(100);

  // Inicio da transmissão I2C 
  Wire.beginTransmission(Addr);
  // Seleão do registro de dados
  Wire.write(0xF6);
  // Para transmissão I2C 
  Wire.endTransmission();

  // Requisita 2 bytes de dados
  Wire.requestFrom(Addr, 2);

  // Leitura dos 2 bytes de dados
  // temp msb, temp lsb
  if (Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

  // Converção de dados
  float temp = ((data[0] * 256.0) + data[1]);

  // Inicia I2C Transmissão
  Wire.beginTransmission(Addr);
  // Seleção do registro de controle de medição
  Wire.write(0xF4);
  // Ativar medição de pressão, OSS = 1
  Wire.write(0x74);
  // Para I2C Transmissão
  Wire.endTransmission();
  delay(100);

  // Inicia I2C Transmissão
  Wire.beginTransmission(Addr);
  // Seleção do registro de controle de medição
  Wire.write(0xF6);
  // Para I2C Transmissão
  Wire.endTransmission();
  // Requisita 3 bytes de dados
  Wire.requestFrom(Addr, 3);

  // Leitura dos 3 bytes de dados
  // pres msb1, pres msb, pres lsb
  if (Wire.available() == 3)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
    data[2] = Wire.read();
  }

  // Converção de dados
  long long pres = (((long long)data[0] * (long long)65536) + ((long)data[1] * 256) + data[2]) / 128;

  // Calibração de temperatura
  double X1 = (temp - AC6) * AC5 / 32768.0;
  double X2 = ((double)coff[9] * 2048.0) / (X1 + coff[10]);
  double B5 = X1 + X2;
  float cTemp = ((B5 + 8.0) / 16.0) / 10.0;
  float fTemp = cTemp * 1.8 + 32;

  // Calibração da pressão
  double B6 = B5 - 4000;
  X1 = (coff[7] * ((long)B6 * (long)B6 / 4096.0)) / 2048.0;
  X2 = coff[1] * B6 / 2048.0;
  double X3 = X1 + X2;
  double B3 = (((coff[0] * 4 + X3) * 2) + 2) / 4.0;
  X1 = coff[2] * B6 / 8192.0;
  X2 = (coff[6] * ((long)B6 * (long)B6 / 2048.0)) / 65536.0;
  X3 = ((X1 + X2) + 2) / 4.0;
  double B4 = AC4 * (X3 + 32768) / 32768.0;
  long long B7 = (((unsigned long long )pres - (unsigned long long )B3) * ((unsigned long long )25000.0));
  float pressure = 0.0;
  if (B7 < 2147483648)
  {
    pressure = (B7 * 2) / B4;
  }
  else
  {
    pressure = (B7 / B4) * 2;
  }
  X1 = (pressure / 256.0) * (pressure / 256.0);
  X1 = (X1 * 3038.0) / 65536.0;
  X2 = ((-7357) * pressure) / 65536.0;
  pressure = (pressure + (X1 + X2 + 3791) / 16.0) / 100;

  // Calculo da altidute
  float height = 44330 * (1 - pow((pressure / 1013.25), 0.1903));

  // Output do serial monitor
  Serial.print("Altitude : ");
  Serial.print(height);
  Serial.println(" m");
  Serial.print("Pressao : ");
  Serial.print(pressure);
  Serial.println(" hPa");
  Serial.print("Temperature em Celsius : ");
  Serial.print(cTemp);
  Serial.println(" C");
  Serial.print("Temperature em Fahrenheit : ");
  Serial.print(fTemp);
  Serial.println(" F");

  //Abre o arquivo.txt do cartão SD
  File datafile = SD.open("dadosMPU/BMP.txt", FILE_WRITE);
  //Grava os dados no arquivo
  if(datafile)
  {
    datafile.print(AX);
    datafile.print(" ");
    datafile.print(AY);
    datafile.print(" ");
    datafile.print(AZ);
    datafile.print(" ");
    datafile.print(GX);
    datafile.print(" ");
    datafile.print(GY);
    datafile.print(" ");
    datafile.print(GZ);
    datafile.print(" ");
    datafile.print(height);
    datafile.print(" ");
    datafile.print(pressure);
    datafile.print(" ");
    datafile.println(cTemp);
    datafile.close();
  }
  else
  {
    //Mensagem de erro para a abertura do arquivo
    Serial.println("Erro ao abrir dadosMPU/BMP.txt !");
  }  
  delay(50);
}
