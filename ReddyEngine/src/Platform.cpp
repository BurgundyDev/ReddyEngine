#include "Engine/Platform.h"
#include <Engine/Utils.h>

#include <windows.h>

#define ERROR_MSG_LENGTH 256

namespace Engine
{
	namespace Platform
	{
		int ShowErrorMessageBox(const std::string& title, const std::string& text)
		{
			return MessageBox(NULL, text.c_str(), title.c_str(), MB_OK);
		}

		int ShowFatalErrorMessageBox(const std::string& errMsg)
		{
			return ShowErrorMessageBox("Fatal Error", errMsg);
		}
	}
}