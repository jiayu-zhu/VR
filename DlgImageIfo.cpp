// DlgImageIfo.cpp : implementation file
//

#include "stdafx.h"
#include "DCMOpen.h"
#include "DlgImageIfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgImageIfo dialog


CDlgImageIfo::CDlgImageIfo(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgImageIfo::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgImageIfo)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgImageIfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgImageIfo)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgImageIfo, CDialog)
	//{{AFX_MSG_MAP(CDlgImageIfo)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgImageIfo message handlers

void CDlgImageIfo::OnPaint() 
{//DICOM信息的显示
	CPaintDC dc(this); // device context for painting

		// 字符串
		CString str;
		
		// 获取绘制坐标的静态框
		CWnd* pWnd = GetDlgItem(IDC_LIST1);
		
		// 指针，这个就是初始化显示的指针，可以不用管
		CDC* pDC = pWnd->GetDC();
		pWnd->Invalidate();
		pWnd->UpdateWindow();
		
		CPen* pPenBlue = new CPen;
		
		// 蓝色画笔
		pPenBlue->CreatePen(PS_SOLID,3,RGB(0,0,255));//PS_SOLID画笔的性质，即实线，3是画笔的粗细，RGB(0,0,255)是其颜色
		CGdiObject* pOldPen = pDC->SelectObject(pPenBlue);//选择蓝色画笔
		// 绘制矩形区域
		pDC->Rectangle(0,0,730,650);//函数里面参数用来定义矩形区域的大小和位置

		// 灰色画笔,绘制网格
		CPen* pPenWire = new CPen;
		pPenWire->CreatePen(PS_SOLID,1,RGB(200,200, 200));
		pDC->SelectObject(pPenWire);

		for(int j=0;j<31;j++)
		{
			pDC->MoveTo(0,28+20*j);//将点移至某坐标点
			pDC->LineTo(730,28+20*j);//画线，通过MoveTo（）函数和LineTo（）函数实现画线功能
		}
		pDC->MoveTo(120,0);
		pDC->LineTo(120,650);
		pDC->MoveTo(335,0);
		pDC->LineTo(335,650);
		pDC->MoveTo(385,0);
		pDC->LineTo(385,650);
		pDC->MoveTo(435,0);
		pDC->LineTo(435,650);
		// 标题
		pDC->SetTextColor(RGB(255,0,0));//显示标题
		pDC->TextOut(5, 5, "TAG");//TextOut（）函数显示文字，5，5是其位置，"TAG"是其文字
		pDC->TextOut(128, 5, "TAG NAME");
		pDC->TextOut(340, 5, "VR");
		pDC->TextOut(390, 5, "VL");
		pDC->TextOut(440, 5, "VF");
		
		// 数据
		pDC->SetTextColor(RGB(0,0,0));
		for(int i=0;i<30;i++)
		{
			int gt=m_Dataset[i].group_tag;
			int et=m_Dataset[i].element_tag;
			str.Format("(0x%d%d%d%d,0x%d%d%d%d)",gt/(16*16*16),gt%(16*16*16)/(16*16),gt%(16*16)/16,gt%16,\
				et/(16*16*16),et%(16*16*16)/(16*16),et%(16*16)/16,et%16);//显示文字，通过str.Format（）和TextOut（）实现文字的显示，显示出str中存储的文字，str。Format（）是用来存储文字
			pDC->TextOut(5, 20*i+30, str);
			pDC->TextOut(128, 20*i+30, m_TagName[i]);
			str.Format("%d",m_Dataset[i].length);
			pDC->TextOut(390, 20*i+30,str);
			pDC->TextOut(440, 20*i+30, m_Dataset[i].data);
		}
		pDC->SelectObject(pOldPen);	//这个可以不用管
		
		delete pPenWire;
		delete pPenBlue;		

}

