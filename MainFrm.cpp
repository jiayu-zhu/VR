// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DCMOpen.h"

#include "MainFrm.h"
#include "DCMOpenDoc.h"
#include "DCMOpenView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CLOCK,
	IDS_INDICATOR_MOUSEPOSITION,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	m_MousePos.x=0;
	m_MousePos.y=0;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	SetTimer(1,50,NULL);
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent==1)
	{
		CTime t;
		t=CTime::GetCurrentTime();//得到当前时间
		CString str=t.Format("%Y-%m-%d %H:%M:%S");
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(ID_INDICATOR_CLOCK),str);//将时间显示到状态栏中

		//((CDCMOpenApp*)::AfxGetApp())->pDocTemplate;
		POSITION posD = ((CDCMOpenApp*)::AfxGetApp())->pDocTemplate->GetFirstDocPosition();
		CDCMOpenDoc* pDoc = (CDCMOpenDoc*)(((CDCMOpenApp*)::AfxGetApp())->pDocTemplate->GetNextDoc(posD));//得到doc指针，以得到doc，对当前所用的doc进行操作
		POSITION posV = pDoc->GetFirstViewPosition();
		CDCMOpenView* pView = (CDCMOpenView*)(pDoc->GetNextView(posV));//得到view指针，以对当前所用的显示进行操作

		str.Format("[%d,%d]",pView->m_MPoint.x,pView->m_MPoint.y);
		m_wndStatusBar.SetPaneText(m_wndStatusBar.CommandToIndex(IDS_INDICATOR_MOUSEPOSITION),str);//将当前鼠标位置信息显示到状态栏
	}
//	CString str;

	CFrameWnd::OnTimer(nIDEvent);//用来定时吊用此函数，实现双线程
}






















