#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

typedef struct
{
	char filename[256];
	long fileSize;
	unsigned int mode;
	char *fileData;
} structArchive;

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

void addToArchive(structArchive f, char *archName)
{
	FILE *fp = fopen(archName, "ab");
	if (fwrite(&f.fileSize, sizeof(f.fileSize), 1, fp)) 
	{
		fwrite(&f.filename, sizeof(char) * 256 + 1, 1, fp);
		fwrite(&f.mode, sizeof(unsigned int), 1, fp);
		fwrite(f.fileData, f.fileSize, 1, fp);
	}
	fclose(fp);
}

void deleteFromArchive(char *archName, char *extractingFile)
{
	//FILE *in = fopen("buf", "wr");
	FILE *out = fopen(archName, "rb");
//	printf("almost ok");
        structArchive f;
	while (fread(&f.fileSize, sizeof(long) + 1, 1, out))
	{
		fread(&f.filename, sizeof(char) * 256 + 1, 1, out);
		char* filename = f.filename;
		fread(&f.mode, sizeof(unsigned int), 1, out);
		f.fileData = (char *) malloc(sizeof(char) * (f.fileSize + 1));
		fread(f.fileData, f.fileSize, 1, out);
		if (strcmp(filename, extractingFile) != 0) 
		{
	//		printf("ok");
			addToArchive(f, "buf");
		}
	}
	//printf("ok");
	fclose(out);
	//fclose(in);
	/*FILE *arch  = fopen(archName, "wb");
	FILE *bufArch = fopen("buf", "rb");
	int ch;
	ch = fgetc(bufArch);
	while(!feof(bufArch))
	{
		fputc(ch, arch);
		ch = fgetc(bufArch);
	}
	fclose(bufArch);
	fclose(arch);*/
}

int main(int argc, char **argv)
{
	if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) 
	{
		printf("./archiver arch_name –i(--input) file1\n"
			   "./archiver arch_name –e(--extract) file1\n"
			   "./archiver arch_name –s(--stat) // вывести текущее состояние архива \n"
			   "./archiver –h(--help) // вывести справку по работе с архиватором \n");
	}
	else if (strcmp(argv[2], "-i") == 0 || strcmp(argv[2], "--input") == 0) 
	{
		structArchive archive;
		struct stat stats;
		strcpy(archive.filename, argv[3]);
		archive.fileSize = getFileSize(argv[3]);
		stat(archive.filename, &stats);
		archive.mode = stats.st_mode;
		archive.fileData = (char *) malloc(sizeof(char) * (archive.fileSize + 1));

		FILE *fp = fopen(argv[3], "rb");
		int i = 0;
		char ch;
		long size = getFileSize(argv[3]);
		char *fileData = (char *) malloc(sizeof(char) * size + 1);
		while ((ch = fgetc(fp)) != EOF) 
		{
			fileData[i] = ch;
			i++;
		}
		fclose(fp);
		strcpy(archive.fileData, fileData);
		addToArchive(archive, argv[1]);
		if (remove(argv[3]) != 0)
			return -1;
	}
	else if (strcmp(argv[2], "-e") == 0 || strcmp(argv[2], "--extract") == 0)
	{
		FILE *fp = fopen(argv[1], "rb");
		structArchive f;
		while (fread(&f.fileSize, sizeof(long), 1, fp)) 
		{
			fread(&f.filename, sizeof(char) * 256 + 1, 1, fp);
			char *filename = f.filename;
			fread(&f.mode, sizeof(unsigned int), 1, fp);
			f.fileData = (char *) malloc(sizeof(char) * (f.fileSize + 1));
			fread(f.fileData, f.fileSize, 1, fp);

			if (strcmp(filename, argv[3]) == 0 || strcmp(argv[3], "all") == 0) 
			{
				FILE *fp = fopen(filename, "wb");
				char *fileData = f.fileData;
				fputs(fileData, fp);
				chmod(filename, f.mode);
				printf("%s extracted %u\n", filename, f.mode);
				if (strcmp(argv[3], "all") != 0) break;
			}
		}
		fclose(fp);
		deleteFromArchive(argv[1], argv[3]);
	}
	return 0;
}
