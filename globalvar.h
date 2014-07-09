//pin definition
#define WELLSPEED 4        //inverter speed signal 0-5V
#define OWINT 5            //on board 1wire bus
#define OWHP A14          //heatpump 1w sensors
#define OWEXT A15          //external 1w sensors
#define ENCCS 53           //ethernet controller chip select output

//UDN driver signals
#define WELLPUMP  23      //well pump start
#define BATON  25          //connect battery to system
#define CIRCPUMP 27        //heating water pump
#define COMPRESSOR 29      //AC compressor
#define EVAPFAN 31          //evaporator fan
#define EEV 33              //electronic expansion valve start
#define OUT7  35
#define OUT8  37
#define UDNFAULT 39        //output driver fault input
#define UDNENABLE 41        //output driver OE

//LEDs
#define LED 13
#define ERRORLED 12

//control inputs
#define COOLING 15        //cooling request input
#define HEATING 14        //heating request
#define DHV 2             //hot water request

//analog inputs
#define EVAP1  A8        //evaporator diff pressure1
#define EVAP2  A9        //evaporator diff pressure2
#define CONDP  A0        //condenser pressure 4-20mA
#define MA420  A1        //spare 4-20mA input

//HMI pins
#define LCDRS  28        //LCD rs signal
#define LCDE   30        //LCD enable signal
#define LCDD4  32        //LCD D4-D7
#define LCDD5  34
#define LCDD6  36
#define LCDD7  38
//#define ENCA 18        //encoder A signal 
#define ENCB   47        //encoder B signal
#define BTTN   48        //encoder button

//inputs
#define ETHPKT 49          //Ethernet packet available

//interrupt pins
#define WELLPULSES 21      //INT0 
#define HEATINGPULSES 20   //INT1 
#define ENERGYPULSES  19   //INT2 
#define ENCA 18            //INT3 
#define EEVERR 3           //INT5 (atmega intno, arduino pin)

#define WATERPULSE (float)1/253.77  // L/imp    1/288???
#define ENERGYPULSE (float)1/800*3600000000   // Wms/imp (W milisecundum)

#define ee2port ee2word
#define port2ee word2ee


//IP settings 4-280
#define EEIP 4  //4bytes IP address
#define EENM 8  //4bytes netmask
#define EEGW 12  //4bytes gateway
#define EEDNS 16  //4bytes DNS server IP
#define EELOGTOIP 20 //4bytes Log server IP address
#define EECMDP 24 //2bytes user interface (UDP) listen port
#define EEWEBP 26 //2bytes WEB interface (TCP) listen port
#define EELOGTOPORT 18 //2bytes log server port
#define NUMOFIPSETTINGS 5
#define NUMOFPORTSETTINGS 3
#define EEQUERYSTRING 30  //log query string ee address
#define QUERYSTRINGMAXLEN 250  //log query string maximal length

//1w related constants 300-696 (max. 44 devices)
#define OWMAXSEARCH 20      //nr. of devices to search on bus
#define OWADDRLEN 8          //lenght of 1W device address (including crc)
#define EEOWADDRLEN (OWADDRLEN+1)    //address space in eeprom (device address + bus no.)
#define EEOWMAXDEVS 15        //number of devices handled by system
#define EEOWADDRFIRST 300      //address space in eeprom starting from. 
#define EEOWBUSADDR(a) a*EEOWADDRLEN+EEOWADDRFIRST
#define EEOWADDR(b) EEOWBUSADDR(b)+1
#define NOTASSIGNED -99
#define UNKNOWNOWDEV -98
#define CRCERROR -97

//1W device indexesin EEPROM
#define AINADDR 0
#define AOUTADDR 1
#define AEVAPADDR 2
#define WEVAPLADDR 3
#define WEVAPGADDR 4
#define WWINADDR 5
#define WWOUTADDR 6
#define CONDGADDR 7
#define CONDLADDR 8
#define HWINADDR 9
#define HWOUTADDR 10
#define EBOXADDR 11
#define COMPADDR 12
#define SUCTADDR 13
#define DISCADDR 14

//config settings
#define EECRSTARTR 700    //int cRestartRate
#define EECSTARTD  702   //int cStartDelay
#define EEERRRECOVERY 704 //int errorRecoveryDelay
#define EEBATOFF 706     //int battSwitchOffDelay
#define EEPOSTHEAT 708   //int postHeatCirc
#define EEPREHEAT 710    //int preHeatCirc
#define EEIGNOREEEV 712  //int ignoreEEVErr
#define EEHWFRATE 714   //int heatingWellFlowRate
#define EECWFRATE 716   //int coolingWellFlowRate
#define EEMINWFRATE 718 //int minWellFlowRate
#define EEMINHFRATE 720 //int minHeatFlowRate
#define NUMOFINTSETTINGS 11
#define EEMINWOUTT  722 //float minWellWaterOutTemp
#define EEMAXCONDP 726 //float maxCondPressure
#define EESHLLIMIT 730  //float superHeatLowLimit
#define EESHHLIMIT 734  //float superHeatHighLimit
#define EEMAXCOMPRT 738  //float maxComprT
#define EEMAXSTARTPDIFF  742 //float maxPressureDiff
#define NUMOFEESETTINGS 17


#define NUMOFELEMENTS(x)  (sizeof(x) / sizeof(x[0]))

#define K0 -273
#define PASSWD "2135"
#define CMDMAXLEN 20
#define MAXUDPSIZE 220
#define SIZEOFIP 4
#define SIZEOFPORT 2
#define WSTANDARDGRAVITY 0.99701  //kg/L
#define WSTANDARDHEAT 1.160555556 //W/kg/C
#define WHEATLITER (float)WSTANDARDGRAVITY* (float)WSTANDARDHEAT/1000 //kW/L/C
#define MINNETHEATINGTIME 10    //minimum heating duration if network command
#define MAXNETHEATINGTIME 3600    //maximum heating duration if network command
#define MINNETCOOLINGTIME 300    //minimum cooling duration if network command
#define MAXNETCOOLINGTIME 3600    //maximum cooling duration if network command
#define MIN_PRESSURE 0
#define MAX_PRESSURE 50000  //mBar
#define MIN_PRESSURE_VOLTAGE 0.8  //V
#define MAX_PRESSURE_VOLTAGE 4
#define VREF 5
#define RESOLUTION 1024
#define MIN_PRESSURE_ANALOG RESOLUTION*MIN_PRESSURE_VOLTAGE/VREF
#define MAX_PRESSURE_ANALOG RESOLUTION*MAX_PRESSURE_VOLTAGE/VREF

//communication channel
#define SERIAL 0
#define ETH 1
#define BT 2

//error bits
#define WELLFLOWRATE 0
#define HEATINGFLOWRATE 1
#define WELLWATERTEMP 2
#define CONDENSERPRESSURE 3
#define MINSUPERHEAT 4
#define UDNERROR 5
#define STARTPRESSDIFF 6
#define EEVERROR 7
#define MAXCOMPTEMP 8

//error bit masks
#define M_WELLFLOWRATE 1<<WELLFLOWRATE
#define M_HEATINGFLOWRATE 1<<HEATINGFLOWRATE
#define M_WELLWATERTEMP 1<<WELLWATERTEMP
#define M_CONDENSERPRESSURE 1<<CONDENSERPRESSURE
#define M_MINSUPERHEAT 1<<MINSUPERHEAT
#define M_UDNERROR 1<<UDNERROR
#define M_STARTPRESSDIFF 1<<STARTPRESSDIFF
#define M_EEVERROR 1<<EEVERROR
#define M_MAXCOMPTEMP 1<<MAXCOMPTEMP
#define M_MAXPOWER 1<<MAXCOMPTEMP

#define WAIT(x) if(now - timeStamp > x) timeStamp=now; else break;
#define ERROR_HANDLER(y) if(logError=(error & (y))) { heatingState=5; break;}

volatile static unsigned int wellWaterCounter=0, heatingWaterCounter=0, wellWaterPulses=0, heatingWaterPulses=0;
volatile static bool oneSecTrigger=false, firstEnergyPulse=true, newEnergyPulse=false, ethTrigger=true, configMode=false;
volatile static unsigned long energyPulsePeriod=0;
static long cStartDelay,errorRecoveryDelay,battSwitchOffDelay,postHeatCirc,preHeatCirc,ignoreEEVErr;
static int coolingWellFlowRate,minWellFlowRate,minHeatFlowRate,error,heaterrmask,cRestartRate,heatingWellFlowRate ;
static float minWellWaterOutTemp,maxCondPressure, evapPRaw=0,condPRaw=0;
static long lastEnergyPulsePeriod=0;
static unsigned long lastEnergyPulseTime=0, totalHeatingPulses=0;
static float wellFlowRate=0, heatingFlowRate=0, consumption=0, heatingPower=0, wellPower=0, evapPressure=0, condPressure=0, subCoolT;
static float superHeatT, maxComprT, maxPressureDiff;
static float superHeatLowLimit=0, superHeatHighLimit=0;
static char str[256];
static unsigned long currentMillis;
static bool heating=false,cooling=false,dhv=false,preStartDelay=false;
static unsigned char wellPumpPwm=0;
char cssGreen[] = "<g>", cssRed[] = "<r>",cssBlue[] = "<b>",cssYellow[] = "<y>",cssNone[]="";
static float owTemperature[EEOWMAXDEVS];
static boolean netHeating=false,netCooling=false;
static byte logChannel=0;

char owN0[] PROGMEM = "Air in";
char owN1[] PROGMEM = "Air out";
char owN2[] PROGMEM = "Air evap. gas";
char owN3[] PROGMEM = "Water evap. liquid";
char owN4[] PROGMEM = "Water evap. gas";
char owN5[] PROGMEM = "Well water in";
char owN6[] PROGMEM = "Well water out";
char owN7[] PROGMEM = "Condenser gas";
char owN8[] PROGMEM = "Condenser liquid";
char owN9[] PROGMEM = "Heating water in";
char owN10[] PROGMEM = "Heating Water out";
char owN11[] PROGMEM = "Electric box";
char owN12[] PROGMEM = "Compressor";
char owN13[] PROGMEM = "Suction temp";
char owN14[] PROGMEM = "Discharge temp";
const char *owNames[] PROGMEM = {
  owN0,owN1,owN2,owN3,owN4,owN5,owN6,owN7,owN8,owN9,owN10,owN11,owN12,owN13,owN14};

char owN0H[] PROGMEM = "Levego be";
char owN1H[] PROGMEM = "Levego ki";
char owN2H[] PROGMEM = "Levegos elpar. goz";
char owN3H[] PROGMEM = "Vizes elpar. folyadek";
char owN4H[] PROGMEM = "Vizes elpar. goz";
char owN5H[] PROGMEM = "Kutviz be";
char owN6H[] PROGMEM = "Kutviz ki";
char owN7H[] PROGMEM = "Kondenzator goz";
char owN8H[] PROGMEM = "Kondenzator folyadek";
char owN9H[] PROGMEM = "Futoviz visszatero";
char owN10H[] PROGMEM = "Futoviz eloremeno";
char owN11H[] PROGMEM = "Villamos doboz";
char owN12H[] PROGMEM = "Kompresszor";
char owN13H[] PROGMEM = "Szivocso";
char owN14H[] PROGMEM = "Nyomocso  ";
const char *owNamesH[] PROGMEM = {
  owN0H,owN1H,owN2H,owN3H,owN4H,owN5H,owN6H,owN7H,owN8H,owN9H,owN10H,owN11H,owN12H,owN13H,owN14H};

char netIP[] PROGMEM = "IP address";
char netNM[] PROGMEM = "netmask";
char netGW[] PROGMEM = "gateway";
char netDNS[] PROGMEM = "DNS server";
char netLOGIP[] PROGMEM ="log server IP address";
char netUI[] PROGMEM = "UI port (UDP)";
char netWEB[] PROGMEM = "WEB port (TCP)";
char netLOGPORT[] PROGMEM = "log server port";
const char *netNames[] PROGMEM = {
  netIP,netNM,netGW,netDNS,netLOGIP,netUI,netWEB,netLOGPORT};

char showInputStr[] PROGMEM = "Energy: %d\nHeating: %d\nDHV: %d\nCooling: %d\nEEV Error: %d\nWell flow: %d\nHeat flow: %d\nCondenser pressure A/D: %d\nEvaporator pressure1 A/D: %d\nEvaporator pressure2 A/D: %d\n4-20mA A/D: %d\nUDN2987 driver fault: %d";
char showOutputStr[] PROGMEM = "Well pump pwm: %d\nWell pump run: %d\nBattery on: %d\nHeating circulation pump: %d\nCompressor: %d\nAir evaporator fan: %d\nEEV: %d\nOUT7: %d\nOUT8: %d\nUDN2987 driver enable: %d";

char listSettingsStr1[] PROGMEM = 
"0; Maximum compressor restart rate (1/hour): %d\n"  //cRestartRate
"1; Compressor Start delay (s): %ld\n"    //cStartDelay
"2; Error recovery delay (s): %ld\n"    //errorRecoveryDelay
"3; Battery switch off delay (s): %ld\n"    //battSwitchOffDelay
"4; Post heating circulation duration (s): %ld";    //postHeatCirc
char listSettingsStr2[] PROGMEM =   
"5; Pre heating circulation duration (s): %ld\n"    //preHeatCirc
"6; EEV error ignore time (s): %ld\n"    //ignoreEEVErr
"7; Well water flowrate heating(L/h): %d\n"      //heatingWellFlowRate
"8; Well water flowrate cooling(L/h): %d\n"       //coolingWellFlowRate
"9; Minimum well flowrate (L/h): %d";            //minWellFlowRate
char listSettingsStr3[] PROGMEM =   
"10; Minimum heating water flowrate (L/h): %d\n"    //minHeatFlowRate
"11; Minimum well water discharge temperature (C): %d.%d\n"    //minWellWaterOutTemp
"12; Maximum condenser pressure (Bar): %d.%d\n"                //condPressureLimit
"13; Minimum superheat temperature (C): %d.%d";              //superHeatLowLimit
char listSettingsStr4[] PROGMEM =   
"14; Maximum superheat temperature (C): %d.%d\n"             //superheatHighLimit
"15; Maximum compressor temperature (C): %d.%d\n"
"16; Maximum starting pressure difference (Bar) %d.%d";


char answConfOn[] PROGMEM = "Config mode ON";
char answConfOff[] PROGMEM = "Config mode OFF";
char answ1w[] PROGMEM = "List of 1W devices:";
char answnet[] PROGMEM = "Networking settings:";
char answinput[] PROGMEM = "List inputs:";
char answoutput[] PROGMEM = "List outputs:";
char answResetUdn[] PROGMEM = "reset UDN2987:";
char answsettings[] PROGMEM = "Operating settings:";
char answNotInConfigMode[] PROGMEM = "Not in config mode!\nPrior to modify any settings please use \"c <password>\" to enter config mode.";
char answLogOn[] PROGMEM = "Display log messages enabled.";
char answLogOff[] PROGMEM = "Display log messages disabled.";
char answHeatingStart[] PROGMEM = "Start heating for: %d seconds.";
char answHeatingStop[] PROGMEM = "Stop heating.";
char answCoolingStart[] PROGMEM = "Start cooling for: %d seconds.";
char answCoolingStop[] PROGMEM = "Stop cooling.";

char answUnknown[] PROGMEM = "Unknown command or wrong parameters!\nFor help press \"?\"";
char answHelp[] PROGMEM =
"\"c <password>\" enter config mode.\n"
"\"c <anything>\" leave config mode\n"
"\"l 1w\" list 1W devices\n"
"\"l input\" list input states\n"
"\"l output\" list output states\n"
"\"l settings\" list operating settings";
char answHelp2[] PROGMEM =
"\"l net\" list networking settings\n"
"\"l on\" Display log messages\n"
"\"l off\" Stop display log messages\n"
"\"a <bus nr.> <1W nr.> <function nr.>\" assign 1W device to function";
char answHelp3[] PROGMEM =
"\"d <function nr.>\" unlink 1w device from function\n"
"\"n <IP setting nr.>\" <setting> set IP parameters\n"
"\"s <setting nr.> <value>\" set parameters\n"
"\"r udn\" reset UDN2987 output driver\n"
"\"reset!\" do a sowtware reset";


