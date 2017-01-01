#include <GSM.h>

GSM gsmAccess;
GSM2 gsmAccessV2;

void setup()
{
  Serial.begin(9600);

  while(!Serial) 
  {
    ;
  }

  boolean notConnected = true;

  while(notConnected) 
  {
    if(gsmAccess.begin("") == GSM_READY) 
    {
      notConnected = false;
    } 
    
    else 
    {
      delay(1000);
    }
  }
  
  gsmAccessV2.muteControl(0); 
  gsmAccessV2.CommandEcho(1);  
  gsmAccessV2.speakerMode(1);  
  gsmAccessV2.speakerLoudness(3);
  gsmAccessV2.swapAudioChannel(1);
  gsmAccessV2.microphoneGainLevel(1,13);
  gsmAccessV2.ringerSoundLevel(20);  
  gsmAccessV2.alertSoundMode(0);   
  gsmAccessV2.loudSpeakerVolumeLevel(20);
  
  Serial.println("Ready!");
}

void loop()
{
  char val = Serial.read();

  /*
   * 1 - Make voice call
   * 2 - Send SMS
   */

  if(val == '1')
  {
    makeVoiceCall();
  }

  if(val == '2')
  {
    sendSMS();
  }

  recvCall();
  recvSMS();
}

void makeVoiceCall()
{ 
  GSMVoiceCall vcs;
  String remoteNumber = Serial.readStringUntil('#');  
  char charbuffer[20];

  remoteNumber.toCharArray(charbuffer, 20);

  if(vcs.voiceCall(charbuffer)) 
  {
    //char v = Serial.read();
    
    while(/*v*/Serial.read() != '/' || (vcs.getvoiceCallStatus() == TALKING))
    {
      //v = Serial.read();
    }
    
    vcs.hangCall();
  }
}

void sendSMS()
{
  GSM_SMS sms;
  String remoteNum = Serial.readStringUntil('#');
  char charbuffer[20];

  remoteNum.toCharArray(charbuffer, 20);

  String txtMSG = Serial.readStringUntil('|');
  char charbuffer2[200];

  txtMSG.toCharArray(charbuffer2, 200);

  sms.beginSMS(charbuffer);
  sms.print(charbuffer2);
  sms.endSMS();  
}

void recvCall()
{
  GSMVoiceCall vcs;
  char numtel[20];

  vcs.hangCall();

  switch(vcs.getvoiceCallStatus())
  {
    case IDLE_CALL:
      break;

    case RECEIVINGCALL:
      vcs.retrieveCallingNumber(numtel, 20);
      vcs.answerCall();

      break; 

    case TALKING:
      //char v = Serial.read();
      
      while(/*v*/Serial.read() != '/')
      {
        delay(100);
        
        //v = Serial.read();
      }

      vcs.hangCall();

      break;
  }

  delay(1000);
}

void recvSMS()
{
  GSM_SMS sms;
  char senderNumber[20];
  char c;
  String txtMSG;
  char charbuffer[200];

  if(sms.available())
  {
    sms.remoteNumber(senderNumber, 20);

    if(sms.peek() == '#')
    {
      sms.flush();
    }

    while(c = sms.read())
    {
      txtMSG += c; 
    }

    sms.flush();

    txtMSG.toCharArray(charbuffer, 300);
    
    Serial.print(senderNumber);
    Serial.print(": ");
    Serial.print(charbuffer);
    Serial.print("\n");
  }

  delay(1000);
}
