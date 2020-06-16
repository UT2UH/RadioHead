// abz_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_ABZ class. RH_ABZ class does not provide for addressing or
// reliability, so you should only use RH_ABZ directly if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example abz_server
// Tested with Tested with EcoNode SmartTrap, Arduino 1.8.9, GrumpyOldPizza Arduino Core for STM32L0.

#include <SPI.h>
#include <RH_ABZ.h>
#include <RHEncryptedDriver.h>
#include <Speck.h>
extern "C" void SX1276SetBoardTcxo( bool state );

// Singleton instance of the radio driver
RH_ABZ abz;
Speck myCipher;   // Instanciate a Speck block ciphering
RHEncryptedDriver myDriver(abz, myCipher); // Instantiate the driver with those two

float frequency = 868.0; // Change the frequency here. 
unsigned char encryptkey[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; // The very secret key

void setup() 
{  
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_BLU, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  Serial.begin(9600);
  while (!Serial) ;
  Serial.println("ABZ Encrypted Server"); 

  if (!abz.init())
    Serial.println("init failed");  
  
  // On the K33 board, the radio TCXO is powered by MCU output PH1, so you have
  // to enable the power to the TCXO before telling the radio to use it
  SX1276SetBoardTcxo(true);

  abz.setFrequency(frequency);

  // You can change the moduation speed etc from the default
//  abz.setModemConfig(RH_RF95::Bw125Cr45Sf128);
  //abz.setModemConfig(RH_RF95::Bw125Cr45Sf2048);

  // The default transmitter power is 13dBm, using PA_BOOST.
  // You can set transmitter powers from 2 to 20 dBm:
  abz.setTxPower(3); // Max power

  myCipher.setKey(encryptkey, sizeof(encryptkey));
}

void loop()
{
  digitalWrite(PIN_LED_BLU, 1);
  digitalWrite(PIN_LED_GRN, 0);
  digitalWrite(PIN_LED_RED, 0);
  
  if (abz.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (myDriver.recv(buf, &len))
    {
       digitalWrite(PIN_LED_GRN, 1);

//      RH_ABZ::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(abz.lastRssi(), DEC);

      // Send a reply
      uint8_t data[] = "And hello back to you";
      myDriver.send(data, sizeof(data));
      abz.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(PIN_LED_GRN, 0);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
