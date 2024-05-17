#include <ShlObj.h>
#include <atlcomcli.h>
#include <iostream>
#include <system_error>
#pragma comment( linker, "/subsystem:\"windows\" /entry:\"wmainCRTStartup\"" )

template<typename T>
void ThrowIfFailed(HRESULT hr, T &&msg) {
    if (FAILED(hr))
        throw std::system_error{hr, std::system_category(), std::forward<T>(msg)};
}

// RAII COM
struct CComInit {
    CComInit() { ThrowIfFailed(::CoInitialize(nullptr), "CoInitialize failed"); }

    ~CComInit() { ::CoUninitialize(); }

    CComInit(CComInit const &) = delete;

    CComInit &operator=(CComInit const &) = delete;
};

// 发现视图
void FindView(CComPtr<IShellView> &spView, std::string const &interfaceName) {
    CComPtr<IShellWindows> spShellWindows;
    ThrowIfFailed(
            spShellWindows.CoCreateInstance(CLSID_ShellWindows),
            "Failed to create IShellWindows instance");

    CComVariant vtLoc(CSIDL_DESKTOP);
    CComVariant vtEmpty;
    long lhwnd;
    CComPtr<IDispatch> spdisp;
    ThrowIfFailed(
            spShellWindows->FindWindowSW(
                    &vtLoc, &vtEmpty, SWC_DESKTOP, &lhwnd, SWFO_NEEDDISPATCH, &spdisp),
            "Failed to find desktop window");

    CComQIPtr<IServiceProvider> spProv(spdisp);
    if (!spProv)
        ThrowIfFailed(E_NOINTERFACE, "Failed to get IServiceProvider interface for desktop");

    CComPtr<IShellBrowser> spBrowser;
    ThrowIfFailed(
            spProv->QueryService(SID_STopLevelBrowser, IID_PPV_ARGS(&spBrowser)),   // 获取顶层浏览器
            "Failed to get IShellBrowser for desktop");

    CComPtr<IShellView> spShellView;
    ThrowIfFailed(
            spBrowser->QueryActiveShellView(&spShellView),   // 获取当前活动的视图
            "Failed to query IShellView for desktop");

    ThrowIfFailed(
            spShellView->QueryInterface(IID_PPV_ARGS(&spView)),  // 获取视图的接口
            "Could not query desktop IShellView for interface " + interfaceName);
}


void ToggleDesktopIcons() {
    CComPtr<IShellView> spShellView;
    FindView(spShellView, "IFolderView2");   // 获取视图
    CComPtr<IFolderView2> spView;
    ThrowIfFailed(
            spShellView->QueryInterface(IID_PPV_ARGS(&spView)),  // 获取视图的接口
            "Could not query desktop IFolderView2 interface");
    DWORD flags = 0;    // 标志位
    ThrowIfFailed(
            spView->GetCurrentFolderFlags(&flags),  // 获取当前文件夹标志
            "GetCurrentFolderFlags failed");
    ThrowIfFailed(
            spView->SetCurrentFolderFlags(FWF_NOICONS, flags ^ FWF_NOICONS),    // 切换可见性
            "SetCurrentFolderFlags failed");
}


int wmain(int argc, wchar_t **argv) {
    try {
        CComInit com;
        // Toggle desktop icons
        // 图标需要获取COM视图并修改属性
        ToggleDesktopIcons();
        // Hide taskbar
        // 任务栏直接通过FindWindow函数找到窗口句柄，然后通过ShowWindow函数隐藏
        HWND taskbar = FindWindow("Shell_TrayWnd", nullptr);
        if (taskbar) {
            LONG style = GetWindowLong(taskbar, GWL_STYLE);
            SetWindowLong(taskbar, GWL_STYLE, style ^ WS_VISIBLE);  // 切换可见性
        } else {
            std::cerr << "Taskbar not found.\n";
        }
    }
    catch (std::system_error const &e) {
        std::cerr << "ERROR: " << e.what() << ", error code: " << e.code() << "\n";
        return 1;
    }

    return 0;
}

