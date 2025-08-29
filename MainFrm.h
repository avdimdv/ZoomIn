#pragma once

#include "Settings.h"

class CMainFrame;

class CWireboxWnd : public CWnd
{
	DECLARE_DYNAMIC(CWireboxWnd)
public:
	CWireboxWnd(CMainFrame* pMainFrame);
	virtual ~CWireboxWnd();

protected:
	CMainFrame* m_pMainFrame;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CMainFrame : public CFrameWnd
{
	friend class CWireboxWnd;

protected: // create from serialization only
	DECLARE_DYNCREATE(CMainFrame)

// Attributes
public:

// Operations
public:
	CMainFrame();
	virtual ~CMainFrame();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

#ifdef _DEBUG
	virtual void AssertValid() const {CFrameWnd::AssertValid();}
	virtual void Dump(CDumpContext &dc) const {CFrameWnd::Dump(dc);}
#endif

protected:
	HICON m_hIconLarge, m_hIconSmall;
	HCURSOR m_hCursorScan, m_hCursorPrev;
	CMenu m_menu;
	CStatusBar m_wndStatusBar;
	CScrollBar m_wndScrollBar;
	CPalette m_palPhysical;
	static UINT s_wmTaskbarCreated;
	CWireboxWnd m_wireboxWnd;
	int m_nWireboxLineWidth = 2;

	CSettings m_settings;
	bool m_bSaveSettings;

	int m_nxScreenMin;
	int m_nxScreenMax;		// Width of the screen (less 1)
	int m_nxZoomed;			// Client width in zoomed pixels
	int m_nyScreenMin;
	int m_nyScreenMax;		// Height of the screen (less 1)
	int m_nyZoomed;			// Client height in zoomed pixels
	bool m_bIsLooking;
	CPoint m_ptZoom;
	int m_nTimerID;

	CRect GetDesktopSize(void);
	void CreatePhysicalPalette(void);
	int GetStatusBarHeight(void) const;
	void CalcZoomedSize(void);
	void DrawZoomRect(void);
	void DoTheGrid(CDC *pDC);
	void DoTheZoom(CDC *pDC = NULL);
	void MoveView(int nDirectionCode, bool bFast, bool bPeg);
	void EnableAutoRefresh(const bool bEnable);

	void AddTrayIcon(void) const;
	void RemoveTrayIcon(void) const;

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(CREATESTRUCT *pCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO *pMMI);
	afx_msg void OnCopy();
	afx_msg void OnExit();
	afx_msg void OnRestore();
	afx_msg void OnRefresh();
	afx_msg void OnRefreshRate();
	afx_msg void OnGrid();
	afx_msg void OnNegativeGrid();
	afx_msg void OnUpdateNegativeGrid(CCmdUI *pCmdUI);
	afx_msg void OnAbsoluteNumbering();
	afx_msg void OnSaveSettings();
	afx_msg void OnAlwaysOnTop();
	afx_msg void OnWindowPosChanging(WINDOWPOS *pwndpos);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSave();
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
	afx_msg LRESULT OnMouseMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMinimizeToTray();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
