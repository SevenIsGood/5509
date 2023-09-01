/*
* Example to verify the C5545 BP INA device access from MSP432 LP
* 
* This program is free software; you can redistribute it and/or modify it 
* under the terms of the GNU General Public License as published by the 
* Free Software Foundation; version 2.
*
* This program is distributed in the hope that it will be useful, but 
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
* or FITNESS FOR A PARTICULAR PURPOSE. 
* See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along 
* with this program; if not, write to the Free Software Foundation, Inc., 
* 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*
*/

#include <Wire.h>

int reading = 0;
float read_data;

char c;
float current_lsb[4] = {0.00147796630, 0.00167398071, 0.000417755126, 0.00054138183};
byte con[2] = {0x3f, 0x9f}; /*Calibration register value is fixed for four slaves*/
byte cal[2];

void setup()
{  
 
  Serial.begin(9600);  
  
  delay(500);
  
  Serial.println("");
  Serial.println("MSP432 INA Device Test");
  Serial.println("");
  
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  digitalWrite(9, HIGH);
  digitalWrite(10, HIGH);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(5, HIGH);
  delay(5000);  
  
  Wire.begin(); 

  /*for slave CVDD*/
  cal[0] = 0x6C;
  cal[1] = 0x41;
  
  Wire.beginTransmission(64);                            

  Wire.write(byte(0x00));     
  Wire.write(con[0]);         
  Wire.write(con[1]);         
  Wire.endTransmission();     

  Wire.write(byte(0x05));     
  Wire.write(cal[0]);         
  Wire.write(cal[1]);         
  Wire.endTransmission();     

  delay(70);                   

  Serial.println("");	
  Serial.println("Reading values from CVDD port");
  Serial.println("");

  Wire.beginTransmission(64); 
  Wire.write(byte(0x01));      
  Wire.endTransmission();      

  
  Wire.requestFrom(64, 2);    

  
  if(2 <= Wire.available())   
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = read_data * 0.01;
  Serial.print("shunt Voltage -");   
  Serial.print( read_data);   
  Serial.print("mv");
  Serial.println("");

  delay(70);                   
  
  Wire.beginTransmission(64); 
  Wire.write(byte(0x02));     
  Wire.endTransmission();     
  
  Wire.requestFrom(64, 2);    

  
  if(2 <= Wire.available())   
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading >> 3;
  read_data = read_data * 0.004;
  Serial.print("Bus Voltage - ");   
  Serial.print(read_data);   
  Serial.print("V");
  Serial.println("");

  delay(70);                   
  
  Wire.beginTransmission(64); 
  Wire.write(byte(0x03));      
  Wire.endTransmission();      

  
  Wire.requestFrom(64, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading;
  read_data = current_lsb[0] * 20 * read_data;
  Serial.print("Power - ");   
  Serial.print(read_data);   
  Serial.print("mW");
  Serial.println("");

  delay(70);                   
  
  Wire.beginTransmission(64); 
  Wire.write(byte(0x04));      
  Wire.endTransmission();      

  
  Wire.requestFrom(64, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = current_lsb[0] * read_data;
  Serial.print("Current - ");   
  Serial.print(read_data);   
  Serial.print("mA");
  Serial.println("");
           
  /*For slave LDOI*/            
  cal[0] = 0x59;
  cal[1] = 0x6C;


Wire.beginTransmission(65); 
                              

  Wire.write(byte(0x00));     
  Wire.write(con[0]);         
  Wire.write(con[1]);         
  Wire.endTransmission();     

  Wire.write(byte(0x05));     
  Wire.write(cal[0]);         
  Wire.write(cal[1]);         
  Wire.endTransmission();     

  delay(70);                   

  Serial.println("");
  Serial.println("Reading values from LDOI port");
  Serial.println("");
  
  Wire.beginTransmission(65); 
  Wire.write(byte(0x01));      
  Wire.endTransmission();      

  
  Wire.requestFrom(65, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = read_data * 0.01;
  Serial.print("shunt Voltage -");   
  Serial.print( read_data);   
  Serial.print("mv");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(65); 
  Wire.write(byte(0x02));      
  Wire.endTransmission();      

  
  Wire.requestFrom(65, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading >> 3;
  read_data = read_data * 0.004;
  Serial.print("Bus Voltage - ");   
  Serial.print(read_data);   
  Serial.print("V");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(65); 
  Wire.write(byte(0x03));      
  Wire.endTransmission();      

  
  Wire.requestFrom(65, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading;
  read_data = current_lsb[0] * 20 * read_data;
  Serial.print("Power - ");   
  Serial.print(read_data);   
  Serial.print("mW");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(65); 
  Wire.write(byte(0x04));      
  Wire.endTransmission();      

  
  Wire.requestFrom(65, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = current_lsb[0] * read_data;
  Serial.print("Current - ");   
  Serial.print(read_data);   
  Serial.print("mA");
  Serial.println("");


 /*For slave DSP_DVDDIO */
  cal[0] = 0xBF;
  cal[1] = 0x7F;
  
    Wire.beginTransmission(68); 
                              

  Wire.write(byte(0x00));     
  Wire.write(con[0]);         
  Wire.write(con[1]);         
  Wire.endTransmission();     

  Wire.write(byte(0x05));     
  Wire.write(cal[0]);         
  Wire.write(cal[1]);         
  Wire.endTransmission();     

  delay(70);                   

  Serial.println("");
  Serial.println("Reading values from DSP_DVDDIO port");
  Serial.println("");
  
  Wire.beginTransmission(68); 
  Wire.write(byte(0x01));      
  Wire.endTransmission();      

  
  Wire.requestFrom(68, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = read_data * 0.01;
  Serial.print("shunt Voltage -");   
  Serial.print( read_data);   
  Serial.print("mv");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(68); 
  Wire.write(byte(0x02));      
  Wire.endTransmission();      

  
  Wire.requestFrom(68, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading >> 3;
  read_data = read_data * 0.004;
  Serial.print("Bus Voltage - ");   
  Serial.print(read_data);   
  Serial.print("V");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(68); 
  Wire.write(byte(0x03));      
  Wire.endTransmission();      

  
  Wire.requestFrom(68, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading;
  read_data = current_lsb[0] * 20 * read_data;
  Serial.print("Power - ");   
  Serial.print(read_data);   
  Serial.print("mW");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(68); 
  Wire.write(byte(0x04));      
  Wire.endTransmission();      

  
  Wire.requestFrom(68, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = current_lsb[0] * read_data;
  Serial.print("Current - ");   
  Serial.print(read_data);   
  Serial.print("mA");
  Serial.println("");
            
  /*For slave VCC3V3_USB*/   
  cal[0] = 0x93;
  cal[1] = 0xC5;

      
 Wire.beginTransmission(72); 
                              

  Wire.write(byte(0x00));     
  Wire.write(con[0]);         
  Wire.write(con[1]);         
  Wire.endTransmission();     

  Wire.write(byte(0x05));     
  Wire.write(cal[0]);         
  Wire.write(cal[1]);         
  Wire.endTransmission();     

 
  delay(70);                   

  Serial.println("");
  Serial.println("Reading values from VCC3V3_USB port");
  Serial.println("");
  
  Wire.beginTransmission(72); 
  Wire.write(byte(0x01));      
  Wire.endTransmission();      

  
  Wire.requestFrom(72, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = read_data * 0.01;
  Serial.print("shunt Voltage -");   
  Serial.print( read_data);   
  Serial.print("mv");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(72); 
  Wire.write(byte(0x02));      
  Wire.endTransmission();      

  
  Wire.requestFrom(72, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading >> 3;
  read_data = read_data * 0.004;
  Serial.print("Bus Voltage - ");   
  Serial.print(read_data);   
  Serial.print("V");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(72); 
  Wire.write(byte(0x03));      
  Wire.endTransmission();      

  
  Wire.requestFrom(72, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading;
  read_data = current_lsb[0] * 20 * read_data;
  Serial.print("Power - ");   
  Serial.print(read_data);   
  Serial.print("mW");
  Serial.println("");
  
  delay(70);                   
  
  Wire.beginTransmission(72); 
  Wire.write(byte(0x04));      
  Wire.endTransmission();      

  
  Wire.requestFrom(72, 2);    

  
  if(2 <= Wire.available())    
  {
    reading = Wire.read();  
    reading = reading << 8;    
    reading |= Wire.read(); 
  }

  delay(250);                  

  read_data = reading & 0x7fff;
  read_data = current_lsb[0] * read_data;
  Serial.print("Current - ");   
  Serial.print(read_data);   
  Serial.print("mA");
  Serial.println("");

  Serial.println(""); 
  Serial.print("MSP432 INA Device Test Completed");

}

void loop()
{


}
