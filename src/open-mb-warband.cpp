#include "core/engine/engine.h"

#if defined(_WIN32) && defined(_RELEASE)
    #include <Windows.h>
#endif // _WIN32

#if defined(_DEBUG) || !defined(_WIN32)
int main(int argc, char* argv[])
#else
int WINAPI WinMain(HINSTANCE instance, HINSTANCE last_instance, LPSTR cmd_line, int flags)
#endif // _DEBUG
{
    Engine engine("Open M&W Warband", 1024, 768);
    engine.run();

    return 0;
}
