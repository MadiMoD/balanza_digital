//=========================================================================================ULTIMA MODIFICACION = 4 NOV 2018 20:22

//=============================LIBRERIAS
#include <Keypad.h>
#include <SD.h>
#include <SPI.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include "HX711.h"
#include <SoftwareSerial.h>                        

//=============================CONSTANTES
#define DOUT  A1
#define CLK  A0
HX711 balanza(DOUT, CLK);
LiquidCrystal_PCF8574 lcd(0x27);
SoftwareSerial BT1(30,31); // RX, TX            
const byte rowsCount = 4;
const byte columsCount = 4;

//=============================TECLADO
char keys[rowsCount][columsCount] = {
   { '1','2','3', 'A' },
   { '4','5','6', 'B' },
   { '7','8','9', 'C' },
   { '*','0','#', 'D' }
};
const byte rowPins[rowsCount] = { 2, 10, 9, 8 };
const byte columnPins[columsCount] = { 7, 6, 5, 3 };
Keypad keypad = Keypad(makeKeymap(keys), rowPins, columnPins, rowsCount, columsCount);

File myFile; //SD

//=============================VARIABLES
String ced=""; // se almacena el numero de cedula String
float resu_peso_ref=0; // 
int peso=0; //
String info=""; // variable que contiene el valor de ced + . + peso
float resu1=0; // Variable que almacena el calculo de....
float set_scale=0; //
int ref=0;// Variable para reconocer si se realizo el proceso de calibracion

void setup()
{
  Serial.begin(9600);
  BT1.begin(9600);                         
  pinMode(53,OUTPUT); // PIN SC-SS DEL MODULO SD AL ARDUINO
  lcd.setBacklight(255);
  Wire.begin();
  Wire.beginTransmission(0x27);
  lcd.begin(20, 4); // initialize the lcd
         lcd.clear();
         lcd.print("* DIGITAL PEDIATRIC");
         lcd.setCursor(0,1);
         lcd.print("BALANCE.");
         delay(1000);
             if (!SD.begin(53))
             {
               lcd.setCursor(0,3);
               lcd.print("* SD STATUS: FAILED.");
               delay(2000);
               return;
             }
             lcd.setCursor(0,3);
             lcd.print("* SD STATUS: OK.");
             delay(2000);
}
 
void loop()
{
   lcd.clear();
   delay(500);
   lcd.setCursor(0,1);
   lcd.print("TO CALIBRATE PRESS 1");
   lcd.setCursor(0,3);
   lcd.print("TO WEIGHT PRESS 2");
         
         char n;
         int y=0; // variable para salir del lado do while para elegir opcion 1 o 2 
         
             do
             {
                n= keypad.getKey();
                if (n!=NO_KEY)
                {
                      if((n=='1')||(n=='2'))
                      {
                        y=1;
                      }
                        else
                        { 
                          lcd.clear();
                          lcd.setCursor(0,1);
                          lcd.print("* INVALID CHARACTER.");
                          delay(1000);
                          lcd.clear();
                          lcd.setCursor(0,1);
                          lcd.print("TO CALIBRATE PRESS 1");
                          lcd.setCursor(0,3);
                          lcd.print("TO WEIGHT PRESS 2");
                        }
                }
              }while(y!=1);

                   if (n=='1')
                        {
                         pre_calibrar();
                         calibrar();  
                         calcular();
                         ref=1; // se realizó el proceso de calibración
                        }
                          else if (n=='2')
                             {
                              cedula();       
                              pre_pesar();
                              pesar();
                              grabar();                     
                             }            
}

void cedula()
{
   lcd.clear();
   lcd.print("* TYPE THE C.I.");
   lcd.setCursor(0,1);
   lcd.print("NUMBER.");
   delay(500);
   lcd.setCursor(0,2);
   lcd.print("* THEN,");
   delay(500);
   lcd.setCursor(0,3);
   lcd.print("* PRESS # TO SAVE.");
   char key1[10];
         char n;
         int i=0;
         char y=""; // variable para salir del lazo do while
         ced="";    
             do
             {
                n= keypad.getKey();
                if (n!=NO_KEY)
                {
                  key1[i]=n;
                              if (i<9)
                              {
                                 ced+=String(key1[i]);
                                 lcd.clear();
                                 lcd.print("* C.I. NUMBER: ");
                                 lcd.setCursor(0,1);
                                 lcd.print(ced);  
                                i++;
                              }
                              else
                              {
                                 ced+=String(key1[i]);
                                 lcd.clear();
                                 lcd.print("* C.I. NUMBER: ");
                                 lcd.setCursor(0,1);
                                 lcd.print(ced); 
                                    do
                                    { 
                                        lcd.setCursor(0,3);
                                        lcd.print("PRESS # TO CONTINUE.");
                                 
                                           n= keypad.getKey();
                                           if (n!=NO_KEY)
                                           {
                                            y='#';
                                           }
                                    }while(n!='#');    
                              }
                }
              }while(y!='#');
}

void pre_pesar()
{
  float a=0;
  lcd.clear();
  lcd.print("* READING ADC: "); 
  lcd.setCursor(0,1);
  lcd.print(balanza.read());  
  delay(500);
  lcd.setCursor(0,2);
  lcd.print("DON'T PUT ANY WEIGHT");  
  lcd.setCursor(0,3);
  delay(500);
  lcd.print("* DESTARING...");  
  delay(500);
  if (ref==0)
  a=10.74;
  else
  a=set_scale;
  balanza.set_scale(a); // Establecemos la escala   9900 en kg y 10.84 en gramos 
  balanza.tare(20); //El peso actual es considerado Tara.
}


void pesar()
{
  lcd.clear();
  lcd.print("* PUT THE WEIGHT ON");
  lcd.setCursor(0,1);
  lcd.print("THE BALANCE.");
  delay(500);
  lcd.setCursor(0,2);
  lcd.print("* THEN,");
  delay(500);
  lcd.setCursor(0,3);
  lcd.print("* PRESS C TO SHOW.");  
         char n;     
             do
             {
                n= keypad.getKey();
                if (n!=NO_KEY)
                {
                }
              }while(n!='C');
              
                lcd.clear();
                lcd.print("* WAIT PLEASE...");
                
                    do{
                      peso=balanza.get_units(20);
                      }while(peso<0);
                      
                        lcd.setCursor(0,1);
                        lcd.print("* WEIGHT IN GRAMS:");
                        lcd.setCursor(0,2);
                        lcd.print(peso);
                        delay(1000);
}

void pre_calibrar()
{
  lcd.clear(); 
  lcd.print("TYPE A KNOWN WEIGHT.");
    char key1[5];
               char n;
               int i=0;
               char y=""; // variable para salir del lazo do while
               String peso_ref="";    
                     do
                      {
                        n= keypad.getKey();
                        if (n!=NO_KEY)
                        {
                          key1[i]=n;
                          
                              if (i<4)
                              {
                                  peso_ref+=String(key1[i]);
                                  lcd.setCursor(0,1);
                                  lcd.print("* KNOWN WEIGHT IN gr");
                                  lcd.setCursor(0,2);
                                  lcd.print(peso_ref);      
                                i++;
                              }
                              else
                              {
                                  peso_ref+=String(key1[i]);
                                  lcd.setCursor(0,1); 
                                  lcd.print("* KNOWN WEIGHT IN gr");
                                  lcd.setCursor(0,2);
                                  lcd.print(peso_ref);
                                    
                                    do
                                    { 
                                        lcd.setCursor(0,3);
                                        lcd.print("PRESS # TO CONTINUE.");
                                           n= keypad.getKey();
                                           if (n!=NO_KEY)
                                           {
                                            y='#';
                                           }
                                    }while(n!='#');   
                              }
                        }
                      }while(y!='#');
 
                                  resu_peso_ref= peso_ref.toFloat(); 
}  

void calcular()
{ 
  set_scale=resu1/resu_peso_ref;
  lcd.setCursor(0,2);
  lcd.print(set_scale);
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,1);
  lcd.print("BALANCE IS CALIBRATED.");
                          lcd.setCursor(0,3);
                          lcd.print("PRESS A TO CONTINUE.");  
                          char n;     
                               do
                               {
                                  n= keypad.getKey();
                                  if (n!=NO_KEY)
                                  {
                                  }
                                }while(n!='A');
}
  
void calibrar()
{  
  String resu="";
  lcd.clear(); 
  lcd.print("* READING ADC...");
  lcd.setCursor(0,1);
  lcd.print(balanza.read());
  delay(500);
  lcd.setCursor(0,2);
  lcd.print("DON'T PUT ANY WEIGHT");
  lcd.setCursor(0,3);
  lcd.print("...ON THE BALANCE.");
  delay(2000);
  lcd.clear(); 
  lcd.print("* DESTARING...");
  delay(1000);
  balanza.set_scale(); //La escala por defecto es 1
  balanza.tare(20);  //El peso actual es considerado Tara.
  lcd.clear(); 
  lcd.print("* PUT A KNOWN WEIGHT");
  lcd.setCursor(0,1);
  lcd.print("... ON THE BALANCE.");
  delay(500);
  lcd.setCursor(0,2);
  lcd.print("* THEN,");
  delay(500);
  lcd.setCursor(0,3);
  lcd.print("PRESS D TO CONTINUE.");           
        char n;     
             do
             {
                n= keypad.getKey();
                if (n!=NO_KEY)
                {
                }
              }while(n!='D');
              
  lcd.clear(); 
  lcd.print("* READING VALUE:");
  resu=balanza.get_value(10);
  resu1=resu.toFloat();
  lcd.setCursor(0,1);
  lcd.print(resu1);
  delay(500);
}

void grabar()
{
   lcd.clear();
   lcd.print("* SAVING...");
    delay(500);
    info= ced+'.'+peso;
    lcd.clear();
    lcd.print("* C. I.: ");
    lcd.setCursor(9,0);
    lcd.print(ced);
    delay(500);
    lcd.setCursor(0,1);
    lcd.print("* WEIGHT (gr): ");
    lcd.setCursor(15,1);
    lcd.print(peso);
    delay(500); 
                  myFile = SD.open("datalog1.txt", FILE_WRITE);//abrimos  el archivo
                       if (myFile)
                       { 
                          myFile.println(info);
                          BT1.print(info); // transmision de variable por el bluetooth
                          delay(500);
                          myFile.close(); //cerramos el archivo
                          lcd.setCursor(0,2);
                          lcd.print("* SAVED."); 
                          delay(1000);
                       }
                        else 
                        {
                          lcd.setCursor(0,2);
                          lcd.print("SAVED PROCESS FAILED");
                          delay(1000);
                        } 
                          lcd.setCursor(0,3);
                          lcd.print("PRESS B TO CONTINUE.");  
                          char n;     
                               do
                               {
                                  n= keypad.getKey();
                                  if (n!=NO_KEY)
                                  {
                                  }
                                }while(n!='B');
}


