/*
 * Edited by Adria Luque
 * Based on Velleman's online example
 * PIN Layout (RF -> Arduino):
 *    VCC -> 3.3V (NOT 5V!!)
 *    RST -> Pin 9
 *    GND -> GND
 *    MISO -> Pin 12
 *    MOSI -> Pin 11
 *    SCK -> Pin 13
 *    NSS -> Pin 10
 *    IRQ -> Not Used
*/

#include <SPI.h>
#include <RFID.h>

#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN, RST_PIN);

int buzz = 7;
int redLed = 8;
int greenLed = 4;
int serNum[5];

// tag 0 ->   91  226    6   22  169
// tag 1 ->  129  117  237   43   50
// tag 2 ->   57  131    5  107  212
// tag 3 ->  236  220  230  171  125
// tag 4 ->   54  199  133  187  207
// tag 5 ->  210  116    4  107  201

int tags[][5] = {{91,226,6,22,169}, {129,117,237,43,50},
                 {57,131,5,107,212}, {236,220,230,171,125},
                 {54,199,133,187,207}, {210,116,4,107,201}};


// Generic card  -> 32  231 137 171 229
                 
int card[][5] = {{32,231,137,171,229}, {0,0,0,0,0}};

float prices[6] = {1.5, 3.7, 2.1, 5.0, 1.2, 6.6};
bool bought[6] = {false, false, false, false, false, false};

float totalcost = 0;



void printid() {
  Serial.print(rfid.serNum[0]);
  Serial.print(" ");
  Serial.print(rfid.serNum[1]);
  Serial.print(" ");
  Serial.print(rfid.serNum[2]);
  Serial.print(" ");
  Serial.print(rfid.serNum[3]);
  Serial.print(" ");
  Serial.print(rfid.serNum[4]);
  Serial.println("");
}



void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.init();

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);

  pinMode(buzz, OUTPUT);

}


void loop() {
  
  if( rfid.isCard() ) {
    if ( rfid.readCardSerial() ) {

      bool product = false;
      bool payment = false;
      int item;
      int buyer;

      for(int x=0; x<sizeof(tags); x++) {
        for(int i=0; i<sizeof(rfid.serNum); i++) {
          
          if(rfid.serNum[i] != tags[x][i]) {
            product = false;
            break;
          }
          else {
            item = x;
            product = true;
          }
          
        }
        if(product) break;
      }

      for(int x=0; x<sizeof(card); x++) {
        for(int i=0; i<sizeof(rfid.serNum); i++) {
          
          if(rfid.serNum[i] != card[x][i]) {
            payment = false;
            break;
          }
          else {
            buyer = x;
            payment = true;
          }
          
        }
        if(payment) break;
      }
      
      

      if(product) {

        bool canbuy = false;
        if (bought[item] == false) {
          canbuy = true;
          bought[item] = true;
        }

        if(canbuy) {
          
          printid();
          tone(buzz, 1600);
          digitalWrite(greenLed, HIGH);
          Serial.print("Buying product. Tag number: ");
          Serial.println(item);
          delay(150);
          noTone(buzz);
          delay(150);
          digitalWrite(greenLed, LOW);

          totalcost += prices[item];
          Serial.print("Subtotal: ");
          Serial.print(totalcost);
          Serial.println(" $");
          Serial.println("");

          while( rfid.readCardSerial() ) {
            delay(1);
          }

        }

        else {
          digitalWrite(redLed, HIGH);
          tone(buzz, 400);
          Serial.println("ITEM ALREADY BOUGHT!");
          Serial.print("Tag number: ");
          Serial.println(item);
          Serial.println("");
          delay(200);
          digitalWrite(redLed, LOW);
          noTone(buzz);
          delay(200);
        }
        
      }

      else if((payment)) {
        if (totalcost != 0) {
          Serial.print("Credit card accepted. ID: ");
          tone(buzz, 1000);
          digitalWrite(greenLed, HIGH);
          printid();
          delay(100);
          tone(buzz, 2000);
          Serial.print("Now paying. Total: ");
          Serial.print(totalcost);
          Serial.println(" $");
          Serial.println("");
          delay(100);
          noTone(buzz);
          digitalWrite(greenLed, LOW);
          totalcost = 0;
          delay(500);
        }
        
      }

      else {
        printid();
        Serial.println("IS NOT A VALID ID!");
        Serial.println("");
        digitalWrite(redLed, HIGH);
        tone(buzz, 300);
        delay(100);
        tone(buzz, 200);
        delay(200);
        digitalWrite(redLed, LOW);
        noTone(buzz);
        delay(300);
      }
      
    }
    
  }
}









