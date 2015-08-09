/*****************************************************************************
 * Program Author:  Ethan Grant
 * Student ID:      12011576
 * File name:       DuckShoot.c
 * Course code:     UFCETS-20-1 C Programming              
 * Date created:    29/11/2012
 * 
 * Description: This program is a Duck shoot game made for the Altera DE0-Nano 
 *              board. To win the game you must turn off all the LED's(Ducks)
 *              by shooting them using "Button 2" ,the player will lose if all
 *              LED's are lit. In this game the player will always aim at the 
 *              least significant bit "LED 0". Game speed can be changed using
 *              switches 6 and 7. The direction of the LED's can be changed
 *              using switch 1. 
 * **************************************************************************/
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "alt_types.h"


int main (void) __attribute__ ((weak, alias ("alt_main")));

void seg7_out (int num);
void empty_loop (void);
void end_game (int led);

#define switch0 0x01
#define switch6 0x40
#define switch7 0x80

#define speed1 150000
#define speed2 300000
#define speed3 500000
#define speed4 750000

int alt_main(void)
{
    alt_u16 led = 0x2AA, switches;
    alt_u16 button = 0x00;
    alt_16 speedswitches;
    alt_u32 lose = 0xC7C09286;
    alt_u32 end = 0x86C8A17F;
    int count = 0;
    int speed = speed1;
    volatile int i;
    
    while(1)
        {
            /*Read switches*/
            switches = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
            switches = (switches & 0x0C1);  /*Mask unused switches*/
            speedswitches = (switches >> 6) & 0x03; 
            led = (led & 0x3FF);    /*Mask out unused bits*/
            switch(speedswitches)
            {
                case 3          : (speed = speed4); break;
                case 2          : (speed = speed3); break;
                case 1          : (speed = speed2); break;
                case 0          : (speed = speed1); break;
            }
            
            /*Read button and check for shot*/
            button = IORD_ALTERA_AVALON_PIO_DATA(BUTTONS_BASE);
            button = (~button & 0x04);
            alt_u8 state = 0;
            if ((button & 0x04) && !state)  /*Inverts button 2*/
            {
                state = 1;
            }    
                if(state == 1)
                {
                    if(led & 0x01)          /*Check if least significant bit is on*/
                        {
                            led -= 0x01;    /*If least significant bit is on turn it off*/
                            state = 0;
                            count++;
                        }
                    else if(!(led & 0x01))  /*Check if least significant bit is off*/
                        {
                            led += 0x01;    /*If least significant bit is off turn it on*/
                            state = 1;
                            count++;
                        }
                }
                IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, led);
                empty_loop();
                
               
                seg7_out(count);    /*Display seven segment with count*/
                    
                empty_loop();
                /*Read LED's and set direction*/
                IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, led);
           if (switches & switch0)
                { 
                    if (led & 0x001)    /*check if least significant bit is on*/
                        {
                            led = led >> 1;  
                            led += 0x200;   /*Turn most significant bit on*/
                            empty_loop();
                        }
                     else
                        {
                            led = led >> 1;
                            empty_loop();
                        }
               }
           else
               { 
                    if (led & 0x200)    /*check most significant bit is on*/
                        {
                            led = led << 1;
                            led += 0x001;   /*Turn on least significant bit*/
                        }
                        else
                        {
                            led = led << 1; 
                        }
               }
            empty_loop();
            IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, led);
            if (led == !0x3FF)  /*Game is won*/
                {   
                    IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, end );
                    break;
                }
            else if(led == 0x3FF)   /*Game is lost*/
                {
                    IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, lose );
                    break;
                }
            
            i = 0;
            while (i < speed)
            i++;
        }
    return 0;
}

/*****************************************************************************
 * Function name: seg7_out()
 * 
 * Description: This function creates a count on the seven segment from 0-9999
 *              the counter increments everytime the button is pressed. 
 * **************************************************************************/
void seg7_out (int num)
{
   alt_u8 *p;
   alt_u32 display = 0xFFFFFF;
   alt_u8 values[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90}; /*Hex values 1 - 9*/
     
   p = (unsigned char *) &display;
  
   *p = values[num % 10];               /*4th segment*/
   *(p + 1) = values[(num/10) % 10];    /*3rd segment*/
   *(p + 2) = values[(num/100) % 10];   /*2nd segment*/
   *(p + 3) = values[(num/1000) % 10];  /*1st segment*/
   
    IOWR_ALTERA_AVALON_PIO_DATA(SEG7_BASE, display);
}
/*****************************************************************************
 * Function name: empty_loop()
 * 
 * Description: This function is just an empty for loop used to create delays
 * **************************************************************************/
void empty_loop (void)
{
    int p = 0;
    for (p = 0; p < 10000; p ++)
    {
    }
}