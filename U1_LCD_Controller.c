/*
 * LCD4B.c
 *
 * Created: 24/12/2023 5:48:50 AM
 * Author : Dell
 */ 
#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
void cmd_4bit(unsigned char);
void data_4bit(unsigned char);
void String_4bit(char *str);
void Init_4bit();
void SPI_SlaveInit(void);
uint8_t R=0x00,T;
int main(void)
{
  
  SPI_SlaveInit();
  
  DDRC = 0x00;
  PORTC |= (1<<0) | (1<<1) | (1<<2) | (1<<3);
  ///////////////////////////////////////////////////
  
  DDRD = 0xff;
  DDRD&=~(1<<PD3);
   MCUCR|=(1<<ISC11);
    GICR|=(1<<INT1);
  sei();
  Init_4bit();
    while (1) 
    {
   if( (PINC & (1<<0)) == 0 )
   {
   T&=~(0x07); 
     T |= 0x01; ///////

   }
   if( (PINC & (1<<1)) == 0 )
   {
     T&=~(0x07);
     T |= 0x02; ///////
     
   }
   
   if( (PINC & (1<<2)) == 0 )
   {
     T&=~(0x07);
     T |= 0x04; ///////
     
   }
   
   if( (PINC & (1<<3)) == 0 )
   {
	if(((R & 0x40) ==0x00)&&((R & 0x80) ==0x00)){
     _delay_ms(1);

   cmd_4bit(0xc0);
    T = 0x08; ///////
    String_4bit("STEP AWAY");
   _delay_ms(100);
   }
   }
   if ((R & 0X01) == 0x01) {
      cmd_4bit(0x01); //clear display
    cmd_4bit(0x80);
      String_4bit("   Floor: 1"); //display
          _delay_ms(100);
        }
   if ((R & 0X02) == 0x02) {
          cmd_4bit(0x01); //clear display
      cmd_4bit(0x80);
          String_4bit("   Floor: 2"); //display
          _delay_ms(100);
        }
   if ((R & 0X04) == 0x04) {
          cmd_4bit(0x01); //clear display
      cmd_4bit(0x80);
          String_4bit("   Floor: 3"); //display
          _delay_ms(100);
        }
   if ((R & 0x10) == 0x10) {
     cmd_4bit(0xc0); //clear display
     String_4bit("THERE IS FIRE<!>");
     _delay_ms(100);
   }
   else if ((R & 0X20) == 0x20) {
	      cmd_4bit(0xc0);
	      String_4bit("   EMERGENCY");
	      _delay_ms(100);
      }
   else if ((R & 0X08) == 0x08) {
             cmd_4bit(0xc0);
             String_4bit("   Too Heavy");
             _delay_ms(100);
           }

    }
}

void cmd_4bit(unsigned char cmd)
{
  unsigned char a,b;
  a = cmd & 0xf0; //first 4-bit
  PORTD = a | 0X04; //RS = 0, RW = 0, E = 1 PB0=0 PB1=0 PB2=1
  _delay_ms(1);
  PORTD &= ~(0X04); //clear enable pin PB2=0
  
  b = cmd<<4;  //last 4-bit
  PORTD = b | 0x04;
  _delay_ms(1);
  PORTD &= ~(0X04);
}

void data_4bit(unsigned char x)
{
  unsigned char a,b;
  a = x & 0xf0;
  PORTD = 0x05 | a; //RS1 RW0 E1
  _delay_ms(1);
  PORTD &= ~(0x04); //E0
  
  
  b = x<<4;
  PORTD = 0x05 | b; //RS1 RW0 E1
  _delay_ms(1);
  PORTD &= ~(0x04); //E0
}
void String_4bit(char *str){
  
  int i;
  for(i=0;str[i]!=0;i++)    /* Send each char of string till the NULL */
  {
    data_4bit(str[i]);
  }
}
void Init_4bit(){
    cmd_4bit(0x02); //return home
    cmd_4bit(0x28); //4bit 5x7 dots
    cmd_4bit(0x0e);  //display on blink off
    cmd_4bit(0x06); //entry mode set
    cmd_4bit(0x01);  //clear display
    cmd_4bit(0x80); //set ddram adress
    
}
void SPI_SlaveInit(void)
{
  DDRB &= ~((1 <<DDB2) | (1 << DDB3) | (1 << DDB5));
  DDRB |= (1 << DDB4);
  PORTB|=(1<<PB4);
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
ISR(INT1_vect){
  
  R=SPI_SlaveReceive(T);
  T=0x00;
  
  
}
