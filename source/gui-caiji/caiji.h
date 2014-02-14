#ifndef   _ZL_CAIJI_H_
#define   _ZL_CAIJI_H_

#define FILE_BUFLEN  1024

class MyApp : public wxApp
{
public:
	virtual bool OnInit();
	virtual void OnFatalException();
	wxSQLite3Database *db;
};

class MyStatusBar : public wxStatusBar
{
public:
    MyStatusBar(int fieldnum,wxWindow *parent, wxWindowID id, long style = wxST_SIZEGRIP, const wxString& name = _T("statusBar"));

    void OnSize(wxSizeEvent& event);
	void OnUpdateProgress(wxCommandEvent& event);

    enum
    {
        Field_Text1,
		Field_Text2,
        Field_Process,
		Field_Percent
    };
public:
    wxGauge    *m_ProgressBar;
	int m_total;
	int m_pos;

    DECLARE_EVENT_TABLE()
};

class MyTreeCtrl : public wxTreeCtrl
{
public:
	MyTreeCtrl(){}
	MyTreeCtrl(wxWindow *parent,const wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style);
	virtual ~MyTreeCtrl(){};

	void OnItemStateClick(wxTreeEvent& event);
	void OnItemActivated(wxTreeEvent& event);

private:
	//DECLARE_DYNAMIC_CLASS(MyTreeCtrl)
    DECLARE_EVENT_TABLE()
};

class MyTreeItemData : public wxTreeItemData
{
public:
	MyTreeItemData(const wxString& modID,const wxString& cateID,const wxString& catname,const wxString& url) : m_modID(modID),m_cateID(cateID),m_catname(catname),m_url(url) { }
   const wxString& GetModID() const { return m_modID; }
public:
   wxString m_modID;
   wxString m_cateID;
   wxString m_catname;
   wxString m_url;
}; 

class MyTaskBarIcon;

class MyFrame: public wxFrame
{
public:

    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);
	~MyFrame();

	wxFont myfont; //���οؼ�����
	wxTextCtrl * textUrlName; //������ַ�������
	wxRichTextCtrl * textForLog; //�����ϸ��־
	wxTextCtrl * textForDebug; //�������������
	//wxTextCtrl * textPatternTest; //�������
	MyTreeCtrl * treeCtrl; //���οؼ���ʾ������Ϣ��
	MyStatusBar * m_statusbar;
	wxAuiToolBar * m_toolbar;
	wxAuiToolBar * m_toolbar2;
	//wxGauge * m_gauge;
	wxThread *mythread;
	wxThread *mythread2;
	wxTextCtrl *m_textpostnum;
	wxString m_DefPerspective;
	wxGauge    *m_ProgressBar;
	MyTaskBarIcon   *m_taskBarIcon;
	wxCheckBox * m_checkShowBall;
	wxCheckBox * m_checkCaijiCompany; //�ж��Ƿ�ɼ���˾�ĸ�ѡ��
	wxCheckBox * m_checkAutoPass; //�ж��Ƿ����Զ����˲���Ҫ�������
	wxTextCtrl * m_textForTimeOut; //��ʱ
	wxTextCtrl * m_textForMaxUploadIMG; //����ϴ�ͼƬ��
	wxFindReplaceData m_findData;
	wxFindReplaceDialog *m_dlgFind; //���ҷ���Ի���
	bool m_isDebugPause; //������ͣ��־
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
	void OnHelp(wxCommandEvent& event); //�򿪰����ֲ�
	void OnUrlNameBtn(wxAuiToolBarEvent& event); //��ȡ������Ϣ��ť����¼�
	void OnTextCtrlEnter(wxCommandEvent& event);
	void OnTextDebugEnter(wxCommandEvent& event); //���Կ��а��»س�ʱ
	//void OnMyEvent(wxCommandEvent& event);
	void OnMyEvent(wxMyLogEvent& event);
	void OnUpdateCate(wxCommandEvent& event);
	void OnChecks(wxCommandEvent& event);
	void OnClickStart(wxAuiToolBarEvent& event);
	void OnClearLogs(wxCommandEvent& event);
	void OnClickPause(wxAuiToolBarEvent& event);
	void OnClickStop(wxAuiToolBarEvent& event);
	void OnClickTest(wxAuiToolBarEvent& event);
	void OnRestorePespective(wxCommandEvent& event);
	void OnClickResults(wxAuiToolBarEvent& event);
	void OnClickShowLog(wxAuiToolBarEvent& event);
	void OnClickWebOffice(wxAuiToolBarEvent& event);
	void OnMinimize(wxIconizeEvent& event);
	void OnCaijiProgress(wxMyProgressEvent& event);
	void OnPauseThread(wxCommandEvent& event);
	void OnDebugPauseThread(wxCommandEvent& event);
	void OnClearAll_Logs(wxCommandEvent& event);
	void OnClickCheckCaijiCompany(wxCommandEvent& event); //����ɼ���˾��ѡ��ʱ�Ĵ���
	void OnResetInitScript(wxAuiToolBarEvent& event); //���¼��س�ʼ���ű�
	void OnFindCategory(wxCommandEvent& event); //����ĳ������
	void OnFindDialog(wxFindDialogEvent& event); //���ҷ����¼�����
	wxTreeItemId findTreeItem(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, 
								   const wxString& text, bool bIsNext,bool bCaseSensitive, bool bExactMatch); //�ڷ������н��в���
	void OnSetCurlNum(wxCommandEvent& event); //����ץ��������
	void OnCategorySelectAll(wxCommandEvent& event); //ȫѡ�����б�
	void OnCategoryUnSelectAll(wxCommandEvent& event); //ȫ��ѡ�����б�
	void OnCategoryExpandAll(wxCommandEvent& event); //ȫ��չ��
	void OnCategoryUnExpandAll(wxCommandEvent& event); //ȫ���۵�
	void OnExtraCheck(wxCommandEvent& event); //�ɼ����⹦��

public:
	wxAuiManager m_mgr;
    DECLARE_EVENT_TABLE()
};

class MyThread : public wxThread
{
public:
	MyThread(MyFrame * frame);
	virtual void *Entry();
	virtual void OnExit();
public:
	MyFrame *m_frame;
};

class MyThread2 : public wxThread
{
public:
	MyThread2(MyFrame * frame);
	virtual void *Entry();
	virtual void OnExit();
public:
	MyFrame *m_frame;
};

class MyKeyDialog : public wxDialog
{
public:
	MyKeyDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid,wxString modid,wxString catname);

	bool Create();
	void OnEditKey(wxCommandEvent& event);
	void OnNewKey(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);
	void OnShowData(wxCommandEvent& event);
	void OnJumpToCaijiUrl(wxCommandEvent& event);
	void OnOk(wxCommandEvent& event);
	bool RefreshListBox(int index);

public:
	wxListBox * m_listbox;
	wxButton * m_new;
	wxButton * m_edit;
	wxButton * m_delete;
	wxButton * m_showdata;
	wxButton * m_jumpToCaijiUrl;
	wxButton * m_ok;
	wxButton * m_cancel;
	wxString m_catid;
	wxString m_modid;
	wxString m_catname;

	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyKeyDialog);
};

class MyKeyWordDialog : public wxDialog
{
public:
	MyKeyWordDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString id);

	bool Create(wxString keyword,wxString mustText,wxString needSuffix);
	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

public:
	wxTextCtrl * m_keyword;
	wxTextCtrl * m_mustText;
	wxComboBox * m_needSuffix;
	wxButton * m_ok;
	wxButton * m_cancel;
	wxString m_id;

	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyKeyWordDialog);
};

class MyNewKeyWordDialog : public wxDialog
{
public:
	MyNewKeyWordDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid);
	bool Create();
	void OnOk(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);

public:
	wxTextCtrl * m_keyword;
	wxTextCtrl * m_mustText;
	wxComboBox * m_needSuffix;
	wxButton * m_ok;
	wxButton * m_cancel;
	wxString m_catid;

	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyNewKeyWordDialog);
};

class MyTestDialog : public wxDialog
{
public:
	MyTestDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size);
	bool Create();
	void OnTest(wxCommandEvent& event);
	void OnCancel(wxCommandEvent& event);
	void OnLoadTestFile(wxCommandEvent& event);
	//void OnSize(wxSizeEvent& event);

public:
	wxTextCtrl * m_express;
	wxTextCtrl * m_url;
	wxTextCtrl * m_log;
	wxComboBox * m_dot;

	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyTestDialog);
};

class MyResultsDialog : public wxDialog
{
public:
	MyResultsDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size);
	bool Create();
	bool RefreshListBox(int index);
	void OnListboxSelected(wxCommandEvent& event);
	void OnViewContent(wxCommandEvent& event);
	void OnClose(wxCommandEvent& event);
	void OnDelete(wxCommandEvent& event);

public:
	wxListBox * m_listbox;
	wxButton * m_delete;
	wxButton * m_close;
	wxButton * m_view;
	wxTextCtrl * m_logs;
	wxStaticText * m_static_title;
	wxStaticText * m_catname;
	wxTextCtrl * m_title_text;
	
	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyResultsDialog);
};

class MyGridDialog : public wxDialog
{
public:
	MyGridDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid,wxString modid,wxString catname);
	bool Create();
	void OnClickPre(wxCommandEvent& event);
	void OnClickNext(wxCommandEvent& event);
	void OnClickJump(wxCommandEvent& event);
	void OnClickDelete(wxCommandEvent& event);
	void OnClickClearData(wxCommandEvent& event);
	void OnGridChanging(wxGridEvent& event);
	void OnGridChanged(wxGridEvent& event);
	void RefreshGridData();
	void DeleteData(int row);
	
	enum 
	{
		MYGRID_DATA_SHOWNUM = 90
	};

public:
	wxGrid * m_grid;
	wxTextCtrl * m_page;
	wxStaticText * m_pagenum;
	wxRadioBox * m_radio;
	wxString m_catid;
	wxString m_modid;
	wxString m_catname;

	DECLARE_EVENT_TABLE()
    wxDECLARE_NO_COPY_CLASS(MyGridDialog);
};

class MyAreaObj
{
public:
	MyAreaObj(wxString strName,wxString strID);

	wxString areaName;
	wxString areaID;
};

WX_DECLARE_OBJARRAY(MyAreaObj,ArrayOfMyAreaObj);

enum 
{
	MY_RICHTEXT_NORMAL_STYLE,
	MY_RICHTEXT_RED_STYLE,
	MY_RICHTEXT_GREEN_STYLE
};

enum
{
    ID_Quit = 1,
    ID_About,
	IDCaiji_Tree_Ctrl =	1000,
	IDUrl_Name_TextCtrl = 1001,
	IDUrlName_Btn = 1002,
	ID_MY_WINDOW = 1003,
	ID_ITEM_START = 1004,
	ID_TEXT_PATTERN = 1005,
	ID_KEY_NEW = 1006,
	ID_KEY_EDIT = 1007,
	ID_KEY_DELETE = 1008,
	ID_KEY_OK = 1009,
	ID_KEY_CANCEL = 1010,
	ID_KEYWORD_OK =1011,
	ID_KEYWORD_CANCEL = 1012,
	ID_KEY_LISTBOX = 1013,
	ID_NEWKEY_OK = 1014,
	ID_NEWKEY_CANCEL = 1015,
	ID_ITEM_PAUSE = 1016,
	ID_ITEM_STOP = 1017,
	ID_TEST_CLICK = 1018,
	ID_TEST_CANCEL = 1019,
	ID_ITEM_TEST = 1020,
	ID_TEST_LOADFILE = 1021,
	ID_ITEM2_RESULT = 1022,
	ID_MENU_RESTORE_PERSPECTIVE = 1023,
	ID_RESULTS_BUTTON_DELETE = 1024,
	ID_RESULTS_BUTTON_VIEW = 1025,
	ID_RESULTS_BUTTON_CLOSE = 1026,
	ID_KEY_SHOWDATA = 1027,
	ID_SHOWDATA_PRE = 1028,
	ID_SHOWDATA_NEXT = 1029,
	ID_SHOWDATA_JUMP = 1030,
	ID_SHOWDATA_DELETE = 1031,
	ID_RADIOBOX = 1032,
	ID_SHOWDATA_CLEARBTN = 1033,
	ID_KEY_JUMPTO_CAIJI_URL = 1034,
	ID_ITEM_SHOW_LOG = 1035,
	ID_ITEM_WEB_OFFICE = 1036,
	ID_MENU_CLEAR_LOGS = 1037,
	ID_CHECK_CAIJI_COMPANY = 1038,
	ID_ITEM_RESET_INIT = 1039,
	ID_MENU_CATEGORY_FIND = 1040,
	ID_MENU_CATEGORY_SELECTALL = 1041,
	ID_MENU_CATEGORY_UNSELECTALL = 1042,
	ID_MENU_CATEGORY_EXPANDALL = 1043,
	ID_MENU_CATEGORY_UNEXPANDALL = 1044,
	ID_MENU_EXTRA_REVERSE_ORDER = 1045,
	ID_MENU_EXTRA_POST_PENDING = 1046,
	ID_MENU_EXTRA_LOCAL_IMG = 1047,
	ID_MENU_FILE_HELP = 1048,
	ID_TEXT_FOR_DEBUG = 1049,
	ID_MENU_EXTRA_DEBUG = 1050,
};

DECLARE_EVENT_TYPE(wxEVT_MY_EVENT, -1)
DECLARE_EVENT_TYPE(wxEVT_MY_UPDATECATE, -2)
DECLARE_EVENT_TYPE(wxEVT_MY_CHECKS, -3)
DECLARE_EVENT_TYPE(wxEVT_MY_PROGRESS, -4)
DECLARE_EVENT_TYPE(wxEVT_MY_CLEARLOGS, -5)
DECLARE_EVENT_TYPE(wxEVT_MY_CAIJI_PROGRESS, -6)
DECLARE_EVENT_TYPE(wxEVT_MY_PAUSE_THREAD, -7)
DECLARE_EVENT_TYPE(wxEVT_MY_DEBUG_PAUSE_THREAD, -8)

extern "C" size_t mywxcurl_string_write(void* ptr, size_t size, size_t nmemb, void* pcharbuf);
extern "C" size_t mywxcurl_file_write(void *ptr, size_t size, size_t nmemb, void *stream);
extern bool GetAllAreas();
extern void global_GetUrlContent(wxString remote_url);
extern wxString TrimBoth( IN const wxString& str );

extern wxString myglStr;
extern int myglStyle;
extern MyFrame *glmainFrame;
extern wxSQLite3Database * glDB;
extern MyHashString glHashStr;
extern wxString global_SettingFile;
extern wxSQLite3Database  *settingDB;
extern int global_mod_processNum;
extern int global_mod_processPos;
extern int global_mod_totalpostnum;
extern int global_mod_totalcaijinum;
extern int single_mod_processNum;
extern int single_mod_processPos;
extern int single_mod_curpostnum;
extern int single_mod_caijinum;
extern wxString single_mod_cat_table;
extern CURL * curl;
extern char * char_myglStr;
extern int charlength;
extern FILE *fp;
extern int char_myglTotalnum;
extern bool glIsStop;
extern wxString glSiteDBPath;
extern wxString glCaijiWebName;
extern wxString single_mod_catname;
extern wxString single_mod_catid;
extern wxString single_mod_modid;
extern wxString ContactContent;
extern ArrayOfMyAreaObj global_MyAreas;
extern MyHashString global_MyModules;
extern MyHashString glMyModuleNames;
extern unsigned int global_seed;
extern wxString global_my_weburl;
extern wxString gl_zenglrun_article_modname;
extern wxString gl_zenglrun_brand_modname; 
extern wxString gl_zenglrun_buy_modname;
extern wxString gl_zenglrun_exhibit_modname;
extern wxString gl_zenglrun_group_modname;
extern wxString gl_zenglrun_info_modname;
extern wxString gl_zenglrun_job_modname;
extern wxString gl_zenglrun_know_modname;
extern wxString gl_zenglrun_mall_modname;
extern wxString gl_zenglrun_quote_modname;
extern wxString gl_zenglrun_sell_modname;
extern wxString gl_zenglrun_special_modname;
extern wxString gl_zenglrun_photo_modname;
extern wxString gl_zenglrun_video_modname;
extern wxString gl_zenglrun_down_modname;
//func.c
extern "C" void myexit_true(char * err,...);

//run.h
typedef enum _MEMOPTYPE{  //�ڴ��������
	ADDMEM_INT,ADDMEM_DOUBLE,ADDMEM_PTR,GETMEM,SETMEM_INT,SETMEM_DOUBLE,SETMEM_PTR,
	ADDMEM_ADDR,SETMEM_ADDR,ADDMEM_MEMBLOCK,SETMEM_MEMBLOCK
}MEMOPTYPE;
typedef enum _IDTYPE{  //�ڴ棬�Ĵ������ֵ��ö�����ͣ�����ʱ���ͣ������������㣬�ַ��������ã������ڴ��ȡ�
	IDINT,IDFLOAT,IDSTR,IDADDR,IDADDR_LOC,IDADDR_MEMBLK,IDMEM_BLOCK
}IDTYPE;
typedef struct _MEM_STRUCT
{
	IDTYPE idtype;
	bool isvalid; //�ж��ڴ��Ƿ�ռ��
	int ptrIndex;	//Ϊ�����ptrָ�����ڴ���е�����
	int memptrIndex; //Ϊ�����memblock���ڴ���е�������
	struct{
		char byte;
		wchar_t word;
		int dword;
		void * memblock;
		void * ptr;
		double qword;
	}val;
}MEM_STRUCT;  //ÿ���ڴ浥Ԫ�Ͷ�ջ��Ԫ���ڴ�ṹ�����Ͷ��塣

typedef struct{
	int size;
	int count;
	int refcount; //�ڴ������ü�������Ҫ��������֮��Ķ�̬������ڴ�顣
	int ptrIndex; //Ϊ�����mem_arrayָ�����ڴ���е�����
	MEM_STRUCT * mem_array;
}MEM_LIST;  //ȫ�������ڴ涯̬���飬ȫ�ֶ�ջ�ڴ涯̬��������Ͷ��塣
//run.c
extern "C" MEM_LIST * realloc_memblock(MEM_LIST * ptr,int index);
extern "C" MEM_STRUCT memop_array_addr(MEMOPTYPE op,MEM_LIST * ptr,int index,MEM_STRUCT * tmpmem);

class MyUserExtraData{
public:
	MyUserExtraData();
	~MyUserExtraData();
public:
	wxSQLite3Database db;
	wxSQLite3ResultSet sqlset;
	int isQuerySql;
	CURL * curl;
}; //���ݸ�������Ķ���������

extern void * CUR_CAIJI_VM;
extern wxSQLite3ResultSet gl_sqlset_forZenglRun;
extern int gl_isQuerySql_forZenglRun;

#endif //_ZL_CAIJI_H_