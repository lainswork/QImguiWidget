#pragma once
#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLExtraFunctions>
#include <QColor>
#include <QSharedPointer>
#include <QMouseEvent>
#include "imgui.h"

class QImguiWidget : public QOpenGLWidget, private QOpenGLExtraFunctions {
    Q_OBJECT
public:
    QImguiWidget(QWidget* parent);
    ~QImguiWidget();
    bool InitImgui(QSharedPointer<ImFontAtlas> FontAtlas = QSharedPointer<ImFontAtlas>(new ImFontAtlas()));
    QSharedPointer<ImFontAtlas> GetFontAtlas() { return FontAtlas; }

protected:/*Interface*/
    virtual void RunImguiWidgets();
private:/*qt impl backend*/
    void QtImguiImplNewFarme();
protected:/*opengl impl backend*/
    //override functions
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    //opengl utils
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
    void* impl;
    GLuint FontTex = {};
    QSharedPointer<ImFontAtlas> FontAtlas;
    QColor ClearColor = {20,20,20,20};
};
