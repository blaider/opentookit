// #############################################################################
// *****************************************************************************
//                  Copyright (c) 2015, Advantech Automation Corp.
//      THIS IS AN UNPUBLISHED WORK CONTAINING CONFIDENTIAL AND PROPRIETARY
//               INFORMATION WHICH IS THE PROPERTY OF ADVANTECH AUTOMATION CORP.
//
//    ANY DISCLOSURE, USE, OR REPRODUCTION, WITHOUT WRITTEN AUTHORIZATION FROM
//               ADVANTECH AUTOMATION CORP., IS STRICTLY PROHIBITED.
// *****************************************************************************
// #############################################################################
//
// File:   	 updatepasswd.c
// Author:  suchao.wang
// Created: Jul 23, 2015
//
// Description:  File download process class.
// ----------------------------------------------------------------------------
//////////////////////////////////////////////////////////////////////////



/* vi: set sw=4 ts=4: */
/*
 * update_passwd
 *
 * update_passwd is a common function for passwd and chpasswd applets;
 * it is responsible for updating password file (i.e. /etc/passwd or
 * /etc/shadow) for a given user and password.
 *
 * Moved from loginutils/passwd.c by Alexander Shishkin <virtuoso@slind.org>
 *
 * Modified to be able to add or delete users, groups and users to/from groups
 * by Tito Ragusa <farmatito@tiscali.it>
 *
 * Licensed under GPLv2, see file LICENSE in this source tree.
 */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#define bb_perror_msg printf
#define bb_perror_nomsg(x) (x)
/*
 1) add a user: update_passwd(FILE, USER, REMAINING_PWLINE, NULL)
    only if CONFIG_ADDUSER=y and applet_name[0] == 'a' like in adduser

 2) add a group: update_passwd(FILE, GROUP, REMAINING_GRLINE, NULL)
    only if CONFIG_ADDGROUP=y and applet_name[0] == 'a' like in addgroup

 3) add a user to a group: update_passwd(FILE, GROUP, NULL, MEMBER)
    only if CONFIG_FEATURE_ADDUSER_TO_GROUP=y, applet_name[0] == 'a'
    like in addgroup and member != NULL

 4) delete a user: update_passwd(FILE, USER, NULL, NULL)

 5) delete a group: update_passwd(FILE, GROUP, NULL, NULL)

 6) delete a user from a group: update_passwd(FILE, GROUP, NULL, MEMBER)
    only if CONFIG_FEATURE_DEL_USER_FROM_GROUP=y and member != NULL

 7) change user's password: update_passwd(FILE, USER, NEW_PASSWD, NULL)
    only if CONFIG_PASSWD=y and applet_name[0] == 'p' like in passwd
    or if CONFIG_CHPASSWD=y and applet_name[0] == 'c' like in chpasswd

 This function does not validate the arguments fed to it
 so the calling program should take care of that.

 Returns number of lines changed, or -1 on error.
*/

static FILE* xfdopen_helper(unsigned fd_and_rw_bit)
{
	FILE* fp = fdopen(fd_and_rw_bit >> 1, fd_and_rw_bit & 1 ? "w" : "r");
	return fp;
}
FILE* xfdopen_for_write(int fd)
{
	return xfdopen_helper((fd << 1) + 1);
}
/* Get line.  Remove trailing \n */
char* xmalloc_fgetline(FILE *file)
{
	int i;
	char *c = NULL;
	char buf[1024];
	memset(buf,0,sizeof(buf));
//	if(!feof(file))
	if(	fgets(buf,1024,file) != NULL)
	{	i = strlen(buf);

		if (i && buf[--i] == '\n')
			buf[i] = '\0';

		c = strdup(buf);
	}

//		printf("%s:%s",__func__,c);
	return c;
}
int  update_passwd(const char *filename,
		const char *name,
		const char *new_passwd,
		const char *member)
{
	struct stat sb;
	struct flock lock;
	FILE *old_fp;
	FILE *new_fp;
	char *fnamesfx;
	char *sfx_char;
	char *name_colon;
	unsigned user_len;
	int old_fd;
	int new_fd;
	int i;
	int changed_lines;
	int ret = -1; /* failure */
	/* used as a bool: "are we modifying /etc/shadow?" */
	const char *shadow = strstr(filename, "shadow");

//	filename = malloc_follow_symlinks(filename);
	if (filename == NULL)
		return ret;


	/* New passwd file, "/etc/passwd+" for now */
	 asprintf(&fnamesfx,"%s+", filename);
//	 printf("%s:%s\n",__func__,fnamesfx);
	sfx_char = &fnamesfx[strlen(fnamesfx)-1];
	asprintf(&name_colon,"%s:", name);
//	printf("%s:%s\n",__func__,name_colon);
	user_len = strlen(name_colon);

	if (shadow)
		old_fp = fopen(filename, "r+");
	if (!old_fp) {
		if (shadow)
			ret = 0; /* missing shadow is not an error */
		goto free_mem;
	}
	old_fd = fileno(old_fp);

	/* Try to create "/etc/passwd+". Wait if it exists. */
	i = 30;
	do {
		// FIXME: on last iteration try w/o O_EXCL but with O_TRUNC?
		new_fd = open(fnamesfx, O_WRONLY|O_CREAT|O_EXCL, 0600);
		if (new_fd >= 0) goto created;
		if (errno != EEXIST) break;
		usleep(100000); /* 0.1 sec */
	} while (--i);
	bb_perror_msg("can't create '%s'", fnamesfx);
	goto close_old_fp;

 created:
	if (fstat(old_fd, &sb) == 0) {
		fchmod(new_fd, sb.st_mode & 0777); /* ignore errors */
		fchown(new_fd, sb.st_uid, sb.st_gid);
	}
	errno = 0;
	new_fp = xfdopen_for_write(new_fd);

	/* Backup file is "/etc/passwd-" */
	*sfx_char = '-';
	/* Delete old backup */
	i = (unlink(fnamesfx) && errno != ENOENT);
	/* Create backup as a hardlink to current */
	if (i || link(filename, fnamesfx))
		bb_perror_msg("warning: can't create backup copy '%s'",
				fnamesfx);
	*sfx_char = '+';

	/* Lock the password file before updating */
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;
	if (fcntl(old_fd, F_SETLK, &lock) < 0)
		bb_perror_msg("warning: can't lock '%s'", filename);
	lock.l_type = F_UNLCK;

	/* Read current password file, write updated /etc/passwd+ */
	changed_lines = 0;
	while (1) {
		char *cp, *line;

		line = xmalloc_fgetline(old_fp);
//		printf("%s:%s\n",__func__,line);
		if (!line) /* EOF/error */
			break;

		if (strncmp(name_colon, line, user_len) != 0) {
			fprintf(new_fp, "%s\n", line);
			goto next;
		}

		/* We have a match with "name:"... */
		cp = line + user_len; /* move past name: */

		if (1) {
			/* Change passwd */
			cp = strchrnul(cp, ':'); /* move past old passwd */

			if (shadow && *cp == ':') {
				/* /etc/shadow's field 3 (passwd change date) needs updating */
				/* move past old change date */
				cp = strchrnul(cp + 1, ':');
				/* "name:" + "new_passwd" + ":" + "change date" + ":rest of line" */
				fprintf(new_fp, "%s%s:%u%s\n", name_colon, new_passwd,
					(unsigned)(time(NULL)) / (24*60*60), cp);
			} else {
				/* "name:" + "new_passwd" + ":rest of line" */
				fprintf(new_fp, "%s%s%s\n", name_colon, new_passwd, cp);
			}
			changed_lines++;
		} /* else delete user or group: skip the line */
 next:
		free(line);
	}

	fcntl(old_fd, F_SETLK, &lock);

	/* We do want all of them to execute, thus | instead of || */
	errno = 0;
	if ((ferror(old_fp) | fflush(new_fp) | fsync(new_fd) | fclose(new_fp))
	 || rename(fnamesfx, filename)
	) {
		/* At least one of those failed */
		perror(filename);
		goto unlink_new;
	}
	/* Success: ret >= 0 */
	ret = changed_lines;

 unlink_new:
	if (ret < 0)
		unlink(fnamesfx);

 close_old_fp:
	fclose(old_fp);

 free_mem:
	free(fnamesfx);
//	free((char *)filename);
	free(name_colon);
	return ret;
}

int main(int argc,char *argv[])
{
	update_passwd("/etc/shadow","root","123458",NULL);
	return 0;
}
