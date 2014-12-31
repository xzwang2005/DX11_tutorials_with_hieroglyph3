// Study_01View.h : interface of the CStudy_01View class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include <gdiplus.h>

class CStudy_01View : public CWindowImpl<CStudy_01View>
{
public:
	DECLARE_WND_CLASS(NULL)

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP(CStudy_01View)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CPaintDC dc(m_hWnd);

		//TODO: Add your drawing code here
		using namespace Gdiplus;
		Graphics graphics(dc.m_hDC);
		graphics.SetTextRenderingHint(TextRenderingHintSingleBitPerPixelGridFit);

		WCHAR line[] = L"Sample Text";

		Font myFont(L"Arial", 16);
		PointF origin(0.0f, 0.0f);
		SolidBrush blackBrush(Color(255, 0, 0, 0));

		graphics.DrawString(
			line,
			12,
			&myFont,
			origin,
			&blackBrush);

		return 0;
	}
};
