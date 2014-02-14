#include "wx/wx.h"
#include "wx/aui/aui.h"
#include "wx/treectrl.h"
#include <curl/curl.h>
#include <wx/sstream.h>
#include <stdio.h>
#include <string.h>
#include <wx/thread.h>
#include <wx/artprov.h>
#include <wx/regex.h>
#include <wx/xml/xml.h> 
#include <wx/dialog.h>
#include <wx/listbox.h>
#include "wx/image.h"
#include "wx/splash.h"
#include "wx/wxsqlite3.h"
#include "wx/filename.h"
#include "stdlib.h"
#include "stdio.h"
#include <wx/richtext/richtextctrl.h>
#include "logEvent.h"
#include "ProgressEvent.h"
#include "MyHashString.h"
#include "MyTaskBarIcon.h"
#include <wx/grid.h>
#include <wx/utils.h>
#include <wx/fdrepdlg.h>
#include <wx/wfstream.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include "caiji.h"

#define ZL_EXP_OS_IN_WINDOWS //���ڸ���zenglǶ��ʽ�ű����Ե�ǰ��ִ�л�����windowsϵͳ����ֹ���ֱ������
#include "zengl_exportfuns.h" //����zengl v1.3.2

DEFINE_EVENT_TYPE(wxEVT_MY_EVENT) //�ɵ���־�¼����Ѿ����������ڲ���logevent.h�ж����wxEVT_MY_LOG_EVENT
DEFINE_EVENT_TYPE(wxEVT_MY_UPDATECATE) //����ȡ����վ�ķ�����Ϣ�����ڸ���treectrl���οؼ���ķ����б���¼���
DEFINE_EVENT_TYPE(wxEVT_MY_CHECKS) //����������б��Ŀ¼����ิѡ��ʱ�����з�ѡ�����б�
DEFINE_EVENT_TYPE(wxEVT_MY_PROGRESS) //������վ��ȡ��������ʱ����ײ��Ľ������¼���
DEFINE_EVENT_TYPE(wxEVT_MY_CLEARLOGS)  //���ɼ���һ������ʱ�����ڻع�����richtext��־���������ݵ��¼���
DEFINE_EVENT_TYPE(wxEVT_MY_CAIJI_PROGRESS) //�ɵĲɼ�ʱ�Ľ������¼����Ѿ����������ڲ���progressevent.h�ﶨ���wxEVT_MY_PROGRESS_EVENT�¼���
DEFINE_EVENT_TYPE(wxEVT_MY_PAUSE_THREAD) //�ɼ����ݵ�����ʱ�����û����������ݵ��¼���
DEFINE_EVENT_TYPE(wxEVT_MY_DEBUG_PAUSE_THREAD) //������ͣ�¼�
DEFINE_EVENT_TYPE(wxEVT_MY_SET_CURL_NUM) //����ץ����������

CURL * curl = NULL; //ȫ�ֵ�libcurl����ץ��������Ҫ��ָ�롣
ZL_EXP_VOID * CUR_CAIJI_VM; //��ǰ���еĲɼ��ű�ʹ�õ�VM�����ָ��

FILE *fp = NULL ,*logfp=NULL,*zengl_debuglogfile = NULL;  //����FILE����ָ�룬fp��Ҫ��text.xml��logfp����־�ļ�mylogs.txt
char * char_myglStr = NULL; //������curl��ȡ������ʱ���ڻص������н�������д��char_myglStr,char_myglStr��һ����̬���ݵ��ڴ档����zengl������Ե��ڴ����취
int char_myglTotalnum = 0; //char_myglStr��Ӧ�����ݵ�size��С��ʹ��strlen��ȡ�������ݲ���׼ȷ�����׳���©�ַ��������
int char_myglTotalnum_forShow = 0;
int charlength = 0;   //char_myglStr�ַ����ȣ�����ʹ�õ���char_myglTotalnum�����charlength����strlen��ȡ���ģ����ʼ���������õģ������ο���
int totallogsLength = 0; //��־�������ݳ���һ����Χʱ�ͽ��лع����������Ŀǰ�и�BUG�������ֵ��һֱ��󣬵�����ʱû����ʹ���ϵ����⣬�ȷ�������������
wxString myglStr=_(""); //char_myglStr����������ջ�תΪwxString���ʹ����myglStr���档
int myglStyle = MY_RICHTEXT_NORMAL_STYLE; //myglStyle��־��ɫ��ʽ,normal��ʾ��ɫ��ͨ��ʽ�����к�ɫ���棬��ɫ��ʽ��
bool glIsStop = false;   //������֮ǰ����ʱ�����ж��߳��Ƿ�ֹͣ�õģ�Ŀǰû����ʹ�õĵط���Ӧ���Ǳ������ˡ�
MyFrame *glmainFrame = NULL;  //ȫ�������򴰿ڵľ����
wxSQLite3Database * glDB;  //ȫ�����ݿ�ָ�룬���ں������ɼ�ģ��������ݹ�ͨ�õġ�
wxString glSiteDBPath = "";  //ȫ����վ���ݿ��ļ���·����
wxString glCaijiWebName = ""; //ȫ����վ���ơ�
bool glIsDialogInShow = false;  //�����жϵ�ǰ�����Ƿ��е����ĶԻ�������е����ĶԻ�����ô���˳�ʱ�������棬��ֹ�����ڴ�����������
extern wxString gl_initManage_Url; //���ù���ҳ���URL ����Ϊ�ɼ��ϴ��ӿ��ļ���
wxString glEscapeSiteName; //ȫ�ֵĵ�ǰ��վdb�ļ���
wxString glCompanyCatID; //��Ҫ�ɼ��Ĺ�˾����ID
wxString gl_zenglrun_article_modname = "Module/article.zl"; //��Ѷģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_brand_modname = "Module/brand.zl"; //Ʒ��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_buy_modname = "Module/buy.zl"; //��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_exhibit_modname = "Module/exhibit.zl"; //չ��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_group_modname = "Module/group.zl"; //�Ź�ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_info_modname = "Module/info.zl"; //����ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_job_modname = "Module/job.zl"; //�˲�ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_know_modname = "Module/know.zl"; //֪��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_mall_modname = "Module/mall.zl"; //�̳�ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_quote_modname = "Module/quote.zl"; //����ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_sell_modname = "Module/sell.zl"; //��Ӧģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_special_modname = "Module/special.zl"; //ר��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_photo_modname = "Module/photo.zl"; //ͼ��ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_video_modname = "Module/video.zl"; //��Ƶģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_zenglrun_down_modname = "Module/down.zl"; //����ģ��Ĺ����ļ�·��,�ڳ�ʼ���ű����Ѿ��޸ġ�
wxString gl_version_number = "v1.3.0"; //�汾����Ϣ
//�ű�XOR��ͨ��������Կ
//wxString gl_encrypt_str = "xingkekn&**&$&^^^#(*@(*#&$&*@&$*0029044*&$*@&$*&$&!!~~!!@))__*#**)#(*#(*(@#BUBUWEBudru!<!`ss`x)&idmmn!vnsme&-2-00/54-b-#i`i`!doe#(:shou!udruZ2-0\\:cmuQshou@ss`x)udru(:cmuUdru@ees)'udru3-#udru3!hr!lnehgx!ho!cmuUdru@ees!i`i`#__*#**)#(*#(*(@#BU*@&$*0029044*&$*@^^^#(*@(*#&$&*@&$*00290@ees)'udru3-#uhr!lnehgx!&**&$&^^^#(*@(*";
//�����ǽű�RC4������Կ
wxString gl_encrypt_str = "zl!:)haha_hello_world!iamsuperman";

BEGIN_EVENT_TABLE(MyStatusBar,wxStatusBar)
	EVT_SIZE(MyStatusBar::OnSize)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_PROGRESS, MyStatusBar::OnUpdateProgress)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Quit, MyFrame::OnQuit)
    EVT_MENU(ID_About, MyFrame::OnAbout)
	EVT_MENU(ID_MENU_FILE_HELP, MyFrame::OnHelp)
	EVT_MENU(ID_MENU_RESTORE_PERSPECTIVE,MyFrame::OnRestorePespective)
	EVT_MENU(ID_MENU_CLEAR_LOGS,MyFrame::OnClearAll_Logs)
	EVT_MENU(ID_MENU_CATEGORY_FIND,MyFrame::OnFindCategory)
	EVT_MENU(ID_MENU_CATEGORY_SELECTALL,MyFrame::OnCategorySelectAll)
	EVT_MENU(ID_MENU_CATEGORY_UNSELECTALL,MyFrame::OnCategoryUnSelectAll)
	EVT_MENU(ID_MENU_CATEGORY_EXPANDALL,MyFrame::OnCategoryExpandAll)
	EVT_MENU(ID_MENU_CATEGORY_UNEXPANDALL,MyFrame::OnCategoryUnExpandAll)
	EVT_MENU(ID_MENU_EXTRA_REVERSE_ORDER,MyFrame::OnExtraCheck)
	EVT_MENU(ID_MENU_EXTRA_POST_PENDING,MyFrame::OnExtraCheck)
	EVT_MENU(ID_MENU_EXTRA_LOCAL_IMG,MyFrame::OnExtraCheck)
	EVT_MENU(ID_MENU_EXTRA_DEBUG,MyFrame::OnExtraCheck)
	EVT_ICONIZE(MyFrame::OnMinimize)
	//EVT_BUTTON(IDUrlName_Btn,MyFrame::OnUrlNameBtn)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(IDUrlName_Btn,MyFrame::OnUrlNameBtn)
	EVT_TEXT_ENTER(IDUrl_Name_TextCtrl,MyFrame::OnTextCtrlEnter)
	EVT_TEXT_ENTER(ID_TEXT_FOR_DEBUG,MyFrame::OnTextDebugEnter)
	EVT_MY_LOG_EVENT  ( wxID_ANY,MyFrame::OnMyEvent)
	EVT_MY_PROGRESS_EVENT  ( wxID_ANY,MyFrame::OnCaijiProgress)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_UPDATECATE, MyFrame::OnUpdateCate)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_CHECKS, MyFrame::OnChecks)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_CLEARLOGS, MyFrame::OnClearLogs)
	//EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_CAIJI_PROGRESS, MyFrame::OnCaijiProgress)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_PAUSE_THREAD, MyFrame::OnPauseThread)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_DEBUG_PAUSE_THREAD, MyFrame::OnDebugPauseThread)
	EVT_COMMAND  (ID_MY_WINDOW, wxEVT_MY_SET_CURL_NUM, MyFrame::OnSetCurlNum)
	EVT_CHECKBOX(ID_CHECK_CAIJI_COMPANY,OnClickCheckCaijiCompany)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_START,MyFrame::OnClickStart)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_PAUSE,MyFrame::OnClickPause)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_STOP,MyFrame::OnClickStop)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_TEST,MyFrame::OnClickTest)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM2_RESULT,MyFrame::OnClickResults)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_SHOW_LOG,MyFrame::OnClickShowLog)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_WEB_OFFICE,MyFrame::OnClickWebOffice)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_ITEM_RESET_INIT,MyFrame::OnResetInitScript)
	EVT_FIND(wxID_ANY, MyFrame::OnFindDialog)
    EVT_FIND_NEXT(wxID_ANY, MyFrame::OnFindDialog)
	EVT_FIND_CLOSE(wxID_ANY, MyFrame::OnFindDialog)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTreeCtrl, wxTreeCtrl)
	EVT_TREE_STATE_IMAGE_CLICK(IDCaiji_Tree_Ctrl, MyTreeCtrl::OnItemStateClick)
	EVT_TREE_ITEM_ACTIVATED(IDCaiji_Tree_Ctrl,MyTreeCtrl::OnItemActivated)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyKeyDialog, wxDialog)
	EVT_BUTTON(ID_KEY_EDIT,MyKeyDialog::OnEditKey)
	EVT_BUTTON(ID_KEY_NEW,MyKeyDialog::OnNewKey)
	EVT_LISTBOX_DCLICK(ID_KEY_LISTBOX,MyKeyDialog::OnEditKey)
	EVT_BUTTON(ID_KEY_OK,MyKeyDialog::OnOk)
	EVT_BUTTON(ID_KEY_DELETE,MyKeyDialog::OnDelete)
	EVT_BUTTON(ID_KEY_SHOWDATA,MyKeyDialog::OnShowData)
	EVT_BUTTON(ID_KEY_JUMPTO_CAIJI_URL,MyKeyDialog::OnJumpToCaijiUrl)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyKeyWordDialog, wxDialog)
	EVT_BUTTON(ID_KEYWORD_OK,MyKeyWordDialog::OnOk)
	EVT_BUTTON(ID_KEYWORD_CANCEL,MyKeyWordDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyNewKeyWordDialog, wxDialog)
	EVT_BUTTON(ID_NEWKEY_OK,MyNewKeyWordDialog::OnOk)
	EVT_BUTTON(ID_NEWKEY_CANCEL,MyNewKeyWordDialog::OnCancel)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTestDialog, wxDialog)
	EVT_BUTTON(ID_TEST_CLICK,MyTestDialog::OnTest)
	EVT_BUTTON(ID_TEST_CANCEL,MyTestDialog::OnCancel)
	EVT_BUTTON(ID_TEST_LOADFILE,MyTestDialog::OnLoadTestFile)
	//EVT_SIZE(MyTestDialog::OnSize)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyResultsDialog, wxDialog)
	EVT_LISTBOX(wxID_ANY,MyResultsDialog::OnListboxSelected)
	EVT_BUTTON(ID_RESULTS_BUTTON_VIEW,MyResultsDialog::OnViewContent)
	EVT_BUTTON(ID_RESULTS_BUTTON_CLOSE,MyResultsDialog::OnClose)
	EVT_BUTTON(ID_RESULTS_BUTTON_DELETE,MyResultsDialog::OnDelete)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyGridDialog, wxDialog)
	EVT_BUTTON(ID_SHOWDATA_PRE,MyGridDialog::OnClickPre)
	EVT_BUTTON(ID_SHOWDATA_NEXT,MyGridDialog::OnClickNext)
	EVT_BUTTON(ID_SHOWDATA_JUMP,MyGridDialog::OnClickJump)
	EVT_BUTTON(ID_SHOWDATA_DELETE,MyGridDialog::OnClickDelete)
	EVT_GRID_CELL_CHANGING(MyGridDialog::OnGridChanging)
	EVT_GRID_CELL_CHANGED(MyGridDialog::OnGridChanged)
	//EVT_RADIOBOX  (ID_RADIOBOX, MyGridDialog::OnClickClearData)
	EVT_BUTTON(ID_SHOWDATA_CLEARBTN,MyGridDialog::OnClickClearData)
END_EVENT_TABLE()

BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_RESTORE, MyTaskBarIcon::OnMenuRestore)
	EVT_MENU(PU_HIDE, MyTaskBarIcon::OnHideToTray)
    EVT_MENU(PU_EXIT,    MyTaskBarIcon::OnMenuExit)
    EVT_TASKBAR_LEFT_UP  (MyTaskBarIcon::OnLeftButtonClick)
END_EVENT_TABLE()

IMPLEMENT_APP(MyApp)

MyStatusBar::MyStatusBar(int fieldnum,wxWindow *parent,wxWindowID id,long style, const wxString& name)
:wxStatusBar(parent,id),
m_total(0),
m_pos(0)
{
	if(fieldnum == 0)
		SetFieldsCount(Field_Percent + 1);
	else
		SetFieldsCount(fieldnum);
	m_ProgressBar = new wxGauge(this,wxID_ANY,100);
}

void MyStatusBar::OnSize(wxSizeEvent &event)
{
	wxRect rect;
    GetFieldRect(Field_Process, rect);
    m_ProgressBar->SetSize(rect.x, rect.y+3, rect.width, 12);
}

void MyStatusBar::OnUpdateProgress(wxCommandEvent &event)
{
	if(m_total <= 0)
		return;
	if(m_ProgressBar->GetRange() != m_total)
		m_ProgressBar->SetRange(m_total);
	m_ProgressBar->SetValue(m_pos);
	wxString percentStr = "";
	int percent = ((float)m_pos/(float)m_total) * 100;
	percentStr.Printf("�ٷֱȣ�%d%%",percent);
	SetStatusText(percentStr,Field_Percent);
}

MyKeyDialog::MyKeyDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid,wxString modid,wxString catname)
			: wxDialog(parent, wxID_ANY, title,
                               point, size,
                               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) ,
			m_catid(catid),
			m_modid(modid),
			m_catname(catname)
{
}

bool MyKeyDialog::Create()
{
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxString sqlstr="";
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	wxSizer * const
		sizerMsgs = new wxStaticBoxSizer(wxHORIZONTAL, this, "�ؼ����б�");
	m_listbox = new wxListBox(this,ID_KEY_LISTBOX,wxDefaultPosition, wxSize(130,200));
	wxFont myfont( 11, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL , wxFONTWEIGHT_NORMAL , false, wxT("����"));
	m_listbox->SetFont(myfont);
	RefreshListBox(0);
	sizerMsgs->Add(m_listbox,3,wxALL,10);
	wxSizer * const sizerRight = new wxBoxSizer(wxVERTICAL);
	m_new = new wxButton(this,ID_KEY_NEW,"�½�");
	m_edit = new wxButton(this,ID_KEY_EDIT,"�༭");
	m_delete = new wxButton(this,ID_KEY_DELETE,"ɾ��");
	m_showdata = new wxButton(this,ID_KEY_SHOWDATA,"���زɼ�������");
	m_jumpToCaijiUrl = new wxButton(this,ID_KEY_JUMPTO_CAIJI_URL,"ѡ�йؼ��ʵĲɼ���վ");
	sizerRight->Add(m_new,0,wxALL,10);
	sizerRight->Add(m_edit,0,wxALL,10);
	sizerRight->Add(m_delete,0,wxALL,10);
	sizerRight->Add(m_showdata,0,wxALL,10);
	sizerRight->Add(m_jumpToCaijiUrl,0,wxALL,10);
	sizerMsgs->Add(sizerRight,3,wxALL,10);
	sizerTop->Add(sizerMsgs,3,wxALL,10);
	m_ok = new wxButton(this,ID_KEY_OK,"�رձ�����");
	//m_cancel = new wxButton(this,ID_KEY_CANCEL,"ȡ��");
	wxSizer * const sizerBottom = new wxBoxSizer(wxHORIZONTAL);
	sizerBottom->Add(m_ok,0,wxALL,10);
	//sizerBottom->Add(m_cancel,0,wxALL,10);
	sizerTop->Add(sizerBottom,0,wxALL | wxALIGN_CENTER,10);
	SetSizerAndFit(sizerTop);
	return true;
}

bool MyKeyDialog::RefreshListBox(int index)
{
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxString sqlstr="";
	sqlstr.Printf("SELECT * FROM keywords WHERE catid='%s'",m_catid);
	wxSQLite3ResultSet set = db->ExecuteQuery(sqlstr);
	wxArrayString keywords_array;
	while(set.NextRow())
	{
		keywords_array.Add(set.GetAsString(1));
	}
	m_listbox->Clear();
	if(keywords_array.GetCount() > 0)
	{
		m_listbox->Append(keywords_array);
		int totalcount = m_listbox->GetCount();
		if(totalcount <= 0)
			return true;
		else if(index >= totalcount)
			index = totalcount - 1;
		m_listbox->SetSelection(index);
	}
	return true;
}

void MyKeyDialog::OnEditKey(wxCommandEvent& event)
{
	int selectItem = m_listbox->GetSelection();
	if(selectItem == wxNOT_FOUND)
	{
		wxMessageBox("û��ѡ��ؼ���","����");
		return;
	}
	wxString selectedKey = m_listbox->GetString(selectItem);
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxSQLite3ResultSet set = db->ExecuteQuery("SELECT * FROM keywords WHERE catid='" + m_catid + "' AND name='"+selectedKey+"'");
	if(set.NextRow())
	{
		wxString must = set.GetAsString(3);
		wxString needSuffix = set.GetAsString(4);
		wxString keywordID = set.GetAsString(0);
		wxPoint mainWinLoc=this->GetScreenPosition();
		mainWinLoc.x += 20;
		mainWinLoc.y += 20;
		wxSize mainWinSize=this->GetSize();
		glIsDialogInShow = true;
		MyKeyWordDialog mykeyword_dlg(this,selectedKey,mainWinLoc,mainWinSize,keywordID);
		mykeyword_dlg.Create(selectedKey,must,needSuffix);
		mykeyword_dlg.ShowModal();
		glIsDialogInShow = false;
		this->RefreshListBox(selectItem);
	}
	else
		wxMessageBox("�����ݿ���û���ҵ���Ӧ�Ĺؼ���","����");
}

void MyKeyDialog::OnNewKey(wxCommandEvent& event)
{
	wxPoint mainWinLoc=this->GetScreenPosition();
	mainWinLoc.x += 20;
	mainWinLoc.y += 20;
	wxSize mainWinSize=this->GetSize();
	glIsDialogInShow = true;
	MyNewKeyWordDialog newkeydlg(this,"��ӹؼ���",mainWinLoc,mainWinSize,m_catid);
	newkeydlg.Create();
	newkeydlg.ShowModal();
	glIsDialogInShow = false;
	this->RefreshListBox(0);
}

void MyKeyDialog::OnOk(wxCommandEvent& event)
{
	this->Close();
}

void MyKeyDialog::OnDelete(wxCommandEvent& event)
{
	int selectItem = m_listbox->GetSelection();
	if(selectItem == wxNOT_FOUND)
	{
		wxMessageBox("û��ѡ��ؼ���","����");
		return;
	}
	wxString oldTitle = this->GetTitle();
	this->SetTitle("ɾ�����������С�����");
	wxString selectedKey = m_listbox->GetString(selectItem);
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxSQLite3ResultSet set = db->ExecuteQuery("SELECT * FROM keywords WHERE catid='" + m_catid + "' AND name='"+selectedKey+"'");
	if(set.NextRow())
	{
		db->ExecuteUpdate("DELETE FROM keywords WHERE catid='" + m_catid + "' AND name='"+selectedKey+"'");
		this->RefreshListBox(selectItem);
	}
	else
		wxMessageBox("�ùؼ��������ݿ��в����ڣ�","�����쳣��");
	this->SetTitle(oldTitle);
}

void MyKeyDialog::OnShowData(wxCommandEvent& event)
{
	glIsDialogInShow = true;
	MyGridDialog mygriddata(this,"["+m_catname+"]���زɼ���������",wxDefaultPosition,wxSize(950,650),m_catid,m_modid,m_catname);
	mygriddata.Create();
	mygriddata.ShowModal();
	glIsDialogInShow = false;
}

extern "C" {
	int zenglrun_main(int argc,char * argv[]);

	ZL_EXP_INT global_userdef_compile_info_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount);
	ZL_EXP_INT global_userdef_compile_error_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount);
	ZL_EXP_INT global_userdef_run_info_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount);
	ZL_EXP_INT global_userdef_run_print_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount);
	ZL_EXP_INT global_userdef_run_error_forZenglRunV2(ZL_EXP_CHAR * infoStrPtr, ZL_EXP_INT infoStrCount);
	ZL_EXP_VOID global_builtin_printf(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount);
	ZL_EXP_VOID global_builtin_module_init(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT moduleID);
	ZL_EXP_VOID global_module_init(ZL_EXP_VOID * VM_ARG);
	void global_JumpToCaiji_InitFuncall(void * VM_ARG);
	ZL_EXP_VOID global_bltSetInitManageUrl(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount);
	ZL_EXP_VOID global_bltSetModulePath(ZL_EXP_VOID * VM_ARG,ZL_EXP_INT argcount);
	ZL_EXP_INT global_debug_break(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * cur_filename,ZL_EXP_INT cur_line,ZL_EXP_INT breakIndex,ZL_EXP_CHAR * log);
	ZL_EXP_INT global_debug_conditionError(ZL_EXP_VOID * VM_ARG,ZL_EXP_CHAR * filename,ZL_EXP_INT line,ZL_EXP_INT breakIndex,ZL_EXP_CHAR * error);
}

void MyKeyDialog::OnJumpToCaijiUrl(wxCommandEvent& event)
{
	int selectItem = m_listbox->GetSelection();
	if(selectItem == wxNOT_FOUND)
	{
		wxMessageBox("û��ѡ��ؼ���","����");
		return;
	}

	ZL_EXP_VOID * VM = zenglApi_Open();
	//zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE | ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO));
	zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE));
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_COMPILE_INFO,global_userdef_compile_info_forZenglRunV2);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_INFO,global_userdef_run_info_forZenglRunV2);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_PRINT,global_userdef_run_print_forZenglRunV2);	
	global_JumpToCaiji_InitFuncall(VM);
	zenglApi_SetExtraData(VM,"catid",(void *)m_catid.c_str().AsChar());
	zenglApi_SetExtraData(VM,"modid",(void *)m_modid.c_str().AsChar());

	wxString selectedKey = m_listbox->GetString(selectItem);
	zenglApi_SetExtraData(VM,"selectedKey",(void *)selectedKey.c_str().AsChar());
	zenglApi_SetExtraData(VM,"moduleName",(void *)global_MyModules[m_modid].c_str().AsChar());
	zenglApi_SetExtraData(VM,"buyScriptName",(void *)gl_zenglrun_buy_modname.c_str().AsChar()); //buy��ģ���Ӧ�Ĳɼ��ű��ļ���

	MyUserExtraData extraData;
	zenglApi_SetExtraData(VM,"extraData",(void *)(&extraData));
	wxString jumpFile = "Module/�ɼ���ת����.zl";
	if(!wxFileExists(jumpFile)) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
	{
		jumpFile = "Module/�ɼ���ת����.zlencrypt";
		//zenglApi_SetSourceXorKey(VM,(char *)gl_encrypt_str.c_str().AsChar()); //�����ܽű�
		zenglApi_SetSourceRC4Key(VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //RC4���ܽű�
	}
	if(zenglApi_Run(VM,(char *)jumpFile.c_str().AsChar()) == -1) //����ִ��zengl�ű�
	{
		wxMessageBox(wxString::Format("\n��������'Module/�ɼ���ת����.zl'ʱ�����쳣��%s\n",zenglApi_GetErrorString(VM)),"����");
		zenglApi_Close(VM);
		return ;
	}
	zenglApi_Close(VM);
}

MyKeyWordDialog::MyKeyWordDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString id)
:wxDialog(parent, wxID_ANY, title,
                  point, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) ,
 m_id(id)
{
}

bool MyKeyWordDialog::Create(wxString keyword,wxString mustText,wxString needSuffix)
{
	wxFont myfont( 12, 74, 90, 90, false, wxT("����"));
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	wxSizer * const sizerH = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * text1 = new wxStaticText(this,wxID_ANY,"�ؼ�������:",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH->Add(text1,0,wxALL,10);
	m_keyword = new wxTextCtrl(this,wxID_ANY,keyword,wxDefaultPosition,wxSize(150,18));
	sizerH->Add(m_keyword,0,wxALL,10);
	wxSizer * const sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"�����б�������Ĵ�(��ѡ):",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH1->Add(text1,0,wxALL,10);
	m_mustText = new wxTextCtrl(this,wxID_ANY,mustText,wxDefaultPosition,wxSize(150,18));
	sizerH1->Add(m_mustText,0,wxALL,10);
	wxSizer * const sizerH2 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"�Ƿ���Ҫ��׺:",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH2->Add(text1,0,wxALL,10);
	wxString choices[] = { _("��Ҫ"),_("����Ҫ")};
	m_needSuffix = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxSize(80,18),2,choices);
	if(needSuffix == "1")
		m_needSuffix->SetSelection(0);
	else
		m_needSuffix->SetSelection(1);
	sizerH2->Add(m_needSuffix,0,wxALL,10);
	wxSizer * const sizerH3 = new wxBoxSizer(wxHORIZONTAL);
	m_ok = new wxButton(this,ID_KEYWORD_OK,"ȷ��",wxDefaultPosition,wxSize(60,25));
	m_cancel = new wxButton(this,ID_KEYWORD_CANCEL,"ȡ��",wxDefaultPosition,wxSize(60,25));
	sizerH3->Add(m_ok,0,wxALL,10);
	sizerH3->Add(m_cancel,0,wxALL,10);
	sizerTop->Add(sizerH,0,wxALL,10);
	sizerTop->Add(sizerH1,0,wxALL,10);
	sizerTop->Add(sizerH2,0,wxALL,10);
	sizerTop->Add(sizerH3,0,wxALL | wxALIGN_CENTER,10);
	SetSizerAndFit(sizerTop);
	return true;
}

void MyKeyWordDialog::OnOk(wxCommandEvent &event)
{
	wxString oldTitle = this->GetTitle();
	this->SetTitle("�����С�����");
	wxString keyword = m_keyword->GetValue();
	wxString mustStr = m_mustText->GetValue();
	wxString needSuffix = m_needSuffix->GetValue();
	if(keyword == "")
	{
		wxMessageBox("�ؼ��ʲ���Ϊ�գ�","����");
		this->SetTitle(oldTitle);
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	needSuffix = needSuffix == "��Ҫ" ? "1" : "0";

	db->ExecuteUpdate("UPDATE keywords SET name='" + keyword + "' , must='" + mustStr + "' , needSuffix='" + needSuffix + "' " + 
							" WHERE id='"+ m_id +"'");
	this->Close();
}

void MyKeyWordDialog::OnCancel(wxCommandEvent &event)
{
	this->Close();
}

MyNewKeyWordDialog::MyNewKeyWordDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid)
:wxDialog(parent, wxID_ANY, title,
                 point, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
m_catid(catid)
{
}

bool MyNewKeyWordDialog::Create()
{
	wxFont myfont( 12, 74, 90, 90, false, wxT("����"));
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	wxSizer * const sizerH = new wxBoxSizer(wxHORIZONTAL);
	wxStaticText * text1 = new wxStaticText(this,wxID_ANY,"�ؼ�������(������):",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH->Add(text1,0,wxALL,10);
	m_keyword = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(150,18));
	sizerH->Add(m_keyword,0,wxALL,10);
	wxSizer * const sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"�����б�������Ĵ�(��ѡ):",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH1->Add(text1,0,wxALL,10);
	m_mustText = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(150,18));
	sizerH1->Add(m_mustText,0,wxALL,10);
	wxSizer * const sizerH2 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"�Ƿ���Ҫ��׺:",wxDefaultPosition,wxSize(210,18),wxALIGN_RIGHT);
	text1->SetFont(myfont);
	sizerH2->Add(text1,0,wxALL,10);
	wxString choices[] = { _("��Ҫ"),_("����Ҫ")};
	m_needSuffix = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxSize(80,18),2,choices);
	m_needSuffix->SetSelection(0);
	sizerH2->Add(m_needSuffix,0,wxALL,10);
	wxSizer * const sizerH3 = new wxBoxSizer(wxHORIZONTAL);
	m_ok = new wxButton(this,ID_NEWKEY_OK,"ȷ��",wxDefaultPosition,wxSize(60,25));
	m_cancel = new wxButton(this,ID_NEWKEY_CANCEL,"ȡ��",wxDefaultPosition,wxSize(60,25));
	sizerH3->Add(m_ok,0,wxALL,10);
	sizerH3->Add(m_cancel,0,wxALL,10);
	sizerTop->Add(sizerH,0,wxALL,10);
	sizerTop->Add(sizerH1,0,wxALL,10);
	sizerTop->Add(sizerH2,0,wxALL,10);
	sizerTop->Add(sizerH3,0,wxALL | wxALIGN_CENTER,10);
	SetSizerAndFit(sizerTop);
	return true;
}

void MyNewKeyWordDialog::OnOk(wxCommandEvent &event)
{
	wxString keyword = m_keyword->GetValue();
	if(keyword == "")
	{
		wxMessageBox("�ؼ��ʲ�����Ϊ�գ�","����");
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = "db/" + glmainFrame->textUrlName->GetValue() + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxSQLite3ResultSet set = db->ExecuteQuery("SELECT * FROM keywords WHERE catid='" + m_catid + "' AND name='"+keyword+"'");
	if(set.NextRow())
	{
		wxMessageBox("�÷����Ѿ�����ͬ���ؼ��ʣ�","����");
		return;
	}
	this->SetTitle("��Ӵ����С�����");
	wxString mustStr = m_mustText->GetValue();
	wxString needSuffix = m_needSuffix->GetValue();
	needSuffix = needSuffix == "��Ҫ" ? "1" : "0";
	wxString sqlstr="";
	sqlstr.Printf("INSERT INTO keywords (name,catid,must,needSuffix) VALUES ('%s','%s','%s','%s')",
				keyword,m_catid,mustStr,needSuffix);
	db->ExecuteUpdate(sqlstr);
	this->Close();
}

void MyNewKeyWordDialog::OnCancel(wxCommandEvent &event)
{
	this->Close();
}

MyTestDialog::MyTestDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size)
:wxDialog(parent, wxID_ANY, title,
                 point, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
}

bool MyTestDialog::Create()
{
	int myborderSize = 2;
	wxFont myfont( 10, 74, 90, 90, false, wxT("����"));
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	wxSizer * const sizerH = new wxBoxSizer(wxVERTICAL);
	wxStaticText * text1 = new wxStaticText(this,wxID_ANY,"������ʽ:",wxDefaultPosition,wxSize(150,18),wxALIGN_LEFT);
	text1->SetFont(myfont);
	sizerH->Add(text1,0,wxALL,myborderSize);
	m_express = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(550,40),wxTE_MULTILINE | wxEXPAND | wxNO_BORDER);
	sizerH->Add(m_express,1,wxEXPAND | wxALL,myborderSize);
	wxSizer * const sizerH_1 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"С��������:",wxDefaultPosition,wxSize(150,18),wxALIGN_LEFT);
	text1->SetFont(myfont);
	sizerH_1->Add(text1,0,wxALL,myborderSize);
	wxString choices[] = { _("��ƥ�任�з�"),_("ƥ�����з���")};
	m_dot = new wxComboBox(this,wxID_ANY,"",wxDefaultPosition,wxSize(150,18),2,choices,wxALIGN_LEFT);
	m_dot->SetSelection(0);
	sizerH_1->Add(m_dot,0,wxALL,myborderSize);
	wxSizer * const sizerH1 = new wxBoxSizer(wxHORIZONTAL);
	text1 = new wxStaticText(this,wxID_ANY,"��ַ:",wxDefaultPosition,wxSize(150,18),wxALIGN_LEFT);
	text1->SetFont(myfont);
	sizerH1->Add(text1,0,wxALL,myborderSize);
	m_url = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(400,18));
	sizerH1->Add(m_url,0, wxALL,myborderSize);
	wxSizer * const sizerH2 = new wxBoxSizer(wxVERTICAL);
	text1 = new wxStaticText(this,wxID_ANY,"��־�����Ϣ:",wxDefaultPosition,wxSize(210,18),wxALIGN_LEFT);
	text1->SetFont(myfont);
	sizerH2->Add(text1,0,wxALL,myborderSize);
	m_log = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(450,300),wxTE_MULTILINE | wxEXPAND | wxNO_BORDER);
	sizerH2->Add(m_log,1,wxEXPAND | wxALL,myborderSize);
	wxSizer * const sizerH3 = new wxBoxSizer(wxHORIZONTAL);
	sizerH3->Add(new wxButton(this,ID_TEST_CLICK,"����",wxDefaultPosition,wxSize(60,25)),0,wxALL,myborderSize);
	sizerH3->Add(new wxButton(this,ID_TEST_CANCEL,"�رմ���",wxDefaultPosition,wxSize(60,25)),0,wxALL,myborderSize);
	sizerH3->Add(new wxButton(this,ID_TEST_LOADFILE,"����������ʽ����.txt�ļ�",wxDefaultPosition,wxSize(250,25)),0,wxALL,myborderSize);
	sizerTop->Add(sizerH, 0 , wxALL,myborderSize);
	sizerTop->Add(sizerH_1, 0 ,wxALL,myborderSize);
	sizerTop->Add(sizerH1, 0 , wxALL,myborderSize);
	sizerTop->Add(sizerH2, 1 ,wxEXPAND | wxALL,myborderSize);
	sizerTop->Add(sizerH3, 0 ,wxALL,myborderSize);
	SetSizerAndFit(sizerTop);
	return true;
}

void MyTestDialog::OnTest(wxCommandEvent& event)
{
	if(m_express->GetValue() == "")
	{
		wxMessageBox("������ʽ����Ϊ�գ�","����");
		return;
	}
	wxString urlText = m_url->GetValue();
	if(urlText!="")
	{
		curl=curl_easy_init();
		curl_easy_setopt(curl, CURLOPT_URL, m_url->GetValue().c_str().AsChar());
		curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60);//���ó�ʱʱ��
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_string_write);
		curl_easy_perform(curl);
		if(char_myglStr==NULL)
		{
			m_log->AppendText(" null char content get! ԭ���������վ����ʧ�ܻ�ʱ��\n");
			return;
		}
		charlength = strlen(char_myglStr);
		if((myglStr = wxString(char_myglStr)) == "")
			myglStr = wxString::FromUTF8(char_myglStr);
		curl_easy_cleanup(curl);
		curl = NULL;
		free(char_myglStr);
		char_myglStr = NULL;
		char_myglTotalnum = 0;
	}
	else
	{
		myglStr = m_log->GetValue();
		if(myglStr == "")
		{
			wxMessageBox("û��Ҫ���Ե�����Դ��","����");
			return;
		}
	}
	m_log->Clear();
	wxString result;
	wxRegEx ex;
	wxArrayString tmp_links;
	wxArrayString content_links;
	size_t start = 0;
	size_t len = 0;
	size_t prevstart = 0;
	result = "";
	int pattern_suffix = wxRE_ADVANCED | wxRE_ICASE;
	if(m_dot->GetSelection() == 0)
		pattern_suffix |= wxRE_NEWLINE;
	if(ex.Compile(m_express->GetValue(),pattern_suffix))
	{
		while(ex.Matches(myglStr.Mid(prevstart)))
		{
			int matchCount = ex.GetMatchCount();
			for(int i=0;i<matchCount;i++)
			{
				if(ex.GetMatch(&start,&len,i))
				{
					result += "��������<"+wxString::Format("%d>(start:%d,end:%d)",i,start,len)+":\n";
					result += myglStr.Mid(prevstart + start, len) + "\n";
				}
			}
			result += "************************\n";
			prevstart += start + len;
		}
	}
	wxString express = m_express->GetValue();
	express.Replace("\\","\\\\");
	//express.Replace("\"","\\\"");
	express.Replace("'","\\'");
	m_log->AppendText("ԭʼ�ɼ�����Ϊ��\n"+ myglStr + "\n");
	m_log->AppendText("\n\n//////////////////////////////////////////////\n���򲶻���Ϊ��\n"+result);
	m_log->AppendText("\n\n//////////////////////////////////////////////\n������ʽ��Ӧ��C�����ַ���Ϊ��\n"+express+"\n");
}

void MyTestDialog::OnCancel(wxCommandEvent& event)
{
	this->Close();
}

void MyTestDialog::OnLoadTestFile(wxCommandEvent& event)
{
	if(!wxFileExists("������ʽ����.txt"))
	{
		wxMessageBox("������ʽ����.txt�ļ������ڣ�","����");
		return;
	}
	FILE *file;  
	file=fopen("������ʽ����.txt","rb");
	fseek(file,SEEK_SET,SEEK_END);
	long int fileLength=ftell(file);
	char * buf = (char *)malloc(fileLength + 1);
	fseek(file,0,SEEK_SET);
	int beginpos = 0;
	while(fgets(buf + beginpos ,FILE_BUFLEN,file))
	{
		beginpos = strlen(buf);
	}
	buf[fileLength] = NULL;
	fclose(file);
	wxString textContent;
	if((textContent = wxString(buf)) == "")
		textContent = wxString::FromUTF8(buf);
	m_log->Clear();
	m_log->AppendText(textContent);
	free(buf);
	buf = NULL;
}

MyResultsDialog::MyResultsDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size)
:wxDialog(parent, wxID_ANY, title,
                 point, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
}

bool MyResultsDialog::Create()
{
	int myborderSize = 10;
	wxFont myfont( 10, 74, 90, 90, false, wxT("����"));
	wxSizer * const sizerTop = new wxBoxSizer(wxHORIZONTAL);
	wxSizer * const sizerH = new wxStaticBoxSizer(wxVERTICAL,this,"���زɼ������б�");
	m_listbox = new wxListBox(this,ID_KEY_LISTBOX,wxDefaultPosition, wxSize(300,350));
	m_listbox->SetFont(myfont);
	sizerH->Add(m_listbox,0, wxALL,myborderSize);
	wxSizer * const sizerH1 = new wxBoxSizer(wxVERTICAL);
	m_static_title = new wxStaticText(this,wxID_ANY,"[��������]:",wxDefaultPosition,wxSize(120,20));
	m_title_text = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(120,110),wxTE_MULTILINE | wxNO_BORDER);
	m_view = new wxButton(this,ID_RESULTS_BUTTON_VIEW,"�鿴����",wxDefaultPosition,wxSize(120,23));
	m_delete = new wxButton(this,ID_RESULTS_BUTTON_DELETE,"����ɾ��",wxDefaultPosition,wxSize(120,23));
	m_close = new wxButton(this,ID_RESULTS_BUTTON_CLOSE,"�رձ�����",wxDefaultPosition,wxSize(120,23));
	m_catname = new wxStaticText(this,wxID_ANY,"[��ǰ����]:\n"+single_mod_catname,wxDefaultPosition,wxSize(120,100));
	sizerH1->Add(m_static_title,0, wxALL | wxCenter,myborderSize);
	sizerH1->Add(m_title_text,0,wxALL | wxCenter,myborderSize);
	sizerH1->Add(m_view,0, wxALL | wxCenter,myborderSize);
	sizerH1->Add(m_delete,0, wxALL | wxCenter,myborderSize);
	sizerH1->Add(m_close,0, wxALL | wxCenter,myborderSize);
	sizerH1->Add(m_catname,0, wxALL | wxCenter,myborderSize);
	m_logs = new wxTextCtrl(this,wxID_ANY,"",wxDefaultPosition,wxSize(400,350),wxTE_MULTILINE | wxEXPAND | wxNO_BORDER);
	wxSizer * const sizerH2 = new wxStaticBoxSizer(wxVERTICAL,this,"�ɼ���Ϣ���飺");
	sizerH2->Add(m_logs,0, wxEXPAND | wxALL,myborderSize);
	sizerTop->Add(sizerH, 0 , wxALL,myborderSize);
	sizerTop->Add(sizerH1, 0 , wxALL,myborderSize);
	sizerTop->Add(sizerH2, 1 ,wxEXPAND | wxALL,myborderSize);
	SetSizerAndFit(sizerTop);
	RefreshListBox(0);
	return true;	
}

bool MyResultsDialog::RefreshListBox(int index)
{
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return false;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxString sqlstr="";
	sqlstr.Printf("SELECT * FROM '" + single_mod_cat_table + "' WHERE state='0' AND catid='" + single_mod_catid +"'");
	wxSQLite3ResultSet set = db->ExecuteQuery(sqlstr);
	wxArrayString keywords_array;
	while(set.NextRow())
	{
		keywords_array.Add(set.GetAsString(2));
	}
	m_listbox->Clear();
	if(keywords_array.GetCount() > 0)
	{
		m_listbox->Append(keywords_array);
		int totalcount = m_listbox->GetCount();
		if(totalcount <= 0)
			return true;
		else if(index >= totalcount)
			index = totalcount - 1;
		m_listbox->SetSelection(index);
	}
	if(m_listbox->GetSelection() == wxNOT_FOUND)
		m_title_text->SetValue("");
	else
		m_title_text->SetValue(m_listbox->GetString(m_listbox->GetSelection()));
	return true;
}

void MyResultsDialog::OnListboxSelected(wxCommandEvent& event)
{
	m_title_text->SetValue(m_listbox->GetString(m_listbox->GetSelection()));
	return;
}

void MyResultsDialog::OnViewContent(wxCommandEvent& event)
{
	if(m_listbox->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox("��û��ѡ���κα��⣬����ԭ���б�Ϊ�գ�","����");
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxString escape_title = m_listbox->GetString(m_listbox->GetSelection()).mb_str(wxCSConv(wxT("GB2312")));
	escape_title.Replace("'","''");
	wxSQLite3ResultSet set = db->ExecuteQuery(wxString::Format("SELECT * FROM '%s' WHERE title='%s' AND catid='%s'",
									single_mod_cat_table,escape_title ,single_mod_catid));
	if(set.NextRow())
	{
		int setcount = set.GetColumnCount();
		wxString textcontent = "";
		for(int i=0;i<setcount;i++)
		{
			if(!set.IsNull(i))
				textcontent += "\n******************* "+ set.GetColumnName(i) +" *******************\n" + set.GetAsString(i) + "\n";
		}
		m_logs->SetValue(textcontent);
	}
}

void MyResultsDialog::OnClose(wxCommandEvent& event)
{
	this->Close(true);
}

void MyResultsDialog::OnDelete(wxCommandEvent& event)
{
	if(m_listbox->GetSelection() == wxNOT_FOUND)
	{
		wxMessageBox("��û��ѡ���κα��⣬����ԭ���б�Ϊ�գ�","����");
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	wxString escape_title = m_listbox->GetString(m_listbox->GetSelection()).mb_str(wxCSConv(wxT("GB2312")));
	escape_title.Replace("'","''");
	db->ExecuteUpdate(wxString::Format("UPDATE '%s' SET state='1' WHERE title='%s' AND catid='%s'",
									single_mod_cat_table,escape_title ,single_mod_catid));
	RefreshListBox(m_listbox->GetSelection());
}

MyGridDialog::MyGridDialog(wxWindow *parent,wxString title,wxPoint point,wxSize size,wxString catid,wxString modid,wxString catname)
:wxDialog(parent, wxID_ANY, title,
                 point, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER),
m_catid(catid),
m_modid(modid),
m_catname(catname)
{
}

bool MyGridDialog::Create()
{
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	wxSQLite3ResultSet set;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return false;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	if(!db->TableExists(global_MyModules[m_modid]))
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ�����ڣ�ԭ��������Ϊ����û�вɼ���["+m_catname+"]���࣡","����");
		this->Close(true);
		return false;
	}
	set = db->ExecuteQuery(wxString::Format("SELECT count(*) as totalcount FROM '%s' WHERE catid='%s'",global_MyModules[m_modid],m_catid));
	wxFont myfont( 13, 74, 90, 90, false, wxT("����"));
	int pagenum = 1;
	long totalcount;
	int itotalcount;
	if(set.NextRow())
	{
		set.GetAsString("totalcount").ToLong(&totalcount);
		itotalcount = (int)totalcount;
		pagenum = itotalcount/MYGRID_DATA_SHOWNUM;
		if(pagenum <= 0)
		{
			pagenum = 1;
		}
		else if(itotalcount % MYGRID_DATA_SHOWNUM != 0)
		{
			pagenum++;
		}
	}
	else
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ�����ѯ����ʱʧ�ܣ�","����");
		this->Close(true);
		return false;
	}
	set = db->ExecuteQuery(wxString::Format("SELECT * FROM '%s' WHERE catid='%s' limit 0,%d",global_MyModules[m_modid],m_catid,MYGRID_DATA_SHOWNUM));
	int colcount = set.GetColumnCount();
	wxSizer * const sizerTop = new wxBoxSizer(wxVERTICAL);
	m_grid = new wxGrid(this,wxID_ANY,wxDefaultPosition,wxSize(950,500));
	m_grid->CreateGrid( 0, 0 );
	m_grid->AppendRows(100);
	m_grid->AppendCols(colcount);
	int i;
	for(i=0 ; i < colcount; i++)
	{
		m_grid->SetColLabelValue(i,set.GetColumnName(i));
	}
	int j=0;
	while(set.NextRow())
	{
		for(i=0;i < colcount;i++)
		{
			wxString tmpColumnName = set.GetColumnName(i);
			if(tmpColumnName == "content" || tmpColumnName == "company_introduce") //����̫�࣬����Ҫ�ü�����ֹ���ֿ������
				m_grid->SetCellValue(j,i,set.GetAsString(i).Mid(0,45) + "....");
			else
				m_grid->SetCellValue(j,i,set.GetAsString(i));
		}
		j++;
	}
	m_grid->AutoSizeColumn(2,false);
	m_grid->AutoSizeColumn(3,false);
	m_grid->AutoSizeColumn(5,false);
	m_grid->AutoSizeColumn(colcount - 1,false);
	sizerTop->Add(m_grid,0,wxALL,5);
	wxSizer * const hsizer = new wxBoxSizer(wxHORIZONTAL);
	hsizer->Add(new wxButton(this,ID_SHOWDATA_PRE,"��һҳ"),0,wxALL,5);
	m_page = new wxTextCtrl(this,wxID_ANY,"1",wxDefaultPosition,wxSize(30,20));
	hsizer->Add(m_page,0,wxALL,5);
	m_pagenum = new wxStaticText(this,wxID_ANY,wxString::Format("�ܹ�%dҳ%d����¼",pagenum,itotalcount));
	m_pagenum->SetFont(myfont);
	hsizer->Add(m_pagenum,0,wxALL,5);
	hsizer->Add(new wxButton(this,ID_SHOWDATA_NEXT,"��һҳ"),0,wxALL,5);
	hsizer->Add(new wxButton(this,ID_SHOWDATA_JUMP,"��ת"),0,wxALL,5);
	hsizer->Add(new wxButton(this,ID_SHOWDATA_DELETE,"ɾ��"),0,wxALL,5);
	sizerTop->Add(hsizer,0,wxALL,5);
	wxSizer * const hsizer2 = new wxBoxSizer(wxHORIZONTAL);
	wxString choices[] =
    {
		"���["+m_catname+"]��������б��زɼ�����", "���["+glMyModuleNames[m_modid]+"]ģ������б��زɼ�����",
		"�������վ�����б��زɼ�����"
    };
	m_radio = new wxRadioBox(this, ID_RADIOBOX, wxT("�����ѡ������"),
                             wxPoint(10,10), wxDefaultSize,
                             WXSIZEOF(choices), choices,
                             1, wxRA_SPECIFY_ROWS ); 
	hsizer2->Add(m_radio,0,wxALL,5);
	hsizer2->Add(new wxButton(this,ID_SHOWDATA_CLEARBTN,"���"),0,wxALL,5);
	sizerTop->Add(hsizer2,0,wxALL,5);
	SetSizerAndFit(sizerTop);
	SetInitialSize(wxSize(950,650));
	return true;
}

void MyGridDialog::OnClickPre(wxCommandEvent & event)
{
	wxString page = m_page->GetValue();
	long lpage;
	int ipage;
	page.ToLong(&lpage);
	ipage = (int)lpage;
	ipage--;
	m_page->SetValue(wxString::Format("%d",ipage));
	RefreshGridData();
}

void MyGridDialog::OnClickNext(wxCommandEvent & event)
{
	wxString page = m_page->GetValue();
	long lpage;
	int ipage;
	page.ToLong(&lpage);
	ipage = (int)lpage;
	ipage++;
	m_page->SetValue(wxString::Format("%d",ipage));
	RefreshGridData();
}

void MyGridDialog::OnClickJump(wxCommandEvent & event)
{
	wxString page = m_page->GetValue();
	long lpage;
	int ipage;
	page.ToLong(&lpage);
	ipage = (int)lpage;
	if(ipage < 1)
	{
		ipage = 1;
	}
	RefreshGridData();
}

void MyGridDialog::OnClickDelete(wxCommandEvent & event)
{
	wxString oldtitle = GetTitle();
	SetTitle(oldtitle + " ��������ɾ���С��������Եȣ�����");
	if ( m_grid->IsSelection() )
    {
        wxGridUpdateLocker locker(m_grid);
        for ( int n = 0; n < m_grid->GetNumberRows();  n++)
        {
            if ( m_grid->IsInSelection( n , 0 ) )
			{
				DeleteData(n);
			}
        }
    }
	RefreshGridData();
	SetTitle(oldtitle);
}

void MyGridDialog::OnClickClearData(wxCommandEvent & event)
{
	wxString radioStr = m_radio->GetString(m_radio->GetSelection());
	wxMessageDialog msgdlg(this,"��ܰ��ʾ��ȷ��Ҫ����<"+radioStr+">�ò�����\n\n"
				"�����ȷ���Ļ�����ѡ��\"�ǵģ���ʼ���\"\n\n"
				"������ѡ��\"����ˣ�������˼\"��ȡ�������������\n\n",
				"���ǰ����ܰ��ʾ", wxYES_NO);
	msgdlg.SetYesNoLabels("�ǵģ���ʼ���", "����ˣ�������˼");
	int dlgret = msgdlg.ShowModal();
	if(dlgret == wxID_NO)
	{
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	if(!db->TableExists(global_MyModules[m_modid]))
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ������","����");
		this->Close(true);
		return;
	}

	if(radioStr == "���["+m_catname+"]��������б��زɼ�����")
	{
		db->ExecuteUpdate(wxString::Format("DELETE FROM '%s' WHERE catid='%s'",
								global_MyModules[m_modid],m_catid )
						  );
	}
	else if(radioStr ==  "���["+glMyModuleNames[m_modid]+"]ģ������б��زɼ�����")
	{
		db->ExecuteUpdate(wxString::Format("DELETE FROM '%s'",global_MyModules[m_modid]));
	}
	else if(radioStr == "�������վ�����б��زɼ�����")
	{
		MyHashString::iterator it;
		wxString key,value;
		for( it = global_MyModules.begin(); it != global_MyModules.end(); ++it )
		{
			key = it->first;
			value = it->second;
			// do something useful with key and value
			if(!db->TableExists(value))
				continue;
			db->ExecuteUpdate(wxString::Format("DELETE FROM '%s'",value) );
		}
	}
	else
	{
		wxMessageBox("��ûѡ���κ�ѡ��","����");
		return;
	}
	RefreshGridData();
}

void MyGridDialog::DeleteData(int row)
{
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	if(!db->TableExists(global_MyModules[m_modid]))
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ������","����");
		this->Close(true);
		return;
	}
	wxString id = m_grid->GetCellValue(row,0);
	if(id == "")
	{
		//long LId;
		//id.ToLong(&LId);
		//wxMessageBox(wxString::Format("��%d�м�¼�����ڣ�ûʲô����ɾ���ģ�",(int)LId+1),"����");
		return;
	}
	db->ExecuteUpdate(wxString::Format("DELETE FROM '%s' WHERE id='%s'",
							global_MyModules[m_modid],id));
}

void MyGridDialog::RefreshGridData()
{
	wxString page = m_page->GetValue();
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	wxSQLite3ResultSet set;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	if(!db->TableExists(global_MyModules[m_modid]))
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ������","����");
		this->Close(true);
		return;
	}
	set = db->ExecuteQuery(wxString::Format("SELECT count(*) as totalcount FROM '%s' WHERE catid='%s'",global_MyModules[m_modid],m_catid));
	int pagenum = 1;
	long totalcount,lpage;
	int itotalcount,ipage;
	if(set.NextRow())
	{
		set.GetAsString("totalcount").ToLong(&totalcount);
		itotalcount = (int)totalcount;
		pagenum = itotalcount/MYGRID_DATA_SHOWNUM;
		if(pagenum <= 0)
		{
			pagenum = 1;
		}
		else if(itotalcount % MYGRID_DATA_SHOWNUM != 0)
		{
			pagenum++;
		}
	}
	else
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ�����ѯ����ʱʧ�ܣ�","����");
		return;
	}
	page.ToLong(&lpage);
	ipage = (int)lpage;
	if(ipage > pagenum)
	{
		ipage = 1;
	}
	if(ipage < 1)
	{
		ipage = pagenum;
	}
	set = db->ExecuteQuery(wxString::Format("SELECT * FROM '%s' WHERE catid='%s' limit %d,%d",global_MyModules[m_modid],m_catid,(ipage - 1) * MYGRID_DATA_SHOWNUM ,MYGRID_DATA_SHOWNUM));
	int colcount = set.GetColumnCount();
	m_grid->ClearGrid();
	int i=0,j=0;
	while(set.NextRow())
	{
		for(i=0;i < colcount;i++)
		{
			if(i==3)
				m_grid->SetCellValue(j,i,set.GetAsString(i).Mid(0,45) + "....");
			else
				m_grid->SetCellValue(j,i,set.GetAsString(i));
		}
		j++;
	}
	m_page->SetValue(wxString::Format("%d",ipage));
	m_pagenum->SetLabel(wxString::Format("�ܹ�%dҳ%d����¼",pagenum,itotalcount));
}

void MyGridDialog::OnGridChanging(wxGridEvent & event)
{
	int row = event.GetRow(),
        col = event.GetCol();
	if(col == 3)
	{
		event.Veto();
		wxMessageBox("ϵͳ�������޸���Ϣ�����ֶΣ����޸������ֶΣ�","����");
		return;
	}
}

void MyGridDialog::OnGridChanged(wxGridEvent & event)
{
	int row = event.GetRow(),
        col = event.GetCol();
	if(col != 4)
	{
		event.Veto();
		wxMessageBox("��ǰϵͳֻ�����޸ĵ�5��state״̬�ֶΣ�","����");
		return;
	}
	wxSQLite3Database * db = wxGetApp().db;
	wxString SiteDBPath = glSiteDBPath;
	if(wxFile::Exists(SiteDBPath) == false)
	{
		wxMessageBox(SiteDBPath+"�ļ�������!","����");
		this->Close(true);
		return;
	}
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	if(!db->TableExists(global_MyModules[m_modid]))
	{
		wxMessageBox(global_MyModules[m_modid]+"���ݿ������","����");
		this->Close(true);
		return;
	}
	wxString id = m_grid->GetCellValue(row,0);
	if(id == "")
	{
		wxMessageBox("���м�¼�����ڣ�ûʲô�����޸ĵģ�","����");
		return;
	}
	wxString colname = m_grid->GetColLabelValue(col);
	db->ExecuteUpdate(wxString::Format("UPDATE '%s' SET '%s'='%s' WHERE catid='%s' AND id='%s'",
							global_MyModules[m_modid],colname,m_grid->GetCellValue(row,col),m_catid,id));
}

bool MyApp::OnInit()
{
	wxImage::AddHandler(new wxPNGHandler);
	MyFrame *frame = new MyFrame( _("���ܲɼ���2013PC����� ") + gl_version_number + _(" ����:zenglong ��zengl.com�ṩ����֧��"), wxPoint(50, 50), wxSize(980, 650) );
	wxBitmap bitmap;
	bool ok = bitmap.LoadFile(wxT("splash.png"), wxBITMAP_TYPE_PNG); //��������
	if (ok)
    {
        new wxSplashScreen(bitmap,
            wxSPLASH_CENTRE_ON_SCREEN|wxSPLASH_TIMEOUT,
            6000, frame, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxSIMPLE_BORDER|wxSTAY_ON_TOP);
    }
	frame->Center();
    frame->Show(true);
    SetTopWindow(frame);
	glmainFrame = frame;
	db = new wxSQLite3Database();
	glDB = db;
	settingDB = new wxSQLite3Database();
	GetAllAreas();
    return true;
}

void MyApp::OnFatalException()
{
	//((MyFrame *)GetTopWindow())->OnSave((wxCommandEvent)NULL);
	wxMessageBox(wxT("���������쳣���Ѿ��������ݣ�׼���˳�"),wxT("�쳣�˳�����"));
}

MyTreeCtrl::MyTreeCtrl(wxWindow *parent, const wxWindowID id,
               const wxPoint& pos,
               const wxSize& size,
               long style)
			:wxTreeCtrl(parent, id, pos, size, style)
{
	return;
}

#include "icon1.xpm"
#include "icon3.xpm"
#include "checked.xpm"
#include "unchecked.xpm"
#include "sample.xpm"


MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size)
{
	m_mgr.SetManagedWindow(this);
    wxMenu *menuFile = new wxMenu;

	menuFile->Append( ID_MENU_FILE_HELP, _("����(&H)") );
    menuFile->Append( ID_About, _("����(&A)...") );
    menuFile->AppendSeparator();
    menuFile->Append( ID_Quit, _("�˳�(&E)") );

	wxMenu *PersPective = new wxMenu;
	PersPective->Append( ID_MENU_RESTORE_PERSPECTIVE,_("�ָ�Ĭ�ϲ���(&R)"));
	PersPective->Append( ID_MENU_CLEAR_LOGS,_("������־(&C)"));

	wxMenu *Category = new wxMenu;
	Category->Append(ID_MENU_CATEGORY_FIND,_("���������б��в��ҷ���\tCtrl+F"));
	Category->Append(ID_MENU_CATEGORY_SELECTALL,_("�������б�ȫѡ"));
	Category->Append(ID_MENU_CATEGORY_UNSELECTALL,_("�������б�ȫ��ѡ"));
	Category->Append(ID_MENU_CATEGORY_EXPANDALL,_("�������б�ȫ��չ��"));
	Category->Append(ID_MENU_CATEGORY_UNEXPANDALL,_("�������б�ȫ���۵�"));

	wxMenu * ExtraFeature = new wxMenu;
	ExtraFeature->Append(ID_MENU_EXTRA_REVERSE_ORDER,_("���򷢲�"),_("���򷢲�"),true);
	ExtraFeature->Append(ID_MENU_EXTRA_POST_PENDING,_("�����������"),_("�����������"),true);
	ExtraFeature->Append(ID_MENU_EXTRA_LOCAL_IMG,_("ͼƬ���ص��������ϴ�"),_("��Զ��ͼƬ���ص��������ϴ�"),true);
	ExtraFeature->Append(ID_MENU_EXTRA_DEBUG,_("�����ű�����"),_("�����ű�����"),true);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, _("�ļ�������(&F)") );
	menuBar->Append( PersPective , _("��ͼ(&P)"));
	menuBar->Append( Category , _("�����б����(&C)"));
	menuBar->Append( ExtraFeature , _("�ɼ����⹦��(&E)"));

    SetMenuBar( menuBar );

	m_statusbar = new MyStatusBar(5,this,wxID_ANY);
	m_statusbar->SetStatusText("��ӭʹ�����ܲɼ���",0);
	int statuswidth[5] = {150,100,300,150,150};
	m_statusbar->SetStatusText("�¼���������",1);
	m_statusbar->SetStatusText("�ٷֱ�",3);
	m_statusbar->SetStatusText("ץ���¼�",4);
	m_statusbar->SetStatusWidths(5,statuswidth);
	//wxStatusBarPane statusPane= m_statusbar->GetField(1);
	//statusPane.
	//m_gauge = new wxGauge((wxWindow *)&statusPane,wxID_ANY,100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxNO_BORDER);
	SetStatusBar((wxStatusBar *)m_statusbar);
	
    //SetStatusText( _("Welcome to wxWidgets!") );
	
	wxFont myfont( 12, 74, 90, 90, false, wxT("����"));
	treeCtrl = new MyTreeCtrl(this,IDCaiji_Tree_Ctrl,wxDefaultPosition, wxDefaultSize,wxTR_DEFAULT_STYLE | wxTR_EDIT_LABELS);
	wxIcon icons[4];
    icons[0] = wxIcon(icon3_xpm);
    icons[1] = wxIcon(icon1_xpm);
	//int width  = icons[1].GetWidth(),height = icons[1].GetHeight();
	int width=14,height=20;
	wxImageList* imageList = new wxImageList(width, height);
	imageList->Add(wxIcon(icons[0])); //�����ļ���ͼ�� ��ʱû��������ͼ�꣬�����հ׼�϶
	imageList->Add(wxIcon(icons[1])); //�����ļ�ͼ�� 
	treeCtrl->AssignImageList(imageList);
	icons[2] = wxIcon(unchecked_xpm);
	icons[3] = wxIcon(checked_xpm);
	width  = icons[2].GetWidth();
	height = icons[2].GetHeight();
	imageList = new wxImageList(width, height);
	imageList->Add(wxIcon(icons[2])); //���÷�ѡ��ʱ��ͼ��
	imageList->Add(wxIcon(icons[3])); //����ѡ��ʱ��ͼ��
	treeCtrl->AssignStateImageList(imageList); //����tree�ؼ���״̬ͼ�ꡣ

	wxTreeItemId rootId = treeCtrl->AddRoot(wxT("û�л�ȡ����"), -1, -1,new MyTreeItemData(wxT(""),wxT(""),wxT(""),wxT("")));
	
	treeCtrl->SetItemState(rootId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
	treeCtrl->SetItemFont(rootId,myfont);
	                                    
	textForLog = new wxRichTextCtrl(this, -1, _(""),
					  wxDefaultPosition, wxSize(200,150),
					  wxNO_BORDER | wxTE_MULTILINE | wxTE_READONLY);
	textForDebug = new wxTextCtrl(this, ID_TEXT_FOR_DEBUG, _("�����������"),wxDefaultPosition,wxSize(200,20),wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	
	wxAuiToolBar * tb = new wxAuiToolBar(this,wxID_ANY,wxDefaultPosition,wxSize(200,30), wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT); //������
	wxBitmap tb_bmp1 = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16,16));
	textUrlName = new wxTextCtrl(tb, IDUrl_Name_TextCtrl, _("������ַ"),wxDefaultPosition,wxSize(200,20),wxTE_PROCESS_ENTER | wxTE_PROCESS_TAB);
	//textPatternTest = new wxTextCtrl(tb, ID_TEXT_PATTERN, _("�������"),wxDefaultPosition,wxSize(450,20),wxTE_PROCESS_TAB);
	//wxButton * urlNameBtn = new wxButton(tb,IDUrlName_Btn,_("��ȡ������Ϣ"));
	tb->AddControl(textUrlName);
	tb->AddTool(IDUrlName_Btn,_("��ȡ������Ϣ"),tb_bmp1);
	//tb->AddControl(urlNameBtn);
	tb->AddTool(ID_ITEM_START,_("��ʼ"),tb_bmp1);
	tb->AddTool(ID_ITEM_PAUSE,_("��ͣ"),tb_bmp1);
	tb->AddTool(ID_ITEM_STOP,_("ֹͣ"),tb_bmp1);
	tb->AddTool(ID_ITEM_TEST,_("����"),tb_bmp1);
	tb->AddTool(ID_ITEM_WEB_OFFICE,_("�򿪳�ʼ���ű�"),tb_bmp1);
	tb->AddTool(ID_ITEM_RESET_INIT,_("���¼��س�ʼ���ű�"),tb_bmp1);
	m_checkAutoPass = new wxCheckBox(tb,wxID_ANY,"�Ƿ����Զ�����");
	tb->AddControl(m_checkAutoPass);
	m_toolbar = tb;
	wxAuiToolBar * tb2 = new wxAuiToolBar(this,wxID_ANY,wxDefaultPosition,wxSize(200,30), wxAUI_TB_DEFAULT_STYLE |
                                         wxAUI_TB_OVERFLOW |
                                         wxAUI_TB_TEXT |
                                         wxAUI_TB_HORZ_TEXT);
	m_textpostnum = new wxTextCtrl(tb2,wxID_ANY,_("����ÿ������Ĳɼ�����"),wxDefaultPosition,wxSize(200,20));
	tb2->AddControl(m_textpostnum);
	tb2->AddTool(ID_ITEM2_RESULT,_("�����زɼ����"),tb_bmp1);
	m_checkShowBall = new wxCheckBox(tb2,wxID_ANY,"�ɼ�������ʱ��ʾ����");
	tb2->AddControl(m_checkShowBall);
	m_textForTimeOut = new wxTextCtrl(tb2,wxID_ANY,_("60"),wxDefaultPosition,wxSize(40,20));
	tb2->AddControl(m_textForTimeOut);
	tb2->AddControl(new wxStaticText(tb2,wxID_ANY,"��ɼ���ʱ"));
	tb2->AddTool(ID_ITEM_SHOW_LOG,_("��־"),tb_bmp1);
	m_checkCaijiCompany = new wxCheckBox(tb2,ID_CHECK_CAIJI_COMPANY,"�Ƿ�ɼ���˾��Ϣ");
	tb2->AddControl(m_checkCaijiCompany);
	tb2->AddControl(new wxStaticText(tb2,wxID_ANY,"  ���ɼ�"));
	m_textForMaxUploadIMG = new wxTextCtrl(tb2,wxID_ANY,_("����"),wxDefaultPosition,wxSize(40,20));
	tb2->AddControl(m_textForMaxUploadIMG);
	tb2->AddControl(new wxStaticText(tb2,wxID_ANY,"��ͼƬ"));
	m_toolbar2 = tb2;
	//tb->AddControl(textPatternTest);
	m_mgr.AddPane(tb,wxAuiPaneInfo().Name(_("textUrlName")).MinSize(200,30).Caption(_("��ַ����toolbar")).ToolbarPane().Top().Row(1));
	m_mgr.AddPane(tb2,wxAuiPaneInfo().Name(_("textResult")).MinSize(200,30).Caption(_("�������toolbar")).ToolbarPane().Top().Row(2));
	m_mgr.AddPane(treeCtrl, wxAuiPaneInfo().Left().MinSize(260,500).Caption(_("�������")));
	m_mgr.AddPane(textForLog, wxAuiPaneInfo().Center().MinSize(650,500).Caption(_("��־���")).Name(_("text2")));
	m_mgr.AddPane(textForDebug, wxAuiPaneInfo().Bottom().MinSize(200,20).Caption(_("�������������")).Name(_("text3")));
	m_ProgressBar = new wxGauge(this,wxID_ANY,100);
	m_mgr.AddPane(m_ProgressBar,wxAuiPaneInfo().Bottom().MinSize(710,20).Caption(_("�ɼ��ܽ���: 0%")));
	m_mgr.Update();
	
	m_DefPerspective = m_mgr.SavePerspective();
	m_taskBarIcon = new MyTaskBarIcon();

    // we should be able to show up to 128 characters on recent Windows versions
    // (and 64 on Win9x)
	wxIcon icon(sample_xpm);
    if ( !m_taskBarIcon->SetIcon(icon,
                                 "���ܲɼ���2013PC����� "+gl_version_number+"\n"
								 "����: zenglong\n"
								 "www.zengl.com�ṩ����֧��\n"
								 "��ܰ��ʾ���������غ���ʾ��") )
    {
		wxMessageBox("ϵͳ��֧����������ͼ�꣡","����");
    }
	SetIcon(sample_xpm);
	mythread = mythread2 = NULL;
	curl_global_init(CURL_GLOBAL_ALL); 
	logfp = fopen("mylogs.txt","w+");
	if(logfp == NULL)
	{
		wxMessageBox("����־�ļ�ʧ�ܣ�","����");
	}
	else
		fclose(logfp);
	logfp = NULL;
	zengl_debuglogfile = fopen("zengl_debuglogs.txt","w+");
	m_dlgFind = NULL;
	ZL_EXP_VOID * VM = zenglApi_Open();
	zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE));
	zenglApi_SetModFunHandle(VM,0,"bltSetInitManageUrl",global_bltSetInitManageUrl);
	zenglApi_SetModFunHandle(VM,0,"bltSetModulePath",global_bltSetModulePath);
	if(zenglApi_Run(VM,"��ʼ���ű�.zl") == -1)
		wxMessageBox(wxString::Format("'��ʼ���ű�.zl'ִ��ʧ�ܣ�%s",zenglApi_GetErrorString(VM)),"����");
	zenglApi_Close(VM);

	//�����ȡconfig.ini�����ļ�
	wxFileInputStream is(wxT("config.ini"));
	wxFileConfig *conf = new wxFileConfig(is);
	wxString value;
	conf->Read(_("ToolbarItems/UrlName"),&value);
	textUrlName->SetValue(value);
	conf->Read(_("ToolbarItems/PostNum"),&value);
	m_textpostnum->SetValue(value);
	conf->Read(_("ToolbarItems/TimeOut"),&value);
	m_textForTimeOut->SetValue(value);
	conf->Read(_("ToolbarItems/MaxUploadIMG"),&value);
	m_textForMaxUploadIMG->SetValue(value);
	long tmp;
	conf->Read(_("ToolbarItems/checkShowBall"),&tmp);
	if(tmp == 1)
		m_checkShowBall->SetValue(true);
	conf->Read(_("ToolbarItems/CaijiCompany"),&tmp);
	if(tmp == 1)
		m_checkCaijiCompany->SetValue(true);
	conf->Read(_("ToolbarItems/AutoPass"),&tmp);
	if(tmp == 1)
		m_checkAutoPass->SetValue(true);
	conf->Read(_("ExtraMenus/ReverseOrder"),&tmp);
	if(tmp == 1)
		ExtraFeature->Check(ID_MENU_EXTRA_REVERSE_ORDER,true);
	conf->Read(_("ExtraMenus/PostPending"),&tmp);
	if(tmp == 1)
		ExtraFeature->Check(ID_MENU_EXTRA_POST_PENDING,true);
	conf->Read(_("ExtraMenus/LocalImg"),&tmp);
	if(tmp == 1)
		ExtraFeature->Check(ID_MENU_EXTRA_LOCAL_IMG,true);
	conf->Read(_("ExtraMenus/Debug"),&tmp);
	if(tmp == 1)
		ExtraFeature->Check(ID_MENU_EXTRA_DEBUG,true);
	m_isDebugPause = false;
	delete conf;
}

MyFrame::~MyFrame()
{
	global_MyAreas.Clear();
	m_mgr.UnInit();
	if(mythread != NULL)
	{
		//if(!mythread->IsPaused())
			//mythread->Pause();
		mythread->Delete();
		mythread = NULL;
	}
	if(mythread2 != NULL)
	{
		//if(!mythread2->IsPaused())
			//mythread2->Pause();
		mythread2->Delete();
		mythread2 = NULL;
	}
	curl_global_cleanup();
	//if(gl_isQuerySql_forZenglRun)
	//{
		gl_sqlset_forZenglRun.~wxSQLite3ResultSet(); //���ﲻ�ͷţ�������ڴ�й©����Ϊȫ��sql�����
	//	gl_isQuerySql_forZenglRun = 0;
	//}
	wxGetApp().db->Close();
	delete wxGetApp().db;
	settingDB->Close();
	delete settingDB;
	delete m_taskBarIcon;
	if(logfp != NULL)
	{
		fclose(logfp);
		logfp = NULL;
	}
	if(zengl_debuglogfile != NULL)
	{
		fclose(zengl_debuglogfile);
		zengl_debuglogfile = NULL;
	}
	if(m_dlgFind != NULL)
	{
		delete m_dlgFind;
		m_dlgFind = NULL;
	}

	//���汣��������Ϣ
	wxFileInputStream is(wxT("config.ini"));
	wxFileConfig *conf = new wxFileConfig(is);
	conf->Write("ToolbarItems/UrlName",textUrlName->GetValue());
	conf->Write("ToolbarItems/PostNum",m_textpostnum->GetValue());
	conf->Write("ToolbarItems/TimeOut",m_textForTimeOut->GetValue());
	conf->Write("ToolbarItems/MaxUploadIMG",m_textForMaxUploadIMG->GetValue());
	conf->Write("ToolbarItems/checkShowBall",m_checkShowBall->IsChecked());
	conf->Write("ToolbarItems/CaijiCompany",m_checkCaijiCompany->IsChecked());
	conf->Write("ToolbarItems/AutoPass",m_checkAutoPass->IsChecked());
	wxMenuBar * menubar = this->GetMenuBar();
	conf->Write("ExtraMenus/ReverseOrder",menubar->IsChecked(ID_MENU_EXTRA_REVERSE_ORDER));
	conf->Write("ExtraMenus/PostPending",menubar->IsChecked(ID_MENU_EXTRA_POST_PENDING));
	conf->Write("ExtraMenus/LocalImg",menubar->IsChecked(ID_MENU_EXTRA_LOCAL_IMG));
	conf->Write("ExtraMenus/Debug",menubar->IsChecked(ID_MENU_EXTRA_DEBUG));
	wxFileOutputStream os(wxT("config.ini")); 
	conf->Save(os);
	os.Close(); 
	delete conf;
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxString zlversion = wxString::Format("v%d.%d.%d",ZL_EXP_MAJOR_VERSION,ZL_EXP_MINOR_VERSION,ZL_EXP_REVISION);
	wxMessageBox( _("���ܲɼ���2013PC����� ")+ gl_version_number +_("\n��ϵͳ����C++,wxWidgets�Լ�zenglǶ��ʽ�������"+zlversion+"����\n ����: zenglong\nwww.zengl.com�ṩ����֧��"),
                  _("����"),
                  wxOK | wxICON_INFORMATION, this );
}

void MyFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
	wxExecute("hh.exe help.chm");
}

void MyFrame::OnRestorePespective(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.LoadPerspective(m_DefPerspective);
}

extern "C"
{
	size_t mywxcurl_string_write(void* ptr, size_t size, size_t nmemb, void* pcharbuf)
    {
        size_t iRealSize = size * nmemb;
		//int length;
		if(char_myglStr == NULL)
		{
			char_myglTotalnum = iRealSize ;
			char_myglStr = (char *)malloc(char_myglTotalnum + 1);
			//char_myglStr[0] = NULL;
		}
		else
		{
			char_myglTotalnum += iRealSize;
			char_myglStr = (char *)realloc(char_myglStr , char_myglTotalnum + 1);
		}
		strncpy(char_myglStr+(char_myglTotalnum - iRealSize),(char *)ptr ,iRealSize);
		char_myglStr[char_myglTotalnum] = NULL;
		char_myglTotalnum_forShow = char_myglTotalnum;
		wxCommandEvent eventForSetCurlNum(wxEVT_MY_SET_CURL_NUM,ID_MY_WINDOW);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventForSetCurlNum.Clone());
        return iRealSize;
    }

	size_t mywxcurl_file_write(void *ptr, size_t size, size_t nmemb, void *stream)
	{
		size_t iRealSize = size * nmemb;
		if(char_myglTotalnum == 0)
			char_myglTotalnum = iRealSize ;
		else
			char_myglTotalnum += iRealSize;
		size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);
		char_myglTotalnum_forShow = char_myglTotalnum;
		wxCommandEvent eventForSetCurlNum(wxEVT_MY_SET_CURL_NUM,ID_MY_WINDOW);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventForSetCurlNum.Clone());
		return iRealSize;
	}

	size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream) 

	{

		int written = fwrite(ptr, size, nmemb, (FILE *)stream);

		return written;

	}
}


void MyFrame::OnUrlNameBtn(wxAuiToolBarEvent& WXUNUSED(event))
{
	if(mythread != NULL || mythread2 != NULL)
	{
		wxMessageBox("�ɼ������߳����������У�ԭ����������ڻ�ȡ������Ϣ�������ڽ��вɼ���\n��ȴ���ز������������ԣ�","����");
		return;
	}
	wxString strUrlName = TrimBoth(textUrlName->GetValue());
	if(strUrlName != textUrlName->GetValue())
	{
		textUrlName->SetValue(strUrlName);
	}
	//textForLog->AppendText(_("�������ַ�ǣ�")+strUrlName+'\n');
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�������ַ�ǣ�")+strUrlName+",������ķ���ܶ࣬�ﵽ�����ʱ�������úó�ʱʱ�䣬���������ʱ������Ϊ1000��ĳ�ʱʱ�䣬ϵͳҲ���Զ����ã���Ȼ����Ҫ��ô���������賤�㱣�գ������ĵȴ�����������������������Ҫһ�����Ӳ��ܽ����ݴ�������\n",
								MY_RICHTEXT_NORMAL_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	glmainFrame->m_textForTimeOut->SetValue("1000");
	if(mythread == NULL && mythread2 == NULL)
	{
		mythread = new MyThread(this);
		mythread->Create();
		mythread->Run();
	}
	else
	{
		wxMessageBox("�ɼ������߳����������У�ԭ����������ڻ�ȡ������Ϣ�������ڽ��вɼ���\n��ȴ���ز������������ԣ�","����");
	}
    return;
}

void MyFrame::OnTextCtrlEnter(wxCommandEvent& WXUNUSED(event))
{
	wxAuiToolBarEvent event2;
	OnUrlNameBtn(event2);
}

void MyFrame::OnTextDebugEnter(wxCommandEvent& WXUNUSED(event))
{
	if(m_isDebugPause == true && mythread2->IsPaused())
	{
		m_isDebugPause =false;
		mythread2->Resume();
	}
}

void MyFrame::OnMyEvent(wxMyLogEvent& event)
{
	int totalnum = textForLog->GetNumberOfLines();
	if(totalnum > 150) 
	{
		wxCommandEvent eventForClearLogs( wxEVT_MY_CLEARLOGS , ID_MY_WINDOW);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
	}
	/*if(textForLog->GetNumberOfLines() > 150) 
	{
		textForLog->Remove(0, textForLog->GetLineLength(textForLog->GetNumberOfLines()/2) + 1);
		textForLog->SetInsertionPointEnd();
    }*/
	textForLog->SetInsertionPointEnd();
	if(event.m_msgcolorEnum == MY_RICHTEXT_RED_STYLE)
	{
		textForLog->BeginTextColour(*wxRED);
	}
	else if(event.m_msgcolorEnum == MY_RICHTEXT_GREEN_STYLE)
	{
		wxColour green(27,124,11);
		textForLog->BeginTextColour(green);
	}
	else
	{
		wxColour black(0,0,0);
		textForLog->BeginTextColour(black);
	}
	totallogsLength += event.m_strEventMsg.length();
	textForLog->WriteText(event.m_strEventMsg);
	logfp = fopen("mylogs.txt","a");
	if(logfp == NULL)
	{
		wxMessageBox("����־�ļ�ʧ�ܣ�","����");
	}
	else
	{
		wxScopedCharBuffer myglStrForUTF8 = event.m_strEventMsg.ToUTF8();
		fwrite(myglStrForUTF8.data(), 1 , myglStrForUTF8.length(),logfp);
		fclose(logfp);
		logfp = NULL;
	}
	//textForLog->AppendText(wxString::Format(_("length is %d res is %s \n"),charlength ,myglStr));
	textForLog->ShowPosition(textForLog->GetLastPosition()); 
	textForLog->ScrollLines(-1);
	//if(event.m_msgcolorEnum != MY_RICHTEXT_NORMAL_STYLE)
	//{
	textForLog->EndTextColour();
	//}
}

void MyFrame::OnClearLogs(wxCommandEvent& event)
{
	int totalnum = textForLog->GetNumberOfLines();
	if(totalnum > 150) 
	{
		int perlength = (float)totallogsLength / (float)totalnum;
		int clearlength = totallogsLength - perlength * 100;
		totallogsLength -= clearlength;
		textForLog->BeginSuppressUndo();
		textForLog->Delete(wxRichTextRange(0, clearlength));
		textForLog->BeginSuppressUndo();
	}
}

void MyFrame::OnClearAll_Logs(wxCommandEvent& event)
{
	textForLog->Clear();
}

void MyFrame::OnFindCategory(wxCommandEvent& event)
{
	if ( m_dlgFind )
    {
        wxDELETE(m_dlgFind);
    }
    else
    {
        m_dlgFind = new wxFindReplaceDialog
                        (
                            this,
                            &m_findData,
                            _("�ڷ����б��в��ҷ���")
                        );
		m_findData.SetFlags(wxFR_DOWN);
        m_dlgFind->Show(true);
		glIsDialogInShow = true;
    }
}

void MyFrame::OnCategorySelectAll(wxCommandEvent& event)
{
	wxTreeItemId rootId = treeCtrl->GetRootItem();
	wxTreeItemIdValue cookie,cookie2;
	wxTreeItemId itemId = treeCtrl->GetFirstChild(rootId,cookie);
	do{
		if(itemId.IsOk())
		{
			wxTreeItemId childid = treeCtrl->GetFirstChild(itemId,cookie2);
			do{
				if(childid.IsOk())
					treeCtrl->SetItemState(childid,1); //��Ϊ1 ��Ϊѡ��״̬
			}while(childid = treeCtrl->GetNextChild(childid,cookie2));
		}
	}while(itemId = treeCtrl->GetNextChild(itemId,cookie));
}

void MyFrame::OnCategoryUnSelectAll(wxCommandEvent& event)
{
	wxTreeItemId rootId = treeCtrl->GetRootItem();
	wxTreeItemIdValue cookie,cookie2;
	wxTreeItemId itemId = treeCtrl->GetFirstChild(rootId,cookie);
	do{
		if(itemId.IsOk())
		{
			wxTreeItemId childid = treeCtrl->GetFirstChild(itemId,cookie2);
			do{
				if(childid.IsOk())
					treeCtrl->SetItemState(childid,0); //��Ϊ0 ��Ϊ��ѡ��״̬
			}while(childid = treeCtrl->GetNextChild(childid,cookie2));
		}
	}while(itemId = treeCtrl->GetNextChild(itemId,cookie));
}

void MyFrame::OnCategoryExpandAll(wxCommandEvent& event)
{
	treeCtrl->ExpandAll();
	return ;
}

void MyFrame::OnCategoryUnExpandAll(wxCommandEvent& event)
{
	treeCtrl->CollapseAll();
	return ;
}

void MyFrame::OnExtraCheck(wxCommandEvent& event)
{
	int menuid = event.GetId();
	wxMenuBar * menubar = this->GetMenuBar();
	if(menuid == ID_MENU_EXTRA_REVERSE_ORDER)
	{
		if(event.IsChecked() && menubar->IsChecked(ID_MENU_EXTRA_POST_PENDING))
		{
			wxMessageDialog msgdlg(this,"���ѹ�ѡ�˷����������\n\n�����ʱ��DT��̨������ݺ󣬸���ʱ��ܿ����ֻᵹ����\n\nһ��[��Ӧ] [�̳�]�Ȳ�����ͬʱ��ѡ����ʹ����"
						"\n\n����[�Ź�]���ֿ���ͬʱ��ѡ(��DTϵͳ����)������������Լ���վ��ʵ�����������!",
						"���򷢲� - ��ܰ��ʾ��", wxYES_NO | wxCANCEL);
			msgdlg.SetYesNoCancelLabels("ȷ��ͬʱ��ѡ", "ȡ�������ֻ����" , "ȡ�����β���");
			int dlgret = msgdlg.ShowModal();
			if(dlgret==wxID_YES)
			{
				menubar->Check(menuid,event.IsChecked());
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("��ѡ�˵��򷢲���ͬʱ�����������\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				return ;
			}
			else if(dlgret == wxID_CANCEL)
			{
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("ȡ�����򷢲�����\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				menubar->Check(menuid,false);
				return ;
			}
			else //ȡ�������ֻ����
			{
				menubar->Check(menuid,event.IsChecked());
				menubar->Check(ID_MENU_EXTRA_POST_PENDING,false);
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("��ѡ�˵��򷢲���ȡ�������������\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				return;
			}
		}
		else
		{
			menubar->Check(menuid,event.IsChecked());
			wxString printStr;
			if(event.IsChecked())
				printStr = _("��ѡ�˵��򷢲�\n");
			else
				printStr = _("ȡ�����򷢲�\n");
			wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,printStr,MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
			return ;
		}
	}
	else if(menuid == ID_MENU_EXTRA_POST_PENDING)
	{
		if(event.IsChecked() && menubar->IsChecked(ID_MENU_EXTRA_REVERSE_ORDER))
		{
			wxMessageDialog msgdlg(this,"���ѹ�ѡ�˵��򷢲�\n\n�����ʱ��DT��̨������ݺ󣬸���ʱ��ܿ����ֻᵹ����\n\nһ��[��Ӧ] [�̳�]�Ȳ�����ͬʱ��ѡ����ʹ����"
						"\n\n����[�Ź�]���ֿ���ͬʱ��ѡ(��DTϵͳ����)������������Լ���վ��ʵ�����������!",
						"����������� - ��ܰ��ʾ", wxYES_NO | wxCANCEL);
			msgdlg.SetYesNoCancelLabels("ȷ��ͬʱ��ѡ", "ȡ������ֻ�����" , "ȡ�����β���");
			int dlgret = msgdlg.ShowModal();
			if(dlgret==wxID_YES)
			{
				menubar->Check(menuid,event.IsChecked());
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("��ѡ�˷���������ˣ�ͬʱ���򷢲�\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				return ;
			}
			else if(dlgret == wxID_CANCEL)
			{
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("ȡ������������˲���\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				menubar->Check(menuid,false);
				return ;
			}
			else //ȡ������ֻ�����
			{
				menubar->Check(menuid,event.IsChecked());
				menubar->Check(ID_MENU_EXTRA_REVERSE_ORDER,false);
				wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("��ѡ�˷���������ˣ�ȡ�����򷢲�\n"),MY_RICHTEXT_NORMAL_STYLE);
				wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
				return ;
			}
		}
		else
		{
			menubar->Check(menuid,event.IsChecked());
			wxString printStr;
			if(event.IsChecked())
				printStr = _("��ѡ�˷����������\n");
			else
				printStr = _("ȡ�������������\n");
			wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,printStr,MY_RICHTEXT_NORMAL_STYLE);
			wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
			return ;
		}
	}
	else if(menuid == ID_MENU_EXTRA_LOCAL_IMG)
	{
		menubar->Check(menuid,event.IsChecked());
		wxString printStr;
		if(event.IsChecked())
			printStr = _("��ѡ��ͼƬ���ص��������ϴ�\n");
		else
			printStr = _("ȡ��ͼƬ���ص��������ϴ���ȡ����ͼƬ����������վ������Զ������\n");
		wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,printStr,MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
		return ;
	}
	else if(menuid  == ID_MENU_EXTRA_DEBUG)
	{
		menubar->Check(menuid,event.IsChecked());
		wxString printStr;
		if(event.IsChecked())
			printStr = _("��ѡ�˿����ű����ԣ��ɼ��ű�ִ��ʱ�ͻ���һ��ʼ�����жϣ���ʱ�����ڵ����������������ص�������(���������h�������鿴����ĵ��԰�����Ϣ)\n");
		else
			printStr = _("�û�ȡ���˽ű�����\n");
		wxMyLogEvent eventlog( wxEVT_MY_LOG_EVENT,wxID_ANY,printStr,MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(this->GetEventHandler(),eventlog.Clone());
		return ;
	}
}

wxTreeItemId MyFrame::findTreeItem(wxTreeCtrl* pTreeCtrl, const wxTreeItemId& root, 
								   const wxString& text, bool bIsNext,bool bCaseSensitive, bool bExactMatch) //�ڷ������н��в���
{
	wxTreeItemId item=root, child,parent;
	wxTreeItemIdValue cookie;
	wxString findtext(text), itemtext;
	bool bFound;
	if(!bCaseSensitive) findtext.MakeLower();
 
	do
	{
		while(item.IsOk())
		{
			itemtext = pTreeCtrl->GetItemText(item);
			if(!bCaseSensitive) itemtext.MakeLower();
			bFound = bExactMatch ? (itemtext == findtext) : itemtext.Contains(findtext);
			if(bFound) return item;
			child = pTreeCtrl->GetFirstChild(item, cookie);
			if(child.IsOk()) child = findTreeItem(pTreeCtrl, child, text,bIsNext, bCaseSensitive, bExactMatch);
			if(child.IsOk()) return child;
			if(bIsNext)
				item = pTreeCtrl->GetNextSibling(item);
			else
				item = pTreeCtrl->GetPrevSibling(item);
		} // while(item.IsOk())
		parent = pTreeCtrl->GetItemParent(item);
		if(parent.IsOk())
		{
			if(bIsNext)
				item = pTreeCtrl->GetNextSibling(parent);
			else
				item = pTreeCtrl->GetPrevSibling(parent);
		}
		else
			break;
	}while(item.IsOk());
 
	return item;
}

void MyFrame::OnFindDialog(wxFindDialogEvent& event)
{
    wxEventType type = event.GetEventType();

    if ( type == wxEVT_COMMAND_FIND || type == wxEVT_COMMAND_FIND_NEXT )
    {
		wxTreeItemId item,item2,rootid,parentid;
		wxTreeItemIdValue cookie;
        wxString findText = event.GetFindString();
		if(findText == "")
			return;
		item=treeCtrl->GetFocusedItem();
		rootid = treeCtrl->GetRootItem();
		if(!item.IsOk())
			item = rootid;
		if(!treeCtrl->GetCount()) return; // empty tree control - i.e. just cleared it?
		wxFindReplaceFlags findFlags = (wxFindReplaceFlags)event.GetFlags();
		bool bIsNext = findFlags & wxFR_DOWN ? true : false;
		bool bCaseSensitive = findFlags & wxFR_MATCHCASE ? true : false;
		bool bExactMatch = findFlags & wxFR_WHOLEWORD ? true : false;
		wxString itemtext;
		bool bFound;
		if(!bCaseSensitive) findText.MakeLower();
		do
		{
			if(treeCtrl->HasChildren(item))
			{
				if(bIsNext)
				{
					item = treeCtrl->GetFirstChild(item, cookie);
					if(!item.IsOk())
						goto not_found_loc;
				}
				else
				{
					parentid = treeCtrl->GetPrevSibling(item);
					if(!parentid.IsOk())
						goto not_found_loc;
					if(treeCtrl->HasChildren(parentid))
					{
						item = treeCtrl->GetLastChild(parentid);
						if(!item.IsOk())
							goto not_found_loc;
					}
					else
						item = parentid;
				}
			}
			else
			{
				if(bIsNext)
				{
					item2 = treeCtrl->GetNextSibling(item);
					if(!item2.IsOk())
					{
						parentid = treeCtrl->GetNextSibling(treeCtrl->GetItemParent(item));
						if(parentid.IsOk())
							item = parentid;
						else
							goto not_found_loc;
					}
					else
						item = item2;
				}
				else
				{
					item2 = treeCtrl->GetPrevSibling(item);
					if(!item2.IsOk())
					{
						parentid = treeCtrl->GetItemParent(item);
						if(parentid.IsOk())
							item = parentid;
						else
							goto not_found_loc;
					}
					else
						item = item2;
				}
			}
			itemtext = treeCtrl->GetItemText(item);
			if(!bCaseSensitive) itemtext.MakeLower();
			bFound = bExactMatch ? (itemtext == findText) : itemtext.Contains(findText);
			if(bFound) 
			{
				treeCtrl->SelectItem(item, true);
				treeCtrl->EnsureVisible(item);
				treeCtrl->SetFocus();
				return;
			}
		}while(item.IsOk());
		//item = findTreeItem(treeCtrl, item, findText, bIsNext , bCaseSensitive, bExactMatch);
		if(!item.IsOk()) 
		{
not_found_loc:
			wxMessageBox("û�ҵ��÷���","ϵͳ��ʾ");
			return;
		}
    }
    else if ( type == wxEVT_COMMAND_FIND_CLOSE )
    {
		wxFindReplaceDialog *dlg = event.GetDialog();
		dlg->Destroy();
        wxDELETE(m_dlgFind);
		m_dlgFind = NULL;
		glIsDialogInShow = false;
    }
    else
    {
        wxMessageBox(wxT("δ֪�ĶԻ�������¼�!"),"���󾯸�");
    }
}

void MyFrame::OnClickCheckCaijiCompany(wxCommandEvent& event)
{
	if(m_checkCaijiCompany->IsChecked())
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);
		eventforlog.SetEventMsg(_("����ѡ�˲ɼ���˾�����ڹ�Ӧ,�̳�,�Ź�,����,Ʒ��,���ⱦ��ģ��ɼ�ʱ���Զ��ɼ���˾,ÿ�ɼ�һ����˾�ͻ�����һ����Ա�����������ʵ������������ɼ����๫˾����ɽ϶�ķ���ʵ�û�(��Ϊ��Щ�û��ǲɼ����ɵģ������Ǳ���ע��ģ�)\n"),
			MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
}

void MyFrame::OnCaijiProgress(wxMyProgressEvent& event)
{
	int percent = 0;
	wxString percentStr="";
	if(m_statusbar->m_ProgressBar->GetRange() != event.m_processNum * 2)
		m_statusbar->m_ProgressBar->SetRange(event.m_processNum * 2);
	m_statusbar->m_ProgressBar->SetValue(event.m_processPos);
	percent = ((float)event.m_processPos/(float)(event.m_processNum * 2)) * 100;
	percentStr.Printf("�ٷֱȣ�%d%%",percent);
	m_statusbar->SetStatusText(percentStr,3);
	if(m_ProgressBar->GetRange() != event.m_total_processNum * 2)
		m_ProgressBar->SetRange(event.m_total_processNum * 2);
	m_ProgressBar->SetValue(event.m_total_processPos);
	percent = ((float)event.m_total_processPos/(float)(event.m_total_processNum * 2)) * 100;
	m_mgr.GetPane(m_ProgressBar).Caption(wxString::Format("�ɼ��ܽ���: %d%%",percent));
	m_mgr.Update();
}

void MyFrame::OnPauseThread(wxCommandEvent& event)
{
	//textForLog->AppendText(_("ϵͳ��ͣ��\n"));
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("ϵͳ��ͣ���ɵ����������[����]��ť������...\n"),
								MY_RICHTEXT_NORMAL_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	if(mythread2 != NULL && !mythread2->IsPaused())
	{
		mythread2->Pause();
		wxAuiToolBarItem *item = m_toolbar->FindTool(ID_ITEM_START);
		item->SetLabel("����");
		m_toolbar->Refresh(true); //ˢ�¹��������ü�����ť������ʾ����
		m_taskBarIcon->ShowBalloon("�ɼ����ݵ��������ݿ������ϵͳ��ͣ�������Դ������������ϴ���","��ܰ��ʾ",NIIF_INFO,
												10000);
	}
}

void MyFrame::OnDebugPauseThread(wxCommandEvent& event)
{
	//textForLog->AppendText(_("ϵͳ��ͣ��\n"));
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("\n>>> debug input:"),
								MY_RICHTEXT_NORMAL_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	if(m_isDebugPause == false && mythread2 != NULL && !mythread2->IsPaused())
	{
		m_isDebugPause = true;
		mythread2->Pause();
	}
}

void MyFrame::OnSetCurlNum(wxCommandEvent& event)
{
	if(char_myglTotalnum_forShow > 0)
		m_statusbar->SetStatusText(wxString::Format("ץ������:%d�ֽ�",char_myglTotalnum_forShow),4);
}

void MyFrame::OnUpdateCate(wxCommandEvent& event)
{
	wxTreeItemId rootId = treeCtrl->GetRootItem();
	wxFont myfont( 12, 74, 90, 90, false, wxT("����"));
	wxSQLite3Database *db = wxGetApp().db;
	wxString SiteName = textUrlName->GetValue();
	wxString SiteDBPath = "db/" + glEscapeSiteName + ".db";
	if(!db->IsOpen())
		db->Open(SiteDBPath);
	
	wxSQLite3ResultSet set = db->ExecuteQuery(wxT("SELECT url,chname,type FROM website where id = 1"));
	if(!set.NextRow())
	{
		//textForLog->AppendText(_("�����ݿ�website��ȡ��վ��Ϣʧ�ܣ�\n"));
		//textForLog->ShowPosition(textForLog->GetLastPosition()); 
		//textForLog->ScrollLines(-1);
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�����ݿ�website��ȡ��վ��Ϣʧ�ܣ�\n"),
								MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return;
	}
	wxString myurl = set.GetAsString("url");
	SiteName = TrimBoth(set.GetAsString(1));
	wxXmlDocument doc2;
	if (!doc2.Load("db/"+SiteName+".xml"))
	{
		/*textForLog->AppendText("����db/"+SiteName+".xml�ļ�ʧ��\n");
		textForLog->ShowPosition(textForLog->GetLastPosition()); 
		textForLog->ScrollLines(-1);*/
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,"����db/"+SiteName+".xml�ļ�ʧ��\n",
								MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return;
	}
	wxXmlNode * root2 = doc2.GetRoot();
	wxXmlNode *child2 = root2->GetChildren();
	if(global_MyModules.size() > 0)
	{
		global_MyModules.clear();
		glMyModuleNames.clear();
	}
	while(child2)
	{
		global_MyModules[child2->GetAttribute("id")] = child2->GetAttribute("module");
		glMyModuleNames[child2->GetAttribute("id")] = child2->GetAttribute("name");
		child2 = child2->GetNext();
	}
	treeCtrl->Delete(rootId);
	rootId = treeCtrl->AddRoot(set.GetAsString(1), -1, -1,
			new MyTreeItemData(wxT(""),_(""),_(""),set.GetAsString(0)));
	glCaijiWebName = set.GetAsString(1);
	glSiteDBPath = SiteDBPath;
	treeCtrl->SetItemState(rootId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
	treeCtrl->SetItemFont(rootId,myfont);
	wxTreeItemId itemId , parentItemid;
	wxCommandEvent eventForProgress( wxEVT_MY_PROGRESS , ID_MY_WINDOW);
	set = db->ExecuteQuery(wxT("SELECT count(*)as TotalCatNum FROM category"));
	set.NextRow();
	int totalcount,pernum=0;
	set.GetAsString("TotalCatNum").ToLong((long *)&totalcount);
	m_statusbar->m_total = totalcount;
	set = db->ExecuteQuery(wxT("SELECT catid,modid,catname FROM category"));
	wxString strModuleID = "0";
	while(set.NextRow())
	{
		if(strModuleID != set.GetAsString(1)) //��ģ�����ָ
		{
			itemId = treeCtrl->AppendItem(rootId,
										glMyModuleNames[set.GetAsString(1)], -1, -1,
										new MyTreeItemData(set.GetAsString(1),"0","",""));
			treeCtrl->SetItemState(itemId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
			treeCtrl->SetItemFont(itemId,myfont);
			parentItemid = itemId ;
		}
		itemId = treeCtrl->AppendItem(parentItemid,
									set.GetAsString(2) + "<"+glMyModuleNames[set.GetAsString(1)]+">", -1, -1,
									new MyTreeItemData(set.GetAsString(1),set.GetAsString(0),set.GetAsString(2),""));
		treeCtrl->SetItemState(itemId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
		treeCtrl->SetItemFont(itemId,myfont);
		pernum++;
		if(pernum==1)
			treeCtrl->ExpandAll();
		strModuleID = set.GetAsString(1);
		m_statusbar->m_pos = pernum; //8 task
		wxQueueEvent(m_statusbar->GetEventHandler(),eventForProgress.Clone());
	}
	treeCtrl->ExpandAll();
	m_mgr.GetPane(treeCtrl).Caption(wxString::Format("������� һ������%d������",totalcount));
	m_mgr.Update();
	//treeCtrl->Fit();
	//m_mgr.Update();
	/*textForLog->AppendText("��վ"+SiteName+"���سɹ���\n");
	textForLog->ShowPosition(textForLog->GetLastPosition()); 
	textForLog->ScrollLines(-1); */
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,"��վ"+SiteName+"���سɹ���\n",
								MY_RICHTEXT_NORMAL_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());

	/*wxXmlDocument doc;
	if (!doc.Load("test.xml"))
	{
		textForLog->AppendText(_("����test.xml�ļ�ʧ��\n"));
		//return NULL;
	}
	wxXmlNode *root = doc.GetRoot();
	rootId = treeCtrl->AddRoot(root->GetAttribute(_("name")), -1, -1,
                         new MyTreeItemData(wxT(""),_(""),root->GetAttribute(_("url")) ));
	treeCtrl->SetItemState(rootId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
	treeCtrl->SetItemFont(rootId,myfont);
	wxXmlNode *child = root->GetChildren();
	wxTreeItemId itemId;
	while(child)
	{
		itemId = treeCtrl->AppendItem(rootId,
                         child->GetAttribute(_("name")), -1, -1,
                         new MyTreeItemData(child->GetAttribute(_("modid")),child->GetAttribute(_("catid")),child->GetAttribute(_("")))
						 );
		treeCtrl->SetItemState(itemId, 0); //���ó�ʼ״̬ͼ��Ϊ0����ѡ��ʱ��ͼ��
		treeCtrl->SetItemFont(itemId,myfont);
		child = child->GetNext();
	}*/
}

void MyFrame::OnChecks(wxCommandEvent& event) //��ûʹ��
{
	wxTreeItemId rootId = treeCtrl->GetRootItem();
	wxTreeItemIdValue cookie;
	wxTreeItemId itemId = treeCtrl->GetFirstChild(rootId,cookie);
	do{
		treeCtrl->SetItemState(itemId,wxTREE_ITEMSTATE_NEXT);
	}while(itemId = treeCtrl->GetNextChild(itemId,cookie));
}

void MyFrame::OnClickStart(wxAuiToolBarEvent &event)
{
	wxString strStartLabel =m_toolbar->FindTool(ID_ITEM_START)->GetLabel();
	if(strStartLabel == "������")
	{
		//textForLog->AppendText(_("\nϵͳ�Ѿ�������״̬�У�\n"));
		//textForLog->ShowPosition(textForLog->GetLastPosition()); 
		//textForLog->ScrollLines(-1);
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("\nϵͳ�Ѿ�������״̬�У�\n"),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		return;
	}
	else if(strStartLabel == "����")
	{
		/*textForLog->AppendText(_("\n�û�����˼�����ϵͳ����ִ��\n"));
		textForLog->ShowPosition(textForLog->GetLastPosition()); 
		textForLog->ScrollLines(-1);*/
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("\n�û�����˼�����ϵͳ����ִ��\n"),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
	if(single_mod_cat_table == "")
	{
		//textForLog->AppendText(_("����˿�ʼ��")+glCaijiWebName+'\n');
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("����˿�ʼ��")+glCaijiWebName+'\n',MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		if(glCaijiWebName == "" || glSiteDBPath == "")
		{
			wxMessageBox("����û����Ҫ�ɼ�����վ���߻�û����վ��ȡ������Ϣ��","����");
			return;
		}
		if(m_textpostnum->GetValue()=="" || !m_textpostnum->GetValue().IsNumber())
		{
			wxMessageBox("������ÿ������Ҫ�ɼ�������\n��������������Ҫ̫��Ŷ��","����");
			m_textpostnum->SetValue("");
			m_textpostnum->SetFocus();
			return;
		}
		long caijinum;
		m_textpostnum->GetValue().ToLong(&caijinum);
		if(caijinum <= 0)
		{
			wxMessageBox("�ɼ��������������0����Ҫ̫��Ŷ��","����");
			m_textpostnum->SetValue("");
			m_textpostnum->SetFocus();
			return;
		}
		if(m_checkShowBall->IsChecked()==false)
		{
			wxMessageDialog msgdlg(this,"��ܰ��ʾ���Ƿ���Ҫ�ɼ����ݵ�����ʱ�����ѷ���\n\n"
						"ʹ�ñ����ܿ��������ݲɼ�������ʱ������������ͣ�ɼ�\n\n"
						"���������ʹ�ù������Ĵ���ɼ������ť������������\n\n"
						"����������ϴ�������ɾ���ɼ����Ĳ���Ҫ����Ϣ\n\n"
						"��Ҳ�����ֶ��ڹ���������ɼ����ݵ�����ʱ���Ѹ�ѡ��\n\n"
						"���Ի���ֻ��ûѡ�и�ѡ��ʱ�Զ�������",
						"�ɼ�ǰ����ܰ��ʾ", wxYES_NO | wxCANCEL);
			msgdlg.SetYesNoCancelLabels("���ã�лл", "OK���õ�" , "ȡ�����βɼ�����");
			int dlgret = msgdlg.ShowModal();
			if(dlgret==wxID_NO)
			{
				m_checkShowBall->SetValue(true);
			}
			else if(dlgret == wxID_CANCEL)
			{
				return;
			}
		}
		else
		{
			wxMessageDialog msgdlg(this,"��ܰ��ʾ����Ҫ�ɼ�����վ��["+glCaijiWebName+"]\n\n"
						"��������ÿ������������Ϣ�ɼ�����Ϊ"+m_textpostnum->GetValue()+"\n\n"
						"�����ֶ������˲ɼ����ݵ�����ʱ�����ѷ���\n\n"
						"�����ȷ����Щ��Ϣ��ȷ�Ļ�����ѡ��\"�ǵģ���ʼ�ɼ�\"\n\n"
						"������ѡ��\"����ˣ�������˼\"��ȡ�����βɼ�����\n\n",
						"�ɼ�ǰ����ܰ��ʾ", wxYES_NO);
			msgdlg.SetYesNoLabels("�ǵģ���ʼ�ɼ�", "����ˣ�������˼");
			int dlgret = msgdlg.ShowModal();
			if(dlgret == wxID_NO)
			{
				return;
			}
		}
	}
	if(mythread2 == NULL)
	{
		mythread2 = new MyThread2(this);
		mythread2->Create();
		mythread2->Run();
		wxAuiToolBarItem *item = m_toolbar->FindTool(ID_ITEM_START);
		item->SetLabel("������");
	}
	else
	{
		if(mythread2->IsPaused())
			mythread2->Resume();
		wxAuiToolBarItem *item = m_toolbar->FindTool(ID_ITEM_START);
		item->SetLabel("������");
	}
	m_mgr.Update();
	return;
}

void MyFrame::OnClickPause(wxAuiToolBarEvent &event)
{
	if(curl == NULL)
	{
		//textForLog->AppendText(_("�������ͣ��ϵͳ׼����ͣ\n"));
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�������ͣ��ϵͳ׼����ͣ\n"),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
	else
	{
		//textForLog->AppendText(_("�������ͣ�����ǵ�ǰϵͳ���ڽ���ץ������������Ҫ�ȵ�ǰ������ɻ�ʱ��ʱ�����ͣ��\n"));
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�������ͣ�����ǵ�ǰϵͳ���ڽ���ץ������������Ҫ�ȵ�ǰ������ɻ�ʱ��ʱ�����ͣ��\n"),
						MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
	//textForLog->ShowPosition(textForLog->GetLastPosition()); 
	//textForLog->ScrollLines(-1);
	if(mythread2 != NULL && !mythread2->IsPaused())
	{
		mythread2->Pause();
		wxAuiToolBarItem *item = m_toolbar->FindTool(ID_ITEM_START);
		item->SetLabel("����");
		m_mgr.Update();
	}
}

void MyFrame::OnClickStop(wxAuiToolBarEvent &event)
{
	if(curl == NULL)
	{
		//textForLog->AppendText(_("�����ֹͣ��ϵͳ׼��ֹͣ\n"));
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�����ֹͣ��ϵͳ׼��ֹͣ\n"),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
	else
	{
		//textForLog->AppendText(_("�����ֹͣ�����ǵ�ǰϵͳ���ڽ���ץ������������Ҫ�ȵ�ǰ������ɻ�ʱ��ʱ���ֹͣ��\n"));
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,_("�����ֹͣ�����ǵ�ǰϵͳ���ڽ���ץ������������Ҫ�ȵ�ǰ������ɻ�ʱ��ʱ���ֹͣ��\n"),
					MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	}
	//textForLog->ShowPosition(textForLog->GetLastPosition()); 
	//textForLog->ScrollLines(-1);
	if(mythread2 != NULL)
	{
		if(CUR_CAIJI_VM != NULL)
			zenglApi_Stop(CUR_CAIJI_VM);
		if(!mythread2->IsPaused())
			mythread2->Pause();
		mythread2->Delete();
		mythread2 = NULL;
		wxAuiToolBarItem *item = m_toolbar->FindTool(ID_ITEM_START);
		item->SetLabel("��ʼ");
		/*textForLog->AppendText(wxString::Format("\n����[%s]һ���ɼ���%d����Ϣ�������ɹ�%d����Ϣ//////////////////////////////\n",
								single_mod_catname,single_mod_caijinum,single_mod_curpostnum));*/
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n����[%s]һ���ɼ���%d����Ϣ�������ɹ�%d����Ϣ//////////////////////////////\n",single_mod_catname,single_mod_caijinum,single_mod_curpostnum),
					MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		wxTreeItemId rootid = treeCtrl->GetRootItem();
		wxString webname = treeCtrl->GetItemText(rootid);
		eventforlog.SetEventMsg(wxString::Format("\n��վ[%s]һ���ɼ���%d����Ϣ�������ɹ�%d����Ϣ//////////////////////////////\n",
								webname,global_mod_totalcaijinum,global_mod_totalpostnum),MY_RICHTEXT_NORMAL_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		/*textForLog->AppendText(wxString::Format("\n��վ[%s]һ���ɼ���%d����Ϣ�������ɹ�%d����Ϣ//////////////////////////////\n",
								webname,global_mod_totalcaijinum,global_mod_totalpostnum));*/
		//textForLog->ShowPosition(textForLog->GetLastPosition()); 
		//textForLog->ScrollLines(-1);
		global_mod_processNum = 0;
		global_mod_processPos = 0;
		global_mod_totalpostnum = 0;
		global_mod_totalcaijinum = 0;
		single_mod_processNum = 0;
		single_mod_processPos = 0;
		single_mod_curpostnum = 0;
		single_mod_caijinum = 0;
		single_mod_cat_table = "";
		single_mod_catname = "";
		single_mod_catid = "";
		m_mgr.Update();
	}
}

extern "C"
{
	void wxMessageBox_forZenglRun(char * str)
    {
        wxMessageBox(str,"��ӡ��Ϣ");
    }
}

void MyFrame::OnClickTest(wxAuiToolBarEvent &event)
{
	/*ZENGL_EXPORT_VM_MAIN_ARGS vm_main_args = {global_userdef_compile_info_forZenglRunV2 , 
											  global_userdef_compile_error_forZenglRunV2,
											  global_userdef_run_info_forZenglRunV2,
											  global_userdef_run_print_forZenglRunV2,
											  global_userdef_run_error_forZenglRunV2,
											  global_module_init,
											  ZL_EXP_CP_AF_IN_DEBUG_MODE | 
											  ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO};
	zenglApi_Load("test.zl",&vm_main_args); //����zengl v1.2.0
	return;*/
	/*ZL_EXP_INT builtinID;
	ZL_EXP_VOID * VM;
	char * teststr;
	int testint;
	double testdouble;
	VM = zenglApi_Open();
	zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE | ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO));
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_COMPILE_INFO,global_userdef_compile_info_forZenglRunV2);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_INFO,global_userdef_run_info_forZenglRunV2);
	zenglApi_SetHandle(VM,ZL_EXP_VFLAG_HANDLE_RUN_PRINT,global_userdef_run_print_forZenglRunV2);
	builtinID = zenglApi_SetModInitHandle(VM,"builtin",global_builtin_module_init);
	zenglApi_SetModFunHandle(VM,builtinID,"bltTest",global_builtin_printf);
	if(zenglApi_Run(VM,"test.zl") == -1) //����ִ��zengl�ű�
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n���󣺱���<test.zl>ʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	if((teststr = zenglApi_GetValueAsString(VM,"glmytest")) == ZL_EXP_NULL)
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��ȡ����glmytestʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	if(zenglApi_GetValueAsInt(VM,"i",&testint) == -1)
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��ȡ����iʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	if(zenglApi_GetValueAsDouble(VM,"floatnum",&testdouble) == -1)
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��ȡ����floatnumʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,
				wxString::Format("the value of glmytest in test.zl is %s , i is %d , floatnum is %.16g\n",teststr,testint,testdouble),
				MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
	zenglApi_Reset(VM);
	builtinID = zenglApi_SetModInitHandle(VM,"builtin",global_builtin_module_init);
	//zenglApi_SetModFunHandle(VM,0,"printf",global_builtin_printf);
	if(zenglApi_Run(VM,"test2.zl") == -1) //����ִ��zengl�ű�
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n���󣺱���<test2.zl>ʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	zenglApi_Reset(VM);
	zenglApi_Push(VM,ZL_EXP_FAT_INT,0,1415,0);
	zenglApi_Push(VM,ZL_EXP_FAT_STR,"test second arg",0,0);
	if(zenglApi_Call(VM,"test.zl","init","clsTest") == -1) //����ִ��zengl�ű�����
	{
		wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n���󣺱���ִ��<test fun call>ʧ�ܣ�%s\n",zenglApi_GetErrorString(VM)),
					MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
		zenglApi_Close(VM);
		return;
	}
	zenglApi_Close(VM);
	return;*/


	wxPoint mainWinLoc=this->GetScreenPosition();
	mainWinLoc.x += 180;
	mainWinLoc.y = 200;
	//wxSize mainWinSize=this->GetSize();
	glIsDialogInShow = true;
	MyTestDialog mytestdlg(this,"����������ʽ - ����������Ա�����ã���",mainWinLoc,wxSize(400,200));
	mytestdlg.Create();
	mytestdlg.ShowModal();
	glIsDialogInShow = false;
	/*char *zengl_test[] = {"zenglrun","test.zlc"};
	//for(;;)
	global_my_weburl = "www.zenglong2.qq";
	single_mod_modid = "21";
	single_mod_catid = "4";
	single_mod_catname = "��ҵ����";
	zenglrun_main(2,zengl_test);*/
}

void MyFrame::OnClickResults(wxAuiToolBarEvent &event)
{
	if(glSiteDBPath == "")
	{
		wxMessageBox("����û�д���վ��ȡ������Ϣ��","����");
		return;
	}
	else if(wxFileExists(glSiteDBPath)==false)
	{
		wxMessageBox("�������ݿ�["+glSiteDBPath+"]�ļ������ڣ�","����");
		return;
	}
	else if(single_mod_cat_table == "")
	{
		wxMessageBox("����û��ʼ�ɼ����ݣ�ֻ�п�ʼ�ɼ�ʱ���ſ��Դ���ɼ��ı������ݣ�","����");
		return;
	}
	glIsDialogInShow = true;
	MyResultsDialog myresultsdlg(this,"�ɼ�������ݴ���",wxDefaultPosition,wxSize(400,350));
	myresultsdlg.Create();
	myresultsdlg.Center();
	myresultsdlg.ShowModal();
	glIsDialogInShow = false;
}

void MyFrame::OnClickShowLog(wxAuiToolBarEvent &event)
{
	wxExecute("notepad.exe mylogs.txt");
}

void MyFrame::OnClickWebOffice(wxAuiToolBarEvent &event)
{
	//wxLaunchDefaultBrowser("http://" + gl_initManage_Url);
	//wxLaunchDefaultBrowser("http://www.zengl.com");
	wxExecute("notepad.exe ��ʼ���ű�.zl");
}

void MyFrame::OnResetInitScript(wxAuiToolBarEvent &event)
{
	ZL_EXP_VOID * VM = zenglApi_Open();
	zenglApi_SetFlags(VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE));
	zenglApi_SetModFunHandle(VM,0,"bltSetInitManageUrl",global_bltSetInitManageUrl);
	zenglApi_SetModFunHandle(VM,0,"bltSetModulePath",global_bltSetModulePath);
	if(zenglApi_Run(VM,"��ʼ���ű�.zl") == -1)
		wxMessageBox(wxString::Format("'��ʼ���ű�.zl'ִ��ʧ�ܣ�%s",zenglApi_GetErrorString(VM)),"����");
	else
		wxMessageBox("��ʼ���ű��������","��ӡ��Ϣ");
	zenglApi_Close(VM);
}

void MyFrame::OnMinimize(wxIconizeEvent &event)
{
	//Show(false);
}

MyThread::MyThread(MyFrame *frame)
		:wxThread()
{
	m_frame = frame;
}

void *MyThread::Entry()
{
	wxCommandEvent eventForProgress( wxEVT_MY_PROGRESS , ID_MY_WINDOW);
	eventForProgress.SetEventObject(m_frame->m_statusbar);
	int totaltask = 9,curtask = -1,totalprocess = 100;
	m_frame->m_statusbar->m_total = totalprocess; 
	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //0 task
	//m_frame->m_statusbar->GetEventHandler()->ProcessEvent( eventForProgress );
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	wxString SiteUrlName = m_frame->textUrlName->GetValue().mb_str(wxCSConv(wxT("gb2312")));
	SiteUrlName.Replace("http://","");
	if(SiteUrlName.Last()=='/')
		SiteUrlName.RemoveLast();
	m_frame->textUrlName->SetValue(SiteUrlName);
	wxSQLite3Database *db = wxGetApp().db;
	wxString EscapeSiteName = SiteUrlName;
	EscapeSiteName.Replace("/","_");
	glEscapeSiteName = EscapeSiteName;
	wxString sitedbpath = wxT("db/") + glEscapeSiteName + ".db";
	if(db->IsOpen())
		db->Close();
	if(wxFileExists(sitedbpath))
	{
		wxMessageDialog msgdlg(m_frame,"����վ�ķ�����Ϣ�Ѿ����ڣ������ͬ���������ϵ�������ѡ��ͬ������������\n\n�����ʹ��֮ǰ���ر����������ѡ��ʹ�ñ������ݿ�\n\nע������������ܶ�(�����ʱ)�������Ե�Ƭ�̡�����",
					"������Ϣ��ʾ��", wxYES_NO | wxCANCEL);
		//msgdlg.SetOKCancelLabels("ͬ������������","ʹ�ñ������ݿ�");
		msgdlg.SetYesNoCancelLabels("ʹ�ñ������ݿ�", "ͬ������������" , "ȡ������");
		int dlgret = msgdlg.ShowModal();
		if(dlgret==wxID_YES)
		{
			glmainFrame->m_textForTimeOut->SetValue("60");
			wxCommandEvent eventUpdateCate(wxEVT_MY_UPDATECATE , ID_MY_WINDOW);
			eventUpdateCate.SetEventObject(m_frame);
			m_frame->GetEventHandler()->ProcessEvent( eventUpdateCate );
			//wxQueueEvent(m_frame->GetEventHandler(),eventUpdateCate.Clone());
			return NULL;
		}
		else if(dlgret == wxID_CANCEL)
		{
			glmainFrame->m_textForTimeOut->SetValue("60");
			return NULL;
		}

		wxRemoveFile(sitedbpath);
	}

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //1 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	struct curl_httppost *post=NULL;
	struct curl_httppost *last=NULL;
	long timeout;
	glmainFrame->m_textForTimeOut->GetValue().ToLong(&timeout);
	wxString strUrlName = SiteUrlName + "/mydetectTitle.php";
	curl=curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, strUrlName.c_str().AsChar()); 
	curl_easy_setopt(curl, CURLOPT_USERAGENT ,"Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.1.4322)");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION ,1);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, (int)timeout);//���ó�ʱʱ��
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, mywxcurl_string_write);
	//char * testdata = (char *)SiteUrlName.data();
	//curl_formadd(&post,&last,CURLFORM_COPYNAME,"SiteUrlName",CURLFORM_COPYCONTENTS, SiteUrlName.data().AsChar(), CURLFORM_END);
	//curl_formadd(&post,&last,CURLFORM_COPYNAME,"secret_loginForCaiji",CURLFORM_COPYCONTENTS, "disnglelngsin1988yxj", CURLFORM_END);
	curl_formadd(&post,&last,CURLFORM_COPYNAME,"_zlmy_action",CURLFORM_COPYCONTENTS, "getCatsXml", CURLFORM_END);
	curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

	if((fp=fopen("test.xml","w"))==NULL)
    {
        curl_easy_cleanup(curl);
		return NULL;
    }
	curl_easy_setopt(curl, CURLOPT_WRITEDATA , fp);
    curl_easy_perform(curl);

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //2 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	if(char_myglStr==NULL)
	{
		charlength = 0;
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY," null char content get!(ԭ���������ַ���ˣ���û�ϴ��ɼ��ӿڣ��������ķ������ܶ࣬�����м�������࣬�뽫�������ĳ�ʱʱ����Ϊ1000�����1000�����������㣬�����ԣ�лл��)\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),event.Clone());
		fclose(fp);
		return NULL;
	}
	charlength = strlen(char_myglStr);
	//fwrite( char_myglStr, 1 , char_myglTotalnum ,fp);
	myglStr = "";
	if((myglStr = wxString(char_myglStr)) == "")
		myglStr = wxString::FromUTF8(char_myglStr);
	
	if(myglStr == "" || myglStr=="null")
	{
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY," ��ȡ����ʧ�ܻ�����û�з�����Ϣ������ɼ��ӿ��Ƿ������\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),event.Clone());
		fclose(fp);
		free(char_myglStr);
		char_myglStr = NULL;
		return NULL;
	}
	myglStr.Replace("&","");
	wxScopedCharBuffer myglStrForUTF8 = myglStr.ToUTF8();
	fwrite(myglStrForUTF8.data(), 1 , myglStrForUTF8.length(),fp);
	fclose(fp);
	free(char_myglStr);
	char_myglStr = NULL;
	char_myglTotalnum = 0;
    curl_easy_cleanup(curl);
	wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY);

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //3 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	myglStr = "";
	if(!db->IsOpen())
	{
		db->Open(sitedbpath);
		myglStr += SiteUrlName + "�����ݿ��Ѿ��򿪣�\n";
	}
	//if(!db->TableExists(wxT("website")))
	//{
	db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS website"));
	db->ExecuteUpdate(wxT("CREATE TABLE website (id INTEGER PRIMARY KEY ASC,url,chname,type)"));
	//myglStr += wxT("����website���ݱ�\n");
	eventforlog.SetEventMsg("����website���ݱ�\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	//}
	//if(!db->TableExists(wxT("category")))
	//{
	db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS category"));
	db->ExecuteUpdate(wxT("CREATE TABLE category (id INTEGER PRIMARY KEY ASC,catid,modid,catname)"));
	//myglStr += wxT("����category���ݱ�\n");
	eventforlog.SetEventMsg("����category���ݱ�\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	//}
	//if(!db->TableExists(wxT("keywords")))
	//{
	db->ExecuteUpdate(wxT("DROP TABLE IF EXISTS keywords"));
	db->ExecuteUpdate(wxT("CREATE TABLE keywords (id INTEGER PRIMARY KEY ASC,name,catid,must,needSuffix)"));
	//myglStr += wxT("����keywords���ݱ�\n");
	eventforlog.SetEventMsg("����keywords���ݱ�\n׼������test.xml�ļ����������������Եȡ���",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	//}

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //4 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	wxXmlDocument doc;
	if (!doc.Load("test.xml"))
	{
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,"����test.xml�ļ�ʧ��\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),event.Clone());
		return NULL;
	}
	
	wxXmlNode *root = doc.GetRoot();
	int InitCount = 0;
	wxString sqlstr = "";
	wxSQLite3ResultSet set = db->ExecuteQuery(wxT("SELECT * FROM website where chname = '") + root->GetAttribute("name") + _("'"));
	if(!set.NextRow())
	{
		sqlstr.Printf("INSERT INTO website (url,chname,type) values('%s','%s','')",
									root->GetAttribute("url"),TrimBoth(root->GetAttribute("name")) );
		db->ExecuteUpdate( sqlstr );
		eventforlog.SetEventMsg("��ʼ��website��\n",MY_RICHTEXT_GREEN_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	}
	eventforlog.SetEventMsg("׼����"+root->GetAttribute("url")+"��վ��ȡģ�����ݣ�\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	wxXmlDocument doc2;
	global_my_weburl = root->GetAttribute("url"); //��ȡ��վ��ȫ��URL
	strUrlName = root->GetAttribute("url")+"mydetectTitle.php?_zlmy_get_action=getModules";
	curl=curl_easy_init();
	global_GetUrlContent(strUrlName);
	curl_easy_cleanup(curl);
	curl = NULL;
	if(myglStr == "")
	{
		eventforlog.SetEventMsg("��ȡ"+root->GetAttribute("url")+"��վ��ģ������ʧ�ܣ�\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
		free(char_myglStr);
		char_myglStr = NULL;
		return NULL;
	}
	if((fp=fopen("db/"+ TrimBoth(root->GetAttribute("name"))+".xml","w+"))==NULL)
	{
		eventforlog.SetEventMsg("���ļ�db/"+root->GetAttribute("name")+".xmlʧ�ܣ�\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
		fclose(fp);
		free(char_myglStr);
		char_myglStr = NULL;
		return NULL;
	}
	myglStrForUTF8 = myglStr.ToUTF8();
	fwrite(myglStrForUTF8.data(), 1 , myglStrForUTF8.length(),fp);
	fclose(fp);
	eventforlog.SetEventMsg("��ȡ�����ݣ�׼����ʼ��"+root->GetAttribute("url")+"��վ��ģ������...",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	if (!doc2.Load("db/"+TrimBoth(root->GetAttribute("name"))+".xml"))
	{
		eventforlog.SetEventMsg("����db/"+root->GetAttribute("name")+".xml�ļ�ʧ��\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
		return NULL;
	}
	wxXmlNode * root2 = doc2.GetRoot();
	wxXmlNode *child2 = root2->GetChildren();
	if(global_MyModules.size() > 0)
	{
		global_MyModules.clear();
		glMyModuleNames.clear();
	}
	while(child2)
	{
		global_MyModules[child2->GetAttribute("id")] = child2->GetAttribute("module");
		glMyModuleNames[child2->GetAttribute("id")] = child2->GetAttribute("name");
		child2 = child2->GetNext();
	}
	eventforlog.SetEventMsg("��ʼ����ϣ�\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //5 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	db->ExecuteUpdate(_("DELETE from category"));
	wxXmlNode *child = root->GetChildren();
	db->ExecuteUpdate(_("DELETE from keywords"));
	wxXmlNode *keychild;

	m_frame->m_statusbar->m_pos = ((float)(++curtask)/(float)totaltask) * totalprocess; //6 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());
	
	int pretotalprocess = m_frame->m_statusbar->m_pos;
	totalprocess -= pretotalprocess;
	curtask = 0;
	totaltask = 0;
	wxXmlNode * tmpnode = child;
	while(child)
	{
		child = child->GetNext();
		totaltask++;
	}
	totaltask += 3;
	child = tmpnode;
	eventforlog.SetEventMsg("׼���������͹ؼ�����Ϣ...���Ե�...\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	db->Begin();
	while(child)
	{
		sqlstr.Printf("INSERT INTO category (catid,modid,catname) values('%s','%s','%s')",child->GetAttribute("catid"),child->GetAttribute("modid"),
						child->GetAttribute("name") );
		db->ExecuteUpdate(sqlstr);
		keychild = child->GetChildren();
		while(keychild)
		{
			sqlstr.Printf("INSERT INTO keywords (name,catid,must,needSuffix) values('%s','%s','%s','%s')",keychild->GetAttribute("name"),child->GetAttribute("catid"),keychild->GetAttribute("must"),
						keychild->GetAttribute("needSuffix") );
			db->ExecuteUpdate(sqlstr);
			keychild = keychild->GetNext();
		}
		//eventforlog.SetEventMsg("�������<"+child->GetAttribute("name")+">...",MY_RICHTEXT_GREEN_STYLE);
		//wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
		child = child->GetNext();
		m_frame->m_statusbar->m_pos = pretotalprocess + ((float)(++curtask)/(float)totaltask) * totalprocess; //n task
		wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());
	}
	db->Commit();
	eventforlog.SetEventMsg("����͹ؼ�����Ϣ�������...\n",MY_RICHTEXT_GREEN_STYLE);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());
	//myglStr += "��ʼ��category���keywords��\n";

	m_frame->m_statusbar->m_pos = pretotalprocess + ((float)(++curtask)/(float)totaltask) * totalprocess; //7 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());
	
	myglStyle = MY_RICHTEXT_GREEN_STYLE;
	eventforlog.SetEventMsg("\n��ʼ��category���keywords��\n׼���������ķ����б�����������ܶ࣬�����Ե�...\n",myglStyle);
	wxQueueEvent(m_frame->GetEventHandler(),eventforlog.Clone());

	m_frame->m_statusbar->m_pos = pretotalprocess + ((float)(++curtask)/(float)totaltask) * totalprocess; //8 task
	wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	wxCommandEvent eventUpdateCate(wxEVT_MY_UPDATECATE , ID_MY_WINDOW);
	eventUpdateCate.SetEventObject(m_frame);
	m_frame->GetEventHandler()->ProcessEvent( eventUpdateCate );
	//wxQueueEvent(m_frame->GetEventHandler(),eventUpdateCate.Clone());

	//m_frame->m_statusbar->m_pos = pretotalprocess + ((float)(++curtask)/(float)totaltask) * totalprocess; //9 task
	//wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),eventForProgress.Clone());

	glmainFrame->m_textForTimeOut->SetValue("60");

	return NULL;
}

void MyThread::OnExit()
{
	//wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	m_frame->mythread = NULL;
}

MyThread2::MyThread2(MyFrame * frame)
{
	m_frame = frame;
}

void InitHashMap();

void *MyThread2::Entry()
{
	MyTreeCtrl * treeCtrl = m_frame->treeCtrl;
	wxMenuBar * menubar = m_frame->GetMenuBar();
	wxTreeItemId rootid = treeCtrl->GetRootItem();
	MyTreeItemData * itemdata = (MyTreeItemData *)treeCtrl->GetItemData(rootid);
	wxString webname = treeCtrl->GetItemText(rootid);
	wxString weburl = itemdata->m_url;
	global_my_weburl = weburl;
	myglStyle = MY_RICHTEXT_GREEN_STYLE;
	wxString SiteDBPath = "db/" + glEscapeSiteName + ".db";
	if(!wxFileExists(SiteDBPath))
	{
		myglStyle = MY_RICHTEXT_RED_STYLE;
		myglStr = "[" +webname + "]��վ��Ӧ�ı������ݿ��ļ������ڣ�\n";
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
		wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),event.Clone());
		return NULL;
	}
	myglStr = "��վ���ƣ�" + webname + " url is " + itemdata->m_url + "\n";
	wxMyLogEvent eventForLog( wxEVT_MY_LOG_EVENT,wxID_ANY,myglStr,myglStyle);
	wxCommandEvent eventForClearLogs( wxEVT_MY_CLEARLOGS , ID_MY_WINDOW);
	//eventForLog
	wxQueueEvent(glmainFrame->GetEventHandler(),eventForLog.Clone());
	wxTreeItemIdValue cookie,cookie2;
	wxTreeItemId cateitemid = treeCtrl->GetFirstChild(rootid,cookie), cateitemChild;
	if((bool)m_frame->m_textpostnum->GetValue().IsNumber() != true)
	{
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,"����ʧ�ܣ�ԭ��ÿ������Ĳɼ�����������������",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),event.Clone());
		return NULL;
	}
	int totalcount=0,postnum=0;
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
			if(treeCtrl->GetItemState(cateitemChild) == 1 && itemdata->m_modID != "4")
			{
				totalcount++;
			}
		}while(cateitemChild = treeCtrl->GetNextChild(cateitemChild,cookie2));
	}while(cateitemid = treeCtrl->GetNextChild(cateitemid,cookie));
	if(totalcount <= 0)
	{
		wxMyLogEvent event( wxEVT_MY_LOG_EVENT,wxID_ANY,"��������б�������ѡ��һ��Ҫ�ɼ��ķ���(ע�⣺��˾�Ǻ͹�Ӧ�̳ǵ�ģ��һ��ɼ��ģ����������ֻ��ѡ�˹�˾Ҳ����ʾ�˴���)��\n",MY_RICHTEXT_RED_STYLE);
		wxQueueEvent(m_frame->m_statusbar->GetEventHandler(),event.Clone());
		return NULL;
	}
	wxMyProgressEvent eventForProgress(wxEVT_MY_PROGRESS_EVENT,ID_MY_WINDOW);
	glmainFrame->m_textpostnum->GetValue().ToLong((long *)&postnum);
	global_mod_processNum = totalcount * postnum;
	global_mod_processPos = 0;
	global_mod_totalpostnum = 0;
	global_mod_totalcaijinum = 0;
	single_mod_processNum = postnum;
	single_mod_processPos = 0;
	single_mod_curpostnum = 0;
	single_mod_caijinum = 0;
	eventForProgress.SetEventNum(single_mod_processPos,single_mod_processNum,global_mod_processPos,global_mod_processNum);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventForProgress.Clone());
	cateitemid = treeCtrl->GetFirstChild(rootid,cookie);
	int i = 0,itemstate,iRemain=0;
	InitHashMap();
	wxString ModFilePath;
	if(CUR_CAIJI_VM != ZL_EXP_NULL)
	{
		zenglApi_Close(CUR_CAIJI_VM);
		CUR_CAIJI_VM =ZL_EXP_NULL;
	}
	CUR_CAIJI_VM = zenglApi_Open();
	zenglApi_SetFlags(CUR_CAIJI_VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE | ZL_EXP_CP_AF_OUTPUT_DEBUG_INFO));
	//zenglApi_SetFlags(CUR_CAIJI_VM,(ZENGL_EXPORT_VM_MAIN_ARG_FLAGS)(ZL_EXP_CP_AF_IN_DEBUG_MODE));
	zenglApi_SetHandle(CUR_CAIJI_VM,ZL_EXP_VFLAG_HANDLE_COMPILE_INFO,global_userdef_compile_info_forZenglRunV2);
	zenglApi_SetHandle(CUR_CAIJI_VM,ZL_EXP_VFLAG_HANDLE_RUN_INFO,global_userdef_run_info_forZenglRunV2);
	zenglApi_SetHandle(CUR_CAIJI_VM,ZL_EXP_VFLAG_HANDLE_RUN_PRINT,global_userdef_run_print_forZenglRunV2);
	do
	{
		if(!cateitemid.IsOk())
			break;
		cateitemChild = treeCtrl->GetFirstChild(cateitemid,cookie2);
		do
		{
			if(!cateitemChild.IsOk())
				break;

			if(TestDestroy())
			{
				if(CUR_CAIJI_VM != ZL_EXP_NULL)
				{
					zenglApi_Close(CUR_CAIJI_VM);
					CUR_CAIJI_VM =ZL_EXP_NULL;
				}
				return NULL;
			}
			itemstate = treeCtrl->GetItemState(cateitemChild);
			itemdata = (MyTreeItemData *)treeCtrl->GetItemData(cateitemChild);
			if(itemstate == 1)
			{
				single_mod_processNum = postnum;
				single_mod_processPos = 0;
				single_mod_curpostnum = 0;
				single_mod_caijinum = 0;
				single_mod_modid = itemdata->m_modID;
				if(global_MyModules[itemdata->m_modID] == "article")
				{
					single_mod_cat_table = "article";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					//ModFilePath =  gl_zenglrun_article_modname + "c";
					ModFilePath =  gl_zenglrun_article_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "quote")
				{
					single_mod_cat_table = "quote";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath =  gl_zenglrun_quote_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "special")
				{
					single_mod_cat_table = "special";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_special_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "info")
				{
					single_mod_cat_table = "info";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_info_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "brand")
				{
					single_mod_cat_table = "brand";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_brand_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "mall")
				{
					single_mod_cat_table = "mall";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_mall_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "group")
				{
					single_mod_cat_table = "group";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_group_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "sell")
				{
					single_mod_cat_table = "sell";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_sell_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "buy")
				{
					single_mod_cat_table = "buy";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_buy_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "exhibit")
				{
					single_mod_cat_table = "exhibit";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_exhibit_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "job")
				{
					single_mod_cat_table = "job";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_job_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "know")
				{
					single_mod_cat_table = "know";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_know_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "photo")
				{
					single_mod_cat_table = "photo";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_photo_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "video")
				{
					single_mod_cat_table = "video";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_video_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "down")
				{
					single_mod_cat_table = "down";
					single_mod_catname = treeCtrl->GetItemText(cateitemChild);
					single_mod_catid = itemdata->m_cateID;
					ModFilePath = gl_zenglrun_down_modname;
					zenglApi_SetModInitHandle(CUR_CAIJI_VM,"builtin",global_builtin_module_init);
					if(ModFilePath.Find(".zlencrypt") != wxNOT_FOUND) //��.zlencryptΪ��׺�ģ�Ĭ��Ϊ���ܽű�
					{
						zenglApi_SetSourceRC4Key(CUR_CAIJI_VM,(char *)gl_encrypt_str.c_str().AsChar(),gl_encrypt_str.Length()); //���ܽű�
					}
					else if(menubar->IsChecked(ID_MENU_EXTRA_DEBUG))
					{
						zenglApi_DebugSetBreakHandle(CUR_CAIJI_VM,global_debug_break,global_debug_conditionError,ZL_EXP_TRUE,ZL_EXP_FALSE);
					}
					if(zenglApi_Run(CUR_CAIJI_VM,(char *)ModFilePath.c_str().AsChar()) == -1) //����ִ��zengl�ű�
					{
						wxMyLogEvent eventforlog( wxEVT_MY_LOG_EVENT,wxID_ANY,wxString::Format("\n��������"+ModFilePath+"ʱ�����쳣��%s\n",zenglApi_GetErrorString(CUR_CAIJI_VM)),
									MY_RICHTEXT_RED_STYLE);
						wxQueueEvent(glmainFrame->GetEventHandler(),eventforlog.Clone());
						zenglApi_Close(CUR_CAIJI_VM);
						CUR_CAIJI_VM = ZL_EXP_NULL;
						return NULL;
					}
					zenglApi_Reset(CUR_CAIJI_VM);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
				}
				else if(global_MyModules[itemdata->m_modID] == "company")
				{
					//wxQueueEvent(glmainFrame->GetEventHandler(),eventForClearLogs.Clone());
					continue;
				}
				else
				{
					myglStyle = MY_RICHTEXT_RED_STYLE;
					myglStr = "����["+treeCtrl->GetItemText(cateitemChild) + "]��Ӧ��["+glHashStr[itemdata->m_modID] + "]ģ�鲻�ڲɼ���Χ�ڣ�\n";
					eventForLog.SetEventMsg(myglStr,myglStyle);
					wxQueueEvent(glmainFrame->GetEventHandler(),eventForLog.Clone());
				}
				single_mod_processPos = single_mod_processNum * 2;
				if((iRemain=global_mod_processPos%(single_mod_processNum * 2))!=0)
					global_mod_processPos += (single_mod_processNum * 2 - iRemain);
				eventForProgress.SetEventNum(single_mod_processPos,single_mod_processNum,global_mod_processPos,global_mod_processNum);
				wxQueueEvent(glmainFrame->GetEventHandler(),eventForProgress.Clone());
			}//if
		}while(cateitemChild = treeCtrl->GetNextChild(cateitemChild,cookie2));
	}while(cateitemid = treeCtrl->GetNextChild(cateitemid,cookie));
	if(TestDestroy())
	{
		if(CUR_CAIJI_VM != ZL_EXP_NULL)
		{
			zenglApi_Close(CUR_CAIJI_VM);
			CUR_CAIJI_VM =ZL_EXP_NULL;
		}
		return NULL;
	}
	eventForLog.SetEventMsg(wxString::Format("\n��վ[%s]һ���ɼ���%d����Ϣ�������ɹ�%d����Ϣ//////////////////////////////\n",
		webname,global_mod_totalcaijinum,global_mod_totalpostnum),MY_RICHTEXT_NORMAL_STYLE);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventForLog.Clone());
	eventForProgress.SetEventNum(2,1,2,1);
	wxQueueEvent(glmainFrame->GetEventHandler(),eventForProgress.Clone());
	m_frame->m_taskBarIcon->ShowBalloon("�������ݲɼ���������ϣ�","��ܰ��ʾ!");
	global_mod_processNum = 0;
	global_mod_processPos = 0;
	global_mod_totalpostnum = 0;
	global_mod_totalcaijinum = 0;
	single_mod_processNum = 0;
	single_mod_processPos = 0;
	single_mod_curpostnum = 0;
	single_mod_caijinum = 0;
	single_mod_cat_table = "";
	single_mod_catname = "";
	single_mod_catid = "";
	single_mod_modid = "";
	if(CUR_CAIJI_VM != ZL_EXP_NULL)
	{
		zenglApi_Close(CUR_CAIJI_VM);
		CUR_CAIJI_VM =ZL_EXP_NULL;
	}
	return NULL;
}

void MyThread2::OnExit()
{
	//wxCriticalSectionLocker enter(wxGetApp().m_critsect);
	m_frame->mythread2 = NULL;
	wxAuiToolBarItem *item = glmainFrame->m_toolbar->FindTool(ID_ITEM_START);
	item->SetLabel("��ʼ");
}

void MyTreeCtrl::OnItemStateClick(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	MyTreeItemData *itemdata = (MyTreeItemData *)GetItemData(itemId);
	if(itemdata->m_url != "")
	{
		/*wxCommandEvent event( wxEVT_MY_CHECKS , ID_MY_WINDOW);
		event.SetEventObject( glmainFrame );
		glmainFrame->GetEventHandler()->ProcessEvent( event );*/
		wxTreeItemIdValue cookie,cookie2;
		wxTreeItemId rootid = GetRootItem();
		wxTreeItemId childId = GetFirstChild(rootid,cookie);
		do
		{
			if(childId.IsOk())
			{
				wxTreeItemId childId2 = GetFirstChild(childId,cookie2);
				do
				{
					if(childId2.IsOk())
						SetItemState(childId2,wxTREE_ITEMSTATE_NEXT);
				}while(childId2 = GetNextChild(childId2,cookie2));
			}
		}while(childId = GetNextChild(childId,cookie));
	}
	else if(itemdata->m_cateID == "0")
	{
		wxTreeItemIdValue cookie;
		wxTreeItemId childId = GetFirstChild(itemId,cookie);
		do{
			if(childId.IsOk())
				SetItemState(childId,wxTREE_ITEMSTATE_NEXT);
		}while(childId = GetNextChild(childId,cookie));
	}
	else
		SetItemState(itemId, wxTREE_ITEMSTATE_NEXT);
}

void MyTreeCtrl::OnItemActivated(wxTreeEvent& event)
{
	wxTreeItemId itemId = event.GetItem();
	MyTreeItemData *itemdata = (MyTreeItemData *)GetItemData(itemId);
	if(itemdata->m_url == "" && itemdata->m_cateID != "0")
	{
		wxPoint mainWinLoc=this->GetScreenPosition();
		mainWinLoc.x += 200;
		mainWinLoc.y += 20;
		wxSize mainWinSize=this->GetSize();
		wxString title = GetItemText(itemId);
		glIsDialogInShow = true;
		MyKeyDialog keydialog(glmainFrame,title,mainWinLoc,mainWinSize,itemdata->m_cateID,itemdata->m_modID,itemdata->m_catname);
		keydialog.Create();
		keydialog.ShowModal();
		glIsDialogInShow = false;
	}
}

void MyTaskBarIcon::OnMenuRestore(wxCommandEvent& )
{
	//if(!glmainFrame->IsTopLevel())
	glmainFrame->Raise();  // bring window to front
	glmainFrame->Show(true); // show the window
	if(glmainFrame->IsIconized())
		glmainFrame->Iconize(false);
}

void MyTaskBarIcon::OnHideToTray(wxCommandEvent& event)
{
	glmainFrame->Show(false); // show the window
	//glmainFrame->Iconize(false);
}

void MyTaskBarIcon::OnMenuExit(wxCommandEvent& )
{
	if(glIsDialogInShow == true)
	{
		wxMessageBox("���жԻ���û�йرգ����ȹر����е����ĶԻ������˳���","����");
		return;
	}
	glmainFrame->Close(true);
}

void MyTaskBarIcon::OnLeftButtonClick(wxTaskBarIconEvent&)
{
	//glmainFrame->Iconize(false); // restore the window if minimized
	//glmainFrame->SetFocus();  // focus on my window
	if(!glmainFrame->IsShown())
	{
		//if(!glmainFrame->IsTopLevel())
		glmainFrame->Raise();  // bring window to front
		glmainFrame->Show(true); // show the window
		if(glmainFrame->IsIconized())
			glmainFrame->Iconize(false);
	}
	else
	{
		glmainFrame->Show(false); // show the window
		//glmainFrame->Iconize(false);
	}
}

// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
    wxMenu *menu = new wxMenu;
    menu->Append(PU_RESTORE, wxT("�ָ�������"));
	menu->Append(PU_HIDE, wxT("����������"));
    menu->AppendSeparator();
    menu->Append(PU_EXIT,    wxT("�˳�"));
    return menu;
}

bool MyTaskBarIcon::ShowBalloon(wxString title, wxString msg, int iconID, unsigned int timeout)
{
	if (!IsOk()) {
        return false;
    }

	NOTIFYICONDATA notifyData;
	memset(&notifyData, 0, sizeof(notifyData));
	notifyData.cbSize = NOTIFYICONDATA_V2_SIZE;//sizeof(notifyData);
	notifyData.hWnd = (HWND)m_win->GetHWND();
	notifyData.uCallbackMessage = ::RegisterWindowMessage(wxT(
        "wxTaskBarIconMessage"));
	notifyData.uFlags = NIF_MESSAGE;

	notifyData.uFlags |= NIF_INFO;
    lstrcpyn(notifyData.szInfo, msg.c_str(), sizeof(notifyData.szInfo));
    lstrcpyn(notifyData.szInfoTitle, title.c_str(), sizeof
        (notifyData.szInfoTitle));
    notifyData.dwInfoFlags = iconID; // | NIIF_NOSOUND; modified by palinx
    notifyData.uTimeout = timeout;

    notifyData.uID = 99;

	if (m_iconAdded) {
        return (Shell_NotifyIcon(NIM_MODIFY, &notifyData) != 0);
    } else {
        return false;
    }
}

MyUserExtraData::MyUserExtraData()
{
	this->isQuerySql = 0;
	this->curl = NULL;
}

MyUserExtraData::~MyUserExtraData()
{
	//if(this->isQuerySql)
		this->sqlset.~wxSQLite3ResultSet();
	if(this->db.IsOpen())
		this->db.Close();
	if(this->curl != NULL)
		curl_easy_cleanup(this->curl);
}
