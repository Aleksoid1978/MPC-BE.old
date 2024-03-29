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

#include "stdafx.h"
#include "PPageSheet.h"
#include "SettingsDefines.h"

// CPPageSheet

IMPLEMENT_DYNAMIC(CPPageSheet, CTreePropSheet)

CPPageSheet::CPPageSheet(LPCTSTR pszCaption, IFilterGraph* pFG, CWnd* pParentWnd, UINT idPage)
	: CTreePropSheet(pszCaption, pParentWnd, 0)
	, m_audioswitcher(pFG)
	, m_bLockPage(false)
{
	int tree_width = 210;
	HDC hdc = ::GetDC(NULL);
	if (hdc) {
		tree_width = MulDiv(tree_width, GetDeviceCaps(hdc, LOGPIXELSX), 96);
		::ReleaseDC(NULL, hdc);
	}
	SetTreeWidth(tree_width);

	AddPage(&m_player);
	AddPage(&m_formats);
	AddPage(&m_acceltbl);
	AddPage(&m_logo);
	AddPage(&m_webserver);
	AddPage(&m_playback);
	AddPage(&m_dvd);
	AddPage(&m_output);
	AddPage(&m_fullscreen);
	AddPage(&m_sync);
	AddPage(&m_tuner);
	AddPage(&m_youtube);
	AddPage(&m_internalfilters);
	AddPage(&m_audioswitcher);
	AddPage(&m_filterspriority);
	AddPage(&m_filtersperformance);
	AddPage(&m_externalfilters);
	AddPage(&m_subtitles);
	AddPage(&m_subMisc);
	AddPage(&m_substyle);
	AddPage(&m_interface);
	AddPage(&m_tweaks);
	AddPage(&m_misc);

	EnableStackedTabs(FALSE);

	SetTreeViewMode(TRUE, TRUE, FALSE);

	if (!idPage) {
		idPage = AfxGetAppSettings().nLastUsedPage;
	}

	if (idPage) {
		for (int i = 0; i < GetPageCount(); i++) {
			if (GetPage(i)->m_pPSP->pszTemplate == MAKEINTRESOURCE(idPage)) {
				SetActivePage(i);

				break;
			}
		}
	}
}

CPPageSheet::~CPPageSheet()
{
}

CTreeCtrl* CPPageSheet::CreatePageTreeObject()
{
	return DNew CTreePropSheetTreeCtrl();
}

BEGIN_MESSAGE_MAP(CPPageSheet, CTreePropSheet)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

BOOL CPPageSheet::OnInitDialog()
{
	BOOL bResult = __super::OnInitDialog();

	if (CTreeCtrl* pTree = GetPageTreeControl()) {
		for (HTREEITEM node = pTree->GetRootItem(); node; node = pTree->GetNextSiblingItem(node)) {
			pTree->Expand(node, TVE_EXPAND);
		}
	}

	if (m_bLockPage) {
		GetPageTreeControl()->EnableWindow (FALSE);
	}

	return bResult;
}

void CPPageSheet::OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/)
{
}

// CTreePropSheetTreeCtrl

IMPLEMENT_DYNAMIC(CTreePropSheetTreeCtrl, CTreeCtrl)
CTreePropSheetTreeCtrl::CTreePropSheetTreeCtrl()
{
}

CTreePropSheetTreeCtrl::~CTreePropSheetTreeCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreePropSheetTreeCtrl, CTreeCtrl)
END_MESSAGE_MAP()

// CTreePropSheetTreeCtrl message handlers

BOOL CTreePropSheetTreeCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	//cs.style &= ~TVS_LINESATROOT;

	return __super::PreCreateWindow(cs);
}
