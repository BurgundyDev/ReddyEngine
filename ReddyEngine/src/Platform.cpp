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
			return MessageBox(NULL, title.c_str(), text.c_str(), MB_OK);
		}

		int ShowFatalErrorMessageBox(const std::string& errMsg)
		{
			return ShowErrorMessageBox(errMsg, "Fatal Error");
		}

		/*const std::string GetLastErrorMessage()
		{
			assert()
				wchar_t errorMsg[ERROR_MSG_LENGTH];

			FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)errorMsg,
				0, NULL);

			return Utils::wideToUtf8(errorMsg);
		}*/
	}
}