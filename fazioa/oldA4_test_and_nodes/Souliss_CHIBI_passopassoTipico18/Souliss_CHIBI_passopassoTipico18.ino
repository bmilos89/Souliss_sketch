/**************************************************************************
 * 	Souliss - Fiat Lux
 * 	
 * 	Fiat Lux is a latin phrase that means Let There Be Light, this example
 * 	show how handle light via hardwired pushbutton geographically distributed
 * 	over the home, the Android (or any other direct) user interface can also
 * 	be used.
 * 	
 * 	It require an Ethernet board based on Wiznet W5100 or Microchip ENC28J60.
 * 	
 * 	Applicable for : 
 * 			- Lights controller via relay,
 * 			- Plugs and other ON/OFF devices.
 * 	
 * 	The first device has
 * 		- A coil (relay or other) on Pin 8 and Pin 9
 * 		- A couple of inputs on Pin 2 and 3, pulldown required
 * 		
 * 	The Remote Device has
 * 		- A couple of inputs on Pin 2 and 3, pulldown required
 * 
 * 	CONFIGURATION IS MANDATORY BEFORE COMPILING
 * 	Before compiling this code, is mandatory the configuration of the framework
 * 	this ensure the use of proper drivers based functionalities and requested
 * 	communication interface.	
 * 	
 * 	Configuration files are located on /conf folder, is suggested to use this 
 * 	code on one of the boards listed below, the code can also compile on other
 * 	boards but may require modification on I/O definitions.	
 * 	
 * 	Run this code on one of the following boards:
 * 	
 * 		Board Conf Code			Board Model
 * 0x03        			Arduino Ethernet (W5100) 
 * 		0x04					Arduino with Ethernet Shield (W5100)
 * 		0x05					Arduino with ENC28J60 Ethernet Shield	
 * 	
 * 	******************** Configuration Parameters *********************
 * 	
 * 		Configuration file		Parameter
 * 		QuickCfg.h				#define	QC_ENABLE			0x01
 * 		QuickCfg.h				#define	QC_BOARDTYPE		0x03, 0x04, 0x05
 * 
 * 	Is required an additional IP configuration using the following parameters
 * 		QuickCfg.h				const uint8_t DEFAULT_BASEIPADDRESS[] = {...}
 * 		QuickCfg.h				const uint8_t DEFAULT_SUBMASK[]       = {...}
 * 		QuickCfg.h				const uint8_t DEFAULT_GATEWAY[]       = {...}
 * 		
 ***************************************************************************/
#include "Souliss.h"
#include "Typicals.h"
#include <SPI.h>

#define network_address_1	0x0011
#define network_chibi_address_1 0x6511
#define network_chibi_address_2	0x6512


#define network_my_subnet	0xFF00
#define network_my_supern	0x0011

#define STEPRELAY 2 // This is the memory slot for the logic that handle the light

// define the shared memory map
U8 memory_map[MaCaco_MEMMAP] = { 0x00};

// flag 
U8 data_changed = 0;

#define time_base_fast		10				// Time cycle in milliseconds
#define time_base_slow		10000			// Time cycle in milliseconds
#define num_phases			255				// Number of phases

U8 phase_speedy=0, phase_fast=0, phase_slow=0;
unsigned long tmr_fast=0, tmr_slow=0;  

void setup()
{
   Serial.begin(9600);	
  Souliss_SetAddress(network_chibi_address_2, network_my_subnet, network_my_supern);		
  Souliss_SetT18(memory_map, STEPRELAY);
  // Define inputs, outputs pins
  pinMode(4, INPUT);	// Hardware pulldown required
  pinMode(6, OUTPUT);	
}


void loop()
{ 

  if(abs(millis()-tmr_fast) > time_base_fast)
  {	
    tmr_fast = millis();
    phase_fast = (phase_fast + 1) % num_phases;

    // Execute the code every 3 time_base_fast		
    if (!(phase_fast % 3))
    {
      Souliss_DigIn2State(4, Souliss_T1n_OnFeedback, Souliss_T1n_OffFeedback, memory_map, STEPRELAY);
      Souliss_Logic_T18(memory_map, STEPRELAY, &data_changed);
      Souliss_DigOut(6, Souliss_T1n_PulseCoil, memory_map, STEPRELAY);	
    } 

    // Execute the code every 5 time_base_fast		  
    if (!(phase_fast % 5))
    {   
      // Retreive data from the communication channel
      Souliss_CommunicationData(memory_map, &data_changed);		
    }	
    else if(abs(millis()-tmr_slow) > time_base_slow)
    {	
      tmr_slow = millis();
      phase_slow = (phase_slow + 1) % num_phases;

      // Execute the code every time_base_slow
      if (!(phase_slow % 1))
      {                 
        // The timer handle timed-on states

        Souliss_T18_Timer(memory_map, STEPRELAY);						
      } 	  
    }	
  }	
} 




