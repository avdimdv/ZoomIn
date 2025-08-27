#pragma once

//---------------- _CDPI ------------ 
// Definition: relative pixel = 1 pixel at 96 DPI and scaled based on actual DPI.
// On Windows dpiX and dpiY are always equal
#ifndef USER_DEFAULT_SCREEN_DPI
#define USER_DEFAULT_SCREEN_DPI 96
#endif

class _CDPI
{
public:
	_CDPI() : _fInitialized(false), _dpi(USER_DEFAULT_SCREEN_DPI) { }

	// Get screen DPI.
	int GetDPI() { _Init(); return _dpi; }

	// Convert between raw pixels and relative pixels.
	int Scale(int x) { _Init(); return ::MulDiv(x, _dpi, USER_DEFAULT_SCREEN_DPI); }
	float ScaleF(float x) { _Init(); return x * float(_dpi) / float(USER_DEFAULT_SCREEN_DPI); }
	int Unscale(int x) { _Init(); return ::MulDiv(x, USER_DEFAULT_SCREEN_DPI, _dpi); }
	float UnscaleF(float x) { _Init(); return x * float(USER_DEFAULT_SCREEN_DPI) / float(_dpi); }

	// Determine the screen dimensions in relative pixels.
	int ScaledScreenWidth() { return _ScaledSystemMetric(SM_CXSCREEN); }
	int ScaledScreenHeight() { return _ScaledSystemMetric(SM_CYSCREEN); }

	// Scale rectangle from raw pixels to relative pixels.
	void ScaleRect(__inout RECT *pRect)
	{
		pRect->left = Scale(pRect->left);
		pRect->right = Scale(pRect->right);
		pRect->top = Scale(pRect->top);
		pRect->bottom = Scale(pRect->bottom);
	}
	void UnscaleRect(__inout RECT *pRect)
	{
		pRect->left = Unscale(pRect->left);
		pRect->right = Unscale(pRect->right);
		pRect->top = Unscale(pRect->top);
		pRect->bottom = Unscale(pRect->bottom);
	}

	// Scale SIZE from raw pixels to relative pixels.
	void ScaleSize(__inout SIZE *pSize)
	{
		pSize->cx = Scale(pSize->cx);
		pSize->cy = Scale(pSize->cy);
	}
	void UnscaleSize(__inout SIZE *pSize)
	{
		pSize->cx = Unscale(pSize->cx);
		pSize->cy = Unscale(pSize->cy);
	}

	// Determine if screen resolution meets minimum requirements in relative
	// pixels.
	bool IsResolutionAtLeast(int cxMin, int cyMin) 
	{ 
		return (ScaledScreenWidth() >= cxMin) && (ScaledScreenHeight() >= cyMin); 
	}

	// Convert a point size (1/72 of an inch) to raw pixels.
	int PointsToPixels(int pt) { _Init(); return ::MulDiv(pt, _dpi, 72); }

	// Invalidate any cached metrics.
	void Invalidate() { _fInitialized = false; }

	bool NeedScaling() { _Init(); return (_dpi != USER_DEFAULT_SCREEN_DPI); }

private:
	void _Init()
	{
		if (!_fInitialized)
		{
			HDC hdc = ::GetDC(NULL);
			if (hdc)
			{
				_dpi = ::GetDeviceCaps(hdc, LOGPIXELSX);
				::ReleaseDC(NULL, hdc);
			}
			_fInitialized = true;
		}
	}

	int _ScaledSystemMetric(int nIndex) 
	{ 
		_Init(); 
		return ::MulDiv(::GetSystemMetrics(nIndex), 96, USER_DEFAULT_SCREEN_DPI); 
	}

private:
	bool _fInitialized;

	int _dpi;
};

