#pragma once
#include <QImguiWidget.h>
#include <QObject>
#include <QBitArray>
#include <QString>
class ImPushButton : public QImguiWidget {
    Q_OBJECT
public:
    ImPushButton::ImPushButton(QWidget* parent = 0);
protected:
    void OnImguiInitialized() override;
    void QImguiBegin()override;
    void QImguiContent()override;
    void QImguiEnd()override;
};
