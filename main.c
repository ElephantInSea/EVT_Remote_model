//The project "Respondent_model". Association "EVT".
/*11 11 2019
A stripped-down version of the defendant is needed for the layout. 
3 indicators instead of 5, 3 buttons instead of 5, 
a conductor instead of a switch.
*/
#include "1886ve4d.h"
#include "int17xxx.h"
#pragma origin 0x8

typedef unsigned char uc;

const uc Translate_num_to_LED[10] = {
//  0,	  1,	2,	  3,	4,	  5,	6,	  7,	8,	  9.
	0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
// Cells for receiving messages
uc a, b, c, d;
uc LED [3];
int led_active;
uc mode;

bit flag_send_mode;
bit flag_rw; // 0 read, 1 write
bit flag_msg_received;
bit flag_manual_auto;
bit flag_mode_ampl;

uc count_receive_data;
uc error_code;
uc error_code_interrupt;
uc led_count;
uc led_on_E;
#include "Interrupts.h"

interrupt iServer(void)
{
	multi_interrupt_entry_and_save

	PERIPHERAL_service:
		Handler_receiver ();
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
#include "Interrupts.c"
/******************/
#include "math24.h"
#include "Functions.h"

void main(void)
{
	Reg_Start_up();
	
	int d_line = 0;	// Working indicator number
	bit flag_first_launch = 1;
    uc led_blink = 0;
    uc led_blink_temp = 0;
    uc mode = 0;
	uc temp = 0;
    
    uc mode_temp = 0, mode_time = 0;
    uc buttons = 0, buttons_time = 0; 
    
    uc E_time = 0;
    bit E_part = 0;

	while(1)
	{
		clrwdt();
		DDRE = 0xF0;
		PORTE = 0xFF;
		// PORT D --------------------------------------------------------------
		temp = 0x20 << d_line; // 08
		//temp |= Show_ERROR (); //d_work_light;
		// Code entered for model --
		PORTC = 0;
		PORTD = temp ^ 0xE0;
		// Code entered for model --
		
		// -------------------------
		for (temp = 0; temp < 5; temp ++) {};
		// -------------------------
		
		// PORT C --------------------------------------------------------------
		if (d_line == led_active)	// For two iterations, the selected
		{							// indicator will be turned off
			led_blink_temp ++;
			if (led_blink_temp > 254)
			{
				led_blink_temp = 0;
				led_blink ++;
				if (led_blink > 254)		// Delay for blinking
					led_blink = 0;
			}
		}
		
		//if (d_line < 4 - led_count)
		//	temp = 0;
		if ((d_line == led_active) && (led_blink & 0x08))
			temp = 0xFF; //0
		else if (d_line > led_count)//(d_line < (2 - led_count))
			temp = 0xFF; //0
		else
		{
			temp = LED[(int)d_line];// The order of indicators is determined 
									// here.
			temp = Translate_num_to_LED[(int)temp];
		}
		
		uc t = 0x01 << d_line;
		if (mode & t)
			temp &= 0x7F;	// Setting a point on the LED indicator 
							// to display the mode.
		
		PORTC = temp ^ 0xFF;
		
		for (temp = 0; temp < 20; temp ++) {};
		
		clrwdt();
		// PORT E --------------------------------------------------------------
		
		PORTC = 0;
		
		temp = PORTE;
		temp = temp ^ 0xE0;
		t = temp >> 1;
		temp = temp >> 5;
		
		
		if((d_line > 0) && (temp > 0))	// mode
		{
			temp = Get_port_e(d_line, temp);
			
			if (mode != temp)
			{
				if(mode_temp == temp)
				{
					mode_time ++;
					if (mode_time > 20)
					{
						mode = temp;
						// flag_send_mode = 1;
						// flag_rw = 0; //Read
						Change_led_count (mode);
					}
				}
				else
				{
					mode = 255;		// Fuse
					flag_send_mode = 0;
					mode_temp = temp;
					mode_time = 0;
					led_active = 0;
					LED[0] = LED[1] = LED[2] = 0;
				}
			}	
		}
		else if (d_line == 0x00)	//Buttons
		{
			if (temp == buttons)
			{
				if (buttons_time <= 10)	// A pressed key will work
					buttons_time ++;	// only once
				if ((buttons_time == 10) && buttons > 0)
					Btns_action (buttons);
			}
			else 
			{
				buttons_time = 0;
				buttons = temp;
			}
		}
		
		
		// отображение включенных пинов
		PORTD = 0xE0; 
		DDRE = 0x00;
		PORTE = 0x0F;
		/*
		PORTE = t | 0x0E;
		for (t = 0; t < 10; t++){};
		*/
		// Индикация о работе
		if(led_on_E != 127)
		{
			if (E_time == 0)
				E_time = 100;
			else if (E_time == 1)
				led_on_E = 127;
			
			E_time -= 1;
		}
		
		E_part = !E_part;
	
		if (E_part == 0)
			PORTE = 0x0E | (led_on_E << 4);
		else
		{
			PORTE = 0x0D | (led_on_E & 0xF0);
			PORTE |= Show_ERROR();
		}
		
			
		for (t = 0; t < 10; t++){};
		
		clrwdt();
		
		d_line ++;
		if (d_line > 2) // 4
			d_line = 0;
		
		// Send Part -----------------------------------------------------------
		if ((flag_send_mode == 1) && (mode != 255))
		{
			Send_part(flag_first_launch);
			
			if (flag_first_launch)
				flag_first_launch = 0;
		}
		
		clrwdt();
	}
	
}

#include "Functions.c"
