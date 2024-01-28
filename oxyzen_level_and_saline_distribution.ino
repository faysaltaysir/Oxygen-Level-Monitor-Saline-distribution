#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

const int pumpPin = 12;  // Pin connected to the pump control
const int tankPin = A0; // Pin connected to the tank level sensor

PulseOximeter pox;



volatile int flow_frequency; // Measures flow sensor pulses
float l_hour; // Calculated litres/hour
unsigned char flowsensor = 2; // Sensor Input
unsigned long currentTime;
unsigned long cloopTime;



void flow () // Interrupt function
{
   flow_frequency++;
}


void setup()
{
   Serial.begin(9600);
   pinMode(pumpPin, OUTPUT);
   pinMode(flowsensor, INPUT);
   digitalWrite(flowsensor, HIGH); // Optional Internal Pull-Up
   attachInterrupt(0, flow, RISING); // Setup Interrupt
   sei(); // Enable interrupts
   currentTime = millis();
   cloopTime = currentTime;

    // Initialize the MAX30100 sensor
    if (!pox.begin())
    {
        Serial.println("MAX30100 initialization failed. Please check your wiring and restart the Arduino.");
        while (1);
    }
    else
    {
        Serial.println("MAX30100 initialization successful.");
    }
}

void loop()
{
    // Update MAX30100 sensor data
    pox.update();

    // Get the current oxygen level
    int simulatedOxygenLevel = pox.getSpO2();

    // Calculate saline quantity based on simulated oxygen level
    int salineQuantity = mapOxygenToSaline(simulatedOxygenLevel);

    // Control pump based on simulated oxygen level
    deliverSaline(salineQuantity);

    // Wait for a moment to simulate real-time readings
    delay(5000);
}

int mapOxygenToSaline(int oxygenLevel)
{
    // Map simulated oxygen level to saline quantity based on the provided ranges
    if (oxygenLevel >= 90 && oxygenLevel <= 94)
    {
        return random(500, 1001);
    }
    else if (oxygenLevel >= 85 && oxygenLevel <= 89)
    {
        return random(1001, 2001);
    }
    else if (oxygenLevel >= 80 && oxygenLevel <= 84)
    {
        return random(2001, 3001);
    }
    else if (oxygenLevel >= 75 && oxygenLevel <= 79)
    {
        return random(3001, 4001);
    }
    else if (oxygenLevel < 75)
    {
        return random(4001, 5001);
    }

    // Default value if oxygen level is out of specified ranges
    return 0;
}

void deliverSaline(int quantity)
{
    digitalWrite(pumpPin, HIGH); // Start the pump
    Serial.print("Delivering ");
    Serial.print(quantity);
    Serial.println(" milliliters of Saline...");

    currentTime = millis();
   // Every second, calculate and print litres/hour
   //float x = 1.0; 
   
   if(currentTime >= (cloopTime + 1000))
   {
      cloopTime = currentTime; // Updates cloopTime
      // Pulse frequency (Hz) = 7.5Q, Q is flow rate in L/min.
      l_hour = ((flow_frequency *1000) / (7.5*60)); // (Pulse frequency x 60 min) / 7.5Q = flowrate in L/hour
      //flow_frequency = 0; // Reset Counter
      
      if(l_hour >= quantity){
        digitalWrite(pumpPin,LOW);delay(5000);
      }
      else if(l_hour < quantity){
        digitalWrite(pumpPin,HIGH);
      }
      
      Serial.print(l_hour, DEC); // Print litres/hour
      Serial.println(" L/sec");

    // Simulate delivering the correct amount of saline (simplified example)
    delay(quantity * 100);

    digitalWrite(pumpPin, LOW); // Stop the pump
    Serial.println("Saline Delivered!");
  }
}
