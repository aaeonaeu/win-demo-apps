
// PWMDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PWM.h"
#include "PWMDlg.h"
#include "afxdialogex.h"

#include "aaeonEAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CPWMDlg dialog



CPWMDlg::CPWMDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_PWM_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPWMDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPWMDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BTN_SET, &CPWMDlg::OnBnClickedBtnSet)
END_MESSAGE_MAP()


// CPWMDlg message handlers

BOOL CPWMDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//Step1: EApiLibInitialize
	err = EApiLibInitialize();
	if (err != EAPI_STATUS_INITIALIZED)
	{
		AfxMessageBox(_T("Can't initialize the application: "));
		return FALSE;
	}

	CString str;
	for (int i = 0; i < 3; i++)
	{
		str.Format(_T("PWM %d"), i);
		((CComboBox*)GetDlgItem(IDC_CMB_DEV_SELECTOR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_CMB_DEV_SELECTOR))->SetCurSel(0);

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT))->SetRange(0, 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT))->SetPageSize(1);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT))->SetPos(0);

	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DUTYCYCLE))->SetRange(0, 0xFF);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DUTYCYCLE))->SetPageSize(1);
	((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DUTYCYCLE))->SetPos(0x80);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPWMDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPWMDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPWMDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CPWMDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CString str;

	CScrollBar* scrollBaseUnitInt = (CScrollBar*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT);
	CScrollBar* scrollDutyCycle = (CScrollBar*)GetDlgItem(IDC_SLIDER_DUTYCYCLE);

	if (pScrollBar == scrollBaseUnitInt)
	{
		str.Format(_T("%X"), ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT))->GetPos());
		SetDlgItemText(IDC_EDIT_BASEUNIT_INT, str);
	}
	else if (pScrollBar == scrollDutyCycle)
	{
		str.Format(_T("%X"), ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DUTYCYCLE))->GetPos());
		SetDlgItemText(IDC_EDIT_DUTYCYCLE, str);
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPWMDlg::OnBnClickedBtnSet()
{
	EApiStatus_t ret = EAPI_STATUS_ERROR;

	int nDevice = ((CComboBox*)GetDlgItem(IDC_CMB_DEV_SELECTOR))->GetCurSel();
	int nBaseUnitInt = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_BASEUNIT_INT))->GetPos();
	int nDutyCycle = ((CSliderCtrl*)GetDlgItem(IDC_SLIDER_DUTYCYCLE))->GetPos();

	ret = EApiPwmSetValue(nDevice, nBaseUnitInt, 0, nDutyCycle);
	if (ret != EAPI_STATUS_SUCCESS)
	{
		AfxMessageBox(_T("Set PWM Failed"));
	}
}
