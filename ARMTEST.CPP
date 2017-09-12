#include <dos.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>

int main(int argc, char *argv[], char *envp[])//envp[] 是所有環境變數
{
	int mode; //Catch Code ASCII
	int PORT1; 
	char cr[100]; //Send Code ASCII
	
    //當command 下的數量不對時, Show help
	if(argc == 1 || argc == 2)
	{
        printf("Invaild command \n");
		printf("NONAME00 <OPTION> <COM> <TEST_STAGE>\n");
		printf("<OPTION>:\n");
		printf("	/? 	:Help.\n");
		printf("	/b	:Tell server boot PASS.\n");
		printf("	/r      :Send result to server.\n");
		printf("<COM>:\n");
		printf("	1	:Set COM1 port.\n");
		printf("	2	:Set COM2 port.\n");
        printf("<TEST_STAGE>:\n");
        printf("[String]:What test stage is running right now?\n");
		return 1;
	}

    //Command /? , show help
	if(strcmp(argv[1], "/?") == 0)
	{
		printf("NONAME00 <OPTION> <COM> <TEST_STAGE>\n");
		printf("<OPTION>:\n");
		printf("	/? 	:Help.\n");
		printf("	/b	:Tell server boot PASS.\n");
		printf("	/r      :Send result to server.\n");
		printf("<COM>:\n");
		printf("	1	:Set COM1 port.\n");
		printf("	2	:Set COM2 port.\n");
        printf("<TEST_STAGE>:\n");
        printf("[String]:What test stage is running right now?\n");
		return 0;
	}
	else if(strcmp(argv[2], "1") == 0)// Command [2] , setting COM port
	{
		printf("Set COM1\n");
		PORT1 = 0x3F8;
	}
	else if(strcmp(argv[2], "2") == 0)// Command [2] , setting COM port
	{
		printf("Set COM2\n");
		PORT1 = 0x2F8;
	}
	else
	{
		printf("Command error.");
		return 1;
	}
    // If command [1] : /b = boot, /r = send result
	if(strcmp(argv[1], "/b") == 0)
	{
		mode = 49; //ASCII=1
		strcpy(cr, argv[3]); //TEST_STAGE
	}else if(strcmp(argv[1], "/r") == 0)
	{
		mode = 67; //ASCII=C
		int i = 0;	
		for(i = 0; envp[i] != NULL; i++)// 找環境變數的X=PASS or FAIL , 轉換成大寫後進行判斷
		{
			if(strcmp(strupr(envp[i]), "X=PASS") == 0)
			{
				printf("PASS");
				strcpy(cr, "PASS");//ASCII=P
				break;
			}else if(strcmp(strupr(envp[i]), "X=FAIL") == 0)
			{
				printf("FAIL");
				strcpy(cr, "FAIL");//ASCII=F
				break;
			}
		}

		if(strcmp(cr, "PASS") != 0)
		{
			strcpy(cr, "FAIL");
		}

	}else
	{
		printf("Command error.");
		return 1;
	}


	int c;
	int ch;
	outportb(PORT1 + 1, 0); /* Turn off interrupts - Port1 */

	/* PORT 1 - Communication Settings */

	outportb(PORT1 + 3, 0x80); /* SET DLAB ON */
	outportb(PORT1 + 0, 0x02); /* Set Baud rate - Divisor Latch Low Byte */
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

	strcat(cr, ";");
    int i;
    for( i = 0; i < strlen(cr); i++ ) 
	{
		outportb( PORT1, cr[i] );
		printf("%c", cr[i]);
	}
	printf("\n");
	printf("\nSample Comm's Program. Press ESC to quit \n");

	do
	{
		c = inportb(PORT1 + 5); /* Check to see if char has been */
		/* received. */
		if (c &1)
		{
			ch = inportb(PORT1); /* If so, then get Char */
			if(ch == mode)
			{
				printf("connected!");
				return 0;
			}

		} /* Print Char to Screen */

		if (kbhit())
		{
			ch = getch(); /* If key pressed, get Char */
		//	outportb(PORT1, ch);
		} /* Send Char to Serial Port */
	}
	while (ch != 27); /* Quit when ESC (ASC 27) is pressed */

    return 0;
}
