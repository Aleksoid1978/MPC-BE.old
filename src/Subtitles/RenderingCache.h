/*
 * (C) 2013-2014 see Authors.txt
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

#include <atlcoll.h>

template<typename K, typename V, class KTraits = CElementTraits<K>, class VTraits = CElementTraits<V>>
class CRenderingCache : private CAtlMap<K, POSITION, KTraits>
{
private:
	size_t m_maxSize;
	struct CPositionValue {
		POSITION pos;
		V value;
	};
	CAtlList<CPositionValue> m_list;

public:
	CRenderingCache(size_t maxSize) : m_maxSize(maxSize) {};

	bool Lookup(KINARGTYPE key, _Out_ typename VTraits::OUTARGTYPE value) {
		POSITION pos;
		bool bFound = __super::Lookup(key, pos);

		if (bFound) {
			m_list.MoveToHead(pos);
			value = m_list.GetHead().value;
		}

		return bFound;
	};

	POSITION SetAt(KINARGTYPE key, typename VTraits::INARGTYPE value) {
		POSITION pos;
		bool bFound = __super::Lookup(key, pos);

		if (bFound) {
			m_list.MoveToHead(pos);
			CPositionValue& posVal = m_list.GetHead();
			pos = posVal.pos;
			posVal.value = value;
		} else {
			if (m_list.GetCount() > m_maxSize - 1) {
				__super::RemoveAtPos(m_list.RemoveTail().pos);
			}
			pos = __super::SetAt(key, m_list.AddHead());
			CPositionValue& posVal = m_list.GetHead();
			posVal.pos = pos;
			posVal.value = value;
		}

		return pos;
	};

	void Clear() {
		__super::RemoveAll();
	}
};

template <class Key>
class CKeyTraits : public CElementTraits<Key>
{
public:
	static ULONG Hash(_In_ const Key& element) {
		return element.GetHash();
	};

	static bool CompareElements(_In_ const Key& element1, _In_ const Key& element2) {
		return (element1 == element2);
	};
};

class STSStyle;

class CTextDimsKey
{
private:
	ULONG m_hash;

protected:
	CString m_str;
	CAutoPtr<STSStyle> m_style;

public:
	CTextDimsKey(const CStringW& str, const STSStyle& style);
	CTextDimsKey(const CTextDimsKey& textDimsKey);

	ULONG GetHash() const { return m_hash; };

	void UpdateHash();

	bool operator==(const CTextDimsKey& textDimsKey) const;
};

class CWord;

class COutlineKey : public CTextDimsKey
{
private:
	ULONG m_hash;

protected:
	double m_scalex, m_scaley;
	CPoint m_org;

public:
	COutlineKey(const CWord* word, CPoint org);
	COutlineKey(const COutlineKey& outLineKey);

	ULONG GetHash() const { return m_hash; };

	void UpdateHash();

	bool operator==(const COutlineKey& outLineKey) const;
};

class COverlayKey : public COutlineKey
{
private:
	CPoint m_subp;
	ULONG m_hash;

public:
	COverlayKey(const CWord* word, CPoint p, CPoint org);
	COverlayKey(const COverlayKey& overlayKey);

	ULONG GetHash() const { return m_hash; };

	void UpdateHash();

	bool operator==(const COverlayKey& overlayKey) const;
};
