#pragma once
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QColor>
#include <QSharedPointer>
#include <QScopedPointer>
#include <QMouseEvent>
#include "imgui.h"

class QImguiWidget : public QOpenGLWidget {
    Q_OBJECT
public:
    QImguiWidget(QWidget* parent);
    ~QImguiWidget();
   
    static ImFontAtlas* GetFontAtlas();

protected:/*Interface*/
    virtual void RunImguiWidgets();
    virtual void OnImguiInitialized();
private:/*qt impl backend*/
    void InitImgui();
    void QtImguiImplNewFarme();
protected:/*opengl impl backend*/
    //override functions
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void closeEvent(QCloseEvent* event)override;
private:
    //opengl utils
    void QtOpenGlDevicesCreate();
    void QtOpenGlDevicesClean();
    void QtOpenGlNewFarme();
    void QtOpenGlSetClearRenderTarget();
    void QtOpenGlRenderData();
    GLuint CreateTexture(uint8_t* data, int w, int h, int fmt);
private:/*qt event override functions for input*/
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent* event)override;
#if QT_CONFIG(wheelevent)
    void wheelEvent(QWheelEvent* event) override;
#endif
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void inputMethodEvent(QInputMethodEvent* event) override;
private:
    void* impl = nullptr;
    GLuint FontTex = {};
    //QSharedPointer<ImFontAtlas> FontAtlas;
    QColor ClearColor = {20,20,20,20};
};
