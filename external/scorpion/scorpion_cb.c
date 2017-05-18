#include	<stdio.h>
#include	<string.h>
#include	<stdbool.h>

#include	<stdlib.h>
#include	<assert.h>

#include	"scorpion_cb.h"

#define		CB_MEMCPY		memcpy
#define 	CB_MEMSET		memset
#define		CB_MALLOC		malloc
#define		CB_MFREE		free

#define 	CB_LOCK(mutex)			pthread_mutex_lock(&mutex);
#define 	CB_UNLOCK(mutex)		pthread_mutex_unlock(&mutex);
#define		CB_INIT_LOCK(mutex)		pthread_mutex_init(&mutex, NULL);

cb_bool cb_full(CircleBufferMngr *pmngr)
{
	return (cb_bool)(pmngr->buflen == pmngr->datalen);	  
}

cb_bool cb_empty(CircleBufferMngr *pmngr)
{
	return (cb_bool)(0 == pmngr->datalen);	  
}

bool cb_init(CircleBufferMngr **ppmngr, cb_u32 buflen)
{	 
	if (NULL != *ppmngr)
		return false;
	 
	if (0 == buflen)
		return false;
 
	*ppmngr = (CircleBufferMngr*)CB_MALLOC(sizeof(CircleBufferMngr));
	CB_MEMSET((void*)*ppmngr, 0, sizeof(CircleBufferMngr));
	 
	(*ppmngr)->bufptr = (cb_u8*)CB_MALLOC(buflen);
	(*ppmngr)->buflen = buflen;
	CB_MEMSET((void*)(*ppmngr)->bufptr, 0, buflen);

	CB_INIT_LOCK((*ppmngr)->mutex);

	return true;
}
 
bool cb_deinit(CircleBufferMngr **ppmngr)
{ 
	if(NULL == *ppmngr)
		return false;
	 
	if (NULL != (*ppmngr)->bufptr)
	{
		CB_MFREE((*ppmngr)->bufptr);
		(*ppmngr)->bufptr = NULL;
		(*ppmngr)->buflen = 0;
	}
 
	CB_MFREE(*ppmngr);
	*ppmngr = NULL;

	return true;
}

void cb_clear(CircleBufferMngr *pmngr)
{
	pmngr->readpos = pmngr->writepos = pmngr->datalen = 0;
}

cb_s32 cb_read(CircleBufferMngr *pmngr,cb_u8 *outbuf,cb_s32 buflen)
{
	cb_u32 readlen = 0, tmplen = 0;

	if(cb_empty(pmngr))
		return 0;
	 
	CB_LOCK(pmngr->mutex);
	 
	readlen = buflen > pmngr->datalen ? pmngr->datalen : buflen;
	tmplen = pmngr->buflen - pmngr->readpos;
	 
	if(NULL != outbuf)
	{
		if(readlen <= tmplen)
		{
			 CB_MEMCPY(
				 (void*)outbuf,
				 (void*)&pmngr->bufptr[pmngr->readpos],
				 readlen);
		}
		else
		{
			CB_MEMCPY(
				 (void*)outbuf,
				 (void*)&pmngr->bufptr[pmngr->readpos],
				 tmplen);
			 
			CB_MEMCPY(
				 (void*)&outbuf[tmplen],
				 (void*)pmngr->bufptr,
				 readlen - tmplen);
		}
	}
	 
	pmngr->readpos = (pmngr->readpos + readlen) % pmngr->buflen;
	pmngr->datalen -= readlen;
	
	CB_UNLOCK(pmngr->mutex);
	 
	return readlen;
}


cb_s32 cb_read_no_offset(CircleBufferMngr *pmngr,unsigned char *outbuf,cb_s32 buflen)
{
	cb_u32 readlen = 0, tmplen = 0;

	if(cb_empty(pmngr))
		return 0;
	 
	CB_LOCK(pmngr->mutex);
	 
	readlen = buflen > pmngr->datalen ? pmngr->datalen : buflen;
	tmplen = pmngr->buflen - pmngr->readpos;
	 
	if(NULL != outbuf)
	{
		if(readlen <= tmplen)
		{
			 CB_MEMCPY(
				 (void*)outbuf,
				 (void*)&pmngr->bufptr[pmngr->readpos],
				 readlen);
		}
		else
		{
			CB_MEMCPY(
				 (void*)outbuf,
				 (void*)&pmngr->bufptr[pmngr->readpos],
				 tmplen);
			 
			CB_MEMCPY(
				 (void*)&outbuf[tmplen],
				 (void*)pmngr->bufptr,
				 readlen - tmplen);
		}
	}
	 
	
	CB_UNLOCK(pmngr->mutex);
	 
	return readlen;
}


void cb_read_move_offset(CircleBufferMngr *pmngr,cb_s32 offset)
{
	cb_s32 len;

	CB_LOCK(pmngr->mutex);

	len = cb_datalen(pmngr);
	
	if(len <= offset)
	{
		pmngr->readpos = pmngr->writepos;
		pmngr->datalen = 0;
	}
	else
	{
		pmngr->readpos = (pmngr->readpos + offset) % pmngr->buflen;
		pmngr->datalen -= offset;
	}
	
	CB_UNLOCK(pmngr->mutex);
}
 
cb_s32 cb_write(CircleBufferMngr *pmngr, cb_u8 *datptr, cb_s32 datlen)
{
	cb_s32 writelen = 0, tmplen = 0;
	  
	if(cb_full(pmngr))
		return 0;

	CB_LOCK(pmngr->mutex);

	tmplen = pmngr->buflen - pmngr->datalen;
	writelen = tmplen > datlen ? datlen : tmplen;

	if(pmngr->writepos < pmngr->readpos)
	{
		CB_MEMCPY(
				(void*)&pmngr->bufptr[pmngr->writepos],
				(void*)datptr,
				writelen);
	}
	else
	{
		tmplen = pmngr->buflen - pmngr->writepos;
		if(writelen <= tmplen)
		{
			CB_MEMCPY(
					(void*)&pmngr->bufptr[pmngr->writepos],
					(void*)datptr,
					writelen);
		}
		else
		{
			CB_MEMCPY(
					(void*)&pmngr->bufptr[pmngr->writepos],
					(void*)datptr,
					tmplen);

			CB_MEMCPY(
					(void*)pmngr->bufptr,
					(void*)&datptr[tmplen],
					writelen - tmplen);
		}
	}

	pmngr->writepos = (pmngr->writepos + writelen) % pmngr->buflen;
	pmngr->datalen += writelen;

	CB_UNLOCK(pmngr->mutex);
		  
	return writelen;
}


cb_s32 cb_datalen(CircleBufferMngr *pmngr)
{
	return pmngr->datalen;
}

cb_s32 cb_freelen(CircleBufferMngr *pmngr)
{
	cb_s32 len;

 	len = pmngr->buflen - pmngr->datalen;

	return len;
}



