#include <stdio.h>

class Button {          //class for defining button pins with debounce built in
    const byte pin;
    int reading;
    unsigned long buttonDownMS;
    int DebounceDelay;
  public:
    int state;
    Button(byte attachTo) :
      pin(attachTo)
    {
    }


    void setup() {
      pinMode(pin, INPUT);
      state = LOW;
      reading = LOW;
      buttonDownMS = millis();
      DebounceDelay = 50;
    }

    void loop() {
      
      int prevReading = reading;
      reading = digitalRead(pin);
      
      if (prevReading != reading) {
        if(millis()- buttonDownMS < DebounceDelay){
          state=reading;}
        buttonDownMS = millis();
      }
      else if (prevReading == reading) {
        if (millis() - buttonDownMS < DebounceDelay) {
          if(reading != state){
            state = reading;
            buttonDownMS = millis();
          }
        }
      }
    }
};

//define pins
Button charge_button(4);
Button discharge_button(2);
int charge_signal_pin = 7;
int discharge1_signal_pin = 8;
int discharge2_signal_pin = 12;
unsigned int potentiometer = A0;

//potentiometer value
int pot = 10;
//other constants
unsigned long discharge_start_us = 0;
unsigned long discharge_time1;  //when discharge is engaged, this value is calculated to be dt1c*pot
unsigned long dt1c = 500;     //discharge_time1 constant
unsigned long discharge_time2;  //when discharge is engaged, this value is calculated to be dt2c*pot
unsigned long dt2c = 500;     //discharge_time2 constant
unsigned long discharge_delay;  //amount of time between coil 1 and coil 2 turn on
unsigned long dd = 500;       //


//state variables
int charge_signal;
int previous_charge_button_state;
int prev_discharge_state;
int discharge1_signal;
int discharge2_signal;
bool discharge_engaged;

int i = 0;   //for sprintf buffer

void setup() {
  Serial.begin(9600); //temporary serial input for debugging;
  charge_button.setup();
  discharge_button.setup();
  pinMode(potentiometer, INPUT);
  pinMode(charge_signal, OUTPUT);
  pinMode(discharge1_signal_pin, OUTPUT);
  pinMode(discharge2_signal_pin, OUTPUT);
  charge_signal = LOW;
  previous_charge_button_state = LOW;
  prev_discharge_state=LOW;
  discharge1_signal = LOW;
  discharge2_signal = LOW;
  discharge_engaged = false;
  digitalWrite(charge_signal_pin, charge_signal);
  digitalWrite(discharge1_signal_pin, LOW);
  digitalWrite(discharge2_signal_pin, LOW);
}

void loop() {

  //button loops read and debounce current button state
  charge_button.loop();
  discharge_button.loop();

  //modify charge_signal based on charge_button (charge button acts as flip flop)
  if(charge_button.state == HIGH && previous_charge_button_state == LOW){
    charge_signal = !charge_signal;
  }
  previous_charge_button_state = charge_button.state;

  //Check if discharge button is pushed to engage discharge sequence
  if(discharge_button.state == HIGH && prev_discharge_state == LOW && !discharge_engaged){
    discharge_engaged = true;
    pot = analogRead(potentiometer);
    discharge_start_us = micros();
    discharge_time1 = pot*dt1c;
    discharge_time2 = pot*dt2c;
    discharge_delay = pot*dd;
    char buffer [128];
    i=sprintf (buffer, "discharge start: %lu\npotentiometer: %d\ndischarge time 1: %lu\ndischarge time 2: %lu\ndischarge delay: %lu\n",discharge_start_us,pot,discharge_time1,discharge_time2,discharge_delay);
    for(int l= 0; l<=i; l++)
    Serial.print(buffer[l]);
  }
  prev_discharge_state = discharge_button.state;

  //Discharge sequence [set discharge signals in sequence]
  if (discharge_engaged){
    unsigned long current_time = micros();
    Serial.print(current_time-discharge_start_us);

    
   
    if((current_time-discharge_start_us) < discharge_time1){  //if coil 1 discharge time has not passed since discharge start
      discharge1_signal = HIGH;                           //discharge coil 1
      /*char buffer [50];
      i=sprintf (buffer, "current time: %lu  ON 1\n",current_time);
      for(int l= 0; l<=i; l++)
      Serial.print(buffer[l]);*/
      Serial.println("   Box 1");
    }
    else if(current_time-discharge_start_us > discharge_time1){      //if coil 1 discharge time has expired
      discharge1_signal = LOW;                              //turn off coil 1 discharge
     /*  char buffer [50];
      i=sprintf (buffer, "current time: %lu  OFF 1\n",current_time);
      for(int l= 0; l<=i; l++)
      Serial.print(buffer[l]);*/
      Serial.println("   Box 2");
    }
    if(discharge_delay < (current_time - (discharge_start_us + discharge_time1)) && (current_time - (discharge_start_us + discharge_time1 + discharge_delay)) < discharge_time2){
      discharge2_signal = HIGH;                             //turn on coil 2 discharge
      /*char buffer [50];
      i=sprintf (buffer, "current time: %lu  ON 2\n",current_time);
      for(int l= 0; l<=i; l++)
      Serial.print(buffer[l]);*/
      Serial.println("   Box 3");
    }
    else if((current_time-discharge_start_us) >= (discharge_delay + discharge_time1 + discharge_time2)){  //discharge time expired for all coils
      discharge1_signal = LOW;  //turn off coil 1
      discharge2_signal = LOW;  //turn off coil 2
      discharge_engaged = false; //disengage discharge
      Serial.println("   Box 4");
      /*char buffer [50];
      i=sprintf (buffer, "current time: %lu  OFF 2\n",current_time);
      for(int l= 0; l<=i; l++)
      Serial.print(buffer[l]);*/
    }
  }
  
  //This section writes the signal states out to their respective signal pins
  digitalWrite(charge_signal_pin, charge_signal);
  digitalWrite(discharge1_signal_pin, discharge1_signal);
  digitalWrite(discharge2_signal_pin, discharge2_signal);
  
}


void capacitor_voltage_monitor(){
  
}
