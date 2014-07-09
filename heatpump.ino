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

#include <digitalWriteFast.h>
#include <avr/pgmspace.h>
#include <OneWire.h>
#include "globalvar.h"
#include "avr/interrupt.h"
#include <EtherCard.h>
#include <IPAddress.h>
#include <string.h>
#include "r407cptarrays.h"
#include <EEPROMex.h>

OneWire ow1(OWHP);
OneWire ow2(OWEXT);
OneWire ow3(OWINT);
OneWire owBus[]={
  ow1,ow2,ow3};

static byte mymac[6] = {
  0x02,'A','r','d','0','2'};
byte Ethernet::buffer[1600]; // tcp/ip send and receive buffer
static word dstPrt,srcPrt;
static byte dstIp[4];
static BufferFiller bfill;
static byte myip[4];  // ethernet interface ip address
static byte gwip[4];  // gateway ip address
static byte netmask[4];  // netmask
static byte dnsip[4];  // dns ip address
static word udpHmiPort;

void setup(){
  int i;
  //setup inputs  
  pinMode(WELLPULSES,INPUT_PULLUP);
  pinMode(HEATINGPULSES,INPUT_PULLUP);
  pinMode(ENERGYPULSES,INPUT_PULLUP);
  pinMode(ETHPKT,INPUT_PULLUP);
  pinMode(EEVERR,INPUT);
  pinMode(COOLING,INPUT);
  pinMode(HEATING,INPUT);
  pinMode(DHV,INPUT);
  pinMode(ENCA,INPUT);
  pinMode(ENCB,INPUT);
  pinMode(BTTN,INPUT);
  pinMode(UDNFAULT,INPUT);

  //setup analog inputs
  analogReference(EXTERNAL);
  pinMode(EVAP1,INPUT);
  pinMode(EVAP2,INPUT);
  pinMode(CONDP,INPUT);
  pinMode(MA420,INPUT);

  //setup outputs
  pinMode(LED,OUTPUT);
  digitalWriteFast(LED,LOW);
  pinMode(ERRORLED,OUTPUT);
  digitalWriteFast(ERRORLED,HIGH);
  pinMode(WELLPUMP,OUTPUT);
  digitalWriteFast(WELLPUMP,LOW);
  pinMode(BATON,OUTPUT);
  digitalWriteFast(BATON,LOW);
  pinMode(CIRCPUMP,OUTPUT);
  digitalWriteFast(CIRCPUMP,LOW);
  pinMode(COMPRESSOR,OUTPUT);
  digitalWriteFast(COMPRESSOR,LOW);
  pinMode(EVAPFAN,OUTPUT);
  digitalWriteFast(EVAPFAN,LOW);
  pinMode(EEV,OUTPUT);
  digitalWriteFast(EEV,LOW);
  pinMode(OUT7,OUTPUT);
  digitalWriteFast(OUT7,LOW);
  pinMode(OUT8,OUTPUT);
  digitalWriteFast(OUT8,HIGH);
  pinMode(UDNENABLE,OUTPUT);
  resetUdn();
  pinMode(WELLSPEED,OUTPUT);
  setWellPumpPwm(0);  

  Serial2.begin(115200);  //bluetooth port
  Serial.begin(115200);
  Serial.println("RESTART");
  ether.begin(sizeof Ethernet::buffer, mymac,ENCCS);
  setupIp();
  //  if (!ether.dhcpSetup())
  //  Serial.println("DHCP failed");
  EEPROM.setMemPool(0,EEPROMSizeMega);
  readEEConfig();

  //external interrupts setup
  EICRA=0b10111111;  //set external interrupt rising edges
  EICRB=0b00001000;  //faling edge EINT5 outage change interrupt on INT4
  //timer3 configuration
  TCCR3A=0;
  TCCR3B=0b00001100;  //reset on compare mach 1/256 divider
  OCR3A=62499;    //compare register value 16M/256-1 for 1s period

  cli();  
  bitSet(TIMSK3,OCIE3A);  //enable interrupt on compare match
  EIMSK |= 0b00000111;  //enable EINT5-0 (Mega pin 3,2,18,19,20,21 )  
  sei();  //enable interrupts
  delay(500);
  digitalWrite(ERRORLED,LOW);
  digitalWrite(LED,HIGH);
  delay(200);
  digitalWrite(LED,LOW);
}


void loop(){
  word pktLen=0;
  static long ms10=0,ms300=0;

  currentMillis=millis();
  errorStatus();
  heatingControl();
  if (oneSecTrigger){
    oneSecTrigger=false;
    calculateFlowRates();
    calculateConsumption(false);
    calculateHeatingPower();
    calculateWellPower();
    subCoolT=subCool(condenserPressure(),owTemperature[CONDLADDR]);
    superHeatT=superHeat(evaporatorPressure(),owTemperature[SUCTADDR]);
  }
  if (newEnergyPulse)
    calculateConsumption(true);

  pktLen=ether.packetLoop(ether.packetReceive());
  if (pktLen){
    ether.httpServerReplyAck(); // send ack to the request
    ether.httpServerReply_with_flags(monitorPage1(),TCP_FLAGS_ACK_V);
    ether.httpServerReply_with_flags(monitorPage2()-1,TCP_FLAGS_ACK_V|TCP_FLAGS_FIN_V); //last tcp packet
  }

  if (currentMillis - ms10 > 10){
    ms10=currentMillis;
    updateLeds();
    evapPRaw+=0.02*(analogRead(EVAP2)-analogRead(EVAP1)-evapPRaw);
    condPRaw+=0.01*(analogRead(CONDP)-condPRaw);
  }
  else if (currentMillis - ms300 > 300){
    ms300=currentMillis;
    updateTemperatures();
  }
}


