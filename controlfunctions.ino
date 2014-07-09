byte heatingControl(){
  static byte heatingState=0,i;
  static int logError;
  unsigned long now=millis();
  static unsigned long timeStamp;

  if (!heatingRequest(-1)){
    if (heatingState<5)
      timeStamp=now;
    if (heatingState>0 && heatingState<5)    //no heating requested jump state 5 if not there yet
      heatingState=5;    //stopping
  }
  switch (heatingState){
  case 0:      //no heating    
    if (heatingRequest(-1) && !(error & (M_UDNERROR | M_EEVERROR | M_MAXCOMPTEMP))){        //evaporator fan, prestart delay
      WAIT(500);
      digitalWrite(EVAPFAN,HIGH);
      digitalWrite(CIRCPUMP,HIGH);
      heatingState=1;
      sendLogMessage(0,heatingState);
    }
    break;
  case 1:
    ERROR_HANDLER(M_UDNERROR | M_EEVERROR | M_MAXCOMPTEMP);
    WAIT(preHeatCirc);
    ERROR_HANDLER(M_HEATINGFLOWRATE);
    digitalWrite(EEV,HIGH);
    heatingState=2;
    sendLogMessage(0,heatingState);
    break;
  case 2:
    ERROR_HANDLER(M_UDNERROR | M_EEVERROR | M_MAXCOMPTEMP | M_HEATINGFLOWRATE | M_CONDENSERPRESSURE);
    WAIT(cStartDelay);
    ERROR_HANDLER(M_STARTPRESSDIFF);
    digitalWrite(COMPRESSOR,HIGH);
    heatingState=3;
    sendLogMessage(0,heatingState);
    break;
  case 3:
    ERROR_HANDLER(M_UDNERROR | M_EEVERROR | M_MAXCOMPTEMP | M_HEATINGFLOWRATE | M_CONDENSERPRESSURE | M_MAXPOWER);
    WAIT(ignoreEEVErr);    
    heatingState=4;
    sendLogMessage(0,heatingState);
    break;
  case 4:
    ERROR_HANDLER(M_UDNERROR | M_EEVERROR | M_MAXCOMPTEMP | M_HEATINGFLOWRATE | M_CONDENSERPRESSURE | M_MAXPOWER | M_MINSUPERHEAT);
    break;
  case 5:
    sendLogMessage(logError,heatingState);
    digitalWrite(EVAPFAN,LOW);
    digitalWrite(EEV,LOW);
    digitalWrite(COMPRESSOR,LOW);
    timeStamp=now;
    heatingState=6;
    sendLogMessage(logError,heatingState);
    break;
  case 6:
    WAIT(postHeatCirc);
    digitalWrite(CIRCPUMP,LOW);
    heatingState=7;
    sendLogMessage(logError,heatingState);
    break;
  case 7:
    WAIT(errorRecoveryDelay);
    heatingState=0;    
    sendLogMessage(logError,heatingState);
    break;  
  }
  return heatingState;
}

boolean dhvRequest(){
  return !digitalReadFast(DHV);
}

boolean heatingRequest(int newDuration){
  static unsigned long now,netDuration;
  static unsigned long timeStamp;

  now=millis();
  if (newDuration>0){      //new heating request from net or stop if 0
    timeStamp=now;
    netDuration=(long)newDuration*1000;
    netHeating=true;
  }
  else if (newDuration==0)
    netHeating=false;

  if (netHeating && (now - timeStamp>netDuration))        //if net heating active check if time is over
    netHeating=false;

  if (!dhvRequest() && (!digitalReadFast(HEATING) || netHeating))
    return true;
  else
    return false;
}


boolean coolingRequest(int newDuration){
  static unsigned long now=millis(),netDuration;
  static unsigned long timeStamp;

  if (newDuration>0){      //new heating request from net or stop if 0
    timeStamp=now;
    netDuration=newDuration;
    netCooling=true;
  }
  else if (newDuration==0)
    netCooling=false;

  switch (netCooling){        //if net heating active check if time is over
  case 1:
    WAIT(netDuration);
    netCooling=false;
    break;
  }

  if ((!digitalReadFast(COOLING) || netCooling) &&!heatingRequest(-1)&&!dhvRequest())
    return true;
  else
    return false;
}



