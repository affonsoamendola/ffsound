#include <stdio.h>

#include <util.h>

char readc_stream(char far ** byte_stream)
{
	char read_char;

	read_char = *(*byte_stream);
	
	*byte_stream = (*byte_stream) + 1;

	return read_char;
}

unsigned long int fgeti32le(FILE * file)
{
	unsigned long int value;

	value =  ((unsigned long int)fgetc(file));
	value |= (((unsigned long int)fgetc(file)) << 8);
	value |= (((unsigned long int)fgetc(file)) << 16);
	value |= (((unsigned long int)fgetc(file)) << 24);

	return value;
}

unsigned int fgeti16le(FILE * file)
{
	unsigned int value;

	value = ((unsigned int)fgetc(file));
	value = value | (((unsigned int)fgetc(file)) << 8);

	return value;
}