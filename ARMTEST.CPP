#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <dos.h>
#include <conio.h>

//傳輸協定: 
//內容前面加上@, 後面加上;, EX: @SATA;
//如果是單一測試結束, 內容會是測試階段名稱後面加上/P 或是 /F, EX: @SATA/P;
//如果是全部測試結束, 內容會是PASSF 或是 FAILF, EX: @PASSF;

int PORT1;
//內容前面加上@
char cr[100] = "@"; 

//程式使用說明
void showProgramGuide();

//_argc: 檢查輸入參數用, _com: 檢查Com Port, _testStage: 測試階段名稱, _fileName: 藉由此檔案名稱找到這個測項的測試結果

//開始測試前執行
int startTest(const int &_argc, const char *_com, const char *_testStage);		
//結束測試後執行																  
int finishTest(const int &_argc, const char *_com, const char *_fileName, const char *_testStage);
//所有測試結束
int finalTest(const int &_argc, const char *_com);
//檢查ComPort
int settingCOM(const char *_str);
//轉換成大寫後進行判斷
void transferToUpperCase(char * const _str);

int main(int argc, char *argv[], char *envp[])//envp[] 是所有環境變數
{
    if(strcmp(argv[1], "/s") == 0)//開始測試前執行
    {
        if(startTest(argc, argv[2], argv[3]) == 0)
            return 1;
    }else if(strcmp(argv[1], "/f") == 0)//結束測試後執行
    {
        if(finishTest(argc, argv[2], argv[4], argv[3]) == 0)
            return 1;
    }else if(strcmp(argv[1], "/e") == 0)//所有測試結束
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

        if(strcmp(cr, "@") == 0)
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

	//內容後面加上;
    strcat(cr, ";");
    int i;
	//一個一個字元傳送出去
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
    printf("ARMTEST <OPTION> [COM] ( [TEST_STAGE] )( [FILE_NAME] )\n");
    printf("<OPTION>:\n");
    printf("	\"/s\" [COM] [TEST_STAGE]             :Send each test stage start testing.\n");
    printf("	\"/f\" [COM] [TEST_STAGE] [FILE_NAME] :Send each testing finished.\n");
    printf("	\"/e\" [COM]                          :Send final test result.\n");
    printf("[COM]:\n");
    printf("	\"1\"       :Set COM1 port.\n");
    printf("	\"2\"       :Set COM2 port.\n");
    printf("[TEST_STAGE]:\n");
    printf("    Key in which test stage is running right now.\n");
    printf("[FILE_NAME]:\n");
    printf("    Name of the test result file.\n");

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
	//字元不能超過13, 超過會傳不過去
    if(strlen(_testStage) > 13)
    {
        printf("TestStage name is too long.\n");
		showProgramGuide();
        return 0;
    }

    strcat(cr, _testStage); //TEST_STAGE
	return 1;
}	

int finishTest(const int &_argc, const char *_com, const char *_fileName, const char *_testStage)
{
    //當command 下的數量不對時, Show help
    if(_argc < 5)
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
	//字元不能超過13, 超過會傳不過去
    if(strlen(_testStage) > 13)
    {
        printf("TestStage name is too long.\n");
		showProgramGuide();
        return 0;
    }

    strcat(cr, _testStage); //TEST_STAGE

    char result[10] = "/F";

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
            strcpy(result, "/P");
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
