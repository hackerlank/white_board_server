#ifndef _POSIX_SEM_H_
#define _POSIX_SEM_H_


#include<semaphore.h>
#include<stdint.h>

int create_semaphone(sem_t* sem, uint32_t init_value);

int  destroy_semaphone( sem_t* sem);

int  semaphone_post( sem_t* sem);

int  semaphone_wait( sem_t* sem);



















#endif
