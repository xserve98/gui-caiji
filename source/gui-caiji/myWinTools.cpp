#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>

int myGetLenWC2GBK(wchar_t * ws)
{
	int bad;
	return WideCharToMultiByte(CP_ACP, 0, ws, -1, NULL, 0, "", &bad);
}

int myConvWC2GBK(wchar_t * ws,char * buf,int buf_size)
{
    int bad;

    if ( !WideCharToMultiByte(CP_ACP, 0, ws, -1, buf, buf_size, "", &bad) )
    {
        return 0;
    }

    return 1;
}

 /* 
    ��������myGetFileSize(char * strFileName)  
    ���ܣ���ȡָ���ļ��Ĵ�С 
    ������ 
        strFileName (char *)���ļ��� 
    ����ֵ�� 
        size (int)���ļ���С 
 */  
int myGetFileSize(char * strFileName)   
{  
    FILE * fp = fopen(strFileName, "r"); 
	int size;
    fseek(fp, 0L, SEEK_END);  
    size = ftell(fp);  
    fclose(fp);  
    return size;  
}
