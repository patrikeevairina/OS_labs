#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <libgen.h>

typedef struct 
{
	char filename[256];
	long filesize;
	unsigned int mode;
	char *fileData;
} structArchive;

char *getFileName(char *filePath) 
	{
	for (unsigned long i = strlen(filePath) - 1; i; i--) 
	{
		if (filePath[i] == '/') 
		{
			return &filePath[i + 1];
		}
	}
	return filePath;
}

long getFileSize(char *filePath) 
{
	FILE *fp1 = fopen(filePath, "rb");
	long prev = ftell(fp1);
	fseek(fp1, 0L, SEEK_END);
	long sz = ftell(fp1);
	fseek(fp1, prev, SEEK_SET);
	fclose(fp1);
	return sz;
}

void addToArchive(structArchive f, char *path) 
{
	FILE *fp = fopen(path, "ab");
	if (fwrite(&f.filesize, sizeof(f.filesize), 1, fp)) 
	{
		fwrite(&f.filename, sizeof(char) * 256 + 1, 1, fp);
		fwrite(&f.mode, sizeof(unsigned int), 1, fp);
		fwrite(f.fileData, f.filesize, 1, fp);
	}
	fclose(fp);
}

int main(int argc, char *argv[]) 
{
	char *arg1 = argv[1];
	char *arg2 = argv[2];
	char *arg3 = argv[3];

	if (strcmp(arg1, "-h") == 0 || strcmp(arg1, "--help") == 0) 
	{
		printf("./archiver arch_name -i(--input) file1\n"
			   "./archiver arch_name -e(--extract) file1\n"
			   "./archiver arch_name -s(--stat)  \n"
			   "./archiver -h(--help) \n");
	} 
	else if (strcmp(arg2, "-i") == 0 || strcmp(arg2, "--input") == 0) 
	{
		structArchive archive;
		struct stat stats;

		strcpy(archive.filename, arg3);
		stat(archive.filename, &stats);
		archive.filesize = getFileSize(arg3);
		archive.mode = stats.st_mode;
		archive.fileData = (char *) malloc(sizeof(char) * (archive.filesize + 1));

		FILE *fp = fopen(arg3, "rb");
		char ch;
		long size = getFileSize(arg3);
		char *fileData = (char *) malloc(sizeof(char) * size + 1);
		for (int i = 0; (ch = fgetc(fp)) != EOF; i++) 
		{
			fileData[i] = ch;
		}
		fclose(fp);

		strcpy(archive.fileData, fileData);
		addToArchive(archive, arg1);
		if (remove(argv[3]) != 0) return -1;
	}
	else if (strcmp(arg2, "-s") == 0 || strcmp(arg2, "--stat") == 0) 
	{
		FILE *fp = fopen(arg1, "rb");
		structArchive f;
		while (fread(&f.filesize, sizeof(long), 1, fp)) 
		{
			fread(&f.filename, sizeof(char) * 256 + 1, 1, fp);
			fread(&f.mode, sizeof(unsigned int), 1, fp);
			f.fileData = (char *) malloc(sizeof(char) * (f.filesize + 1));
			fread(f.fileData, f.filesize, 1, fp);
			printf("%s\t%ld\t%u\n", f.filename, f.filesize, f.mode);
		}
		fclose(fp);
	}
	else if (strcmp(arg2, "-e") == 0 || strcmp(arg2, "--extract") == 0) 
	{
		FILE *fp = fopen(arg1, "rb");
		structArchive f;
		while (fread(&f.filesize, sizeof(long), 1, fp)) 
		{
			fread(&f.filename, sizeof(char) * 256 + 1, 1, fp);
			char *filename = f.filename;
			fread(&f.mode, sizeof(unsigned int), 1, fp);
			f.fileData = (char *) malloc(sizeof(char) * (f.filesize + 1));
			fread(f.fileData, f.filesize, 1, fp);

			if (strcmp(filename, arg3) == 0 || strcmp(arg3, "all") == 0) 
			{
				FILE *fp = fopen(filename, "wb");
				char *fileData = f.fileData;
				fputs(fileData, fp);

				chmod(filename, f.mode);
				printf("%s extracted %u\n", filename, f.mode);
			} 
			else 
			{
				addToArchive(f, "temp");
			}
		}
		fclose(fp);
		remove(arg1);
		rename("temp", arg1);
	}
	return 0;
}
