const int BTNPIN1 = 3;
const int BTNPIN2 = 4;
const int BTNPIN3 = 5;

const int KLPPIN = 9; 
const int KVPPIN = 10; 

int sensorValue = 0;       
int btn =0; 
int clock = 250;

//counting the time 
int timer(long nowtemp,long holdtemp);

//the first one the vaccum an
struct data_type { int vPWM; int lPWM; int clock;};


data_type demo [1] = { { 64, 128, 250 }};
data_type mydata1 [5] = { { 64, 96, 200 }, { 64, 96, 800 }, { 64, 96, 200 }, 
                          { 64, 96, 800 }, { 64, 96 ,200 }};
data_type mydata2 [5] = { { 64, 64, 500 }, { 64, 128, 250 }, { 64, 128, 500 },
                          { 64, 64, 800 }, { 64, 128 ,500 }};
data_type mydata3 [5] = { { 64, 64, 200 }, { 64, 64, 200 }, { 64, 64, 200 }, 
                          { 64, 64, 200 }, { 64, 64 ,200 }};
// for different mode 
bool controller(int btn); 
//output the PWM with our data 
void pump (data_type* data ,int length); 
//the KLPval is no less then 32 and the KVPval is no less then 64 
void analogoutput(int KVPval, int KLPval);

//print case to serial pot for debugging
void printcase(int btn);
void setup()

{
  // set the serial port receive cycle 
  Serial.begin(9600); 
  //set the pin mode for arduino
  pinMode(BTNPIN1, INPUT);
  pinMode(BTNPIN2, INPUT);
  pinMode(BTNPIN3, INPUT);
  pinMode(KLPPIN, OUTPUT);
  pinMode(KVPPIN, OUTPUT);;
}

void loop() {
   while(Serial.available()==0 ){  
    //change the digital input into the case 
    btn = 100*digitalRead(BTNPIN1)+10*digitalRead(BTNPIN2)+digitalRead(BTNPIN3);
    controller(btn);  
  }
  btn  = Serial.parseInt();
}

int timer (long nowtemp,long holdtemp){
  long nowtime = nowtemp;
  static long starttime = nowtime;
  static int count = nowtime;
  long holdtime = holdtemp;
  if(nowtime > holdtime+starttime){
    count++;
    starttime = nowtime; 
  }
  return count; 
}

bool controller(int btn){

  switch (btn) {
    // close both pump
    case 0: 
      analogoutput(0, 0);
      return 0; 
    // only open liquid pump 
    case 1:  
      analogoutput(0, 255);
      return 0;
    // only open air pump 
    case 10: 
      analogoutput(255, 0);
      return 0;
    // open both pump
    case 11: 
      analogoutput(255, 255);
      return 0;  
    // set switch between air pump and liguid pump
    case  100: 
      pump(demo, sizeof(demo)/sizeof(data_type));
      return 1;
    //output data in one of three data sets  
    case 101:
      pump(mydata1, sizeof(mydata1)/sizeof(data_type));
      return 1;
    case 110:
      pump(mydata2, sizeof(mydata2)/sizeof(data_type));
      return 1;
    case 111:
      pump(mydata3, sizeof(mydata3)/sizeof(data_type));
      return 1;
    default:
      return 1;
  }
 
}

void analogoutput(int KVPval, int KLPval){
  analogWrite(KVPPIN, 255-KVPval);
  analogWrite(KLPPIN, 255-KLPval); 
}

void pump(data_type *data, int length){  
  Serial.println(length); 
  for (int i = 0; i < length; i++) {
    int trigger = timer(millis(),data[i].clock);
    while (trigger+2 > timer(millis(),data[i].clock)){
      
      if (timer(millis() ,data[i].clock) % 2 == 1)
        analogoutput(data[i].vPWM, 0);
      else 
        analogoutput(0, data[i].lPWM);     
    }  
  }
}

void printcase(int btn){
  Serial.println(btn);  
  switch (btn) {
    case 11:
      Serial.println("reset");    
      break;
    case 0:
      Serial.println("halt");    
      break;
    case 100:
      Serial.println("demo");
      break;
    case 1:
      Serial.println("liquid");
      break;
    case 10:
      Serial.println("vacuum");
      break;
    default:
      Serial.println("running");
      break;
  } 
}
