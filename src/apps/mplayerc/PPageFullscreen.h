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
#include "PlayerListCtrl.h"

// CPPageFullscreen dialog

class CPPageFullscreen : public CPPageBase
{
	DECLARE_DYNAMIC(CPPageFullscreen)

	//	private:
	CAtlArray<dispmode> m_dms;
	CAtlArray<CString> sl;
	CStringArray m_MonitorDisplayNames, m_MonitorDeviceName;

public:
	CPPageFullscreen();
	virtual ~CPPageFullscreen();

	BOOL m_launchfullscreen;
	BOOL m_fSetFullscreenRes;
	BOOL m_fSetDefault;
	BOOL m_fSetGlobal;

	CPlayerListCtrl m_list;
	enum {COL_Z, COL_VFR_F, COL_VFR_T, COL_SRR};

	AChFR m_AutoChangeFullscrRes;
	CStringW m_f_hmonitor;
	CStringW m_f_hmonitorID;
	int iCurMon;

	int m_iMonitorType;
	CComboBox m_iMonitorTypeCtrl;

	BOOL m_iShowBarsWhenFullScreen;
	int m_nShowBarsWhenFullScreenTimeOut;
	BOOL m_fExitFullScreenAtTheEnd, m_fExitFullScreenAtFocusLost;
	CSpinButtonCtrl m_nTimeOutCtrl;
	BOOL m_fRestoreResAfterExit;

	int m_iSel;
	int m_iSeldm[MaxFpsCount];

	enum { IDD = IDD_PPAGEFULLSCREEN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	virtual BOOL OnApply();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnUpdateList(CCmdUI* pCmdUI);
	afx_msg void OnNMClickList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBeginlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDolabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnCheckChangeList();
	afx_msg void OnUpdateApplyDefault(CCmdUI* pCmdUI);

	afx_msg void OnUpdateSetGlobal(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFullScrComboCtrl(CCmdUI* pCmdUI);
	afx_msg void OnUpdateSetFullscreenRes();
	afx_msg void OnUpdateLaunchfullscreen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowBarsWhenFullScreen(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExitFullScreenAtTheEnd(CCmdUI* pCmdUI);
	afx_msg void OnUpdateShowBarsWhenFullScreenTimeOut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatic1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStatic2(CCmdUI* pCmdUI);
	afx_msg void OnUpdateExitFullScreenAtFocusLost(CCmdUI* pCmdUI);

	afx_msg void OnUpdateFullScrCombo();
	afx_msg void OnUpdateTimeout(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRestoreRes(CCmdUI* pCmdUI);
	afx_msg void OnRemove();
	afx_msg void OnUpdateRemove(CCmdUI* pCmdUI);
	afx_msg void OnAdd();
	afx_msg void OnUpdateAdd(CCmdUI* pCmdUI);
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnUpdateUp(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDown(CCmdUI* pCmdUI);

	void ReindexList();
	void ReindexListSubItem();
	void GetCurDispModeString(CString& strMode);
	void ModesUpdate();
};
