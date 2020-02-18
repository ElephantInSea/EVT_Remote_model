/*The project "Respondent". Association "EVT".*/

void Btns_action (uc btn)
{
	uc temp = btn, count = 0;
	while(temp)
	{
		if (temp & 1)
			count ++;
		temp = temp >> 1;
	}
	// Will not work if none is pressed, or pressed more than 2 buttons
	if (count != 1)
		return;
		
	if (btn & 0x10)	// Up
	{
		LED[led_active] = LED[led_active] + 1;
		if (LED[led_active] > 9)
			LED[led_active] = 0;
	}
	else if (btn & 0x08)	// Down
	{
		if (LED[led_active] == 0)
			LED[led_active] = 10;
		LED[led_active] = LED[led_active] - 1;
	}
	else if (btn & 0x04)	// Left. btn - blue
	{
		if (led_active == 2 - led_count) //4
			led_active = 3;//5
		led_active --;
	}
	else if (btn & 0x02)	// Right
	{
		led_active ++;
		if (led_active > 2)//4
			led_active = 2 - led_count;//4
	}
	else if (btn & 0x01)	// Send
	{
		if(flag_send_mode == 0)
		{
			flag_send_mode = 1;
			flag_rw = 1; //Write
		}
		else //STOP sending
		{
			flag_send_mode = flag_rw = 0;
		}
		//--------------------------
	}
	return;
}

uc Get_port_e(uc part)
{
	uc ans = 0;
	if (part == 1) // 3
		ans += 5;
	
	part = (PORTE ^ 0xF8) >> 3; // 0b000xxxxx
	
	while (part != 0x01)
	{
		part = part >> 1;
		ans += 1;
	}
	if (ans > 6)
		ans += 1;
	/* Here you can enter the setting of the amplitude mode 1, 2, 3 */
	return ans;
}

void Reg_Start_up ()
{
	GLINTD = 1;		// Disable All Interrupts
	PORTE = 0x00;	// Getting button codes and modes
	DDRE  = 0x00;
	PORTC = 0x00;	// Numbers on the scoreboard cell
	DDRC  = 0x00;
	PORTD = 0x00;	// Power for indicator and button polling
	DDRD  = 0x00;
	
	// Start signal. Only needed for layout
	DDRE = 0;
	PORTE = 0x2E; // 0b00101110
	
	for (a = 0; a < 255; a ++)
		for (b = 0; b < 255; b ++);
	
	a = b = 0;	
	
	PIR1    = 0x00;	// Reset Interrupt Request Flags
	PIE1    = 0x01;	// RCIE setting: USART receiver interrupt enable bit 
					// (there is data in the receiver buffer)
	T0STA   = 0x28;	// Switching on TMR0 (internal clock frequency, 1:16 pre-selector)
	// T0STA does not matter since interruptions not allowed
	INTSTA  = 0x08;	// PEIE setting
	
	TXSTA = 0x42;	// 0b01000010 9bit, asynchronous,
	RCSTA = 0x90;	// 0b10010000 on port, 9bit, continuous reception
	SPBRG = 0x9B;	// 155
	USB_CTRL = 0x01;	// USB launch. Low Speed (1.5 Ìבטע/c),
	
	
	GLINTD  = 0; // Reset All Interrupt Disable Bit
	CREN = 0;
	
	
	DDRE = 0xF0;//FC 	// 0b11111100 Buttons * 5 and MANUAL/AUTO
	
	PORTE = 0x0F;
	
	LED[0] = LED[1] = LED[2] = 0;
	
    flag_send_mode = 0;		// Turn on to receive data
    flag_rw = 0;
	led_active = 4;	// The number of the selected indicator. 
					// 4 is the far left
    mode = 255;
    
    count_receive_data = 0;
    a = b = c = d = 0;
    flag_msg_received = 0;	// Flag of received message
    error_code = 0;
    error_code_interrupt = 0;
    led_count = 2;
}
