#include "global.h"
#include <wx/arrimpl.cpp>

#define ZL_EXP_OS_IN_WINDOWS //���ڸ���zenglǶ��ʽ�ű����Ե�ǰ��ִ�л�����windowsϵͳ����ֹ���ֱ������
#include "zengl_exportfuns.h" //����zengl v1.2.2

wxString global_SettingFile ="�����������ݿ�.db";
wxSQLite3Database  *settingDB;
MyHashString glHashStr,global_MyModules,glMyModuleNames;
int global_mod_processNum = 0;
int global_mod_processPos = 0;
int global_mod_totalpostnum = 0;
int global_mod_totalcaijinum = 0;
int single_mod_processNum = 0;
int single_mod_processPos = 0;
int single_mod_curpostnum = 0;
int single_mod_caijinum = 0;
unsigned int global_seed = 0;
wxString single_mod_cat_table = "";
wxString single_mod_catname = "";
wxString single_mod_catid = "";
wxString single_mod_modid = "";
wxString ContactContent = "";
wxString global_my_weburl = "";
wxString gl_initManage_Url = "";
extern wxString glCompanyCatID;
wxScopedCharBuffer myglStrForUTF8_ZenglRun;

extern FILE * zengl_debuglogfile;

WX_DEFINE_OBJARRAY(ArrayOfMyAreaObj);

ArrayOfMyAreaObj global_MyAreas;

MyAreaObj::MyAreaObj(wxString strName,wxString strID)
{
	areaName = strName;
	areaID = strID;
}

void InitHashMap()
{
	if(glHashStr.size() > 0)
		return;
	glHashStr["21"] = _("��Ѷ");
	glHashStr["13"] = _("Ʒ��");
	glHashStr["6"] = _("��");
	glHashStr["4"] = _("��˾");
	glHashStr["15"] = _("����");
	glHashStr["8"] = _("չ��");
	glHashStr["17"] = _("�Ź�");
	glHashStr["22"] = _("��Ϣ/����");
	glHashStr["9"] = _("�˲�");
	glHashStr["10"] = _("֪��");
	glHashStr["16"] = _("�̳�");
	glHashStr["7"] = _("����");
	glHashStr["5"] = _("��Ӧ");
	glHashStr["11"] = _("ר��");
	glHashStr["12"] = _("ͼ��");
	glHashStr["14"] = _("��Ƶ");

}

void global_GetUrlContent(wxString remote_url)
{
	curl_easy_setopt(curl, CURLOPT_URL, remote_url.c_str().AsChar());
	curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
	long timeout;
	glmainFrame->m_textForTimeOut->GetValue().ToLong(&timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int)timeout);//���ó�ʱʱ��
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);//���ó�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_string_write);
	/*if((fp=fopen("test","w"))==NULL)
    {
        curl_easy_cleanup(curl);
		myglStyle = MY_RICHTEXT_RED_STYLE;
		myglStr = "��test�ļ�ʧ�ܣ�\n";
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
		wxQueueEvent(glmainFrame->GetEventHandler(),event.Clone());
		return;
    }*/
	curl_easy_perform(curl);
	if(char_myglStr==NULL)
	{
		charlength = 0;
		myglStyle = MY_RICHTEXT_RED_STYLE;
		myglStr = _T("ץȡ��ַ��") + remote_url + _T("ʱ��û�л�ȡ���κ����ݣ���ϸ��Ϣ��");
		myglStr += " null char content get! ԭ���������վ����ʧ�ܻ�ʱ�������Ǹ���ַ�޷����ʵ�ԭ��\n";
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
		wxQueueEvent(glmainFrame->GetEventHandler(),event.Clone());
		return;
	}
	charlength = strlen(char_myglStr);
	if((myglStr = wxString(char_myglStr)) == "")
		myglStr = wxString::FromUTF8(char_myglStr);
	
	//wxScopedCharBuffer myglStrForUTF8 = myglStr.ToUTF8();
	//fwrite(myglStrForUTF8.data(), 1 , myglStrForUTF8.length(),fp);
	//fclose(fp);
	free(char_myglStr);
	char_myglStr = NULL;
	char_myglTotalnum = 0;
}

extern "C"
{
	wxSQLite3ResultSet gl_sqlset_forZenglRun;
	int gl_isQuerySql_forZenglRun = 0;
	
	ZL_EXP_INT global_userdef_compile_info_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
	{
		fprintf(zengl_debuglogfile,"%s",infoStrPtr);
		return 0;
	}

	ZL_EXP_INT global_userdef_compile_error_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount,ZL_EXP_VOID * VM_ARG)
	{
		wxString printStr = infoStrPtr;
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		eventforlog.SetEventMsg(printStr,MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return 0;
	}

	ZL_EXP_INT global_userdef_run_info_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
	{
		fprintf(zengl_debuglogfile,"%s",infoStrPtr);
		return 0;
	}

	ZL_EXP_INT global_userdef_run_print_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount)
	{
		wxString printStr = infoStrPtr;
		printStr += "\n";
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		eventforlog.SetEventMsg(printStr,MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return 0;
	}

	ZL_EXP_INT global_userdef_run_error_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount,ZL_EXP_VOID * VM_ARG)
	{
		wxString printStr = infoStrPtr;
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		eventforlog.SetEventMsg(printStr,MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return 0;
	}
	
	/*printfģ�麯������LOG��־�ı�������ʾ��Ϣ����*/
	ZL_EXP_VOID global_printf(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_INT color;
		wxString printStr;
		if(argcount != 2)
		{
			zenglApi_SetErrThenStop(VM_ARG,"printf�����������2������");
			return;
		}
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
		{
			zenglApi_SetErrThenStop(VM_ARG,"printf������һ�������������ַ�������ʾҪ��ʾ������");
			return;
		}
		printStr = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
		{
			zenglApi_SetErrThenStop(VM_ARG,"printf�����ڶ���������������������ʾҪ��ʾ����ɫ��0������ͨ��ɫ��1�����ɫ��2������ɫ");
			return;
		}
		color = arg.val.integer;
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		if(color == 0)
			eventforlog.SetEventMsg(printStr,MY_RICHTEXT_NORMAL_STYLE);
		else if(color == 1)
			eventforlog.SetEventMsg(printStr,MY_RICHTEXT_RED_STYLE);
		else if(color == 2)
			eventforlog.SetEventMsg(printStr,MY_RICHTEXT_GREEN_STYLE);
		else
		{
			zenglApi_SetErrThenStop(VM_ARG,"printf�����ڶ�������ֵ������ɫ����ֻ����0,1,2����ֵ֮һ��0������ͨ��ɫ��1�����ɫ��2������ɫ\n");
			return;
		}
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}
	
	/*readģ�麯��������һ������Ի����ڸöԻ����п��Խ����û�������*/
	ZL_EXP_VOID global_read(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		wxString ReadStr = "",Message = "",Caption = "";
		if(argcount >= 3)
		{
			for(int i=1;i<=3;i++)
			{
				zenglApi_GetFunArg(VM_ARG,i,&arg);
				switch(arg.type)
				{
				case ZL_EXP_FAT_INT:
					ReadStr.sprintf("%d",arg.val.integer);
					break;
				case ZL_EXP_FAT_FLOAT:
					ReadStr.sprintf("%.16g",arg.val.floatnum);
					break;
				case ZL_EXP_FAT_STR:
					ReadStr.sprintf("%s",arg.val.str);
					break;
				default:
					zenglApi_SetErrThenStop(VM_ARG,"read������%d������������Ч��Ŀǰֻ֧���ַ��������������������͵Ĳ���",i);
					return;
					break;
				} //switch(arg.type)
				switch(i)
				{
				case 1:
					Message = ReadStr;
					break;
				case 2:
					Caption = ReadStr;
					break;
				}
			} //for(i=1;i<=3;i++)
		} //if(argcount >= 3)
		wxTextEntryDialog dialog(glmainFrame,
                           Message,
                           Caption,
                           ReadStr,
                           wxOK | wxCANCEL);
		if(dialog.ShowModal() == wxID_OK)
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(ZL_EXP_CHAR *)dialog.GetValue().c_str().AsChar(),0,0);
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,"",0,0);
	}
	
	/*bltRandomģ�麯�������������*/
	ZL_EXP_VOID global_bltRandom(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		time_t t;
		static int random_seed;
		if(random_seed == 0) //��һ��ʹ��ʱ����Ϊ������ӡ�
		{
			srand((unsigned) time(&t));
			random_seed = rand();
		}
		else //����ʱ��ʹ����һ�����ɵ��������Ϊ�������
		{
			srand(random_seed);
			random_seed = rand();
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,random_seed,0);
	}
	
	/*arrayģ�麯�������ڴ���zengl�ű��Ķ�̬����*/
	ZL_EXP_VOID global_array(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MEMBLOCK memblock = {0};
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_INT i;
		if(argcount == 0) //���array����û���������򴴽�һ��Ĭ�ϴ�С��δ��ʼ��������
		{
			if(zenglApi_CreateMemBlock(VM_ARG,&memblock,0) == -1)
				zenglApi_Exit(VM_ARG,zenglApi_GetErrorString(VM_ARG));
			zenglApi_SetRetValAsMemBlock(VM_ARG,&memblock);
		}
		else if(argcount >= 1) //������˲�����ʹ�ò�����Ϊ�����ĳ�ʼֵ
		{
			if(zenglApi_CreateMemBlock(VM_ARG,&memblock,0) == -1)
				zenglApi_Exit(VM_ARG,zenglApi_GetErrorString(VM_ARG));
			for(i=1;i<=argcount;i++)
			{
				zenglApi_GetFunArg(VM_ARG,i,&arg);
				switch(arg.type)
				{
				case ZL_EXP_FAT_INT:
				case ZL_EXP_FAT_FLOAT:
				case ZL_EXP_FAT_STR:
				case ZL_EXP_FAT_MEMBLOCK:
				case ZL_EXP_FAT_ADDR:
				case ZL_EXP_FAT_ADDR_LOC:
				case ZL_EXP_FAT_ADDR_MEMBLK:
					zenglApi_SetMemBlock(VM_ARG,&memblock,i,&arg);
					break;
				default:
					zenglApi_Exit(VM_ARG,"array������%d������������Ч",i);
					break;
				}
			}
			zenglApi_SetRetValAsMemBlock(VM_ARG,&memblock);
		}
		else
			zenglApi_Exit(VM_ARG,"array�����쳣����������С��0");
	}
	
	/*�ݹ��ӡ��������Ϣ*/
	ZL_EXP_VOID global_print_array(ZL_EXP_VOID * VM_ARG,ZENGL_EXPORT_MEMBLOCK memblock,ZL_EXP_INT recur_count)
	{
		ZL_EXP_INT size,i,j;
		ZENGL_EXPORT_MOD_FUN_ARG mblk_val = {ZL_EXP_FAT_NONE,{0}};
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		zenglApi_GetMemBlockInfo(VM_ARG,&memblock,&size,ZL_EXP_NULL);
		for(i=1;i<=size;i++)
		{
			mblk_val = zenglApi_GetMemBlock(VM_ARG,&memblock,i);
			switch(mblk_val.type)
			{
			case ZL_EXP_FAT_INT:
			case ZL_EXP_FAT_FLOAT:
			case ZL_EXP_FAT_STR:
			case ZL_EXP_FAT_MEMBLOCK:
				for(j=0;j<recur_count;j++)
				{
					eventforlog.SetEventMsg("  ",MY_RICHTEXT_NORMAL_STYLE);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				}
				break;
			}
			switch(mblk_val.type)
			{
			case ZL_EXP_FAT_INT:
				eventforlog.SetEventMsg(wxString::Format("[%d] %d\n",i-1,mblk_val.val.integer),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				break;
			case ZL_EXP_FAT_FLOAT:
				eventforlog.SetEventMsg(wxString::Format("[%d] %.16g\n",i-1,mblk_val.val.floatnum),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				break;
			case ZL_EXP_FAT_STR:
				eventforlog.SetEventMsg(wxString::Format("[%d] %s\n",i-1,mblk_val.val.str),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				break;
			case ZL_EXP_FAT_MEMBLOCK:
				eventforlog.SetEventMsg(wxString::Format("[%d] <array or class obj type> begin:\n",i-1),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				global_print_array(VM_ARG,mblk_val.val.memblock,recur_count+1);
				eventforlog.SetEventMsg(wxString::Format("[%d] <array or class obj type> end\n",i-1),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
				break;
			}
		}
	}

	/*bltPrintArrayģ�麯������ӡ�����е�Ԫ��*/
	ZL_EXP_VOID global_bltPrintArray(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount < 1)
			zenglApi_Exit(VM_ARG,"bltPrintArray������������Ϊ�գ�����ָ��һ������������Ϊ����");
		zenglApi_GetFunArg(VM_ARG,1,&arg);
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltPrintArray�����Ĳ�����������������");
		global_print_array(VM_ARG,arg.val.memblock,0);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}
	
	/*bltTestAddrģ�麯��(��������)�����Բ�������*/
	ZL_EXP_VOID global_bltTestAddr(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		if(argcount < 2)
			zenglApi_Exit(VM_ARG,"bltTestAddr��������������������");
		zenglApi_GetFunArgInfo(VM_ARG,1,&arg);
		if(arg.type != ZL_EXP_FAT_ADDR && 
			arg.type != ZL_EXP_FAT_ADDR_LOC &&
			arg.type != ZL_EXP_FAT_ADDR_MEMBLK
			)
			zenglApi_Exit(VM_ARG,"��һ�����������Ǳ��������ã��������Ա�����ã�����������Ա������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //��ȡ��һ��������ֵ��zenglApi_GetFunArg���Եݹ����ã��ҵ����õı�����ֵ
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltTestAddr����Ŀǰֻ�ܽ����ַ�����Ϊ����");
		eventforlog.SetEventMsg(wxString::Format("the value of first arg is %s\n",arg.val.str),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ������������øò������ַ���ֵ���õ�һ���������õı���
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltTestAddr����Ŀǰֻ�ܽ����ַ�����Ϊ����");
		zenglApi_SetFunArg(VM_ARG,1,&arg);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}
	
	/*bltSetArrayģ�麯����ʹ�õ�2������3���Ȳ��������õ�һ��������Ӧ�������е�Ԫ��*/
	ZL_EXP_VOID global_bltSetArray(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZENGL_EXPORT_MEMBLOCK memblock = {0};
		ZL_EXP_INT i;
		if(argcount < 2)
			zenglApi_Exit(VM_ARG,"bltSetArray��������������������");
		zenglApi_GetFunArg(VM_ARG,1,&arg);
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltSetArray������һ����������������������������ߵ���������");
		memblock = arg.val.memblock;
		for(i=2;i<=argcount;i++)
		{
			zenglApi_GetFunArg(VM_ARG,i,&arg);
			zenglApi_SetMemBlock(VM_ARG,&memblock,i-1,&arg);
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}
	
	ZL_EXP_VOID global_module_init(ZL_EXP_VOID * VM_ARG);

	/*bltLoadScriptģ�麯�����½�һ������������ز�ִ��ĳ���ű�*/
	ZL_EXP_VOID global_bltLoadScript(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * scriptName = ZL_EXP_NULL;
		zenglApi_GetFunArg(VM_ARG,1,&arg); //��ȡ��һ������Ϊ�ű���
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltLoadScript������һ�����������ַ���������Ҫ���صĽű��ļ���");
		scriptName = arg.val.str;
		ZENGL_EXPORT_VM_MAIN_ARGS vm_main_args = {global_userdef_compile_info_forZenglRunV2 , 
								  global_userdef_compile_error_forZenglRunV2,
								  global_userdef_run_info_forZenglRunV2,
								  global_userdef_run_print_forZenglRunV2,
								  global_userdef_run_error_forZenglRunV2,
								  global_module_init,
								  ZL_EXP_CP_AF_IN_DEBUG_MODE | 
								  ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO};
		zenglApi_Load(scriptName,&vm_main_args); 
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}
	
	/*bltGetZLVersionģ�麯������ȡ��ǰzengl�汾����Ϣ���ַ�����ʽ*/
	ZL_EXP_VOID global_bltGetZLVersion(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZL_EXP_CHAR version[20] = {0};
		sprintf(version,"%d.%d.%d",ZL_EXP_MAJOR_VERSION,ZL_EXP_MINOR_VERSION,ZL_EXP_REVISION);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,version,0,0);
	}
	
	/*bltGetCaijiNumģ�麯������ȡ��ǰÿ������Ҫ�ɼ�������*/
	ZL_EXP_VOID global_bltGetCaijiNum(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,single_mod_processNum,0);
	}
	
	/*bltGetCatidģ�麯���� ��ȡ��ǰ�ķ���ID*/
	ZL_EXP_VOID global_bltGetCatid(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		long ret;
		single_mod_catid.ToLong(&ret);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,(int)ret,0);
	}

	/*bltGetModidģ�麯������ȡ��ǰ��ģ��ID*/
	ZL_EXP_VOID global_bltGetModid(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		long ret;
		single_mod_modid.ToLong(&ret);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,(int)ret,0);
	}
	
	/*bltGetCatNameģ�麯������ȡ��ǰ�ɼ��ķ�����*/
	ZL_EXP_VOID global_bltGetCatName(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)single_mod_catname.c_str().AsChar(),0,0);
	}

	/*bltGetWebUrlģ�麯������ȡ��ǰ�ɼ�Ҫ�ϴ���Ŀ����ַ*/
	ZL_EXP_VOID global_bltGetWebUrl(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		global_my_weburl.Replace("http://","");
		if(global_my_weburl.Last()=='/')
			global_my_weburl.RemoveLast();
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)global_my_weburl.c_str().AsChar(),0,0);
	}

	/*bltGetInitManageUrlģ�麯������ȡ��������URL �ڱ��ɼ����м�Ϊ�ɼ��ϴ��ӿڵ��ļ���*/
	ZL_EXP_VOID global_bltGetInitManageUrl(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)gl_initManage_Url.c_str().AsChar(),0,0);
	}

	/*bltGetKeyWordsģ�麯������ȡ����Ĺؼ���*/
	ZL_EXP_VOID global_bltGetKeyWords(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZENGL_EXPORT_MEMBLOCK keywords_memblock = {0},musts_memblock = {0};
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltGetKeyWords������������������");
		zenglApi_GetFunArg(VM_ARG,1,&arg);
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltGetKeyWords������һ���������������飬���Դ�Źؼ�����Ϣ");
		keywords_memblock = arg.val.memblock;
		zenglApi_GetFunArg(VM_ARG,2,&arg);
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltGetKeyWords�����ڶ����������������飬���Դ�Źؼ��ֵı����������Ϣ");
		musts_memblock = arg.val.memblock;
		InitHashMap();
		wxSQLite3Database * db = glDB;
		wxString tmpStr;
		if(!wxFileExists(glSiteDBPath))
		{
			tmpStr = glCaijiWebName + "��վ��Ӧ�ı������ݿ��ļ�"+glSiteDBPath+"�����ڣ�\n";
			zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
			return;
		}
		if(!db->IsOpen())
			db->Open(glSiteDBPath);
		wxString sqlstr="";
		sqlstr.Printf("SELECT * FROM keywords WHERE catid='%s'",single_mod_catid);
		wxSQLite3ResultSet set = db->ExecuteQuery(sqlstr);
		if(set.Eof())
		{
			tmpStr = "bltGetKeyWords��������["+glHashStr[single_mod_modid] + "]ģ��("+single_mod_catname+")����û�йؼ��ʣ�\n";
			zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
			return;
		}
		wxArrayString keywordsArray;
		wxArrayString mustsArray;
		while(set.NextRow())
		{
			keywordsArray.Add(set.GetAsString(1));
			mustsArray.Add(set.GetAsString(3));
		}
		int keyarrayCount = keywordsArray.GetCount();
		int mustarrayCount = mustsArray.GetCount();
		for(int i = 1;i <= keyarrayCount; i++)
		{
			arg.type = ZL_EXP_FAT_STR;
			arg.val.str = (char *)keywordsArray.Item(i - 1).c_str().AsChar();
			zenglApi_SetMemBlock(VM_ARG,&keywords_memblock,i,&arg);
			arg.type = ZL_EXP_FAT_STR;
			arg.val.str = (char *)mustsArray.Item(i - 1).c_str().AsChar();
			zenglApi_SetMemBlock(VM_ARG,&musts_memblock,i,&arg);
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,keyarrayCount,0);
	}
	
	/*bltCurlEncodeģ�麯�������ַ�������URL����*/
	ZL_EXP_VOID global_bltCurlEncode(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltCurlEncode�����������һ������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCurlEncode�����ĵ�һ�������������ַ���");
		if(VM_ARG == CUR_CAIJI_VM)
		{
			char * retstr;
			curl=curl_easy_init();
			retstr = curl_easy_escape(curl,arg.val.str,0);
			curl_easy_cleanup(curl);
			curl = NULL;
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,retstr,0,0);
			curl_free(retstr);
		}
		else
		{
			MyUserExtraData * extraData = (MyUserExtraData *)zenglApi_GetExtraData(VM_ARG,"extraData");
			char * retstr;
			extraData->curl = curl_easy_init();
			retstr = curl_easy_escape(extraData->curl,arg.val.str,0);
			curl_easy_cleanup(extraData->curl);
			extraData->curl = NULL;
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,retstr,0,0);
			curl_free(retstr);
		}
	}
	
	/*bltCurlGetUrlģ�麯��������curlץȡ��ҳurl��վ����*/
	ZL_EXP_VOID global_bltCurlGetUrl(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * url = ZL_EXP_NULL;
		int isForceConv = 0;
		if(argcount != 1 && argcount != 2)
			zenglApi_Exit(VM_ARG,"bltCurlGetUrl�����������һ������������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCurlGetUrl�����ĵ�һ�������������ַ�������ʾҪץȡ����ַ");
		url = arg.val.str;
		if(argcount == 2)
		{
			zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
			if(arg.type != ZL_EXP_FAT_INT)
				zenglApi_Exit(VM_ARG,"bltCurlGetUrl�����ĵڶ���������������������ʾ�Ƿ�ǿ��ת��δʶ��ı���");
			isForceConv = arg.val.integer;
		}
		curl=curl_easy_init();
        global_GetUrlContent(url);
		curl_easy_cleanup(curl);
		curl = NULL;
		if(isForceConv == 0)
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)myglStr.c_str().AsChar(),0,0);
		else
		{
			wchar_t * ws = (wchar_t *)myglStr.wc_str();
			int buf_size = myGetLenWC2GBK(ws);
			if(buf_size == 0)
			{
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,"",0,0);
				return;
			}
			char * buf = (char *)zenglApi_AllocMem(VM_ARG,buf_size);
			if(myConvWC2GBK(ws,buf,buf_size))
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,buf,0,0);
			else
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,"",0,0);
			zenglApi_FreeMem(VM_ARG,buf);
		}
	}
	
	/*bltInfoBoxģ�麯����������Ϣ�Ի�����ʾ��Ϣ*/
	ZL_EXP_VOID global_bltInfoBox(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltInfoBox�����������һ������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltInfoBox�����ĵ�һ�������������ַ�������ʾҪ��ʾ���ַ�����Ϣ");
		wxMessageBox(arg.val.str,"��ӡ��Ϣ");
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltRegexMatchesģ�麯����ƥ��������ʽ������php��preg_match_all*/
	ZL_EXP_VOID global_bltRegexMatches(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZENGL_EXPORT_MEMBLOCK array_memblock = {0};
		ZL_EXP_CHAR * pattern,* content;
		ZL_EXP_INT index;
		ZL_EXP_INT unique_Flag;
		ZL_EXP_INT dotany_Flag;
		if(argcount != 6)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����������6������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵ�һ�������������ַ�������ʾҪƥ���������ʽ");
		pattern = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵڶ���������������������ʾƥ��ڼ���Ԫ��");
		index = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵ����������������ַ�������ʾҪ����ƥ�������");
		content = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,4,&arg); //�õ����ĸ�����
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵ��ĸ���������������������(�������������ڴ������)���������ƥ����");
		array_memblock = arg.val.memblock;
		zenglApi_GetFunArg(VM_ARG,5,&arg); //�õ����������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵ����������������������ʾ�Ƿ���Ҫ�Խ������Ψһ������1��ʾҪΨһ����0��ʾ��Ҫ");
		unique_Flag = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,6,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexMatches�����ĵ�����������������������ʾ�����е�С�����Ƿ�ƥ�任�з���1��ʾƥ�䣬0��ʾ��ƥ��");
		dotany_Flag = arg.val.integer;
		wxRegEx ex;
		wxArrayString tmp_array,final_array;
		wxArrayString * tmp_array_ptr = NULL;
		wxString tmpstr;
		size_t start = 0;
		size_t len = 0;
		size_t prevstart = 0;
		int flags;
		int MatchCount = 0;
		if(dotany_Flag == 1)
			flags = wxRE_ADVANCED | wxRE_ICASE;
		else
			flags = wxRE_ADVANCED | wxRE_NEWLINE | wxRE_ICASE;
		wxString tmpContent = content;
		if(ex.Compile(pattern,flags))
		{
			MatchCount = ex.GetMatchCount();
			if(index >= MatchCount)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltRegexMatches�����ĵڶ���Ԫ�������Ч��������ʽ�й���%d��Ԫ�飬����Ҫ���ʵ�%d��Ԫ�飬Ԫ������ֵ����С����Ԫ����"
									,MatchCount,index);
				return;
			}
			while(ex.Matches(tmpContent.Mid(prevstart)))
			{
				if(ex.GetMatch(&start,&len,index))
				{
					tmp_array.Add(tmpContent.Mid(prevstart + start, len));
				}
				prevstart += start + len;
			}
			if(unique_Flag == 1)
			{
				int count = tmp_array.GetCount();
				for(int i = 0; i < count; ++i)
				{
					tmpstr = tmp_array.Item(i);
					if(final_array.Index(tmpstr) == wxNOT_FOUND)
						final_array.Add(tmpstr);
				}
				tmp_array_ptr = &final_array;
			}
			else
				tmp_array_ptr = &tmp_array;
			int arrayCount = tmp_array_ptr->GetCount();
			for(int i = 1;i <= arrayCount; i++)
			{
				arg.type = ZL_EXP_FAT_STR;
				arg.val.str = (char *)tmp_array_ptr->Item(i - 1).c_str().AsChar();
				zenglApi_SetMemBlock(VM_ARG,&array_memblock,i,&arg);
			}
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,arrayCount,0);
			return;
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}

	/*bltArrayInsertStringģ�麯���������������һ���ַ���*/
	ZL_EXP_VOID global_bltArrayInsertString(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZENGL_EXPORT_MEMBLOCK array_memblock = {0};
		ZL_EXP_CHAR * str;
		ZL_EXP_INT position;
		if(argcount != 3)
			zenglApi_Exit(VM_ARG,"bltArrayInsertString�����������3������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltArrayInsertString�����ĵ�һ����������������������(�������������ڴ������)����ʾҪ�����ݲ��뵽��������");
		array_memblock = arg.val.memblock;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltArrayInsertString�����ĵڶ��������������ַ�������ʾҪ������ַ���");
		str = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltArrayInsertString�����ĵ�����������������������ʾҪ���������λ�ã�0��ʾ���뵽��ͷ���Դ�����");
		position = arg.val.integer;
		ZL_EXP_INT i;
		ZL_EXP_INT array_mblk_count;
		wxArrayString tmpArray;
		zenglApi_GetMemBlockInfo(VM_ARG,&array_memblock,ZL_EXP_NULL,&array_mblk_count);
		for(i=1;i<=array_mblk_count;i++)
		{
			arg = zenglApi_GetMemBlock(VM_ARG,&array_memblock,i);
			if(arg.type != ZL_EXP_FAT_STR)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltArrayInsertString������һ�������������������ݶ��������ַ�������");
				return;
			}
			tmpArray.Add(arg.val.str);
		}
		tmpArray.Insert(str,position);
		ZL_EXP_INT count = tmpArray.GetCount();
		for(i=1;i<=count;i++)
		{
			arg.type = ZL_EXP_FAT_STR;
			arg.val.str = (char *)tmpArray.Item(i - 1).c_str().AsChar();
			zenglApi_SetMemBlock(VM_ARG,&array_memblock,i,&arg);
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,count,0);
	}
	
	/*bltSqlTableExistsģ�麯������sqlite���ݿ��в�ѯĳ��վ��ĳ���Ƿ����*/
	ZL_EXP_VOID global_bltSqlTableExists(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * tablename;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSqlTableExists�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSqlTableExists�����ĵ�һ�������������ַ�������ʾҪ���ı���");
		tablename = arg.val.str;
		wxSQLite3Database * db = glDB;
		wxString tmpStr;
		if(!wxFileExists(glSiteDBPath))
		{
			tmpStr = glCaijiWebName + "��վ��Ӧ�ı������ݿ��ļ�"+glSiteDBPath+"�����ڣ�\n";
			zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
			return;
		}
		if(!db->IsOpen())
			db->Open(glSiteDBPath);
		if(db->TableExists(tablename))
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}

	/*bltSqlEscapeģ�麯�������ַ�������sqlite��ʽ��ת��*/
	ZL_EXP_VOID global_bltSqlEscape(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * content;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSqlEscape�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSqlEscape�����ĵ�һ�������������ַ�������ʾҪ����ת����ַ���");
		content = arg.val.str;
		wxString tmpStr = content;
		tmpStr.Replace("'","''");
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)tmpStr.c_str().AsChar(),0,0);
	}

	/*bltSqlQueryģ�麯������sqlite���ݿ���ִ��sql query��ѯ*/
	ZL_EXP_VOID global_bltSqlQuery(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * sql;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSqlQuery�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSqlQuery�����ĵ�һ�������������ַ�������ʾҪ���в�ѯ��sql���");
		sql = arg.val.str;
		if(VM_ARG == CUR_CAIJI_VM)
		{
			wxSQLite3Database * db = glDB;
			wxString tmpStr;
			if(!wxFileExists(glSiteDBPath))
			{
				tmpStr = glCaijiWebName + "��վ��Ӧ�ı������ݿ��ļ�"+glSiteDBPath+"�����ڣ�\n";
				zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
				return;
			}
			if(!db->IsOpen())
				db->Open(glSiteDBPath);
			//if(gl_isQuerySql_forZenglRun)
			//{
				gl_sqlset_forZenglRun.~wxSQLite3ResultSet();
			//	gl_isQuerySql_forZenglRun = 0;
			//}
			try
			{
				gl_sqlset_forZenglRun = db->ExecuteQuery(wxString::Format("%s",sql));
				gl_isQuerySql_forZenglRun = 1;
			}
			catch(wxSQLite3Exception e)
			{
				gl_isQuerySql_forZenglRun = 0;
				tmpStr = e.GetMessage()+'\n';
				zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
				return;
			}
		}
		else
		{
			MyUserExtraData * extraData = (MyUserExtraData *)zenglApi_GetExtraData(VM_ARG,"extraData");
			wxSQLite3Database * db = &(extraData->db);
			wxString tmpStr;
			if(!wxFileExists(glSiteDBPath))
			{
				tmpStr = glCaijiWebName + "��վ��Ӧ�ı������ݿ��ļ�"+glSiteDBPath+"�����ڣ�\n";
				zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
				return;
			}
			if(!db->IsOpen())
				db->Open(glSiteDBPath);
			//if(extraData->isQuerySql)
			//{
				extraData->sqlset.~wxSQLite3ResultSet();
			//	extraData->isQuerySql = 0;
			//}
			try
			{
				extraData->sqlset = db->ExecuteQuery(wxString::Format("%s",sql));
				extraData->isQuerySql = 1;
			}
			catch(wxSQLite3Exception e)
			{
				extraData->isQuerySql = 0;
				tmpStr = e.GetMessage()+'\n';
				zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
				return;
			}
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltExitģ�麯����ֱ���˳�zengl�ű�*/
	ZL_EXP_VOID global_bltExit(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount > 0)
		{
			zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
			if(arg.type != ZL_EXP_FAT_STR)
				zenglApi_Exit(VM_ARG,"bltExit�����ĵ�һ�������������ַ�������ʾ�˳��ű�ʱ��Ҫ��ʾ����Ϣ");
			zenglApi_Exit(VM_ARG,arg.val.str);
		}
		else
			zenglApi_Exit(VM_ARG,"");
	}
	
	/*bltSqlMoveToNextģ�麯������sqlite�жԽ������MoveToNext�����α��Ƶ���һ����¼*/
	ZL_EXP_VOID global_bltSqlMoveToNext(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltSqlMoveToNext����Ŀǰ�������κβ���");
		if(VM_ARG == CUR_CAIJI_VM)
		{
			if(!gl_isQuerySql_forZenglRun)
			{
				zenglApi_Exit(VM_ARG,"bltSqlMoveToNext����������Ч�Ľ����������ִ��bltSqlQuery���ɽ����\n");
			}
			if(gl_sqlset_forZenglRun.NextRow() == true)
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
			else
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
		}
		else
		{
			MyUserExtraData * extraData = (MyUserExtraData *)zenglApi_GetExtraData(VM_ARG,"extraData");
			if(!extraData->isQuerySql)
				zenglApi_Exit(VM_ARG,"bltSqlMoveToNext����������Ч�Ľ����������ִ��bltSqlQuery���ɽ����\n");
			if(extraData->sqlset.NextRow() == true)
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
			else
				zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
		}
	}

	/*bltSqlGetStringģ�麯������sqlite���ݿ��ȡĳ�н���������ַ�������ʽ���ؽ��*/
	ZL_EXP_VOID global_bltSqlGetString(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * columnName;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSqlGetString�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSqlGetString�����ĵ�һ�������������ַ�������ʾҪ��ѯ������");
		columnName = arg.val.str;
		wxString retStr;
		if(VM_ARG == CUR_CAIJI_VM)
		{
			if(!gl_isQuerySql_forZenglRun)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltSqlGetString����������Ч�Ľ����������ִ��bltSqlQuery���ɽ����\n");
				return;
			}
			try
			{
				retStr = gl_sqlset_forZenglRun.GetAsString(columnName);
			}
			catch(wxSQLite3Exception e)
			{
				retStr = e.GetMessage()+ "\n";
				zenglApi_SetErrThenStop(VM_ARG,(char *)retStr.c_str().AsChar());
				return;
			}
		}
		else
		{
			MyUserExtraData * extraData = (MyUserExtraData *)zenglApi_GetExtraData(VM_ARG,"extraData");
			if(!extraData->isQuerySql)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltSqlGetString����������Ч�Ľ����������ִ��bltSqlQuery���ɽ����\n");
				return;
			}
			try
			{
				retStr = extraData->sqlset.GetAsString(columnName);
			}
			catch(wxSQLite3Exception e)
			{
				retStr = e.GetMessage()+ "\n";
				zenglApi_SetErrThenStop(VM_ARG,(char *)retStr.c_str().AsChar());
				return;
			}
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)retStr.c_str().AsChar(),0,0);
	}

	/*bltSqlReleaseģ�麯������sqlite�жԽ���������ͷ�*/
	ZL_EXP_VOID global_bltSqlRelease(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltSqlRelease����Ŀǰ�������κβ���");
		if(VM_ARG == CUR_CAIJI_VM)
		{
			//if(gl_isQuerySql_forZenglRun)
			//{
				gl_sqlset_forZenglRun.~wxSQLite3ResultSet();
			//	gl_isQuerySql_forZenglRun = 0;
			//}
		}
		else
		{
			MyUserExtraData * extraData = (MyUserExtraData *)zenglApi_GetExtraData(VM_ARG,"extraData");
			//if(extraData->isQuerySql)
			//{
				extraData->sqlset.~wxSQLite3ResultSet();
			//	extraData->isQuerySql = 0;
			//}
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0); //���ýű�AX�Ĵ���������ֵΪ1
	}

	/*bltSqlExecģ�麯������sqlite��ִ�д���������֮��Ĳ���*/
	ZL_EXP_VOID global_bltSqlExec(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * sql;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSqlExec�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSqlExec�����ĵ�һ�������������ַ�������ʾҪִ�е�sql���");
		sql = arg.val.str;
		wxSQLite3Database * db = glDB;
		wxString tmpStr;
		if(!wxFileExists(glSiteDBPath))
		{
			myglStr = glCaijiWebName + "��վ��Ӧ�ı������ݿ��ļ�"+glSiteDBPath+"�����ڣ�\n";
			zenglApi_SetErrThenStop(VM_ARG,(char *)myglStr.c_str().AsChar());
			return;
		}
		if(!db->IsOpen())
			db->Open(glSiteDBPath);
		try
		{
			db->ExecuteUpdate(wxString::Format("%s",sql));
		}
		catch(wxSQLite3Exception e)
		{
			tmpStr = e.GetMessage()+'\n';
			zenglApi_SetErrThenStop(VM_ARG,(char *)tmpStr.c_str().AsChar());
			return;
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0); //���ýű�AX�Ĵ���������ֵΪ1
	}

	/*bltRegexMatchFirstģ�麯����������ʽƥ�䣬����php��preg_match*/
	ZL_EXP_VOID global_bltRegexMatchFirst(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * pattern,* content;
		ZL_EXP_INT index;
		ZL_EXP_INT dotany_Flag;
		if(argcount != 5)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����������5������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����ĵ�һ�������������ַ�������ʾ������ʽ�Ĺ���");
		pattern = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����ĵڶ���������������������ʾƥ��ڼ���Ԫ��");
		index = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����ĵ����������������ַ�������ʾҪ����ƥ�������");
		content = arg.val.str;
		zenglApi_GetFunArgInfo(VM_ARG,4,&arg); //�õ����ĸ�����
		if(arg.type != ZL_EXP_FAT_ADDR && 
			arg.type != ZL_EXP_FAT_ADDR_LOC)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����ĵ��ĸ�����������ȫ�ֱ�����ֲ��������������ͣ��������ƥ����");
		zenglApi_GetFunArg(VM_ARG,5,&arg); //�õ����������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexMatchFirst�����ĵ����������������������ʾ�����е�С�����Ƿ�ƥ�任�з�");
		dotany_Flag = arg.val.integer;
		wxRegEx ex;
		size_t start = 0;
		size_t len = 0;
		size_t prevstart = 0;
		int flags;
		int MatchCount = 0;
		if(dotany_Flag == 1)
			flags = wxRE_ADVANCED | wxRE_ICASE;
		else
			flags = wxRE_ADVANCED | wxRE_NEWLINE | wxRE_ICASE;
		wxString tmpContent = content;
		if(ex.Compile(pattern,flags))
		{
			MatchCount = ex.GetMatchCount();
			if(index >= MatchCount)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltRegexMatchFirst�����ĵڶ���Ԫ�������Ч��������ʽ�й���%d��Ԫ�飬����Ҫ���ʵ�%d��Ԫ�飬Ԫ������ֵ����С����Ԫ����"
									,MatchCount,index);
				return;
			}
			if(ex.Matches(tmpContent.Mid(prevstart)))
			{
				if(ex.GetMatch(&start,&len,index))
				{
					myglStr = tmpContent.Mid(prevstart + start, len);
					arg.type = ZL_EXP_FAT_STR;
					arg.val.str = (char *)myglStr.c_str().AsChar();
					zenglApi_SetFunArg(VM_ARG,4,&arg);
					zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,start,0); //���÷���ֵΪ�ҵ���ƥ���ַ�������ʼ�ַ�������λ��
					return ;
				}
			}
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,-1,0);
	}
	
	/*bltRegexReplaceģ�麯����������ʽ�滻������PHP��preg_replace*/
	ZL_EXP_VOID global_bltRegexReplace(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * pattern = NULL;
		ZL_EXP_CHAR * replace = NULL;
		ZL_EXP_CHAR * content = NULL;
		ZL_EXP_INT dotany_Flag;
		if(argcount != 4)
			zenglApi_Exit(VM_ARG,"bltRegexReplace�����������4������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexReplace�����ĵ�һ�������������ַ�������ʾҪƥ���������ʽ�Ĺ���");
		pattern = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexReplace�����ĵڶ��������������ַ�������ʾҪ�滻���ַ���");
		replace = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltRegexReplace�����ĵ����������������ַ�������ʾҪ����ƥ�������");
		content = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,4,&arg); //�õ����ĸ�����
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltRegexReplace�����ĵ��ĸ�������������������ʾ�����е�С�����Ƿ�ƥ�任�з�");
		dotany_Flag = arg.val.integer;
		wxRegEx ex;
		wxString retStr;
		int flags;
		if(dotany_Flag == 1)
			flags = wxRE_ADVANCED | wxRE_ICASE;
		else
			flags = wxRE_ADVANCED | wxRE_NEWLINE | wxRE_ICASE;
		retStr = content;
		if(ex.Compile(pattern,flags))
		{
			ex.Replace(&retStr,replace);
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)retStr.c_str().AsChar(),0,0); //���ýű�����ֵΪ�滻����ַ���
			return;
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,content,0,0);
	}

	/*bltTrimģ�麯����ȥ�����ҿո�����PHP��trim*/
	ZL_EXP_VOID global_bltTrim(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * content = NULL;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltTrim�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltTrim�����ĵ�һ�������������ַ�������ʾҪ���д��������");
		content = arg.val.str;
		wxString retStr;
		retStr = TrimBoth(content);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)retStr.c_str().AsChar(),0,0);
	}

	/*bltCheckTitleMustģ�麯�������Ա������Ƿ���������*/
	ZL_EXP_VOID global_bltCheckTitleMust(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * title = NULL;
		ZL_EXP_CHAR * must = NULL;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltCheckTitleMust�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCheckTitleMust�����ĵ�һ�������������ַ�������ʾ����");
		title = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCheckTitleMust�����ĵڶ��������������ַ�������ʾ��������Ĵ�");
		must = arg.val.str;
		if(CheckTitleMust(title , must) == true)
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}
	
	/*bltStrReplaceģ�麯�����ַ����滻������PHP��str_replace*/
	ZL_EXP_VOID global_bltStrReplace(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * pattern = NULL;
		ZL_EXP_CHAR * replace = NULL;
		ZL_EXP_CHAR * content = NULL;
		if(argcount != 3)
			zenglApi_Exit(VM_ARG,"bltStrReplace�����������3������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltStrReplace�����ĵ�һ�������������ַ�������ʾҪ�滻�Ĳ���");
		pattern = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltStrReplace�����ĵڶ��������������ַ�������ʾҪ�滻ֵ");
		replace = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltStrReplace�����ĵ����������������ַ�������ʾҪ������ַ���");
		content = arg.val.str;
		wxString retStr;
		retStr = content;
		retStr.Replace(pattern,replace);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)retStr.c_str().AsChar(),0,0);
	}

	/*bltCheckAutoPassģ�麯�����ж��Ƿ���Ҫ�Զ����˲���Ҫ�������*/
	ZL_EXP_VOID global_bltCheckAutoPass(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltCheckAutoPass����Ŀǰ�������κβ���");
		if(glmainFrame->m_checkAutoPass->IsChecked())
		{
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		}
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}
	
	/*bltPostDataģ�麯�����ϴ����ݵ���վ*/
	ZL_EXP_VOID global_bltPostData(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * url = NULL;
		ZENGL_EXPORT_MEMBLOCK memblock;
		ZL_EXP_CHAR * cookie = NULL;
		if(argcount != 3 && argcount != 4)
			zenglApi_Exit(VM_ARG,"bltPostData�����������3��4������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltPostData�����ĵ�һ�������������ַ�������ʾҪ�ϴ���Ŀ����վ��ַ");
		url = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_MEMBLOCK)
			zenglApi_Exit(VM_ARG,"bltPostData�����ĵڶ����������������飬�����֮����ڴ�����ͣ���ʾҪ�ϴ������ݵ���ֵ������");
		memblock = arg.val.memblock;
		zenglApi_GetFunArgInfo(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_ADDR &&
			arg.type != ZL_EXP_FAT_ADDR_LOC)
			zenglApi_Exit(VM_ARG,"bltPostData�����ĵ���������������ȫ�ֱ�����ֲ��������������ͣ��������post���󷵻ص�����\n");
		if(argcount == 4)
		{
			zenglApi_GetFunArg(VM_ARG,4,&arg); //�õ����ĸ�����
			if(arg.type != ZL_EXP_FAT_STR)
				zenglApi_Exit(VM_ARG,"bltPostData�����ĵ��ĸ������������ַ�������ʾcookie��Ϣ");
			cookie = arg.val.str;
		}
		struct curl_httppost *post=NULL;
		struct curl_httppost *last=NULL;
		char * key;
		char * value;
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		int i,mblk_count;
		wxArrayString tmpArray;
		zenglApi_GetMemBlockInfo(VM_ARG,&memblock,ZL_EXP_NULL,&mblk_count);
		for(i=1;i<=mblk_count;i++)
		{
			arg = zenglApi_GetMemBlock(VM_ARG,&memblock,i);
			if(arg.type != ZL_EXP_FAT_STR)
			{
				zenglApi_SetErrThenStop(VM_ARG,"bltPostData�����ĵڶ���������Ӧ����ֵ�������е�����Ԫ�ض��������ַ�������\n");
				return;
			}
			tmpArray.Add(arg.val.str);
		}
		int count = tmpArray.GetCount();
		curl = curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
		if(count<=0)
		{
			zenglApi_SetErrThenStop(VM_ARG,"bltPostData��������ʱ����Ҫ�ϴ������ݶ�Ӧ������û��Ԫ�أ�\n");
			return;
		}
		else if(count%2 != 0)
		{
			zenglApi_SetErrThenStop(VM_ARG,"bltPostData��������ʱ����Ҫ�ϴ������ݶ�Ӧ�������������2�ı������޷�������ֵ�Ե���ʽ��\n");
			return;
		}
		int findpos;
		wxString tmpwxValue;
		for(i=0;i<count;i++)
		{
			key = (char *)tmpArray.Item(i).c_str().AsChar();
			value = (char *)tmpArray.Item(++i).c_str().AsChar();
			//value = (char *)tmpArray.Item(++i).ToUTF8().data();
			tmpwxValue = value;
			if((findpos = tmpwxValue.Find("@"))!=wxNOT_FOUND && findpos == 0)
			{
				tmpwxValue.Replace("@","");
				curl_formadd(&post,&last,CURLFORM_COPYNAME,key,CURLFORM_FILE,(char *)tmpwxValue.c_str().AsChar(), CURLFORM_END);
			}
			else
				curl_formadd(&post,&last,CURLFORM_COPYNAME,key,CURLFORM_COPYCONTENTS,value, CURLFORM_END);
		}
		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
		curl_easy_setopt(curl, CURLOPT_HEADER, 0); 
		long timeout;
		glmainFrame->m_textForTimeOut->GetValue().ToLong(&timeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int)timeout);//���ó�ʱʱ��
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_string_write);
		if(cookie != NULL)
		{
			curl_easy_setopt(curl, CURLOPT_COOKIE, cookie); //����cookie������Ϣ
		}
		CURLcode errcode = curl_easy_perform(curl);
		wxString tmpStr;
		if(errcode != 0)
		{
			eventforlog.SetEventMsg(wxString::Format("error:%s...",curl_easy_strerror(errcode)),MY_RICHTEXT_RED_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
			tmpStr = "";
			curl_easy_cleanup(curl);
			curl_formfree(post);
			curl = NULL;
			char_myglTotalnum = 0;
			arg.type = ZL_EXP_FAT_STR;
			arg.val.str = (char *)tmpStr.c_str().AsChar();
			zenglApi_SetFunArg(VM_ARG,3,&arg);
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
			return;
		}
		if(char_myglStr==NULL)
		{
			charlength = 0;
			eventforlog.SetEventMsg( _("����������Ӧ..."),MY_RICHTEXT_RED_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
			tmpStr = "";
			curl_easy_cleanup(curl);
			curl_formfree(post);
			curl = NULL;
			char_myglTotalnum = 0;
			arg.type = ZL_EXP_FAT_STR;
			arg.val.str = (char *)tmpStr.c_str().AsChar();
			zenglApi_SetFunArg(VM_ARG,3,&arg);
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
			return;
		}
		charlength = strlen(char_myglStr);
		if((myglStr = wxString(char_myglStr)) == "")
			myglStr = wxString::FromUTF8(char_myglStr);
		
		curl_easy_cleanup(curl);
		curl_formfree(post);
		curl = NULL;
		/*if((fp=fopen("test","w"))==NULL)
		{
			curl_easy_cleanup(curl);
			return NULL;
		}
		fwrite( char_myglStr, 1 , char_myglTotalnum ,fp);
		fclose(fp);*/
		free(char_myglStr);
		char_myglStr = NULL;
		char_myglTotalnum = 0;
		arg.type = ZL_EXP_FAT_STR;
		arg.val.str = (char *)myglStr.c_str().AsChar();
		zenglApi_SetFunArg(VM_ARG,3,&arg);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*	bltIntToStrģ�麯����
		������תΪ�ַ�������ʽ������bltIntToStr(25,5,'0')��ô�õ��Ľ�������ַ���'00025'
		��Ϊ�ڶ�������5��ָ�ܿ�ȣ���һ������25�Ŀ��ֻ��2��С��5��
		������߻��Ե���������'0'���롣
	*/
	ZL_EXP_VOID global_bltIntToStr(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char buf[100],dest[100];
		int num,len,i;
		if(argcount != 3)
			zenglApi_Exit(VM_ARG,"bltIntToStr�����������3������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltIntToStr�����ĵ�һ��������������������ʾҪ����ת��������ֵ");
		num = arg.val.integer;
		sprintf(buf,"%d",num);
		len = strlen(buf);
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltIntToStr�����ĵڶ���������������������ʾ�ܿ�ȣ��������Ŀ�Ȳ���ʱ������ఴ�������������в���");
		num = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,3,&arg); //�õ�����������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltIntToStr�����ĵ����������������ַ������ͣ���ʾҪ���в����Ԫ��");
		if(len < num)  //����һ���������ַ������Ȳ���ʱ�����õ��������������롣
		{
			for(i=0;i<num-len;i++)
				dest[i] = arg.val.str[0];
			strncpy(dest+num-len,buf,len);
			dest[num] = '\0';
		}
		if(len < num)
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)dest,0,0);
		else 
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)buf,0,0);
	}

	/*bltAddProgressģ�麯����������Ϊ�������ӽ�����*/
	ZL_EXP_VOID global_bltAddProgress(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		int num;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltAddProgress�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltAddProgress�����ĵ�һ��������������������ʾҪ���ӵĽ���ֵ");
		num = arg.val.integer;
		global_mod_processPos += num;
		single_mod_processPos += num;
		wxMyProgressEvent eventForProgress(wxEVT_MY_PROGRESS_EVENT,ID_MY_WINDOW,single_mod_processPos,single_mod_processNum,global_mod_processPos,global_mod_processNum);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventForProgress.Clone());
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltCheckUserPauseģ�麯�����жϵ�ǰ�û��Ƿ���Ҫ�ڲɼ�����ͣ*/
	ZL_EXP_VOID global_bltCheckUserPause(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltCheckUserPause����Ŀǰ�������κβ���");
		if(glmainFrame->m_checkShowBall->IsChecked())
		{
			wxCommandEvent eventForPause(wxEVT_MY_PAUSE_THREAD,ID_MY_WINDOW);
			wxQueueEvent(glmainFrame->GetEventHandler(),eventForPause.Clone());
			wxThread::Yield();
			wxThread::Sleep(500);
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		}
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}

	/*bltGetMaxUploadIMGģ�麯������ȡ���ݲ������ɼ�������ͼƬ*/
	ZL_EXP_VOID global_bltGetMaxUploadIMG(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltGetMaxUploadIMG����Ŀǰ�������κβ���");
		wxString retStr;
		retStr = glmainFrame->m_textForMaxUploadIMG->GetValue();
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)retStr.c_str().AsChar(),0,0);
	}

	/*bltGetTimeNowģ�麯������ȡϵͳ��ǰʱ��*/
	ZL_EXP_VOID global_bltGetTimeNow(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		struct tm * time_info;
		time_t current_time;
		char timeString[20];
		char * format;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltGetTimeNow�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetTimeNow�����ĵ�һ�������������ַ�������ʾʱ��ĸ�ʽ");
		format = arg.val.str;
		time(&current_time);
		time_info = localtime(&current_time);
		strftime(timeString, sizeof(timeString), format, time_info);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)timeString,0,0);
	}

	/*bltAddProgressExģ�麯�����ϴ�ʱ�������ý�����*/
	ZL_EXP_VOID global_bltAddProgressEx(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		int RowCount;
		int totalRowCount;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltAddProgressEx�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltAddProgressEx�����ĵ�һ��������������������ʾ�ϴ��ļ�¼��");
		RowCount = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltAddProgressEx�����ĵڶ���������������������ʾ�ϴ����ܼ�¼��");
		totalRowCount = arg.val.integer;
		float growSteps = ((float)RowCount/(float)totalRowCount) * (float)single_mod_processNum;
		int old_single_mod_processPos = single_mod_processPos;
		single_mod_processPos = single_mod_processNum + (int)growSteps;
		global_mod_processPos += single_mod_processPos - old_single_mod_processPos;
		wxMyProgressEvent eventForProgress(wxEVT_MY_PROGRESS_EVENT,ID_MY_WINDOW,single_mod_processPos,single_mod_processNum,global_mod_processPos,global_mod_processNum);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventForProgress.Clone());
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltTellSysCaijiStatusģ�麯�������߲ɼ�����ǰ����Ĳɼ����ͷ�����*/
	ZL_EXP_VOID global_bltTellSysCaijiStatus(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		int caijinum;
		int postnum;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltTellSysCaijiStatus�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltTellSysCaijiStatus�����ĵ�һ��������������������ʾ�Ѿ��ɼ�������");
		caijinum = arg.val.integer;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_INT)
			zenglApi_Exit(VM_ARG,"bltTellSysCaijiStatus�����ĵڶ���������������������ʾ�ɹ��ϴ�������");
		postnum = arg.val.integer;
		single_mod_caijinum = caijinum;
		single_mod_curpostnum = postnum;
		global_mod_totalcaijinum += single_mod_caijinum;
		global_mod_totalpostnum += single_mod_curpostnum;
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltConvToIntģ�麯����������תΪ������ʽ*/
	ZL_EXP_VOID global_bltConvToInt(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		int ret;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltConvToInt�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		switch(arg.type)
		{
		case ZL_EXP_FAT_FLOAT:
			ret = (int)arg.val.floatnum;
			break;
		case ZL_EXP_FAT_STR:
			ret = atoi(arg.val.str);
			break;
		case ZL_EXP_FAT_INT:
			ret = arg.val.integer;
			break;
		default:
			zenglApi_Exit(VM_ARG,"bltConvToInt��������ֻ�������������������ַ�������");
			break;
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,ret,0);
	}

	/*bltGetArgStringģ�麯������ȡ�û����ݹ����Ķ������ݣ������ַ���ָ�����ʽ���أ�������ȷ���ö����������ַ�������*/
	ZL_EXP_VOID global_bltGetArgString(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * extraName;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltGetArgString�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetArgString�����ĵ�һ�������������ַ�������ʾ�û������д��ݸ��ű��Ķ�������");
		extraName = arg.val.str;
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(ZL_EXP_CHAR *)zenglApi_GetExtraData(VM_ARG,extraName),0,0);
	}

	/*bltStrFindģ�麯���������ַ���*/
	ZL_EXP_VOID global_bltStrFind(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * content;
		char * find;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltStrFind�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltStrFind�����ĵ�һ�������������ַ�������ʾҪ�ڸ������н��в���");
		content = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltStrFind�����ĵڶ��������������ַ�������ʾҪ���ҵ�����");
		find = arg.val.str;
		wxString string = content;
		int findpos;
		if((findpos = string.Find(find))!=wxNOT_FOUND)
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,findpos,0);
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,-1,0);
	}

	/*bltLaunchDefaultBrowserģ�麯��������Ĭ�������*/
	ZL_EXP_VOID global_bltLaunchDefaultBrowser(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * url;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltLaunchDefaultBrowser�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltLaunchDefaultBrowser�����ĵ�һ�������������ַ�������ʾurl��ַ");
		url = arg.val.str;
		wxLaunchDefaultBrowser(url);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltToUTF8ģ�麯����תΪUTF8����*/
	ZL_EXP_VOID global_bltToUTF8(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * inputstr;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltToUTF8�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltToUTF8�����ĵ�һ�������������ַ�������ʾҪ����ת�����ַ���");
		inputstr = arg.val.str;
		wxString inputStr = inputstr;
		wxScopedCharBuffer inputStrForUTF8 = inputStr.ToUTF8();
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)inputStrForUTF8.data(),0,0);
	}

	/*bltSetInitManageUrlģ�麯�������ù����õ���ַURL�����ɼ����ڷ���˵ķ����ӿ�*/
	ZL_EXP_VOID global_bltSetInitManageUrl(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * url;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltSetInitManageUrl�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSetInitManageUrl�����ĵ�һ�������������ַ�������ʾ�ɼ����ڷ���˵ķ����ӿ��ļ����磺zengl_caiji.php");
		url = arg.val.str;
		gl_initManage_Url = url;
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltSetModulePathģ�麯��������ģ���Ӧ�Ĺ����ļ���*/
	ZL_EXP_VOID global_bltSetModulePath(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * modname;
		char * filepath;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltSetModulePath�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSetModulePath�����ĵ�һ�������������ַ�������ʾģ����");
		modname = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltSetModulePath�����ĵڶ��������������ַ�������ʾ�ɼ�����ű��ļ���·��");
		filepath = arg.val.str;
		wxString tmp_modname = modname;
		wxString tmp_filepath = filepath;
		if(tmp_modname == "article")
			gl_zenglrun_article_modname = tmp_filepath;
		else if(tmp_modname == "brand")
			gl_zenglrun_brand_modname = tmp_filepath;
		else if(tmp_modname == "buy")
			gl_zenglrun_buy_modname = tmp_filepath;
		else if(tmp_modname == "exhibit")
			gl_zenglrun_exhibit_modname = tmp_filepath;
		else if(tmp_modname == "group")
			gl_zenglrun_group_modname = tmp_filepath;
		else if(tmp_modname == "info")
			gl_zenglrun_info_modname = tmp_filepath;
		else if(tmp_modname == "job")
			gl_zenglrun_job_modname = tmp_filepath;
		else if(tmp_modname == "know")
			gl_zenglrun_know_modname = tmp_filepath;
		else if(tmp_modname == "mall")
			gl_zenglrun_mall_modname = tmp_filepath;
		else if(tmp_modname == "quote")
			gl_zenglrun_quote_modname = tmp_filepath;
		else if(tmp_modname == "sell")
			gl_zenglrun_sell_modname = tmp_filepath;
		else if(tmp_modname == "special")
			gl_zenglrun_special_modname = tmp_filepath;
		else if(tmp_modname == "photo")
			gl_zenglrun_photo_modname = tmp_filepath;
		else if(tmp_modname == "video")
			gl_zenglrun_video_modname = tmp_filepath;
		else if(tmp_modname == "down")
			gl_zenglrun_down_modname = tmp_filepath;
		else
		{
			wxString errStr = "������("+tmp_modname+")ģ����";
			zenglApi_SetErrThenStop(VM_ARG,(char *)errStr.c_str().AsChar());
			return;
		}
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
	}

	/*bltGetAreaID����ȡ����ID�����������ID*/
	ZL_EXP_VOID global_bltGetAreaID(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * areaname;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltGetAreaID�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetAreaID�����ĵ�һ�������������ַ�������ʾ������");
		areaname = arg.val.str;
		wxString tmpStr = GetAreaID(areaname);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)tmpStr.c_str().AsChar(),0,0);
	}

	/*bltGetAreaID_NoRandom����ȡ����ID*/
	ZL_EXP_VOID global_bltGetAreaID_NoRandom(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * areaname;
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltGetAreaID_NoRandom�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetAreaID_NoRandom�����ĵ�һ�������������ַ�������ʾ������");
		areaname = arg.val.str;
		wxString tmpStr = GetAreaID_NoRandom(areaname);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)tmpStr.c_str().AsChar(),0,0);
	}

	/*bltGetAreaName_NoRandom����ȡ������*/
	ZL_EXP_VOID global_bltGetAreaName_NoRandom(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		char * areaname,* defaultName;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltGetAreaName_NoRandom�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetAreaName_NoRandom�����ĵ�һ�������������ַ�������ʾ������");
		areaname = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetAreaName_NoRandom�����ĵڶ��������������ַ�������ʾû�ҵ�ʱ��Ҫ���ص�Ĭ������");
		defaultName = arg.val.str;
		wxString tmpStr = GetAreaName_NoRandom(areaname,defaultName);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)tmpStr.c_str().AsChar(),0,0);
	}

	/*bltCheckCaijiCompanyģ�麯�����ж��Ƿ���Ҫ������˾��Ϣ*/
	ZL_EXP_VOID global_bltCheckCaijiCompany(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltCheckCaijiCompany����Ŀǰ�������κβ���");
		if(glmainFrame->m_checkCaijiCompany->IsChecked())
		{
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		}
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}

	/*bltGetCompanyCatidģ�麯������ȡҪ�������Ĺ�˾�ķ���ID*/
	ZL_EXP_VOID global_bltGetCompanyCatid(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		if(argcount != 0)
			zenglApi_Exit(VM_ARG,"bltGetCompanyCatid����Ŀǰ�������κβ���");
		long ret;
		MyTreeCtrl * treeCtrl = glmainFrame->treeCtrl;
		wxTreeItemId rootid = treeCtrl->GetRootItem();
		wxTreeItemIdValue cookie,cookie2;
		wxTreeItemId cateitemid = treeCtrl->GetFirstChild(rootid,cookie),cateitemChild;
		MyTreeItemData * itemdata = NULL;
		do
		{
			if(!cateitemid.IsOk())
				break;
			cateitemChild = treeCtrl->GetFirstChild(cateitemid,cookie2);
			do
			{
				if(!cateitemChild.IsOk())
					break;
				itemdata = (MyTreeItemData *)treeCtrl->GetItemData(cateitemChild);
				if(itemdata->m_modID == "4" && treeCtrl->GetItemState(cateitemChild) == 1)
				{
					glCompanyCatID = itemdata->m_cateID;
					glCompanyCatID.ToLong(&ret);
					zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,(int)ret,0);
					return ;
				}
			}while(cateitemChild = treeCtrl->GetNextChild(cateitemChild,cookie2));
		}while(cateitemid = treeCtrl->GetNextChild(cateitemid,cookie));
		glCompanyCatID = "3";
		glCompanyCatID.ToLong(&ret);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,(int)ret,0);
	}

	/*bltBase64Encodeģ�麯�������ַ�����Ϣ����Base64����*/
	ZL_EXP_VOID global_bltBase64Encode(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltBase64Encode�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltBase64Encode�����ĵ�һ�������������ַ�������ʾҪ���б�����ַ���");
		int len = strlen(arg.val.str);
		wchar_t * output = (wchar_t *)zenglApi_AllocMem(VM_ARG,(((len + 2) / 3) * 4 + 1) * sizeof(wchar_t));
		if(BASE64_Encode((unsigned char *)arg.val.str,len,output) == -1)
			zenglApi_Exit(VM_ARG,"bltBase64Encode��������ʧ�ܣ�");
		wxString wxOutput = output;
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,(char *)wxOutput.c_str().AsChar(),0,0);
		zenglApi_FreeMem(VM_ARG,output);
	}

	/*bltBase64Decodeģ�麯������Base64���뻹ԭΪ�ַ���*/
	ZL_EXP_VOID global_bltBase64Decode(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltBase64Decode�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltBase64Decode�����ĵ�һ�������������ַ�������ʾҪ�����BASE64����");
		wxString wxInput = arg.val.str;
		int len = (int)wxInput.Length();
		int output_len = (len >> 2) * 3;
		char * output = (char *)zenglApi_AllocMem(VM_ARG,(output_len + 1) * sizeof(char));
		int ret = 0;
		ret = BASE64_Decode((wchar_t *)wxInput.wc_str(),len,(unsigned char *)output);
		if(ret == -1)
		{
			zenglApi_SetErrThenStop(VM_ARG,"bltBase64Decode��������ʱ�����ڲ���������");
			return;
		}
		else if(ret == -2)
		{
			zenglApi_SetErrThenStop(VM_ARG,"bltBase64Decode����������Ч��BASE64����");
			return;
		}
		output[output_len] = '\0';
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_STR,output,0,0);
		zenglApi_FreeMem(VM_ARG,output);
	}

	/*bltCurlDownloadģ�麯������url������ĳ���ļ�*/
	ZL_EXP_VOID global_bltCurlDownload(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		ZL_EXP_CHAR * dest_filename;
		ZL_EXP_CHAR * url;
		if(argcount != 2)
			zenglApi_Exit(VM_ARG,"bltCurlDownload�����������2������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCurlDownload�����ĵ�һ�������������ַ�������ʾ���غ�Ҫ������ļ���");
		dest_filename = arg.val.str;
		zenglApi_GetFunArg(VM_ARG,2,&arg); //�õ��ڶ�������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltCurlDownload�����ĵڶ��������������ַ�������ʾurl���ص�ַ");
		url = arg.val.str;
		curl=curl_easy_init();
        curl_easy_setopt(curl, CURLOPT_URL,url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
		long timeout;
		glmainFrame->m_textForTimeOut->GetValue().ToLong(&timeout);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int)timeout);//���ó�ʱʱ��
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_file_write);
		FILE * fp;
		if((fp=fopen(dest_filename,"wb+"))==NULL)
		{
			curl_easy_cleanup(curl);
			curl = NULL;
			myglStyle = MY_RICHTEXT_RED_STYLE;
			myglStr = _("����Ϊ�ļ� <") + dest_filename + _("> ʧ�ܣ�\n");
			wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
			wxQueueEvent(glmainFrame->GetEventHandler(),event.Clone());
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,-1,0);
			return;
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_perform(curl);
		curl_easy_cleanup(curl);
		curl = NULL;
		fclose(fp);
		int fileSize = myGetFileSize(dest_filename);
		zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,fileSize,0);
	}

	/*bltGetMenuCheckģ�麯�����ж�ĳ���˵����Ƿ�checkѡ�У�����״̬*/
	ZL_EXP_VOID global_bltGetMenuCheck(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount)
	{
		ZENGL_EXPORT_MOD_FUN_ARG arg = {ZL_EXP_FAT_NONE,{0}};
		if(argcount != 1)
			zenglApi_Exit(VM_ARG,"bltGetMenuCheck�����������1������");
		zenglApi_GetFunArg(VM_ARG,1,&arg); //�õ���һ������
		if(arg.type != ZL_EXP_FAT_STR)
			zenglApi_Exit(VM_ARG,"bltGetMenuCheck�����ĵ�һ�������������ַ�������ʾҪ���Ĳ˵���");
		wxString menuName = arg.val.str;
		wxMenuBar * menubar = glmainFrame->GetMenuBar();
		int menuid;
		if(menuName == "���򷢲�")
			menuid = ID_MENU_EXTRA_REVERSE_ORDER;
		else if(menuName == "�����������")
			menuid = ID_MENU_EXTRA_POST_PENDING;
		else if(menuName == "ͼƬ���ص��������ϴ�")
			menuid = ID_MENU_EXTRA_LOCAL_IMG;
		else
		{
			zenglApi_SetErrThenStop(VM_ARG,"bltGetMenuCheck������һ��������Ӧ�Ĳ˵���'%s'���ԣ����߸ò˵����޷�����ѡ�д�״̬",menuName.c_str().AsChar());
			return;
		}
		if(menubar->IsChecked(menuid))
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,1,0);
		else
			zenglApi_SetRetVal(VM_ARG,ZL_EXP_FAT_INT,ZL_EXP_NULL,0,0);
	}

	ZL_EXP_VOID global_builtin_module_init(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT moduleID)
	{
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"printf",global_printf);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"read",global_read);
		//zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRandom",global_bltRandom);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRandom",zenglApiBMF_bltRandom);
		//zenglApi_SetModFunHandle(VM_ARG,moduleID,"array",global_array);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"array",zenglApiBMF_array);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"unset",zenglApiBMF_unset);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltPrintArray",global_bltPrintArray);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltTestAddr",global_bltTestAddr);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSetArray",global_bltSetArray);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltLoadScript",global_bltLoadScript);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetZLVersion",global_bltGetZLVersion);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetCaijiNum",global_bltGetCaijiNum);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetCatid",global_bltGetCatid);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetModid",global_bltGetModid);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetCatName",global_bltGetCatName);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetWebUrl",global_bltGetWebUrl);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetInitManageUrl",global_bltGetInitManageUrl);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetKeyWords",global_bltGetKeyWords);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCurlEncode",global_bltCurlEncode);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCurlGetUrl",global_bltCurlGetUrl);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltInfoBox",global_bltInfoBox);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRegexMatches",global_bltRegexMatches);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltArrayInsertString",global_bltArrayInsertString);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlTableExists",global_bltSqlTableExists);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlEscape",global_bltSqlEscape);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlQuery",global_bltSqlQuery);
		//zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltExit",global_bltExit);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltExit",zenglApiBMF_bltExit);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlMoveToNext",global_bltSqlMoveToNext);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlGetString",global_bltSqlGetString);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlRelease",global_bltSqlRelease);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlExec",global_bltSqlExec);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRegexMatchFirst",global_bltRegexMatchFirst);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltRegexReplace",global_bltRegexReplace);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltTrim",global_bltTrim);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCheckTitleMust",global_bltCheckTitleMust);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltStrReplace",global_bltStrReplace);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCheckAutoPass",global_bltCheckAutoPass);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltPostData",global_bltPostData);
		//zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltIntToStr",global_bltIntToStr);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltIntToStr",zenglApiBMF_bltIntToStr);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltAddProgress",global_bltAddProgress);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCheckUserPause",global_bltCheckUserPause);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetMaxUploadIMG",global_bltGetMaxUploadIMG);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetTimeNow",global_bltGetTimeNow);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltAddProgressEx",global_bltAddProgressEx);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltTellSysCaijiStatus",global_bltTellSysCaijiStatus);
		//zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltConvToInt",global_bltConvToInt);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltConvToInt",zenglApiBMF_bltConvToInt);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetArgString",global_bltGetArgString);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltStrFind",global_bltStrFind);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltLaunchDefaultBrowser",global_bltLaunchDefaultBrowser);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltToUTF8",global_bltToUTF8);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSetInitManageUrl",global_bltSetInitManageUrl);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSetModulePath",global_bltSetModulePath);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetAreaID",global_bltGetAreaID);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetAreaID_NoRandom",global_bltGetAreaID_NoRandom);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetAreaName_NoRandom",global_bltGetAreaName_NoRandom);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCheckCaijiCompany",global_bltCheckCaijiCompany);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetCompanyCatid",global_bltGetCompanyCatid);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltBase64Encode",global_bltBase64Encode);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltBase64Decode",global_bltBase64Decode);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCurlDownload",global_bltCurlDownload);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetMenuCheck",global_bltGetMenuCheck);
	}

	void global_JumpToCaiji_InitFuncall(void * VM_ARG)
	{
		int moduleID = 0;
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltGetArgString",global_bltGetArgString);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlEscape",global_bltSqlEscape);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlQuery",global_bltSqlQuery);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltExit",global_bltExit);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlMoveToNext",global_bltSqlMoveToNext);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltConvToInt",global_bltConvToInt);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlGetString",global_bltSqlGetString);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltStrFind",global_bltStrFind);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltCurlEncode",global_bltCurlEncode);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltLaunchDefaultBrowser",global_bltLaunchDefaultBrowser);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltToUTF8",global_bltToUTF8);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltInfoBox",global_bltInfoBox);
		zenglApi_SetModFunHandle(VM_ARG,moduleID,"bltSqlRelease",global_bltSqlRelease);
	}

	ZL_EXP_VOID global_module_init(ZL_EXP_VOID * VM_ARG)
	{
		zenglApi_SetModInitHandle(VM_ARG,"builtin",global_builtin_module_init);
	}

	ZL_EXP_VOID global_print_debug(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * debug_str)
	{
		ZENGL_EXPORT_MOD_FUN_ARG reg_debug;
		ZL_EXP_INT debug_str_len = strlen(debug_str);
		ZL_EXP_BOOL has_semi = ZL_EXP_FALSE;
		wxMyLogEvent mylogevent( wxEVT_MY_LOG_EVENT,wxID_ANY);
		zenglApi_GetDebug(VM_ARG,&reg_debug);
		if(debug_str[debug_str_len-1]==';')
		{
			debug_str[debug_str_len-1] = ' ';
			has_semi = ZL_EXP_TRUE;
		}
		mylogevent.SetEventMsg(wxString::Format("%s:",debug_str),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
		switch(reg_debug.type)
		{
		case ZL_EXP_FAT_NONE:
			mylogevent.SetEventMsg(wxString::Format("none type , number equal %d",reg_debug.val.integer),MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		case ZL_EXP_FAT_INT:
			mylogevent.SetEventMsg(wxString::Format("integer:%d",reg_debug.val.integer),MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		case ZL_EXP_FAT_FLOAT:
			mylogevent.SetEventMsg(wxString::Format("float:%.16g",reg_debug.val.floatnum),MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		case ZL_EXP_FAT_STR:
			mylogevent.SetEventMsg(wxString::Format("string:%s",reg_debug.val.str),MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		case ZL_EXP_FAT_MEMBLOCK:
			global_print_array(VM_ARG,reg_debug.val.memblock,0);
			break;
		case ZL_EXP_FAT_ADDR:
		case ZL_EXP_FAT_ADDR_LOC:
		case ZL_EXP_FAT_ADDR_MEMBLK:
			mylogevent.SetEventMsg("addr type",MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		case ZL_EXP_FAT_INVALID:
			mylogevent.SetEventMsg("invalid type",MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
			break;
		}
		mylogevent.SetEventMsg("\n",MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
		if(has_semi == ZL_EXP_TRUE)
		{
			debug_str[debug_str_len-1] = ';';
			has_semi = ZL_EXP_FALSE;
		}
	}

	/*�ݹ鷵��������Ϣ*/
	ZL_EXP_VOID global_retStr_debugArray(ZL_EXP_VOID * VM_ARG,ZENGL_EXPORT_MEMBLOCK memblock,ZL_EXP_INT recur_count,ZL_EXP_VOID * retStr)
	{
		ZL_EXP_INT size,i,j;
		ZENGL_EXPORT_MOD_FUN_ARG mblk_val = {ZL_EXP_FAT_NONE,{0}};
		wxString result = "";
		wxString tmp = "";
		zenglApi_GetMemBlockInfo(VM_ARG,&memblock,&size,ZL_EXP_NULL);
		for(i=1;i<=size;i++)
		{
			mblk_val = zenglApi_GetMemBlock(VM_ARG,&memblock,i);
			switch(mblk_val.type)
			{
			case ZL_EXP_FAT_INT:
			case ZL_EXP_FAT_FLOAT:
			case ZL_EXP_FAT_STR:
			case ZL_EXP_FAT_MEMBLOCK:
				for(j=0;j<recur_count;j++)
				{
					result += "  ";
				}
				break;
			}
			switch(mblk_val.type)
			{
			case ZL_EXP_FAT_INT:
				result += wxString::Format("[%d] %d\n",i-1,mblk_val.val.integer);
				break;
			case ZL_EXP_FAT_FLOAT:
				result += wxString::Format("[%d] %.16g\n",i-1,mblk_val.val.floatnum);
				break;
			case ZL_EXP_FAT_STR:
				result += wxString::Format("[%d] %s\n",i-1,mblk_val.val.str);
				break;
			case ZL_EXP_FAT_MEMBLOCK:
				result += wxString::Format("[%d] <array or class obj type> begin:\n",i-1);
				global_retStr_debugArray(VM_ARG,mblk_val.val.memblock,recur_count+1,&tmp);
				result += tmp;
				result += wxString::Format("[%d] <array or class obj type> end\n",i-1);
				break;
			}
		}
		(*(wxString *)retStr) = result;
		return;
	}

	ZL_EXP_VOID global_retStr_debug(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * debug_str,ZL_EXP_VOID * retStr)
	{
		ZENGL_EXPORT_MOD_FUN_ARG reg_debug;
		ZL_EXP_INT debug_str_len = strlen(debug_str);
		ZL_EXP_BOOL has_semi = ZL_EXP_FALSE;
		wxString result = "";
		wxString tmp = "";
		zenglApi_GetDebug(VM_ARG,&reg_debug);
		if(debug_str[debug_str_len-1]==';')
		{
			debug_str[debug_str_len-1] = ' ';
			has_semi = ZL_EXP_TRUE;
		}
		result += wxString::Format("%s:",debug_str);
		switch(reg_debug.type)
		{
		case ZL_EXP_FAT_NONE:
			result += wxString::Format("none type , number equal %d",reg_debug.val.integer);
			break;
		case ZL_EXP_FAT_INT:
			result += wxString::Format("integer:%d",reg_debug.val.integer);
			break;
		case ZL_EXP_FAT_FLOAT:
			result += wxString::Format("float:%.16g",reg_debug.val.floatnum);
			break;
		case ZL_EXP_FAT_STR:
			result += wxString::Format("string:%s",reg_debug.val.str);
			break;
		case ZL_EXP_FAT_MEMBLOCK:
			global_retStr_debugArray(VM_ARG,reg_debug.val.memblock,0,&tmp);
			result += tmp;
			break;
		case ZL_EXP_FAT_ADDR:
		case ZL_EXP_FAT_ADDR_LOC:
		case ZL_EXP_FAT_ADDR_MEMBLK:
			result += _("addr type");
			break;
		case ZL_EXP_FAT_INVALID:
			result += _("invalid type");
			break;
		}
		result += ("\n");
		if(has_semi == ZL_EXP_TRUE)
		{
			debug_str[debug_str_len-1] = ';';
			has_semi = ZL_EXP_FALSE;
		}
		(*(wxString *)retStr) = result;
		return;
	}

	ZL_EXP_VOID global_debug_printlog(wxMyLogEvent &mylogevent,wxString log)
	{
		mylogevent.SetEventMsg(log,MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
	}

	ZL_EXP_CHAR * global_getDebugArg(ZL_EXP_CHAR * str,ZL_EXP_INT * start,ZL_EXP_BOOL needNull)
	{
		int i;
		char * ret;
		if((*start) < 0)
		{
			(*start) = -1;
			return ZL_EXP_NULL;
		}
		for(i=(*start);;i++)
		{
			if(str[i] == ' ' || str[i] == '\t')
				continue;
			else if(str[i] == STRNULL)
			{
				(*start) = -1;
				return ZL_EXP_NULL;
			}
			else
			{
				ret = str + i;
				while(++i)
				{
					if(str[i] == ' ' || str[i] == '\t')
					{
						if(needNull != ZL_EXP_FALSE)
							str[i] = STRNULL;
						(*start) = i+1;
						break;
					}
					else if(str[i] == STRNULL)
					{
						(*start) = -1;
						break;
					}
				}
				return ret;
			} //else
		}//for(i=(*start);;i++)
		(*start) = -1;
		return ZL_EXP_NULL;
	}

	ZL_EXP_BOOL global_isNumber(ZL_EXP_CHAR * str)
	{
		int len = strlen(str);
		int i;
		for(i=0;i<len;i++)
		{
			if(!isdigit(str[i]))
				return ZL_EXP_FALSE;
		}
		return ZL_EXP_TRUE;
	}

	ZL_EXP_INT global_debug_break(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * cur_filename,ZL_EXP_INT cur_line,ZL_EXP_INT breakIndex,ZL_EXP_CHAR * log)
	{
		wxMyLogEvent mylogevent( wxEVT_MY_LOG_EVENT,wxID_ANY);
		char * str = ZL_EXP_NULL;
		char * inputstr_ptr = ZL_EXP_NULL;
		char * tmpstr = ZL_EXP_NULL;
		char * command,* arg;
		int i,start;
		int inputlength = 0;
		int exit = 0;
		int str_size = 0;
		int str_count = 0;
		int tmplen;
		if(log != ZL_EXP_NULL)
		{
			if(zenglApi_Debug(VM_ARG,log) == -1)
			{
				global_debug_printlog(mylogevent,wxString::Format("log��־�ϵ����%s",zenglApi_GetErrorString(VM_ARG)));
			}
			else
			{
				global_print_debug(VM_ARG,log);
				return 0;
			}
		}
		global_debug_printlog(mylogevent,wxString::Format("* %s:%d ",cur_filename,cur_line));
		if(breakIndex == -1)
			global_debug_printlog(mylogevent,wxString::Format("Single Break [current]\n"));
		else
			global_debug_printlog(mylogevent,wxString::Format("Break index:%d [current]\n",breakIndex));
		if(str == ZL_EXP_NULL)
		{
			str_size = DEBUG_INPUT_MAX;
			str = (char *)zenglApi_AllocMem(VM_ARG,str_size);
		}
		while(!exit)
		{
			if(glmainFrame->m_isDebugPause == false && glmainFrame->mythread2 != NULL && !glmainFrame->mythread2->IsPaused())
			{
				wxCommandEvent eventForPause(wxEVT_MY_DEBUG_PAUSE_THREAD,ID_MY_WINDOW);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventForPause.Clone());
				wxThread::Yield();
				wxThread::Sleep(500);
				wxString inputstr = glmainFrame->textForDebug->GetValue();
				mylogevent.SetEventMsg(inputstr+"\n",MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(glmainFrame->GetEventHandler(),mylogevent.Clone());
				inputstr_ptr = (char *)inputstr.c_str().AsChar();
				inputlength = strlen(inputstr_ptr);
				for(i=0;i < inputlength;i++)
				{
					if(i >= str_size - 10) //i�������һ��Ԫ��ʱ����str�������ݣ�str_size - 10��str������β��Ԥ��һ�οռ�
					{
						str_size += DEBUG_INPUT_MAX;
						tmpstr = (char *)zenglApi_AllocMem(VM_ARG,str_size);
						strcpy(tmpstr,str);
						zenglApi_FreeMem(VM_ARG,str);
						str = tmpstr;
					}
					str[i] = inputstr_ptr[i];
				}
				str[i] = STRNULL;
				str_count = i;
				start = 0;
				command = global_getDebugArg(str,&start,ZL_EXP_TRUE);
				if(command == ZL_EXP_NULL || strlen(command) != 1)
				{
					global_debug_printlog(mylogevent,wxString::Format("���������һ���ַ�\n"));
					continue;
				}
				switch(command[0])
				{
				case 'P':
				case 'p':
					{
						arg = global_getDebugArg(str,&start,ZL_EXP_FALSE);
						tmplen = arg != ZL_EXP_NULL ? strlen(arg) : 0;
						if(arg != ZL_EXP_NULL && tmplen > 0)
						{
							if(arg[tmplen - 1] != ';' && str_count < str_size - 1)
							{
								arg[tmplen] = ';';
								arg[tmplen+1] = STRNULL;
							}
							if(zenglApi_Debug(VM_ARG,arg) == -1)
							{
								global_debug_printlog(mylogevent,wxString::Format("p���Դ���%s\n",zenglApi_GetErrorString(VM_ARG)));
								continue;
							}
							if(command[0] == 'P')
							{
								wxString retStr;
								global_retStr_debug(VM_ARG,arg,&retStr);
								wxMessageBox(retStr,"��ӡ������Ϣ");
							}
							else
								global_print_debug(VM_ARG,arg);
						}
						else
							global_debug_printlog(mylogevent,wxString::Format("p����ȱ�ٲ���\n"));
					}
					break;
				case 'b':
					{
						char * filename = ZL_EXP_NULL;
						int line = 0;
						int count = 0;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0)
							filename = arg;
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("b����ȱ���ļ�������\n"));
							continue;
						}
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0)
							line = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("b����ȱ���кŲ���\n"));
							continue;
						}
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0)
							count = atoi(arg);
						if(zenglApi_DebugSetBreak(VM_ARG,filename,line,ZL_EXP_NULL,ZL_EXP_NULL,count,ZL_EXP_FALSE) == -1)
							global_debug_printlog(mylogevent,wxString::Format("b����error:%s\n",zenglApi_GetErrorString(VM_ARG)));
						else
							global_debug_printlog(mylogevent,wxString::Format("���öϵ�ɹ�\n"));
					}
					break;
				case 'B':
					{
						int size;
						int totalcount;
						int i;
						char * filename = ZL_EXP_NULL;
						char * condition = ZL_EXP_NULL;
						char * log = ZL_EXP_NULL;
						int count;
						int line;
						ZL_EXP_BOOL disabled;
						if(breakIndex == -1)
							global_debug_printlog(mylogevent,wxString::Format("* %s:%d Single Break [current]\n",cur_filename,cur_line));
						size = zenglApi_DebugGetBreak(VM_ARG,-1,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,&totalcount,ZL_EXP_NULL,ZL_EXP_NULL);
						for(i=0;i<size;i++)
						{
							if(zenglApi_DebugGetBreak(VM_ARG,i,&filename,&line,&condition,&log,&count,&disabled,ZL_EXP_NULL) == -1)
								continue;
							else
							{
								global_debug_printlog(mylogevent,wxString::Format("[%d] %s:%d",i,filename,line));
								if(condition != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format(" C:%s",condition));
								if(log != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format(" L:%s",log));
								global_debug_printlog(mylogevent,wxString::Format(" N:%d",count));
								if(disabled == ZL_EXP_FALSE)
									global_debug_printlog(mylogevent,wxString::Format(" D:enable"));
								else
									global_debug_printlog(mylogevent,wxString::Format(" D:disable"));
								if(i == breakIndex)
									global_debug_printlog(mylogevent,wxString::Format(" [current]"));
								global_debug_printlog(mylogevent,wxString::Format("\n"));
							}
						}
						global_debug_printlog(mylogevent,wxString::Format("total:%d\n",totalcount));
					}
					break;
				case 'T':
					{	
						int arg = -1;
						int loc = -1;
						int pc = -1;
						int ret;
						int line = 0;
						char * fileName = ZL_EXP_NULL;
						char * className = ZL_EXP_NULL;
						char * funcName = ZL_EXP_NULL;
						while(ZL_EXP_TRUE)
						{
							ret = zenglApi_DebugGetTrace(VM_ARG,&arg,&loc,&pc,&fileName,&line,&className,&funcName);
							if(ret == 1)
							{
								global_debug_printlog(mylogevent,wxString::Format(" %s:%d ",fileName,line));
								if(className != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format("%s:",className));
								if(funcName != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format("%s",funcName));
								global_debug_printlog(mylogevent,wxString::Format("\n"));
								continue;
							}
							else if(ret == 0)
							{
								global_debug_printlog(mylogevent,wxString::Format(" %s:%d ",fileName,line));
								if(className != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format("%s:",className));
								if(funcName != ZL_EXP_NULL)
									global_debug_printlog(mylogevent,wxString::Format("%s",funcName));
								global_debug_printlog(mylogevent,wxString::Format("\n"));
								break;
							}
							else if(ret == -1)
							{
								global_debug_printlog(mylogevent,wxString::Format("%s",zenglApi_GetErrorString(VM_ARG)));
								break;
							}
						}
					}
					break;
				case 'r':
					{
						int arg = -1;
						int loc = -1;
						int pc = -1;
						int tmpPC;
						int ret;
						int size,i;
						int line = 0;
						char * fileName = ZL_EXP_NULL;
						ZL_EXP_BOOL hasBreaked = ZL_EXP_FALSE;
						ret = zenglApi_DebugGetTrace(VM_ARG,&arg,&loc,&pc,&fileName,&line,ZL_EXP_NULL,ZL_EXP_NULL);
						if(ret == 1)
						{
							zenglApi_DebugGetTrace(VM_ARG,&arg,&loc,&pc,&fileName,&line,ZL_EXP_NULL,ZL_EXP_NULL);
							pc++;
							size = zenglApi_DebugGetBreak(VM_ARG,-1,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL);
							for(i=0;i<size;i++)
							{
								if(zenglApi_DebugGetBreak(VM_ARG,i,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,&tmpPC) == -1)
									continue;
								else if(pc == tmpPC)
								{
									hasBreaked = ZL_EXP_TRUE;
									break;
								}
							}
							if(!hasBreaked)
							{
								if(zenglApi_DebugSetBreakEx(VM_ARG,pc,ZL_EXP_NULL,ZL_EXP_NULL,1,ZL_EXP_FALSE) == -1)
									global_debug_printlog(mylogevent,wxString::Format("%s",zenglApi_GetErrorString(VM_ARG)));
								else
									exit = 1;
							}
							else
								exit = 1;
						}
						else if(ret == 0)
							exit = 1;
						else if(ret == -1)
							global_debug_printlog(mylogevent,wxString::Format("%s",zenglApi_GetErrorString(VM_ARG)));
					}
					break;
				case 'd':
					{
						int index;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							index = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("d����ȱ�ٶϵ���������\n"));
							continue;
						}
						if(zenglApi_DebugDelBreak(VM_ARG,index) == -1)
							global_debug_printlog(mylogevent,wxString::Format("d����error:��Ч�Ķϵ�����"));
						else
							global_debug_printlog(mylogevent,wxString::Format("ɾ���ϵ�ɹ�"));
						global_debug_printlog(mylogevent,wxString::Format("\n"));
					}
					break;
				case 'D':
					{
						int index;
						char * filename = ZL_EXP_NULL;
						char * condition = ZL_EXP_NULL;
						char * log = ZL_EXP_NULL;
						int count;
						int line;
						ZL_EXP_BOOL disabled;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							index = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("D����ȱ�ٶϵ���������\n"));
							continue;
						}
						if(zenglApi_DebugGetBreak(VM_ARG,index,&filename,&line,&condition,&log,&count,&disabled,ZL_EXP_NULL) == -1)
						{
							global_debug_printlog(mylogevent,wxString::Format("D����error:��Ч�Ķϵ�����\n"));
							continue;
						}
						else
						{
							if(zenglApi_DebugSetBreak(VM_ARG,filename,line,condition,log,count,ZL_EXP_TRUE) == -1)
								global_debug_printlog(mylogevent,wxString::Format("D������öϵ�error:%s",zenglApi_GetErrorString(VM_ARG)));
							else
								global_debug_printlog(mylogevent,wxString::Format("D������öϵ�ɹ�"));
							global_debug_printlog(mylogevent,wxString::Format("\n"));
						}
					}
					break;
				case 'C':
					{
						int index;
						char * newCondition;
						char * filename = ZL_EXP_NULL;
						char * condition = ZL_EXP_NULL;
						char * log = ZL_EXP_NULL;
						int count;
						int line;
						ZL_EXP_BOOL disabled;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							index = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("C����ȱ�ٶϵ���������\n"));
							continue;
						}
						arg = global_getDebugArg(str,&start,ZL_EXP_FALSE);
						tmplen = arg != ZL_EXP_NULL ? strlen(arg) : 0;
						if(arg != ZL_EXP_NULL && tmplen > 0)
						{
							if(arg[tmplen - 1] != ';' && str_count < str_size - 1)
							{
								arg[tmplen] = ';';
								arg[tmplen+1] = STRNULL;
							}
							newCondition = arg;
						}
						else
							newCondition = ZL_EXP_NULL;
						if(zenglApi_DebugGetBreak(VM_ARG,index,&filename,&line,&condition,&log,&count,&disabled,ZL_EXP_NULL) == -1)
						{
							global_debug_printlog(mylogevent,wxString::Format("C����error:��Ч�Ķϵ�����\n"));
							continue;
						}
						else
						{
							if(zenglApi_DebugSetBreak(VM_ARG,filename,line,newCondition,log,count,disabled) == -1)
								global_debug_printlog(mylogevent,wxString::Format("C�������������ϵ�error:%s",zenglApi_GetErrorString(VM_ARG)));
							else
								global_debug_printlog(mylogevent,wxString::Format("C�������������ϵ�ɹ�"));
							global_debug_printlog(mylogevent,wxString::Format("\n"));
						}
					}
					break;
				case 'L':
					{
						int index;
						char * newLog;
						char * filename = ZL_EXP_NULL;
						char * condition = ZL_EXP_NULL;
						char * log = ZL_EXP_NULL;
						int count;
						int line;
						ZL_EXP_BOOL disabled;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							index = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("L����ȱ�ٶϵ���������\n"));
							continue;
						}
						arg = global_getDebugArg(str,&start,ZL_EXP_FALSE);
						tmplen = arg != ZL_EXP_NULL ? strlen(arg) : 0;
						if(arg != ZL_EXP_NULL && tmplen > 0)
						{
							if(arg[tmplen - 1] != ';' && str_count < str_size - 1)
							{
								arg[tmplen] = ';';
								arg[tmplen+1] = STRNULL;
							}
							newLog = arg;
						}
						else
							newLog = ZL_EXP_NULL;
						if(zenglApi_DebugGetBreak(VM_ARG,index,&filename,&line,&condition,&log,&count,&disabled,ZL_EXP_NULL) == -1)
						{
							global_debug_printlog(mylogevent,wxString::Format("L����error:��Ч�Ķϵ�����\n"));
							continue;
						}
						else
						{
							if(zenglApi_DebugSetBreak(VM_ARG,filename,line,condition,newLog,count,disabled) == -1)
								global_debug_printlog(mylogevent,wxString::Format("L����������־�ϵ�error:%s",zenglApi_GetErrorString(VM_ARG)));
							else
								global_debug_printlog(mylogevent,wxString::Format("L����������־�ϵ�ɹ�"));
							global_debug_printlog(mylogevent,wxString::Format("\n"));
						}
					}
					break;
				case 'N':
					{
						int index;
						int newCount;
						char * filename = ZL_EXP_NULL;
						char * condition = ZL_EXP_NULL;
						char * log = ZL_EXP_NULL;
						int count;
						int line;
						ZL_EXP_BOOL disabled;
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							index = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("N����ȱ�ٶϵ���������\n"));
							continue;
						}
						arg = global_getDebugArg(str,&start,ZL_EXP_TRUE);
						if(arg != ZL_EXP_NULL && strlen(arg) > 0 && global_isNumber(arg))
							newCount = atoi(arg);
						else
						{
							global_debug_printlog(mylogevent,wxString::Format("N����ȱ�ٶϵ��������\n"));
							continue;
						}
						if(zenglApi_DebugGetBreak(VM_ARG,index,&filename,&line,&condition,&log,&count,&disabled,ZL_EXP_NULL) == -1)
						{
							global_debug_printlog(mylogevent,wxString::Format("N����error:��Ч�Ķϵ�����\n"));
							continue;
						}
						else
						{
							if(zenglApi_DebugSetBreak(VM_ARG,filename,line,condition,log,newCount,disabled) == -1)
								global_debug_printlog(mylogevent,wxString::Format("N�������öϵ����error:%s",zenglApi_GetErrorString(VM_ARG)));
							else
								global_debug_printlog(mylogevent,wxString::Format("N�������öϵ�����ɹ�"));
							global_debug_printlog(mylogevent,wxString::Format("\n"));
						}
					}
					break;
				case 's':
					zenglApi_DebugSetSingleBreak(VM_ARG,ZL_EXP_TRUE);
					exit = 1;
					break;
				case 'S':
					zenglApi_DebugSetSingleBreak(VM_ARG,ZL_EXP_FALSE);
					exit = 1;
					break;
				case 'c':
					exit = 1;
					break;
				case 'h':
					global_debug_printlog(mylogevent,wxString::Format(" p ���Ա�����Ϣ usage:p express\n"
						" P ��������Ϣ�����InfoBox�Ի��� usage:P express\n"
						" b ���öϵ� usage:b filename lineNumber\n"
						" B �鿴�ϵ��б� usage:B\n"
						" T �鿴�ű������Ķ�ջ������Ϣ usage:T\n"
						" d ɾ��ĳ�ϵ� usage:d breakIndex\n"
						" D ����ĳ�ϵ� usage:D breakIndex\n"
						" C ���������ϵ� usage:C breakIndex condition-express\n"
						" L ������־�ϵ� usage:L breakIndex log-express\n"
						" N ���öϵ���� usage:N breakIndex count\n"
						" s �������� usage:s\n"
						" S �������� usage:S\n"
						" r ִ�е����� usage:r\n"
						" c ����ִ�� usage:c\n"));
					break;
				default:
					global_debug_printlog(mylogevent,wxString::Format("��Ч������\n"));
					break;
				}//switch(command[0])
			}//if(glmainFrame->mythread2 != NULL && !glmainFrame->mythread2->IsPaused())
			else
			{
				break;
			}
		} //while(!exit)
		if(str != ZL_EXP_NULL)
			zenglApi_FreeMem(VM_ARG,str);
		return 0;
	}

	ZL_EXP_INT global_debug_conditionError(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * filename,ZL_EXP_INT line,ZL_EXP_INT breakIndex,ZL_EXP_CHAR * error)
	{
		ZL_EXP_CHAR * condition;
		zenglApi_DebugGetBreak(VM_ARG,breakIndex,ZL_EXP_NULL,ZL_EXP_NULL,&condition,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL,ZL_EXP_NULL);
		myglStyle = MY_RICHTEXT_NORMAL_STYLE;
		myglStr = wxString::Format("\n%s [%d] <%d %s> error:%s\n",filename,line,breakIndex,condition,error);
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
		wxQueueEvent(glmainFrame->GetEventHandler(),event.Clone());
		return 0;
	}
}

bool global_checkTitleRepeat(wxString baseurl,wxString title,wxString catid)
{
	wxString detectUrl = "http://"+ baseurl +"/mydetectTitle.php";
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
	curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
	curl_easy_setopt(curl, CURLOPT_URL, detectUrl.c_str().AsChar());
	wxString mypost_mod_table;
	if(single_mod_modid >= "21")
		mypost_mod_table = single_mod_cat_table + "_" + single_mod_modid;
	else
		mypost_mod_table = single_mod_cat_table;
	curl_formadd(&post,&last,CURLFORM_COPYNAME,"_zlmy_detectTable",CURLFORM_COPYCONTENTS,mypost_mod_table.c_str().AsChar(), CURLFORM_END);
	curl_formadd(&post,&last,CURLFORM_COPYNAME,"_zlmy_detectTitle",CURLFORM_COPYCONTENTS,title.c_str().AsChar(), CURLFORM_END);
	curl_formadd(&post,&last,CURLFORM_COPYNAME,"_zlmy_catid",CURLFORM_COPYCONTENTS,catid.c_str().AsChar(), CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0); 
	long timeout;
	glmainFrame->m_textForTimeOut->GetValue().ToLong(&timeout);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int)timeout);//���ó�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_string_write);
	CURLcode errcode = curl_easy_perform(curl);
	if(errcode != 0 || char_myglStr==NULL)
	{
		eventforlog.SetEventMsg(wxString::Format("error:%s...",curl_easy_strerror(errcode)),MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		charlength = 0;
		curl_easy_cleanup(curl);
		curl_formfree(post);
		curl = NULL;
		char_myglTotalnum = 0;
		return false;
	}
	charlength = strlen(char_myglStr);
	if((myglStr = wxString(char_myglStr)) == "")
		myglStr = wxString::FromUTF8(char_myglStr);
	
	curl_easy_cleanup(curl);
	curl_formfree(post);
	curl = NULL;
	free(char_myglStr);
	char_myglStr = NULL;
	char_myglTotalnum = 0;
	return true;
}

inline bool IsSpace( IN wxChar ch )
{
    return ch == wxT(' ');

}

inline wxString TrimLeft( IN const wxString& str )
{
    const wxChar * pszStr = str.c_str();
    for ( int index = (int) str.Length() - 1; index >= 0; index -- )
    {
        if ( ! IsSpace( pszStr[index] ) )
        {
            return str.Left( index + 1 );
        }
    }
    return wxEmptyString;

}

inline wxString TrimRight( IN const wxString& str )
{
    const wxChar * pszStr = str.c_str();
    for ( int index = 0; index < (int) str.Length(); index ++ )
    {
        if ( ! IsSpace( pszStr[index] ) )
        {
            return str.Right( str.Length() - index );
        }
    }
    return wxEmptyString;

}

wxString TrimBoth( IN const wxString& str )
{
    return TrimLeft( TrimRight( str ) );

} 

void delScript(wxString * str)
{
	wxRegEx ex;
	wxString script_pattern = "<script.*>.*</script>";
	wxString script_pattern2 = "on(mousewheel|mouseover|click|load|onload|submit|focus|blur)=\"[^\"]*\"";
	if(ex.Compile(script_pattern,wxRE_ADVANCED | wxRE_ICASE))
	{
		ex.Replace(str,"");
	}
	if(ex.Compile(script_pattern2,wxRE_ADVANCED | wxRE_ICASE))
	{
		ex.Replace(str,"");
	}
}

bool CheckTitleMust(wxString title , wxString must)
{
	bool totalbooler=true;
	bool tmpbooler=false;
	wxString tmpstr;
	tmpstr = "";
	const wxChar * pszStr = must.c_str();
	int mustlength = (int)must.Length();
	for ( int index = 0; index < mustlength; index ++ )
    {
        if ( pszStr[index]==wxT('|'))
        {
			if(title.Find(tmpstr) == wxNOT_FOUND)
				tmpbooler |= false;
			else
				tmpbooler |= true;
			tmpstr = "";
        }
		else if( pszStr[index]==wxT('&'))
		{
			if(title.Find(tmpstr) == wxNOT_FOUND)
				tmpbooler |= false;
			else
				tmpbooler |= true;
			if(!(totalbooler && tmpbooler))
				return false;
			tmpstr = "";
			tmpbooler = false;
		}
		else
		{
			tmpstr += pszStr[index];
			if(index == mustlength - 1)
			{
				if(title.Find(tmpstr) == wxNOT_FOUND)
					tmpbooler |= false;
				else
					tmpbooler |= true;
				if(!(totalbooler && tmpbooler))
					return false;
				tmpstr = "";
				tmpbooler = false;
			}
		}
    }
	return true;
}

bool GetAllAreas()
{
	wxXmlDocument doc;
	wxMyLogEvent eventforlog(wxEVT_MY_LOG_EVENT,wxID_ANY);
	eventforlog.SetEventMsg("׼�����ص����ļ�area.xml...\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	if (!doc.Load("area.xml"))
	{
		eventforlog.SetEventMsg("����area.xml�ļ�ʧ��\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return false;
	}
	wxXmlNode *root = doc.GetRoot();
	wxXmlNode *child = root->GetChildren();
	if(global_MyAreas.GetCount() > 0)
		global_MyAreas.Clear();
	while(child)
	{
		global_MyAreas.Add(new MyAreaObj(child->GetAttribute("name"),child->GetAttribute("areaid")));
		child = child->GetNext();
	}
	eventforlog.SetEventMsg(wxString::Format("���ص����ļ��ɹ���һ��������%d����������\n",global_MyAreas.GetCount()),MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	return true;
}

wxString GetAreaID_NoRandom(wxString strArg)
{
	int areaCount = (int)global_MyAreas.GetCount();
	for(int i=0;i < areaCount;i++)
	{
		if(strArg.Find(global_MyAreas[i].areaName) != wxNOT_FOUND)
		{
			return global_MyAreas[i].areaID;
		}
	}
	return "0";
}

wxString GetAreaName_NoRandom(wxString strArg,wxString defaultRet)
{
	int areaCount = (int)global_MyAreas.GetCount();
	for(int i=0;i < areaCount;i++)
	{
		if(strArg.Find(global_MyAreas[i].areaName) != wxNOT_FOUND)
		{
			return global_MyAreas[i].areaName;
		}
	}
	return defaultRet;
}

wxString GetAreaID(wxString strArg)
{
	int areaCount = (int)global_MyAreas.GetCount();
	for(int i=0;i < areaCount;i++)
	{
		if(strArg.Find(global_MyAreas[i].areaName) != wxNOT_FOUND)
		{
			return global_MyAreas[i].areaID;
		}
	}
	return (wxString::Format("%d",GetRandomNum()%391 + 1));
}

int GetRandomNum()
{
	if(global_seed == 0)
	{
		time_t current_time;
		time(&current_time);
		global_seed = (unsigned int)current_time;
	}
	srand(global_seed);
	return (global_seed = rand());
}


char *global_rand_str(char *str,const int len)
{
   int i;
   for(i=0;i<len;++i)
       str[i]='a'+rand()%26;
   str[i]='\0';
   return str;
}