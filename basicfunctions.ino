/*
    Takyka's heat pump controller
    Copyright (C) 2014  Balazs Takacs e-mail: takyka@freemail.hu

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 3 as published by
    the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void sendLogMessage(int err,byte state){
  char *eos;
  if (logChannel>0){
    eos=str;
    eos+=sprintf_P(str,PSTR("***Log: uptime:%lds, heating state: %d, 0x%X"),millis()/1000,state,err);
    sendToUser(str,logChannel);
  }
}

int errorStatus(){
  if (minWellFlowRate>wellFlowRate)
    bitSet(error,WELLFLOWRATE);
  else
    bitClear(error,WELLFLOWRATE);
  if (minHeatFlowRate>heatingFlowRate)
    bitSet(error,HEATINGFLOWRATE);
  else
    bitClear(error,HEATINGFLOWRATE);
  if (owTemperature[WWOUTADDR]<minWellWaterOutTemp)
    bitSet(error,WELLWATERTEMP);
  else
    bitClear(error,WELLWATERTEMP);
  if (condPressure>maxCondPressure)
    bitSet(error,CONDENSERPRESSURE);
  else
    bitClear(error,CONDENSERPRESSURE);
  if (superHeatT<superHeatLowLimit)
    bitSet(error,MINSUPERHEAT);
  else
    bitClear(error,MINSUPERHEAT);
  if (owTemperature[COMPADDR]>maxComprT)
    bitSet(error,MAXCOMPTEMP);
  else
    bitClear(error,MAXCOMPTEMP);
  if ((condPressure - evapPressure) > maxPressureDiff)      
    bitSet(error,STARTPRESSDIFF);
  else
    bitClear(error,STARTPRESSDIFF);
  bitWrite(error,EEVERROR,!digitalReadFast(EEVERR));
  bitWrite(error,UDNERROR,!digitalReadFast(UDNFAULT));
  return error;
}

void softReset(){
  asm volatile ("  jmp 0");
}

void setupIp(){
  ee2ip(myip,EEIP);
  ee2ip(gwip,EEGW);
  ee2ip(netmask,EENM);
  ee2ip(dnsip,EEDNS);
  udpHmiPort=ee2port(EECMDP);

  ether.staticSetup(myip, gwip, dnsip);
  ether.copyIp(ether.mymask,netmask);
  ether.udpServerListenOnPort(&udpEvent, udpHmiPort);
}

void readEEConfig(){
  cRestartRate=EEPROM.readInt(EECRSTARTR);
  cStartDelay=(long)EEPROM.readInt(EECSTARTD)*1000;
  errorRecoveryDelay=(long)EEPROM.readInt(EEERRRECOVERY)*1000;
  battSwitchOffDelay=(long)EEPROM.readInt(EEBATOFF)*1000;
  postHeatCirc=(long)EEPROM.readInt(EEPOSTHEAT)*1000;
  preHeatCirc=(long)EEPROM.readInt(EEPREHEAT)*1000;
  ignoreEEVErr=(long)EEPROM.readInt(EEIGNOREEEV)*1000;
  heatingWellFlowRate=EEPROM.readInt(EEHWFRATE);
  coolingWellFlowRate=EEPROM.readInt(EECWFRATE);
  minWellFlowRate=EEPROM.readInt(EEMINWFRATE);
  minHeatFlowRate=EEPROM.readInt(EEMINHFRATE);
  minWellWaterOutTemp=EEPROM.readFloat(EEMINWOUTT);
  maxCondPressure=EEPROM.readFloat(EEMAXCONDP);
  superHeatLowLimit=EEPROM.readFloat(EESHLLIMIT);
  superHeatHighLimit=EEPROM.readFloat(EESHHLIMIT);
  maxPressureDiff=EEPROM.readFloat(EEMAXSTARTPDIFF);
  maxComprT=EEPROM.readFloat(EEMAXCOMPRT);
}

float subCool(float pressure,float temperature){
  int index = int(pressure*10);    //temperatures stored in array by 0,1Bar steps
  if (index<0 || index>NUMOFELEMENTS(r407cBubble))
    return -99;
  else
    return temperature-pgm_read_float(&r407cBubble[index]);
}
float superHeat(float pressure,float temperature){
  int index = int(pressure*10);    //temperatures stored in array by 0,1Bar steps
  if (index<0 || index>NUMOFELEMENTS(r407cDew))
    return -99;
  else
    return temperature-pgm_read_float(&r407cDew[index]);
}

void updateTemperatures(){
  static byte i=0,addressIndex=0;
  byte owAddress[8], bus;
  unsigned long time;

  if (i==0){
    startTempConversion(owBus[0]);
    startTempConversion(owBus[1]);
    startTempConversion(owBus[2]);
    addressIndex=0;
  }
  else if (i>3 && addressIndex<EEOWMAXDEVS){
    ee2ow(owAddress,EEOWADDR(addressIndex));
    if (owAddress[0]!=0)    //address in use
      owTemperature[addressIndex]=readTemp(owAddress,owBus[EEPROM.read(EEOWBUSADDR(addressIndex))]);    //fill up temperature array
    else
      owTemperature[addressIndex]=NOTASSIGNED;
    addressIndex++;
  }

  if (i<19)
    i++;
  else
    i=0;
  return;
}

void setWellPumpPwm(byte pumpSpeed){
  analogWrite(WELLSPEED,pumpSpeed);
  wellPumpPwm=pumpSpeed;
}

void updateLeds(){
  static byte interval=0,actionAt=0;

  if (interval==0){
    if (error==0)
      digitalWriteFast(LED,HIGH);
    else
      digitalWriteFast(ERRORLED,HIGH);
    actionAt=10;
    if (heating || cooling)
      actionAt=40;
    if (dhv)
      actionAt=60;
    if (preStartDelay)
      actionAt=20;
  }
  if (interval==actionAt){
    digitalWriteFast(LED,LOW);
    digitalWriteFast(ERRORLED,LOW);
  }
  if (interval < 100)
    interval++;
  else
    interval=0;
}

void sendToUser(char *buff,byte channel){
  byte len=strlen(buff);
  if (strlen(buff)>0){
    switch (channel){
    case SERIAL:
      Serial.println(buff);
      break;
    case ETH:
      ether.udpPrepare(srcPrt,dstIp,dstPrt);
      if (len>=MAXUDPSIZE){
        buff[MAXUDPSIZE-1]='\0';
        len=MAXUDPSIZE;
      }
      strcpy((char*)Ethernet::buffer+UDP_DATA_P,buff);
      strcat((char*)Ethernet::buffer+UDP_DATA_P,"\n");
      ether.udpTransmit(len+1); 
      break;
    case BT:
      Serial2.println(buff);
      break;
    }
  }
}

void udpEvent(word port, byte ip[4],const char *data, word len){
  srcPrt=port;
  dstPrt=(word)(Ethernet::buffer[UDP_SRC_PORT_H_P]<<8)+Ethernet::buffer[UDP_SRC_PORT_L_P];
  ether.copyIp(dstIp,ip);
  if(!commandInterpreter((char*) data,ETH)){
    strcpy((char*)Ethernet::buffer+UDP_DATA_P,"NOK\n");
    ether.udpPrepare(srcPrt,dstIp,dstPrt);
    ether.udpTransmit(3);
  }
}

void serialEvent(){
  static char serStr[CMDMAXLEN+1];
  static char serStrIndex=0;

  serStr[serStrIndex]=Serial.read();
  switch (serStr[serStrIndex]){
  case -1 :
  case '\r':
    return;
  case '\n':
    Serial.write(serStr[serStrIndex]);
    serStr[serStrIndex+1]='\0';
    serStrIndex=0;
    if (!commandInterpreter(serStr,SERIAL))
      Serial.println(F("NOK"));
    break;
  default :
    Serial.write(serStr[serStrIndex]);
    if (serStrIndex<CMDMAXLEN)
      serStrIndex++;
  }

}

void serialEvent2(){
  static char btStr[CMDMAXLEN+1];
  static char btStrIndex=0;

  btStr[btStrIndex]=Serial2.read();
  switch (btStr[btStrIndex]){
  case -1 :
  case '\r':
    return;
  case '\n':
    Serial2.write(btStr[btStrIndex]);
    btStr[btStrIndex+1]='\0';
    btStrIndex=0;
    if (!commandInterpreter(btStr,BT))
      Serial2.println(F("NOK"));
    break;
  default :
    Serial2.write(btStr[btStrIndex]);
    if (btStrIndex<CMDMAXLEN)
      btStrIndex++;
  }

}
void calculateFlowRates(){
  wellFlowRate=pulses2Flowrate(wellWaterPulses);
  heatingFlowRate=pulses2Flowrate(heatingWaterPulses);
}

void calculateConsumption(boolean impulse){
  if (impulse){
    newEnergyPulse=false;
    lastEnergyPulseTime=currentMillis;
    lastEnergyPulsePeriod=energyPulsePeriod;
    consumption=period2Wh(energyPulsePeriod);
  }
  else if (lastEnergyPulsePeriod>0 && (currentMillis-lastEnergyPulseTime>lastEnergyPulsePeriod))
    consumption=period2Wh(currentMillis-lastEnergyPulseTime);
}

void calculateHeatingPower(){
  heatingPower=(float)WHEATLITER*heatingFlowRate*(owTemperature[HWOUTADDR]-owTemperature[HWINADDR]);
}
void calculateWellPower(){
  wellPower=(float)WHEATLITER*wellFlowRate*(owTemperature[WWINADDR]-owTemperature[WWOUTADDR]);
}

float condenserPressure(){
  return condPressure=float(map(condPRaw,MIN_PRESSURE_ANALOG,MAX_PRESSURE_ANALOG,MIN_PRESSURE,MAX_PRESSURE))*0.001;
}

float evaporatorPressure(){
  return evapPressure=float(map((long)(evapPRaw*100),14700,37300,260,780))*0.01;
}

boolean resetUdn(){
  digitalWrite(UDNENABLE,LOW);
  delay(1);
  digitalWrite(UDNENABLE,HIGH);
  delay(1);
  return digitalReadFast(UDNFAULT);
}

float pulses2Flowrate(unsigned int pulses){ //return flow rate in L/h
  return WATERPULSE*pulses*3600;
}

float period2Wh(unsigned long period){    //calculate power consumption from impulse period
  float result=0;
  if (period > 0)
    result=ENERGYPULSE/period;
  return result;
}

void startTempConversion(OneWire owBus){
  owBus.reset();
  owBus.skip();
  owBus.write(0x44,1); //start conversion
}



float readTemp(byte *addr, OneWire owBus){
  byte i, data[12], searchAddr[8];

  owBus.reset();
  if (addr==NULL){
    owBus.reset_search();
    if(!owBus.search(searchAddr) || (OneWire::crc8(searchAddr, 7) != searchAddr[7]))
      return -997;    //not found any or crc error
    addr=searchAddr;  
    owBus.reset();
  }
  if (addr[0]!=0x10 && addr[0]!=0x28 && addr[0]!=0x22)
    return UNKNOWNOWDEV;    //not one of DS18S20 or DS18B20 or DS1822
  owBus.select(addr);
  owBus.write(0xBE);  //read scratchpad
  for ( i = 0; i < 9; i++)           // we need 9 bytes
    data[i] = owBus.read();
  if (OneWire::crc8(data,8)!=data[8])
    return CRCERROR;    //scratchpad read crc error
  int raw = (data[1] << 8) | data[0];
  if (addr[0]==0x10){ //DS18S20
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
    else {
      byte cfg = (data[4] & 0x60);
      if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
      else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
      else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
      // default is 12 bit resolution, 750 ms conversion time
    }
  }
  return (float)raw / 16.0;
}

float ee2float(int eeaddr){
  byte temp[4];
  temp[0]=EEPROM.read(eeaddr++);
  temp[1]=EEPROM.read(eeaddr++);
  temp[2]=EEPROM.read(eeaddr++);
  temp[3]=EEPROM.read(eeaddr);
  return (float) *temp;
}

void float2ee(int eeaddr,float value){
  byte* temp;
  temp=(byte*) &value;
  EEPROM.write(eeaddr++,*(temp++));
  EEPROM.write(eeaddr++,*(temp++));
  EEPROM.write(eeaddr++,*(temp++));
  EEPROM.write(eeaddr,*temp);

}

byte* ee2ow( byte* owBuff, int eeaddr){
  byte i;

  for (i=0;i<8;i++){
    owBuff[i]=EEPROM.read(eeaddr++);
  }
  return owBuff;
}

void ow2ee(word eeaddr, byte* owBuff){
  byte i;

  for (i=0;i<8;i++){
    EEPROM.write(eeaddr++,owBuff[i]);
  }
}


void ee2ip( byte* ipBuff, word eeaddr){
  byte i;

  for (i=0;i<4;i++){
    ipBuff[i]=EEPROM.read(eeaddr++);
  }
}

void ip2ee(word eeaddr, byte* ipBuff){
  byte i;

  for (i=0;i<4;i++){
    EEPROM.write(eeaddr++,ipBuff[i]);
  }
}

word ee2word(word eeaddr){
  return ((word)(EEPROM.read(eeaddr++)<<8)+EEPROM.read(eeaddr));
}

void word2ee(word eeaddr, word port){
  EEPROM.write(eeaddr++,port>>8);
  EEPROM.write(eeaddr,port);
}

char* ee2string(word eeaddr,char *str,int len){
  int i;
  for (i=0;i<len;i++){
    if (str[i]=EEPROM.read(eeaddr)=='\0')
      return str;
  }
  str[i+1]='\0';
  return str;
}

int eeOwAddr(byte *owAddr,int nextEeAddr){ //looking for 1w device address in eeprom, return the address
  byte owAddrTmp[8];

  if (nextEeAddr<0)
    return -1;
  for (;nextEeAddr<EEOWMAXDEVS;nextEeAddr++){
    if (cmpOwAddr(ee2ow(owAddrTmp,EEOWADDR(nextEeAddr)),owAddr)){
      return nextEeAddr;    
    }
  }
  return -1;
}

bool cmpOwAddr(byte* owAddress1, byte* owAddress2){
  if (owAddress1!=NULL && owAddress2!=NULL && 0==memcmp(owAddress1,owAddress2,OWADDRLEN))
    return true;
  return false; 
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}


