/*
 * joystick.c
 *
 * Created: 2019-03-18 3:31:37 PM
 *  Author: Andrew
 */ 

void joystick_init(Joystick* joystick, unsigned int x_pin, unsigned int y_pin){
	DDRK = 0x00;
	DDRF = 0x00;
		
	joystick->x_pin = x_pin;
	joystick->y_pin = y_pin;
	joystick->x_value = 512;
	joystick->y_value = 512;
}

void read_joystick_task(void* joystick){
	
}