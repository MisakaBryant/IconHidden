# 一键隐藏桌面图标与任务栏

手动设置隐藏图标和任务栏太麻烦了，网上搜了一下，写（chao）了一个同时隐藏桌面图标和任务栏的。

### 使用方法

推荐搭配`AutoHotKey`使用，[点此下载](https://www.autohotkey.com)，新建脚本如下：

```ahk
#Requires AutoHotkey v2.0

!h::Run "your_path\IconHidden.exe"
```

保存`.ahk`脚本后放在`启动`文件夹(AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup)中，每次开机后按下`Alt+H`即可切换隐藏桌面图标和任务栏，热键组合和触发条件可以看教程自己改。

### 关于源码

参考了博客 https://blog.csdn.net/qq_39529180/article/details/132375276

源代码依赖`atlcomcli.h`库，如果没有，在 Visual Studio Installer 修改中勾选安装最新`ATL`即可。

不知道为什么通过`SetWindowLong`修改任务栏的`WS_VISIBLE`属性对于win10不起作用，貌似只能禁用不能隐藏，怀疑没有重新渲染，只能用`ShowWindow`函数隐藏和显示。