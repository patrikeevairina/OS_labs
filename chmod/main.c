#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <regex.h>

int main(int argc, char *argv[]) 
{
	if (argc == 1)
		return 1;
	char *s = argv[1];

	regex_t regex;
	
	regcomp(&regex, "^-([ugo]{1}[ar]{1}[rwx]{1,3})|([h]{1})$", REG_EXTENDED);
	if (regexec(&regex, s, 0, NULL, 0) == REG_NOMATCH) 
	{
		printf("Pattern chmod error");
		exit(1);
	}

	struct stat st;
	stat(argv[2], &st);
	
	__mode_t mode = st.st_mode & 07777, newMode = 0;
	int remove = 0, type = 0;
	
	for (int i = 1;  s[i] != '\0'; i++) 
	{
		switch (s[i]) 
		{
			case 'h': printf("Подобие системной утилиты chmod\n"
				"chmod -h // вывести справку по использованию\n"
				"chmod -uaw file1 // добавить право пользователю на запись\n"
				"chmod -grr file1 // убрать право группы на чтение\n"
				"chmod -oarwx file1 // добавить права на запись, чтение и исполнение файла для всех остальных\n");
				return 0;
			case 'g': type = 1; break;
			case 'o': type = 2; break;
			case 'r': i == 2 ? remove = 1 : (newMode |= S_IRUSR); break;
			case 'w': newMode |= S_IWUSR; break; 
			case 'x': newMode |= S_IXUSR; break;
		}
	}
	newMode >>= (3 * type);
	if (chmod(argv[2], remove ? mode & (~newMode) : (mode | newMode))) perror("chmod");
}
