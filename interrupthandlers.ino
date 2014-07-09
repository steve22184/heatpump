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

