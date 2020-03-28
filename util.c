inline char readc_stream(char ** byte_stream)
{
	read_char = **(byte_stream);
	*byte_stream = (*byte_stream) + 1;
	return read_char;
}