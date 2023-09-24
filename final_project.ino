const int BUTTON_PIN = 2;  // the number of the pushbutton pin
const int LED_PIN = 13;
#define USE_ARDUINO_INTERRUPTS true    // Set-up low-level interrupts for most acurate BPM math.
#include <PulseSensorPlayground.h>  

#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

static const uint8_t PIN_MP3_TX = 4; // DFPlayer modülünde RX  pinine
static const uint8_t PIN_MP3_RX = 3; // DFPlayer modülünde TX pinine
SoftwareSerial softwareSerial(PIN_MP3_RX, PIN_MP3_TX);

// Oynatıcı objemizi Player adında oluşturuyoruz.
DFRobotDFPlayerMini player;   // Includes the PulseSensorPlayground Library.   

//  Variables
const int PulseWire = 0;       // PulseSensor PURPLE WIRE connected to ANALOG PIN 0
const int LED = LED_BUILTIN;          // The on-board Arduino LED, close to PIN 13.
int Threshold = 515;           // Determine which Signal to "count as a beat" and which to ignore.
                               // Use the "Gettting Started Project" to fine-tune Threshold Value beyond default setting.
                               // Otherwise leave the default "550" value. 
                               
PulseSensorPlayground pulseSensor;  // Creates an instance of the PulseSensorPlayground object called "pulseSensor"

    // the number of the LED pin

// variables will change:
  // variable for reading the pushbutton status
byte lastButtonState = LOW;
byte ledState = LOW;
void setup() {
  Serial.begin(9600);
  softwareSerial.begin(9600);

  // Eğer arduino ile dfplayer arasında bağlantı kurulmuş ise OK yazıdırıyoruz
  if (player.begin(softwareSerial)) {
   Serial.println("OK, music player is ready too.");

    // Ses seviyesini belirliyoruz (0 to 30).
    player.volume(20);
    // sd karttaki ilk müziği oynatıyoruz.
    //player.play(1);
  } else {
    // dfplayer ile bağlantının kurulamadığını yazdırıyoruz.
    Serial.println("Connecting to DFPlayer Mini failed!");

  }
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  // Configure the PulseSensor object, by assigning our variables to it. 
  pulseSensor.analogInput(PulseWire);   
  pulseSensor.blinkOnPulse(LED);       //auto-magically blink Arduino's LED with heartbeat.
  pulseSensor.setThreshold(Threshold);   

  // Double-check the "pulseSensor" object was created and "began" seeing a signal. 
   if (pulseSensor.begin()) {
    Serial.println("We created a pulseSensor Object !");  //This prints one time at Arduino power-up,  or on Arduino reset.  
  }



}
void loop() {
  bool x = true;
  int toggleCount = 0;
  bool musicplaying = false;
  while (x){
    //int toggleCount=0;
    byte buttonState = digitalRead(BUTTON_PIN);
    if (buttonState != lastButtonState) {
      lastButtonState = buttonState;
      if (buttonState == LOW) {
        //Serial.print(toggleCount);
        //Serial.println("toggle");
        toggleCount= toggleCount +1;
        while (toggleCount == 1 && (musicplaying == false)){
          Serial.println("Press your finger, then wait for few seconds...");
          delay(8000);
          int myBPM1=0;
          int myBPM2= 0;
          int myBPM3=0;
          int BPMs[3];
          int beatCount = 0;

        
          while(beatCount<3){
            if (pulseSensor.sawStartOfBeat()) { 
            
              BPMs[beatCount] = pulseSensor.getBeatsPerMinute();
              //Serial.println("♥  A HeartBeat Happened ! "); // If test is "true", print a message "a heartbeat happened".
              //Serial.print("BPM");
              //Serial.print(beatCount+1);
              //Serial.print(": ");
              //Serial.println(BPMs[beatCount]);
              beatCount = beatCount+1;
            }

          }

          if((abs(BPMs[0]-BPMs[1]) <3 )&&(abs(BPMs[0]-BPMs[2]) <3)&&(abs(BPMs[1]-BPMs[2]) <3)){
            int mainBPM = (BPMs[0] + BPMs[1] + BPMs[2])/3;
            Serial.println("BPM: ");
            Serial.println(mainBPM);
            if (mainBPM > 80){
              Serial.println("Music is playing...");
              player.play(1);
              musicplaying = true;
              //Serial.println("playing");
              //delay(654000);
              //Serial.println("delau end");
              
            }
            else{
              Serial.println("You're fine...");
              ledState = LOW;
              lastButtonState= LOW;
              toggleCount = 0;
            }
          }
          else{
            Serial.println("Wrong Read, try again.");
            
          }
        }

          //start all processes
      }
        if (toggleCount == 2){
          //Serial.println("buradayım.");
          if (musicplaying == true){
            player.pause();
            Serial.println("Stopped the music.");
            musicplaying = false;
          }         
          toggleCount = 0;
          Serial.println("stop measuring heart rate");
        }
        
        
    }
    
    delay(10);
  }
}