#include <avr/interrupt.h>

#define PinInt2 digitalRead(dirPin)

volatile int			counts		=	0;
volatile unsigned long	last_micros	=	0;
volatile long			rpm			=	0;;
static int				testpin		=	11;
static int				dirPin		=	7;
volatile int			dir			=	3;
volatile int			dir1		=	0;
volatile int			dir_before	=	0;
volatile int			stats		=	0;

volatile int rpm_array[4] = {0,0,0,0};
volatile long			counts_mean;



void setup()
{
	DDRB|=0x08; //PIN 11
	
	Serial.begin(115200);
	Serial.println("Ready for Speed");
	delay(1000);
	//attachInterrupt(0,counter_interrupt,RISING);

	
	
	
	
	pinMode(testpin,OUTPUT);
	pinMode(dirPin,INPUT);
	
	
	EICRA |= (1<< ISC00) | (1<< ISC01);	// set INT0 (Pin 2) to trigger on rising edge
	EIMSK |= (1<< INT0);				// Turns on INT0
	
	//digitalWrite(testpin,LOW);
	sei();
	last_micros=micros();
	
}

void loop()
{

	//digitalWrite(testpin,HIGH);
	
	while(micros()-last_micros >=10000) // counts for 0,01 s measured timespan 0,0099975 s
	{
		//digitalWrite(testpin,LOW);
		dir1=dir-3;
		if (dir1!=dir_before & stats<=1)
		{
			stats++;
			dir1=-1*dir1;
		}
		else if (dir1!=dir_before & stats>1)
		{
			stats=0;
		}
		 
		if (dir1==-1 || dir1==1 || dir1==0)
		{
			/*
			rpm = dir1*counts * 5.8608402101; // (counts)*(1/measured timespan)*60s/1024 //
			
			*/
			//5 Sample Moving Average To Smooth Out The Data
			rpm_array[0] = rpm_array[1];
			rpm_array[1] = rpm_array[2];
			rpm_array[2] = rpm_array[3];
			rpm_array[3] = counts;
			//rpm_array[4] = rpm_array[5];
			//rpm_array[5] = counts;
			//Last 5 Average RPM Counts Equals....
			counts_mean = ((rpm_array[0] + rpm_array[1] + rpm_array[2] + rpm_array[3])>>2);  // rightshift 2 equals division by 4
			rpm = long(dir1)*long(counts_mean) * 5861; // (counts)*(1/measured timespan)*60s/1024 //
			Serial.print(counts);
			Serial.print(", ");
			Serial.print(dir1);
			Serial.print(", ");
			Serial.println(rpm,DEC);
			counts=0;
			rpm=0;
			dir=3;
			dir_before=dir1;
			last_micros=micros();
			
			
		}
		
		
	}
	
}

/*
void counter_interrupt()
{
counts++;
if (PIND & 0x80)
{
dir=-1;
}
else if(!(PIND & 0x80))
{
dir=1;
}
else
{
dir=0;
}

}
*/

ISR (INT0_vect)		// external pin interrupt on rising edge counts the interrupts
{
	int p = PIND & 0x80;	//Pin 7
	PORTB |=0x08;			//Pin 11 as trigger out
	

	
	if (p)		//check Encoder channel B for status for direction detection
	{
		dir=4;
		//counts++;
		//PORTB &= !0x08;
		//return;
	}
	else if(!p)
	{
		dir=2;
		//counts++;
		//PORTB &= !0x08;
		//return;
	}
	else
	{
		dir=3;
		
	}
	counts++;
	
	

	
	PORTB &= !0x08;
}