#include "ImAreaPlugin.h"
#include "ImArea.h"
/*********************************************************************************************************************************/
ImAreaPlugin::ImAreaPlugin(QObject* parent) {}
bool ImAreaPlugin::isContainer() const { return true; }
bool ImAreaPlugin::isInitialized() const {
    return initialized;
}
QIcon ImAreaPlugin::icon() const {
    const char  SVG[] = {R"(<?xml version="1.0" encoding="UTF-8"?><svg width="48" height="48" viewBox="0 0 48 48" fill="none" xmlns="http://www.w3.org/2000/svg"><path d="M42 8H6C4.89543 8 4 8.89543 4 10V38C4 39.1046 4.89543 40 6 40H42C43.1046 40 44 39.1046 44 38V10C44 8.89543 43.1046 8 42 8Z" fill="#fc8abe" stroke="#333" stroke-width="4"/></svg>)"};


    return QIcon(QPixmap::fromImage(QImage::fromData(SVG)));
}
QString ImAreaPlugin::domXml() const {
    return QLatin1String(R"(
<ui language="c++">
  <widget class="ImArea" name="imArea">
    <property name="geometry">
      <rect>
        <x>0</x>
        <y>0</y>
        <width>100</width>
        <height>100</height>
      </rect>
    </property>
  </widget>
</ui>
   )");
}
QString ImAreaPlugin::group() const {
    return QStringLiteral("QImgui Widgets");
}
QString ImAreaPlugin::includeFile()const {
    return QStringLiteral("ImArea.h");
}
QString ImAreaPlugin::name()const {
    return QStringLiteral("ImArea");
}
QString ImAreaPlugin::toolTip() const { return QString(""); }
QString ImAreaPlugin::whatsThis() const { return QString(""); }
QWidget* ImAreaPlugin::createWidget(QWidget* parent) {
    auto* w = new ImArea(parent);
    return w;
}
void ImAreaPlugin::initialize(QDesignerFormEditorInterface* core) {

    if (initialized)
        return;

    initialized = true;
}