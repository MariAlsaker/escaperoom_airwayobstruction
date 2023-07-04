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
}

void loop() { //This runs all the time at SUPER HIGH speed

  // put your main code here, to run repeatedly:
  
  /* This is a delta capture subroutine, which allows the processor to wait without disrupting other tasks */
  /* We want the IR LED to flash rapidly, tested this duy cycle of 25% and found that the sensor to register the signal continuously.

                     60 ms                      20 ms
    LED ON ______            ______            ______            ______
                 |          |      |          |      |          |      
                 |          |      |          |      |          |      
    LED OFF      ------------      ------------      ------------      
  */
  if(io_state==0 &&  millis() - output_timestamp > 60) { //Let the sensor rest for 60 milli sec
    output_timestamp = millis(); //Now that the subroutine is activated this is the new relative position we wait from
    io_state = 1; //Negate value, turn 0 into 1 and 1 into 0
    digitalWrite(LED, io_state); //Write the negated value to pin 3
  }
  else if (io_state==1 && millis() - output_timestamp > 20) { //Give the sensor a signal to detect for 20 milli sec
    output_timestamp = millis();
    io_state = 0;
    digitalWrite(LED, io_state);
  }

  /* 
    This measures the same signal coming back through the IR receiver (With noise)
     We measure every 10 ms and count when it switches from LED OFF to LED ON
  */
  if(millis() - sampling_timestamp > 10){ //Every 10 ms
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

    if(pulse_count < 2){ //If we saw less than 2 pulses the last 500 ms
      digitalWrite(OUT_PIN, HIGH); //It's probably blocked
    }else{ //If not
      digitalWrite(OUT_PIN, LOW); //It's probably not blocked
    }
    pulse_count = 0; //Now we have captured for 500 ms, reset count for the next time
  }
}