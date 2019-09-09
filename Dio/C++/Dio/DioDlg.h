// DioDlg.h : header file
//



#include "afxwin.h"

#pragma once


// CDioDlg dialog
class CDioDlg : public CDialog
{
// Construction
public:
	CDioDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_DIO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    HBITMAP m_hBitmap;

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

	uint32_t nPinCount;
	uint32_t nUIPinCount;

	int nSelectedGroup;
	EApiStatus_t err;

	CComboBox	m_CMB_IO0;
	CComboBox	m_CMB_IO1;
	CComboBox	m_CMB_IO2;
	CComboBox	m_CMB_IO3;
	CComboBox	m_CMB_IO4;
	CComboBox	m_CMB_IO5;
	CComboBox	m_CMB_IO6;
	CComboBox	m_CMB_IO7;
	CComboBox	m_CMB_HL0;
	CComboBox	m_CMB_HL1;
	CComboBox	m_CMB_HL2;
	CComboBox	m_CMB_HL3;
	CComboBox	m_CMB_HL4;
	CComboBox	m_CMB_HL5;
	CComboBox	m_CMB_HL6;
	CComboBox	m_CMB_HL7;

	void UpdateDioStatus();
	void UpdateDioPinState(int dPin);
	void SetDioPinState(int dPin);

	afx_msg void OnCancel();
	afx_msg void OnBnClickedBtnSet();
	afx_msg void OnCbnSelchangeCmbGroup();
};
