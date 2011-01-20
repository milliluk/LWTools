/*
replace.c
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

void do_replace(void)
{
	FILE *f;
	FILE *nf;
	unsigned char buf[8];
	long l;
	int c;
	FILE *f2;
	int i;
	char fnbuf[1024];
	char fnbuf2[1024];
		
	sprintf(fnbuf, "%s.tmp", archive_file);
	
	f = fopen(archive_file, "r+");
	if (!f)
	{
		if (errno == ENOENT)
		{
			nf = fopen(fnbuf, "w");
			if (nf)
			{
				fputs("LWAR1V", nf);
				goto doadd;
			}
		}
		perror("Cannot open archive file");
	}
	
	fread(buf, 1, 6, f);
	if (memcmp("LWAR1V", buf, 6))
	{
		fprintf(stderr, "%s is not a valid archive file.\n", archive_file);
		exit(1);
	}

	nf = fopen(fnbuf, "w");
	if (!nf)
	{
		perror("Cannot create temp archive file");
		exit(1);
	}

	fputs("LWAR1V", nf);

	for (;;)
	{
		c = fgetc(f);
		if (c == EOF && ferror(f))
		{
			perror("Reading archive file");
			exit(1);
		}
		if (c == EOF)
			goto doadd;
		
		if (!c)
		{
			goto doadd;
		}
		
		// find the end of the file name
		i = 0;
		while (c)
		{
			fnbuf2[i++] = c;
			c = fgetc(f);
			if (c == EOF || ferror(f))
			{
				fprintf(stderr, "Bad archive file\n");
				exit(1);
			}
		}
		fnbuf2[i] = 0;
		
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
		
		// is it a file we are replacing? if so, do not copy it
		for (i = 0; i < nfiles; i++)
		{
			if (!strcmp(files[i], fnbuf2))
				break;
		}
		if (i < nfiles)
		{
			fseek(f, l, SEEK_CUR);
		}
		else
		{
			// otherwise, copy it
			fprintf(nf, "%s", fnbuf2);
			fputc(0, nf);
			fputc(l >> 24, nf);
			fputc((l >> 16) & 0xff, nf);
			fputc((l >> 8) & 0xff, nf);
			fputc(l & 0xff, nf);
			while (l)
			{
				c = fgetc(f);
				fputc(c, nf);
				l--;
			}
		}
	}
	
	// done with the original file
	fclose(f);
doadd:
	for (i = 0; i < nfiles; i++)
	{
		f2 = fopen(files[i], "r");
		if (!f2)
		{
			fprintf(stderr, "Cannot open file %s:", files[i]);
			perror("");
			exit(1);
		}
		fread(buf, 1, 6, f2);
		if (mergeflag && !memcmp("LWAR1V", buf, 6))
		{
			// add archive contents...
			for (;;)
			{
				c = fgetc(f2);
				if (c == EOF || ferror(f2))
				{
					perror("Reading input archive file");
					exit(1);
				}
				if (c == EOF)
					break;
		
				if (!c)
				{
					break;
				}
		
				// find the end of the file name
				while (c)
				{
					fputc(c, nf);
					c = fgetc(f2);
					if (c == EOF || ferror(f))
					{
						fprintf(stderr, "Bad input archive file\n");
						exit(1);
					}
				}
				fputc(0, nf);
				
				// get length of archive member
				l = 0;
				c = fgetc(f2);
				fputc(c, nf);
				l = c << 24;
				c = fgetc(f2);
				fputc(c, nf);
				l |= c << 16;
				c = fgetc(f2);
				fputc(c, nf);
				l |= c << 8;
				c = fgetc(f2);
				fputc(c, nf);
				l |= c;
		
				while (l)
				{
					c = fgetc(f2);
					fputc(c, nf);
					l--;
				}
			}
			
			fclose(f2);
			continue;
		}
		fseek(f2, 0, SEEK_END);
		l = ftell(f2);
		fseek(f2, 0, SEEK_SET);
		fputs(files[i], nf);
		fputc(0, nf);
		fputc(l >> 24, nf);
		fputc((l >> 16) & 0xff, nf);
		fputc((l >> 8) & 0xff, nf);
		fputc(l & 0xff, nf);
		while (l)
		{
			c = fgetc(f2);
			fputc(c, nf);
			l--;
		}
	}
	
	// flag end of file
	fputc(0, nf);
	
	fclose(nf);
	
	if (rename(fnbuf, archive_file) < 0)
	{
		perror("Cannot replace old archive file");
		unlink(fnbuf);
	}
}
