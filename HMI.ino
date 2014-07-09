void listInputs(char *buff,byte channel){
  char *eos;
  eos=buff;
  eos+=sprintf_P(buff,showInputStr,!digitalReadFast(ENERGYPULSES),!digitalReadFast(HEATING),!digitalReadFast(DHV),!digitalReadFast(COOLING),!digitalReadFast(EEVERR),
  !digitalReadFast(WELLPULSES),!digitalReadFast(HEATINGPULSES),analogRead(CONDP),analogRead(EVAP1),analogRead(EVAP2),analogRead(MA420),!digitalReadFast(UDNFAULT));
  sendToUser(buff,channel);
}

void listOutputs(char *buff,byte channel){
  char *eos;
  eos=buff;
  eos+=sprintf_P(buff,showOutputStr,wellPumpPwm,digitalReadFast(WELLPUMP),digitalReadFast(BATON),digitalReadFast(CIRCPUMP),digitalReadFast(COMPRESSOR),
  digitalReadFast(EVAPFAN),digitalReadFast(EEV),digitalReadFast(OUT7),digitalReadFast(OUT8),digitalReadFast(UDNENABLE));
  sendToUser(buff,channel);
}

void listSettings(char *buff,byte channel){
  char *eos;
  eos=buff;
  eos+=sprintf_P(buff,listSettingsStr1,cRestartRate,cStartDelay/1000,errorRecoveryDelay/1000,battSwitchOffDelay/1000,postHeatCirc/1000);
  sendToUser(buff,channel);
  eos=buff;
  eos+=sprintf_P(buff,listSettingsStr2,preHeatCirc/1000,ignoreEEVErr/1000,heatingWellFlowRate,coolingWellFlowRate,minWellFlowRate);
  sendToUser(buff,channel);
  eos=buff;
  eos+=sprintf_P(buff,listSettingsStr3,minHeatFlowRate,int(minWellWaterOutTemp),int(minWellWaterOutTemp*10)%10,int(maxCondPressure),int(maxCondPressure*10)%10,int(superHeatLowLimit),int(superHeatLowLimit*10)%10);
  sendToUser(buff,channel);
  eos=buff;
  eos+=sprintf_P(buff,listSettingsStr4,int(superHeatHighLimit),int(superHeatHighLimit*10)%10,int(maxComprT),int(maxComprT*10)%10,int(maxPressureDiff),int(maxPressureDiff*10)%10);
  sendToUser(buff,channel);
  eos=buff;
  eos+=sprintf_P(buff,PSTR("error: %x\n"),errorStatus());
  sendToUser(buff,channel);

}

boolean setSettings(char *buff,int channel){
  int setting;
  if (sscanf_P(buff+1,PSTR("%d %s"),&setting,buff)==2 && setting>=0 && setting<NUMOFEESETTINGS){
    if (setting<NUMOFINTSETTINGS){
      EEPROM.writeInt(EECRSTARTR+setting*2,atoi(buff));
      readEEConfig();
      return true;
    }
    else if (setting<NUMOFEESETTINGS){
      EEPROM.writeFloat(EEMINWOUTT+(setting-NUMOFINTSETTINGS)*4,atof(buff));
      readEEConfig();
      return true;
    }
  }
  return false;
}

int listOwDevices(char *buff, OneWire bus,byte channel){
  static byte searchAddr[8],i;
  char *eos;
  float temperature;
  int eeOwAddrPointer,devCount=0;
  bus.reset_search();
  while (bus.search(searchAddr)){ //there is device on bus
    eos=buff;
    *buff='\0';
    devCount++;
    eeOwAddrPointer=0;
    if (devCount>OWMAXSEARCH){
      devCount=-1;
      break;
    }
    eos+=sprintf_P(eos,PSTR("%02d;"),devCount-1);
    if (OneWire::crc8(searchAddr, 7) == searchAddr[7]){  //check for CRC error
      for (i=0;i<OWADDRLEN;i++)
        eos+=sprintf_P(eos,PSTR("%02X"),searchAddr[i]);
      temperature=readTemp(searchAddr,bus);
      if (temperature>=-55){
        *eos++=' ';
        eos+=strlen(dtostrf(temperature,3,2,eos));
        *eos++='C';
      }
      else
        eos+=sprintf_P(eos,PSTR(" ---"));
      eeOwAddrPointer=eeOwAddr(searchAddr,eeOwAddrPointer);
      while ((eeOwAddrPointer=eeOwAddr(searchAddr,eeOwAddrPointer))>=0){
        *eos++=' ';
        eos+=sprintf_P(eos,PSTR(" %d; (%d) "),eeOwAddrPointer,EEPROM.read(EEOWBUSADDR(eeOwAddrPointer)));
        eos+=strlen(strcpy_P(eos,(char*)pgm_read_word(&(owNames[eeOwAddrPointer]))));
        eeOwAddrPointer++;
      }
      *eos='\0';
    } 
    else
      sprintf_P(buff,PSTR("1W CRC ERROR!"));
    sendToUser(buff,channel);
  }
  return devCount;
}

int sprintfOwAddr(char *buff,byte *owAddr){
  byte i,count=0;
  for (i=0;i<OWADDRLEN;i++)
    count+=sprintf_P(buff+count,PSTR("%02X"),owAddr[i]);   
  return count;
}

void listOwFunctions(char *buff,byte channel){
  byte i, owAddrTmp[8];
  char *eos,c;
  for (i=0;i<NUMOFELEMENTS(owNames);i++){
    eos=buff;
    ee2ow(owAddrTmp,EEOWADDR(i));
    if (owAddrTmp[0]!=0)
      c='*';
    else
      c=' ';    
    eos+=sprintf_P(eos,PSTR("%02d; %c "),i,c);
    eos+=strlen(strcpy_P(eos,(char*)pgm_read_word(&(owNames[i]))));
    *eos++='\0';
    sendToUser(buff,channel);
  }
}

boolean unlinkOwFunction(char *buff, byte channel){
  int devNum,i;
  if (sscanf_P(buff+1,PSTR("%d"),&devNum)==1 && devNum>=0 && devNum<NUMOFELEMENTS(owNames) && devNum<EEOWMAXDEVS){
    for (i=0;i<EEOWADDRLEN;i++)
      EEPROM.write(EEOWBUSADDR(devNum)+i,0);
    return true;
  }
  return false;
}

boolean assignOwToFunction(char *buff, byte channel){
  int devNum,functNum,busNum,i;
  byte owAddr[8];

  if (sscanf_P(buff+1,PSTR("%d %d %d"),&busNum,&devNum,&functNum)==3 && devNum>=0 &&  devNum<OWMAXSEARCH)
    if (busNum>=0 && busNum<NUMOFELEMENTS(owBus) && functNum>=0 && functNum<NUMOFELEMENTS(owNames) ){
      owBus[busNum].reset_search();
      for (i=0;i<=devNum;i++){
        if (!owBus[busNum].search(owAddr))
          return false;
      } 
      if (OneWire::crc8(owAddr, 7) == owAddr[7]){  //check for CRC error
        ow2ee(EEOWADDR(functNum),owAddr);  //save 1W device address to eeprom
        EEPROM.write(EEOWBUSADDR(functNum),(byte)busNum);  //save bus number
        return true;
      }
    }
  return false;
}

boolean listNetSettings(char *buff,byte channel){
  byte ip[4],i;
  char *eos;
  for (i=0;i<NUMOFIPSETTINGS;i++){
    eos=buff;
    eos+=sprintf_P(eos,PSTR("%02d;"),i);
    eos+=strlen(strcpy_P(eos,(char*)pgm_read_word(&(netNames[i]))));
    ee2ip(ip,EEIP+i*SIZEOFIP);
    eos+=sprintf_P(eos,PSTR(": %d.%d.%d.%d"),ip[0],ip[1],ip[2],ip[3]);
    sendToUser(str,channel);
  }
  for (;i<NUMOFPORTSETTINGS+NUMOFIPSETTINGS;i++){
    eos=buff;
    eos+=sprintf_P(eos,PSTR("%02d;"),i);
    eos+=strlen(strcpy_P(eos,(char*)pgm_read_word(&(netNames[i]))));
    eos+=sprintf_P(eos,PSTR(": %d"),ee2port(EECMDP+(i-NUMOFIPSETTINGS)*SIZEOFPORT));
    sendToUser(str,channel);
  }
  eos=buff;
  eos+=sprintf_P(eos,PSTR("%02d;"),i+1);
  ee2string(100,eos,100);
  sendToUser(str,channel);
}

boolean writeNetSetting(char *buff,byte channel){
  int data[4],cmd;
  byte ip[4],hits,i;
  hits=sscanf_P(buff+1,PSTR(" %d %d.%d.%d.%d"),&cmd,&data[0],&data[1],&data[2],&data[3]);
  Serial.println(hits);
  Serial.println(cmd);

  if (cmd<NUMOFIPSETTINGS && hits==5){
    for (i=0;i<SIZEOFIP;i++){
      if (data[i]<0 || data[i]>255)
        return false;
      ip[i]=byte(data[i]);
    }
    ip2ee(EEIP+cmd*SIZEOFIP,ip);
    return true;
  }
  if (cmd<NUMOFIPSETTINGS+NUMOFPORTSETTINGS && hits==2){
    port2ee(EECMDP+(cmd-NUMOFIPSETTINGS)*SIZEOFPORT,data[0]);
    return true;
  }
  return false;
}

void notInConfigMode(byte channel){
  strcpy_P(str,answNotInConfigMode);
  sendToUser(str,channel);  
}

boolean commandInterpreter(char* Input,byte channel){      //waiting complete string not fragments!
  int i;
  char cmd[CMDMAXLEN+1];

  if (strlen(Input)>CMDMAXLEN+2)        //truncate incoming data to max length
    Input[CMDMAXLEN]='\0';
  switch (tolower(Input[0])){      //identify commands
  case 'c':                         //config mode, check for password
    sscanf_P(Input+1,PSTR("%s"),cmd);
    if (strcmp(cmd,PASSWD)==0){
      configMode=true;
      strcpy_P(str,answConfOn);
    }
    else {
      configMode=false;
      strcpy_P(str,answConfOff);
    }
    sendToUser(str,channel);
    break;
  case 'h':
    int heatingDuration;
    if (1==sscanf_P(Input,PSTR("heating %d"),&heatingDuration) && (heatingDuration ==0 || (heatingDuration >= MINNETHEATINGTIME && heatingDuration < MAXNETHEATINGTIME))){
      if (heatingDuration > 0)
        sprintf_P(str,answHeatingStart,heatingDuration);
      else
        sprintf_P(str,answHeatingStop);
      sendToUser(str,channel);
      heatingRequest(heatingDuration);
    }
    else
      return false;
    break;
  case '?':              // help
    strncpy_P(str,answHelp,sizeof(str));
    sendToUser(str,channel);
    strncpy_P(str,answHelp2,sizeof(str));
    sendToUser(str,channel);
    strncpy_P(str,answHelp3,sizeof(str));
    sendToUser(str,channel);
    break;
  case 'l':            //list
    sscanf_P(Input+1,PSTR("%s"),cmd);
    if (strcmp_P(cmd,PSTR("1w"))==0){
      strncpy_P(str,answ1w,sizeof(str));
      sendToUser(str,channel);
      for (i=0;i<NUMOFELEMENTS(owBus);i++){
        sprintf_P(str,PSTR("Bus:%i"),i);
        sendToUser(str,channel);
        listOwDevices(str,owBus[i],channel);
      }
      strcpy_P(str,PSTR("Functions:"));
      sendToUser(str,channel);
      listOwFunctions(str,channel);
    }
    else if (strcmp_P(cmd,PSTR("net"))==0 || strcmp_P(cmd,PSTR("n"))==0){
      strncpy_P(str,answnet,sizeof(str));
      sendToUser(str,channel);
      listNetSettings(str,channel);
    }
    else if (strcmp_P(cmd,PSTR("input"))==0 || strcmp_P(cmd,PSTR("i"))==0){
      strncpy_P(str,answinput,sizeof(str));
      sendToUser(str,channel);
      listInputs(str,channel);
    }
    else if (strcmp_P(cmd,PSTR("output"))==0 || strcmp_P(cmd,PSTR("o"))==0){
      strncpy_P(str,answoutput,sizeof(str));
      sendToUser(str,channel);
      listOutputs(str,channel);
    }
    else if (strcmp_P(cmd,PSTR("settings"))==0 || strcmp_P(cmd,PSTR("s"))==0){
      strncpy_P(str,answsettings,sizeof(str));
      sendToUser(str,channel);
      listSettings(str,channel);
    }
    else if (strcmp_P(cmd,PSTR("on"))==0){
      strncpy_P(str,answLogOn,sizeof(str));
      sendToUser(str,channel);
      logChannel=channel;
    }
    else if (strcmp_P(cmd,PSTR("off"))==0){
      strncpy_P(str,answLogOff,sizeof(str));
      sendToUser(str,channel);
      logChannel=0;
    }
    else
      return false;
    break;    
  case 'd':
    if (configMode){
      if (!unlinkOwFunction(Input,channel))
        return false;
    }
    else
      notInConfigMode(channel);
    break;
  case 'a':
    if (configMode){
      if (!assignOwToFunction(Input, channel))
        return false;
    }
    else
      notInConfigMode(channel);
    break;
  case 'n':
    if (configMode){
      if (!writeNetSetting(Input,channel))
        return false;
    }
    else
      notInConfigMode(channel);
    break;
  case 'r':
    if (configMode){
      sscanf_P(Input+1,PSTR("%s"),cmd);
      if (strcmp_P(cmd,PSTR("udn"))==0 || strcmp_P(cmd,PSTR("u"))==0){
        strncpy_P(str,answResetUdn,sizeof(str));
        if (resetUdn())
          strcpy_P(str+strlen(str),PSTR(" OK"));
        else
          strcpy_P(str+strlen(str),PSTR(" Overcurrent!"));
        sendToUser(str,channel);
      }
      else if (strcmp_P(cmd,PSTR("eset!"))==0)
        softReset();
    }
    else
      notInConfigMode(channel);
    break;
  case 's':
    if (configMode){
      if (!setSettings(Input, channel))
        return false;
    }
    else
      notInConfigMode(channel);
    break;
  default :
    return false;
  }
  return true;
}


