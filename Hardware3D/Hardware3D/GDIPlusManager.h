/************************************************************************************************************
 * This is essentially a copy of GDIPlusManager from the DirectX                                            *
 * tutorial series on youtube at: https://www.youtube.com/playlist?list=PLqCJpWy5Fohd3S7ICFXwUomYW0Wv67pDD  *
 ************************************************************************************************************/
#pragma once
#include "WinInclude.h"

class GDIPlusManager
{
public:
	GDIPlusManager();
	~GDIPlusManager();
private:
	static ULONG_PTR token;
	static int refCount;
};
