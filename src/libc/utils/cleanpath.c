#include "system.h"

char	*
basenm(char *s)
{
	char	*t;

	for (t = s; *t; t++);
	do {
		t--;
	} while (*t != '/' && t > s);
	if (*t == '/') t++;
	return (t);
}

/*
 * clean up "..", "." and "//" in a path name
 */
void
cleanPath(char *path, char cleanPath[])
{
	char	buf[MAXPATH], *p, *r, *top;
	int	dotCnt = 0;	/* number of "/.." */
#define isEmpty(buf, r) 	(r ==  &buf[sizeof (buf) - 2])

	r = &buf[sizeof (buf) - 1]; *r-- = 0;
	p = &path[strlen(path) - 1];

	/* for win32 path */
	top = (path[1] == ':') ? &path[2] : path;

	/* trim trailing slash(s) */
	while ((p >= top) && (*p == '/')) p--;

	while (p >= top) { 	/* scan backward */
		if ((p == top) && (p[0] == '.')) {
			p = &p[-1];		/* process "." in the front */
			break;
		} else if (p == &top[1] && (p[-1] == '.') && (p[0] == '.')) {
			dotCnt++; p = &p[-2];	/* process ".." in the front */
			break;
		} else if ((p >= &top[2]) && (p[-2] == '/') &&
		    (p[-1] == '.') && (p[0] == '.')) {
			dotCnt++; p = &p[-3];	/* process "/.." */
		} else if ((p >= &top[1]) && (p[-1] == '/') &&
		    	 (p[0] == '.')) {
			p = &p[-2];		/* process "/." */
		} else {
			if (dotCnt) {
				/* skip dir impacted by ".." */
				while ((p >= top) && (*p != '/')) p--;
				dotCnt--;
			} else {
				/* copy regular directory */
				unless (isEmpty(buf, r)) *r-- = '/';
				while ((p >= top) && (*p != '/')) *r-- = *p--;
			}
		}
		/* skip "/", "//" etc.. */
		while ((p >= top) && (*p == '/')) p--;
	}

	if (isEmpty(buf, r) || (top[0] != '/')) {
		/* put back any ".." with no known parent directory  */
		while (dotCnt--) {
			if (!isEmpty(buf, r) && (r[1] != '/')) *r-- = '/';
			*r-- = '.'; *r-- = '.';
		}
	}

	if (top[0] == '/') {
#ifdef WIN32
		/* if network drive //host/path .. */
		if (!isEmpty(buf, r) && (top == path)
		    && (top[1] == '/') && (top[2] != '/')) {
			*r-- = '/';
		}
#endif
		*r-- = '/';
	}
	if (top != path) { *r-- = path[1]; *r-- = path[0]; }
	if (*++r) {
		strcpy(cleanPath, r);
		/* for win32 path */
		if ((r[1] == ':') && (r[2] == '\0')) strcat(cleanPath, "/");
	} else {
		strcpy(cleanPath, ".");
	}
#undef	isEmpty
}