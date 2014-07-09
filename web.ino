static word monitorPage1() {
  byte i;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR(
  "HTTP/1.0 200 OK\r\n"
    "Content-Type<td>text/html\r\n"
    "Pragma<td>no-cache\r\n"
    "\r\n"
    "<meta http-equiv='refresh' content='10'/>"
    "<title>-= Heatpump monitor =-</title>"
    "<style>"
    "table{font-size:10pt; text-align:center;}"  
    "td{background-color:gray;}"
    "b{background-color:blue; color:white;}"  
    "g{background-color:green;}"
    "r{background-color:red;}"
    "</style>"
    "<body><table>"
    "<tr><th colspan=8>bemenetek</th>"
    "<tr><td>$SFutes<td>$SHMV<td>$SHutes<td>$SEEV hiba<td>$SUDN driver hiba<td>$SNET futes<td>$SNET hutes"
    "<tr><th colspan=8>kimenetek"
    "<tr><td>$SKutszivattyu<div style=\"width:$T%;height:2px;background-color:red;\"></div><td>$SAkku<td>$SKeringeto szivattyu<td>$SKompresszor<td>$SLevegos elpar. venti.<td>$SEEV<td>$SOUT7<td>$SOUT8"
    "<tr><th colspan=8>Nyomasok"
    "<tr><td>Elparologtato<td>$TBar<td>Tulhevites<td>$S$TC&deg<td>Kondenzator<td>$TBar<td>Utohutes<td>$S$TC&deg"
    "<tr><th colspan=8>homersekletek"),
  cssColor(HEATING,cssNone,cssGreen), cssColor(DHV,cssNone,cssGreen),cssColor(COOLING,cssNone,cssGreen),cssColor(EEVERR,cssNone,cssRed),cssColor(UDNFAULT,cssNone,cssRed),cssBoolColor(netHeating,cssGreen,cssNone),cssBoolColor(netCooling,cssGreen,cssNone), //Inputs
  cssColor(WELLPUMP,cssBlue,cssNone),(float)wellPumpPwm/2.55,cssColor(BATON,cssBlue,cssNone),cssColor(CIRCPUMP,cssBlue,cssNone),cssColor(COMPRESSOR,cssBlue,cssNone),
  cssColor(EVAPFAN,cssBlue,cssNone),cssColor(EEV,cssBlue,cssNone),cssColor(OUT7,cssBlue,cssNone),cssColor(OUT8,cssBlue,cssNone),evapPressure,
  superHeatColor(superHeatT,cssRed,cssYellow,cssGreen),superHeatT,condPressure,subCoolColor(subCoolT,cssYellow,cssGreen),subCoolT);
  for (i=0;i<EEOWMAXDEVS;i++){
    if (i%4==0)
      bfill.emit_p(PSTR("<tr>"));
    bfill.emit_p(PSTR("<td>"));
    bfill.emit_p((char*)pgm_read_word(&(owNamesH[i])));
    bfill.emit_p(PSTR("<td>$TC&deg"),owTemperature[i]);

  }
  return bfill.position();
}

static word monitorPage2() {
  long t = millis() / 1000;
  word h = t / 3600;
  byte m = (t / 60) % 60;
  byte s = t%60;
  bfill = ether.tcpOffset();
  bfill.emit_p(PSTR("<tr><th colspan=8>Tomegaramok, Teljesitmenyek</th>"
    "<tr><td>Futoviz<td>$TL/h<td>Kutviz<td>$TL/h"
    "<tr><td>Villamos fogyasztas<td>$TW<td>Futoteljesitmeny<td>$TkW<td>Kutviz telj. felvetel<td>$TkW<td>Pillanatnyi COP<td>$T"
    "<tr><th colspan=8>Hibak</th>"
    "<tr><td>$SKut tomegaram<td>$SF. tomegaram<td>$SKutviz homerseklet<td>$SKond. ny.<td>$STulhevites<td>$SKomp. hom."
    "<td>$Sind. ny. kulonbseg<td>$SAdagolo hiba"
    "<tr><td>$SUDN driver hiba"
    "</table>"),heatingFlowRate,wellFlowRate,consumption,heatingPower,wellPower,(float)heatingPower/consumption*1000,
  cssColorError(bitRead(error,WELLFLOWRATE)),cssColorError(bitRead(error,HEATINGFLOWRATE)),
  cssColorError(bitRead(error,WELLWATERTEMP)),cssColorError(bitRead(error,CONDENSERPRESSURE)),  
  cssColorError(bitRead(error,MINSUPERHEAT)),cssColorError(bitRead(error,MAXCOMPTEMP)),
  cssColorError(bitRead(error,STARTPRESSDIFF)),cssColorError(bitRead(error,EEVERROR)),
  cssColorError(bitRead(error,UDNERROR)));
  bfill.emit_p(PSTR("<br><small>uptime:$D$D:$D$D:$D$D<br>$D</body>"),h/10, h%10, m/10, m%10,s/10 ,s%10,heatingControl());
  return bfill.position();
}

char* cssColor(int IO,char* high,char* low){
  if (digitalReadFast(IO))
    return high;
  else
    return low;
}
char* cssBoolColor(boolean var,char* high,char* low){
  if (var)
    return high;
  else
    return low;
}
char* cssColorError(int errorStatus){
  if (errorStatus)
    return cssRed;
  else
    return cssGreen;
}

char* superHeatColor(float temperature,char* tooLow, char* tooHigh,char* ok){
  if (temperature > superHeatHighLimit)
    return tooHigh;
  else if (temperature < superHeatLowLimit)
    return tooLow;
  return ok;
}

char* subCoolColor(float temperature, char* tooHigh,char* ok){
  if (temperature > 0)
    return tooHigh;
  return ok;
}


