#include "QImguiWidgetQtDesignerPlugin.h"
#include <QImguiWidget/QImguiWidget.h>
QImguiWidgetPlugin::QImguiWidgetPlugin(QObject *parent) {}
bool  QImguiWidgetPlugin::isContainer() const { return false; }
bool  QImguiWidgetPlugin::isInitialized() const {
    return initialized;
}
QIcon QImguiWidgetPlugin::icon() const {
    const char  SVG[] = {R"(<?xml version="1.0" encoding="UTF-8"?><svg width="48" height="48" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M21 43C16.2742 41.2327 12.3325 35.1851 10.3597 31.6428C9.50794 30.1134 9.95664 28.2347 11.3236 27.1411C12.8473 25.9222 15.0438 26.0438 16.4236 27.4236L18 29V17.5C18 16.1193 19.1193 15 20.5 15C21.8807 15 23 16.1193 23 17.5V23.5C23 22.1193 24.1193 21 25.5 21C26.8807 21 28 22.1193 28 23.5V25.5C28 24.1193 29.1193 23 30.5 23C31.8807 23 33 24.1193 33 25.5V27.5C33 26.1193 34.1193 25 35.5 25C36.8807 25 38 26.1193 38 27.5V35.368C38 36.4383 37.7354 37.496 37.1185 38.3707C36.0949 39.8219 34.255 42.0336 32 43C28.5 44.5 25.3701 44.6343 21 43Z" fill="#fc8abe" stroke="#333" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/><path d="M12 9L15 12" stroke="#333" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/><path d="M19 4L20 10" stroke="#333" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/><path d="M27 8L25 11" stroke="#333" stroke-width="4" stroke-linecap="round" stroke-linejoin="round"/></svg>)"};
    return QIcon(QPixmap::fromImage(QImage::fromData(SVG)));
}
QString QImguiWidgetPlugin::domXml() const {
    return QLatin1String(R"(
<ui language="c++">
  <widget class="QImguiWidget" name="imgui_widget">
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>70</width>
        <height>40</height>
      </rect>
    </property>
  </widget>
</ui>
   )");
}
QString QImguiWidgetPlugin::group() const {
    return QStringLiteral("QImgui Widget");
}
QString  QImguiWidgetPlugin::includeFile() const {
     return QStringLiteral("QImguiWidget/QImguiWidget.h");
}
QString  QImguiWidgetPlugin::name() const {
    return QStringLiteral("imguiWidget");
}
QString  QImguiWidgetPlugin::toolTip() const { return QString(""); }
QString  QImguiWidgetPlugin::whatsThis() const { return QString(""); }
QWidget *QImguiWidgetPlugin::createWidget(QWidget *parent) {
    auto *w = new QImguiWidget(parent);
    return w;
}
void QImguiWidgetPlugin::initialize(QDesignerFormEditorInterface *core) {

    if (initialized)
           return;

       initialized = true;
}

/**/
