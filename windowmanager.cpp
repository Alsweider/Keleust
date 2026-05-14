#include "windowmanager.h"

// ════════════════════════════════════════════════
//  Windows
// ════════════════════════════════════════════════
#ifdef Q_OS_WIN
#include <windows.h>

struct EnumData {
    QMap<QString, HWND> windows;
};

static BOOL CALLBACK enumWindowsProc(HWND hwnd, LPARAM lParam)
{
    // Nur sichtbare Top-Level-Fenster mit Titel
    if (!IsWindowVisible(hwnd)) return TRUE;
    if (GetAncestor(hwnd, GA_ROOT) != hwnd) return TRUE;

    wchar_t title[256];
    if (GetWindowTextW(hwnd, title, 256) == 0) return TRUE;

    auto* data = reinterpret_cast<EnumData*>(lParam);
    data->windows[QString::fromWCharArray(title)] = hwnd;
    return TRUE;
}

QMap<QString, NativeWindow> WindowManager::getOpenWindows()
{
    EnumData data;
    EnumWindows(enumWindowsProc, reinterpret_cast<LPARAM>(&data));
    return data.windows;
}

void WindowManager::bringToFront(NativeWindow hwnd)
{
    if (!hwnd) return;

    // Fenster wiederherstellen falls minimiert
    if (IsIconic(hwnd))
        ShowWindow(hwnd, SW_RESTORE);

    // Schritt 1: Temporär als Always-on-Top setzen
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // Schritt 2: Alt-Tastendruck simulieren → hebt Windows Foreground Lock auf
    keybd_event(VK_MENU, 0, 0, 0);
    keybd_event(VK_MENU, 0, KEYEVENTF_KEYUP, 0);

    // Schritt 3: Fenster in den Vordergrund
    DWORD currentThread = GetCurrentThreadId();
    DWORD targetThread  = GetWindowThreadProcessId(hwnd, nullptr);
    AttachThreadInput(currentThread, targetThread, TRUE);
    SetForegroundWindow(hwnd);
    BringWindowToTop(hwnd);
    SetFocus(hwnd);
    AttachThreadInput(currentThread, targetThread, FALSE);

    // Schritt 4: Always-on-Top wieder entfernen
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

// ════════════════════════════════════════════════
//  Linux (X11)
// ════════════════════════════════════════════════
#elif defined(Q_OS_LINUX)
#include <X11/Xlib.h>
#include <X11/Xatom.h>

QMap<QString, NativeWindow> WindowManager::getOpenWindows()
{
    QMap<QString, NativeWindow> windows;
    Display* display = XOpenDisplay(nullptr);
    if (!display) return windows;

    Window root = DefaultRootWindow(display);
    Atom netClientList = XInternAtom(display, "_NET_CLIENT_LIST", True);
    Atom actualType;
    int actualFormat;
    unsigned long nItems, bytesAfter;
    unsigned char* data = nullptr;

    if (XGetWindowProperty(display, root, netClientList, 0, ~0L, False,
                           XA_WINDOW, &actualType, &actualFormat,
                           &nItems, &bytesAfter, &data) == Success) {
        auto* wins = reinterpret_cast<Window*>(data);
        for (unsigned long i = 0; i < nItems; i++) {
            char* name = nullptr;
            if (XFetchName(display, wins[i], &name) && name) {
                windows[QString::fromUtf8(name)] = wins[i];
                XFree(name);
            }
        }
        XFree(data);
    }
    XCloseDisplay(display);
    return windows;
}

void WindowManager::bringToFront(NativeWindow wid)
{
    if (!wid) return;
    Display* display = XOpenDisplay(nullptr);
    if (!display) return;

    XEvent event = {};
    event.xclient.type         = ClientMessage;
    event.xclient.window       = wid;
    event.xclient.message_type = XInternAtom(display, "_NET_ACTIVE_WINDOW", False);
    event.xclient.format       = 32;
    event.xclient.data.l[0]   = 1;
    event.xclient.data.l[1]   = CurrentTime;

    XSendEvent(display, DefaultRootWindow(display), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &event);
    XFlush(display);
    XCloseDisplay(display);
}

// ════════════════════════════════════════════════
//  Fallback (macOS / unbekannt)
// ════════════════════════════════════════════════
#else
QMap<QString, NativeWindow> WindowManager::getOpenWindows() { return {}; }
void WindowManager::bringToFront(NativeWindow) {}
#endif