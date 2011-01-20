/*
extract.c
Copyright © 2009 William Astle

This file is part of LWAR.

LWAR is free software: you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation, either version 3 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program. If not, see <http://www.gnu.org/licenses/>.

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwar.h"

void do_extract(void)
{
	FILE *f;
	char buf[8];
	long l;
	int c;
	char fnbuf[1024];
	int i;
	FILE *nf;
	
	f = fopen(archive_file, "r");
	if (!f)
	{
		perror("Opening archive file");
		exit(1);
	}
	
	fread(buf, 1, 6, f);
	if (memcmp("LWAR1V", buf, 6))
	{
		fprintf(stderr, "%s is not a valid archive file.\n", archive_file);
		exit(1);
	}

	for (;;)
	{
		c = fgetc(f);
		if (ferror(f))
		{
			perror("Reading archive file");
			exit(1);
		}
		if (c == EOF)
			return;
		
		
		// find the end of the file name
		if (!c)
			return;
		
		i = 0;
		while (c)
		{
			fnbuf[i++] = c;
			c = fgetc(f);
			if (c == EOF || ferror(f))
			{
				fprintf(stderr, "Bad archive file\n");
				exit(1);
			}
		}
		fnbuf[i] = 0;
		
		// get length of archive member
		l = 0;
		c = fgetc(f);
		l = c << 24;
		c = fgetc(f);
		l |= c << 16;
		c = fgetc(f);
		l |= c << 8;
		c = fgetc(f);
		l |= c;
		
		for (i = 0; i < nfiles; i++)
		{
			if (!strcmp(files[i], fnbuf))
				break;
		}
		if (i < nfiles || nfiles == 0)
		{
			// extract the file
			nf = fopen(fnbuf, "w");
			if (!nf)
			{
				fprintf(stderr, "Cannot extract '%s': %s\n", fnbuf, strerror(errno));
				exit(1);
			}
			while (l)
			{
				c = fgetc(f);
				fputc(c, nf);
				l--;
			}
			fclose(nf);
		}
		else
		{
			// skip the file
			fseek(f, l, SEEK_CUR);
		}
	}
}
