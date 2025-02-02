#include <xinu.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* When creating a new directory, need to update its parent and parent's parent .. */
// eg: /a/b/c -> when creating 'c' with full path 
// add/replace the entry in /a/b, /b
// updateDir initialize lfltab[Nlfl+1] to /a/b/
// and lfltab[Nlfl] to /a/
// take in parameter tokenized paths

// its more like chechDir

bool8 strcmp(char *, char *);

status updateDir(char paths[][LF_NAME_LEN], int depth) {

	// last entry -> parent directory (current directory)
	struct	lflcblk	*dir_cblk = &lfltab[Nlfl + 1];
	// second last entry -> grandparent directory 
	struct	lflcblk	*pardir_cblk = &lfltab[Nlfl];
	struct 	dentry	dev_ptr;
	struct 	dentry 	par_dev_ptr;

	// read the 0th data block to find out 
	// size of the root directory and first index block
	//kprintf("entering updateDir...\r\n");

	wait(Lf_data.lf_mutex); // waiting on mutex forever?

	//kprintf("get mutex\r\n");

	if (! Lf_data.lf_dirpresent) {
		// cache the root directory
		// should only executed once
		//kprintf("the directory is not present in memory \r\n");
		struct	lfdir	in_root;	// ptr to in-memory dir
		//kprintf("Lf_data.lf_dskdev = %d\r\n", Lf_data.lf_dskdev);

		int r = read(Lf_data.lf_dskdev, (char*)&in_root, LF_AREA_ROOT);
		//kprintf("how many bytes were read in memory = %d\r\n", r);
		
		if (r == SYSERR) {
			//kprintf("updateDir: error when reading root to memory\r\n");
			signal(Lf_data.lf_mutex);
			return SYSERR;
		} else {
			//kprintf("read root in memory\r\n");
		}
		// set the global data dir to root
		Lf_data.lf_dir = in_root;
		Lf_data.lf_dirpresent = TRUE;
		Lf_data.lf_dirdirty = FALSE;
	} else {
		//kprintf("the parent/current directory is present in memory \r\n");
	}
	signal(Lf_data.lf_mutex);

	// reset the control block of parent and grandparent directory
	resetCblk(dir_cblk);
	resetCblk(pardir_cblk);

	dev_ptr.dvminor = Nlfl + 1;
	par_dev_ptr.dvminor = Nlfl;
	
	// the dir_cblk is occupied by root for now
	dir_cblk->lfstate = LF_USED;
	dir_cblk->lfsize = Lf_data.lf_dir.lfd_size; // = 0?
	//kprintf("dir_cblk size = %d\r\n", Lf_data.lf_dir.lfd_size);
	dir_cblk->lffibnum = Lf_data.lf_dir.lfd_ifirst;

	// root directory depth = 0
	int curr_depth = 0;

	struct	ldentry	curr_dir_entry;
	struct	ldentry *curr_dir = &curr_dir_entry;

	// start from root, loop thru all dirs from device switch table, put in ldentry buffer 
	//int num = lflRead(&dev_ptr, (char*)curr_dir, sizeof(struct ldentry));
	//kprintf("%d\r\n", num);

	//kprintf("ldentry size = %d\r\n", sizeof(struct ldentry));
	//int num = lflRead(&dev_ptr, (char*)curr_dir, sizeof(struct ldentry));
	//kprintf("%d\r\n", num);
	
	int readcnt = 0;
	while (curr_depth < depth && (readcnt = lflRead(&dev_ptr, (char*)curr_dir, sizeof(struct ldentry))) != SYSERR) {
		//kprintf("%d\r\n", num);
		// when found the current target file/directory
		if (readcnt != sizeof(struct ldentry)) {
			break;
		}
		if (strcmp(curr_dir->ld_name, paths[curr_depth])) {
			// return error if it is a file instead of a directory
			if (curr_dir->ld_type != LF_TYPE_DIR) {
				//kprintf("UpdateDir: found a file %s which supposed to be a directory \r\n", curr_dir->ld_name);
				return SYSERR;
			}
			
		} else {
			//kprintf("coundn't find directory = %s compared with %s\r\n", paths[curr_depth], curr_dir->ld_name);
			continue;
		}
		//kprintf("found a target directory name = %s\r\n", curr_dir->ld_name);
		// save the current directory as the parent directory of next loop
		memcpy(pardir_cblk, dir_cblk, sizeof(struct lflcblk));
		// reset current directory
		resetCblk(dir_cblk);
		dir_cblk->lfstate = LF_USED;
		dir_cblk->lfsize = curr_dir->ld_size;
		dir_cblk->lffibnum = curr_dir->ld_ilist;
		curr_depth ++;

	}
	// should display different behavior when dealing with "mkdir" and "open"
	// mkdir -> creates directory when reaching target's parent 
	// open -> creates directory when reading target's parent && mode set to "rw"
	// 		-> otherwise, return SYSERR


	// TODO: create a global variable indicating which function called updateDir

 	//kprintf("curr_depth = %d, depth = %d\r\n", curr_depth, depth);
	if (curr_depth != depth) {
		//kprintf("updateDir: %s doesnt exist\r\n", paths[curr_depth]);
		return SYSERR;
	}
	return OK;
}
/*
bool8 strcmp(char *first, char *second) {
	while (*first != NULLCH && *first == *second) {
		first ++;
		second ++;
	}
	return (*first == *second) && (*first == NULLCH);
}
*/
