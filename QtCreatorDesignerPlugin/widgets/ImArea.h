#pragma once
#include <QImguiWidget.h>
#include <QObject>
#include <QBitArray>
#include <QString>
class ImArea : public QImguiWidget {
    Q_OBJECT
public:
    ImArea(QWidget* parent = 0);
protected:
    void OnImguiInitialized() override;
    void QImguiBegin()override;
    void QImguiEnd()override;
private:
};
