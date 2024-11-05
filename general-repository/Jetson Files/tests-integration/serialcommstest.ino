//#include <SoftwareSerial.h>   //Custom serial port configuration 
/*
* Serial on pins 0 (RX) and 1 (TX) (also USB)
* Serial1 on pins 19 (RX) and 18 (TX)
* Serial2 on pins 17 (RX) and 16 (TX)
* Serial3 on pins 15 (RX) and 14 (TX)
*
* *Note about the multiple Serial ports; each Serial port has a recieve (RX) and transmit(TX) that connects to another device's
* transmit (TX) and recieve (RX). Simultaneous read and write on one Serial port should be possible, and extra Serial ports are
* used for extra devices communicating through Serial.
*
*
* Data type sizes:
*  byte = 8 bits, 1 byte
*    > u_byte = (0 - 255)
*    > byte = (-128 - 127)
*  int = 16 bits, 2 bytes
*    > u_int = (0 - 65535)
*    > int = (-32768 - 32767)
*  long = 32 bits, 4 bytes
*    > u_long = (0 - 4294967295)
*    > long = (-2147483648 - 2147483647)
*
*
* Explicit LEDs on the Arduino Mega 2560:
*  RX light blink : Recieving data
*  TX light blink : Transmitting data
*/
unsigned long BAUD_RATE = 250000;

void setup() {
  Serial.begin(BAUD_RATE);     //USB Port output
  Serial1.begin(BAUD_RATE);    //Pins 19/18 RX/TX input
  //while(!Serial) { ; }    //Required for native USB port only (such as Arduino Zero)
}

void loop() {
  //Message through Serial Monitor
  if(Serial.available()){
    Serial1.println(Serial.readStringUntil(10));
  }

  //Comms through 19/18 RX/TX ports
  if(Serial1.available()){   //Check if at least one character is available

    /* Basic Reading */
    //char ch = Serial.read();  //Reads one character at a time
    //String str = Serial.readString();   //Reads string, terminating after newline
    //String str = Serial.readStringUntil('b');  //Reads string, terminating before the character given by parameter (does not include the character) or after newline
    String str = Serial1.readStringUntil(10);   //Terminate before the newline (10 = ASCII linefeed)
    Serial.println("Serial found: " + str);

    Serial.flush();     //Wait explicitly until anything sent through Serial is completely sent, rather than waiting to be sent in the Serial buffer.
                        //Otherwise, the program would move on to the next statement, regardless if there was anything in the buffer.


    /* Parsing and Analyzing */
    //Parsing packets of info with comma separation.
    //Note: input is something like "hello,world,how,are,you" or "hello, world, how, are, you"
    String parse;
    while(str.length() > 0){
      parse = str.substring(0, str.indexOf(','));
      str = str.substring(parse.length() + 1);
      parse.trim();             //Cleans up leading and following whitespace
      Serial.println(parse);
    }

    //Evaluating commands
    // str.trim();
    // if(str.equals("how are you")){
    //   Serial.println("im fine thank you");
    // }


    /* Testing */
    

  }
  
}
