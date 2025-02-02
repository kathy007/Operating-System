/* lflGetc.c  -  lfGetc */

#include <xinu.h>

/*------------------------------------------------------------------------
 * lflGetc  --  Read the next byte from an open local file
 *------------------------------------------------------------------------
 */
devcall	lflGetc (
	  struct dentry *devptr		/* entry in device switch table */
	)
{
	struct	lflcblk	*lfptr;		/* ptr to open file table entry	*/
	//struct	ldentry	*ldptr;		/* ptr to file's entry in the	*/
					/*  in-memory directory		*/
	int32	onebyte;		/* next data byte in the file	*/

	/* Obtain exclusive use of the file */

	//kprintf("entering lflGetc...\r\n");
	lfptr = &lfltab[devptr->dvminor];
	wait(lfptr->lfmutex);

	/* If file is not open, return an error */

	if (lfptr->lfstate != LF_USED) {
		signal(lfptr->lfmutex);
		//kprintf("the file is not open \r\n");
		return SYSERR;
	}

	/* Return EOF for any attempt to read beyond the end-of-file */

	//ldptr = lfptr->lfdirptr;
	//if (lfptr->lfpos >= ldptr->ld_size) {
	//kprintf("lfptr->lfpos=%d, lfptr->lfsize=%d\r\n", lfptr->lfpos, lfptr->lfsize);
	if (lfptr->lfpos >= lfptr->lfsize) {
		signal(lfptr->lfmutex);
		//kprintf("read beyond the end-of-file\r\n");
		return EOF;
	}

	/* If byte pointer is beyond the current data block, */
	/*	set up a new data block			     */

	if (lfptr->lfbyte >= &lfptr->lfdblock[LF_BLKSIZ]) {
		//kprintf("brefore lfsetup\r\n");
		lfsetup(lfptr);
	}

	/* Extract the next byte from block, update file position, and	*/
	/*	return the byte to the caller				*/

	onebyte = 0xff & *lfptr->lfbyte++;
	lfptr->lfpos++;
	signal(lfptr->lfmutex);
	//kprintf("getc returns %d\r\n", onebyte);
	return onebyte;
}
