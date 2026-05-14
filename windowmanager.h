#ifndef WINDOWMANAGER_H
#define WINDOWMANAGER_H

#include <QString>
#include <QMap>

#ifdef Q_OS_WIN
#include <windows.h>
using NativeWindow = HWND;
#elif defined(Q_OS_LINUX)
using NativeWindow = unsigned long; // X11 Window-ID
#else
using NativeWindow = void*;
#endif

class WindowManager
{
public:
    WindowManager() = delete; // reine Utility-Klasse, keine Instanz nötig

    static QMap<QString, NativeWindow> getOpenWindows();
    static void bringToFront(NativeWindow window);
};

#endif // WINDOWMANAGER_H