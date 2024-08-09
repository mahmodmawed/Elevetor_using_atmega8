/*
 * MainController.c
 *
 * Created: 12/10/2023 12:02:39 PM
 * Author : mahmo
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdbool.h>
uint8_t SPI_MasterTransmit1(uint8_t);
uint8_t SPI_MasterTransmit2(uint8_t);
uint8_t SPI_MasterTransmit3(uint8_t);
void SPI_MasterInit(void);
void Change(void);
uint8_t Data1=0,Data2=0,Data3,R1=0x00,R2=0x00,R3=0x00,IN_REQUST_FlOOR=0,OUT_REQUST_FlOOR=0,FLOOR=1;
bool Person_Moving=false,Over_Weight=false,Door_Close=false,Is_Moving=false,EMERGENCY=false,FIRE=false,ARRIVE=false;

int main(void)
{
	
	DDRD=0xFF;
	DDRC=0x01;
SPI_MasterInit();
		         _delay_ms(200);
    /* Replace with your application code */
    while (1) 
    {   
		
		
		
		
		Data1=SPI_MasterTransmit1(R1);
		PORTD=Data1;
		if((Data1&0x01)==0x01){
			IN_REQUST_FlOOR=1;
		}
		else if((Data1&0x02)==0x02){
	        IN_REQUST_FlOOR=2;
        }		
        else if((Data1&0x04)==0x04){
	        IN_REQUST_FlOOR=3;
        }	
        if((Data1&0x08)==0x08){
	        Person_Moving=true;
        }
		else{
			Person_Moving=false;
		}
		_delay_ms(100);
				 
				 
        
		Data2=SPI_MasterTransmit2(R2);
		
		PORTD=Data2;
        if((Data2&0x01)==0x01){
	        OUT_REQUST_FlOOR=1;
        }
        else if((Data2&0x02)==0x02){
	        OUT_REQUST_FlOOR=2;
        }
        else if((Data2&0x04)==0x04){
	        OUT_REQUST_FlOOR=3;
        }
		if((Data2&0x08)==0x08){
			Over_Weight=true;
		}
		else{
			Over_Weight=false;
		}
        if((Data2&0x10)==0x10){
	        Door_Close=true;
         }
		 else{
			 Door_Close=false;
		 }
		_delay_ms(100);
		
       
	    Data3=SPI_MasterTransmit3(R3);			        
	    
        PORTD=Data3;
		if((Data3&0x04)==0x04){
			FLOOR=3;
		}
		else if((Data3&0x02)==0x02){
			FLOOR=2;
		}
		else if((Data3&0x01)==0x01){
			FLOOR=1;
		}
		if((Data3&0x08)==0x08){
			Is_Moving=true;
		}
		else{
			Is_Moving=false;
		}
		if((Data3&0x10)==0x10){	
			if(EMERGENCY){
			EMERGENCY=false;
			}
			else{
		  EMERGENCY=true;
			}
		}
		if((Data3&0x20)==0x20){
			FIRE=true;	
		}
		else{
			FIRE=false;	
		}
		if((Data3&0x40)==0x40){
			ARRIVE=true;
		}
		else{
		ARRIVE=false;	
		}
		
        _delay_ms(100);
		Change();

				 
				 	 
    }
}
void Change(void){
	///////////////////////////////////////////////R1//////////////////////////////
	if(FLOOR==1){
		R1&=~(0x07);
		R1|=0x01;
	}
	else if(FLOOR==2){
		R1&=~(0x07);
		R1|=0x02;
	}
	else if(FLOOR==3){
		R1&=~(0x07);
		R1|=0x04;
	}
	if(Over_Weight){
		R1|=0x08;
	}
	else{
		R1&=~(0x08);
	}
	if(FIRE){
		R1|=0x10;
	}
	else{
		R1&=~(0x10);
	}
	if(EMERGENCY){
		 R1|=0x20;
		 PORTC=0x01;
	}
	else{
		  R1&=~(0x20);
		  PORTC=0x00;
	}
	if(Is_Moving){
		R1|=0x40;
	}
	else{
		R1&=~(0x40);
	}
	if(Door_Close){
		R1|=0x80;
	}
	else{
		R1&=~(0x80);
	}

	//////////////////////////////////////////////R2//////////////////////////////

	if(Is_Moving){
		R2|=0x01;
	}
	else{
		R2&=~(0x01);
	}
	if((EMERGENCY)||(ARRIVE)||(FIRE)||(Person_Moving&&(Is_Moving==false))){
		R2|=0x02;
	}
	else{
		R2&=~(0x02);
	}
	if(Person_Moving){
		R2|=0x04;
	}
	else{
		R2&=~(0x04);
	}
	if(((IN_REQUST_FlOOR)!=0||(OUT_REQUST_FlOOR)!=0)&&!(Door_Close)){
		R2|=0x08;
		
	}
	else
	{
		
		R2&=~(0x08);
	}
	/////////////////////////////////////////////R3//////////////////////////////
     if((FLOOR==OUT_REQUST_FlOOR)&&(Is_Moving==false)){
	 OUT_REQUST_FlOOR=0;
	 }
	 if((FLOOR==IN_REQUST_FlOOR)&&(Is_Moving==false)){
     IN_REQUST_FlOOR=0;
	 }
	if((Person_Moving==false)&&(((IN_REQUST_FlOOR)!=0)||((OUT_REQUST_FlOOR)!=0))&&(Over_Weight==false)&&(FIRE==false)&&(EMERGENCY==false))
	{
		if(FLOOR==1){
			if((OUT_REQUST_FlOOR==2)||(IN_REQUST_FlOOR==2)){
				R3|=0x02;
				if(IN_REQUST_FlOOR==2){
					IN_REQUST_FlOOR=0;
				}
				if(OUT_REQUST_FlOOR==2){
					OUT_REQUST_FlOOR=0;
				}
			}
			else{
				R3|=0x04;
				if(IN_REQUST_FlOOR==3){
					IN_REQUST_FlOOR=0;
				}
				if(OUT_REQUST_FlOOR==3){
					OUT_REQUST_FlOOR=0;
				}				
			}
		}
		else if(FLOOR==2){
			if(IN_REQUST_FlOOR==1){
				R3|=0x01;
				IN_REQUST_FlOOR=0;
			}
			else if(IN_REQUST_FlOOR==3){
				R3|=0x04;
				IN_REQUST_FlOOR=0;
			}
			else if(OUT_REQUST_FlOOR==3){
				R3|=0x04;
				OUT_REQUST_FlOOR=0;
			}
			else if(OUT_REQUST_FlOOR==1){
				R3|=0x01;
				OUT_REQUST_FlOOR=0;
			}else{}
		}
		else if(FLOOR==3){
			if((OUT_REQUST_FlOOR==2)||(IN_REQUST_FlOOR==2)){
				R3|=0x02;
				if(IN_REQUST_FlOOR==2){
					IN_REQUST_FlOOR=0;
				}
				if(OUT_REQUST_FlOOR==2){
					OUT_REQUST_FlOOR=0;
				}
			}
			else{
				R3|=0x01;
				if(IN_REQUST_FlOOR==1){
					IN_REQUST_FlOOR=0;
				}
				if(OUT_REQUST_FlOOR==1){
					OUT_REQUST_FlOOR=0;
				}
			}
		}
		
	}
	else{
		R3&=~(0x07);
	}
}
void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDRB = (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3)|(1<<DDB5);
	DDRB&=~(1<<DDB4);
	PORTB|=(1<<PB0)|(1<<PB1)|(1<<PB2)|(1<<PB3)|(1<<PB4);
	/* Enable SPI, Master, set clock rate fck/16 */
	SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0);

}
uint8_t SPI_MasterTransmit1(uint8_t cData)
{   uint8_t x=0;
	/* Start transmission */
	PORTB&=~(1<<PB0);
	_delay_us(10);
	SPDR = cData;

	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
    x=SPDR;
	PORTB|=(1<<PB0);
	_delay_us(10);
	return x;
}
uint8_t SPI_MasterTransmit2(uint8_t cData)
{ uint8_t x=0;
	/* Start transmission */
	PORTB&=~(1<<PB1);
	_delay_us(10);
	SPDR = cData;

	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));
	x=SPDR;
	PORTB|=(1<<PB1);
	_delay_us(10);
	return x;
}
uint8_t SPI_MasterTransmit3(uint8_t cData)
{
	 uint8_t x=0;
	 /* Start transmission */
	 PORTB&=~(1<<PB2);
	 _delay_us(10);
	 SPDR = cData;

	 /* Wait for transmission complete */
	 while(!(SPSR & (1<<SPIF)));
	 x=SPDR;
	 PORTB|=(1<<PB2);
	 _delay_us(10);
	 return x;
}
