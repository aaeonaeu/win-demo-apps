// I2CDlg.cpp : implementation file
//

#include "stdafx.h"
#include "I2C.h"
#include "I2CDlg.h"

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


// CI2CDlg dialog




CI2CDlg::CI2CDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CI2CDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CI2CDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CI2CDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_READ, &CI2CDlg::OnBnClickedBtnRead)
	ON_BN_CLICKED(IDC_BTN_WRITE, &CI2CDlg::OnBnClickedBtnWrite)
	ON_BN_CLICKED(IDC_RADIO_PROTOCOL_BYTE, &CI2CDlg::OnBnClickedRadioProtocolByte)
	ON_BN_CLICKED(IDC_RADIO_PROTOCOL_RAW, &CI2CDlg::OnBnClickedRadioProtocolRaw)
	ON_BN_CLICKED(IDC_RADIO_PROTOCOL_BLOCK, &CI2CDlg::OnBnClickedRadioProtocolBlock)
	ON_BN_CLICKED(IDC_RADIO_OFFSET_BYTE, &CI2CDlg::OnBnClickedRadioOffsetByte)
	ON_BN_CLICKED(IDC_RADIO_OFFSET_WORD, &CI2CDlg::OnBnClickedRadioOffsetWord)
END_MESSAGE_MAP()


// CI2CDlg message handlers

BOOL CI2CDlg::OnInitDialog()
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

    //Step1. Please EApiLibInitialize I2C ; 
	HRESULT err = EApiLibInitialize();
	if (err != EAPI_STATUS_INITIALIZED)
	{
		AfxMessageBox(_T("Can't initialize the application"));

		OnDestroy();
		return FALSE;
	}

	// Initialize UI
	CString str;
	for (int i = 0; i < 8; i++)
	{
		str.Format(_T("I2C Bus%d"), i);
		((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->SetCurSel(0);

	for (int i = 0; i <= 0x7F; i++)
	{
		str.Format(_T("0x%02X"), i * 2);
		((CComboBox*)GetDlgItem(IDC_CMB_SLAVE_ADDR))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_CMB_SLAVE_ADDR))->SetCurSel(80);

	for (int i = 0; i <= 0xFF; i++)
	{
		str.Format(_T("%02X"), i);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->AddString(str);
		str.Format(_T("%02X"), i);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->AddString(str);
	}
	((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->SetCurSel(0);
	((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->SetCurSel(0);

	for (int i = 1; i < 65; i++)
	{
		str.Format(_T("%d"), i);
		((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->AddString(str);
	}+
	((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->SetCurSel(0);

	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->SetCheck(BST_CHECKED);
	OnBnClickedRadioProtocolByte();
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->SetCheck(BST_CHECKED);
	OnBnClickedRadioOffsetByte();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CI2CDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CI2CDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CI2CDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CI2CDlg::OnDestroy()
{
	// Step5, close the I2C handle.
	EApiLibUnInitialize();

	CDialog::OnDestroy();
}

BOOL CI2CDlg::HexStrToDec(CString str, BYTE* pbValue)
{
	int dec_value = 0, shift = 1;
	int nCount = str.GetLength();

	for (int i = nCount - 1; i >= 0; i--)
	{
		int n = str[i];
		if (n >= 48 && n <= 57)
			n = n - 48;
		else if (n >= 65 && n <= 70)
			n = n - 55;
		else if (n >= 97 && n <= 102)
			n = n - 87;
		else
		{
			*pbValue = 0;
			return FALSE;
		}

		dec_value += n * shift;
		shift = shift * 16;
	}
	*pbValue = dec_value;

	return TRUE;
}

BOOL CI2CDlg::HexStrToArray(CString str, BYTE* bDataArr)
{
	BOOL err = TRUE;
	BYTE bDataLength = 0x00;

	if ((str.GetLength() % 2) == 1)
	{
		bDataLength = (str.GetLength() / 2) + 1;
		str = _T("0") + str;
	}
	else
	{
		bDataLength = str.GetLength() / 2;
	}

	if (bDataLength > 32)
	{
		return FALSE;
	}

	BYTE bDataTemp = 0x00;
	for (int i = 0; i < bDataLength; i++)
	{
		err = HexStrToDec(str.Mid(i * 2, 2), &bDataTemp);

		if (!err)
		{
			return FALSE;
		}

		bDataArr[i] = bDataTemp;
	}

	return err;
}

void CI2CDlg::OnBnClickedRadioProtocolByte()
{
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_RAW))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BLOCK))->SetCheck(BST_UNCHECKED);

	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->EnableWindow(TRUE);

	if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->GetCheck() == BST_CHECKED)
	{
		((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->GetCheck())
	{
		((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
	}

	((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->EnableWindow(FALSE);
	((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->SetCurSel(0);
}

void CI2CDlg::OnBnClickedRadioProtocolRaw()
{
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_RAW))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BLOCK))->SetCheck(BST_UNCHECKED);

	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->EnableWindow(FALSE);

	((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(FALSE);
	((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(FALSE);

	((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->EnableWindow(TRUE);
}

void CI2CDlg::OnBnClickedRadioProtocolBlock()
{
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_RAW))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BLOCK))->SetCheck(BST_CHECKED);

	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->EnableWindow(TRUE);

	if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->GetCheck() == BST_CHECKED)
	{
		((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(TRUE);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->GetCheck())
	{
		((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(FALSE);
		((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
	}

	((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->EnableWindow(TRUE);
}

void CI2CDlg::OnBnClickedRadioOffsetByte()
{
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->SetCheck(BST_CHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->SetCheck(BST_UNCHECKED);

	((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(FALSE);
	((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
}

void CI2CDlg::OnBnClickedRadioOffsetWord()
{
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->SetCheck(BST_UNCHECKED);
	((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->SetCheck(BST_CHECKED);

	((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->EnableWindow(TRUE);
	((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->EnableWindow(TRUE);
}

// Through the "Read" button, you can get the data from the slave device
void CI2CDlg::OnBnClickedBtnRead()
{
	EApiStatus_t ret = EAPI_STATUS_ERROR;
	EApiId_t I2CId = 0;
	uint32_t Cmd = 0;
	CString str;
	
	uint32_t bSlaveAddr = ((CComboBox*)GetDlgItem(IDC_CMB_SLAVE_ADDR))->GetCurSel() & 0x7F;
	BYTE bRegHigh = ((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->GetCurSel();
	BYTE bRegLow = ((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->GetCurSel();
	uint32_t bDataLength = ((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->GetCurSel() + 1;

	if (0 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_I2C_EXTERNAL;
	}
	else if (1 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_2;
	}
	else if (2 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_3;
	}
	else if (3 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_4;
	}
	else if (4 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_5;
	}
	else if (5 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_6;
	}
	else if (6 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_7;
	}
	else if (7 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_8;
	}

	if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->GetCheck() == BST_CHECKED)
	{
		Cmd = bRegHigh << 8 | bRegLow;
		Cmd = EAPI_I2C_ENC_EXT_CMD(Cmd);	//Extended Cmd
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->GetCheck())
	{
		Cmd = bRegLow;
		Cmd = EAPI_I2C_ENC_STD_CMD(Cmd);	//Standard Cmd
	}

	/*Read Byte Mode*/
	if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->GetCheck() == BST_CHECKED)
	{
		BYTE bData = 0x00;

		ret = EApiI2CReadTransfer(I2CId, bSlaveAddr, Cmd, &bData, bDataLength, bDataLength);
		if (ret == EAPI_STATUS_SUCCESS)
		{
			str.Format(_T("%02X"), bData);
			SetDlgItemText(IDC_EDIT_DATA_OUTPUT, str);
		}
		else
		{
			SetDlgItemText(IDC_EDIT_DATA_OUTPUT, _T(""));
		}
	}
	/*Read Raw Mode*/
	else if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_RAW))->GetCheck() == BST_CHECKED)
	{
		BYTE bData[128] = { 0 };
		ret = EApiI2CReadRaw(I2CId, bSlaveAddr, bData, bDataLength, bDataLength);
		if (ret == EAPI_STATUS_SUCCESS)
		{
			CString strTemp;
			for (uint32_t i = 0; i < bDataLength; i++)
			{
				strTemp.Format(_T("%02X"), bData[i]);
				str = str + strTemp + _T(" ");
				SetDlgItemText(IDC_EDIT_DATA_OUTPUT, str);
			}
		}
		else
		{
			SetDlgItemText(IDC_EDIT_DATA_OUTPUT, _T(""));
		}
	}
	/*Read Block Mode*/
	else if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BLOCK))->GetCheck() == BST_CHECKED)
	{
		BYTE bData[128] = { 0 };
		ret = EApiI2CReadTransfer(I2CId, bSlaveAddr, Cmd, bData, bDataLength, bDataLength);
		if (ret == EAPI_STATUS_SUCCESS)
		{
			CString strTemp;
			for (uint32_t i = 0; i < bDataLength; i++)
			{
				strTemp.Format(_T("%02X"), bData[i]);
				str = str + strTemp + _T(" ");
				SetDlgItemText(IDC_EDIT_DATA_OUTPUT, str);
			}
		}
		else
		{
			SetDlgItemText(IDC_EDIT_DATA_OUTPUT, _T(""));
		}
	}
	else
	{
		AfxMessageBox(_T("Please Choose a Protocol"));
	}

	switch (ret)
	{
	case EAPI_STATUS_NOT_INITIALIZED:
		str.Format(_T("STATUS_NOT_INITIALIZED!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_INVALID_PARAMETER:
		str.Format(_T("STATUS_INVALID_PARAMETER!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_INVALID_BLOCK_LENGTH:
		str.Format(_T("STATUS_INVALID_BLOCK_LENGTH!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_MORE_DATA:
		str.Format(_T("STATUS_MORE_DATA!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_ERROR:
		str.Format(_T("STATUS_ERROR!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_UNSUPPORTED:
		str.Format(_T("STATUS_UNSUPPORTED!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_SUCCESS:
		str.Format(_T("STATUS_SUCCESS!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	default:
		break;
	}
}

// Through the "Write" button, you can get the data from the slave device
void CI2CDlg::OnBnClickedBtnWrite()
{
	EApiStatus_t ret = EAPI_STATUS_ERROR;
	EApiId_t I2CId = 0;
	uint32_t Cmd = 0;
	CString str, strData;
	BYTE bData[128] = { 0 };

	uint32_t bSlaveAddr = ((CComboBox*)GetDlgItem(IDC_CMB_SLAVE_ADDR))->GetCurSel() & 0x7F;
	BYTE bRegHigh = ((CComboBox*)GetDlgItem(IDC_CMB_REG_HIGH))->GetCurSel();
	BYTE bRegLow = ((CComboBox*)GetDlgItem(IDC_CMB_REG_LOW))->GetCurSel();
	uint32_t bDataLength = ((CComboBox*)GetDlgItem(IDC_CMB_DATALENGTH))->GetCurSel() + 1;

	if (0 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_I2C_EXTERNAL;
	}
	else if (1 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_2;
	}
	else if (2 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_3;
	}
	else if (3 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_4;
	}
	else if (4 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_5;
	}
	else if (5 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_6;
	}
	else if (6 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_7;
	}
	else if (7 == ((CComboBox*)GetDlgItem(IDC_CMB_BUSSELECTOR))->GetCurSel())
	{
		I2CId = EAPI_ID_AONCUS_I2C_EXTERNAL_8;
	}

	if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_WORD))->GetCheck() == BST_CHECKED)
	{
		Cmd = bRegHigh << 8 | bRegLow;
		Cmd = EAPI_I2C_ENC_EXT_CMD(Cmd);	//Extended Cmd
	}
	else if (((CButton*)GetDlgItem(IDC_RADIO_OFFSET_BYTE))->GetCheck())
	{
		Cmd = bRegLow;
		Cmd = EAPI_I2C_ENC_STD_CMD(Cmd);	//Standard Cmd
	}

	GetDlgItemText(IDC_EDIT_DATA_INPUT, strData);
	if (!HexStrToArray(strData, bData))
	{
		AfxMessageBox(_T("Input Data Error"));
		return;
	}

	/*Write Byte Mode*/
	if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BYTE))->GetCheck() == BST_CHECKED)
	{
		ret = EApiI2CWriteTransfer(I2CId, bSlaveAddr, Cmd, bData, bDataLength);
	}
	/*Write Raw Mode*/
	else if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_RAW))->GetCheck() == BST_CHECKED)
	{
		ret = EApiI2CWriteRaw(I2CId, bSlaveAddr, bData, bDataLength);
	}
	/*Write Block Mode*/
	else if (((CButton*)GetDlgItem(IDC_RADIO_PROTOCOL_BLOCK))->GetCheck() == BST_CHECKED)
	{
		ret = EApiI2CWriteTransfer(I2CId, bSlaveAddr, Cmd, bData, bDataLength);
	}
	else
	{
		AfxMessageBox(_T("Please Choose a Protocol"));
	}

	switch (ret)
	{
	case EAPI_STATUS_NOT_INITIALIZED:
		str.Format(_T("STATUS_NOT_INITIALIZED!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_INVALID_PARAMETER:
		str.Format(_T("STATUS_INVALID_PARAMETER!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_INVALID_BLOCK_LENGTH:
		str.Format(_T("STATUS_INVALID_BLOCK_LENGTH!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_MORE_DATA:
		str.Format(_T("STATUS_MORE_DATA!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_ERROR:
		str.Format(_T("STATUS_ERROR!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_UNSUPPORTED:
		str.Format(_T("STATUS_UNSUPPORTED!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	case EAPI_STATUS_SUCCESS:
		str.Format(_T("STATUS_SUCCESS!!"));
		SetDlgItemText(IDC_EDIT_STATUS, str);
		break;
	default:
		break;
	}
}