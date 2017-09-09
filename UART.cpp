/* Name : Sample Comm's Program - Polled Version - termpoll.c */
/* Written By : Craig Peacock <cpeacock@senet.com.au> */
#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

/* Defines Serial Ports Base Address */
/* COM1 0x3F8 */
/* COM2 0x2F8 */
/* COM3 0x3E8 */
/* COM4 0x2E8 */

int main(int argc, char *argv[])
{
	int PORT1;
	if(argc == 1)
	{
		printf("CHECKCON.EXE <COM port number>\n");
		return 1;
	}

	if(strcmp(argv[1], "1") == 0)
	{
		printf("Set COM1\n");
		PORT1 = 0x3F8;
	}
	else if(strcmp(argv[1], "2") == 0)
	{
		printf("Set COM2\n");
		PORT1 = 0x2F8;
	}
	else
	{
		printf("ERROR\n");
		return 1;
	}

	int c;
	int ch;
	outportb(PORT1 + 1, 0); /* Turn off interrupts - Port1 */

	/* PORT 1 - Communication Settings */

	outportb(PORT1 + 3, 0x80); /* SET DLAB ON */
	outportb(PORT1 + 0, 0x0C); /* Set Baud rate - Divisor Latch Low Byte */
	/* Default 0x03 = 38,400 BPS */
	/* 0x01 = 115,200 BPS */
	/* 0x02 = 57,600 BPS */
	/* 0x06 = 19,200 BPS */
	/* 0x0C = 9,600 BPS */
	/* 0x18 = 4,800 BPS */
	/* 0x30 = 2,400 BPS */
	outportb(PORT1 + 1, 0x00); /* Set Baud rate - Divisor Latch High Byte */
	outportb(PORT1 + 3, 0x03); /* 8 Bits, No Parity, 1 Stop Bit */
	outportb(PORT1 + 2, 0xC7); /* FIFO Control Register */
	outportb(PORT1 + 4, 0x0B); /* Turn on DTR, RTS, and OUT2 */

	printf("\nSample Comm's Program. Press ESC to quit \n");

	do
	{
		c = inportb(PORT1 + 5); /* Check to see if char has been */
		/* received. */
		if (c &1)
		{
			ch = inportb(PORT1); /* If so, then get Char */
			if(ch == 49)
			{
				printf("connected!");
				return 0;
			}

		} /* Print Char to Screen */

		ch = 48;
		outportb(PORT1, ch);
		
		if (kbhit())
		{
			ch = getch(); /* If key pressed, get Char */
		//	outportb(PORT1, ch);
		} /* Send Char to Serial Port */
	}
	while (ch != 27); /* Quit when ESC (ASC 27) is pressed */
	return 0;
}

