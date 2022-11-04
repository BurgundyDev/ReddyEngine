#include "Engine/Platform.h"

namespace Engine
{
	namespace Platform
	{
		int ErrorMessageBox(LPCTSTR title, LPCTSTR text)
		{
			return MessageBox(NULL, title, text, MB_OK);
		}

		int FatalErrorMessageBox()
		{
			return ErrorMessageBox("Fatal Error", "FATAL ERROR!");
		}
	}
}