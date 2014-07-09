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

ISR(INT0_vect){
  wellWaterCounter++;
}  //interrupt handler (well water meter)
ISR(INT1_vect){
  heatingWaterCounter++; 
  totalHeatingPulses++;
}  //interrupt handler (heating water meter)
ISR(INT2_vect){               //interrupt handler (Watt meter)
  static unsigned long oldMillis=0;
  unsigned long currentMillis=millis();
  if (firstEnergyPulse)
    firstEnergyPulse=false;
  else
    energyPulsePeriod=currentMillis-oldMillis;
  oldMillis=currentMillis;
  newEnergyPulse=true;
  return;
}

ISR(INT3_vect){
}        //interrupt handler (encoder A signal)
ISR(INT5_vect){
}  //interrupt handler (EEV error)
ISR(TIMER3_COMPA_vect){
  unsigned int wWCt, hWCt;
  static unsigned int wWCtLast=0, hWCtLast=0;
  oneSecTrigger=true;
  cli();
  wWCt=wellWaterCounter;
  hWCt=heatingWaterCounter;
  sei();
  wellWaterPulses=wWCt-wWCtLast;
  wWCtLast=wWCt;
  heatingWaterPulses=hWCt-hWCtLast;
  hWCtLast=hWCt;
  return;
}

