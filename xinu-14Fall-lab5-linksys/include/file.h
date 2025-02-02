/* file.h - definitions commmon to remote and local file systems */

/* Definition of file mode bits */

#define	F_MODE_R	0x01		/* Bit to grant read access	*/
#define	F_MODE_W	0x02		/* Bit to grant write access	*/
#define	F_MODE_RW	0x03		/* Mask for read and write bits	*/
#define	F_MODE_N	0x04		/* Bit for "new" mode		*/
#define	F_MODE_O	0x08		/* Bit for "old" mode		*/
					/* Note: new and old mode bits 	*/
					/*  are mutually exclusive	*/
#define	F_MODE_NO	0x0C		/* Mask for "n" and "o" bits	*/

/* Control functions for files */

#define	F_CTL_DEL	1		/* Delete a file		*/
#define	F_CTL_TRUNC	2		/* Truncate a file		*/
#define	F_CTL_MKDIR	3		/* make a directory		*/
#define F_CTL_LS	4		/* List contents of a directory */
#define	F_CTL_RMDIR	5		/* remove a directory		*/
#define F_CTL_SIZE	6		/* Obtain the size of a file	*/
//#define F_CTL_TO_HIERARCHY 7
