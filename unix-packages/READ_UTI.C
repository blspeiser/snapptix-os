#include "gtk_install.h"

unsigned short read_until_token(int fd,
				char *buff,
				char token,
				unsigned max_length)
{
	unsigned position = 0;
	int read_bytes;

	do
	{
		read_bytes = read(fd, buff + position, 1);
		if (read_bytes < 0)
			return READ_ERR;
		if (!read_bytes)
			return READ_DONE;
		if (buff[position] == token)
		{
			/* make it a string */
			buff[position] = 0x00;
			return READ_OK;
		}
		position++;
	} while (position < max_length);
	return READ_ERR;
}

/* Eats spaces and backspaces */
unsigned short read_off_whitespace(int fd,
				   char *buff)
{
	int read_bytes;

	do
	{
		read_bytes = read(fd, buff, 1);
		if (read_bytes < 0)
			return READ_ERR;
		if (!read_bytes)
			return READ_DONE;
	} while ((buff[0] == ' ') || (buff[0] == BACKSPACE));
	return READ_OK;
}
