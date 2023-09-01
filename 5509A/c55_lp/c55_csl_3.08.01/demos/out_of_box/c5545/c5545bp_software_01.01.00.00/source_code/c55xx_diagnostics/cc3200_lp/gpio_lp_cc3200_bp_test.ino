/*
* Example to verify the interface lines between CC3200 LP and C5545 BP as GPIOs
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

char var=0;
String inputString = ""; 
boolean stringComplete = false; 
int onetimeCounter=0;
int retValue = false;
int softrst = 5;
int counter =0;
int incomingByte;

//digital pins
  int uartrx = 3;
  int uarttx = 4;
  int i2s1fs = 27;
  int i2s1clk = 28;
  int i2s1rx = 29;
  int i2s1dx = 30;

//  int softrst = 5;
  int i2c1scl = 9;
  int i2c1sda = 10;
  int rstout = 16;

  int ret=0;
void setup()
{
   // put your setup code here, to run once:
   Serial.begin(9600);
   // var = menu();
   //LP_SOFT_RST
   if(counter==0)
   {
   pinMode(softrst, OUTPUT);//INPUT_PULLUP);
   digitalWrite(softrst, HIGH);
   counter = counter +1;
   }
   
   delay(100);
    
   Serial.print("CC3200 LP to C5545 BP GPIO Test");
   Serial.println("");      
   
  pinMode(uartrx, INPUT);//INPUT_PULLUP);  
  pinMode(i2s1dx, INPUT);//INPUT_PULLUP);
  
  pinMode(uarttx, OUTPUT);
  pinMode(i2s1fs, OUTPUT);
  pinMode(i2s1clk, OUTPUT);
  pinMode(i2s1rx, OUTPUT);
  
  if (onetimeCounter==0)
  {
       delay(5000);
       onetimeCounter = onetimeCounter + 1;
  }
  
  /*************READ TEST************************/
  
  Serial.println("");
  Serial.print("Enter Any Character to Proceed further:\n");
  incomingByte = Serial.read();  
  
  Serial.print("Waiting for DSP to write HIGH");
  Serial.println("");      
  
  while(1)
  {
     ret = digitalRead(i2s1dx);
    
    if(ret)
   {

      delay(100);
      Serial.print("Values are HIGH from DSP");  
       Serial.println("");      
      Serial.print("i2s1dx");     
      Serial.print("\t");    
      Serial.print("uartrx");     
      Serial.print("\t");    
       Serial.println("");   
      Serial.print(ret);
      break;
   }
  }

  Serial.print("\t");    
   
  ret = digitalRead(uartrx);
  Serial.print(ret);
  Serial.print("\t");    

  Serial.println("");      
  if(!ret)
     fail_func();

  /***************WRITE TEST*****************************************/
   
 // Serial.println("");
  // Serial.print("\t Launchpad Write test.\n"); 
   Serial.println("");
  
  Serial.print("Launchpad as Output, writing HIGH");
  Serial.println("");
 
    Serial.print("uarttx");     
   Serial.print("\t");    
     Serial.print("i2s1fs");     
    Serial.print("\t");    
     Serial.print("i2s1clk");     
    Serial.print("\t");    
     Serial.print("i2s1rx");     
    Serial.print("\t");    
    
   Serial.println("");      
  
    digitalWrite(uarttx, HIGH);
  Serial.print(1);
   Serial.print("\t");    
  delay(1);
  digitalWrite(i2s1fs, HIGH);
  Serial.print(1);
   Serial.print("\t");    
  delay(1);
  digitalWrite(i2s1clk, HIGH);
  Serial.print(1);
   Serial.print("\t");    
  delay(1);
   digitalWrite(i2s1rx, HIGH);
  Serial.print(1);
    
 
   
   /******************************************READ TEST********************/
   Serial.println("");      
  // Serial.println("");      
  // Serial.println("");
  //Serial.print(" LaunchPad Read test.\n"); 
  Serial.println("");
   Serial.print("Waiting for DSP to write LOW");
   Serial.println("");      
     
  while(1)
  { 
 ret = digitalRead(i2s1dx);    

   if(!ret)
   {
     delay(100);
      Serial.print("Values are read LOW from DSP");    
      Serial.println("");       
       Serial.print("i2s1dx");     
    Serial.print("\t");    
      Serial.print("uartrx");     
      Serial.print("\t");        
     Serial.println("");      
    Serial.print(ret);
      break;
   }
  }

  Serial.print("\t");    
 
    ret = digitalRead(uartrx);
  Serial.print(ret);
  Serial.print("\t");    
  
   
   Serial.println("");      
   Serial.println("");      
   if(ret)
     fail_func();
   /*************writetest low************************************/
  Serial.print("Launchpad as Output, writing LOW");
  
  Serial.println("");
  
    Serial.print("uarttx");     
  Serial.print("\t");    
     Serial.print("i2s1fs");     
    Serial.print("\t");    
     Serial.print("i2s1clk");     
    Serial.print("\t");    
     Serial.print("i2s1rx");     
    Serial.print("\t");    
    Serial.println("");      
    digitalWrite(uarttx, LOW);
  Serial.print(0);
   Serial.print("\t");    
  delay(1);
  digitalWrite(i2s1fs, LOW);
  Serial.print(0);
   Serial.print("\t");    
  delay(1);
  digitalWrite(i2s1clk, LOW);
  Serial.print(0);
   Serial.print("\t");    
  delay(1);
  digitalWrite(i2s1rx, LOW);
  Serial.print(0);
  Serial.println("");      
 
  Serial.println("");       
     
  Serial.print("CC3200 LP to C5545 BP GPIO Test Passed!");
 
    
 }

void loop()
{

}
void fail_func()
{
  Serial.println("CC3200 LP to C5545 BP GPIO Test Failed!");
   
     while(1);  
}


