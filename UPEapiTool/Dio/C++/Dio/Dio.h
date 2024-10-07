// Dio.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CDioApp:
// See Dio.cpp for the implementation of this class
//

class CDioApp : public CWinApp
{
public:
	CDioApp();

	ULONG_PTR m_gdiplusToken;

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CDioApp theApp;