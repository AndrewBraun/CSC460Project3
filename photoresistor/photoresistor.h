/*
 * photoresistor.h
 *
 * Created: 2019-04-01 3:20:31 PM
 *  Author: Andrew
 */ 


#ifndef PHOTORESISTOR_H_
#define PHOTORESISTOR_H_

void photoresistor_init();

void Task_UpdatePhotoresistor(void* param_ptr);

void shadow_realm();

#endif /* PHOTORESISTOR_H_ */