 
/*
 * fina;_project_lap.c
 *
 * Created: 12/6/2023 6:33:21 PM
 * Author : SamiP
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
void SPI_SlaveInit(void);
uint8_t tx = 0x00;
uint8_t rx = 0x00;
int cur_floor = 0;
bool Emergncy=false;

int main(void)
{
    DDRD=0x00; //input port
  DDRC=0xFF; //output port
  PORTD |= (1 << PIND4) | (1 << PIND2) | (1 << PIND3) | (1 << PIND5); //pull-up resistors
  
  MCUCR |= (1 << ISC11) | (1 << ISC01);
  GICR |= (1 << INT1)|(1<<INT0); //int enable
  sei();
  SPI_SlaveInit();
    while (1) 
    {
if(Emergncy==false){     
      if ((rx & 0x01)==0x01) // floor 0
      {
        if ( cur_floor == 1 )
        {
        PORTC |= (1 << PINC1);
    tx |= (1 << 3);
        _delay_ms(500);
         PORTC &= 0xFD;
    tx &= 0b11110111;
	if(Emergncy==false){
        cur_floor = 0;
        tx = 0b01000001;
	}
        }
        else if (cur_floor == 2 )
        {
          PORTC |= (1 << PINC1);
      tx |= (1 << 3);
                  _delay_ms(500);
                  if(Emergncy==false)
                  {
	                  cur_floor = 1;
	                  tx = 0b01000010;
                  }
                  _delay_ms(500);
          PORTC &= 0xFD;
      tx &= 0b11110111;
	  	if(Emergncy==false){
          cur_floor = 0;
          tx = 0b01000001;
		  }
        }
      }
      else if ((rx & 0x02)==0x02) // floor 1
      {
        if ( cur_floor == 0 )
        {
        PORTC |= (1 << PINC0);
    tx |= (1 << 3);
        _delay_ms(500);
        PORTC &= 0xFE;
    tx &= 0b11110111;
		if(Emergncy==false){
        cur_floor = 1;
        tx = 0b01000010;
		}
        }
        else if (cur_floor == 2 )
        {
          PORTC |= (1 << PINC1);
      tx |= (1 << 3);
          _delay_ms(500);
          PORTC &= 0xFD;
      tx &= 0b11110111;
	  	if(Emergncy==false){
          cur_floor = 1;
          tx = 0b01000010;
		  }
        }
      }
      else if ((rx & 0x04)==0x04)  //floor 2
      {
        if ( cur_floor == 0 )
        {
        PORTC |= (1 << PINC0);
    tx |= (1 << 3);
	
	
        _delay_ms(500);
		if(Emergncy==false)
		{
	        cur_floor = 1;
	        tx = 0b01000010;
		}
		_delay_ms(500);
        PORTC &= 0xFE;
    tx &= 0b11110111;
		if(Emergncy==false){
        cur_floor = 2;
        tx = 0b01000100;
		}
        }
        else if (cur_floor == 1 )
        {
          PORTC |= (1 << PINC0);
      tx |= (1 << 3);
          _delay_ms(500);
          PORTC &= 0xFE;
      tx &= 0b11110111;
	  	if(Emergncy==false){
          cur_floor = 2;
          tx = 0b01000100;
		  }
        }
      }
}
      if (!(PIND & (1 << PIND4)))  // smoke detector
      {
          _delay_ms(100);
          PORTC |= (1 << PINC5);
          tx = (1 << 5);
          _delay_ms(100);
          PORTC &= 0b11011111;
      }
      if (!(PIND & (1 << PIND5))) // bell button
      {
        tx |= (1 << 4);
      }

  }
  return 0;
}

ISR(INT1_vect)// force stop button  
{

  PORTC &= 0xFC;
  tx |= (1 << 4);
  tx &= 0b11110111;
  if(Emergncy==false){
	  Emergncy=true;
  }else{
	  Emergncy=false;
  }
  
  
}

void SPI_SlaveInit(void)
{
  DDRB &= ~((1 <<DDB2) | (1 << DDB3) | (1 << DDB5));
  DDRB |= (1 << DDB4);
  PORTB|=(1 << PORTB4);
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
  
  rx=SPI_SlaveReceive(tx);
  tx=0x00;
  
  
}
