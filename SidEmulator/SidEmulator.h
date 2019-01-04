
// SidEmulator.h: Hauptheaderdatei für die PROJECT_NAME-Anwendung
//

#pragma once

#ifndef __AFXWIN_H__
	#error "'stdafx.h' vor dieser Datei für PCH einschließen"
#endif

#include "resource.h"		// Hauptsymbole


// CSidEmulatorApp:
// Siehe SidEmulator.cpp für die Implementierung dieser Klasse
//

class CSidEmulatorApp : public CWinApp
{
public:
	CSidEmulatorApp();

// Überschreibungen
public:
	virtual BOOL InitInstance();

// Implementierung

	DECLARE_MESSAGE_MAP()
};

extern CSidEmulatorApp theApp;