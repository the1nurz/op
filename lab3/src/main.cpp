#include "MainWindow.h"

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int showCommand) {
    MainWindow window;
    if (!window.Create(instance, showCommand)) return 1;
    return window.Run();
}
