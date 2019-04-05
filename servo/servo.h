/*
 * servo.h
 *
 * Created: 2019-04-01 4:11:04 PM
 *  Author: Andrew
 */ 


#ifndef SERVO_H_
#define SERVO_H_

/*
 * Servo struct used by the Roomba Arduino.
 * Pin is the signal pin for the servo motor.
 * Position is the servo's current position
 * Velocity is the direction the servo will move when update_servo_task is called.
 * If velocity = 1, position will be incremented
 * If veloctiy = -1, position will be decremented
 * If velocity = 0, position will not be changed.
 */
typedef struct servo{
	uint8_t pin;
	int8_t position;
	int8_t velocity;
} Servo;

Servo pan_servo, tilt_servo;

/*
 * Method for the controller to write the servo movement to the other Arduino.
 */
void send_servo_joystick_task(void* param_ptr);

/*
 * Method for Roomba Arduino to initialize a servo motor.
 */
void servo_init(Servo* servo, uint8_t pin);

/*
 * Changes the position of the servo motor.
 * If new_direction = 1, then it increments the position up to 180.
 * If new_direction = -1, it decrements the position down to 0.
 * If new_direction = 0, the position does not change.
 */
void Task_UpdateServo(void* servo_arg);

#endif /* SERVO_H_ */