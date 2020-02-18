/*Место для пояснений*/
#include "1886ve4d.h"
#include "int17xxx.h"
#pragma origin 0x8

typedef unsigned char uc;

const uc Translate_num_to_LED[10] = {
//  0,	  1,	2,	  3,	4,	  5,	6,	  7,	8,	  9.
	0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
// Cells for receiving messages
uc a, b, c, d;
uc LED [5];
int led_active;
uc mode;

bit flag_send_mode;
bit flag_rw; // 0 read, 1 write
bit flag_msg_received;
bit flag_manual_auto;

uc count_receive_data;
uc error_code;
uc error_code_interrupt;

interrupt iServer(void)
{
    multi_interrupt_entry_and_save

    PERIPHERAL_service:
        PEIF = 0;
        interrupt_exit_and_restore
    TMR0_service:
    	// save on demand: PRODL,PRODH,TBLPTRH,TBLPTRL,FSR0,FSR1
    	/* process Timer 0 interrupt */
    	// T0IF is automatically cleared when the CPU vectors to 0x10
    	// restore on demand: PRODL,PRODH,TBLPTRH,TBLPTRL,FSR0,FSR1
        T0IF = 0;
        interrupt_exit_and_restore
    T0CKI_service:
        T0CKIF = 0;
        interrupt_exit_and_restore
    INT_service:
        INTF = 0;
        interrupt_exit_and_restore
}
/******************/
#include "math24.h"
#include "Functions.h"

void main(void)
{
	Reg_Start_up();
	
	uc temp = 0;
	int d_line = 0;	// Working indicator number
    uc led_blink = 0;
    uc led_temp = 0;
    uc count0 = 0, count1 = 0;
    uc mode = 0;
    led_active = 0;
    /*
    uc engine_cycle = 0;
    while (1)
    {
		clrwdt();
		
		// Engine-
		PORTD = 0;
		engine_cycle ++;
		if (engine_cycle > 2)
			engine_cycle = 0;
		temp = 0x01 << engine_cycle;
		PORTD |= temp;
		for (temp = 0; temp < 100; temp ++){};
		// -------
	}*/
	
	while(1)
	{
		clrwdt();
		// PORT D --------------------------------------------------------------
		temp = 0x20 << d_line; // 08
		//temp |= Show_ERROR (); //d_work_light;
		// Code entered for model --
		PORTC = 0;
		// -------------------------
		PORTD = temp;
		
		// PORT C --------------------------------------------------------------
		if (d_line == led_active)	// For two iterations, the selected
		{							// indicator will be turned off
			led_temp ++;
			if (led_temp > 254)
			{
				led_temp = 0;
				led_blink ++;
			}
			if (led_blink > 254)		// Delay for blinking
				led_blink = 0;
		}
		
		//if ((d_line == led_active) && (led_temp < 200))// (led_blink & 0x08)
		if ((d_line == led_active) && (led_blink & 0x08))
			temp = 0xFF; //0
		else
		{
			temp = LED[(int)d_line];// The order of indicators is determined 
									// here.
			temp = Translate_num_to_LED[(int)temp];
		}
		PORTC = temp;
		
		
		
		// Code entered for model --
		PORTC ^= 0xFF;
		PORTD ^= 0xE0;
		
		if (mode == 0)
			count0 ++;
		
		if (count0 == 255)
			count1++;
			
		if (count1 == 8)
		{
			count1 = 0;
			LED [0] ++;
			if (LED[0] > 9)
			{
				LED[0] = 0;
				LED[1] ++;
			}
			if (LED[1] > 9)
			{
				LED[1] = 0;
				LED[2] ++;
			}
			if (LED[2] > 9)
				LED[2] = 0;
		}
		
		DDRE = 0xF0;
		PORTE = 0x0B;
		for (temp = 0; temp < 20; temp ++){};
		mode = (PORTE ^ 0xF0) >> 4;
		
		
		
		// -------------------------
		d_line ++;
		if (d_line > 2) // 4
			d_line = 0;
		clrwdt();
	}
	
}

#include "Functions.c"
