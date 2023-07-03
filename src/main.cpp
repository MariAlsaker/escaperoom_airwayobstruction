#include <Arduino.h>

uint32_t output_timestamp = 0;
uint32_t sampling_timestamp = 0;
uint32_t capture_timestamp = 0;
uint8_t io_state = 0;

uint32_t pulse_count = 0;
uint8_t previous_read = 0;
boolean count_open_time = 0;

int SENSOR_PIN = 3; 
int LED = 2; 
int OUT_PIN = 13;

void setup() {
  // put your setup code here, to run once:
  pinMode(OUT_PIN, OUTPUT); // Output to mother board - HIGH if blocked, LOW if not
  pinMode(SENSOR_PIN, INPUT); // Reading IR sensor
  pinMode(LED, OUTPUT); // Making LED blink

  output_timestamp = millis();
  sampling_timestamp = millis();
  capture_timestamp = millis();
  Serial.begin(9600);
}

void loop() { //This runs all the time at SUPER HIGH speed

  // put your main code here, to run repeatedly:
  
  /* This is a delta capture subroutine, which allows the processor to wait without disrupting other tasks */
  /* Every 20 ms we want to flip the output of the IR LED for it to flash rapidly 

                 20 ms             20 ms
    LED ON ______      ______      ______      ______      
                 |    |      |    |      |    |      |    |
                 |    |      |    |      |    |      |    |
    LED OFF      ------      ------      ------      ------
  */
  if(millis() - output_timestamp > 20){ //Every 20 ms
    output_timestamp = millis(); //Now that the subroutine is activated this is the new relative position we wait from
    io_state = !io_state; //Negate value, turn 0 into 1 and 1 into 0
    digitalWrite(LED, io_state); //Write the negated value to pin 3
  }

  /* 
    This measures the same signal coming back through the IR receiver (With noise)
     We measure every 1 ms and count when it switches from LED OFF to LED ON
  */
  if(millis() - sampling_timestamp > 10){ //Every 1 ms
    sampling_timestamp = millis();
    if(!digitalRead(SENSOR_PIN) && (previous_read == HIGH)){ //If we see HIGH signal and last time we saw LOW
      previous_read = LOW; //Then we remember that we saw HIGH
      pulse_count++; //We counted one pulse
    }else if(digitalRead(SENSOR_PIN) && (previous_read == LOW)){ //If we see LOW signal and last time we saw HIGH
      previous_read = HIGH; //Then we remember that we saw LOW
    }
  }

  if(millis() - capture_timestamp > 500){ //Every 500 ms
    capture_timestamp = millis();

    Serial.println(pulse_count); //We report the pulses we saw
    if(pulse_count < 2){ //If we saw less than 2 pulses the last 500 ms
      digitalWrite(OUT_PIN, HIGH); //It's probably blocked
    }else{ //If not
      digitalWrite(OUT_PIN, LOW); //It's probably not blocked
    }
    pulse_count = 0; //Now we have captured for 500 ms, reset count for the next time
  }
}