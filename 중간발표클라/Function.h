#ifndef Function_h__
#define Function_h__

#include "Include.h"


template <typename T> void Safe_Delete(T& pointer)
{
	if (NULL != pointer)
	{
		delete pointer;
		pointer = NULL;
	}
}


template <typename T> void Safe_Delete_Array(T& pointer)
{
	if (NULL != pointer)
	{
		delete[] pointer;
		pointer = NULL;
	}
}


template <typename T> void Safe_Release(T& pointer)
{
	if (NULL != pointer)
	{
		pointer->Release();
		pointer = NULL;
	}
}

template <typename T> void Safe_Single_Destory(T& pointer)
{
	if (NULL != pointer)
	{
		pointer->DestroyInstance();
		pointer = NULL;
	}
}


template <typename T> _ulong Safe_Com_Release(T& pointer)
{
	_ulong	iRefCnt = 0;

	if (NULL != pointer)
	{
		iRefCnt = pointer->Release();

		if (0 == iRefCnt)
			pointer = NULL;
	}

	return iRefCnt;
}

#endif