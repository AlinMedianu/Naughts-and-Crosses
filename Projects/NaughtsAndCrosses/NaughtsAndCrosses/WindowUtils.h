#ifndef WINDOWSUTILS
#define WINDOWSUTILS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
#include <sstream>


//open our main window
bool InitMainWindow(int width, int height, HINSTANCE hInstance, const std::string& appName, WNDPROC mssgHandler);
//repeatedly run update+render as fast as possible
int Run(void(*pUpdate)(float), void(*pRender)(float));
//our windows code will get lots of messages, some will involve
//resizing or dragging or ALT+ENTER and the windows code needs to
//tell D3D what's changed.
void OnResize_internal();
LRESULT DefaultMssgHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

//all windows apps have an application instance and main window handle
HINSTANCE GetAppInst();
HWND GetMainWnd();
//height and width
void GetClientExtents(int& width, int& height);


#endif
