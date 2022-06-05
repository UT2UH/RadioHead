// lgt92_server.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple messageing server
// with the RH_L0RA class. RH_L0RA class does not provide for addressing or
// reliability, so you should only use RH_L0RA directly if you do not need the higher
// level messaging abilities.
// It is designed to work with the other example lgt92_client_xx
// Tested with Dragino LGT92 Tracker, Arduino 1.8.13, GrumpyOldPizza Arduino Core for STM32L0.

#include <SPI.h>
#include <RH_L0RA.h>

// Singleton instance of the radio driver
RH_L0RA rfm95;


void setup() 
{  
  pinMode(PIN_LED_GRN, OUTPUT);
  pinMode(PIN_LED_BLU, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);

  Serial.begin(9600);
  // Wait for serial port to be available 
  // If you do this, it will block here until a USB serial connection is made.
  // If not, it will continue without a Serial connection, but DFU mode will not be available
  // to the host without resetting the CPU with the Boot button
//  while (!Serial) ; 

  if (!rfm95.init())
    Serial.println("init failed");  
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on

  // On the K33 board, the radio TCXO is powered by MCU output PH1, so you have
  // to enable the power to the TCXO before telling the radio to use it
  SX1276SetBoardTcxo(true);

  rfm95.setFrequency(868.0);

  // You can change the moduation speed etc from the default
//  rfm95.setModemConfig(RH_RF95::Bw125Cr45Sf128);
  //rfm95.setModemConfig(RH_RF95::Bw125Cr45Sf2048);

  // The default transmitter power is 13dBm, using PA_BOOST.
  // You can set transmitter powers from 2 to 20 dBm:
  //rfm95.setTxPower(20); // Max power

}

void loop()
{
  digitalWrite(PIN_LED_BLU, 1);
  digitalWrite(PIN_LED_GRN, 0);
  digitalWrite(PIN_LED_RED, 0);
  
  if (rfm95.available())
  {
    // Should be a message for us now   
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rfm95.recv(buf, &len))
    {
       digitalWrite(PIN_LED_GRN, 1);

//      RH_L0RA::printBuffer("request: ", buf, len);
      Serial.print("got request: ");
      Serial.println((char*)buf);
//      Serial.print("RSSI: ");
//      Serial.println(rfm95.lastRssi(), DEC);

      // Send a reply
      uint8_t data[] = "And hello back to you";
      rfm95.send(data, sizeof(data));
      rfm95.waitPacketSent();
      Serial.println("Sent a reply");
      digitalWrite(PIN_LED_GRN, 0);
    }
    else
    {
      Serial.println("recv failed");
    }
  }
}
