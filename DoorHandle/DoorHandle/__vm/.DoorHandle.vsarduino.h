/* 
	Editor: https://www.visualmicro.com/
			visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
			the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
			all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
			note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: Generic ESP8266 Module, Platform=esp8266, Package=esp8266
*/

#define __ESP8266_ESp8266__
#define __ESP8266_ESP8266__
#define ARDUINO 10805
#define ARDUINO_MAIN
#define printf iprintf
#define __ESP8266__
#define __ets__
#define ICACHE_FLASH
#define F_CPU {build.f_cpu}
#define LWIP_OPEN_SRC
#define ARDUINO 10805
#define ARDUINO_ESP8266_GENERIC
#define ARDUINO_ARCH_ESP8266
//
//
void handleLogin();

#include "common.h" 
#include "pins_arduino.h" 
#include "arduino.h"
#include "DoorHandle.ino"
