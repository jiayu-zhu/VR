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
{//DICOM��Ϣ����ʾ
	CPaintDC dc(this); // device context for painting

		// �ַ���
		CString str;
		
		// ��ȡ��������ľ�̬��
		CWnd* pWnd = GetDlgItem(IDC_LIST1);
		
		// ָ�룬������ǳ�ʼ����ʾ��ָ�룬���Բ��ù�
		CDC* pDC = pWnd->GetDC();
		pWnd->Invalidate();
		pWnd->UpdateWindow();
		
		CPen* pPenBlue = new CPen;
		
		// ��ɫ����
		pPenBlue->CreatePen(PS_SOLID,3,RGB(0,0,255));//PS_SOLID���ʵ����ʣ���ʵ�ߣ�3�ǻ��ʵĴ�ϸ��RGB(0,0,255)������ɫ
		CGdiObject* pOldPen = pDC->SelectObject(pPenBlue);//ѡ����ɫ����
		// ���ƾ�������
		pDC->Rectangle(0,0,730,650);//��������������������������Ĵ�С��λ��

		// ��ɫ����,��������
		CPen* pPenWire = new CPen;
		pPenWire->CreatePen(PS_SOLID,1,RGB(200,200, 200));
		pDC->SelectObject(pPenWire);

		for(int j=0;j<31;j++)
		{
			pDC->MoveTo(0,28+20*j);//��������ĳ�����
			pDC->LineTo(730,28+20*j);//���ߣ�ͨ��MoveTo����������LineTo��������ʵ�ֻ��߹���
		}
		pDC->MoveTo(120,0);
		pDC->LineTo(120,650);
		pDC->MoveTo(335,0);
		pDC->LineTo(335,650);
		pDC->MoveTo(385,0);
		pDC->LineTo(385,650);
		pDC->MoveTo(435,0);
		pDC->LineTo(435,650);
		// ����
		pDC->SetTextColor(RGB(255,0,0));//��ʾ����
		pDC->TextOut(5, 5, "TAG");//TextOut����������ʾ���֣�5��5����λ�ã�"TAG"��������
		pDC->TextOut(128, 5, "TAG NAME");
		pDC->TextOut(340, 5, "VR");
		pDC->TextOut(390, 5, "VL");
		pDC->TextOut(440, 5, "VF");
		
		// ����
		pDC->SetTextColor(RGB(0,0,0));
		for(int i=0;i<30;i++)
		{
			int gt=m_Dataset[i].group_tag;
			int et=m_Dataset[i].element_tag;
			str.Format("(0x%d%d%d%d,0x%d%d%d%d)",gt/(16*16*16),gt%(16*16*16)/(16*16),gt%(16*16)/16,gt%16,\
				et/(16*16*16),et%(16*16*16)/(16*16),et%(16*16)/16,et%16);//��ʾ���֣�ͨ��str.Format������TextOut����ʵ�����ֵ���ʾ����ʾ��str�д洢�����֣�str��Format�����������洢����
			pDC->TextOut(5, 20*i+30, str);
			pDC->TextOut(128, 20*i+30, m_TagName[i]);
			str.Format("%d",m_Dataset[i].length);
			pDC->TextOut(390, 20*i+30,str);
			pDC->TextOut(440, 20*i+30, m_Dataset[i].data);
		}
		pDC->SelectObject(pOldPen);	//������Բ��ù�
		
		delete pPenWire;
		delete pPenBlue;		

}

