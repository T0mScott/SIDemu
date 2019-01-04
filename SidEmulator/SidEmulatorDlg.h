
// SidEmulatorDlg.h: Headerdatei
//

#pragma once


// CSidEmulatorDlg-Dialogfeld
class CSidEmulatorDlg : public CDialogEx
{
// Konstruktion
public:
	CSidEmulatorDlg(CWnd* pParent = NULL);	// Standardkonstruktor

// Dialogfelddaten
	enum { IDD = IDD_SIDEMULATOR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedVoice1Noise();
	afx_msg void OnBnClickedVoice1Pulse();
	afx_msg void OnBnClickedVoice1Sawtoth();
	afx_msg void OnBnClickedVoice1Sawtoth2();
	afx_msg void OnBnClickedVoice1Triangle();
	afx_msg void OnBnClickedVoice1Test();
	afx_msg void OnBnClickedVoice1Sync();
	afx_msg void OnBnClickedVoice1Ringmod();
	afx_msg void OnBnClickedVoice1Gate();
	afx_msg void OnBnClickedVoice1Sawtooth();
	afx_msg void OnCbnSelchangeVoice1Attack();
	afx_msg void OnCbnSelchangeVoice1Decay();
	afx_msg void OnCbnSelchangeVoice1Sustain();
	afx_msg void OnCbnSelchangeVoice1Release();
	afx_msg void OnNMCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderVoice1Freq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderVoice1Pw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkboxVoice2Noise();
	afx_msg void OnBnClickedChkboxVoice2Pulse();
	afx_msg void OnBnClickedChkboxVoice2Sawtooth();
	afx_msg void OnBnClickedChkboxVoice2Triangle();
	afx_msg void OnBnClickedChkboxVoice2Test();
	afx_msg void OnBnClickedChkboxVoice2Ringmod();
	afx_msg void OnBnClickedChkboxVoice2Sync();
	afx_msg void OnBnClickedChkboxVoice2Gate();
	afx_msg void OnCbnSelchangeComboboxVoice2Attack();
	afx_msg void OnCbnSelchangeComboboxVoice2Decay();
	afx_msg void OnCbnSelchangeComboboxVoice2Sustain();
	afx_msg void OnCbnSelchangeComboboxVoice2Release();
	afx_msg void OnTRBNThumbPosChangingSliderVoice2Freq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderVoice2Pw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkboxVoice3Noise();
	afx_msg void OnBnClickedChkboxVoice3Pulse();
	afx_msg void OnBnClickedChkboxVoice3Sawtooth();
	afx_msg void OnBnClickedChkboxVoice3Triangle();
	afx_msg void OnBnClickedChkboxVoice3Test();
	afx_msg void OnBnClickedChkboxVoice3Ringmod();
	afx_msg void OnBnClickedChkboxVoice3Sync();
	afx_msg void OnBnClickedChkboxVoice3Gate();
	afx_msg void OnCbnSelchangeComboboxVoice3Attack();
	afx_msg void OnCbnSelchangeComboboxVoice3Decay();
	afx_msg void OnCbnSelchangeComboboxVoice3Sustain();
	afx_msg void OnCbnSelchangeComboboxVoice3Release();
	afx_msg void OnTRBNThumbPosChangingSliderVoice3Freq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderVoice3Pw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkboxFilterVoice1();
	afx_msg void OnBnClickedChkboxFilterVoice2();
	afx_msg void OnBnClickedChkboxFilterVoice3();
	afx_msg void OnTRBNThumbPosChangingSliderFilterFreq(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderFilterFreq2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderFilterRes(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTRBNThumbPosChangingSliderMasterVol(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedChkboxFilterHp();
	afx_msg void OnBnClickedChkboxFilterBp();
	afx_msg void OnBnClickedChkboxFilterLp();
	afx_msg void OnBnClickedChkboxFilterVoice3Off();
};


// CAboutDlg-Dialogfeld für Anwendungsbefehl "Info"
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialogfelddaten
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	DECLARE_MESSAGE_MAP()
};
