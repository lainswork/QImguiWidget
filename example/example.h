#pragma once
#include <QImguiWidget/QImguiWidget.h>
class example :public QImGuiInterface, public QImguiSingleton<example> {
    Q_OBJECT
public:
    example();
    void OnImguiFrameShow()  override;
    void preGlWidgetChange() override;
    void onGlWidgetChanged() override;
};