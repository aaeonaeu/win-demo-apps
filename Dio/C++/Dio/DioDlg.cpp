// DioDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Dio.h"
#include "DioDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CDioDlg dialog




CDioDlg::CDioDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDioDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CDioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
	DDX_Control(pDX, IDC_CMB_IO0, m_CMB_IO0);
	DDX_Control(pDX, IDC_CMB_IO1, m_CMB_IO1);
	DDX_Control(pDX, IDC_CMB_IO2, m_CMB_IO2);
	DDX_Control(pDX, IDC_CMB_IO3, m_CMB_IO3);
	DDX_Control(pDX, IDC_CMB_IO4, m_CMB_IO4);
	DDX_Control(pDX, IDC_CMB_IO5, m_CMB_IO5);
	DDX_Control(pDX, IDC_CMB_IO6, m_CMB_IO6);
	DDX_Control(pDX, IDC_CMB_IO7, m_CMB_IO7);
	DDX_Control(pDX, IDC_CMB_HL0, m_CMB_HL0);
	DDX_Control(pDX, IDC_CMB_HL1, m_CMB_HL1);
	DDX_Control(pDX, IDC_CMB_HL2, m_CMB_HL2);
	DDX_Control(pDX, IDC_CMB_HL3, m_CMB_HL3);
	DDX_Control(pDX, IDC_CMB_HL4, m_CMB_HL4);
	DDX_Control(pDX, IDC_CMB_HL5, m_CMB_HL5);
	DDX_Control(pDX, IDC_CMB_HL6, m_CMB_HL6);
	DDX_Control(pDX, IDC_CMB_HL7, m_CMB_HL7);
}

BEGIN_MESSAGE_MAP(CDioDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_SET, &CDioDlg::OnBnClickedBtnSet)

	ON_CBN_SELCHANGE(IDC_CMB_GROUP, &CDioDlg::OnCbnSelchangeCmbGroup)
END_MESSAGE_MAP()


// CDioDlg message handlers

BOOL CDioDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
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
	m_CMB_IO0.AddString(L"Input");
	m_CMB_IO0.AddString(L"Output");
	m_CMB_IO1.AddString(L"Input");
	m_CMB_IO1.AddString(L"Output");
	m_CMB_IO2.AddString(L"Input");
	m_CMB_IO2.AddString(L"Output");
	m_CMB_IO3.AddString(L"Input");
	m_CMB_IO3.AddString(L"Output");
	m_CMB_IO4.AddString(L"Input");
	m_CMB_IO4.AddString(L"Output");
	m_CMB_IO5.AddString(L"Input");
	m_CMB_IO5.AddString(L"Output");
	m_CMB_IO6.AddString(L"Input");
	m_CMB_IO6.AddString(L"Output");
	m_CMB_IO7.AddString(L"Input");
	m_CMB_IO7.AddString(L"Output");
	
	m_CMB_HL0.AddString(L"Low");
	m_CMB_HL0.AddString(L"High");
	m_CMB_HL1.AddString(L"Low");
	m_CMB_HL1.AddString(L"High");
	m_CMB_HL2.AddString(L"Low");
	m_CMB_HL2.AddString(L"High");
	m_CMB_HL3.AddString(L"Low");
	m_CMB_HL3.AddString(L"High");
	m_CMB_HL4.AddString(L"Low");
	m_CMB_HL4.AddString(L"High");
	m_CMB_HL5.AddString(L"Low");
	m_CMB_HL5.AddString(L"High");
	m_CMB_HL6.AddString(L"Low");
	m_CMB_HL6.AddString(L"High");
	m_CMB_HL7.AddString(L"Low");
	m_CMB_HL7.AddString(L"High");

    uint32_t bDioDisable = 0;

	err = EApiLibInitialize();
	if (err != EAPI_STATUS_INITIALIZED)
	{
		AfxMessageBox(_T("Can't initialize the application: "));
	}  

	EApiGPIOGetCaps(EAPI_GPIO_GPIO_ID((uint32_t)0), &nPinCount, &bDioDisable);

	if (nPinCount > 8)
		nUIPinCount = 8;
    else
		nUIPinCount = nPinCount;

	if (nPinCount > 8)
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->EnableWindow(TRUE);
	else
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->EnableWindow(FALSE);

	CString str;
	((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->AddString(_T("Dio1~8"));
	if (nPinCount > 8)
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->AddString(_T("Dio9~16"));
	if (nPinCount > 16)
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->AddString(_T("Dio17~24"));
	if (nPinCount > 24)
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->AddString(_T("Dio25~32"));
	if (nPinCount > 32)
		((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->AddString(_T("Dio33~40"));
	((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->SetCurSel(0);

	OnCbnSelchangeCmbGroup();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDioDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDioDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting

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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDioDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDioDlg::OnCancel()
{
	EApiLibUnInitialize();
	CDialog::OnCancel();
}

void CDioDlg::UpdateDioStatus()
{
	uint32_t nPinCount = 0;
	uint32_t bDioDisable = 0;
	for (uint32_t i = 0; i < nUIPinCount; i++)
	{
		EApiGPIOGetCaps(EAPI_GPIO_GPIO_ID((int)(i + (8 * (nSelectedGroup - 1)))), &nPinCount, &bDioDisable);

		if (bDioDisable != 0x01)
		{
			UpdateDioPinState(i);
		}
		else
		{
			SetDlgItemText(IDC_IO1 + i, _T(""));
			SetDlgItemText(IDC_LH1 + i, _T(""));
		}
	}
}

void CDioDlg::UpdateDioPinState(int dPin)
{
	EApiStatus_t err1 = EAPI_STATUS_SUCCESS;
	EApiStatus_t err2 = EAPI_STATUS_SUCCESS;
	uint32_t bInput;
	uint32_t bHigh;
	CString str;

    err1 = EApiGPIOGetDirection(EAPI_GPIO_GPIO_ID((int)(dPin + (8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, &bInput);	  
    if (err1 != EAPI_STATUS_SUCCESS)
    {
		SetDlgItemText(IDC_IO1 + dPin, _T("X"));
    }
	else
	{
		if (bInput)
			SetDlgItemText(IDC_IO1 + dPin, _T("INPUT"));
		else
			SetDlgItemText(IDC_IO1 + dPin, _T("OUTPUT"));
	}
		
    err2 = EApiGPIOGetLevel(EAPI_GPIO_GPIO_ID((int)(dPin + (8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, &bHigh);
    if (err2 != EAPI_STATUS_SUCCESS)
    {
		SetDlgItemText(IDC_LH1 + dPin, _T("X"));
    }
	else
	{
		if (bHigh)
			SetDlgItemText(IDC_LH1 + dPin, _T("HIGH"));
		else
			SetDlgItemText(IDC_LH1 + dPin, _T("LOW"));
	}

	if (err1 != EAPI_STATUS_SUCCESS || err2 != EAPI_STATUS_SUCCESS)
	{
		if (err1 == EAPI_STATUS_DEVICE_NOT_READY || err2 == EAPI_STATUS_DEVICE_NOT_READY)
		{
			str.Format(_T("Can't read DIO %d value:\nHardware not ready. Please check BIOS setting."), dPin + 1 + (8 * (nSelectedGroup - 1)));
		}
		else
		{
			str.Format(_T("Can't read DIO value."));
		}

		AfxMessageBox(str, MB_ICONSTOP);
	}
}

void CDioDlg::OnBnClickedBtnSet()
{
	uint32_t nPinCount = 0;
	uint32_t bDioDisable = 0;

	for (uint32_t i = 0; i < nUIPinCount; i++)
	{
		EApiGPIOGetCaps(EAPI_GPIO_GPIO_ID((int)(i + (8 * (nSelectedGroup - 1)))), &nPinCount, &bDioDisable);
		if (bDioDisable != 0x01)
		{
			SetDioPinState(i);
		}
	}

	UpdateDioStatus();
}

void CDioDlg::SetDioPinState(int dPin)
{
	EApiStatus_t err1 = EAPI_STATUS_SUCCESS;
	EApiStatus_t err2 = EAPI_STATUS_SUCCESS;
	uint32_t nInput;
	uint32_t nHigh;
	CString str;

	nInput = ((CComboBox*)GetDlgItem(IDC_CMB_IO0 + dPin))->GetCurSel() == 0;
	err1 = EApiGPIOSetDirection(EAPI_GPIO_GPIO_ID((int)(dPin + (8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, nInput);

	nHigh = ((CComboBox*)GetDlgItem(IDC_CMB_HL0 + dPin))->GetCurSel() == 0;
	err2 = EApiGPIOSetLevel(EAPI_GPIO_GPIO_ID((int)(dPin + (8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, nHigh);

	if (err1 != EAPI_STATUS_SUCCESS || err2 != EAPI_STATUS_SUCCESS)
	{
		if (err1 == EAPI_STATUS_DEVICE_NOT_READY || err2 == EAPI_STATUS_DEVICE_NOT_READY)
		{
			str.Format(_T("Can't set DIO %d value:\nHardware not ready. Please check BIOS setting."), dPin + 1 + (8 * (nSelectedGroup - 1)));
		}
		else
		{
			str.Format(_T("Can't set DIO value."));
		}
		AfxMessageBox(str, MB_ICONSTOP);
	}
}

void CDioDlg::OnCbnSelchangeCmbGroup()
{
	EApiStatus_t err1 = EAPI_STATUS_SUCCESS;
	EApiStatus_t err2 = EAPI_STATUS_SUCCESS;
	uint32_t nInput = 0;
	uint32_t nHigh = 0;

	uint32_t bDioDisable = 0;
	CString str;

	nSelectedGroup = ((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->GetCurSel() + 1;

	for (int i = 0; i < 8; i++)
    {
		str.Format(_T(""), i);
		SetDlgItemText(IDC_Pin0 + i, str);
		SetDlgItemText(IDC_DPin0 + i, str);

		SetDlgItemText(IDC_LH1 + i, _T(""));
		SetDlgItemText(IDC_IO1 + i, _T(""));
	}

	if (nSelectedGroup == ((CComboBox*)GetDlgItem(IDC_CMB_GROUP))->GetCount())
	{
		nUIPinCount = nPinCount % 8;
		if (nUIPinCount == 0)
			nUIPinCount = 8;
	}
	else
		nUIPinCount = 8;

	for (uint32_t i = 0; i < nUIPinCount; i++)
	{
		str.Format(_T("Dio%d"), i + (8 * (nSelectedGroup - 1)) + 1);

		SetDlgItemText(IDC_Pin0 + i, str);
		SetDlgItemText(IDC_DPin0 + i, str);

		EApiGPIOGetCaps(EAPI_GPIO_GPIO_ID((int)(i + (8 * (nSelectedGroup - 1)))), &nPinCount, &bDioDisable);

		if (bDioDisable != 0x01)
		{
			err1 = EApiGPIOGetDirection(EAPI_GPIO_GPIO_ID((int)(i + (8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, &nInput);
			if (err1 != EAPI_STATUS_SUCCESS)
			{
				SetDlgItemText(IDC_IO1 + i, _T("X"));
			}
			else
			{
				((CComboBox*)GetDlgItem(IDC_CMB_IO0 + i))->SetCurSel(nInput == 0);

				if (nInput)
					SetDlgItemText(IDC_IO1 + i, _T("INPUT"));
				else
					SetDlgItemText(IDC_IO1 + i, _T("OUTPUT"));
			}

			err2 = EApiGPIOGetLevel(EAPI_GPIO_GPIO_ID((int)((i)+(8 * (nSelectedGroup - 1)))), 0xFFFFFFFF, &nHigh);
			if (err2 != EAPI_STATUS_SUCCESS)
			{
				SetDlgItemText(IDC_LH1 + i, _T("X"));
			}
			else
			{
				((CComboBox*)GetDlgItem(IDC_CMB_HL0 + i))->SetCurSel(nHigh == 0);

				if (nHigh)
					SetDlgItemText(IDC_LH1 + i, _T("HIGH"));
				else
					SetDlgItemText(IDC_LH1 + i, _T("LOW"));
			}

			if (err1 != EAPI_STATUS_SUCCESS || err2 != EAPI_STATUS_SUCCESS)
			{
				if (err1 == EAPI_STATUS_DEVICE_NOT_READY || err2 == EAPI_STATUS_DEVICE_NOT_READY)
				{
					str.Format(_T("Can't read DIO %d value:\nHardware not ready. Please check BIOS setting."), i + 1 + (8 * (nSelectedGroup - 1)));
				}
				else
				{
					str.Format(_T("Can't read DIO value."));
				}
				AfxMessageBox(str, MB_ICONSTOP);
			}
		}
		else
		{
			SetDlgItemText(IDC_IO1 + i, _T(""));
			SetDlgItemText(IDC_LH1 + i, _T(""));
		}

		((CComboBox*)GetDlgItem(IDC_CMB_IO0 + i))->EnableWindow(bDioDisable == 0);
		((CComboBox*)GetDlgItem(IDC_CMB_HL0 + i))->EnableWindow(bDioDisable == 0);
	}
}
