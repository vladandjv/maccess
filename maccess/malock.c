/**************************************************************************/
/* PROGRAM: MALOCK.C                                                      */
/* DESCRIPTION: Memory cache locking system based on Unix System V        */
/* semaphores.                                                            */
/* AUTHOR: VLADAN DJORDJEVIC                                              */
/**************************************************************************/
#include "defs.h"
#include "maccess.h"
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include "semun.h"

static key_t key;
static int semid;
static union semun arg;
static struct sembuf sb;
/**************************************************************************/
void DB_Lock_Init(key_t semaphore_code)
{
#ifndef SINGLE_USER_NO_SHARED_MEMORY
	key = semaphore_code;

	/* create a semaphore set with 1 semaphore: */
	if ((semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0600)) == -1)
	{
		/* grab the semaphore set already created */
		if ((semid = semget(key, 1, 0)) == -1)
		{
			IOStatus = ErrSemaphoreAttach;
			TAIOCheck();
		}
	}
	else
	{
		/* initialize semaphore #0 to 1: */
		arg.val = 1;
		if (semctl(semid, 0, SETVAL, arg) == -1)
		{
			IOStatus = ErrSemaphoreSet;
			TAIOCheck();
		}
	}
#endif
	return;
}
/**************************************************************************/
void SHM_Lock() /* Shared memory lock */
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
	return;
#else
	sb.sem_num = 0;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(semid, &sb, 1);

#endif
}
/**************************************************************************/
void SHM_UnLock() /* Shared memory unlock */
{
#ifdef SINGLE_USER_NO_SHARED_MEMORY
	return;
#else
	sb.sem_op = 1; /* free resource */
	semop(semid, &sb, 1);
#endif
}
/**************************************************************************/
long long Destroy_Semaphore() /* Removing the semaphore */
{
#ifndef SINGLE_USER_NO_SHARED_MEMORY
	if (semctl(semid, 0, IPC_RMID, arg) == -1)
	{
		logMessage("Destroy_SHM: Cannot destroy the shared memory segment!");
		return (-1);
	}

#endif
	return 0;
}
/**************************************************************************/
