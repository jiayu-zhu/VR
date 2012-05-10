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
{//�Բ����ĳ�ʼ��
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
{//��DICOMͼ����ʾ����
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
	pDoc->m_DCM.DCMDraw(pDC);

	if(pDoc->m_DCM.p_DCM!=NULL)
	{//��ͼ����Ҳ࣬��һЩ��Ϣ��ʾ�������������ͼ���Ҳ࣬���ܿ�����Щ��Ϣ

		CString str;
		char *name = new char [128];
		memcpy(name,pDoc->m_DCM.CRDataset[19].data,128);//��CDCMOpenDoc�е�m_DCM�еĵ�19�����е����ݿ�����name
		name[2]=' ';
		pDC->TextOut(600,10,name);//TextOut������������������ʾ���ֵ�
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
{//�����������º�isLBDown�����ͻᷢ���仯���⽫����OnMouseMove�еĴ���������
	// TODO: Add your message handler code here and/or call default
	isLBDown=TRUE;
	m_LPoint=point;
	m_prePoint=point;
	CView::OnLButtonDown(nFlags, point);
}

void CDCMOpenView::OnLButtonUp(UINT nFlags, CPoint point) 
{//��������̧���isLBDown�����ͻᷢ���仯���⽫����OnMouseMove�еĴ��벻������
	// TODO: Add your message handler code here and/or call default
	isLBDown=FALSE;
	CView::OnLButtonUp(nFlags, point);
}

void CDCMOpenView::OnMouseMove(UINT nFlags, CPoint point) 
{//����껬��ʱ�򣬴���λ�ᷢ���仯��point���������Ļ�е�λ�ã����ֵ���ᵼ�£�����λ��ֵ�����仯
	// TODO: Add your message handler code here and/or call default
	CDCMOpenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	
	if(pDoc->m_DCM.p_DCM==NULL)
		return ;
	m_MPoint=point;	
	if(isLBDown)//ֻ�е��������֮�����´���Żᷢ���ı䣬isLBDownֵ��OnLButtonDown����������OnLButtonUp���������л�����仯
	{
// 		pDoc->m_DCM.WW+=(point.x-m_prePoint.x)*5;//���㴰��
// 		pDoc->m_DCM.WC+=(point.y-m_prePoint.y)*5;//���㴰λ
// 		if(pDoc->m_DCM.WW>4311)
// 			pDoc->m_DCM.WW=4311;//���������4311����ֱ�Ӹ�ֵ4311
// 		if(pDoc->m_DCM.WW<1)
// 			pDoc->m_DCM.WW=1;//ͬ��
// 		if(pDoc->m_DCM.WC>2119)
// 			pDoc->m_DCM.WC=2119;//ͬ��
// 		if(pDoc->m_DCM.WC<15)
// 			pDoc->m_DCM.WC=15;//ͬ��
		m_angel += (point.x - m_prePoint.x)/5;
		m_prePoint=point;

		int wc=pDoc->m_DCM.WC;
		int ww=pDoc->m_DCM.WW;

		delete[] pDoc->m_DCM.p_data1;
		pDoc->m_DCM.p_data1 = new BYTE[pDoc->m_DCM.m_srcWidth*pDoc->m_DCM.m_srcHeight];
		pDoc->m_DCM.CreatBMPDate(pDoc->m_DCM.p_data1 , wc ,ww );//ÿ������λ�����ı�͵���CreatBMPDate��ͼ��ֵ�ı䣬�����ı�ͼ�����ʵ
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
{//��������ǽ�����λ��ԭ������ԭͼ��ֵ���ڰ�������˵����С�default����ʱ��
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
	dlg->Create(IDD_DLG_IMAGEINFO);//������ʾdicom������Ϣ�Ŀ�ܣ������ڰ�������˵����С�ͼ����Ϣ����ʱ��
	for(int i=0;i<30;i++)
	{
		dlg->m_Dataset[i]=pDoc->m_DCM.CRDataset[i];
		dlg->m_TagName[i]=pDoc->m_DCM.TagName[i];
	}

	dlg->ShowWindow(SW_RESTORE);//���ÿ���Դ��ĺ�������ʾ��������Ϣ���ڴ�ͬʱ�������CDlgImageIfo�е�OnPaint��������
}























void CDCMOpenView::OnFuctionFangda()
{
	// TODO: �ڴ���������������
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
	// TODO: �ڴ���������������
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
