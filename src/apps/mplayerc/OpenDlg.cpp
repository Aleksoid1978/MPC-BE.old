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
#include <atlpath.h>
#include <shlobj.h>
#include <dlgs.h>
#include "OpenDlg.h"
#include "PlayerYouTube.h"
#include "MainFrm.h"
#include "../../DSUtil/Filehandle.h"

// COpenDlg dialog

//IMPLEMENT_DYNAMIC(COpenDlg, CResizableDialog)

COpenDlg::COpenDlg(CWnd* pParent /*=NULL*/)
	: CResizableDialog(COpenDlg::IDD, pParent)
	, m_fMultipleFiles(false)
	, m_fAppendPlaylist(FALSE)
{
	m_hIcon = (HICON)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);
}

COpenDlg::~COpenDlg()
{
	if (m_hIcon) {
		DestroyIcon(m_hIcon);
	}
}

void COpenDlg::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_COMBO1, m_mrucombo);
	DDX_CBString(pDX, IDC_COMBO1, m_path);
	DDX_Control(pDX, IDC_COMBO2, m_mrucombo2);
	DDX_CBString(pDX, IDC_COMBO2, m_path2);
	DDX_Control(pDX, IDC_STATIC1, m_label2);
	DDX_Check(pDX, IDC_CHECK1, m_fAppendPlaylist);
}

BEGIN_MESSAGE_MAP(COpenDlg, CResizableDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedBrowsebutton)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedBrowsebutton2)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_UPDATE_COMMAND_UI(IDC_STATIC1, OnUpdateDub)
	ON_UPDATE_COMMAND_UI(IDC_COMBO2, OnUpdateDub)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON2, OnUpdateDub)
	ON_UPDATE_COMMAND_UI(IDOK, OnUpdateOk)
END_MESSAGE_MAP()

// COpenDlg message handlers

BOOL COpenDlg::OnInitDialog()
{
	__super::OnInitDialog();

	AppSettings& s = AfxGetAppSettings();

	CRecentFileList& MRU = s.MRU;
	MRU.ReadList();
	m_mrucombo.ResetContent();

	for (int i = 0; i < MRU.GetSize(); i++)
		if (!MRU[i].IsEmpty()) {
			m_mrucombo.AddString(MRU[i]);
		}

	CorrectComboListWidth(m_mrucombo);

	CRecentFileList& MRUDub = s.MRUDub;
	MRUDub.ReadList();
	m_mrucombo2.ResetContent();

	for (int i = 0; i < MRUDub.GetSize(); i++)
		if (!MRUDub[i].IsEmpty()) {
			m_mrucombo2.AddString(MRUDub[i]);
		}

	CorrectComboListWidth(m_mrucombo2);

	if (m_mrucombo.GetCount() > 0) {
		m_mrucombo.SetCurSel(0);
	}

	if (::IsClipboardFormatAvailable(CF_UNICODETEXT) && ::OpenClipboard(m_hWnd)) {
		HGLOBAL hglb = ::GetClipboardData(CF_UNICODETEXT);
		if (hglb) {
			LPCTSTR pText = (LPCTSTR)::GlobalLock(hglb);
			if (pText) {
				if (AfxIsValidString(pText)) {
					CString tmpData(CString(pText).MakeLower());
					if (PlayerYouTubeCheck(tmpData) || PlayerYouTubePlaylistCheck(tmpData) || tmpData.Find(VIMEO_URL) != -1) {
						m_mrucombo.SetWindowTextW(pText);
					}
				}
				GlobalUnlock(hglb);
			}
		}
		CloseClipboard();
	}

	m_fns.RemoveAll();
	m_path = _T("");
	m_path2 = _T("");
	m_fMultipleFiles = false;
	m_fAppendPlaylist = FALSE;

	AddAnchor(m_mrucombo, TOP_LEFT, TOP_RIGHT);
	AddAnchor(m_mrucombo2, TOP_LEFT, TOP_RIGHT);
	AddAnchor(IDC_BUTTON1, TOP_RIGHT);
	AddAnchor(IDC_BUTTON2, TOP_RIGHT);
	AddAnchor(IDOK, TOP_RIGHT);
	AddAnchor(IDCANCEL, TOP_RIGHT);
	AddAnchor(IDC_STATIC1, TOP_LEFT, TOP_RIGHT);

	CRect r;
	GetWindowRect(r);
	CSize sr = r.Size();
	SetMinTrackSize(sr);
	sr.cx = 1000;
	SetMaxTrackSize(sr);

	if (m_hIcon != NULL) {
		CStatic *pStat = (CStatic*)GetDlgItem(IDC_MAINFRAME_ICON);
		pStat->SetIcon(m_hIcon);
	}

	return TRUE;
}

static CString GetFileName(CString str)
{
	CPath p = str;
	p.StripPath();

	return (LPCTSTR)p;
}

void COpenDlg::OnBnClickedBrowsebutton()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();

	CString filter;
	CAtlArray<CString> mask;
	s.m_Formats.GetFilter(filter, mask);

	DWORD dwFlags = OFN_EXPLORER|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_ALLOWMULTISELECT|OFN_ENABLEINCLUDENOTIFY|OFN_NOCHANGEDIR;

	if (!s.fKeepHistory) {
		dwFlags |= OFN_DONTADDTORECENT;
	}

	COpenFileDlg fd(mask, true, NULL, m_path, dwFlags, filter, this);

	if (fd.DoModal() != IDOK) {
		return;
	}

	m_fns.RemoveAll();

	POSITION pos = fd.GetStartPosition();
	while (pos) {
		/*
		CString str = fd.GetNextPathName(pos);
		POSITION insertpos = m_fns.GetTailPosition();
		while (insertpos && GetFileName(str).CompareNoCase(GetFileName(m_fns.GetAt(insertpos))) <= 0)
			m_fns.GetPrev(insertpos);
		if (!insertpos) m_fns.AddHead(str);
		else m_fns.InsertAfter(insertpos, str);
		*/
		m_fns.AddTail(fd.GetNextPathName(pos));
	}

	if (m_fns.GetCount() > 1
			|| m_fns.GetCount() == 1
			&& (m_fns.GetHead()[m_fns.GetHead().GetLength()-1] == '\\'
				|| m_fns.GetHead()[m_fns.GetHead().GetLength()-1] == '*')) {
		m_fMultipleFiles = true;
		EndDialog(IDOK);
		return;
	}

	m_mrucombo.SetWindowText(fd.GetPathName());
}

void COpenDlg::OnBnClickedBrowsebutton2()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();

	CString filter;
	CAtlArray<CString> mask;
	s.m_Formats.GetAudioFilter(filter, mask);

	DWORD dwFlags = OFN_EXPLORER|OFN_ENABLESIZING|OFN_HIDEREADONLY|OFN_ENABLEINCLUDENOTIFY|OFN_NOCHANGEDIR;

	if (!s.fKeepHistory) {
		dwFlags |= OFN_DONTADDTORECENT;
	}

	COpenFileDlg fd(mask, false, NULL, m_path2, dwFlags, filter, this);

	if (fd.DoModal() != IDOK) {
		return;
	}

	m_mrucombo2.SetWindowText(fd.GetPathName());
}

void COpenDlg::OnBnClickedOk()
{
	UpdateData();

	m_fns.RemoveAll();
	m_fns.AddTail(PlayerYouTubePlaylist(m_path, 0));

	if (m_mrucombo2.IsWindowEnabled()) {
		m_fns.AddTail(m_path2);

		if (::PathFileExists(m_path2)) {
			((CMainFrame*)AfxGetMainWnd())->AddAudioPathsAddons(m_path2);
		}
	}

	m_fMultipleFiles = false;

	OnOK();
}

void COpenDlg::OnUpdateDub(CCmdUI* pCmdUI)
{
	UpdateData();

	pCmdUI->Enable(AfxGetAppSettings().GetRtspEngine(m_path) == DirectShow
					&& ((CString(m_path).MakeLower().Find(_T("://"))) == -1));
}

void COpenDlg::OnUpdateOk(CCmdUI* pCmdUI)
{
	UpdateData();

	pCmdUI->Enable(!m_path.IsEmpty() || !m_path2.IsEmpty());
}

// COpenFileDlg

#define __DUMMY__ _T("*.*")

bool COpenFileDlg::m_fAllowDirSelection = false;
WNDPROC COpenFileDlg::m_wndProc = NULL;

IMPLEMENT_DYNAMIC(COpenFileDlg, CFileDialog)
COpenFileDlg::COpenFileDlg(CAtlArray<CString>& mask, bool fAllowDirSelection, LPCTSTR lpszDefExt, LPCTSTR lpszFileName,
						   DWORD dwFlags, LPCTSTR lpszFilter, CWnd* pParentWnd)
	: CFileDialog(TRUE, lpszDefExt, lpszFileName, dwFlags|OFN_NOVALIDATE, lpszFilter, pParentWnd, 0)
	, m_mask(mask)
{
	m_fAllowDirSelection = fAllowDirSelection;

	CString str(lpszFileName);

	if (str.IsEmpty()) {
		CRecentFileList& MRU = AfxGetAppSettings().MRU;
		MRU.ReadList();

		for (int i = 0; i < MRU.GetSize(); i++) {
			if (!MRU[i].IsEmpty()) {
				str = MRU[i];
				break;
			}
		}
	}

	if (str.Find(_T("://")) > 0) {
		str.Empty();
	}

	str = GetFolderOnly(str);

	int size = max(1000, str.GetLength() + 1);
	m_InitialDir = DNew TCHAR[size];
	memset(m_InitialDir, 0, size * sizeof(TCHAR));
	_tcscpy(m_InitialDir, str);
	m_pOFN->lpstrInitialDir = m_InitialDir;

	size = 10000;
	m_buff = DNew TCHAR[size];
	memset(m_buff, 0, size * sizeof(TCHAR));
	m_pOFN->lpstrFile	= m_buff;
	m_pOFN->nMaxFile	= size;
}

COpenFileDlg::~COpenFileDlg()
{
	delete [] m_InitialDir;
	delete [] m_buff;
}

BEGIN_MESSAGE_MAP(COpenFileDlg, CFileDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// COpenFileDlg message handlers

LRESULT CALLBACK COpenFileDlg::WindowProcNew(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message ==  WM_COMMAND && HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == IDOK
			&& m_fAllowDirSelection) {
		CAutoVectorPtr<TCHAR> path;
		path.Allocate(_MAX_PATH+1);

		if (::GetDlgItemText(hwnd, cmb13, (TCHAR*)path, _MAX_PATH) == 0) {
			::SendMessage(hwnd, CDM_SETCONTROLTEXT, edt1, (LPARAM)__DUMMY__);
		}
	}

	return CallWindowProc(COpenFileDlg::m_wndProc, hwnd, message, wParam, lParam);
}

BOOL COpenFileDlg::OnInitDialog()
{
	CFileDialog::OnInitDialog();

	m_wndProc = (WNDPROC)SetWindowLongPtr(GetParent()->m_hWnd, GWLP_WNDPROC , (LONG_PTR)WindowProcNew);

	return TRUE;
}

void COpenFileDlg::OnDestroy()
{
	int i = GetPathName().Find(__DUMMY__);

	if (i >= 0) {
		m_pOFN->lpstrFile[i] = m_pOFN->lpstrFile[i+1] = 0;
	}

	CFileDialog::OnDestroy();
}

BOOL COpenFileDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	ASSERT(pResult != NULL);

	OFNOTIFY* pNotify = (OFNOTIFY*)lParam;

	if (__super::OnNotify(wParam, lParam, pResult)) {
		ASSERT(pNotify->hdr.code != CDN_INCLUDEITEM);
		return TRUE;
	}

	switch (pNotify->hdr.code) {
		case CDN_INCLUDEITEM:
			if (OnIncludeItem((OFNOTIFYEX*)lParam, pResult)) {
				return TRUE;
			}

			break;
	}

	return FALSE;
}

BOOL COpenFileDlg::OnIncludeItem(OFNOTIFYEX* pOFNEx, LRESULT* pResult)
{
	TCHAR buff[_MAX_PATH];

	if (!SHGetPathFromIDList((LPCITEMIDLIST)pOFNEx->pidl, buff)) {
		STRRET s;
		HRESULT hr = ((IShellFolder*)pOFNEx->psf)->GetDisplayNameOf((LPCITEMIDLIST)pOFNEx->pidl, SHGDN_NORMAL|SHGDN_FORPARSING, &s);

		if (S_OK != hr) {
			return FALSE;
		}

		switch (s.uType) {
			case STRRET_CSTR:
				_tcscpy_s(buff, CString(s.cStr));
				break;
			case STRRET_WSTR:
				_tcscpy_s(buff, CString(s.pOleStr));
				CoTaskMemFree(s.pOleStr);
				break;
			default:
				return FALSE;
		}
	}

	CString fn(buff);
	/*
		WIN32_FILE_ATTRIBUTE_DATA fad;
		if (GetFileAttributesEx(fn, GetFileExInfoStandard, &fad)
		&& (fad.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			return FALSE;
	*/

	int i = fn.ReverseFind('.'), j = fn.ReverseFind('\\');

	if (i < 0 || i < j) {
		return FALSE;
	}

	CString mask = m_mask[pOFNEx->lpOFN->nFilterIndex-1] + _T(";");
	CString ext = fn.Mid(i).MakeLower() + _T(";");

	*pResult = mask.Find(ext) >= 0 || mask.Find(_T("*.*")) >= 0;

	return TRUE;
}
