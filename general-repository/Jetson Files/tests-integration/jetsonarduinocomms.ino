/*
* Serial on pins 0 (RX) and 1 (TX) (also USB)
* Serial1 on pins 19 (RX) and 18 (TX)
* Serial2 on pins 17 (RX) and 16 (TX)
* Serial3 on pins 15 (RX) and 14 (TX)
*
*/

int currentPosX = 0, currentPosY = 0;
unsigned long BAUD_RATE = 250000;

void setup() {
  Serial.begin(BAUD_RATE);     //USB Port output
  Serial1.begin(BAUD_RATE);    //Pins 19/18 RX/TX input
}

void loop() {
  //Message through Serial Monitor
  if(Serial.available()){
    //Serial1.println(Serial.readStringUntil(10));
  }

  //Realign buffer?
  //int count = 0;
  //while(Serial1.available() && count < 5){ count++; }

  //Comms through 19/18 RX/TX ports
  if(Serial1.available()){

    /* Read Serial */
    //String str = Serial1.readString();
    String str = Serial1.readStringUntil(10);   //Terminate before the newline (10 = ASCII linefeed)
    Serial.println("Serial found: " + str);
    Serial.flush();     //Wait explicitly until anything sent through Serial is completely sent

    /* Parsing and Analyzing */
    String parse;
    while(str.length() > 0){
      parse = str.substring(0, str.indexOf(','));
      str = str.substring(parse.length() + 1);
      parse.trim();             //Cleans up leading and following whitespace

      //Determine if the command is supported
      if(parse.equals("goto")){
        Serial.println("Executing: \"goto\" ");

        //Extract x and y coords
        String xCoord, yCoord;
        xCoord = str.substring(0, str.indexOf(','));
        xCoord.trim();
        str = str.substring(parse.length() + 1);
        yCoord = str.substring(0, str.indexOf(','));
        yCoord.trim();
        str = str.substring(parse.length() + 1);

        //Head to coords and signal Jetson
        Serial.println("Heading to: x:" + xCoord + " y:" + yCoord);
        delay(2000);
        Serial.println("Arrived at: x:" + xCoord + " y:" + yCoord);
        currentPosX = xCoord.toInt();
        currentPosY = yCoord.toInt();

        Serial1.println("goto success");
      }
      else if(parse.equals("establish")){
        Serial.println("Executing: \"establish\"");
        Serial.println("Confirming with Jetson...");
        Serial1.println("establish success");
      }
      else{
        Serial.println("Command not recognized: \"" + parse + "\"");
        Serial.println("Flushing buffer...");
        // Serial1.end();
        // Serial1.begin(BAUD_RATE);
      }
    }
  }
}
