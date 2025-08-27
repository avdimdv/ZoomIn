#pragma once

#define MIN_ZOOM	1
#define MAX_ZOOM	32

// Returns x bound by min and max. The returned value is guaranteed between min and max.
#define BOUND(x,min,max)	((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

class CSettings
{
public:
	CSettings();
	CSettings(const CSettings &src);
	virtual ~CSettings();

	CSettings &operator=(const CSettings &src);

	void LoadDefaults(void);
	bool Load(void);
	void Save(void) const;
	void Delete(void) const;

	void GetWindowPos(CPoint &point, CSize &size) const {point = m_pt; size = m_size;}
	void SetWindowPos(const CPoint &point, const CSize &size) {m_pt = point; m_size = size;}

	int GetZoom(void) const {return m_nZoom;}
	void SetZoom(const int nZoom) {m_nZoom = BOUND(nZoom, MIN_ZOOM, MAX_ZOOM);}
	void IncreaseZoom(const int nIncrease = 1) {m_nZoom = BOUND(m_nZoom + nIncrease, MIN_ZOOM, MAX_ZOOM);}
	void DecreaseZoom(const int nDecrease = 1) {m_nZoom = BOUND(m_nZoom - nDecrease, MIN_ZOOM, MAX_ZOOM);}

	bool GetAutoRefresh(void) const {return m_bAutoRefresh;}
	void SetAutoRefresh(const bool bAutoRefresh) {m_bAutoRefresh = bAutoRefresh;}
	void ToggleAutoRefresh(void) {m_bAutoRefresh ^= true;}

	int GetRefreshInterval(void) const {return m_nRefreshInterval;}
	void SetRefreshInterval(const int nRefreshInterval) {m_nRefreshInterval = nRefreshInterval;}

	bool GetGrid(void) const {return m_bGrid;}
	void SetGrid(const bool bGrid) {m_bGrid = bGrid;}
	void ToggleGrid(void) {m_bGrid ^= true;}

	bool GetNegativeGrid(void) const {return m_bNegativeGrid;}
	void SetNegativeGrid(const bool bNegativeGrid) {m_bNegativeGrid = bNegativeGrid;}
	void ToggleNegativeGrid(void) {m_bNegativeGrid ^= true;}

	bool GetAbsoluteNumbering(void) const {return m_bAbsoluteNumbering;}
	void SetAbsoluteNumbering(const bool bAbsoluteNumbering) {m_bAbsoluteNumbering = bAbsoluteNumbering;}
	void ToggleAbsoluteNumbering(void) {m_bAbsoluteNumbering ^= true;}

	bool GetMinimizeToTray(void) const {return m_bMinimizeToTray;}
	void SetMinimizeToTray(const bool bMinimizeToTray) {m_bMinimizeToTray = bMinimizeToTray;}
	void ToggleMinimizeToTray(void) {m_bMinimizeToTray ^= true;}

	bool GetAlwaysOnTop(void) const {return m_bAlwaysOnTop;}
	void SetAlwaysOnTop(const bool bAlwaysOnTop) {m_bAlwaysOnTop = bAlwaysOnTop;}
	void ToggleAlwaysOnTop(void) {m_bAlwaysOnTop ^= true;}

protected:
	CPoint m_pt;
	CSize m_size;
	int m_nZoom;
	bool m_bAutoRefresh;
	int m_nRefreshInterval;
	bool m_bGrid;
	bool m_bNegativeGrid;
	bool m_bAbsoluteNumbering;
	bool m_bMinimizeToTray;
	bool m_bAlwaysOnTop;
};
