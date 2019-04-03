/*
 * servo.h
 *
 * Created: 2019-04-01 4:11:04 PM
 *  Author: Andrew
 */ 


#ifndef SERVO_H_
#define SERVO_H_

typedef enum {
	HIGH,
	LOW,
} servo_state;

typedef struct servo{
	uint8_t pin;
	uint8_t on_time;
	servo_state;
} Servo;

Servo pan_servo, tilt_servo;

void servo_init(Servo* servo, uint8_t pin);

void update_servo(Servo* servo, uint8_t new_period);

void servo_change_state(Servo* servo);

#endif /* SERVO_H_ */