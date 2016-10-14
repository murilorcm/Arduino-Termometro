/* 
Programa:  
  
  Leitor de Temperatura em Celsios com armazenamento de dados em 
  Micro SD integrado com RTC DS1307 e exibindo resultados no LCD
  16x2. 

*/

#include <LiquidCrystal.h>  // LCD 16x2.
#include <DS1307.h>         // RTC DS1307
#include <SPI.h>            // SD
#include <SD.h>             // SD

/* - - - - - - LM35 - - - - - -*/
const int LM35 = A0;
float tempC, tempMaxC = 0, tempMinC = 0;
/* - - - - - - - - - - - - - - */

/* - - - - -RTC DS1307 - - - - */

//RTC DS1307 ligado as portas A1(SDA) e A2(SCL) do Arduino.
DS1307 rtc(A2, A1);
String hora;
/* - - - - - - - - - - - - - - */

/* - - - - - - SD - - - - - - -*/
File meuArquivo;
const int chipSelect = 10;
int flag = 0;
/* - - - - - - - - - - - - - - */

/* - - - - - LCD 16x2 - - - - -*/

//LCD 16x2 ligado as portas: 9(RS), 8(E), 6(D4), 5(D5), 4(D6) 3(D7),  do Arduino.
LiquidCrystal lcd(9, 8, 6, 5, 4, 3);
const int luzDeFundo = 7;

byte temperatura[8] = {
 B00100,
 B01010,
 B01010,
 B01010,
 B01110,
 B11111,
 B11111,
 B01110
};
byte grau[8] = {
 B11100,
 B10100,
 B11100,
 B00000,
 B00000,
 B00000,
 B00000,
 B00000
};
byte maxi[8] = {
 B00100,
 B01110,
 B01110,
 B11111,
 B11111,
 B00100,
 B00100,
 B00100
};
byte mini[8] = { 
 B00100,
 B00100,
 B00100,
 B11111,
 B11111,
 B01110,
 B01110,
 B00100
};
/* - - - - - - - - - - - - - - */



float medirTemperaturaC()
{
  float tC = 0;
  int i;
  
  for(i = 0;i < 8; i++)
  {
    tC += ( 5.0 * analogRead(LM35) * 100.0) / 1024.0;
    delay(100);
  }
  
  tC = tC / 8.0;
  
  return tC;
}

float compararTemperaturaMaxC(float tC, float tMC)
{
  float auxtMC;
  
  if(tMC == 0)
  {
    auxtMC = tC;  
  }
  else if(tC > tMC)
  {  
    auxtMC = tC;
  }
  else
  {
    auxtMC = tMC; 
  }
  
  return auxtMC;
}

float compararTemperaturaMinC(float tC, float tMC)
{
  float auxtMC;
  
  if(tMC == 0)
  {
    auxtMC = tC;  
  }
  else if(tC < tMC)
  {  
    auxtMC = tC;
  }
  else
  {
    auxtMC = tMC; 
  }
  
  return auxtMC;
}

void exibirLcdTempData(int tC, int tMaC, int tMiC)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.write(1); // Ícone de temperatura.
  lcd.setCursor(2, 0);
  lcd.print(tC, DEC); // Temperatura.
  lcd.write(2); // Símbolo de grau.
  lcd.print("C"); // C = Celsius.

  lcd.setCursor(7, 0);
  lcd.write(3);
  lcd.print(tMaC, DEC);
  lcd.write(2);

  lcd.setCursor(12, 0);
  lcd.write(4);
  lcd.print(tMiC, DEC);
  lcd.write(2);

  lcd.setCursor(1, 1);
  lcd.print(rtc.getTimeStr(FORMAT_SHORT));
  lcd.setCursor(7, 1);
  lcd.print(rtc.getDateStr(FORMAT_SHORT));
}

int iniciaSD()
{
  int erro;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando o SD");
  lcd.setCursor(0, 1);
  lcd.print("Card...");
  delay(3000);
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Falha ao Iniciar!");
    erro = 1;
  }
  else
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("SD card, pronto");
    lcd.setCursor(0, 1);
    lcd.print("para uso!");
    erro = 0;
  }
  delay(100);
  
  return erro;
}

int armazenarTemperaturaC(float tC)
{
  int erro;
  
  meuArquivo = SD.open("dados.txt", FILE_WRITE);// Abre o arquivo dados.txt do cartao SD

  // Grava os dados no arquivo
  if (meuArquivo) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gravando info em");
    lcd.setCursor(0, 1);
    lcd.print("dados.txt...");
    delay(5000);
    meuArquivo.print("TEMPERATURA: ");
    meuArquivo.print(tC);
    meuArquivo.println(" ºC.");
    meuArquivo.print("HORA : ");
    meuArquivo.println(rtc.getTimeStr());
    meuArquivo.print(" DATA: ");
    meuArquivo.print(rtc.getDateStr(FORMAT_SHORT));
    meuArquivo.print(" ");
    meuArquivo.println(rtc.getDOWStr());
    meuArquivo.println("");
    meuArquivo.close();
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print("Concluido!");
    erro = 0;
   }
   else
   {
    // Mensagem de erro caso ocorra algum problema na abertura do arquivo
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Erro ao abrir");
    lcd.setCursor(0, 1);
    lcd.print("dados.txt !");
    erro = 1;
   }
   meuArquivo.close();
   
   delay(1000);
   return erro;
}

void setup()
{
  /* - - - - - LCD 16x2 - - - - -*/
  pinMode(luzDeFundo, OUTPUT);
  analogWrite(luzDeFundo, 110);// 0 - 254.
  
  lcd.begin(16, 2);
  
  lcd.createChar(1, temperatura);
  lcd.createChar(2, grau);
  lcd.createChar(3, maxi);
  lcd.createChar(4, mini);
  /* - - - - - - - - - - - - - - */
  
  /* - - - - -RTC DS1307 - - - - */
  //Aciona o relogio
  rtc.halt(false);
  //As linhas abaixo setam a data e hora do modulo
  //e podem ser comentada apos a primeira utilizacao
  rtc.setDOW(FRIDAY);      //Define o dia da semana
  rtc.setTime(16, 54, 0);     //Define o horario
  rtc.setDate(7, 10, 2016);   //Define o dia, mes e ano
  //Definicoes do pino SQW/Out
  rtc.setSQWRate(SQW_RATE_1);
  rtc.enableSQW(true);
  /* - - - - - - - - - - - - - - */
  
  /* - - - - - - SD - - - - - - -*/
  pinMode(2, OUTPUT);
  flag = iniciaSD();
  if(flag != 0)
  {
   digitalWrite(2, HIGH);
  }
  delay(3000);
  /* - - - - - - - - - - - - - - */
}
void loop()
{ 
  
  tempC = medirTemperaturaC();
  
  tempMaxC = compararTemperaturaMaxC(tempC, tempMaxC);
  tempMinC = compararTemperaturaMinC(tempC, tempMinC);
  
  
  exibirLcdTempData(int(tempC), int(tempMaxC), int(tempMinC));
  
  hora = rtc.getTimeStr();
  
  if ((hora == "08:00:00") || (hora == "10:00:00") || (hora == "12:00:00") || (hora == "14:00:00") || (hora == "16:00:00") || (hora == "18:00:00") || (hora == "20:00:00"))
  {
    flag = armazenarTemperaturaC(tempC);
    if(flag != 0)
    {
     digitalWrite(2, HIGH);
    }
    delay(3000);
  }
  tempC = 0;
  delay(500);
}
