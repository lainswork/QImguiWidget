#pragma once
#include <QObject>
#include <QIcon>
#include <QString>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>
class ImPushButtonPlugin : public QObject,
    public QDesignerCustomWidgetInterface
{
    Q_OBJECT
#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)
        Q_PLUGIN_METADATA(IID "org.qt-project.Qt.ImPushButtonPlugin")
#endif
        Q_INTERFACES(QDesignerCustomWidgetInterface)

public:
    explicit ImPushButtonPlugin(QObject* parent = nullptr);

    bool isContainer() const override;
    bool isInitialized() const override;
    QIcon icon() const override;
    QString domXml() const override;
    QString group() const override;
    QString includeFile() const override;
    QString name() const override;
    QString toolTip() const override;
    QString whatsThis() const override;
    QWidget* createWidget(QWidget* parent) override;
    void initialize(QDesignerFormEditorInterface* core) override;

private:
    bool initialized = false;
};


