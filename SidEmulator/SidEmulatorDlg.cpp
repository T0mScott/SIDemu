//
// SidEmulatorDlg.cpp: Implementierungsdatei
//

#include "stdafx.h"
#include "SidEmulator.h"
#include "SidEmulatorDlg.h"
#include "afxdialogex.h"
#include "sid.h"
#include "sidregister.h"
#include "SidWrapper.h"
#include "soundwmm.h"

#include "WaveOut.h"

#define BUF_LEN                  4096
#define BUF_NUMBERS              4
#define MAX_LOADSTRING           100
#define SOUND_SAMPLE_RATE        44100
#define C64_PAL_CYCLES_PER_SEC   985248
#define C64_PAL_CYCLES_PER_LINE  63
#define C64_PAL_SCREEN_LINES     312
#define C64_PAL_CYCLES_PER_RFSH  (C64_PAL_SCREEN_LINES * C64_PAL_CYCLES_PER_LINE)
#define SOUND_FRAGMENT_SIZE      1 /* 10ms */
#define SOUND_BITS_PER_SAMPLE    16
#define SOUND_NUMBER_OF_CHANNELS 1
#define SOUND_BLOCK_ALIGN        ((SOUND_NUMBER_OF_CHANNELS * SOUND_BITS_PER_SAMPLE) / 8)
#define SOUND_AVG_BYTES_PER_SEC  (SOUND_BLOCK_ALIGN * SOUND_SAMPLE_RATE)

/* PAL refresh rate: 50.123432124542124 */
#define C64_PAL_RFSH_PER_SEC    (1.0 / ((double)C64_PAL_CYCLES_PER_RFSH / (double)C64_PAL_CYCLES_PER_SEC))


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Global Variables:
short        asBuf[BUF_LEN]; // Sample Buffer
WAVEFORMATEX wfx = { WAVE_FORMAT_PCM, SOUND_NUMBER_OF_CHANNELS, SOUND_SAMPLE_RATE, SOUND_AVG_BYTES_PER_SEC, SOUND_BLOCK_ALIGN, SOUND_BITS_PER_SAMPLE, 0 };
CWaveOut     WaveOut(&wfx, BUF_NUMBERS, BUF_LEN);
SidWrapper   oSID;
FILE        *pFile;
TCHAR asAttackComboBoxString[16][10] = {	TEXT("2ms"), TEXT("8ms"), TEXT("16ms"), TEXT("24ms"), TEXT("38ms"), TEXT("56ms"), TEXT("68ms"),	TEXT("80ms"),
											TEXT("100ms"), TEXT("250ms"), TEXT("500ms"), TEXT("800ms"), TEXT("1s"), TEXT("3s"),	TEXT("5s"),	TEXT("8s") };

// Forward declarations of functions included in this code module:
VOID CALLBACK pfTimerCb( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);


CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CSidEmulatorDlg-Dialogfeld
CSidEmulatorDlg::CSidEmulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSidEmulatorDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}


void CSidEmulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSidEmulatorDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_NOISE, &CSidEmulatorDlg::OnBnClickedVoice1Noise)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_PULSE, &CSidEmulatorDlg::OnBnClickedVoice1Pulse)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_TRIANGLE, &CSidEmulatorDlg::OnBnClickedVoice1Triangle)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_TEST, &CSidEmulatorDlg::OnBnClickedVoice1Test)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_SYNC, &CSidEmulatorDlg::OnBnClickedVoice1Sync)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_RINGMOD, &CSidEmulatorDlg::OnBnClickedVoice1Ringmod)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_GATE, &CSidEmulatorDlg::OnBnClickedVoice1Gate)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE1_SAWTOOTH, &CSidEmulatorDlg::OnBnClickedVoice1Sawtooth)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE1_ATTACK, &CSidEmulatorDlg::OnCbnSelchangeVoice1Attack)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE1_DECAY, &CSidEmulatorDlg::OnCbnSelchangeVoice1Decay)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE1_SUSTAIN, &CSidEmulatorDlg::OnCbnSelchangeVoice1Sustain)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE1_RELEASE, &CSidEmulatorDlg::OnCbnSelchangeVoice1Release)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE1_FREQ, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice1Freq)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE1_PW, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice1Pw)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_NOISE, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Noise)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_PULSE, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Pulse)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_SAWTOOTH, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Sawtooth)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_TRIANGLE, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Triangle)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_TEST, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Test)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_RINGMOD, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Ringmod)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_SYNC, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Sync)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE2_GATE, &CSidEmulatorDlg::OnBnClickedChkboxVoice2Gate)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE2_ATTACK, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Attack)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE2_DECAY, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Decay)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE2_SUSTAIN, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Sustain)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE2_RELEASE, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Release)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE2_FREQ, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice2Freq)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE2_PW, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice2Pw)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_NOISE, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Noise)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_PULSE, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Pulse)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_SAWTOOTH, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Sawtooth)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_TRIANGLE, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Triangle)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_TEST, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Test)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_RINGMOD, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Ringmod)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_SYNC, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Sync)
	ON_BN_CLICKED(IDC_CHKBOX_VOICE3_GATE, &CSidEmulatorDlg::OnBnClickedChkboxVoice3Gate)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE3_ATTACK, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Attack)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE3_DECAY, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Decay)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE3_SUSTAIN, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Sustain)
	ON_CBN_SELCHANGE(IDC_COMBOBOX_VOICE3_RELEASE, &CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Release)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE3_FREQ, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice3Freq)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_VOICE3_PW, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice3Pw)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_VOICE1, &CSidEmulatorDlg::OnBnClickedChkboxFilterVoice1)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_VOICE2, &CSidEmulatorDlg::OnBnClickedChkboxFilterVoice2)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_VOICE3, &CSidEmulatorDlg::OnBnClickedChkboxFilterVoice3)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_FILTER_FREQ2, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderFilterFreq2)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_FILTER_RES, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderFilterRes)
	ON_NOTIFY(TRBN_THUMBPOSCHANGING, IDC_SLIDER_MASTER_VOL, &CSidEmulatorDlg::OnTRBNThumbPosChangingSliderMasterVol)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_HP, &CSidEmulatorDlg::OnBnClickedChkboxFilterHp)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_BP, &CSidEmulatorDlg::OnBnClickedChkboxFilterBp)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_LP, &CSidEmulatorDlg::OnBnClickedChkboxFilterLp)
	ON_BN_CLICKED(IDC_CHKBOX_FILTER_VOICE3_OFF, &CSidEmulatorDlg::OnBnClickedChkboxFilterVoice3Off)
END_MESSAGE_MAP()


// CSidEmulatorDlg-Meldungshandler
BOOL CSidEmulatorDlg::OnInitDialog()
{
#if (TEST_MODE == 1)
	errno_t  tErr;
    char     acErrBuf[80];

    tErr = fopen_s(&pFile, "sample.snd", "w");

    if( tErr != 0 ) 
    {
      printf("Cannot open file.\n");
      strerror_s(acErrBuf, 80, tErr);
      fprintf( stderr, acErrBuf );

      getchar();
      exit(1);
    }
#endif /* TEST_MODE */

	CDialogEx::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
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

	// Symbol für dieses Dialogfeld festlegen. Wird automatisch erledigt
	// wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// Init ComboBoxes
	CComboBox *m_ComboBox;
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_ATTACK);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_DECAY);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_SUSTAIN);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_RELEASE);
	m_ComboBox->SetCurSel(0);

	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_ATTACK);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_DECAY);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_SUSTAIN);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_RELEASE);
	m_ComboBox->SetCurSel(0);

	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_ATTACK);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_DECAY);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_SUSTAIN);
	m_ComboBox->SetCurSel(0);
	m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_RELEASE);
	m_ComboBox->SetCurSel(0);

	// Init Sliders and belonging Edit control boxes
	CSliderCtrl *m_CSlider;
	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE1_FREQ);
	m_CSlider->SetRange(0, 0xFFFF);
	SetDlgItemText(IDC_EDIT_VOICE1_FREQ, L"Freq: 0 Hz");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE1_PW);
	m_CSlider->SetRange(0, 0x0FFF);
	SetDlgItemText(IDC_EDIT_VOICE1_PW, L"PW: 0%");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE2_FREQ);
	m_CSlider->SetRange(0, 0xFFFF);
	SetDlgItemText(IDC_EDIT_VOICE2_FREQ, L"Freq: 0 Hz");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE2_PW);
	m_CSlider->SetRange(0, 0x0FFF);
	SetDlgItemText(IDC_EDIT_VOICE2_PW, L"PW: 0%");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE3_FREQ);
	m_CSlider->SetRange(0, 0xFFFF);
	SetDlgItemText(IDC_EDIT_VOICE3_FREQ, L"Freq: 0 Hz");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_VOICE3_PW);
	m_CSlider->SetRange(0, 0x7FF);
	SetDlgItemText(IDC_EDIT_VOICE3_PW, L"PW: 0%");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FILTER_FREQ2);
	m_CSlider->SetRange(0, 0x07FF);
	SetDlgItemText(IDC_EDIT_FILTER_FREQ, L"Freq: 0% Hz");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_FILTER_RES);
	m_CSlider->SetRange(0, 0x0F);
	SetDlgItemText(IDC_EDIT_FILTER_RES, L"Res: 0%");

	m_CSlider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_MASTER_VOL);
	m_CSlider->SetRange(0, 0x0F);
	SetDlgItemText(IDC_EDIT_MASTER_VOL, L"Vol: 0%");

	// Initialize SID
	oSID.reset();
    oSID.set_chip_model(MOS6581);

    SetTimer(1234,               // timer identifier 
             USER_TIMER_MINIMUM, // 10ms interval (minimum)
             pfTimerCb);         // timer callback

#if (TEST_MODE == 1)
    oSID.write(SIDREG_FREQ_LO_1, 0x86); // 0xCD -> Note A4
    oSID.write(SIDREG_FREQ_HI_1, 0x1C); // 0x01 -> Note A4
    oSID.write(SIDREG_CONTROL_REG_1, (SIDCTRL_TRIANGLE | SIDCTRL_GATE));
    oSID.write(SIDREG_ATTACK_DECAY_1, 0x66);
    oSID.write(SIDREG_SUSTAIN_RELEASE_1, 0xF8);
    oSID.write(SIDREG_MODE_VOL, 0x0F);
#endif /* TEST_MODE */

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}


void CSidEmulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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


// Wenn Sie dem Dialogfeld eine Schaltfläche "Minimieren" hinzufügen, benötigen Sie
//  den nachstehenden Code, um das Symbol zu zeichnen. Für MFC-Anwendungen, die das 
//  Dokument/Ansicht-Modell verwenden, wird dies automatisch ausgeführt.
void CSidEmulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Gerätekontext zum Zeichnen

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Symbol in Clientrechteck zentrieren
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Symbol zeichnen
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// Die System ruft diese Funktion auf, um den Cursor abzufragen, der angezeigt wird, während der Benutzer
//  das minimierte Fenster mit der Maus zieht.
HCURSOR CSidEmulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


// ***************************************************************************
// Timer callback
// ***************************************************************************
VOID CALLBACK pfTimerCb( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
  static size_t uiLiveTimeIdx = 0;
  static DWORD  ulTimeLast = 0;
  int    iDeltaT;
  size_t uiBufIdx = 0;
  size_t uiIndex = 0;

  if( ulTimeLast == 0 )
  {
    ulTimeLast = dwTime;
    return;
  }

  // Sample rate: 10ms * 441 = 44.100 Samples/s
  iDeltaT = (dwTime - ulTimeLast) * (SOUND_SAMPLE_RATE / (dwTime - ulTimeLast));
  ulTimeLast = dwTime;

  while( iDeltaT ) 
  {
    uiBufIdx += oSID.clock(iDeltaT, &asBuf[uiBufIdx], (int)(BUF_LEN - uiBufIdx));

	if( uiBufIdx >= BUF_LEN )
	{ // Buffer overflow
	  return;
	}

	// Write to Soundcard
#if (TEST_MODE == 1)
	fprintf( pFile, "%d\n", uiBufIdx * sizeof(short) );
#endif /* TEST_MODE */

	WaveOut.Write((PBYTE)&asBuf, uiBufIdx * sizeof(short));
	uiBufIdx = 0;
  }

  return;
}


/* --------------------------------------------------------------------------------------------- */
/* -- Voice 1 - Message Handler ---------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */
void CSidEmulatorDlg::OnBnClickedVoice1Noise()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_NOISE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_NOISE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_NOISE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedVoice1Pulse()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_PULSE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_PULSE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_PULSE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedVoice1Triangle()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_TRIANGLE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_TRIANGLE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_TRIANGLE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}




void CSidEmulatorDlg::OnBnClickedVoice1Test()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_TEST);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_TEST) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_TEST) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedVoice1Sync()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_SYNC);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_SYNC) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_SYNC) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedVoice1Ringmod()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_RINGMOD);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_RINGMOD) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_RINGMOD) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}



void CSidEmulatorDlg::OnBnClickedVoice1Gate()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_GATE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_GATE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_GATE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedVoice1Sawtooth()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE1_SAWTOOTH);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) | SIDCTRL_SAWTOOTH) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_1, (oSID.read(SIDREG_CONTROL_REG_1) & ~SIDCTRL_SAWTOOTH) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnCbnSelchangeVoice1Attack()
{
	int iSelectedItem = 0;
	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_ATTACK);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_ATTACK_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_1, ((oSID.read(SIDREG_ATTACK_DECAY_1) & SIDREG_DECAY_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeVoice1Decay()
{
	int iSelectedItem = 0;
	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_DECAY);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_DECAY_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_1, ((oSID.read(SIDREG_ATTACK_DECAY_1) & SIDREG_ATTACK_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnCbnSelchangeVoice1Sustain()
{
	int iSelectedItem = 0;
	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_SUSTAIN);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_SUSTAIN_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_1, ((oSID.read(SIDREG_SUSTAIN_RELEASE_1) & SIDREG_RELEASE_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeVoice1Release()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE1_RELEASE);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_RELEASE_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_1, ((oSID.read(SIDREG_SUSTAIN_RELEASE_1) & SIDREG_SUSTAIN_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice1Freq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0xFFFF) )
	{
		float fFreq = (float)(((float)pNMTPC->dwPos)/16.77);
		str1.Format(L"Freq: %d Hz", (UINT)fFreq);
		SetDlgItemText(IDC_EDIT_VOICE1_FREQ, str1);
		oSID.write( SIDREG_FREQ_HI_1, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_FREQ_LO_1, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice1Pw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0FFF) )
	{
		str1.Format(L"PW: %d%%", (pNMTPC->dwPos)/41);
		SetDlgItemText(IDC_EDIT_VOICE1_PW, str1);
		oSID.write( SIDREG_PW_HI_1, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_PW_LO_1, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}

/* --------------------------------------------------------------------------------------------- */
/* -- Voice 2 - Message Handler ---------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */
void CSidEmulatorDlg::OnBnClickedChkboxVoice2Noise()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_NOISE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_NOISE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_NOISE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Pulse()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_PULSE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_PULSE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_PULSE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Sawtooth()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_SAWTOOTH);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_SAWTOOTH) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_SAWTOOTH) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Triangle()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_TRIANGLE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_TRIANGLE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_TRIANGLE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Test()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_TEST);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_TEST) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_TEST) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Ringmod()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_RINGMOD);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_RINGMOD) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_RINGMOD) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Sync()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_SYNC);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_SYNC) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_SYNC) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice2Gate()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE2_GATE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) | SIDCTRL_GATE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_2, (oSID.read(SIDREG_CONTROL_REG_2) & ~SIDCTRL_GATE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Attack()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_ATTACK);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_ATTACK_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_2, ((oSID.read(SIDREG_ATTACK_DECAY_2) & SIDREG_DECAY_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Decay()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_DECAY);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_DECAY_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_2, ((oSID.read(SIDREG_ATTACK_DECAY_2) & SIDREG_ATTACK_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Sustain()
{
	int iSelectedItem = 0;
	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_SUSTAIN);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_SUSTAIN_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_2, ((oSID.read(SIDREG_SUSTAIN_RELEASE_2) & SIDREG_RELEASE_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice2Release()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE2_RELEASE);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_RELEASE_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_2, ((oSID.read(SIDREG_SUSTAIN_RELEASE_2) & SIDREG_SUSTAIN_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice2Freq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0xFFFF) )
	{
		float fFreq = (float)(((float)pNMTPC->dwPos)/16.77);
		str1.Format(L"Freq: %d Hz", (UINT)fFreq);
		SetDlgItemText(IDC_EDIT_VOICE2_FREQ, str1);
		oSID.write( SIDREG_FREQ_HI_2, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_FREQ_LO_2, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice2Pw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0FFF) )
	{
		str1.Format(L"PW: %d%%", (pNMTPC->dwPos)/41);
		SetDlgItemText(IDC_EDIT_VOICE2_PW, str1);
		oSID.write( SIDREG_PW_HI_2, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_PW_LO_2, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}


/* --------------------------------------------------------------------------------------------- */
/* -- Voice 3 - Message Handler ---------------------------------------------------------------- */
/* --------------------------------------------------------------------------------------------- */
void CSidEmulatorDlg::OnBnClickedChkboxVoice3Noise()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_NOISE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_NOISE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_NOISE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Pulse()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_PULSE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_PULSE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_PULSE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Sawtooth()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_SAWTOOTH);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_SAWTOOTH) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_SAWTOOTH) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Triangle()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_TRIANGLE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_TRIANGLE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_TRIANGLE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Test()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_TEST);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_TEST) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_TEST) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Ringmod()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_RINGMOD);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_RINGMOD) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_RINGMOD) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Sync()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_SYNC);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_SYNC) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_SYNC) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxVoice3Gate()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_VOICE3_GATE);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) | SIDCTRL_GATE) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_CONTROL_REG_3, (oSID.read(SIDREG_CONTROL_REG_3) & ~SIDCTRL_GATE) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Attack()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_ATTACK);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_ATTACK_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_3, ((oSID.read(SIDREG_ATTACK_DECAY_3) & SIDREG_DECAY_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Decay()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_DECAY);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_DECAY_RANGE);
	oSID.write( SIDREG_ATTACK_DECAY_3, ((oSID.read(SIDREG_ATTACK_DECAY_3) & SIDREG_ATTACK_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Sustain()
{
	int iSelectedItem = 0;
	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_SUSTAIN);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_SUSTAIN_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_3, ((oSID.read(SIDREG_SUSTAIN_RELEASE_3) & SIDREG_RELEASE_MASK) | (iSelectedItem << 4))  );
}


void CSidEmulatorDlg::OnCbnSelchangeComboboxVoice3Release()
{
	int iSelectedItem = 0;

	CComboBox *m_ComboBox = (CComboBox*)GetDlgItem(IDC_COMBOBOX_VOICE3_RELEASE);
	iSelectedItem = (m_ComboBox->GetCurSel() & SIDREG_RELEASE_RANGE);
	oSID.write( SIDREG_SUSTAIN_RELEASE_3, ((oSID.read(SIDREG_SUSTAIN_RELEASE_3) & SIDREG_SUSTAIN_MASK) | iSelectedItem)  );
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice3Freq(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0xFFFF) )
	{
		float fFreq = (float)(((float)pNMTPC->dwPos)/16.77);
		str1.Format(L"Freq: %d Hz", (UINT)fFreq);
		SetDlgItemText(IDC_EDIT_VOICE3_FREQ, str1);
		oSID.write( SIDREG_FREQ_HI_3, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_FREQ_LO_3, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderVoice3Pw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0FFF) )
	{
		str1.Format(L"PW: %d%%", (pNMTPC->dwPos)/41);
		SetDlgItemText(IDC_EDIT_VOICE3_PW, str1);
		oSID.write( SIDREG_PW_HI_3, (reg8)(pNMTPC->dwPos >> 8) );
		oSID.write( SIDREG_PW_LO_3, (reg8)(pNMTPC->dwPos) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnBnClickedChkboxFilterVoice1()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_VOICE1);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) | SIDFILTER_VOICE1) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) & ~SIDFILTER_VOICE1) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxFilterVoice2()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_VOICE2);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) | SIDFILTER_VOICE2) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) & ~SIDFILTER_VOICE2) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}



void CSidEmulatorDlg::OnBnClickedChkboxFilterVoice3()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_VOICE3);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) | SIDFILTER_VOICE3) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) & ~SIDFILTER_VOICE3) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderFilterFreq2(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;
	UINT32                 uiFilterFreq;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0FFF) )
	{
#define FILTER_FREQ_FKT(X)	((INT32)((-1.2386*(X)*(X)*(X)*(X)) + (38.599*(X)*(X)*(X)) - (241.1*(X)*(X)) + (481.51*(X)) - 31.084))
#define FILTER_FREQ(X)		(FILTER_FREQ_FKT(X) < 0 ? 0 : FILTER_FREQ_FKT(X))

		str1.Format( L"Freq: %d Hz", FILTER_FREQ( (((double)pNMTPC->dwPos)/128.0) ) );
		SetDlgItemText(IDC_EDIT_FILTER_FREQ, str1);
		oSID.write( SIDREG_FC_LO, (reg8)(pNMTPC->dwPos & 0x07) );
		oSID.write( SIDREG_FC_HI, (reg8)((pNMTPC->dwPos >> 3) & 0xFF) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderFilterRes(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0F) )
	{
		str1.Format( L"Res: %d%%", ((pNMTPC->dwPos*100)/0x0F) );
		SetDlgItemText(IDC_EDIT_FILTER_RES, str1);
		oSID.write( SIDREG_RES_FILT, (oSID.read(SIDREG_RES_FILT) & 0x0F) | ((reg8)((pNMTPC->dwPos & 0x0F) << 4)) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnTRBNThumbPosChangingSliderMasterVol(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMTRBTHUMBPOSCHANGING *pNMTPC;
	CString                str1;

	pNMTPC = reinterpret_cast<NMTRBTHUMBPOSCHANGING *>(pNMHDR);

	if( (pNMTPC->dwPos >= 0) && (pNMTPC->dwPos <= 0x0F) )
	{
		str1.Format( L"Vol: %d%%", ((pNMTPC->dwPos*100)/0x0F) );
		SetDlgItemText(IDC_EDIT_MASTER_VOL, str1);
		oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) & 0xF0) | ((reg8)(pNMTPC->dwPos & 0x0F)) );
	}

	*pResult = 0;
}


void CSidEmulatorDlg::OnBnClickedChkboxFilterHp()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_HP);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) | SIDFILTER_MODE_HP) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) & ~SIDFILTER_MODE_HP) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxFilterBp()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_BP);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) | SIDFILTER_MODE_BP) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) & ~SIDFILTER_MODE_BP) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}



void CSidEmulatorDlg::OnBnClickedChkboxFilterLp()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_LP);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) | SIDFILTER_MODE_LP) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) & ~SIDFILTER_MODE_LP) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}


void CSidEmulatorDlg::OnBnClickedChkboxFilterVoice3Off()
{
	CButton *m_ctlCheck = (CButton*)GetDlgItem(IDC_CHKBOX_FILTER_HP);
	int ChkBox = m_ctlCheck->GetCheck();

	switch( ChkBox )
	{
		case BST_CHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) | SIDFILTER_MODE_VOICE3_OFF) );
			break;
		}
		case BST_UNCHECKED :
		{
			oSID.write( SIDREG_MODE_VOL, (oSID.read(SIDREG_MODE_VOL) & ~SIDFILTER_MODE_VOICE3_OFF) );
			break;
		}
		default :
		{	/* nothing to do */
			break;
		}
	}
}
