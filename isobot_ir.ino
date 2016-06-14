//-------------------info about bits-----------------------

#define totallength 22 //number of highs/bits 4 channel +18 command
#define channelstart 0
#define commandstart 4 //bit where command starts
#define channellength 4
#define commandlength 18
//---------determined empirically--------------
#define headerlower 2300 //lower limit
#define headernom 2550 //nominal
#define headerupper 2800 //upper limit
#define zerolower 300
#define zeronom 500 //380 //nominal
#define zeroupper 650
#define onelower 800
#define onenom 1000//850 //nominal
#define oneupper 1100
#define highnom 630
//---------------------pin assignments--------------
#define TXpin 7
#define RXpin 2 //doesnt use interrupts so can be anything
//----------------------variables----------------------
#define countin 1048576
boolean bit2[totallength];
unsigned long buttonnum;
char msg = ' ';
unsigned long x = 0;
unsigned long count = countin;
unsigned long buf = 0;

void setup() {
 Serial.begin(38400);
 pinMode(RXpin, INPUT);
 pinMode(TXpin, OUTPUT);
}
void loop() {
 while (Serial.available() > 0){ //Serial control
   //msg = Serial.read()
   char switcher= (byte) Serial.read();
   if (switcher == '\r') {
     Serial.print("Break: ");
     Serial.println(buf,HEX);
     buttonwrite(TXpin, buf);
     buf = 0;
     count = countin;
     delayMicroseconds(300);
     break;
   }
   x = SerialReadHexDigit(switcher);
  
   x = x * count;
   //Serial.println(x,BIN);
   buf = buf + x;
   count = count / 16;
   //Serial.print("Buffer: ");
   //Serial.println(buf,HEX);


 } // end while

}
int SerialReadHexDigit(char digit)
{
 //byte c = WaitAndRead();
 byte c = (byte) digit;
 if (c >= '0' && c <= '9') {
 return c - '0';
 } else if (c >= 'a' && c <= 'f') {
 return c - 'a' + 10;
 } else if (c >= 'A' && c <= 'F') {
 return c - 'A' + 10;
 } else {
 return -1; // non-hexadecimal character
 }
}
void ItoB(unsigned long integer, int length){
//needs bit2[length]
 Serial.println("ItoB");
 for (int i=0; i<length; i++){
   if ((integer / power2(length-1-i))==1){
   integer-=power2(length-1-i);
   bit2[i]=1;
   }
 else bit2[i]=0;
 Serial.print(bit2[i]);
 }
 Serial.println();
}
unsigned long power2(int power){ //gives 2 to the (power)
 unsigned long integer=1; //apparently both bitshifting and pow functions had problems
 for (int i=0; i<power; i++){ //so I made my own
 integer*=2;
 }
 return integer;
}
void buttonwrite(int txpin, unsigned long integer){
//must be full integer (channel + command)
 ItoB(integer, 22); //must have bit2[22] to hold values
 oscWrite(txpin, headernom);
 for(int i=0;i<totallength;i++){
 if (bit2[i]==0) delayMicroseconds(zeronom);
 else delayMicroseconds(onenom);
 oscWrite(txpin, highnom);
 }
 delay(205);
}
void oscWrite(int pin, int time) { //writes at approx 38khz
 for(int i = 0; i < (time / 26) - 1; i++){
//prescaler at 26 for 16mhz, 52 at 8mhz, ? for 20mhz
 digitalWrite(pin, HIGH);
 delayMicroseconds(10);
 digitalWrite(pin, LOW);
 delayMicroseconds(10);
 }
}
