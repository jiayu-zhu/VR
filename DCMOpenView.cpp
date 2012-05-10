// DCMOpenView.cpp : implementation of the CDCMOpenView class
//

#include "stdafx.h"
#include "DCMOpen.h"

#include "DCMOpenDoc.h"
#include "DCMOpenView.h"
#include "DlgImageIfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView

IMPLEMENT_DYNCREATE(CDCMOpenView, CView)

BEGIN_MESSAGE_MAP(CDCMOpenView, CView)
	//{{AFX_MSG_MAP(CDCMOpenView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_COMMAND(ID_DEFAULT, OnDefault)
	ON_COMMAND(ID_DICOMINFO, OnDicominfo)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_COMMAND(ID_FUCTION_32775, &CDCMOpenView::OnFuctionFangda)
	ON_COMMAND(ID_FUCTION_32776, &CDCMOpenView::OnFuctionSuoXiao)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView construction/destruction

CDCMOpenView::CDCMOpenView()
{//对参数的初始化
	// TODO: add construction code here
	m_LPoint.x=0;
	m_LPoint.y=0;
	m_MPoint.x=0;
	m_MPoint.y=0;
	isLBDown=FALSE;
	m_zoom = 1.0;
	m_angel = 0.0;
}

CDCMOpenView::~CDCMOpenView()
{
	
}

BOOL CDCMOpenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView drawing

void CDCMOpenView::OnDraw(CDC* pDC)
{//将DICOM图像显示出来
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	pDoc->m_DCM.DCMDraw(pDC);

	if(pDoc->m_DCM.p_DCM!=NULL)
	{//在图像的右侧，将一些信息显示出来，对照软件图像右侧，就能看到这些信息

		CString str;
		char *name = new char [128];
		memcpy(name,pDoc->m_DCM.CRDataset[19].data,128);//将CDCMOpenDoc中的m_DCM中的第19个表中的数据拷贝至name
		name[2]=' ';
		pDC->TextOut(600,10,name);//TextOut（）函数就是用来显示文字的
		pDC->TextOut(600,30,"PID:");
		pDC->TextOut(650,30,pDoc->m_DCM.CRDataset[20].data);
		pDC->TextOut(600,50,"SEX:");
		pDC->TextOut(650,50,pDoc->m_DCM.CRDataset[21].data);
		pDC->TextOut(600,70,pDoc->m_DCM.CRDataset[2].data);
		pDC->TextOut(600,110,pDoc->m_DCM.CRDataset[14].data);
		pDC->TextOut(600,130,pDoc->m_DCM.CRDataset[15].data);
		int wc=pDoc->m_DCM.WC;
		int ww=pDoc->m_DCM.WW;

		str.Format("Window Center:  %d ",wc);
		pDC->TextOut(600,200,str);
		str.Format("Window Width :  %d ",ww);
		pDC->TextOut(600,220,str);

	}

}

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView printing

BOOL CDCMOpenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDCMOpenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDCMOpenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView diagnostics

#ifdef _DEBUG
void CDCMOpenView::AssertValid() const
{
	CView::AssertValid();
}

void CDCMOpenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDCMOpenDoc* CDCMOpenView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDCMOpenDoc)));
	return (CDCMOpenDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CDCMOpenView message handlers

void CDCMOpenView::OnLButtonDown(UINT nFlags, CPoint point) 
{//当鼠标左键按下后，isLBDown参数就会发生变化，这将导致OnMouseMove中的代码起作用
	// TODO: Add your message handler code here and/or call default
	isLBDown=TRUE;
	m_LPoint=point;
	m_prePoint=point;
	CView::OnLButtonDown(nFlags, point);
}

void CDCMOpenView::OnLButtonUp(UINT nFlags, CPoint point) 
{//当鼠标左键抬起后，isLBDown参数就会发生变化，这将导致OnMouseMove中的代码不起作用
	// TODO: Add your message handler code here and/or call default
	isLBDown=FALSE;
	CView::OnLButtonUp(nFlags, point);
}

void CDCMOpenView::OnMouseMove(UINT nFlags, CPoint point) 
{//当鼠标滑动时候，窗宽窗位会发生变化，point是鼠标在屏幕中的位置，这个值将会导致，窗宽窗位的值发生变化
	// TODO: Add your message handler code here and/or call default
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if(pDoc->m_DCM.p_DCM==NULL)
		return ;
	m_MPoint=point;	
	if(isLBDown)//只有当左键按下之后，以下代码才会发生改变，isLBDown值在OnLButtonDown（）函数和OnLButtonUp（）函数中会产生变化
	{
// 		pDoc->m_DCM.WW+=(point.x-m_prePoint.x)*5;//计算窗宽
// 		pDoc->m_DCM.WC+=(point.y-m_prePoint.y)*5;//计算窗位
// 		if(pDoc->m_DCM.WW>4311)
// 			pDoc->m_DCM.WW=4311;//当窗宽大于4311，就直接赋值4311
// 		if(pDoc->m_DCM.WW<1)
// 			pDoc->m_DCM.WW=1;//同上
// 		if(pDoc->m_DCM.WC>2119)
// 			pDoc->m_DCM.WC=2119;//同上
// 		if(pDoc->m_DCM.WC<15)
// 			pDoc->m_DCM.WC=15;//同上
		m_angel += (point.x - m_prePoint.x)/5;
		m_prePoint=point;

		int wc=pDoc->m_DCM.WC;
		int ww=pDoc->m_DCM.WW;

		delete[] pDoc->m_DCM.p_data1;
		pDoc->m_DCM.p_data1 = new BYTE[pDoc->m_DCM.m_srcWidth*pDoc->m_DCM.m_srcHeight];
		pDoc->m_DCM.CreatBMPDate(pDoc->m_DCM.p_data1 , wc ,ww );//每当窗宽窗位发生改变就吊用CreatBMPDate将图像值改变，进而改变图像的现实
		::initTransPixelBuffer(pDoc->m_DCM.m_srcWidth,pDoc->m_DCM.m_srcHeight);
		::GetTrans((unsigned char*)pDoc->m_DCM.p_data1,pDoc->m_DCM.p_data1,pDoc->m_DCM.m_srcWidth,pDoc->m_DCM.m_srcHeight,m_angel);
		pDoc->m_DCM.m_Width = pDoc->m_DCM.m_srcWidth;
		pDoc->m_DCM.m_Height = pDoc->m_DCM.m_srcHeight;
		//pDoc->m_DCM.TransFer(m_angel);
		Invalidate(TRUE);
		

	}
//	Invalidate(TRUE);
	CView::OnMouseMove(nFlags, point);
}

void CDCMOpenView::OnDefault() 
{//这个函数是将窗宽窗位复原，并复原图像值，在按下软件菜单栏中“default”的时候
	// TODO: Add your command handler code here
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	pDoc->m_DCM.WC=pDoc->m_DCM.m_WinCenter;
	pDoc->m_DCM.WW=pDoc->m_DCM.m_WinWidth;
	int wc=pDoc->m_DCM.WC;
	int ww=pDoc->m_DCM.WW;
	
	pDoc->m_DCM.CreatBMPDate(pDoc->m_DCM.p_data1 , wc ,ww );
	Invalidate(TRUE);
	
}

void CDCMOpenView::OnDicominfo() 
{
	// TODO: Add your command handler code here
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CDlgImageIfo*  dlg;  	 
	dlg=new CDlgImageIfo(this);
	dlg->Create(IDD_DLG_IMAGEINFO);//创建显示dicom数据信息的框架，就是在按下软件菜单栏中“图像信息”的时候
	for(int i=0;i<30;i++)
	{
		dlg->m_Dataset[i]=pDoc->m_DCM.CRDataset[i];
		dlg->m_TagName[i]=pDoc->m_DCM.TagName[i];
	}

	dlg->ShowWindow(SW_RESTORE);//吊用框架自带的函数，显示，数据信息，在此同时，会吊用CDlgImageIfo中的OnPaint（）函数
}























void CDCMOpenView::OnFuctionFangda()
{
	// TODO: 在此添加命令处理程序代码
	m_zoom = m_zoom*2.0;
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int wc=pDoc->m_DCM.WC;
	int ww=pDoc->m_DCM.WW;

	delete[] pDoc->m_DCM.p_data1;
	pDoc->m_DCM.p_data1 = new BYTE[pDoc->m_DCM.m_srcWidth*pDoc->m_DCM.m_srcHeight];
	pDoc->m_DCM.CreatBMPDate(pDoc->m_DCM.p_data1 , wc ,ww );
	pDoc->m_DCM.m_Width = pDoc->m_DCM.m_srcWidth;
	pDoc->m_DCM.m_Height = pDoc->m_DCM.m_srcHeight;
	pDoc->m_DCM.Scale(m_zoom,0,0);
	Invalidate(TRUE);
}


void CDCMOpenView::OnFuctionSuoXiao()
{
	// TODO: 在此添加命令处理程序代码
	m_zoom = m_zoom/2.0;
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	int wc=pDoc->m_DCM.WC;
	int ww=pDoc->m_DCM.WW;

	delete[] pDoc->m_DCM.p_data1;
	pDoc->m_DCM.p_data1 = new BYTE[pDoc->m_DCM.m_srcWidth*pDoc->m_DCM.m_srcHeight];
	pDoc->m_DCM.CreatBMPDate(pDoc->m_DCM.p_data1 , wc ,ww );
	pDoc->m_DCM.m_Width = pDoc->m_DCM.m_srcWidth;
	pDoc->m_DCM.m_Height = pDoc->m_DCM.m_srcHeight;
	pDoc->m_DCM.Scale(m_zoom,0,0);
	Invalidate(TRUE);
}
