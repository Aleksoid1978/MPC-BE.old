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
#include "MainFrm.h"
#include "PPageWebServer.h"
#include "../../DSUtil/SysVersion.h"


// CPPageWebServer dialog

IMPLEMENT_DYNAMIC(CPPageWebServer, CPPageBase)
CPPageWebServer::CPPageWebServer()
	: CPPageBase(CPPageWebServer::IDD, CPPageWebServer::IDD)
	, m_fEnableWebServer(FALSE)
	, m_nWebServerPort(13579)
	, m_nWebServerQuality(85)
	, m_launch(_T("http://localhost:13579/"))
	, m_fWebServerPrintDebugInfo(FALSE)
	, m_fWebServerUseCompression(FALSE)
	, m_fWebRoot(FALSE)
	, m_fWebServerLocalhostOnly(FALSE)
{
}

CPPageWebServer::~CPPageWebServer()
{
}

void CPPageWebServer::DoDataExchange(CDataExchange* pDX)
{
	__super::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK1, m_fEnableWebServer);
	DDX_Text(pDX, IDC_EDIT1, m_nWebServerPort);
	DDX_Text(pDX, IDC_EDIT4, m_nWebServerQuality);
	DDX_Control(pDX, IDC_SPIN1, m_nWebServerQualityCtrl);
	DDX_Control(pDX, IDC_STATIC1, m_launch);
	DDX_Check(pDX, IDC_CHECK2, m_fWebServerPrintDebugInfo);
	DDX_Check(pDX, IDC_CHECK3, m_fWebServerUseCompression);
	DDX_Check(pDX, IDC_CHECK4, m_fWebRoot);
	DDX_Text(pDX, IDC_EDIT2, m_WebRoot);
	DDX_Check(pDX, IDC_CHECK5, m_fWebServerLocalhostOnly);
	DDX_Text(pDX, IDC_EDIT3, m_WebServerCGI);
	DDX_Text(pDX, IDC_EDIT9, m_WebDefIndex);
}

BOOL CPPageWebServer::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN && pMsg->hwnd == m_launch.m_hWnd) {
		UpdateData();

		AppSettings& s = AfxGetAppSettings();

		if (CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd()) {
			if (m_fEnableWebServer) {
				if (s.nWebServerPort != m_nWebServerPort) {
					AfxMessageBox(ResStr(IDS_WEBSERVER_ERROR_TEST), MB_ICONEXCLAMATION | MB_OK);

					return TRUE;
				}
			}
		}
	}

	return CPPageBase::PreTranslateMessage(pMsg);
}

BOOL CPPageWebServer::OnInitDialog()
{
	__super::OnInitDialog();

	AppSettings& s = AfxGetAppSettings();

	m_fEnableWebServer = s.fEnableWebServer;
	m_nWebServerPort = s.nWebServerPort;
	m_nWebServerQuality = s.nWebServerQuality;
	m_fWebServerPrintDebugInfo = s.fWebServerPrintDebugInfo;
	m_fWebServerLocalhostOnly = s.fWebServerLocalhostOnly;
	m_fWebServerUseCompression = s.fWebServerUseCompression;
	m_fWebRoot = s.strWebRoot.Find('*') < 0;
	m_WebRoot = s.strWebRoot;
	m_WebRoot.TrimLeft(_T('*'));
	m_WebDefIndex = s.strWebDefIndex;
	m_WebServerCGI = s.strWebServerCGI;

	m_launch.EnableWindow(m_fEnableWebServer);

	m_nWebServerQualityCtrl.SetRange(70, 100);
	m_nWebServerQualityCtrl.SetPos(m_nWebServerQuality);

	UpdateData(FALSE);

	OnEnChangeEdit1();

	return TRUE;
}

BOOL CPPageWebServer::OnApply()
{
	UpdateData();

	AppSettings& s = AfxGetAppSettings();

	CString NewWebRoot = m_WebRoot;

	if (!m_fWebRoot) {
		NewWebRoot = _T("*") + NewWebRoot;
	}

	bool fRestart = s.nWebServerPort != m_nWebServerPort
					|| s.strWebRoot != NewWebRoot || s.strWebServerCGI != m_WebServerCGI;

	s.fEnableWebServer = !!m_fEnableWebServer;
	s.nWebServerPort = m_nWebServerPort;
	s.nWebServerQuality = m_nWebServerQuality;
	s.fWebServerPrintDebugInfo = !!m_fWebServerPrintDebugInfo;
	s.fWebServerLocalhostOnly = !!m_fWebServerLocalhostOnly;
	s.fWebServerUseCompression = !!m_fWebServerUseCompression;
	s.strWebRoot = NewWebRoot;
	s.strWebDefIndex = m_WebDefIndex;
	s.strWebServerCGI = m_WebServerCGI;

	if (CMainFrame* pWnd = (CMainFrame*)AfxGetMainWnd()) {
		if (m_fEnableWebServer) {
			if (fRestart) {
				pWnd->StopWebServer();
			}

			pWnd->StartWebServer(m_nWebServerPort);
		} else {
			pWnd->StopWebServer();
		}
	}

	m_launch.EnableWindow(m_fEnableWebServer);

	return __super::OnApply();
}

CString CPPageWebServer::GetMPCDir()
{
	CString dir;
	GetModuleFileName(AfxGetInstanceHandle(), dir.GetBuffer(_MAX_PATH), _MAX_PATH);
	dir.ReleaseBuffer();
	CPath path(dir);
	path.RemoveFileSpec();

	return (LPCTSTR)path;
}

CString CPPageWebServer::GetCurWebRoot()
{
	CString WebRoot;
	GetDlgItem(IDC_EDIT2)->GetWindowText(WebRoot);
	WebRoot.Replace('/', '\\');

	CPath path;
	path.Combine(GetMPCDir(), WebRoot);
	return path.IsDirectory() ? (LPCTSTR)path : _T("");
}

static int __stdcall BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED && lpData) {
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, lpData);
	}

	return 0;
}

bool CPPageWebServer::PickDir(CString& dir)
{
	CString strTitle = ResStr(IDS_PPAGEWEBSERVER_0);
	bool success = false;

	if (IsWinVistaOrLater()) {
		CFileDialog dlg(TRUE);
		IFileOpenDialog *openDlgPtr = dlg.GetIFileOpenDialog();

		if (openDlgPtr != NULL) {
			openDlgPtr->SetTitle(strTitle);
			openDlgPtr->SetOptions(FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_PATHMUSTEXIST);

			CComPtr<IShellItem> psiFolder;
			if (SUCCEEDED(afxGlobalData.ShellCreateItemFromParsingName(dir, NULL, IID_PPV_ARGS(&psiFolder)))) {
				openDlgPtr->SetFolder(psiFolder);
			}

			if (SUCCEEDED(openDlgPtr->Show(m_hWnd))) {
				dir = dlg.GetFolderPath();
				success = true;
			}

			openDlgPtr->Release();
		}
	} else {
		TCHAR buff[_MAX_PATH];

		BROWSEINFO bi;
		bi.hwndOwner = m_hWnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = buff;
		bi.lpszTitle = strTitle;
		bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_VALIDATE | BIF_USENEWUI;
		bi.lpfn = BrowseCtrlCallback;
		bi.lParam = (LPARAM)(LPCTSTR)dir;
		bi.iImage = 0;

		LPITEMIDLIST iil = SHBrowseForFolder(&bi);

		if (iil) {
			SHGetPathFromIDList(iil, buff);
			dir = buff;
			success =  true;
		}
	}

	return success;
}

BEGIN_MESSAGE_MAP(CPPageWebServer, CPPageBase)
	ON_EN_CHANGE(IDC_EDIT1, OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_UPDATE_COMMAND_UI(IDC_BUTTON1, OnUpdateButton2)
END_MESSAGE_MAP()

// CPPageWebServer message handlers

void CPPageWebServer::OnEnChangeEdit1()
{
	UpdateData();

	CString link;
	link.Format(_T("http://localhost:%d/"), m_nWebServerPort);
	m_launch.m_link = link;

	SetModified();
}

void CPPageWebServer::OnBnClickedButton1()
{
	CString dir = GetCurWebRoot();

	if (PickDir(dir)) {
		CPath path;

		if (path.RelativePathTo(GetMPCDir(), FILE_ATTRIBUTE_DIRECTORY, dir, FILE_ATTRIBUTE_DIRECTORY)) {
			dir = (LPCTSTR)path;
		}

		m_WebRoot = dir;

		UpdateData(FALSE);
		SetModified();
	}
}

void CPPageWebServer::OnBnClickedButton2()
{
	CString dir;

	if (PickDir(dir)) {
		dir += _T("\\");
		CWebServer::Deploy(dir);
	}
}

void CPPageWebServer::OnUpdateButton2(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(GetDlgItem(IDC_EDIT2)->GetWindowTextLength() > 0);
}
