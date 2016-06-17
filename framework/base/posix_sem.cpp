#include "posix_sem.h"
#include <assert.h>

int create_semaphone(sem_t* t, uint32_t init_value)
{
	assert(t);
	int ret = sem_init(t, 0, init_value);
	assert(ret == 0);
	return ret;
}

int  destroy_semaphone( sem_t* sem)
{
	assert(sem);
	int ret = sem_destroy(sem);
	return ret;
}


int  semaphone_post( sem_t* sem)
{
	assert(sem);
	int ret = sem_post(sem);
	return ret;
}


int  semaphone_wait( sem_t* sem)
{
	assert(sem);
	int ret = sem_wait(sem);
	return ret;
}
