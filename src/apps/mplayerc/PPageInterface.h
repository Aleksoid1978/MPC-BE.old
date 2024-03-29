/*
 * (C) 2003-2006 Gabest
 * (C) 2006-2014 see Authors.txt
 *
 * This file is part of MPC-BE.
 *
 * MPC-BE is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-BE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#pragma once

#include "PPageBase.h"
#include "StaticLink.h"


// CPPageInterface dialog

class CPPageInterface : public CPPageBase
{
	DECLARE_DYNAMIC(CPPageInterface)

public:
	CPPageInterface();
	virtual ~CPPageInterface();

	enum { IDD = IDD_PPAGEINTERFACE };
	BOOL m_fDisableXPToolbars;
	CButton m_fDisableXPToolbarsCtrl;
	int m_nThemeBrightness;
	int m_nThemeBrightness_Old;
	int m_nThemeRed;
	int m_nThemeGreen;
	int m_nThemeBlue;
	int m_nThemeRed_Old;
	int m_nThemeGreen_Old;
	int m_nThemeBlue_Old;
	int m_nOSDTransparent;
	int m_nOSDTransparent_Old;
	BOOL m_fFileNameOnSeekBar;
	CSliderCtrl m_ThemeBrightnessCtrl;
	CSliderCtrl m_ThemeRedCtrl;
	CSliderCtrl m_ThemeGreenCtrl;
	CSliderCtrl m_ThemeBlueCtrl;
	CSliderCtrl m_OSDTransparentCtrl;
	int m_OSDBorder;
	int m_OSDBorder_Old;
	CSpinButtonCtrl m_OSDBorderCtrl;
	int m_clrFaceABGR;
	int m_clrOutlineABGR;
	int m_clrFontABGR;
	int m_clrGrad1ABGR;
	int m_clrGrad2ABGR;

	BOOL m_fUseWin7TaskBar;
	BOOL m_fUseTimeTooltip;
	CComboBox m_TimeTooltipPosition;
	CComboBox m_FontSize;
	CComboBox m_FontType;
	int m_OSD_Size;
	CString	m_OSD_Font;
	BOOL m_fSmartSeek;
	BOOL m_fChapterMarker;
	BOOL m_fFlybar;
	BOOL m_fFontShadow;
	BOOL m_fFontShadow_Old;
	BOOL m_fFontAA;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();
	void OnCancel();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg BOOL OnToolTipNotify(UINT id, NMHDR* pNMH, LRESULT* pResult);
	afx_msg void OnUpdateCheck3(CCmdUI* pCmdUI);
	afx_msg void OnCheckShadow();
	afx_msg void OnCheckAA();
	afx_msg void OnUpdateOSDBorder(CCmdUI* pCmdUI);
	afx_msg void OnClickClrDefault();
	afx_msg void OnClickClrFace();
	afx_msg void OnClickClrOutline();
	afx_msg void OnClickClrFont();
	afx_msg void OnClickClrGrad1();
	afx_msg void OnClickClrGrad2();
	afx_msg void OnCustomDrawBtns(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUseTimeTooltipClicked();
	afx_msg void OnChngOSDCombo();
	afx_msg void OnUpdateThemeBrightness(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThemeRed(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThemeGreen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateThemeBlue(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOSDTransparent(CCmdUI* pCmdUI);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnThemeChange();
};
