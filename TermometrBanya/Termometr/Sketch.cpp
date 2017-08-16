/*Begining of Auto generated code by Atmel studio */
#include <Arduino.h>

/*End of auto generated code by Atmel studio */

#include <TimerOne.h>
#include <Wire.h>
#include <MultiFuncShield.h>
#include <OneWire.h>
#include <DallasTemperature.h>

//Output lead: red (VCC), yellow (DATA), blue (GND)
#define ONE_WIRE_BUS 5                // Data wire is plugged into port 2 on the Arduino
#define TEMPERATURE_PRECISION 12
OneWire oneWire(ONE_WIRE_BUS);        // Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);  // Pass our oneWire reference to Dallas Temperature.
DeviceAddress saunaThermometer =  { 0x28, 0xDB, 0xF5, 0xA0, 0x8, 0x0, 0x0, 0xDA }, //28DBF5A0080000DA
			  insideThermometer = { 0x28, 0x45, 0xC7, 0xA3, 0x8, 0x0, 0x0, 0xF7 }, //2845C7A3080000F7
              outsideThermometer = { 0x28, 0x9A, 0xC9, 0xA2, 0x8, 0x0, 0x0, 0x3A }; //289AC9A20800003A

char lock80, lock100;

// function to print a temperature to LED
float printTemperatureLED(DeviceAddress deviceAddress)
{
	float tempC = sensors.getTempC(deviceAddress);
	if (deviceAddress == saunaThermometer) 	        MFS.write("Saun");
	else if (deviceAddress == insideThermometer) 	MFS.write("Beer");
	else if (deviceAddress == outsideThermometer) 	MFS.write("AIR");
	delay(1000);
	MFS.write(tempC, 1);  // display to 2 decimal places.
	delay(3000);
	return tempC;
}

/*
Alarm when temp reaches 80 and 100 degrees
lock realizes when temp coll down to 75 and 95 degrees
*/
void alarm_80_100 (float Temperature)
{
	if (Temperature < 95.0) lock100 = false;
	if (Temperature < 75.0) lock80 = false;
	
	if (!lock80){
		if ((Temperature >= 80.0) && (Temperature < 100.0)) {
			lock80 = true;
			MFS.beep(5,    // beep for 50 milliseconds
			5,    // silent for 50 milliseconds
			4,    // repeat above cycle 4 times
			3,    // loop 3 times
			50    // wait 500 milliseconds between loop
			);
		}
	}
	
	if (!lock100){
		if (Temperature >= 100.0) {
			lock100 = true;
			MFS.beep(5,    // beep for 50 milliseconds
			5,    // silent for 50 milliseconds
			4,    // repeat above cycle 4 times
			3,    // loop 3 times
			50    // wait 500 milliseconds between loop
			);			
		}
		
	}
	

}


// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
	for (uint8_t i = 0; i < 8; i++)
	{
		// zero pad the address if necessary
		if (deviceAddress[i] < 16) Serial.print("0");
		Serial.print(deviceAddress[i], HEX);
	}
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
	float tempC = sensors.getTempC(deviceAddress);
	Serial.print("Temp C: ");
	Serial.print(tempC);
	Serial.print(" Temp F: ");
	Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
	Serial.print("Resolution: ");
	Serial.print(sensors.getResolution(deviceAddress));
	Serial.println();
}

// main function to print information about a device
void printDataSerial(DeviceAddress deviceAddress)
{
	Serial.print("Device Address: ");
	printAddress(deviceAddress);
	Serial.print(" ");
	printTemperature(deviceAddress);
	Serial.println();
}


void setup() {
	  lock80 = false;
	  lock100 = false;
	
	  Timer1.initialize();
	  MFS.initialize(&Timer1);    // initialize multi-function shield library
	  MFS.beep();                 // short beep for 200 milliseconds
	  MFS.write("Hi");
	  delay(1000);
	  
	 Serial.begin(9600); // start serial port
	 
	 Serial.println("Dallas Temperature IC Control Library Demo");
	 sensors.begin(); // Start up the library
	 Serial.print("Locating devices..."); // locate devices on the bus
	 Serial.print("Found ");
	 Serial.print(sensors.getDeviceCount(), DEC);
	 Serial.println(" devices.");
	 Serial.print("Parasite power is: "); // report parasite power requirements
	 if (sensors.isParasitePowerMode()) Serial.println("ON");
	 else Serial.println("OFF");

	 // Search for devices on the bus and assign based on an index. Ideally,
	 // you would do this to initially discover addresses on the bus and then
	 // use those addresses and manually assign them (see above) once you know
	 // the devices on your bus (and assuming they don't change).
	 //
	 // method 1: by index
	 //if (!sensors.getAddress(saunaThermometer, 0)) Serial.println("Unable to find address for Device 0");
	 //if (!sensors.getAddress(insideThermometer, 1)) Serial.println("Unable to find address for Device 1");
	 //if (!sensors.getAddress(outsideThermometer, 2)) Serial.println("Unable to find address for Device 2");
	 
	 Serial.print("Device 0 Address: "); // show the addresses we found on the bus
	 printAddress(saunaThermometer);
	 Serial.println();	 
	 Serial.print("Device 1 Address: ");
	 printAddress(insideThermometer);
	 Serial.println();
	 Serial.print("Device 2 Address: ");
	 printAddress(outsideThermometer);
	 Serial.println();

	 // set the resolution to 9 bit per device
	 sensors.setResolution(saunaThermometer, TEMPERATURE_PRECISION);	 
	 sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
	 sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
	 Serial.print("Device 0 Resolution: ");
	 Serial.print(sensors.getResolution(saunaThermometer), DEC);
	 Serial.println();
	 Serial.print("Device 1 Resolution: ");
	 Serial.print(sensors.getResolution(insideThermometer), DEC);
	 Serial.println();
	 Serial.print("Device 2 Resolution: ");
	 Serial.print(sensors.getResolution(outsideThermometer), DEC);
	 Serial.println();  
}




void loop() {

	 Serial.print("Requesting temperatures...");
	 sensors.requestTemperatures();
	 Serial.println("DONE");
	 printDataSerial(saunaThermometer);	 // print the device information to UART
	 printDataSerial(insideThermometer);
	 printDataSerial(outsideThermometer);
	 
	 alarm_80_100(printTemperatureLED(saunaThermometer));
	 printTemperatureLED(insideThermometer);
	 printTemperatureLED(outsideThermometer);
	 
}
