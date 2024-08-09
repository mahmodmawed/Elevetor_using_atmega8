#define F_CPU 8000000UL
#include <avr/io.h>
#include <stdbool.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define PWM_MAX 2000  // Maximum PWM value for a 20ms period (1ms to 2ms)
uint8_t SPI_SlaveReceive(uint8_t Data);
void SPI_SlaveInit(void);
void Close_Door();
void Open_Door();
void set_PWM(uint16_t angle);
void init_PWM();
void ADC_Start();
void ADC_Init();
bool OVER_WEIGHT=false,IsOpen=false,Is_Emergcy=false;
uint8_t adcValue,T,R;
float v;
int weight;
int main(void)
{   DDRC=0x00;
	PORTC=0x07;
	DDRB |= (1 << DDB1);  // Set PB1/OC1A as output for PWM signal
    init_PWM(); 
	ADC_Init();
	SPI_SlaveInit(); 
	MCUCR|=(1<<ISC11)|(1<<ISC10)|(1<<ISC01);
    GICR|=(1<<INT0)|(1<<INT1);
	sei();
    while (1) 
    {
       ADC_Start();
	   
		if(!(PINC&(1<<PINC0))){
			   T&=~(0x07); 
		       T|=0x01; 

        }
   	    else if(!(PINC&(1<<PINC1))){
	   		  T&=~(0x07); 
				  T|=0x02;

   		}
	    else if(!(PINC&(1<<PINC2))){
			   T&=~(0x07); 
			   T|=0x04;

   	    }
		   if((R&0x02)==0x02){
			   Open_Door();
			   Is_Emergcy=true;  
		   }
		   else{
			   Is_Emergcy=false;
		   }
		   if((R&0x08)==0x08){
			   Close_Door();
			   
		   }
		
    }
}
void ADC_Init(){
		ADMUX=0x23;
		ADCSRA=0x87;
}
void ADC_Start(){
			ADCSRA |= (1 << ADSC);
			while(ADCSRA & (1<<ADSC));
			adcValue = ADCH;
			v=(adcValue*5)/255;
			weight=v*100;
			if(weight>=350){
				T|=0x08;
				OVER_WEIGHT=true;
			}
			else{
				T&=~(0x08);
				OVER_WEIGHT=false;
			}
	
}
void init_PWM() {
	// Configure Timer1 for Fast PWM, non-inverting mode
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11); // Prescaler 8
	ICR1 = 19999;  // Set TOP value for 20ms period
}
void set_PWM(uint16_t angle) {
	// Map angle (0 to 180 degrees) to PWM values (1000 to 2000)
	uint16_t pwm_value = 1000 + ((angle * (PWM_MAX - 1000)) / 180);
	
	// Set PWM duty cycle
	OCR1A = pwm_value;
}
void Open_Door(){
	
	set_PWM(0);   // Set servo angle to 0 degrees
	T&=~(0x10);
	IsOpen=true;
	_delay_ms(1000);  // Wait for 1 second
	
} 
void Close_Door(){
   
	set_PWM(180);  // Set servo angle to 90 degrees
	T |=0x10;
	IsOpen=false;
	if(Is_Emergcy){
		Open_Door();
	}
	_delay_ms(1000);// Wait for 1 second
			
}
void SPI_SlaveInit(void)
{
	DDRB &= ~((1 <<DDB2) | (1 << DDB3) | (1 << DDB5));
	DDRB |= (1 << DDB4);
	PORTB|=(1<<PORTB4);
	SPCR |= (1 << SPE);
}
uint8_t SPI_SlaveReceive(uint8_t Data)
{
	
	SPDR=Data;
	_delay_us(10);/* Wait for reception complete */
	while(!(SPSR & (1<<SPIF)));
	_delay_us(10);
	return SPDR;
}
ISR(INT0_vect){
	
	R=SPI_SlaveReceive(T);
	T&=0x18;
	
	
}
ISR(INT1_vect){
	ADC_Start();
	if( (IsOpen==true)&&((R&0x04)!=0x04)  && (Is_Emergcy==false) ){
		Close_Door();
		
	}
	else if(   (IsOpen==false)  &&  ((R&0x01)!=0x01) ){
		Open_Door();
		
	}
	
}