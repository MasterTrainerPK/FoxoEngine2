#if defined(FE_PLAT_WINDOWS) && defined(FE_CONF_DIST)
#	include <Windows.h>
#endif

#include "Application.h"

#if defined(FE_PLAT_WINDOWS) && defined(FE_CONF_DIST)
int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
	feApplication* application = feApplication::CreateInstance();
	application->Start();
	feApplication::DeleteInstance(application);
}