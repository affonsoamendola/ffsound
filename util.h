#ifndef FF_UTIL
#define FF_UTIL

char readc_stream(char far ** byte_stream);

unsigned long int fgeti32le(FILE * file);
unsigned int fgeti16le(FILE * file);

#endif