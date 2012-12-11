/*
 * String utility functions.
 */
#include <malloc.h>
#include <string.h>
#include "strutil.h"
#include "defines.h"

int streq(char* a, char* b)
{
	return strcmp(a, b) == 0;
}


// Get register string
const char* regStr(int reg)
{
	switch (reg)
	{
		case AX:
			return "ax";
		case BX:
			return "bx";
		case CX:
			return "cx";
		case DX:
			return "dx";
		case AH:
			return "ah";
		case AL:
			return "al";
		case BH:
			return "bh";
		case BL:
			return "bl";
		case CH:
			return "ch";
		case CL:
			return "cl";
		case DH:
			return "dh";
		case DL:
			return "dl";
		default:
			return "";
	}
}

// Convert a string to lower case
void strToLower(char* str)
{
	int i;
	for (i = 0; str[i] != '\0'; i++)
		str[i] = tolower(str[i]);
}

/* Get directory path from input filepath (with trailing '/' character) */
char* directoryPath(char* filePath)
{
	// Find ending '/'
	int len = strlen(filePath);
	int i;
	int last;
	for (i = len - 1; i >= 0; i--)
	{
		if (filePath[i] == '/')
		{
			last = i;
			break;
		}
	}
	
	if (i < 0)
		return strdup("./");
	
	char* str = (char*) malloc(last + 2);
	str[last + 1] = '\0';
	for (i = 0; i <= last; i++)
		str[i] = filePath[i];
	return str;
}

/* Get file basename. */
char* getBasename(char* filePath)
{
	int len = strlen(filePath);
	int i, j;
	int start = 0;
	for (i = len - 1; i >= 0; i--)
	{
		if (filePath[i] == '/')
		{
			start = i + 1;
			break;
		}
	}
	
	char* str = (char*) malloc(len - start + 1);
	str[len - start] = '\0';
	for (i = start, j = 0; i < len; i++, j++)
		str[j] = filePath[i];
	return str;
}

/* Trims the extension from a filename. */
char* outputPath(char* filePath)
{
	int len = strlen(filePath);
	int i, end;
	for (end = len - 1; end >= 0; end--)
		if (filePath[end] == '.')
			break;
	char* str = (char*) malloc(end + 5);
	for (i = 0; i < end; i++)
		str[i] = filePath[i];
	str[i++] = '.';
	str[i++] = 'a';
	str[i++] = 's';
	str[i++] = 'm';
	str[i] = '\0';
	return str;
}

/* Converts a string in [01]+b or [01]+ format to an int value. */
int bintoint(char* str)
{
	int len = strlen(str);
	int i;
	int value = 0;
	int power = 1;
	for (i = len - 2; i >= 0; i--)
	{
		if (str[i] == '1')
			value += power;
		power *= 2;
	}
	return value;
}


