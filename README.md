# 在QT 中使用imgui
---
## [imgui](https://github.com/ocornut/imgui) 是一个'后端无关'的C++ 即时GUI库, 英文原语'Immediate-Mode-GUI' 
> 由于imgui已事实上成为一种GUI类型,故该项目已更名为 dear imgui 

> 该GUI库特点是即时、高刷、动态UI、后端无关、轻量级、简单实用。

## 使用方式(qmake)

```cmake
# qt 项目 '.pro' 文件中加入
include(xxxxxxx/QImguiWidget/QImguiWidget.pri)
```

1. 继承QImguiWidget 并重写 RunImguiWidgets

```C++
#include <QWidget>
#include "QImguiWidget.h"
class Widget : public QImguiWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
protected:
    void RunImguiWidgets()override;
};
```

2. RunImguiWidgets 中使用imgui控件
```C++
    ImGui::Begin("### hello window" );
    ImGui::LabelText("###lable","hello text lable!");
    ImGui::End();
```

## 使用方式(cmake)

```cmake
# 在CMakeList.txt 中  add_executable 指令前加入:
include(本库路径/QImguiWidget/FindQImguiWidget.cmake)
#在 add_executable 指令添加目标文件后,加入:
target_link_libraries(${PROJECT_NAME} PRIVATE QImguiWidget)
```


## 注意事项

不能在多线程下使用,如果需要使用多线程,需要在imconfig.h中定义如下
```C++
extern thread_local struct ImGuiContext* xxxxx;
#define GImGui xxxxx
```
之后,在你的任意cpp文件中定义
```C++
thread_local struct ImGuiContext* xxxxx = nullptr;
```