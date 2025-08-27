#include "stdafx.h"
#include "ZoomIn.h"
#include "MainFrm.h"
#include "RefreshRateDlg.h"
#include "DibApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define X_SNAP			15
#define Y_SNAP			15

#define ABS(i)			(((i) < 0) ? -(i) : (i))

#define TIMER_ID	1234

#define WM_NOTIFYICON				(WM_USER + 1000)

// Tenths of a millimeter per inch
#define MM10PERINCH		254

// Wirebox window class name
static const TCHAR* WIREBOX_CLASS_NAME = _T("ZoomInWirebox");

// Global wirebox window handle
static HWND g_hWireboxWnd = NULL;

// Window procedure for the wirebox

//LRESULT CALLBACK WireboxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//		RECT rect;
//		GetClientRect(hwnd, &rect);
//		// Draw a 1-pixel black border (XOR for visibility)
//		HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));
//		FrameRect(hdc, &rect, hBrush);
//		DeleteObject(hBrush);
//		EndPaint(hwnd, &ps);
//		return 0;
//	}
//	case WM_ERASEBKGND:
//		return TRUE; // Transparent background
//	case WM_DESTROY:
//		g_hWireboxWnd = NULL;
//		return 0;
//	}
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}

//LRESULT CALLBACK WireboxWndProc3(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = BeginPaint(hwnd, &ps);
//		RECT rect;
//		GetClientRect(hwnd, &rect);
//
//		// Use XOR pen for high-contrast wireframe (mimics DSTINVERT)
//		HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
//		HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
//		SetROP2(hdc, R2_NOTXORPEN); // Inverts pixels for visibility
//
//		// Draw wireframe (1-pixel outline)
//		MoveToEx(hdc, rect.left, rect.top, NULL);
//		LineTo(hdc, rect.right - 1, rect.top);
//		LineTo(hdc, rect.right - 1, rect.bottom - 1);
//		LineTo(hdc, rect.left, rect.bottom - 1);
//		LineTo(hdc, rect.left, rect.top);
//
//		SelectObject(hdc, hOldPen);
//		DeleteObject(hPen);
//
//		EndPaint(hwnd, &ps);
//		return 0;
//	}
//	case WM_ERASEBKGND:
//		return TRUE; // Transparent background
//	case WM_DESTROY:
//		g_hWireboxWnd = NULL;
//		return 0;
//	}
//	return DefWindowProc(hwnd, msg, wParam, lParam);
//}

//LRESULT CALLBACK WireboxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = ::BeginPaint(hwnd, &ps);
//		RECT rect;
//		::GetClientRect(hwnd, &rect);
//
//		// Create memory DC and bitmap
//		HDC hdcMem = ::CreateCompatibleDC(hdc);
//		int width = rect.right - rect.left;
//		int height = rect.bottom - rect.top;
//
//		// Create 32-bit DIB for per-pixel alpha
//		BITMAPINFO bmi = { 0 };
//		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//		bmi.bmiHeader.biWidth = width;
//		bmi.bmiHeader.biHeight = -height; // Top-down
//		bmi.bmiHeader.biPlanes = 1;
//		bmi.bmiHeader.biBitCount = 32;
//		bmi.bmiHeader.biCompression = BI_RGB;
//
//		void* pBits;
//		HBITMAP hBitmap = ::CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);
//		if (hBitmap)
//		{
//			HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hdcMem, hBitmap);
//
//			// Fill with transparent black (alpha=0)
//			memset(pBits, 0, width * height * 4);
//
//			// Draw wireframe with XOR for visibility
//			HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
//			HPEN hOldPen = (HPEN)::SelectObject(hdcMem, hPen);
//			::SetROP2(hdcMem, R2_NOTXORPEN); // Mimics DSTINVERT
//			HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdcMem, ::GetStockObject(NULL_BRUSH));
//
//			// Draw rectangle outline
//			::Rectangle(hdcMem, 0, 0, width, height);
//
//			::SelectObject(hdcMem, hOldPen);
//			::SelectObject(hdcMem, hOldBrush);
//			::DeleteObject(hPen);
//
//			// Update layered window
//			POINT ptSrc = { 0, 0 };
//			SIZE size = { width, height };
//			BLENDFUNCTION blend = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
//			POINT ptDst = { rect.left, rect.top };
//			::GetWindowRect(hwnd, &rect);
//			ptDst.x = rect.left;
//			ptDst.y = rect.top;
//
//			::UpdateLayeredWindow(hwnd, hdc, &ptDst, &size, hdcMem, &ptSrc, 0, &blend, ULW_ALPHA);
//
//			::SelectObject(hdcMem, hOldBitmap);
//			::DeleteObject(hBitmap);
//		}
//		::DeleteDC(hdcMem);
//		::EndPaint(hwnd, &ps);
//		return 0;
//	}
//	case WM_ERASEBKGND:
//		return TRUE; // No background erase
//	case WM_DESTROY:
//		g_hWireboxWnd = NULL;
//		return 0;
//	}
//	return ::DefWindowProc(hwnd, msg, wParam, lParam);
//}

//LRESULT CALLBACK WireboxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
//{
//	switch (msg)
//	{
//	case WM_PAINT:
//	{
//		PAINTSTRUCT ps;
//		HDC hdc = ::BeginPaint(hwnd, &ps);
//		RECT rect;
//		::GetClientRect(hwnd, &rect);
//
//		ATLTRACE("WireboxWndProc: WM_PAINT, rect = LT(%d,%d) RB(%d,%d)\n",
//			rect.left, rect.top, rect.right, rect.bottom);
//
//		// Draw wireframe with XOR for high contrast
//		HPEN hPen = ::CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
//		HPEN hOldPen = (HPEN)::SelectObject(hdc, hPen);
//		::SetROP2(hdc, R2_NOTXORPEN); // Mimics DSTINVERT
//		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));
//
//		// Draw rectangle outline
//		::Rectangle(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top);
//
//		::SelectObject(hdc, hOldPen);
//		::SelectObject(hdc, hOldBrush);
//		::DeleteObject(hPen);
//
//		::EndPaint(hwnd, &ps);
//		return 0;
//	}
//	case WM_ERASEBKGND:
//		ATLTRACE("WireboxWndProc: WM_ERASEBKGND\n");
//		return TRUE; // Transparent background
//	case WM_DESTROY:
//		ATLTRACE("WireboxWndProc: WM_DESTROY\n");
//		g_hWireboxWnd = NULL;
//		return 0;
//	}
//	return ::DefWindowProc(hwnd, msg, wParam, lParam);
//}

static const COLORREF COLOR_KEY = RGB(255, 0, 255);  // Magenta as color key for transparency (unlikely screen color)
static const COLORREF LINE_COLOR = RGB(0, 0, 0);  // Black for outline (change to RGB(255,255,255) for white if needed)

LRESULT CALLBACK WireboxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = ::BeginPaint(hwnd, &ps);
		CRect rect;
		::GetClientRect(hwnd, &rect);

		ATLTRACE("WireboxWndProc: WM_PAINT, rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);

		// Fill with color key for transparency
		HBRUSH hBrush = ::CreateSolidBrush(COLOR_KEY);
		::FillRect(hdc, &rect, hBrush);
		::DeleteObject(hBrush);

		// Draw wireframe outline
		HPEN hPen = ::CreatePen(PS_SOLID, 1, LINE_COLOR);
		HPEN hOldPen = (HPEN)::SelectObject(hdc, hPen);
		//SetROP2(hdc, R2_NOTXORPEN); // Inverts pixels for visibility
		HBRUSH hOldBrush = (HBRUSH)::SelectObject(hdc, ::GetStockObject(NULL_BRUSH));

		CBrush brush(COLOR_KEY);
		::FillRect(hdc, rect, (HBRUSH)brush.GetSafeHandle());

		//::Rectangle(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

		PatBlt(hdc, rect.left, rect.top, rect.Width(), 1, DSTINVERT);
		PatBlt(hdc, rect.left, rect.bottom, 1, -(rect.Height()), DSTINVERT);
		PatBlt(hdc, rect.right - 1, rect.top, 1, rect.Height(), DSTINVERT);
		PatBlt(hdc, rect.right, rect.bottom - 1, -(rect.Width()), 1, DSTINVERT);

		::SelectObject(hdc, hOldPen);
		::SelectObject(hdc, hOldBrush);
		::DeleteObject(hPen);

		::EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_ERASEBKGND:
		ATLTRACE("WireboxWndProc: WM_ERASEBKGND\n");
		return 1; // Transparent background
	case WM_DESTROY:
		ATLTRACE("WireboxWndProc: WM_DESTROY\n");
		g_hWireboxWnd = NULL;
		return 0;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}
// Register a Window Message for the Taskbar Creation
// This message is passed to us if Explorer crashes and reloads
UINT CMainFrame::s_wmTaskbarCreated = RegisterWindowMessage(_T("TaskbarCreated"));

//#ifndef HMONITOR
//DECLARE_HANDLE(HMONITOR);
//#endif
#ifndef MONITOR_DEFAULTTONULL
#define MONITOR_DEFAULTTONULL	0x00000000
#endif

//typedef struct tagMONITORINFO
//{
//	DWORD cbSize;
//	RECT rcMonitor;
//	RECT rcWork;
//	DWORD dwFlags;
//} MONITORINFO, *PMONITORINFO;

//***********************************************
BOOL APIENTRY MonitorEnumProc(HMONITOR hMon, HDC hdcMon, PRECT pRectMon, LPARAM lParam)
{
	_ASSERTE(lParam);
	RECT *pRect = (RECT*)lParam;

	/*
	RECT rcVirtualScreen = { ::GetSystemMetrics(SM_XVIRTUALSCREEN), ::GetSystemMetrics(SM_YVIRTUALSCREEN), ::GetSystemMetrics(SM_CXVIRTUALSCREEN), ::GetSystemMetrics(SM_CYVIRTUALSCREEN) };

	MONITORINFO mi = { 0 };
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(hMon, &mi);

	BOOL isPrimary = (mi.dwFlags & MONITORINFOF_PRIMARY) != 0;

	int x = rcVirtualScreen.left > 0 ? rcVirtualScreen.left : -rcVirtualScreen.left;
	int y = rcVirtualScreen.top > 0 ? rcVirtualScreen.top : -rcVirtualScreen.top;

	RECT rect = { 0 };
	if (isPrimary)
	{
		rect.left = x;
		rect.top = y;
		rect.right = rect.left + (mi.rcMonitor.right - mi.rcMonitor.left);
		rect.bottom = rect.top + (mi.rcMonitor.bottom - mi.rcMonitor.top);
	}
	else
	{
		rect.left = x + mi.rcMonitor.left;
		rect.top = y + mi.rcMonitor.top;
		rect.right = rect.left + (mi.rcMonitor.right - mi.rcMonitor.left);
		rect.bottom = rect.top + (mi.rcMonitor.bottom - mi.rcMonitor.top);
	}
	UnionRect(pRect, pRect, &rect);
	*/

	//// Initial code
	//if(pRectMon->left < pRect->left)
	//	pRect->left = pRectMon->left;
	//if(pRectMon->right > pRect->right)
	//	pRect->right = pRectMon->right;
	//if(pRectMon->top < pRect->top)
	//	pRect->top = pRectMon->top;
	//if(pRectMon->bottom > pRect->bottom)
	//	pRect->bottom = pRectMon->bottom;

	UnionRect(pRect, pRect, pRectMon);

	return TRUE;
}

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SYSCOMMAND()
	ON_WM_GETMINMAXINFO()
	ON_COMMAND(ID_EDIT_COPY, OnCopy)
	ON_COMMAND(ID_MENU_EXIT, OnExit)
	ON_COMMAND(ID_MENU_RESTORE, OnRestore)
	ON_COMMAND(ID_MENU_REFRESH, OnRefresh)
	ON_COMMAND(ID_MENU_REFRESH_RATE, OnRefreshRate)
	ON_COMMAND(ID_MENU_GRID, OnGrid)
	ON_COMMAND(ID_MENU_NEGATIVE_GRID, OnNegativeGrid)
	ON_UPDATE_COMMAND_UI(ID_MENU_NEGATIVE_GRID, OnUpdateNegativeGrid)
	ON_COMMAND(ID_MENU_ABSOLUTE_NUMBERING, OnAbsoluteNumbering)
	ON_COMMAND(ID_MENU_SAVE_SETTINGS, OnSaveSettings)
	ON_COMMAND(ID_MENU_MINIMIZETOTRAY, OnMinimizeToTray)
	ON_COMMAND(ID_MENU_ALWAYSONTOP, OnAlwaysOnTop)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_CAPTURECHANGED()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_FILE_SAVE, OnSave)
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_NOTIFYICON, OnMouseMessage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//***********************************************
CMainFrame::CMainFrame()
{
	// Load the icons and cursors
	m_hIconLarge = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
	m_hIconSmall = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
	m_hCursorScan = (HCURSOR)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_LOOK_CUR), IMAGE_CURSOR, GetSystemMetrics(SM_CXCURSOR), GetSystemMetrics(SM_CYCURSOR), 0);
	m_hCursorPrev = NULL;

	m_bSaveSettings = !m_settings.Load();	// By default it's true, OnSaveSettings negates this
	m_settings.ToggleGrid();				// By default it's true, OnGrid negates this
	m_settings.ToggleAbsoluteNumbering();	// By default it's true, OnAbsoluteNumbering negates this
	m_settings.ToggleMinimizeToTray();		// By default it's true, OnMinimizeToTray negates this
	m_settings.ToggleAlwaysOnTop();			// By default it's true, OnAlwaysOnTop negates this

	CPoint point;
	CSize size;
	m_settings.GetWindowPos(point, size);

	m_menu.LoadMenu(IDR_MAINFRAME);

	CString strWndClass = AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW), (HBRUSH)(COLOR_WINDOW + 1), m_hIconSmall);
	CreateEx(WS_EX_TOPMOST, strWndClass, _T("ZoomIn"), WS_CAPTION | WS_OVERLAPPED | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX, point.x, point.y, size.cx, size.cy, NULL, m_menu.GetSafeHmenu());
	LoadAccelTable(MAKEINTRESOURCE(IDR_MAINFRAME));

	// After CreateEx call
	WNDCLASS wc = { 0 };
	wc.lpfnWndProc = WireboxWndProc;
	wc.hInstance = AfxGetInstanceHandle();
	wc.lpszClassName = WIREBOX_CLASS_NAME;
	wc.hbrBackground = NULL; // Transparent
	RegisterClass(&wc);

	SetIcon(m_hIconLarge, TRUE);		// Set big icon
	SetIcon(m_hIconSmall, FALSE);		// Set small icon

	CRect rectDesktop = GetDesktopSize();
	//m_nxScreenMax = rectDesktop.Width() - 1;
	//m_nyScreenMax = rectDesktop.Height() - 1;
	m_nxScreenMin = rectDesktop.left;
	m_nxScreenMax = rectDesktop.right - 1;
	m_nyScreenMin = rectDesktop.top;
	m_nyScreenMax = rectDesktop.bottom - 1;

	m_bIsLooking = false;
	m_ptZoom = CPoint(0, 0);
	m_nTimerID = 0;

	CreatePhysicalPalette();
	OnGrid();
	OnAbsoluteNumbering();
	OnSaveSettings();
	OnMinimizeToTray();
	OnAlwaysOnTop();

	if(m_settings.GetAutoRefresh())
		EnableAutoRefresh(true);

	CString strMsg;
	strMsg.Format(_T("%dx"), m_settings.GetZoom());
	m_wndStatusBar.SetPaneText(0, strMsg);
	m_wndScrollBar.SetScrollPos(m_settings.GetZoom(), TRUE);
}

//***********************************************
CMainFrame::~CMainFrame()
{
	if(m_bSaveSettings)
		m_settings.Save();
	else
		m_settings.Delete();

	if(m_hIconSmall != NULL)
		DestroyIcon(m_hIconSmall);
	if(m_hIconLarge != NULL)
		DestroyIcon(m_hIconLarge);
	if(m_hCursorScan != NULL)
		DestroyCursor(m_hCursorScan);

	if (g_hWireboxWnd)
		::DestroyWindow(g_hWireboxWnd);
}

//***********************************************
int CMainFrame::OnCreate(CREATESTRUCT *pCreateStruct)
{
	if(CFrameWnd::OnCreate(pCreateStruct) == -1)
		return -1;

	static UINT indicators[] =
	{
		ID_INDICATOR_ZOOM,
		ID_INDICATOR_CURSOR,
		ID_INDICATOR_RGB
	};

	if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT)))
		return -1;

	m_wndStatusBar.SetPaneInfo(0, ID_INDICATOR_ZOOM, SBPS_NORMAL, g_dpi.Scale(20));
	m_wndStatusBar.SetPaneInfo(1, ID_INDICATOR_CURSOR, SBPS_NORMAL, g_dpi.Scale(60));
	m_wndStatusBar.SetPaneInfo(2, ID_INDICATOR_RGB, SBPS_NORMAL, g_dpi.Scale(180));

	// Setup the scroll bar
	RECT rect;
	GetClientRect(&rect);
	rect.bottom -= GetStatusBarHeight();

	m_wndScrollBar.Create(WS_CHILD | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN, rect, this, IDC_VSCROLLBAR);
	m_wndScrollBar.SetScrollRange(MIN_ZOOM, MAX_ZOOM, FALSE);
//	m_wndScrollBar.SetScrollPos(m_settings.GetZoom(), FALSE);

	return 0;
}

//***********************************************
BOOL CMainFrame::DestroyWindow()
{
	if(m_bSaveSettings)
	{
		CRect rect;
		GetWindowRect(rect);
		CPoint point(rect.left, rect.top);
		CSize size(rect.Width(), rect.Height());
		m_settings.SetWindowPos(point, size);
	}

	if(m_nTimerID != 0)
		KillTimer(m_nTimerID);
	return CFrameWnd::DestroyWindow();
}

//***********************************************
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == SC_MINIMIZE && m_settings.GetMinimizeToTray())
	{
		ShowWindow(SW_HIDE);
		AddTrayIcon();
	}
	else
	{
		CFrameWnd::OnSysCommand(nID, lParam);
	}
}

//***********************************************
CRect CMainFrame::GetDesktopSize(void)
{
	CRect rect(0, 0, 0, 0);

	HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
	if(hUser32)
	{
		typedef BOOL (APIENTRY *PFNMONITORENUMPROC)(HMONITOR, HDC, PRECT, LPARAM);
		typedef BOOL (APIENTRY *PFNENUMDISPLAYMONITORS)(HDC, PRECT, PFNMONITORENUMPROC, LPARAM);

		PFNENUMDISPLAYMONITORS pfnEnumDisplayMonitors = (PFNENUMDISPLAYMONITORS)GetProcAddress(hUser32, "EnumDisplayMonitors");
		if(pfnEnumDisplayMonitors)
			pfnEnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&rect);
	}

	if(rect.Width() == 0 || rect.Height() == 0)
	{
		rect.right = GetSystemMetrics(SM_CXSCREEN);
		rect.bottom = GetSystemMetrics(SM_CYSCREEN);
	}

	return rect;
}

//***********************************************
void CMainFrame::CreatePhysicalPalette(void)
{
	PLOGPALETTE plp = (PLOGPALETTE)new BYTE[sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 0xFF)];
	plp->palVersion = 0x300;
	plp->palNumEntries = 0xFF;	// 256 palette entries

	for(int nIndex = 0; nIndex < 0xFF; nIndex++)
	{
		plp->palPalEntry[nIndex].peFlags = PC_EXPLICIT;
		plp->palPalEntry[nIndex].peRed = nIndex;
		plp->palPalEntry[nIndex].peGreen = 0;
		plp->palPalEntry[nIndex].peBlue = 0;
	}

	m_palPhysical.CreatePalette(plp);
	delete[] plp;
}

//***********************************************
void CMainFrame::OnGetMinMaxInfo(MINMAXINFO *pMMI)
{
	pMMI->ptMinTrackSize = CPoint(219, 196);
	CFrameWnd::OnGetMinMaxInfo(pMMI);
}

//***********************************************
void CMainFrame::OnSave()
{
	CFileDialog dlg(FALSE, _T(".bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Bitmap files (*.bmp)|*.bmp|All Files (*.*)|*.*||"), this);

	if(dlg.DoModal() == IDOK)
	{
		RECT rect;
		GetClientRect(&rect);
		rect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
		rect.bottom -= GetStatusBarHeight();

		CClientDC dc(this);
		DoTheZoom(&dc);
		CBitmapSave::Save(&dc, rect, dlg.GetPathName());
	}
}

//***********************************************
void CMainFrame::OnCopy()
{
	if(OpenClipboard())
	{
		EmptyClipboard();

		CClientDC dcSrc(this);
		CDC dcDest;

		CRect rect;
		GetClientRect(&rect);
		rect.right -= ::GetSystemMetrics(SM_CXVSCROLL);
		rect.bottom -= GetStatusBarHeight();

		int nWidth = rect.Width(), nHeight = rect.Height();

		HBITMAP hBmp;
		if(hBmp = CreateCompatibleBitmap(dcSrc.GetSafeHdc(), nWidth, nHeight))
		{
			if(dcDest.CreateCompatibleDC(&dcSrc))
			{
				// Calculate the dimensions of the bitmap and convert them to
				// tenths of a millimeter for setting the size with the
				// SetBitmapDimensionEx call.  This allows programs like Word
				// to retrieve the bitmap and know what size to display it as.
				SetBitmapDimensionEx(hBmp, (nWidth * MM10PERINCH) / dcSrc.GetDeviceCaps(LOGPIXELSX), (nHeight * MM10PERINCH) / dcSrc.GetDeviceCaps(LOGPIXELSY), NULL);

				dcDest.SelectObject(CBitmap::FromHandle(hBmp));
				dcDest.BitBlt(0, 0, nWidth, nHeight, &dcSrc, rect.left, rect.top, SRCCOPY);
				dcDest.DeleteDC();
				SetClipboardData(CF_BITMAP, hBmp);
			}
			else
			{
				DeleteObject(hBmp);
			}
		}
		CloseClipboard();
	}
}

//***********************************************
void CMainFrame::OnRefresh()
{
	DoTheZoom();
}

//***********************************************
void CMainFrame::OnRefreshRate()
{
	CRefreshRateDlg dlg;
	dlg.m_nRefreshInterval = m_settings.GetRefreshInterval();
	dlg.m_bEnableAutoRefresh = m_settings.GetAutoRefresh();

	if(dlg.DoModal() == IDOK)
	{
		m_settings.SetRefreshInterval(dlg.m_nRefreshInterval);
		m_settings.SetAutoRefresh(dlg.m_bEnableAutoRefresh == TRUE);

		// First disable the timer, then turn it back on.
		// This ensures the new timer rate is used.
		EnableAutoRefresh(false);
		if(m_settings.GetAutoRefresh())
			EnableAutoRefresh(true);
	}
}

//***********************************************
void CMainFrame::OnGrid()
{
	m_settings.ToggleGrid();
	DoTheZoom();

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | (m_settings.GetGrid() ? MF_CHECKED : MF_UNCHECKED);
	m_menu.CheckMenuItem(ID_MENU_GRID, nFlags);
}

//***********************************************
void CMainFrame::OnNegativeGrid()
{
	m_settings.ToggleNegativeGrid();
	DoTheZoom();

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | (m_settings.GetNegativeGrid() ? MF_CHECKED : MF_UNCHECKED);
	m_menu.CheckMenuItem(ID_MENU_NEGATIVE_GRID, nFlags);
}

//***********************************************
void CMainFrame::OnUpdateNegativeGrid(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(m_settings.GetGrid());
}

//***********************************************
void CMainFrame::OnAbsoluteNumbering()
{
	m_settings.ToggleAbsoluteNumbering();

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | (m_settings.GetAbsoluteNumbering() ? MF_CHECKED : MF_UNCHECKED);
	m_menu.CheckMenuItem(ID_MENU_ABSOLUTE_NUMBERING, nFlags);
}

//***********************************************
void CMainFrame::OnSaveSettings()
{
	m_bSaveSettings ^= true;

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | m_bSaveSettings ? MF_CHECKED : MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_MENU_SAVE_SETTINGS, nFlags);
}

//***********************************************
void CMainFrame::OnMinimizeToTray()
{
	m_settings.ToggleMinimizeToTray();

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | m_settings.GetMinimizeToTray() ? MF_CHECKED : MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_MENU_MINIMIZETOTRAY, nFlags);
}

//***********************************************
void CMainFrame::OnAlwaysOnTop()
{
	m_settings.ToggleAlwaysOnTop();
	::SetWindowPos(GetSafeHwnd(), m_settings.GetAlwaysOnTop() ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

	// Check/uncheck the menu item
	UINT nFlags = MF_BYCOMMAND | m_settings.GetAlwaysOnTop() ? MF_CHECKED : MF_UNCHECKED;
	m_menu.CheckMenuItem(ID_MENU_ALWAYSONTOP, nFlags);
}

//***********************************************
int CMainFrame::GetStatusBarHeight(void) const
{
	if(!m_wndStatusBar.IsWindowVisible())
		return 0;

	CRect rect;
	m_wndStatusBar.GetWindowRect(&rect);
	return rect.Height();
}

//***********************************************
void CMainFrame::OnWindowPosChanging(WINDOWPOS *pwndpos)
{
	if(!(GetKeyState(VK_CONTROL) & 0x8000))
	{
		CRect rectScreen(0, 0, 0, 0);

		HMODULE hUser32 = GetModuleHandle(_T("user32.dll"));
		if(hUser32)
		{
			typedef HMONITOR (APIENTRY *PFNMONITORFROMWINDOW)(HWND, DWORD);
			typedef BOOL (APIENTRY *PFNGETMONITORINFO)(HMONITOR, LPMONITORINFO);

			PFNMONITORFROMWINDOW pfnMonitorFromWindow = (PFNMONITORFROMWINDOW)GetProcAddress(hUser32, "MonitorFromWindow");
#if defined(UNICODE) || defined(_UNICODE)
			PFNGETMONITORINFO pfnGetMonitorInfo = (PFNGETMONITORINFO)GetProcAddress(hUser32, "GetMonitorInfoW");
#else
			PFNGETMONITORINFO pfnGetMonitorInfo = (PFNGETMONITORINFO)GetProcAddress(hUser32, "GetMonitorInfoA");
#endif
			if(pfnMonitorFromWindow && pfnGetMonitorInfo)
			{
				HMONITOR hMon = pfnMonitorFromWindow(GetSafeHwnd(), MONITOR_DEFAULTTONULL);
				if(hMon)
				{
					MONITORINFO mi = {sizeof(MONITORINFO), 0};
					pfnGetMonitorInfo(hMon, &mi);
					rectScreen = mi.rcWork;
				}
			}
		}

		if(rectScreen.Width() == 0 || rectScreen.Height() == 0)
			SystemParametersInfo(SPI_GETWORKAREA, NULL, &rectScreen, 0);

		if(ABS(pwndpos->x - rectScreen.left) <= X_SNAP)
			pwndpos->x = rectScreen.left;
		else if(ABS(pwndpos->x + pwndpos->cx - rectScreen.right) <= X_SNAP)
			pwndpos->x = rectScreen.right - pwndpos->cx;

		if(ABS(pwndpos->y - rectScreen.top) <= Y_SNAP)
			pwndpos->y = rectScreen.top;
		else if(ABS(pwndpos->y + pwndpos->cy - rectScreen.bottom) <= Y_SNAP)
			pwndpos->y = rectScreen.bottom - pwndpos->cy;
	}
}

//***********************************************
//void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	m_ptZoom.x = point.x;
//	m_ptZoom.y = point.y;
//
//	m_wndStatusBar.SetPaneText(1, _T(""));
//	m_wndStatusBar.SetPaneText(2, _T(""));
//
//	ClientToScreen(&m_ptZoom);
//	DrawZoomRect();
//	DoTheZoom();
//
//	SetCapture();
//	m_bIsLooking = true;
//	m_hCursorPrev = SetCursor(m_hCursorScan);
//
//	CFrameWnd::OnLButtonDown(nFlags, point);
//}
void CMainFrame::OnLButtonDown(UINT nFlags, CPoint point)
{
	// Use GetCursorPos for accurate screen coordinates
	POINT pt;
	GetCursorPos(&pt);
	m_ptZoom = pt;

	ATLTRACE("OnLButtonDown: m_ptZoom=(%d,%d)\n", m_ptZoom.x, m_ptZoom.y);

	m_wndStatusBar.SetPaneText(1, _T(""));
	m_wndStatusBar.SetPaneText(2, _T(""));

	DrawZoomRect();
	DoTheZoom();

	SetCapture();
	m_bIsLooking = true;
	m_hCursorPrev = SetCursor(m_hCursorScan);

	CFrameWnd::OnLButtonDown(nFlags, point);
}
//***********************************************
//void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	if(m_bIsLooking)
//	{
//		DrawZoomRect();
//		ReleaseCapture();
//		m_bIsLooking = false;
//
////		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
//		SetCursor(m_hCursorPrev);
//
//		// Redraw the whole screen
//		::InvalidateRect(NULL, NULL, FALSE);
//	}
//
//	CFrameWnd::OnLButtonUp(nFlags, point);
//}
void CMainFrame::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bIsLooking)
	{
		if (g_hWireboxWnd)
			::ShowWindow(g_hWireboxWnd, SW_HIDE);
		ReleaseCapture();
		m_bIsLooking = false;
		SetCursor(m_hCursorPrev);
		::InvalidateRect(NULL, NULL, FALSE);
	}
	CFrameWnd::OnLButtonUp(nFlags, point);
}
//***********************************************
//void CMainFrame::OnCaptureChanged(CWnd *pWnd)
//{
//	if(m_bIsLooking)
//	{
//		DrawZoomRect();
//		ReleaseCapture();
//		m_bIsLooking = false;
//
////		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
//		SetCursor(m_hCursorPrev);
//	}
//
//	CFrameWnd::OnCaptureChanged(pWnd);
//}
void CMainFrame::OnCaptureChanged(CWnd* pWnd)
{
	if (m_bIsLooking)
	{
		if (g_hWireboxWnd)
			::ShowWindow(g_hWireboxWnd, SW_HIDE);
		ReleaseCapture();
		m_bIsLooking = false;
		SetCursor(m_hCursorPrev);
	}
	CFrameWnd::OnCaptureChanged(pWnd);
}
//***********************************************
void CMainFrame::OnMouseMove(UINT nFlags, CPoint point)
{
	if(m_bIsLooking)
	{
		//ATLTRACE("OnMouseMove point=(%d,%d) m_ptZoom=(%d,%d)\n", point.x, point.y, m_ptZoom.x, m_ptZoom.y);
		//DrawZoomRect();
		//m_ptZoom.x = point.x;
		//m_ptZoom.y = point.y;
		//ClientToScreen(&m_ptZoom);
		//DrawZoomRect();
		//DoTheZoom();

		//POINT pt;
		//GetCursorPos(&pt);
		//ATLTRACE("OnMouseMove: point=(%d,%d) m_ptZoom=(%d,%d)\n", point.x, point.y, pt.x, pt.y);
		////if (g_hWireboxWnd)
		////	::ShowWindow(g_hWireboxWnd, SW_HIDE); // Hide to avoid flicker
		//m_ptZoom = pt;
		//DrawZoomRect();
		//DoTheZoom();

		POINT pt;
		GetCursorPos(&pt);
		ATLTRACE("OnMouseMove: point=(%d,%d) m_ptZoom=(%d,%d)\n", point.x, point.y, pt.x, pt.y);
		m_ptZoom = pt;
		DrawZoomRect();
		DoTheZoom();
	}
	else
	{
		CString strMsg;
		int nXRef = 0, nYRef = 0;

		if(m_settings.GetAbsoluteNumbering())
		{
			CPoint pt;
			pt.x = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
			pt.y = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
			nXRef = pt.x - m_nxZoomed / 2;
			nYRef = pt.y - m_nyZoomed / 2;
		}
		strMsg.Format(_T("(%3d,%3d)"), nXRef + (point.x / m_settings.GetZoom()), nYRef + (point.y / m_settings.GetZoom()));
		m_wndStatusBar.SetPaneText(1, strMsg);

		COLORREF color;
		CClientDC dc(this);
		color = GetPixel(dc.GetSafeHdc(), point.x, point.y);
		strMsg.Format(_T("RGB(%3d,%3d,%3d) #FF%02X%02X%02X"), GetRValue(color), GetGValue(color), GetBValue(color), GetRValue(color), GetGValue(color), GetBValue(color));
		m_wndStatusBar.SetPaneText(2, strMsg);
	}

	CFrameWnd::OnMouseMove(nFlags, point);
}

//***********************************************
void CMainFrame::OnPaint()
{
	CPaintDC dc(this);
	DoTheZoom(&dc);
}

//***********************************************
void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// Move the ScrollBar
	CRect rect;
	GetClientRect(&rect);
	rect.bottom -= GetStatusBarHeight();

	int nSBWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	m_wndScrollBar.SetWindowPos(NULL, rect.right - nSBWidth, rect.top, nSBWidth, rect.Height(), SWP_NOZORDER);

	CalcZoomedSize();
	DoTheZoom();
}

//***********************************************
void CMainFrame::CalcZoomedSize(void)
{
	RECT rect;
	GetClientRect(&rect);

	m_nxZoomed = ((rect.right - GetSystemMetrics(SM_CXVSCROLL)) / m_settings.GetZoom()) + 1;
	m_nyZoomed = ((rect.bottom - GetStatusBarHeight()) / m_settings.GetZoom()) + 1;
}

//***********************************************
void CMainFrame::DoTheZoom(CDC *pDC)
{
	if(!IsWindowVisible())
		return;

	CDC *pdcScreen = NULL;		// CDC ptr to entire screen
	CDC *pdcZoom = NULL;		// CDC ptr to client area of Zoomin window
	CPalette *ppalOld = NULL;	// CPallette ptr to palette of Zoomin window

	CDC dcMem;						// Memory CDC for off-screen drawing
	CBitmap bmpMem;					// Memory BMP for off-screen drawing
	CBitmap *pbmpMemOld = NULL;		// CBitmap ptr to old memory bitmap
	CPalette *ppalMemOld = NULL;	// CPalette ptr to old memory palette
	int nSBWidth = 0, nSBHeight = 0;

	// Establish a pointer to the screen's DC
	pdcZoom = (pDC == NULL) ? GetDC() : pDC;

	// Set clip areas to exclude status bar and scroll bar areas
	CRect rect;
	m_wndStatusBar.GetWindowRect(&rect);
	ScreenToClient(&rect);
	pdcZoom->ExcludeClipRect(&rect);

	// Get ScrollBar Width
	nSBWidth = ::GetSystemMetrics(SM_CXVSCROLL);
	GetClientRect(&rect);
	rect.left = rect.right - nSBWidth;
	pdcZoom->ExcludeClipRect(&rect);

	// Setup off-screen dc (Use an off-screen dc to avoid flickering)
	dcMem.CreateCompatibleDC(pdcZoom);
	bmpMem.CreateCompatibleBitmap(pdcZoom, m_settings.GetZoom() * m_nxZoomed, m_settings.GetZoom() * m_nyZoomed);
	pbmpMemOld = dcMem.SelectObject(&bmpMem);

	// If a palette has been created, set it up for use
	if((HPALETTE)m_palPhysical)
	{
		ppalOld = pdcZoom->SelectPalette(&m_palPhysical, FALSE);
		pdcZoom->RealizePalette();

		ppalMemOld = dcMem.SelectPalette(&m_palPhysical, FALSE);
		dcMem.RealizePalette();
	}

	// Establish point to use for bitmapping
	// The point must not include areas outside the screen dimensions.
	CPoint pt;
	pt.x = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
	pt.y = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));

	// Draw in the off-screen dc
	// Obtain pointer to screen dc
	pdcScreen = CDC::FromHandle(::GetDC(NULL));

	//bool wasVisible = false;
	//if (m_bIsLooking && g_hWireboxWnd)
	//{
	//	wasVisible = ::IsWindowVisible(g_hWireboxWnd);
	//	::ShowWindow(g_hWireboxWnd, SW_HIDE);
	//}

	// Copy screen image to memory dc
	dcMem.FloodFill(1, 1, RGB(0, 0, 0));
	dcMem.SetStretchBltMode(COLORONCOLOR);
	dcMem.StretchBlt(0, 0, m_settings.GetZoom() * m_nxZoomed, m_settings.GetZoom() * m_nyZoomed, pdcScreen, pt.x - m_nxZoomed / 2, pt.y - m_nyZoomed / 2, m_nxZoomed, m_nyZoomed, SRCCOPY);

	//if (m_bIsLooking && g_hWireboxWnd && wasVisible)
	//{
	//	::ShowWindow(g_hWireboxWnd, SW_SHOWNA);  // SW_SHOWNA avoids activation/focus change
	//}

	// Does the zoomed rect intersect with the client window?
	// If so fill the area with black to prevent scanning the scan area.
	CRect rectClip, rectZoom(CPoint(pt.x - m_nxZoomed / 2, pt.y - m_nyZoomed / 2), CSize(m_nxZoomed, m_nyZoomed));
	GetClientRect(&rect);
	rect.right -= nSBWidth;
	rect.bottom -= GetStatusBarHeight();
	ClientToScreen(&rect);
	if(rectClip.IntersectRect(rect, rectZoom))
	{
		CRect rectIntersect;
		rectIntersect.left = (long)(((float)(rectClip.left - rectZoom.left) / (float)m_nxZoomed) * rect.Width()) + rect.left;
		rectIntersect.right = rectIntersect.left + rectClip.Width() * m_settings.GetZoom();
		rectIntersect.top = (long)(((float)(rectClip.top - rectZoom.top) / (float)m_nyZoomed) * rect.Height()) + rect.top;
		rectIntersect.bottom = rectIntersect.top + rectClip.Height() * m_settings.GetZoom();

		ScreenToClient(&rectIntersect);
		dcMem.FillSolidRect(rectIntersect, RGB(0, 0, 0));
	}

	// Finished with screen dc
	ReleaseDC(pdcScreen);

	ATLTRACE("DoTheZoom rectZoom = LT(%d,%d) RB(%d,%d)\n", rectZoom.left, rectZoom.top, rectZoom.right, rectZoom.bottom);

	// Draw in gridlines
	if(m_settings.GetGrid())
		DoTheGrid(&dcMem);

	// Copy off-screen dc to windows dc
	SetStretchBltMode(pdcZoom->GetSafeHdc(), COLORONCOLOR);
	BitBlt(pdcZoom->GetSafeHdc(), 0, 0, m_settings.GetZoom() * m_nxZoomed, m_settings.GetZoom() * m_nyZoomed, dcMem.GetSafeHdc(), 0, 0, SRCCOPY);

	// Perform cleanup
	if(ppalOld != NULL)
	{
		pdcZoom->SelectPalette(ppalOld, FALSE);
		dcMem.SelectPalette(ppalMemOld, FALSE);
	}

	dcMem.SelectObject(pbmpMemOld);
	dcMem.DeleteDC();

	if(pDC == NULL)
		ReleaseDC(pdcZoom);
}

//***********************************************
void CMainFrame::DoTheGrid(CDC *pDC)
{
	_ASSERTE(pDC);

	if(m_settings.GetZoom() < 2)
		return;

	RECT rect;
	GetClientRect(&rect);

	if(m_settings.GetNegativeGrid())
	{	// Draw a "negative" grid
		for(int nX = 0; nX < m_nxZoomed; nX++)
			pDC->PatBlt(nX * m_settings.GetZoom(), 0, 1, rect.bottom, DSTINVERT);

		for(int nY = 0; nY < m_nyZoomed; nY++)
			pDC->PatBlt(0, nY * m_settings.GetZoom(), rect.right, 1, DSTINVERT);
	}
	else
	{	// Draw a normal black grid
		for(int nX = 0; nX < m_nxZoomed; nX++)
		{
			pDC->MoveTo(nX * m_settings.GetZoom(), rect.top);
			pDC->LineTo(nX * m_settings.GetZoom(), rect.bottom);
		}

		for(int nY = 0; nY < m_nyZoomed; nY++)
		{
			pDC->MoveTo(rect.left, nY * m_settings.GetZoom());
			pDC->LineTo(rect.right, nY * m_settings.GetZoom());
		}
	}
}

//***********************************************
//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	HDC hdc = ::GetDC(NULL);
//	PatBlt(hdc, rect.left, rect.top, rect.Width(), 1, DSTINVERT);
//	PatBlt(hdc, rect.left, rect.bottom, 1, -(rect.Height()), DSTINVERT);
//	PatBlt(hdc, rect.right - 1, rect.top, 1, rect.Height(), DSTINVERT);
//	PatBlt(hdc, rect.right, rect.bottom - 1, -(rect.Width()), 1, DSTINVERT);
//	::ReleaseDC(NULL, hdc);
//}

//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	//ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	// Get the monitor containing the zoom center
//	POINT ptCenter = { nX, nY };
//	HMONITOR hMonitor = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
//	if (hMonitor)
//	{
//		MONITORINFOEX mi;
//		mi.cbSize = sizeof(MONITORINFOEX);
//		if (GetMonitorInfo(hMonitor, (LPMONITORINFO)&mi))
//		{
//			// Create DC for this specific monitor
//			HDC hdc = CreateDC(mi.szDevice, NULL, NULL, NULL);
//			if (hdc)
//			{
//				// Adjust coordinates to be relative to the monitor's top-left (0,0)
//				int relLeft = rect.left - mi.rcMonitor.left;
//				int relTop = rect.top - mi.rcMonitor.top;
//				int relRight = rect.right - mi.rcMonitor.left;
//				int relBottom = rect.bottom - mi.rcMonitor.top;
//				int relWidth = rect.Width();
//				int relHeight = rect.Height();
//
//				// Draw the four sides with relative coordinates
//				PatBlt(hdc, relLeft, relTop, relWidth, 1, DSTINVERT);
//				PatBlt(hdc, relLeft, relBottom, 1, -relHeight, DSTINVERT);
//				PatBlt(hdc, relRight - 1, relTop, 1, relHeight, DSTINVERT);
//				PatBlt(hdc, relRight, relBottom - 1, -relWidth, 1, DSTINVERT);
//
//				//PatBlt(hdc, rect.left, rect.top, rect.Width(), 1, DSTINVERT);
//				//PatBlt(hdc, rect.left, rect.bottom, 1, -(rect.Height()), DSTINVERT);
//				//PatBlt(hdc, rect.right - 1, rect.top, 1, rect.Height(), DSTINVERT);
//				//PatBlt(hdc, rect.right, rect.bottom - 1, -(rect.Width()), 1, DSTINVERT);
//
//				ATLTRACE("DrawZoomRect display=%S rect = LT(%d,%d) RB(%d,%d)\n", mi.szDevice, relLeft, relTop, relRight, relBottom);
//
//				DeleteDC(hdc);
//			}
//		}
//	}
//}

//// Struct to pass data to the enum callback
//struct DrawWireboxData {
//	HMONITOR hTargetMonitor;
//	RECT rect;
//	BOOL isWirebox;  // True for DrawZoomRect, false for inverted bar in DoTheZoom
//};
//
//// Callback for EnumDisplayMonitors to draw on the target monitor's DC
//BOOL CALLBACK DrawWireboxEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM lParam) {
//	DrawWireboxData* pData = (DrawWireboxData*)lParam;
//	if (hMonitor == pData->hTargetMonitor && hdcMonitor != NULL) {
//		ATLTRACE("Drawing on monitor %p, bounds LT(%d,%d) RB(%d,%d), rect LT(%d,%d) RB(%d,%d)\n",
//			hMonitor, lprcMonitor->left, lprcMonitor->top, lprcMonitor->right, lprcMonitor->bottom,
//			pData->rect.left, pData->rect.top, pData->rect.right, pData->rect.bottom);
//
//		// Create a memory DC for drawing
//		HDC hdcMem = CreateCompatibleDC(hdcMonitor);
//		if (hdcMem) {
//			HBITMAP hBitmap = CreateCompatibleBitmap(hdcMonitor, pData->rect.right - pData->rect.left, pData->rect.bottom - pData->rect.top);
//			if (hBitmap) {
//				HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcMem, hBitmap);
//
//				// Copy the screen region to the memory DC
//				BitBlt(hdcMem, 0, 0, pData->rect.right - pData->rect.left, pData->rect.bottom - pData->rect.top,
//					hdcMonitor, pData->rect.left, pData->rect.top, SRCCOPY);
//
//				// Draw the wirebox or inverted bar with PatBlt in memory DC (relative to 0,0)
//				if (pData->isWirebox) {
//					PatBlt(hdcMem, 0, 0, pData->rect.right - pData->rect.left, 1, DSTINVERT);
//					PatBlt(hdcMem, 0, pData->rect.bottom - pData->rect.top - 1, 1, -(pData->rect.bottom - pData->rect.top), DSTINVERT);
//					PatBlt(hdcMem, pData->rect.right - pData->rect.left - 1, 0, 1, pData->rect.bottom - pData->rect.top, DSTINVERT);
//					PatBlt(hdcMem, pData->rect.right - pData->rect.left - 1, pData->rect.bottom - pData->rect.top - 1, -(pData->rect.right - pData->rect.left), 1, DSTINVERT);
//				}
//				else {
//					PatBlt(hdcMem, 0, 0, pData->rect.right - pData->rect.left, 8, DSTINVERT);
//				}
//
//				// Copy back to the monitor DC
//				BitBlt(hdcMonitor, pData->rect.left, pData->rect.top, pData->rect.right - pData->rect.left, pData->rect.bottom - pData->rect.top,
//					hdcMem, 0, 0, SRCCOPY);
//
//				SelectObject(hdcMem, hOldBitmap);
//				DeleteObject(hBitmap);
//			}
//			DeleteDC(hdcMem);
//		}
//	}
//	return TRUE;
//}
//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	// Get the monitor containing the zoom center
//	POINT ptCenter = { nX, nY };
//	HMONITOR hTargetMonitor = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
//	if (hTargetMonitor)
//	{
//		ATLTRACE("Target monitor for ptCenter (%d,%d) = %p\n", nX, nY, hTargetMonitor);
//
//		// Get desktop DC
//		HDC hdcDesktop = ::GetDC(NULL);
//		if (hdcDesktop)
//		{
//			// Prepare data for callback
//			DrawWireboxData data;
//			data.hTargetMonitor = hTargetMonitor;
//			data.rect = rect;
//			data.isWirebox = TRUE;
//
//			// Enumerate monitors with desktop DC
//			EnumDisplayMonitors(hdcDesktop, NULL, DrawWireboxEnumProc, (LPARAM)&data);
//			::ReleaseDC(NULL, hdcDesktop);
//		}
//		else
//		{
//			ATLTRACE("GetDC(NULL) failed, error = %d\n", GetLastError());
//		}
//	}
//	else
//	{
//		ATLTRACE("MonitorFromPoint failed for pt (%d,%d)\n", nX, nY);
//	}
//}

//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	// Get the monitor containing the zoom center
//	POINT ptCenter = { nX, nY };
//	HMONITOR hTargetMonitor = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
//	if (hTargetMonitor)
//	{
//		MONITORINFOEX mi;
//		mi.cbSize = sizeof(MONITORINFOEX);
//		if (GetMonitorInfo(hTargetMonitor, (LPMONITORINFO)&mi))
//		{
//			ATLTRACE("Target monitor %S, bounds LT(%d,%d) RB(%d,%d)\n",
//				mi.szDevice, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
//
//			// Get desktop DC
//			HDC hdcDesktop = ::GetDC(NULL);
//			if (hdcDesktop)
//			{
//				// Create clipping region for the target monitor
//				HRGN hRgn = CreateRectRgn(mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
//				if (hRgn)
//				{
//					// Select clipping region
//					if (SelectClipRgn(hdcDesktop, hRgn) != ERROR)
//					{
//						// Draw wirebox using virtual desktop coordinates
//						PatBlt(hdcDesktop, rect.left, rect.top, rect.Width(), 1, DSTINVERT);
//						PatBlt(hdcDesktop, rect.left, rect.bottom, 1, -(rect.Height()), DSTINVERT);
//						PatBlt(hdcDesktop, rect.right - 1, rect.top, 1, rect.Height(), DSTINVERT);
//						PatBlt(hdcDesktop, rect.right, rect.bottom - 1, -(rect.Width()), 1, DSTINVERT);
//					}
//					else
//					{
//						ATLTRACE("SelectClipRgn failed, error = %d\n", GetLastError());
//					}
//					DeleteObject(hRgn);
//				}
//				else
//				{
//					ATLTRACE("CreateRectRgn failed, error = %d\n", GetLastError());
//				}
//				::ReleaseDC(NULL, hdcDesktop);
//			}
//			else
//			{
//				ATLTRACE("GetDC(NULL) failed, error = %d\n", GetLastError());
//			}
//		}
//		else
//		{
//			ATLTRACE("GetMonitorInfo failed, error = %d\n", GetLastError());
//		}
//	}
//	else
//	{
//		ATLTRACE("MonitorFromPoint failed for pt (%d,%d), error = %d\n", nX, nY, GetLastError());
//	}
//}

////void CMainFrame::DrawZoomRect(void)
////{
////	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
////	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
////
////	CRect rect;
////	rect.left = nX - m_nxZoomed / 2;
////	rect.top = nY - m_nyZoomed / 2;
////	rect.right = rect.left + m_nxZoomed;
////	rect.bottom = rect.top + m_nyZoomed;
////	InflateRect(&rect, 1, 1);
////
////	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
////
////	// Verify monitor
////	POINT ptCenter = { nX, nY };
////	HMONITOR hTargetMonitor = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
////	if (hTargetMonitor)
////	{
////		MONITORINFOEX mi;
////		mi.cbSize = sizeof(MONITORINFOEX);
////		if (GetMonitorInfo(hTargetMonitor, (LPMONITORINFO)&mi))
////		{
////			ATLTRACE("Target monitor %S, bounds LT(%d,%d) RB(%d,%d)\n",
////				mi.szDevice, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
////		}
////		else
////		{
////			ATLTRACE("GetMonitorInfo failed, error = %d\n", GetLastError());
////		}
////	}
////	else
////	{
////		ATLTRACE("MonitorFromPoint failed for pt (%d,%d), error = %d\n", nX, nY, GetLastError());
////	}
////
////	// Create or move the wirebox window
////	if (!g_hWireboxWnd)
////	{
////		// Create layered window (transparent, topmost)
////		g_hWireboxWnd = CreateWindowEx(
////			WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
////			WIREBOX_CLASS_NAME,
////			_T("Wirebox"),
////			WS_POPUP | WS_VISIBLE,
////			rect.left, rect.top, rect.Width(), rect.Height(),
////			NULL, NULL, AfxGetInstanceHandle(), NULL);
////		if (g_hWireboxWnd)
////		{
////			// Set 50% opacity
////			::SetLayeredWindowAttributes(g_hWireboxWnd, 0, 64, LWA_ALPHA);
////		}
////		else
////		{
////			ATLTRACE("CreateWindowEx failed for wirebox, error = %d\n", GetLastError());
////		}
////	}
////	else
////	{
////		// Move and show the window
////		::SetWindowPos(g_hWireboxWnd, HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
////	}
////}

//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	// Verify monitor
//	POINT ptCenter = { nX, nY };
//	HMONITOR hTargetMonitor = MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
//	if (hTargetMonitor)
//	{
//		MONITORINFOEX mi;
//		mi.cbSize = sizeof(MONITORINFOEX);
//		if (GetMonitorInfo(hTargetMonitor, (LPMONITORINFO)&mi))
//		{
//			ATLTRACE("Target monitor %S, bounds LT(%d,%d) RB(%d,%d)\n",
//				mi.szDevice, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
//		}
//		else
//		{
//			ATLTRACE("GetMonitorInfo failed, error = %d\n", GetLastError());
//		}
//	}
//	else
//	{
//		ATLTRACE("MonitorFromPoint failed for pt (%d,%d), error = %d\n", nX, nY, GetLastError());
//	}
//
//	// Create or move the wirebox window
//	if (!g_hWireboxWnd)
//	{
//		// Create layered window (fully transparent background, topmost)
//		g_hWireboxWnd = CreateWindowEx(
//			WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
//			WIREBOX_CLASS_NAME,
//			_T("Wirebox"),
//			WS_POPUP | WS_VISIBLE,
//			rect.left, rect.top, rect.Width(), rect.Height(),
//			NULL, NULL, AfxGetInstanceHandle(), NULL);
//		if (g_hWireboxWnd)
//		{
//			// Set full opacity for the wireframe (alpha only affects drawn pixels)
//			::SetLayeredWindowAttributes(g_hWireboxWnd, 0, 255, LWA_ALPHA);
//			::UpdateWindow(g_hWireboxWnd); // Force initial paint
//		}
//		else
//		{
//			ATLTRACE("CreateWindowEx failed for wirebox, error = %d\n", GetLastError());
//		}
//	}
//	else
//	{
//		// Move the window (no redraw needed, WM_PAINT handles it)
//		::SetWindowPos(g_hWireboxWnd, HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
//		::InvalidateRect(g_hWireboxWnd, NULL, FALSE); // Force repaint
//		::UpdateWindow(g_hWireboxWnd);
//	}
//}

//void CMainFrame::DrawZoomRect(void)
//{
//	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
//	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));
//
//	CRect rect;
//	rect.left = nX - m_nxZoomed / 2;
//	rect.top = nY - m_nyZoomed / 2;
//	rect.right = rect.left + m_nxZoomed;
//	rect.bottom = rect.top + m_nyZoomed;
//	InflateRect(&rect, 1, 1);
//
//	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);
//
//	// Verify monitor
//	POINT ptCenter = { nX, nY };
//	HMONITOR hTargetMonitor = ::MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
//	if (hTargetMonitor)
//	{
//		MONITORINFOEX mi;
//		mi.cbSize = sizeof(MONITORINFOEX);
//		if (::GetMonitorInfo(hTargetMonitor, (LPMONITORINFO)&mi))
//		{
//			ATLTRACE("Target monitor %S, bounds LT(%d,%d) RB(%d,%d)\n",
//				mi.szDevice, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
//		}
//		else
//		{
//			ATLTRACE("GetMonitorInfo failed, error = %d\n", ::GetLastError());
//		}
//	}
//	else
//	{
//		ATLTRACE("MonitorFromPoint failed for pt (%d,%d), error = %d\n", nX, nY, ::GetLastError());
//	}
//
//	// Create or move the wirebox window
//	if (!g_hWireboxWnd)
//	{
//		// Create layered window (fully transparent background, topmost)
//		g_hWireboxWnd = ::CreateWindowEx(
//			WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
//			WIREBOX_CLASS_NAME,
//			_T("Wirebox"),
//			WS_POPUP | WS_VISIBLE,
//			rect.left, rect.top, rect.Width(), rect.Height(),
//			NULL, NULL, AfxGetInstanceHandle(), NULL);
//		if (g_hWireboxWnd)
//		{
//			// Set full opacity for drawn pixels, relying on bitmap alpha
//			::SetLayeredWindowAttributes(g_hWireboxWnd, 0, 255, LWA_ALPHA);
//			::UpdateWindow(g_hWireboxWnd);
//		}
//		else
//		{
//			ATLTRACE("CreateWindowEx failed for wirebox, error = %d\n", ::GetLastError());
//		}
//	}
//	else
//	{
//		// Move and resize the window
//		::SetWindowPos(g_hWireboxWnd, HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
//		::InvalidateRect(g_hWireboxWnd, NULL, FALSE);
//		::UpdateWindow(g_hWireboxWnd);
//	}
//}

void CMainFrame::DrawZoomRect(void)
{
	int nX = BOUND(m_ptZoom.x, m_nxScreenMin + (m_nxZoomed / 2), m_nxScreenMax - (m_nxZoomed / 2));
	int nY = BOUND(m_ptZoom.y, m_nyScreenMin + (m_nyZoomed / 2), m_nyScreenMax - (m_nyZoomed / 2));

	CRect rect;
	rect.left = nX - m_nxZoomed / 2;
	rect.top = nY - m_nyZoomed / 2;
	rect.right = rect.left + m_nxZoomed;
	rect.bottom = rect.top + m_nyZoomed;
	InflateRect(&rect, 1, 1);

	ATLTRACE("DrawZoomRect rect = LT(%d,%d) RB(%d,%d)\n", rect.left, rect.top, rect.right, rect.bottom);

	// Verify monitor
	POINT ptCenter = { nX, nY };
	HMONITOR hTargetMonitor = ::MonitorFromPoint(ptCenter, MONITOR_DEFAULTTONEAREST);
	if (hTargetMonitor)
	{
		MONITORINFOEX mi;
		mi.cbSize = sizeof(MONITORINFOEX);
		if (::GetMonitorInfo(hTargetMonitor, (LPMONITORINFO)&mi))
		{
			ATLTRACE("Target monitor %S, bounds LT(%d,%d) RB(%d,%d)\n",
				mi.szDevice, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right, mi.rcMonitor.bottom);
		}
		else
		{
			ATLTRACE("GetMonitorInfo failed, error = %d\n", ::GetLastError());
		}
	}
	else
	{
		ATLTRACE("MonitorFromPoint failed for pt (%d,%d), error = %d\n", nX, nY, ::GetLastError());
	}

	// Create or move the wirebox window
	if (!g_hWireboxWnd)
	{
		g_hWireboxWnd = ::CreateWindowEx(
			WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TRANSPARENT,
			WIREBOX_CLASS_NAME,
			_T("Wirebox"),
			WS_POPUP | WS_VISIBLE,
			rect.left, rect.top, rect.Width(), rect.Height(),
			NULL, NULL, AfxGetInstanceHandle(), NULL);
		if (g_hWireboxWnd)
		{
			ATLTRACE("Wirebox window created, HWND = %p, pos LT(%d,%d) size (%d,%d)\n",
				g_hWireboxWnd, rect.left, rect.top, rect.Width(), rect.Height());

			// Set color key for transparency
			::SetLayeredWindowAttributes(g_hWireboxWnd, COLOR_KEY, 0, LWA_COLORKEY);
			::UpdateWindow(g_hWireboxWnd);
		}
		else
		{
			ATLTRACE("CreateWindowEx failed for wirebox, error = %d\n", ::GetLastError());
		}
	}
	else
	{
		ATLTRACE("Moving wirebox window, HWND = %p, pos LT(%d,%d) size (%d,%d)\n",
			g_hWireboxWnd, rect.left, rect.top, rect.Width(), rect.Height());
		::SetWindowPos(g_hWireboxWnd, HWND_TOPMOST, rect.left, rect.top, rect.Width(), rect.Height(), SWP_NOACTIVATE | SWP_SHOWWINDOW);
		::InvalidateRect(g_hWireboxWnd, NULL, TRUE);
		::UpdateWindow(g_hWireboxWnd);
	}
}
//***********************************************
void CMainFrame::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar *pScrollBar)
{
	switch(nSBCode)
	{
	case SB_LINEDOWN:
		m_settings.IncreaseZoom();
		break;

	case SB_LINEUP:
		m_settings.DecreaseZoom();
		break;

	case SB_PAGEDOWN:
		m_settings.IncreaseZoom(2);
		break;

	case SB_PAGEUP:
		m_settings.DecreaseZoom(2);
		break;

	case SB_THUMBPOSITION:
	case SB_THUMBTRACK:
		m_settings.SetZoom(nPos);
		break;
	}

	m_wndScrollBar.SetScrollPos(m_settings.GetZoom(), TRUE);

	CString strMsg;
	strMsg.Format(_T("%dx"), m_settings.GetZoom());
	m_wndStatusBar.SetPaneText(0, strMsg);

	CalcZoomedSize();
	DoTheZoom();

	CFrameWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

//***********************************************
void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bControlPressed;

	switch(nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		MoveView(nChar, (GetKeyState(VK_SHIFT) & 0x8000) ? true : false, (GetKeyState(VK_CONTROL) & 0x8000) ? true : false);
		break;
	case VK_ADD:
		bControlPressed = (GetKeyState(VK_CONTROL) & 0x8000) ? true : false;
		OnVScroll(bControlPressed ? SB_THUMBPOSITION : SB_LINEDOWN, MAX_ZOOM, &m_wndScrollBar);
		break;
	case VK_SUBTRACT:
		bControlPressed = (GetKeyState(VK_CONTROL) & 0x8000) ? true : false;
		OnVScroll(bControlPressed ? SB_THUMBPOSITION : SB_LINEUP, MIN_ZOOM, &m_wndScrollBar);
		break;
	}

	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

//***********************************************
void CMainFrame::MoveView(int nDirectionCode, bool bFast, bool bPeg)
{
	// If bFast is true move the view faster (more pixels)
	int nDelta = (bFast) ? 8 : 1;

	switch(nDirectionCode)
	{
	case VK_UP:
		if(bPeg)
			m_ptZoom.y = m_nyZoomed / 2;
		else
			m_ptZoom.y -= nDelta;
		m_ptZoom.y = BOUND(m_ptZoom.y, m_nyScreenMin, m_nyScreenMax);
		break;

	case VK_DOWN:
		if(bPeg)
			m_ptZoom.y = m_nyScreenMax - (m_nyZoomed / 2);
		else
			m_ptZoom.y += nDelta;
		m_ptZoom.y = BOUND(m_ptZoom.y, m_nyScreenMin, m_nyScreenMax);
		break;

	case VK_LEFT:
		if(bPeg)
			m_ptZoom.x = m_nxZoomed / 2;
		else
			m_ptZoom.x -= nDelta;
		m_ptZoom.x = BOUND(m_ptZoom.x, m_nxScreenMin, m_nxScreenMax);
		break;

	case VK_RIGHT:
		if(bPeg)
			m_ptZoom.x = m_nxScreenMax - (m_nxZoomed / 2);
		else
			m_ptZoom.x += nDelta;
		m_ptZoom.x = BOUND(m_ptZoom.x, m_nxScreenMin, m_nxScreenMax);
		break;
	}

	DoTheZoom();
}

//***********************************************
void CMainFrame::EnableAutoRefresh(const bool bEnable)
{
	if(bEnable)
	{	// Enable
		if(m_nTimerID == 0)
			m_nTimerID = SetTimer(TIMER_ID, m_settings.GetRefreshInterval() * 100, NULL);
	}
	else
	{	// Disable
		if(m_nTimerID != 0)
		{
			KillTimer(m_nTimerID);
			m_nTimerID = 0;
		}
	}
}

//***********************************************
void CMainFrame::OnTimer(UINT nIDEvent)
{
	DoTheZoom();
	CFrameWnd::OnTimer(nIDEvent);
}

//***********************************************
BOOL CMainFrame::OnEraseBkgnd(CDC *pDC)
{
	return TRUE;
}

//***********************************************
void CMainFrame::AddTrayIcon(void) const
{
	// Add the tray icon
	CString strTip; strTip.LoadString(IDS_SYSTRAY_TIP);
	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetSafeHwnd();
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = m_hIconSmall;
	nid.uCallbackMessage = WM_NOTIFYICON;
	lstrcpyn(nid.szTip, strTip, sizeof(nid.szTip));

	Shell_NotifyIcon(NIM_ADD, &nid);
}

//***********************************************
void CMainFrame::RemoveTrayIcon(void) const
{
	// Remove the tray icon
	NOTIFYICONDATA nid = {0};
	nid.cbSize = sizeof(NOTIFYICONDATA);
	nid.hWnd = GetSafeHwnd();

	Shell_NotifyIcon(NIM_DELETE, &nid);
}

//***********************************************
LRESULT CMainFrame::OnMouseMessage(WPARAM wParam, LPARAM lParam)
{
	// OnMouseMessage - processes callback messages for taskbar icons
	// wParam - first message parameter of the callback message
	// lParam - second message parameter of the callback message

	if(lParam == WM_LBUTTONDBLCLK)
	{
		OnRestore();
	}
	else if(lParam == WM_RBUTTONUP)
	{
		CMenu menu; menu.LoadMenu(IDR_POPUP);
		CMenu *pPopup = menu.GetSubMenu(0);

		SetMenuDefaultItem(pPopup->GetSafeHmenu(), ID_MENU_RESTORE, FALSE);

		POINT pt;
		GetCursorPos(&pt);
		SetForegroundWindow();
		pPopup->TrackPopupMenu(TPM_RIGHTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, this);
		PostMessage(WM_NULL, 0, 0);
	}

	return 0;
}

//***********************************************
void CMainFrame::OnExit(void)
{
	RemoveTrayIcon();
	PostMessage(WM_CLOSE);
}

//***********************************************
void CMainFrame::OnRestore(void)
{
	RemoveTrayIcon();
	ShowWindow(SW_SHOW);
}
