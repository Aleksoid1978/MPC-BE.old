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
#include <MMReg.h>
#include "FLVSplitter.h"
#include "../../../DSUtil/DSUtil.h"
#include "../../../DSUtil/VideoParser.h"

#ifdef REGISTER_FILTER
#include <InitGuid.h>
#endif
#include <moreuuids.h>

#define FLV_AUDIODATA     8
#define FLV_VIDEODATA     9
#define FLV_SCRIPTDATA    18

#define FLV_AUDIO_PCM     0 // Linear PCM, platform endian
#define FLV_AUDIO_ADPCM   1 // ADPCM
#define FLV_AUDIO_MP3     2 // MP3
#define FLV_AUDIO_PCMLE   3 // Linear PCM, little endian
#define FLV_AUDIO_NELLY16 4 // Nellymoser 16 kHz mono
#define FLV_AUDIO_NELLY8  5 // Nellymoser 8 kHz mono
#define FLV_AUDIO_NELLY   6 // Nellymoser
// 7 = G.711 A-law logarithmic PCM (reserved)
// 8 = G.711 mu-law logarithmic PCM (reserved)
// 9 = reserved
#define FLV_AUDIO_AAC     10 // AAC
#define FLV_AUDIO_SPEEX   11 // Speex
// 14 = MP3 8 kHz (reserved)
// 15 = Device-specific sound (reserved)

//#define FLV_VIDEO_JPEG   1 // non-standard? need samples
#define FLV_VIDEO_H263     2 // Sorenson H.263
#define FLV_VIDEO_SCREEN   3 // Screen video
#define FLV_VIDEO_VP6      4 // On2 VP6
#define FLV_VIDEO_VP6A     5 // On2 VP6 with alpha channel
#define FLV_VIDEO_SCREEN2  6 // Screen video version 2
#define FLV_VIDEO_AVC      7 // AVC
#define FLV_VIDEO_HM62    11 // HM6.2
#define FLV_VIDEO_HM91    12 // HM9.1
#define FLV_VIDEO_HM10    13 // HM10.0
#define FLV_VIDEO_HEVC    14 // HEVC (HM version write to MetaData "HM compatibility")

#define AMF_END_OF_OBJECT			0x09

#define KEYFRAMES_TAG				L"keyframes"
#define KEYFRAMES_TIMESTAMP_TAG		L"times"
#define KEYFRAMES_BYTEOFFSET_TAG	L"filepositions"

#define IsValidTag(TagType)			(TagType == FLV_AUDIODATA || TagType == FLV_VIDEODATA || TagType == FLV_SCRIPTDATA)
#define IsAVCCodec(CodecID)			(CodecID == FLV_VIDEO_AVC || CodecID == FLV_VIDEO_HM91 || CodecID == FLV_VIDEO_HM10 || CodecID == FLV_VIDEO_HEVC)


#ifdef REGISTER_FILTER

const AMOVIESETUP_MEDIATYPE sudPinTypesIn[] = {
	{&MEDIATYPE_Stream, &MEDIASUBTYPE_FLV},
	{&MEDIATYPE_Stream, &MEDIASUBTYPE_NULL},
};

const AMOVIESETUP_PIN sudpPins[] = {
	{L"Input", FALSE, FALSE, FALSE, FALSE, &CLSID_NULL, NULL, _countof(sudPinTypesIn), sudPinTypesIn},
	{L"Output", FALSE, TRUE, FALSE, FALSE, &CLSID_NULL, NULL, 0, NULL}
};

const AMOVIESETUP_FILTER sudFilter[] = {
	{&__uuidof(CFLVSplitterFilter), FlvSplitterName, MERIT_NORMAL, _countof(sudpPins), sudpPins, CLSID_LegacyAmFilterCategory},
	{&__uuidof(CFLVSourceFilter), FlvSourceName, MERIT_NORMAL, 0, NULL, CLSID_LegacyAmFilterCategory},
};

CFactoryTemplate g_Templates[] = {
	{sudFilter[0].strName, sudFilter[0].clsID, CreateInstance<CFLVSplitterFilter>, NULL, &sudFilter[0]},
	{sudFilter[1].strName, sudFilter[1].clsID, CreateInstance<CFLVSourceFilter>, NULL, &sudFilter[1]},
};

int g_cTemplates = _countof(g_Templates);

STDAPI DllRegisterServer()
{
	DeleteRegKey(_T("Media Type\\Extensions\\"), _T(".flv"));

	RegisterSourceFilter(CLSID_AsyncReader, MEDIASUBTYPE_FLV, _T("0,4,,464C5601"), NULL);

	return AMovieDllRegisterServer2(TRUE);
}

STDAPI DllUnregisterServer()
{
	UnRegisterSourceFilter(MEDIASUBTYPE_FLV);

	return AMovieDllRegisterServer2(FALSE);
}

#include "../../filters/Filters.h"

CFilterApp theApp;

#endif

//
// CFLVSplitterFilter
//

CFLVSplitterFilter::CFLVSplitterFilter(LPUNKNOWN pUnk, HRESULT* phr)
	: CBaseSplitterFilter(NAME("CFLVSplitterFilter"), pUnk, phr, __uuidof(this))
	, m_TimeStampOffset(0)
	, m_DetectWrongTimeStamp(true)
{
	m_nFlag |= PACKET_PTS_DISCONTINUITY;
	m_nFlag |= PACKET_PTS_VALIDATE_POSITIVE;
	//memset(&meta, 0, sizeof(meta));
}

STDMETHODIMP CFLVSplitterFilter::QueryFilterInfo(FILTER_INFO* pInfo)
{
	CheckPointer(pInfo, E_POINTER);
	ValidateReadWritePtr(pInfo, sizeof(FILTER_INFO));

	if (m_pName && m_pName[0]==L'M' && m_pName[1]==L'P' && m_pName[2]==L'C') {
		(void)StringCchCopyW(pInfo->achName, NUMELMS(pInfo->achName), m_pName);
	} else {
		wcscpy_s(pInfo->achName, FlvSourceName);
	}
	pInfo->pGraph = m_pGraph;
	if (m_pGraph) {
		m_pGraph->AddRef();
	}

	return S_OK;
}

static double int64toDouble(__int64 value)
{
	union
	{
		__int64	i;
		double	f;
	} intfloat64;
	
	intfloat64.i = value;

	return intfloat64.f;
}

CString CFLVSplitterFilter::AMF0GetString(CBaseSplitterFileEx* pFile, UINT64 end)
{
	char name[256] = {0};

	SHORT length = pFile->BitRead(16);
	if (UINT64(pFile->GetPos() + length) > end) {
		return L"";
	}

	if (length > sizeof(name)) {
		pFile->Seek(pFile->GetPos() + length);
		return L"";
	}

	pFile->ByteRead((BYTE*)name, length);

	return CString(name);
}

bool CFLVSplitterFilter::ParseAMF0(CBaseSplitterFileEx* pFile, UINT64 end, const CString key, CAtlArray<AMF0> &AMF0Array)
{
	if (UINT64(pFile->GetPos()) >= (end - 2)) {
		return false;
	}

	AMF0 amf0;

	AMF_DATA_TYPE amf_type = (AMF_DATA_TYPE)pFile->BitRead(8);

	switch (amf_type) {
		case AMF_DATA_TYPE_NUMBER:
			{
				UINT64 value = pFile->BitRead(64);

				amf0.type		= amf_type;
				amf0.name		= key;
				amf0.value_d	= int64toDouble(value);
			}
			break;
		case AMF_DATA_TYPE_BOOL:
			{
				BYTE value = pFile->BitRead(8);

				amf0.type		= amf_type;
				amf0.name		= key;
				amf0.value_b	= !!value;
			}
			break;
		case AMF_DATA_TYPE_STRING:
			{
				amf0.type		= amf_type;
				amf0.name		= key;
				amf0.value_s	= AMF0GetString(pFile, end);
			}
			break;
		case AMF_DATA_TYPE_OBJECT:
			{
				if (key == KEYFRAMES_TAG && m_sps.GetCount() == 0) {
					CAtlArray<double> times;
					CAtlArray<double> filepositions;
					for (;;) {
						CString name	= AMF0GetString(pFile, end);
						if (name.IsEmpty()) {
							break;
						}
						BYTE value		= pFile->BitRead(8);
						if (value != AMF_DATA_TYPE_ARRAY) {
							break;
						}
						WORD arraylen	= pFile->BitRead(32);
						CAtlArray<double>* array = NULL;
						if (name == KEYFRAMES_BYTEOFFSET_TAG) {
							array		= &filepositions;
						} else if (name == KEYFRAMES_TIMESTAMP_TAG) {
							array		= &times;
						} else {
							break;
						}

						for (int i = 0; i < arraylen; i++) {
							BYTE value = pFile->BitRead(8);
							if (value != AMF_DATA_TYPE_NUMBER) {
								break;
							}
							array->Add(int64toDouble(pFile->BitRead(64)));
						}
					}

					if (times.GetCount() && times.GetCount() == filepositions.GetCount()) {
						for (size_t i = 0; i < times.GetCount(); i++) {
							SyncPoint sp = {REFERENCE_TIME(times[i] * UNITS), __int64(filepositions[i])};
							m_sps.Add(sp);
						}
					}
				}
			}
			break;
		case AMF_DATA_TYPE_NULL:
		case AMF_DATA_TYPE_UNDEFINED:
		case AMF_DATA_TYPE_UNSUPPORTED:
			return true;
		case AMF_DATA_TYPE_MIXEDARRAY:
			{
				pFile->BitRead(32);
				for (;;) {
					CString name = AMF0GetString(pFile, end);
					if (name.IsEmpty()) {
						return false;
					}
					if (ParseAMF0(pFile, end, name, AMF0Array) == false) {
						return false;
					}
				}

				return (pFile->BitRead(8) == AMF_END_OF_OBJECT);
			}
		case AMF_DATA_TYPE_ARRAY:
			{
				DWORD arraylen = pFile->BitRead(32);
				for (DWORD i = 0; i < arraylen; i++) {
					if (ParseAMF0(pFile, end, L"", AMF0Array) == false) {
						return false;
					}
				}
			}
			break; // TODO ...
		case AMF_DATA_TYPE_DATE:
			pFile->Seek(pFile->GetPos() + 8 + 2);
			return true;
	}

	if (amf0.type != AMF_DATA_TYPE_EMPTY) {
		AMF0Array.Add(amf0);
	}

	return true;
}

bool CFLVSplitterFilter::ReadTag(Tag& t)
{
	if (FAILED(m_pFile->WaitAvailable(1000, 15))) {
		//return false;
	}

	if (m_pFile->GetRemaining(true) < 15) {
		return false;
	}

	t.PreviousTagSize	= (UINT32)m_pFile->BitRead(32);
	t.TagType			= (BYTE)m_pFile->BitRead(8);
	t.DataSize			= (UINT32)m_pFile->BitRead(24);
	t.TimeStamp			= (UINT32)m_pFile->BitRead(24);
	t.TimeStamp		   |= (UINT32)m_pFile->BitRead(8) << 24;
	t.StreamID			= (UINT32)m_pFile->BitRead(24);

	if (m_DetectWrongTimeStamp && (t.TagType == FLV_AUDIODATA || t.TagType == FLV_VIDEODATA)) {
		if (t.TimeStamp > 0) {
			m_TimeStampOffset = t.TimeStamp;
		}
		m_DetectWrongTimeStamp = false;
	}

	if (m_TimeStampOffset > 0) {
		t.TimeStamp -= m_TimeStampOffset;
		DbgLog((LOG_TRACE, 3, L"CFLVSplitterFilter::ReadTag() : Detect wrong TimeStamp offset, corrected [%d -> %d]",  (t.TimeStamp + m_TimeStampOffset), t.TimeStamp));
	}

	return m_pFile->IsRandomAccess() ? (m_pFile->GetRemaining() >= t.DataSize) : true;
}

bool CFLVSplitterFilter::ReadTag(AudioTag& at)
{
	if (FAILED(m_pFile->WaitAvailable(1000))) {
		//return false;
	}

	if (!m_pFile->GetRemaining(true)) {
		return false;
	}

	at.SoundFormat	= (BYTE)m_pFile->BitRead(4);
	at.SoundRate	= (BYTE)m_pFile->BitRead(2);
	at.SoundSize	= (BYTE)m_pFile->BitRead(1);
	at.SoundType	= (BYTE)m_pFile->BitRead(1);

	return true;
}

bool CFLVSplitterFilter::ReadTag(VideoTag& vt)
{
	if (FAILED(m_pFile->WaitAvailable(1000))) {
		//return false;
	}

	if (!m_pFile->GetRemaining(true)) {
		return false;
	}

	vt.FrameType		= (BYTE)m_pFile->BitRead(4);
	vt.CodecID			= (BYTE)m_pFile->BitRead(4);
	vt.AVCPacketType	= 0;
	vt.tsOffset			= 0;

	if (IsAVCCodec(vt.CodecID)) {
		if (m_pFile->GetRemaining(true) < 3) {
			return false;
		}

		vt.AVCPacketType	= (BYTE)m_pFile->BitRead(8);
		if (vt.AVCPacketType == 1) {
			vt.tsOffset		= (UINT32)m_pFile->BitRead(24);
			vt.tsOffset		= (vt.tsOffset + 0xff800000) ^ 0xff800000; // sign extension
		} else {
			m_pFile->BitRead(24);
		}
	}

	return true;
}

#ifndef NOVIDEOTWEAK
bool CFLVSplitterFilter::ReadTag(VideoTweak& vt)
{
	if (FAILED(m_pFile->WaitAvailable(1000))) {
		//return false;
	}

	vt.x = (BYTE)m_pFile->BitRead(4);
	vt.y = (BYTE)m_pFile->BitRead(4);

	return true;
}
#endif

bool CFLVSplitterFilter::Sync(__int64& pos)
{
	m_pFile->Seek(pos);

	while (m_pFile->GetRemaining(true) >= 15) {
		__int64 limit = m_pFile->GetRemaining(true);
		while (true) {
			BYTE b = (BYTE)m_pFile->BitRead(8);
			if (b == FLV_AUDIODATA || b == FLV_VIDEODATA) {
				break;
			}
			if (--limit < 15) {
				return false;
			}
		}

		pos = m_pFile->GetPos() - 5;
		m_pFile->Seek(pos);

		Tag ct;
		if (ReadTag(ct) && IsValidTag(ct.TagType)) {
			__int64 next = m_pFile->GetPos() + ct.DataSize;
			if (next == m_pFile->GetAvailable() - 4) {
				m_pFile->Seek(pos);
				return true;
			} else if (next <= m_pFile->GetAvailable() - 19) {
				m_pFile->Seek(next);
				Tag nt;
				if (ReadTag(nt) && IsValidTag(nt.TagType)) {
					if (nt.PreviousTagSize == ct.DataSize) {
						m_IgnorePrevSizes = true;
					}
					if ((nt.PreviousTagSize == ct.DataSize + 11) ||
							(m_IgnorePrevSizes &&
								nt.TimeStamp >= ct.TimeStamp/* &&
								nt.TimeStamp - ct.TimeStamp <= 1000*/)) {
						m_pFile->Seek(pos);
						return true;
					}
				}
			}
		}

		m_pFile->Seek(pos + 5);
	}

	return false;
}

HRESULT CFLVSplitterFilter::CreateOutputs(IAsyncReader* pAsyncReader)
{
	CheckPointer(pAsyncReader, E_POINTER);

	HRESULT hr = E_FAIL;

	m_pFile.Free();
	m_pFile.Attach(DNew CBaseSplitterFileEx(pAsyncReader, hr, false, true, true));
	if (!m_pFile) {
		return E_OUTOFMEMORY;
	}
	if (FAILED(hr)) {
		m_pFile.Free();
		return hr;
	}

	m_rtNewStart = m_rtCurrent = 0;
	m_rtNewStop = m_rtStop = m_rtDuration = 0;

	if (m_pFile->BitRead(24) != 'FLV' || m_pFile->BitRead(8) != 1) {
		return E_FAIL;
	}

	EXECUTE_ASSERT(m_pFile->BitRead(5) == 0); // TypeFlagsReserved
	bool fTypeFlagsAudio = !!m_pFile->BitRead(1);
	EXECUTE_ASSERT(m_pFile->BitRead(1) == 0); // TypeFlagsReserved
	bool fTypeFlagsVideo = !!m_pFile->BitRead(1);
	m_DataOffset = (UINT32)m_pFile->BitRead(32);

	// doh, these flags aren't always telling the truth
	fTypeFlagsAudio = fTypeFlagsVideo = true;

	Tag t;
	AudioTag at;
	VideoTag vt;

	UINT32 prevTagSize = 0;
	m_IgnorePrevSizes = false;

	m_pFile->Seek(m_DataOffset);

	REFERENCE_TIME AvgTimePerFrame	= 0;
	REFERENCE_TIME metaDataDuration	= 0;
	DWORD nSamplesPerSec			= 0;
	int   metaHM_compatibility		= 0;

	m_sps.RemoveAll();

	for (int i = 0; i < 100 && ReadTag(t) && (fTypeFlagsVideo || fTypeFlagsAudio); i++) {
		if (!t.DataSize) {
			continue; // skip empty Tag
		}

		UINT64 next = m_pFile->GetPos() + t.DataSize;

		if (!IsValidTag(t.TagType)) {
			m_pFile->Seek(next);
			prevTagSize = 0;
			continue;
		}

		CString name;

		CMediaType mt;
		mt.subtype = GUID_NULL;

		if (prevTagSize != 0 && t.PreviousTagSize != prevTagSize) {
			m_IgnorePrevSizes = true;
		}
		prevTagSize = t.DataSize + 11;

		if (t.TagType == FLV_SCRIPTDATA && t.DataSize) {
			BYTE type = m_pFile->BitRead(8);
			SHORT length = m_pFile->BitRead(16);
			if (type == AMF_DATA_TYPE_STRING && length <= 11) {
				char name[11];
				memset(name, 0, 11);
				m_pFile->ByteRead((BYTE*)name, length);
				if (!strncmp(name, "onTextData", length) || (!strncmp(name, "onMetaData", length))) {
					CAtlArray<AMF0> AMF0Array;
					ParseAMF0(m_pFile, next, CString(name), AMF0Array);

					for (size_t i = 0; i < AMF0Array.GetCount(); i++) {
						if (AMF0Array[i].type == AMF_DATA_TYPE_NUMBER) {
							if (AMF0Array[i].name == L"duration") {
								metaDataDuration = (REFERENCE_TIME)(UNITS * (double)AMF0Array[i]);
							}
							else if (AMF0Array[i].name == L"framerate") {
								double value = AMF0Array[i];
								if (value != 0 && value != 1000) {
									AvgTimePerFrame = (REFERENCE_TIME)(UNITS / (int)value);
								}
							}
							else if (AMF0Array[i].name == L"audiosamplerate") {
								double value = AMF0Array[i];
								if (value != 0) {
									nSamplesPerSec = value;
								}
							}
						}
						else if (AMF0Array[i].type == AMF_DATA_TYPE_STRING && AMF0Array[i].name == L"HM compatibility") {
							metaHM_compatibility = (int)(_tstof(AMF0Array[i].value_s) * 10.0);
						}
					}
				}
			}
		} else if (t.TagType == FLV_AUDIODATA && t.DataSize != 0 && fTypeFlagsAudio) {
			UNREFERENCED_PARAMETER(at);
			AudioTag at;
			name = L"Audio";

			if (ReadTag(at)) {
				int dataSize = t.DataSize - 1;

				fTypeFlagsAudio = false;

				mt.majortype			= MEDIATYPE_Audio;
				mt.formattype			= FORMAT_WaveFormatEx;
				WAVEFORMATEX* wfe		= (WAVEFORMATEX*)mt.AllocFormatBuffer(sizeof(WAVEFORMATEX));
				memset(wfe, 0, sizeof(WAVEFORMATEX));
				wfe->nSamplesPerSec		= nSamplesPerSec ? nSamplesPerSec : 44100*(1<<at.SoundRate)/8;
				wfe->wBitsPerSample		= 8 * (at.SoundSize + 1);
				wfe->nChannels			= at.SoundType + 1;

				switch (at.SoundFormat) {
					case FLV_AUDIO_PCM:
					case FLV_AUDIO_PCMLE:
						mt.subtype = FOURCCMap(wfe->wFormatTag = WAVE_FORMAT_PCM);
						name += L" PCM";
						break;
					case FLV_AUDIO_ADPCM:
						mt.subtype = FOURCCMap(wfe->wFormatTag = WAVE_FORMAT_ADPCM_SWF);
						name += L" ADPCM";
						break;
					case FLV_AUDIO_MP3:
						mt.subtype = FOURCCMap(wfe->wFormatTag = WAVE_FORMAT_MPEGLAYER3);
						name += L" MP3";
						{
							CBaseSplitterFileEx::mpahdr h;
							CMediaType mt2;
							if (m_pFile->Read(h, 4, &mt2)) {
								mt = mt2;
							}
						}
						break;
					case FLV_AUDIO_NELLY16:
						mt.subtype = FOURCCMap(MAKEFOURCC('N','E','L','L'));
						wfe->nSamplesPerSec = 16000;
						name += L" Nellimoser";
						break;
					case FLV_AUDIO_NELLY8:
						mt.subtype = FOURCCMap(MAKEFOURCC('N','E','L','L'));
						wfe->nSamplesPerSec = 8000;
						name += L" Nellimoser";
						break;
					case FLV_AUDIO_NELLY:
						mt.subtype = FOURCCMap(MAKEFOURCC('N','E','L','L'));
						name += L" Nellimoser";
						break;
					case FLV_AUDIO_SPEEX:
						mt.subtype = FOURCCMap(wfe->wFormatTag = WAVE_FORMAT_SPEEX);
						wfe->nSamplesPerSec = 16000;
						name += L" Speex";
						break;
					case FLV_AUDIO_AAC: {
						if (dataSize < 1 || m_pFile->BitRead(8) != 0) { // packet type 0 == aac header
							fTypeFlagsAudio = true;
							break;
						}
						mt.subtype = FOURCCMap(wfe->wFormatTag = WAVE_FORMAT_RAW_AAC1);
						name += L" AAC";

						__int64 configOffset = m_pFile->GetPos();
						UINT32 configSize = dataSize - 1;
						if (configSize < 2) {
							break;
						}

						// Might break depending on the AAC profile, see ff_mpeg4audio_get_config in ffmpeg's mpeg4audio.c
						m_pFile->BitRead(5);
						int iSampleRate = (int)m_pFile->BitRead(4);
						int iChannels   = (int)m_pFile->BitRead(4);
						if (iSampleRate > 12 || iChannels > 7) {
							break;
						}

						const int sampleRates[] = {
							96000, 88200, 64000, 48000, 44100, 32000, 24000,
							22050, 16000, 12000, 11025, 8000, 7350
						};
						const int channels[] = {
							0, 1, 2, 3, 4, 5, 6, 8
						};

						wfe = (WAVEFORMATEX*)mt.AllocFormatBuffer(sizeof(WAVEFORMATEX) + configSize);
						memset(wfe, 0, mt.FormatLength());
						wfe->wFormatTag     = WAVE_FORMAT_RAW_AAC1;
						wfe->nSamplesPerSec = sampleRates[iSampleRate];
						wfe->wBitsPerSample = 16;
						wfe->nChannels      = channels[iChannels];
						wfe->cbSize         = configSize;

						m_pFile->Seek(configOffset);
						m_pFile->ByteRead((BYTE*)(wfe+1), configSize);
					}
				}
				wfe->nBlockAlign		= wfe->nChannels * wfe->wBitsPerSample / 8;
				wfe->nAvgBytesPerSec	= wfe->nSamplesPerSec * wfe->nBlockAlign;

				mt.SetSampleSize(wfe->wBitsPerSample * wfe->nChannels / 8);
			}
		} else if (t.TagType == FLV_VIDEODATA && t.DataSize != 0 && fTypeFlagsVideo) {
			UNREFERENCED_PARAMETER(vt);
			VideoTag vt;
			if (ReadTag(vt) && vt.FrameType == 1) {
				int dataSize = t.DataSize - 1;

				fTypeFlagsVideo = false;
				name = L"Video";

				mt.majortype = MEDIATYPE_Video;
				mt.formattype = FORMAT_VideoInfo;
				VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*)mt.AllocFormatBuffer(sizeof(VIDEOINFOHEADER));
				memset(vih, 0, sizeof(VIDEOINFOHEADER));

				BITMAPINFOHEADER* bih = &vih->bmiHeader;

				int w, h, arx, ary;

				// calculate video fps
				if (!AvgTimePerFrame) {
					__int64 pos = m_pFile->GetPos();
					__int64 sync_pos = m_DataOffset;
					if (Sync(sync_pos)) {
						Tag tag;
						VideoTag vtag;
						UINT32 first_ts, current_ts;
						first_ts = current_ts = 0;
						int frame_cnt = 0;

						while ((frame_cnt < 30) && ReadTag(tag) && !CheckRequest(NULL) && m_pFile->GetRemaining(true)) {
							__int64 _next = m_pFile->GetPos() + tag.DataSize;

							if ((tag.DataSize > 0) && (tag.TagType == FLV_VIDEODATA && ReadTag(vtag) && tag.TimeStamp > 0)) {

								if (tag.TimeStamp != current_ts) {
									frame_cnt++;
								}

								current_ts = tag.TimeStamp;

								if (!first_ts && current_ts) {
									first_ts = current_ts;
								}

								current_ts = tag.TimeStamp;
							}
							m_pFile->Seek(_next);
						}

						AvgTimePerFrame = 10000 * (current_ts - first_ts)/(frame_cnt - 1);
					}

					m_pFile->Seek(pos);
				}

				vih->AvgTimePerFrame = AvgTimePerFrame;

				switch (vt.CodecID) {
					case FLV_VIDEO_H263:   // H.263
						if (m_pFile->BitRead(17) != 1) {
							break;
						}

						m_pFile->BitRead(13); // Version (5), TemporalReference (8)

						switch (BYTE PictureSize = (BYTE)m_pFile->BitRead(3)) { // w00t
							case 0:
							case 1:
								vih->bmiHeader.biWidth = (WORD)m_pFile->BitRead(8*(PictureSize+1));
								vih->bmiHeader.biHeight = (WORD)m_pFile->BitRead(8*(PictureSize+1));
								break;
							case 2:
							case 3:
							case 4:
								vih->bmiHeader.biWidth = 704 / PictureSize;
								vih->bmiHeader.biHeight = 576 / PictureSize;
								break;
							case 5:
							case 6:
								PictureSize -= 3;
								vih->bmiHeader.biWidth = 640 / PictureSize;
								vih->bmiHeader.biHeight = 480 / PictureSize;
								break;
						}

						if (!vih->bmiHeader.biWidth || !vih->bmiHeader.biHeight) {
							break;
						}

						mt.subtype = FOURCCMap(vih->bmiHeader.biCompression = '1VLF');
						name += L" H.263";

						break;
					case FLV_VIDEO_SCREEN: {
						m_pFile->BitRead(4);
						vih->bmiHeader.biWidth  = (LONG)m_pFile->BitRead(12);
						m_pFile->BitRead(4);
						vih->bmiHeader.biHeight = (LONG)m_pFile->BitRead(12);

						if (!vih->bmiHeader.biWidth || !vih->bmiHeader.biHeight) {
							break;
						}

						vih->bmiHeader.biSize = sizeof(vih->bmiHeader);
						vih->bmiHeader.biPlanes = 1;
						vih->bmiHeader.biBitCount = 24;
						vih->bmiHeader.biSizeImage = vih->bmiHeader.biWidth * vih->bmiHeader.biHeight * 3;

						mt.subtype = FOURCCMap(vih->bmiHeader.biCompression = '1VSF');
						name += L" Screen";

						break;
					}
					case FLV_VIDEO_VP6A:  // VP6 with alpha
						m_pFile->BitRead(24);
					case FLV_VIDEO_VP6: { // VP6
#ifdef NOVIDEOTWEAK
						m_pFile->BitRead(8);
#else
						VideoTweak fudge;
						ReadTag(fudge);
#endif

						if (m_pFile->BitRead(1)) {
							// Delta (inter) frame
							fTypeFlagsVideo = true;
							break;
						}
						m_pFile->BitRead(6);
						bool fSeparatedCoeff = !!m_pFile->BitRead(1);
						m_pFile->BitRead(5);
						int filterHeader = (int)m_pFile->BitRead(2);
						m_pFile->BitRead(1);
						if (fSeparatedCoeff || !filterHeader) {
							m_pFile->BitRead(16);
						}

						h = (int)m_pFile->BitRead(8) * 16;
						w = (int)m_pFile->BitRead(8) * 16;

						ary = (int)m_pFile->BitRead(8) * 16;
						arx = (int)m_pFile->BitRead(8) * 16;

						if (arx && arx != w || ary && ary != h) {
							VIDEOINFOHEADER2* vih2		= (VIDEOINFOHEADER2*)mt.AllocFormatBuffer(sizeof(VIDEOINFOHEADER2));
							memset(vih2, 0, sizeof(VIDEOINFOHEADER2));
							vih2->dwPictAspectRatioX	= arx;
							vih2->dwPictAspectRatioY	= ary;
							vih2->AvgTimePerFrame		= AvgTimePerFrame;
							bih = &vih2->bmiHeader;
							mt.formattype = FORMAT_VideoInfo2;
							vih = (VIDEOINFOHEADER *)vih2;
						}

						bih->biWidth = w;
						bih->biHeight = h;
#ifndef NOVIDEOTWEAK
						SetRect(&vih->rcSource, 0, 0, w - fudge.x, h - fudge.y);
						SetRect(&vih->rcTarget, 0, 0, w - fudge.x, h - fudge.y);
#endif

						mt.subtype = FOURCCMap(bih->biCompression = '4VLF');
						name += L" VP6";

						break;
					}
					case FLV_VIDEO_AVC: { // H.264
						if (dataSize < 4 || vt.AVCPacketType != 0) {
							fTypeFlagsVideo = true;
							break;
						}

						__int64 headerOffset = m_pFile->GetPos();
						UINT32 headerSize = dataSize - 4;
						BYTE *headerData = DNew BYTE[headerSize];

						m_pFile->ByteRead(headerData, headerSize);

						CGolombBuffer gb(headerData + 9, headerSize - 9);
						avc_hdr h;
						if (!ParseAVCHeader(gb, h)) {
							break;
						}

						BITMAPINFOHEADER pbmi;
						memset(&pbmi, 0, sizeof(BITMAPINFOHEADER));
						pbmi.biSize			= sizeof(pbmi);
						pbmi.biWidth		= h.width;
						pbmi.biHeight		= h.height;
						pbmi.biCompression	= '1CVA';
						pbmi.biPlanes		= 1;
						pbmi.biBitCount		= 24;

						CSize aspect(h.width * h.sar.num, h.height * h.sar.den);
						ReduceDim(aspect);
						CreateMPEG2VIfromAVC(&mt, &pbmi, AvgTimePerFrame, aspect, headerData, headerSize); 

						delete[] headerData;

						name += L" H.264";

						break;
					}
					case FLV_VIDEO_HM62: { // HEVC HM6.2
						// Source code is provided by Deng James from Strongene Ltd.
						// check is avc header
						if (dataSize < 4 || vt.AVCPacketType != 0) {
							fTypeFlagsVideo = true;
							break;
						}

						__int64 headerOffset = m_pFile->GetPos();
						UINT32 headerSize = dataSize - 4;
						BYTE * headerData = DNew BYTE[headerSize];
						m_pFile->ByteRead(headerData, headerSize);
						m_pFile->Seek(headerOffset + 9);

						mt.formattype = FORMAT_MPEG2Video;
						MPEG2VIDEOINFO* vih = (MPEG2VIDEOINFO*)mt.AllocFormatBuffer(FIELD_OFFSET(MPEG2VIDEOINFO, dwSequenceHeader) + headerSize);
						memset(vih, 0, mt.FormatLength());
						vih->hdr.bmiHeader.biSize = sizeof(vih->hdr.bmiHeader);
						vih->hdr.bmiHeader.biPlanes = 1;
						vih->hdr.bmiHeader.biBitCount = 24;
						vih->dwFlags = (headerData[4] & 0x03) + 1; // nal length size

						vih->dwProfile = (BYTE)m_pFile->BitRead(8); // profile
						m_pFile->BitRead(8); // compatibility
						vih->dwLevel = (BYTE)m_pFile->BitRead(8); // level

						// parse SPS
						UINT ue;
						ue = (UINT)m_pFile->UExpGolombRead(); // seq_parameter_set_id
						ue = (UINT)m_pFile->UExpGolombRead(); // ???
						ue = (UINT)m_pFile->BitRead(3); // max_tid_minus_1

						UINT nWidth  = (UINT)m_pFile->UExpGolombRead(); // video width
						UINT nHeight = (UINT)m_pFile->UExpGolombRead(); // video height

						INT bit = (INT)m_pFile->BitRead(1);
						if(bit == 1) {
							ue = (UINT)m_pFile->UExpGolombRead();
							ue = (UINT)m_pFile->UExpGolombRead();
							ue = (UINT)m_pFile->UExpGolombRead();
							ue = (UINT)m_pFile->UExpGolombRead();
						}
						ue = (UINT)m_pFile->UExpGolombRead();
						ue = (UINT)m_pFile->UExpGolombRead();

						bit = (INT)m_pFile->BitRead(1);
						bit = (INT)m_pFile->BitRead(1);

						ue = (UINT)m_pFile->UExpGolombRead(); // log2_poc_minus_4

						// Fill media type
						CSize aspect(nWidth, nHeight);
						ReduceDim(aspect);

						vih->hdr.dwPictAspectRatioX = aspect.cx;
						vih->hdr.dwPictAspectRatioY = aspect.cy;
						vih->hdr.bmiHeader.biWidth  = nWidth;
						vih->hdr.bmiHeader.biHeight = nHeight;

						CreateSequenceHeaderAVC(headerData, headerSize, vih->dwSequenceHeader, vih->cbSequenceHeader);

						delete[] headerData;

						mt.subtype = FOURCCMap(vih->hdr.bmiHeader.biCompression = 'CVEH');
						break;
					}
					case FLV_VIDEO_HM91:   // HEVC HM9.1
					case FLV_VIDEO_HM10:   // HEVC HM10.0
					case FLV_VIDEO_HEVC: { // HEVC HM11.0 & HM12.0 ...
						if (dataSize < 4 || vt.AVCPacketType != 0) {
							fTypeFlagsVideo = true;
							break;
						}

						__int64 headerOffset = m_pFile->GetPos();
						UINT32 headerSize = dataSize - 4;
						BYTE* headerData = DNew BYTE[headerSize]; // this is AVCDecoderConfigurationRecord struct

						m_pFile->ByteRead(headerData, headerSize);

						DWORD fourcc = MAKEFOURCC('H','E','V','C');
						switch (vt.CodecID) {
							case FLV_VIDEO_HM91:
								fourcc = MAKEFOURCC('H','M','9','1');
								metaHM_compatibility = 91;
								break;
							case FLV_VIDEO_HM10:
								fourcc = MAKEFOURCC('H','M','1','0');
								metaHM_compatibility = 100;
								break;
							case FLV_VIDEO_HEVC:
								if (metaHM_compatibility >= 90 && metaHM_compatibility < 100) {
									fourcc = MAKEFOURCC('H','M','9','1');
								} else if (metaHM_compatibility >= 100 && metaHM_compatibility < 110) {
									fourcc = MAKEFOURCC('H','M','1','0');
								} else if (metaHM_compatibility >= 110 && metaHM_compatibility < 120) {
									fourcc = MAKEFOURCC('H','M','1','1');
								} else if (metaHM_compatibility >= 120 && metaHM_compatibility < 130) {
									fourcc = MAKEFOURCC('H','M','1','2');
								}
								break;
						}

						vc_params_t params;
						if (!ParseAVCDecoderConfigurationRecord(headerData, headerSize, params, metaHM_compatibility)) {
							fTypeFlagsVideo = true;
							break;
						}

						// format type
						mt.formattype = FORMAT_MPEG2Video;
						MPEG2VIDEOINFO* vih = (MPEG2VIDEOINFO*)mt.AllocFormatBuffer(FIELD_OFFSET(MPEG2VIDEOINFO, dwSequenceHeader) + headerSize);
						memset(vih, 0, mt.FormatLength());
						vih->hdr.bmiHeader.biSize     = sizeof(vih->hdr.bmiHeader);
						vih->hdr.bmiHeader.biPlanes   = 1;
						vih->hdr.bmiHeader.biBitCount = 24;
						vih->dwFlags   = params.nal_length_size;
						vih->dwProfile = params.profile;
						vih->dwLevel   = params.level;
						vih->hdr.bmiHeader.biWidth  = params.width;
						vih->hdr.bmiHeader.biHeight = params.height;

						CSize aspect(params.width, params.height);
						ReduceDim(aspect);
						vih->hdr.dwPictAspectRatioX = aspect.cx;
						vih->hdr.dwPictAspectRatioY = aspect.cy;

						CreateSequenceHeaderAVC(headerData, headerSize, vih->dwSequenceHeader, vih->cbSequenceHeader);
						delete[] headerData;

						mt.subtype = FOURCCMap(vih->hdr.bmiHeader.biCompression = fourcc);

						name += L" HEVC";
						if (vt.CodecID == FLV_VIDEO_HM91) {
							name += L" HM9.1";
						} else if (vt.CodecID == FLV_VIDEO_HM10) {
							name += L" HM10.0";
						}
						break;
					}
					default:
						fTypeFlagsVideo = true;
				}
			}
		}

		if (mt.subtype != GUID_NULL) {
			CAtlArray<CMediaType> mts;
			mts.Add(mt);
			CAutoPtr<CBaseSplitterOutputPin> pPinOut(DNew CBaseSplitterOutputPin(mts, name, this, this, &hr));
			EXECUTE_ASSERT(SUCCEEDED(AddOutputPin(t.TagType, pPinOut)));
		}

		m_pFile->Seek(next);
	}

	m_rtDuration = metaDataDuration;
	m_rtNewStop = m_rtStop = m_rtDuration;

	return m_pOutputs.GetCount() > 0 ? S_OK : E_FAIL;
}

bool CFLVSplitterFilter::DemuxInit()
{
	SetThreadName((DWORD)-1, "CFLVSplitterFilter");

	if (m_pFile->IsRandomAccess()) {
		__int64 pos = max(m_DataOffset, m_pFile->GetAvailable() - 256 * 1024);
		
		if (Sync(pos)) {
			Tag t;
			AudioTag at;
			VideoTag vt;

			while (ReadTag(t) && m_pFile->GetRemaining()) {
				UINT64 next = m_pFile->GetPos() + t.DataSize;

				CBaseSplitterOutputPin* pOutPin = dynamic_cast<CBaseSplitterOutputPin*>(GetOutputPin(t.TagType));
				if (!pOutPin) {
					m_pFile->Seek(next);
					continue;
				}

				if ((t.TagType == FLV_AUDIODATA && ReadTag(at)) || (t.TagType == FLV_VIDEODATA && ReadTag(vt))) {
					m_rtDuration = max(m_rtDuration, 10000i64 * t.TimeStamp + pOutPin->GetOffset());
				}

				m_pFile->Seek(next);
			}
		}
	}

	return true;
}

void CFLVSplitterFilter::DemuxSeek(REFERENCE_TIME rt)
{
	if (!m_rtDuration || rt <= 0) {
		m_pFile->Seek(m_DataOffset);
	} else if (!m_IgnorePrevSizes) {
		NormalSeek(rt);
	} else {
		AlternateSeek(rt);
	}
}

void CFLVSplitterFilter::NormalSeek(REFERENCE_TIME rt)
{
	bool fAudio = !!GetOutputPin(FLV_AUDIODATA);
	bool fVideo = !!GetOutputPin(FLV_VIDEODATA);

	__int64 pos	= 0;

	if (m_sps.GetCount() > 1) {
		int i	= range_bsearch(m_sps, rt);
		pos		= i >= 0 ? m_sps[i].fp : 0;
	}

	if (!pos) {
		pos = m_DataOffset + (__int64)(double(m_pFile->GetLength() - m_DataOffset) * rt / m_rtDuration);
	}

	if (pos > m_pFile->GetAvailable()) {
		return;
	}

	if (!Sync(pos)) {
		m_pFile->Seek(m_DataOffset);
		return;
	}

	if (m_IgnorePrevSizes) {
		return AlternateSeek(rt);
	}

	Tag t;
	AudioTag at;
	VideoTag vt;

	while (ReadTag(t)) {
		pos = m_pFile->GetPos() + t.DataSize;

		CBaseSplitterOutputPin* pOutPin = dynamic_cast<CBaseSplitterOutputPin*>(GetOutputPin(t.TagType));
		if (!pOutPin) {
			m_pFile->Seek(pos);
			continue;
		}

		t.TimeStamp += (pOutPin->GetOffset() / 10000i64);

		if (10000i64 * t.TimeStamp >= rt) {
			m_pFile->Seek(m_pFile->GetPos() - 15);
			break;
		}

		m_pFile->Seek(pos);
	}

	while (m_pFile->GetPos() >= m_DataOffset && (fAudio || fVideo) && ReadTag(t)) {
		__int64 prev = max(m_pFile->GetPos() - 15 - t.PreviousTagSize - 4, 0);

		CBaseSplitterOutputPin* pOutPin = dynamic_cast<CBaseSplitterOutputPin*>(GetOutputPin(t.TagType));
		if (pOutPin) {
			t.TimeStamp += (pOutPin->GetOffset() / 10000i64);

			if (10000i64 * t.TimeStamp <= rt) {
				if (t.TagType == FLV_AUDIODATA && ReadTag(at)) {
					fAudio = false;
				} else if (t.TagType == FLV_VIDEODATA && ReadTag(vt) && vt.FrameType == 1) {
					fVideo = false;
					fAudio = false;
				}
			}
		}
		m_pFile->Seek(prev);
	}

	if (fAudio || fVideo) {
		m_pFile->Seek(m_DataOffset);
	}
}

void CFLVSplitterFilter::AlternateSeek(REFERENCE_TIME rt)
{
	bool hasAudio = !!GetOutputPin(FLV_AUDIODATA);
	bool hasVideo = !!GetOutputPin(FLV_VIDEODATA);

	__int64 estimPos = m_DataOffset + (__int64)(double(m_pFile->GetAvailable() - m_DataOffset) * rt / m_rtDuration);

	while (true) {
		estimPos -= 256 * KILOBYTE;
		if (estimPos < m_DataOffset) {
			estimPos = m_DataOffset;
		}

		bool foundAudio = !hasAudio;
		bool foundVideo = !hasVideo;
		__int64 bestPos = estimPos;

		if (Sync(bestPos)) {
			Tag t;

			while (ReadTag(t) && t.TimeStamp * 10000i64 <= (rt - UNITS/2)) {
				__int64 cur = m_pFile->GetPos() - 15;
				__int64 next = cur + 15 + t.DataSize;

				AudioTag at;
				VideoTag vt;

				if (hasAudio && t.TagType == FLV_AUDIODATA && ReadTag(at)) {
					foundAudio = true;
					if (!hasVideo) {
						bestPos = cur;
					}
				} else if (hasVideo && t.TagType == FLV_VIDEODATA && ReadTag(vt) && vt.FrameType == 1) {
					foundVideo = true;
					bestPos = cur;
				}

				m_pFile->Seek(next);
			}
		}

		if (foundAudio && foundVideo) {
			m_pFile->Seek(bestPos);
			return;
		} else if (estimPos == m_DataOffset) {
			m_pFile->Seek(m_DataOffset);
			return;
		}
	}
}

bool CFLVSplitterFilter::DemuxLoop()
{
	HRESULT hr = S_OK;

	CAutoPtr<Packet> p;

	Tag t;
	AudioTag at = {};
	VideoTag vt = {};

	while (SUCCEEDED(hr) && !CheckRequest(NULL)) {

		if (!ReadTag(t)) {
			break;
		}

		if (!m_pFile->GetRemaining(m_pFile->IsStreaming())) {
			m_pFile->WaitAvailable(1500, t.DataSize);

			if (!m_pFile->GetRemaining(m_pFile->IsStreaming())) {
				break;
			}
		}

		__int64 next = m_pFile->GetPos() + t.DataSize;

		if ((t.DataSize > 0) && (t.TagType == FLV_AUDIODATA && ReadTag(at) || t.TagType == FLV_VIDEODATA && ReadTag(vt))) {
			if (t.TagType == FLV_VIDEODATA) {
				if (vt.FrameType == 5) {
					goto NextTag;    // video info/command frame
				}
				if (vt.CodecID == FLV_VIDEO_VP6) {
					m_pFile->BitRead(8);
				} else if (vt.CodecID == FLV_VIDEO_VP6A) {
					m_pFile->BitRead(32);
				} else if (IsAVCCodec(vt.CodecID)) {
					if (vt.AVCPacketType != 1) {
						goto NextTag;
					}

					t.TimeStamp += vt.tsOffset;
				}
			}
			if (t.TagType == FLV_AUDIODATA && at.SoundFormat == FLV_AUDIO_AAC) {
				if (m_pFile->BitRead(8) != 1) {
					goto NextTag;
				}
			}
			__int64 dataSize = next - m_pFile->GetPos();
			if (dataSize <= 0) {
				goto NextTag;
			}

			m_pFile->WaitAvailable(1500, dataSize);
			
			p.Attach(DNew Packet());
			p->TrackNumber	= t.TagType;
			p->rtStart		= 10000i64 * t.TimeStamp;
			p->rtStop		= p->rtStart + 1;
			p->bSyncPoint	= t.TagType == FLV_VIDEODATA ? vt.FrameType == 1 : true;

			p->SetCount((size_t)dataSize);
			m_pFile->ByteRead(p->GetData(), p->GetCount());

			hr = DeliverPacket(p);
		}
NextTag:
		m_pFile->Seek(next);
	}

	return true;
}

// IKeyFrameInfo

STDMETHODIMP CFLVSplitterFilter::GetKeyFrameCount(UINT& nKFs)
{
	CheckPointer(m_pFile, E_UNEXPECTED);
	nKFs = m_sps.GetCount();
	return S_OK;
}

STDMETHODIMP CFLVSplitterFilter::GetKeyFrames(const GUID* pFormat, REFERENCE_TIME* pKFs, UINT& nKFs)
{
	CheckPointer(pFormat, E_POINTER);
	CheckPointer(pKFs, E_POINTER);
	CheckPointer(m_pFile, E_UNEXPECTED);

	if (*pFormat != TIME_FORMAT_MEDIA_TIME) {
		return E_INVALIDARG;
	}

	for (nKFs = 0; nKFs < m_sps.GetCount(); nKFs++) {
		pKFs[nKFs] = m_sps[nKFs].rt;
	}

	return S_OK;
}

//
// CFLVSourceFilter
//

CFLVSourceFilter::CFLVSourceFilter(LPUNKNOWN pUnk, HRESULT* phr)
	: CFLVSplitterFilter(pUnk, phr)
{
	m_clsid = __uuidof(this);
	m_pInput.Free();
}
