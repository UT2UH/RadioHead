// stm32l0ra_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing client
// with the RH_STM32L0RA class. RH_STM32L0RA class does not provide for addressing or
// reliability, so you should only use RH_STM32L0RA directly if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example stm32l0ra_server
// Tested with Tested with EcoNode SmartTrap, Arduino 1.8.9, GrumpyOldPizza Arduino Core for STM32L0.

#include <SPI.h>
#include <RH_STM32L0RA.h>

// Singleton instance of the radio driver
RH_STM32L0RA stm32l0ra;

// Valid for SmartTrap, maybe not other boards

#define GREEN_LED 13
#define YELLOW_LED 12
#define RED_LED 11


void setup() 
{
  pinMode(GREEN_LED, OUTPUT);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(RED_LED, OUTPUT);
  
  Serial.begin(9600);
  // Wait for serial port to be available 
  // If you do this, it will block here until a USB serial connection is made.
  // If not, it will continue without a Serial connection, but DFU mode will not be available
  // to the host without resetting the CPU with the Boot button
//  while (!Serial) ; 

  // You must be sure that the TCXO settings are appropriate for your board and radio.
  // See the RH_STM32L0RA documentation for more information.
  // This call is adequate for Tlera boards supported by the Grumpy Old Pizza Arduino Core
  // It may or may not be innocuous for others
  SX1276SetBoardTcxo(true);
  delay(1);

  if (!stm32l0ra.init())
    Serial.println("init failed");
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  stm32l0ra.setFrequency(868.0);

  // You can change the modulation speed etc from the default
  //stm32l0ra.setModemConfig(RH_RF95::Bw125Cr45Sf128);
  //stm32l0ra.setModemConfig(RH_RF95::Bw125Cr45Sf2048);
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // You can set transmitter powers from 2 to 20 dBm:
  //stm32l0ra.setTxPower(20); // Max power
}

void loop()
{
  digitalWrite(YELLOW_LED, 1);
  digitalWrite(GREEN_LED, 0);
  digitalWrite(RED_LED, 0);
  
  Serial.println("Sending to stm32l0ra_server");
  // Send a message to stm32l0ra_server
  uint8_t data[] = "Hello World!";
  stm32l0ra.send(data, sizeof(data));
  stm32l0ra.waitPacketSent();
  
  // Now wait for a reply
  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);

// You might need a longer timeout for slow modulatiuon schemes and/or long messages
  if (stm32l0ra.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (stm32l0ra.recv(buf, &len))
   {
      Serial.print("got reply: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(stm32l0ra.lastRssi(), DEC);    
      digitalWrite(GREEN_LED, 1);

    }
    else
    {
      Serial.println("recv failed");  
      digitalWrite(RED_LED, 1);

    }
  }
  else
  {
    Serial.println("No reply, is stm32l0ra_server running?");
      digitalWrite(RED_LED, 1);
  }
  digitalWrite(YELLOW_LED, 0);

  delay(400);
}
