// I2CDlg.h : header file
//

#pragma once


// CI2CDlg dialog
class CI2CDlg : public CDialog
{
// Construction
public:
	CI2CDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_I2C_DIALOG };


	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	BOOL HexStrToDec(CString str, BYTE* pbValue);
	BOOL HexStrToArray(CString str, BYTE* bDataArr);

	afx_msg void OnDestroy();

	afx_msg void OnBnClickedBtnRead();
	afx_msg void OnBnClickedBtnWrite();
	afx_msg void OnBnClickedRadioProtocolByte();
	afx_msg void OnBnClickedRadioProtocolRaw();
	afx_msg void OnBnClickedRadioProtocolBlock();
	afx_msg void OnBnClickedRadioOffsetByte();
	afx_msg void OnBnClickedRadioOffsetWord();
};

