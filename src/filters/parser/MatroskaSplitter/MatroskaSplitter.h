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

#include <atlbase.h>
#include <atlcoll.h>
#include "MatroskaFile.h"
#include "MatroskaSplitterSettingsWnd.h"
#include "../BaseSplitter/BaseSplitter.h"
#include <ITrackInfo.h>

#define MatroskaSplitterName L"MPC Matroska Splitter"
#define MatroskaSourceName   L"MPC Matroska Source"

class MatroskaPacket : public Packet
{
protected:
	size_t GetDataSize() {
		size_t size = 0;
		POSITION pos = bg->Block.BlockData.GetHeadPosition();
		while (pos) {
			size += bg->Block.BlockData.GetNext(pos)->GetCount();
		}
		return size;
	}
public:
	CAutoPtr<MatroskaReader::BlockGroup> bg;
};

class CMatroskaSplitterOutputPin : public CBaseSplitterOutputPin
{
	HRESULT DeliverBlock(MatroskaPacket* p);

	unsigned int m_nMinCache;
	REFERENCE_TIME m_rtDefaultDuration;

	CCritSec m_csQueue;
	CAutoPtrList<MatroskaPacket> m_packets;
	CAtlList<MatroskaPacket*> m_rob;

	typedef struct {
		REFERENCE_TIME rtStart, rtStop;
	} timeoverride;
	CAtlList<timeoverride> m_tos;

protected:
	HRESULT DeliverPacket(CAutoPtr<Packet> p);

public:
	CMatroskaSplitterOutputPin(
		unsigned int nMinCache, REFERENCE_TIME rtDefaultDuration,
		CAtlArray<CMediaType>& mts, LPCWSTR pName, CBaseFilter* pFilter, CCritSec* pLock, HRESULT* phr);
	virtual ~CMatroskaSplitterOutputPin();

	HRESULT DeliverEndFlush();
	HRESULT DeliverEndOfStream();
};

class __declspec(uuid("149D2E01-C32E-4939-80F6-C07B81015A7A"))
	CMatroskaSplitterFilter
	: public CBaseSplitterFilter
	, public ITrackInfo
	, public IMatroskaSplitterFilter
	, public ISpecifyPropertyPages2
{
	void SetupChapters(LPCSTR lng, MatroskaReader::ChapterAtom* parent, int level = 0);
	void InstallFonts();
	void SendVorbisHeaderSample();

	CAutoPtr<MatroskaReader::CMatroskaNode> m_pSegment, m_pCluster, m_pBlock;

private:
	CCritSec m_csProps;
	bool m_bLoadEmbeddedFonts;

protected:
	CAutoPtr<MatroskaReader::CMatroskaFile> m_pFile;
	HRESULT CreateOutputs(IAsyncReader* pAsyncReader);

	CAtlMap<DWORD, MatroskaReader::TrackEntry*> m_pTrackEntryMap;
	CAtlArray<MatroskaReader::TrackEntry* > m_pOrderedTrackArray;
	MatroskaReader::TrackEntry* GetTrackEntryAt(UINT aTrackIdx);

	bool DemuxInit();
	void DemuxSeek(REFERENCE_TIME rt);
	bool DemuxLoop();

public:
	CMatroskaSplitterFilter(LPUNKNOWN pUnk, HRESULT* phr);
	virtual ~CMatroskaSplitterFilter();

	DECLARE_IUNKNOWN;
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void** ppv);

	// CBaseFilter

	STDMETHODIMP QueryFilterInfo(FILTER_INFO* pInfo);

	// IKeyFrameInfo

	STDMETHODIMP GetKeyFrameCount(UINT& nKFs);
	STDMETHODIMP GetKeyFrames(const GUID* pFormat, REFERENCE_TIME* pKFs, UINT& nKFs);

	// ITrackInfo

	STDMETHODIMP_(UINT) GetTrackCount();
	STDMETHODIMP_(BOOL) GetTrackInfo(UINT aTrackIdx, struct TrackElement* pStructureToFill);
	STDMETHODIMP_(BOOL) GetTrackExtendedInfo(UINT aTrackIdx, void* pStructureToFill);
	STDMETHODIMP_(BSTR) GetTrackName(UINT aTrackIdx);
	STDMETHODIMP_(BSTR) GetTrackCodecID(UINT aTrackIdx);
	STDMETHODIMP_(BSTR) GetTrackCodecName(UINT aTrackIdx);
	STDMETHODIMP_(BSTR) GetTrackCodecInfoURL(UINT aTrackIdx);
	STDMETHODIMP_(BSTR) GetTrackCodecDownloadURL(UINT aTrackIdx);

	// ISpecifyPropertyPages2

	STDMETHODIMP GetPages(CAUUID* pPages);
	STDMETHODIMP CreatePage(const GUID& guid, IPropertyPage** ppPage);

	// IMatroskaSplitterFilter
	STDMETHODIMP Apply();

	STDMETHODIMP SetLoadEmbeddedFonts(BOOL nValue);
	STDMETHODIMP_(BOOL) GetLoadEmbeddedFonts();
};

class __declspec(uuid("0A68C3B5-9164-4a54-AFAF-995B2FF0E0D4"))
	CMatroskaSourceFilter : public CMatroskaSplitterFilter
{
public:
	CMatroskaSourceFilter(LPUNKNOWN pUnk, HRESULT* phr);
};
