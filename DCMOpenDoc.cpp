// DCMOpenDoc.cpp : implementation of the CDCMOpenDoc class
//

#include "stdafx.h"
#include "DCMOpen.h"

#include "DCMOpenDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenDoc

IMPLEMENT_DYNCREATE(CDCMOpenDoc, CDocument)

BEGIN_MESSAGE_MAP(CDCMOpenDoc, CDocument)
	//{{AFX_MSG_MAP(CDCMOpenDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenDoc construction/destruction

CDCMOpenDoc::CDCMOpenDoc()
{
	// TODO: add one-time construction code here

}

CDCMOpenDoc::~CDCMOpenDoc()
{
}

BOOL CDCMOpenDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CDCMOpenDoc serialization

void CDCMOpenDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenDoc diagnostics

#ifdef _DEBUG
void CDCMOpenDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDCMOpenDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenDoc commands

BOOL CDCMOpenDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	// TODO: Add your specialized creation code here
	::InitOpenCLContext();
	m_DCM.OpenDCMFlie(lpszPathName);//吊用CDCM中的文件打开函数
	return TRUE;
}
