#include "pitches.h"

/////=====Structs_&_Arrays=====/////
struct Button {const int id; const char type; int note; const int pin; int lastButtonState; unsigned long lastDebounceTime; int buttonState;};
/////====White Note Buttons=====/////
Button bC3 = {1, 'n', 56, 8};
Button bD3 = {3, 'n', 58, 9};
Button bE3 = {5, 'n', 60, 10};
Button bF3 = {7, 'n', 61, 11};
Button bG3 = {9, 'n', 63, 12};
Button bA3 = {11, 'n', 0x42, 20};
Button bB3 = {13, 'n', 0x44, 21};
Button bC4 = {15, 'n', 0x46, 22};


/////====Black Note Buttons=====/////
//Button bbC3 = {1, 'n', 0x49, 8};
//Button bbD3 = {3, 'n', 0x4B, 9};
//Button bbE3 = {5, 'n', 0x4D, 10};
//Button bbF3 = {7, 'n', 0x4F, 11};
//Button bbG3 = {9, 'n', 0x51, 12};
//Button bbA3 = {11, 'n', 0x53, 20};
//Button bbB3 = {13, 'n', 0x55, 21};
//Button bbC4 = {15, 'n', 0x57, 22};

/////=====Velocity_Buttons=====/////
Button bC3V = {2, 'v', 56, 2};
Button bD3V = {4, 'v', 58, 3};
Button bE3V = {6, 'v', 60, 4};
Button bF3V = {8, 'v', 61, 5};
Button bG3V = {10, 'v', 63, 6};

/////======Other_Buttons=====/////
Button bSw = {11, 'w', "", 16};
Button bOctDwn = {12, 'd', "", 15};
Button bOctUp = {13, 'u', "", 14};

struct Button noteButtonList[13] = {bC3, bD3, bE3, bF3, bG3, bC3V, bD3V, bE3V, bF3V, bG3V, bSw, bOctDwn, bOctUp};

/////=====Global_Variables=====/////
unsigned long time1;               
unsigned long interval;              // Time interval between the two button presses.
int velocity_setting = 0;            // 0 is velocity sensing off, 1 is on.
int mono_poly = 0;                   // 0 is Mono, 1 is Poly.
unsigned long debounceDelay = 50;    // The debounce time;
int channel = 1;

/////=====Setup=====/////
void setup() {
  //Set MIDI baud rate:
  Serial.begin(31250);
  // Set Serial Rate
  //Serial.begin(9600);
  for (int i; i <  sizeof(noteButtonList); i++){
     pinMode(noteButtonList[i].pin, INPUT_PULLUP);
  }
}

/////-----Functions-----/////
void noteOn(int cmd, int pitch, int velocity) {
  Serial.write(cmd);
  Serial.write(pitch);
  Serial.write(velocity);
}

int getvelfromint(int interval){
  int return_vel;

  if (interval <= 20){
    return_vel = 127;
  } else if (interval <= 50 && interval > 20){
    return_vel = 100;
  } else if (interval <= 70 && interval > 50){
    return_vel = 75;
  } else if (interval <= 90 && interval > 70){
    return_vel = 50;
  } else if (interval <= 110 && interval > 90){
    return_vel = 35;
  } else {
    return_vel = 20;
  }
  return return_vel;
}

void button_read(Button &but){

  int reading = digitalRead(but.pin);

  if (reading != but.lastButtonState && but.type != 'v') {
      but.lastDebounceTime = millis();
    }
    
  if ((millis() - but.lastDebounceTime) > debounceDelay) {
    if (reading != but.buttonState) {
      but.buttonState = reading;
      if (but.buttonState == LOW) {   //if button IS pushed
        while(but.buttonState == HIGH) but.buttonState = digitalRead(but.pin);
        if (but.type == 'n') {
          time1 = millis();
          if (velocity_setting == 0){
            noteOn(0x90, but.note, 0x45);
          }
        } else if (but.type == 'v' && velocity_setting != 0 ) {
          interval = millis() - time1;
          int vel = getvelfromint(interval);
          noteOn(0x90, but.note, vel);
        } else if (but.type == 'u'){
          for (int i = 0; i <  10; i++){
            noteButtonList[i].note += 12;
          }
        } else if (but.type == 'd'){
          for (int i = 0; i <  10; i++){
            noteButtonList[i].note -= 12;
          }
        } else if (but.type == 'w'){
          if (velocity_setting == 1){
            velocity_setting = 0;
          } else {
            velocity_setting = 1;
          }
        } 
      } else if (but.buttonState == HIGH && but.type == 'n' || (but.type == 'v' && velocity_setting != 0)) {
        noteOn(0x90, but.note, 0x00);
      }
    }
  }
  but.lastButtonState = reading;
}

/////-----Main Loop-----/////
void loop() {
  for (int i = 0; i <  13; i++){
      button_read(noteButtonList[i]);
  }
}
