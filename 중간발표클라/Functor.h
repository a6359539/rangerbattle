#ifndef Functor_h__
#define Functor_h__

#include "Typedef.h"

	// for vector or list
	class CRelease_Single
	{
	public:
		explicit CRelease_Single(void) {}
		~CRelease_Single(void) {}
	public:
		template <typename T> void operator () (T& Pointer)
		{
			if (Pointer != NULL)
			{
				Pointer->Release();
				Pointer = NULL;
			}
		}
	};

	// for map
	class CRelease_Pair
	{
	public:
		explicit CRelease_Pair(void) {}
		~CRelease_Pair(void) {}
	public:
		template <typename T> void operator () (T& Pair)
		{
			if (Pair.second != NULL)
			{
				Pair.second->Release();
				Pair.second = NULL;
			}
		}
	};



	class CTag_Finder
	{
	public:
		explicit CTag_Finder(const _tchar* pTag) : m_pTargetTag(pTag) {}
		~CTag_Finder(void) {}
	public:
		template <typename T> _bool operator () (const T& Pair)
		{
			if (0 == lstrcmp(m_pTargetTag, Pair.first))
				return true;
			return false;
		}
	private:
		const _tchar*			m_pTargetTag;
	};



	class CNumFinder
	{
	public:
		explicit CNumFinder(_uint uiNum) : m_uiNum(uiNum) {}
		~CNumFinder(void) {}
	public:
		template <typename T> _bool operator () (T& Pair)
		{
			if (m_uiNum == Pair.first)
				return TRUE;

			return false;
		}
	private:
		_uint				m_uiNum;
	};


#endif
