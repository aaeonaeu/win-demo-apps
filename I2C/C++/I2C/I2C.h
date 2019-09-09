// I2C.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CI2CApp:
// See I2C.cpp for the implementation of this class
//

class CI2CApp : public CWinApp
{
public:
	CI2CApp();

// Overrides
	public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CI2CApp theApp;
