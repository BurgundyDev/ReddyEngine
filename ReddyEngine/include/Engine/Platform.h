#pragma once

#include <string>

namespace Engine
{
	namespace Platform
	{
		int ShowErrorMessageBox(const std::string& title, const std::string& text);
		int ShowFatalErrorMessageBox(const std::string& errorMsg);
	}
}
