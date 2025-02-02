#include <xinu.h>

/* ----------------------------------------------------------
 * lflistdirh - List contents of a directory
 * ---------------------------------------------------------
 */
bool8 strcmp(char *, char *);

status lflistdirh(
		did32		disk,		/* ID of an open disk device	*/
		char 		*path 		/* path */
		)
{
	//kprintf("entering lflistdirh...\r\n");

	char paths[LF_PATH_DEPTH][LF_NAME_LEN];
	
	//wait(cblkmutex);
	int depths = pathTokenize(path, paths);
	//signal(cblkmutex);

	if (depths == SYSERR) {
		return SYSERR;
	}

	wait(cblkmutex);
	// initialize lfltab[Nlfl+1] -> parent direcotry (current)
	// initizlize lfltab[Nlfl] -> grandparent directory
	if (updateDir(paths, depths-1) == SYSERR) {
		//signal(Lf_data.lf_mutex);
		//kprintf("error when allocating directory\r\n");
		signal(cblkmutex);
		return SYSERR;
	}

	// last entry -> parent directory (current directory)
	struct	lflcblk	*dir_cblk = &lfltab[Nlfl + 1];
	// second last entry -> grandparent directory 
	struct	lflcblk	*pardir_cblk = &lfltab[Nlfl];
	struct 	dentry	dev_ptr;
	struct 	dentry 	par_dev_ptr;



	dev_ptr.dvminor = Nlfl + 1;
	par_dev_ptr.dvminor = Nlfl;

	struct	ldentry	curr_dir_entry;
	struct	ldentry* curr_dir = &curr_dir_entry;

	char 	*curr_name = paths[depths-1];
	uint32	pos = 0;
	bool8 	isInitialized = 0;
	int 	found = 0; // if found the target directory

	// similar to updateDir
	
	// case1: (target is root)
	/*
	if (depths == 1 && paths[0][0] == '/' && paths[0][1] == '\0') {
		kprintf("target is root\r\n");
	}*/

	int readcnt = 0;
	// case2: (target is not root) find the entry of the target directory from its parent directory	
	while ((readcnt = lflRead(&dev_ptr, (char*)curr_dir, sizeof(struct ldentry))) != SYSERR) {
		//
		//kprintf("readcnt = %d\r\n", readcnt);
		// if the target is root
		if (depths == 1 && paths[0][0] == '/' && paths[0][1] == '\0') {
			//kprintf("its root\r\n");
			int flag = 0;
			if (readcnt != sizeof(struct ldentry)) {
				flag = 1;
				//kprintf("size not match\r\n");
			}
			if (!curr_dir->isOccupied) {
				flag = 1;
				//kprintf("not occupied\r\n");
			}
			if (flag) {
				break;
			}
			found = 1;
			//kprintf("LS:\r\n");
			kprintf("%s\r\n", curr_dir->ld_name);
			//kprintf("\r\n");

		} else {
			// not root: go to parent and check curr's existance

			if (readcnt != sizeof(struct ldentry)) {
				//kprintf("readcnt != sizeof(struct ldentry)\r\n");
				break;
			}
			if (! curr_dir->isOccupied) {
				if (! isInitialized) {
					pos = dir_cblk->lfpos - sizeof(struct ldentry);
					isInitialized = 1;
				}
				continue;
			}

			// when found the current target file/directory 's parent directory

			if (strcmp(curr_dir->ld_name, curr_name) && curr_dir->isOccupied) {
					
				// make sure it is a directory
				//kprintf("compare: %s == %s\r\n", curr_dir->ld_name, curr_name);
				if (curr_dir->ld_type == LF_TYPE_FILE) {
					//kprintf("lflistdirh: target is a file -> %s\r\n", curr_dir->ld_name);
					
					dir_cblk->lfstate = LF_FREE;
					pardir_cblk->lfstate = LF_FREE;
					signal(cblkmutex);
					return SYSERR;

				}
				// target's parent is a directory: do the list work
				found = 1;
				
				// TODO
				// find the children of the target directory
				int tmpcnt = 0;
				int exit_flag = 0;

				/* save the current directory as the parent directory for the following while loop */
				// while: looping thru parent directory

				//struct	lflcblk	*tmp_cblk = &lfltab[Nlfl + 2];

				memcpy(pardir_cblk, dir_cblk, sizeof(struct lflcblk));
				//memcpy(dir_cblk, pardir_cblk, sizeof(struct lflcblk));
				// reset current directory
				resetCblk(dir_cblk);
				dir_cblk->lfstate = LF_USED;
				dir_cblk->lfsize = curr_dir->ld_size;
				dir_cblk->lffibnum = curr_dir->ld_ilist;

				/* ---------------------------------------------------------------------------- */

				while ((tmpcnt = lflRead(&dev_ptr, (char*)curr_dir, sizeof(struct ldentry))) != SYSERR) {
					if (tmpcnt != sizeof(struct ldentry)) {
						exit_flag = 1;
						break;
					}
					if (! curr_dir->isOccupied) {
						continue;
					}
					kprintf("%s\r\n", curr_dir->ld_name);
				}
				if (exit_flag) {
					break;
				}

				//memcpy(tmp_cblk, dir_cblk, sizeof(struct lflcblk));
				//resetCblk(dir_cblk);

				dir_cblk->lfstate = LF_FREE;
				pardir_cblk->lfstate = LF_FREE;
				signal(cblkmutex);
				break;
			}
		}
	}

	if(!found) // didnt find target
	{
		//kprintf("lflistdirh: no such directory \r\n");

		dir_cblk->lfstate = LF_FREE;
		pardir_cblk->lfstate = LF_FREE;

		signal(cblkmutex);
		return SYSERR;
	}

	signal(cblkmutex);
	//kprintf("successfully list \r\n");
	return OK;		
}
/*
bool8 strcmp(char *first, char *second) {
	while (*first != NULLCH && *first == *second) {
		first ++;
		second ++;
	}
	return (*first == *second) && (*first == NULLCH);
}*/
