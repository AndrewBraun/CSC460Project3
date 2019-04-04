/*
 * photoresistor.h
 *
 * Created: 2019-04-01 3:20:31 PM
 *  Author: Andrew
 */ 


#ifndef PHOTORESISTOR_H_
#define PHOTORESISTOR_H_

void photoresistor_init();

void read_photoresistor_task(void* param_ptr);

#endif /* PHOTORESISTOR_H_ */