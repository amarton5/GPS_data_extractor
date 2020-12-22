// Parse 70mai MP4, extract GPS data to SRT, CSV
// by masteel
// compile: gcc dash70maigps.c -o dash70maigps
// use:
// maigps -cgs input.mp4 -o output
// 		-c : output in CSV
//		-g : output in SRT
//		-s : standard output
//
//		The script will deicide the extension of the file

//
// Parse 70mai MP4, extract GPS data to SRT
// by freezer52000
//
// compile: gcc maigps.c -o maigps
// use:     maigps NO20191226-084003-000598.MP4 > NO20191226-084003-000598.MP4.srt

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>
#include <dirent.h>

struct gx {
	int f1;
	int f2;
	int f3;
	int speed;
	char N;
	int lat;
	char E;
	int lng;
} __attribute__((packed));

long b2l(unsigned char *b, int count)
{
	int i;
	long out=0;
	for (i=0; i<count; i++) {
		out <<= 8;
		out |= b[i];
	}
	return out;
}

int mp4ToSRTfile(unsigned char *b, struct gx *g, int mp, char *const inputFileName[])
{
	int i;
	FILE *fp;
	char outputFileName[255];

	strcpy(outputFileName, inputFileName[2]);
	strcat(outputFileName, ".SRT");

	fp = fopen(outputFileName, "w+");

	for (i=0; i<mp; i++) {
		g = (void *)(b+i*36);
		if (g->N != 'N' && g->N != 'S')
			continue;

		fprintf(fp, "%d\n", g->f3+1);
		fprintf(fp, "00:%02d:%02d,000 --> 00:%02d:%02d,999\n", g->f3/60, g->f3%60, g->f3/60, g->f3%60);
		fprintf(fp, "%c%d.%05d %c%d.%05d\n", 
			g->N, g->lat/100000, (int)((((long)g->lat%100000)*100000)/60000), 
			g->E, g->lng/100000, (int)((((long)g->lng%100000)*100000)/60000));
		fprintf(fp, "%d.%03d km/h\n", g->speed/1000, g->speed%1000);
		fprintf(fp, "\n");
	}

	fclose(fp);
	return 0;
}

int mp4ToCSVfile(unsigned char *b, struct gx *g, int mp, char *const inputFileName[])
{
	int i;
	FILE *fp;
	char outputFileName[255];

	strcpy(outputFileName, inputFileName[2]);
	strcat(outputFileName, ".CSV");
	
	fp = fopen(outputFileName, "w+");

	fprintf(fp, "No;LAT;LONG;speed\n");

	for (i=0; i<mp; i++) {
		g = (void *)(b+i*36);
		if (g->N != 'N' && g->N != 'S')
			continue;

		fprintf(fp, "%d;%c%d,%05d;%c%d,%05d;%d,%03d\n", g->f3+1, g->N, g->lat/100000, (int)((((long)g->lat%100000)*100000)/60000), 
			g->E, g->lng/100000, (int)((((long)g->lng%100000)*100000)/60000), g->speed/1000, g->speed%1000);
	}

	fclose(fp);
	return 0;
}

int mp4ToSRTstd(unsigned char *b, struct gx *g, int mp)
{
	int i;
	for (i=0; i<mp; i++) {
		g = (void *)(b+i*36);
		if (g->N != 'N' && g->N != 'S')
			continue;

		printf("%d\n", g->f3+1);
		printf("00:%02d:%02d,000 --> 00:%02d:%02d,999\n", g->f3/60, g->f3%60, g->f3/60, g->f3%60);
		printf("%c%d.%05d %c%d.%05d\n", 
			g->N, g->lat/100000, (int)((((long)g->lat%100000)*100000)/60000), 
			g->E, g->lng/100000, (int)((((long)g->lng%100000)*100000)/60000));
		printf("%d.%03d km/h\n", g->speed/1000, g->speed%1000);
		printf("ezaz\n");
		printf("\n");
	}
	return 0;
}

int mp4ToCSVstd(unsigned char *b, struct gx *g, int mp)
{
	int i;
	for (i=0; i<mp; i++) {
		g = (void *)(b+i*36);
		if (g->N != 'N' && g->N != 'S')
			continue;

		printf("%d\n", g->f3+1);
		printf("00:%02d:%02d,000 --> 00:%02d:%02d,999\n", g->f3/60, g->f3%60, g->f3/60, g->f3%60);
		printf("%c%d.%05d %c%d.%05d\n", 
			g->N, g->lat/100000, (int)((((long)g->lat%100000)*100000)/60000), 
			g->E, g->lng/100000, (int)((((long)g->lng%100000)*100000)/60000));
		printf("%d.%03d km/h\n", g->speed/1000, g->speed%1000);
		printf("ezaz\n");
		printf("\n");
	}
	return 0;
}

int openAllVideos(char *const abcd[])
{
	DIR *folder;
	// char buff[255];
	struct dirent *entry;
	int files = 0;
	
	// strcpy(buff, abcd[0]);
	// strcat(buff, abcd[1]);

	// printf("%s\n",buff);

	folder = opendir(".");

	while( (entry=readdir(folder)) )
    {
        files++;
        printf("File %3d: %s\n",
                files,
                entry->d_name
              );
    }

	// printf("%s", entry[1]->d_name);

    closedir(folder);

	return 0;
}

// OBSOLETE
int isOutputExists(char outputFileName[])
{
	if( access( outputFileName, F_OK ) != -1 ) {
		// file exists
		printf("%s file exists. Deleting.", outputFileName);
		remove(outputFileName);
	} else {
		// file doesn't exist
		printf("%s file does not exists. Do nothing.", outputFileName);
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int fd, i, mp, pos;
	long sz;
	char *b;
	struct gx *g;
	char name[5];
	// My stuff
	int opt;
	enum { CHARACTER_MODE, WORD_MODE, LINE_MODE } mode = CHARACTER_MODE;
	///////////

	// openAllVideos(argv);

	// Open the file in the 3rd argument
	fd = open(argv[2], O_RDONLY);
	if (fd<0) {
		perror("open");
		return -1;
	}
	b = malloc(36*120*2);

	pos = 0;

	while( strcmp(name, "GPS ") != 0 )
	{
		lseek(fd, pos, SEEK_SET);
		read(fd, b, 8);
		memcpy(name, b+4, 4);
		name[4] = 0;
		sz = b2l((void *)b, 4);

		if (sz==1) 
		{
			lseek(fd, pos+8, SEEK_SET);
			read(fd, b, 8);
			sz = b2l((void *)b, 8);
		}
		pos += sz;
	}

	read(fd, b, 36*120*2);
	close(fd);
	mp = sz / 36;
 
	while ( (opt = getopt(argc, argv, "sc")) != -1 ) 
	{
		switch (opt) 
		{
			case 's':
				mp4ToSRTfile((void *)b, (void *)g, mp, argv);
				free(b);
			break;

			case 'c':
				mp4ToCSVfile((void *)b, (void *)g, mp, argv);
				free(b);
			break;

			default:
				fprintf(stderr, "Usage: %s [-ilw] [file...]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}
	return 0;
}