#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>

int PORT1;
char cr[100] = "@"; //Send Code ASCII

void showProgramGuide();
int startTest(const int &_argc, const char *_com, const char *_testStage);
int finishTest(const int &_argc, const char *_com, const char *_fileName);
int finalTest(const int &_argc, const char *_com);
int settingCOM(const char *_str);
void transferToUpperCase(char * const _str);

int main(int argc, char *argv[], char *envp[])//envp[] 是所有環境變數
{
    if(strcmp(argv[1], "/s") == 0)
    {
        if(startTest(argc, argv[2], argv[3]) == 0)
            return 1;
    }else if(strcmp(argv[1], "/f") == 0)
    {
        if(finishTest(argc, argv[2], argv[3]) == 0)
            return 1;
    }else if(strcmp(argv[1], "/e") == 0)
    {
        if(finalTest(argc, argv[2]) == 0)
            return 1;

        int i = 0;
        for(i = 0; envp[i] != NULL; i++)// 找環境變數的X=PASS or FAIL , 轉換成大寫後進行判斷
        {
            if(strcmp(strupr(envp[i]), "X=PASS") == 0)
            {
				printf("Get pass\n");
				strcat(cr, "PASSF");
                break;
            }else if(strcmp(strupr(envp[i]), "X=FAIL") == 0)
            {
				printf("Get fail\n");
				strcat(cr, "FAILF");
                break;
            }
        }

        if(strcmp(cr, "@PASSF") != 0)
        {
			printf("Get nothing, fail\n");
            strcat(cr, "FAILF");
        }
    }else
    {
        printf("Option argument error\n");
        showProgramGuide();
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

    printf("\n\nComplete \n");

    return 0;
}

void showProgramGuide()
{
    printf("ARMTEST <OPTION> [COM] ( [TEST_STAGE] || [FILE_NAME] )\n");
    printf("<OPTION>:\n");
    printf("	\"/s\" [COM] [TEST_STAGE]   :Send each test stage start testing.\n");
    printf("	\"/f\" [COM] [FILE_NAME]    :Send each testing finished.\n");
    printf("	\"/e\" [COM]                :Send final test result.\n");
    printf("[COM]:\n");
    printf("	\"1\"       :Set COM1 port.\n");
    printf("	\"2\"       :Set COM2 port.\n");
    printf("[TEST_STAGE]:\n");
    printf("    Key in which test stage is running right now.\n");

}

int startTest(const int &_argc, const char *_com, const char *_testStage)
{
    //當command 下的數量不對時, Show help
    if(_argc < 4)
    {
        printf("Invaild command \n");
        showProgramGuide();
        return 0;
    }

    if(settingCOM(_com) != 1)
    {
        printf("COM port error, COM parameter must be \"1\" or \"2\"\n");
        showProgramGuide();
        return 0;
    }

    strcat(cr, _testStage); //TEST_STAGE
	return 1;
}	
int finishTest(const int &_argc, const char *_com, const char *_fileName)
{
    //當command 下的數量不對時, Show help
    if(_argc < 4)
    {
        printf("Invaild command \n");
        showProgramGuide();
        return 0;
    }

    if(settingCOM(_com) != 1)
    {
        printf("COM port error, COM parameter must be \"1\" or \"2\"\n");
        showProgramGuide();
        return 0;
    }

    char result[10] = "FAIL";

    FILE *filePtr;
    //產生檔案路徑
    char *fileName;
    char filePath[200] = "\\ADLINK\\";
    fileName = strcat(filePath, _fileName);

    //開啟檔案
    if((filePtr = fopen( fileName, "r")) == NULL)
    {
        printf("Read file error.\n");
        fclose(filePtr);
        return 0;
    }

    //搜尋是否有Pass關鍵字
    char readLineStr[200];
    while (!feof(filePtr)) {
        fscanf(filePtr, "%s", readLineStr);
        transferToUpperCase(readLineStr);
        printf(" : %s\n", readLineStr );

        if(strstr(readLineStr, "PASS") != NULL){
            strcpy(result, "PASS");
            printf("Find\n");
            break;
        }
    }

    fclose(filePtr);
    strcat(cr, result);
	return 1;
}

int finalTest(const int &_argc, const char *_com)
{
    //當command 下的數量不對時, Show help
    if(_argc < 3)
    {
        printf("Invaild command \n");
        showProgramGuide();
        return 0;
    }

    if(settingCOM(_com) != 1)
    {
        printf("COM port error, COM parameter must be \"1\" or \"2\"\n");
        showProgramGuide();
        return 0;
    }
	return 1;
}

int settingCOM(const char *_str)
{
    if(strcmp(_str, "1") == 0)
    {
        printf("Set COM1\n");
        PORT1 = 0x3F8;
        return 1;
    }
    else if(strcmp(_str, "2") == 0)
    {
        printf("Set COM2\n");
        PORT1 = 0x2F8;
        return 1;
    }
    else
    {
        return 0;
    }
}

void transferToUpperCase(char * const _str)
{

    for(char *ptr = _str; *ptr != '\0'; ptr++)
    {
        if(isupper(*ptr) == 0)
            *ptr = toupper(*ptr);
    }
}
