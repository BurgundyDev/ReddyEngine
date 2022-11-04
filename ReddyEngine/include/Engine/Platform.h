#pragma once

#include <windows.h>

namespace Engine
{
	namespace Platform
	{
		int ErrorMessageBox(LPCTSTR  title, LPCTSTR  text);
		int FatalErrorMessageBox();
	}
}
