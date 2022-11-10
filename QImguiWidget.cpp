#include "QImguiWidget.h"
#include <QApplication>
#include <QMouseEvent>
#include <QElapsedTimer>
#include <QDebug>
#include <QClipboard>
#include <QTimer>
#include <QVector>
#ifdef ANDROID
#define GL_VERTEX_ARRAY_BINDING 0x85B5 // Missing in android as of May 2020
#define USE_GLSL_ES
#endif

#ifdef USE_GLSL_ES
#define IMGUIRENDERER_GLSL_VERSION "#version 300 es\n"
#else
#define IMGUIRENDERER_GLSL_VERSION "#version 330\n"
#endif


/*Qt opengl backend res*/
struct QImguiWidgetImplContext
{
	ImGuiContext* imgui = nullptr;
	int          ShaderHandle = 0, VertHandle = 0, FragHandle = 0;
	int          AttribLocationTex = 0, AttribLocationProjMtx = 0;
	int          AttribLocationPosition = 0, AttribLocationUV = 0, AttribLocationColor = 0;
	unsigned int VboHandle = 0, VaoHandle = 0, ElementsHandle = 0;
	int            fb_width = 0;
	int            fb_height = 0;
	bool frameDrawing = false;
	QVector<ImDrawList*> DrawData;
	QElapsedTimer                                    Timer;
	QImguiWidgetImplContext() {
		Timer.start();
	}
	~QImguiWidgetImplContext() {

	}
};

QImguiWidget::QImguiWidget(QWidget* parent) : QOpenGLWidget(parent) {
	//鼠标跟踪
	this->setMouseTracking(true);
	//接收输入法内容
	this->setAttribute(Qt::WA_InputMethodEnabled);
	//允许切换输入法
	this->setAttribute(Qt::WA_KeyCompression);

	this->FontAtlas = QSharedPointer<ImFontAtlas>(new ImFontAtlas());
	this->impl = new QImguiWidgetImplContext();
	//我们在widget窗口销毁时进行OPENGL资源'后处理'
	//QObject::connect(this, &QWidget::destroyed, [this]() {this->QtOpenGlDevicesClean(); });
	// timer 定时刷新
	QTimer* timer = new QTimer(this);
	QObject::connect(timer, &QTimer::timeout, [this]() {
		this->update();
		});
	timer->start(16);
}

QImguiWidget::~QImguiWidget() {
	//this->QtOpenGlDevicesClean();
	if (this->impl) {
		QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
		for (auto i : ctx->DrawData)
			ImGui::MemFree(i);
		if (ctx->imgui)ImGui::DestroyContext(ctx->imgui);
		delete reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	}
}

QImguiWidget* QImguiWidget::InitImgui(QSharedPointer<ImFontAtlas> FontAtlas) {
	//初始化
	this->FontAtlas.reset();
	this->FontAtlas = FontAtlas;

	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	ctx->imgui = ImGui::CreateContext(FontAtlas.get());


	ImGui::SetCurrentContext(ctx->imgui); //选定imgui上下文
	ImGui::GetIO().SetClipboardTextFn = [](void* user_data, const char* text) {
		Q_UNUSED(user_data);
		QGuiApplication::clipboard()->setText(text);
	};
	ImGui::GetIO().GetClipboardTextFn = [](void* user_data) {
		Q_UNUSED(user_data);
		static QByteArray  g_currentClipboardText = QGuiApplication::clipboard()->text().toUtf8();
		return (const char*)g_currentClipboardText.data();
	};
#ifndef QT_NO_CURSOR
	ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	ImGui::GetIO().BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
#endif

	this->OnImguiInitialized();

	return this;
}
void QImguiWidget::RunImguiWidgets() {
#ifdef _DEBUG
	static bool show_demo_window = true;
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
	ImGui::GetForegroundDrawList()->AddCircleFilled(ImGui::GetIO().MousePos, 5, 0xff0000ff);
	ImGui::GetForegroundDrawList()->AddText({ 10,10 }, 0xff0000ff, QString("DeltaTime[ %1 ] / FPS[ %2 ]").arg(ImGui::GetIO().DeltaTime).arg((int)(1.0f / ImGui::GetIO().DeltaTime)).toUtf8().data());
#endif // DEBUG
}
void QImguiWidget::OnImguiInitialized() {
#ifdef _DEBUG
	ImGui::StyleColorsLight();
#endif
}
void QImguiWidget::QtImguiImplNewFarme() {
	//选定imgui上下文
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	ImGui::SetCurrentContext(ctx->imgui);

	ImGuiIO& io = ImGui::GetIO();
	//显示大小
	io.DisplaySize = ImVec2(this->size().width(), this->size().height());
	//分辨率
	io.DisplayFramebufferScale = ImVec2(this->devicePixelRatio(), this->devicePixelRatio());
	//帧时间差
	if (ctx->Timer.elapsed() <= 0)
		io.DeltaTime = 0.001f;
	else
		io.DeltaTime = (float)ctx->Timer.elapsed() / 1000.f;
	ctx->Timer.restart();
	//imgui更新系统鼠标位置
#ifndef QT_NO_CURSOR
	if (io.WantSetMousePos) {
		QCursor cursor = this->cursor();
		cursor.setPos({ (int)io.MousePos.x, (int)io.MousePos.y });
		this->setCursor(cursor);
	}
	// imgui改变鼠标形态
	if (!(io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)) {
		const ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
		if (io.MouseDrawCursor || (imgui_cursor == ImGuiMouseCursor_None)) {
			this->setCursor(Qt::BlankCursor);
		}
		else {
			switch (imgui_cursor)
			{
			case ImGuiMouseCursor_Arrow:
				this->setCursor(Qt::ArrowCursor);
				break;
			case ImGuiMouseCursor_TextInput:
				this->setCursor(Qt::IBeamCursor);
				break;
			case ImGuiMouseCursor_ResizeAll:
				this->setCursor(Qt::SizeAllCursor);
				break;
			case ImGuiMouseCursor_ResizeNS:
				this->setCursor(Qt::SizeVerCursor);
				break;
			case ImGuiMouseCursor_ResizeEW:
				this->setCursor(Qt::SizeHorCursor);
				break;
			case ImGuiMouseCursor_ResizeNESW:
				this->setCursor(Qt::SizeBDiagCursor);
				break;
			case ImGuiMouseCursor_ResizeNWSE:
				this->setCursor(Qt::SizeFDiagCursor);
				break;
			case ImGuiMouseCursor_Hand:
				this->setCursor(Qt::PointingHandCursor);
				break;
			case ImGuiMouseCursor_NotAllowed:
				this->setCursor(Qt::ForbiddenCursor);
				break;
			default:
				this->setCursor(Qt::ArrowCursor);
				break;
			}
		}
	}
#endif
}
GLuint QImguiWidget::CreateTexture(uint8_t* data, int w, int h, int fmt) {
	GLuint tex{};
	GLint  last_texture{};
    auto* gl =this->context();
    auto* funs = gl->extraFunctions();
	funs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	funs->glGenTextures(1, &tex);
	funs->glBindTexture(GL_TEXTURE_2D, tex);
	funs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	funs->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	funs->glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, data);
	funs->glBindTexture(GL_TEXTURE_2D, last_texture);

	/*
	当QImguiWidget窗口作为 QDockWidget 内容时 : QImguiWidget窗口 从依附的主窗口挪出时，会造成OpenGl上下文切换
	即: this->context() 所得到的指针会变，原OpenGl上下文 会被销毁，因此，我们要在原OpenGl上下文销毁前，删除用该
	上下文创造的所有OpenGl资源。方法:"响应 QOpenGLContext::aboutToBeDestroyed 信号"
	*/
    QObject::connect(this->context(), &QOpenGLContext::aboutToBeDestroyed, [gl,tex]() {
         gl->extraFunctions()->glDeleteTextures(1, &tex);
        });

	return tex;
};
void QImguiWidget::initializeGL() {
	this->QtOpenGlDevicesClean();
	// 初始化opengl api
	this->context()->extraFunctions()->initializeOpenGLFunctions();

	this->QtOpenGlDevicesCreate();
}
void QImguiWidget::resizeGL(int w, int h) {
	this->context()->extraFunctions()->glViewport(0.0f, 0.0f, w, h);
}
void QImguiWidget::paintGL() {


	// 选定imgui上下文
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);

	if (!ctx || !ctx->imgui)
		return QOpenGLWidget::paintGL();


	// frameDrawing 的存在是为了防止递归调用，RunImguiWidgets 中可能存在阻塞式的QT窗口
	if (!ctx->frameDrawing) {
		ctx->frameDrawing = true;
		ImGui::SetCurrentContext(ctx->imgui);
		this->QtImguiImplNewFarme();
		this->QtOpenGlNewFarme();
		ImGui::NewFrame();
		this->RunImguiWidgets();
		ImGui::EndFrame();
		// imgui 生成绘制数据
		ImGui::Render();

		// 将绘制数据放到 ctx->DrawData 中
		auto* DrawData = ImGui::GetDrawData();

		const ImGuiIO& io = ImGui::GetIO();
		ctx->fb_width = (int)(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		ctx->fb_height = (int)(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (ctx->fb_width == 0 || ctx->fb_height == 0)
			return;

		DrawData->ScaleClipRects(io.DisplayFramebufferScale);

        for (int i = 0; i < ctx->DrawData.count(); i++) {
			ctx->DrawData[i]->_ResetForNewFrame();
			ctx->DrawData[i]->CmdBuffer = DrawData->CmdLists[i]->CmdBuffer;
			ctx->DrawData[i]->IdxBuffer = DrawData->CmdLists[i]->IdxBuffer;
			ctx->DrawData[i]->VtxBuffer = DrawData->CmdLists[i]->VtxBuffer;
			ctx->DrawData[i]->Flags = DrawData->CmdLists[i]->Flags;
		}

		int oriDrawListCount = ctx->DrawData.count();
		if (DrawData->CmdListsCount > oriDrawListCount) {
			int needDrawList = DrawData->CmdListsCount - oriDrawListCount;
            for (int i = 0; i < needDrawList; i++)
				ctx->DrawData.push_back(DrawData->CmdLists[oriDrawListCount + i]->CloneOutput());
		}

		ctx->frameDrawing = false;
	}
	else
	{
		qDebug() << u8"绘制旧数据";
	}

	this->QtOpenGlSetClearRenderTarget();
	this->QtOpenGlRenderData();

}
void QImguiWidget::closeEvent(QCloseEvent* event) {
	this->QtOpenGlDevicesClean();
	event->accept();
}
void QImguiWidget::QtOpenGlDevicesCreate() {
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);

	if (!ctx)
		return;
    auto* gl = this->context();
    auto* funs = gl->extraFunctions();
	GLint last_texture, last_array_buffer, last_vertex_array;
	funs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	funs->glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	funs->glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

	const GLchar* vertex_shader = IMGUIRENDERER_GLSL_VERSION "uniform mat4 ProjMtx;\n"
		"in vec2 Position;\n"
		"in vec2 UV;\n"
		"in vec4 Color;\n"
		"out vec2 Frag_UV;\n"
		"out vec4 Frag_Color;\n"
		"void main()\n"
		"{\n"
		"	Frag_UV = UV;\n"
		"	Frag_Color = Color;\n"
		"	gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
		"}\n";

	const GLchar* fragment_shader =
		IMGUIRENDERER_GLSL_VERSION "precision mediump float;"
		"uniform sampler2D Texture;\n"
		"in vec2 Frag_UV;\n"
		"in vec4 Frag_Color;\n"
		"out vec4 Out_Color;\n"
		"void main()\n"
		"{\n"
		"	Out_Color = Frag_Color * texture( Texture, Frag_UV.st);\n"
		"}\n";




	ctx->ShaderHandle = funs->glCreateProgram();
	ctx->VertHandle = funs->glCreateShader(GL_VERTEX_SHADER);
	ctx->FragHandle = funs->glCreateShader(GL_FRAGMENT_SHADER);

	funs->glShaderSource(ctx->VertHandle, 1, &vertex_shader, 0);
	funs->glShaderSource(ctx->FragHandle, 1, &fragment_shader, 0);
	funs->glCompileShader(ctx->VertHandle);
	funs->glCompileShader(ctx->FragHandle);
	funs->glAttachShader(ctx->ShaderHandle, ctx->VertHandle);
	funs->glAttachShader(ctx->ShaderHandle, ctx->FragHandle);
	funs->glLinkProgram(ctx->ShaderHandle);

	ctx->AttribLocationTex = funs->glGetUniformLocation(ctx->ShaderHandle, "Texture");
	ctx->AttribLocationProjMtx = funs->glGetUniformLocation(ctx->ShaderHandle, "ProjMtx");
	ctx->AttribLocationPosition = funs->glGetAttribLocation(ctx->ShaderHandle, "Position");
	ctx->AttribLocationUV = funs->glGetAttribLocation(ctx->ShaderHandle, "UV");
	ctx->AttribLocationColor = funs->glGetAttribLocation(ctx->ShaderHandle, "Color");

	funs->glGenBuffers(1, &ctx->VboHandle);
	funs->glGenBuffers(1, &ctx->ElementsHandle);
	funs->glGenVertexArrays(1, &ctx->VaoHandle);
	funs->glBindVertexArray(ctx->VaoHandle);
	funs->glBindBuffer(GL_ARRAY_BUFFER, ctx->VboHandle);
	funs->glEnableVertexAttribArray(ctx->AttribLocationPosition);
	funs->glEnableVertexAttribArray(ctx->AttribLocationUV);
	funs->glEnableVertexAttribArray(ctx->AttribLocationColor);

#pragma push_macro("OFFSETOF")
#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE *)0)->ELEMENT))
	funs->glVertexAttribPointer(ctx->AttribLocationPosition, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
		(GLvoid*)OFFSETOF(ImDrawVert, pos));
	funs->glVertexAttribPointer(ctx->AttribLocationUV, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert),
		(GLvoid*)OFFSETOF(ImDrawVert, uv));
	funs->glVertexAttribPointer(ctx->AttribLocationColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert),
		(GLvoid*)OFFSETOF(ImDrawVert, col));
#undef OFFSETOF
#pragma pop_macro("OFFSETOF")
	// Restore modified GL state
	funs->glBindTexture(GL_TEXTURE_2D, last_texture);
	funs->glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	funs->glBindVertexArray(last_vertex_array);


    // 表达式值捕获是CXX14 所以这里要多此一举
    auto VaoHandle = ctx->VaoHandle;
    auto ElementsHandle = ctx->ElementsHandle;
    auto VboHandle = ctx->VboHandle;
    auto FragHandle = ctx->FragHandle;
    auto VertHandle = ctx->VertHandle;
    auto ShaderHandle = ctx->ShaderHandle;

	/*
当QImguiWidget窗口作为 QDockWidget 内容时 : QImguiWidget窗口 从依附的主窗口挪出时，会造成OpenGl上下文切换
即: this->context() 所得到的指针会变，原OpenGl上下文 会被销毁，因此，我们要在原OpenGl上下文销毁前，删除用该
上下文创造的所有OpenGl资源。方法:"响应 QOpenGLContext::aboutToBeDestroyed 信号"
*/
    QObject::connect(gl, &QOpenGLContext::aboutToBeDestroyed, [
            gl,
            VaoHandle ,
            ElementsHandle ,
            VboHandle ,
            FragHandle ,
            VertHandle,
            ShaderHandle]() {

		auto* funs = gl->extraFunctions();
		funs->glDeleteVertexArrays(1, &VaoHandle);
		funs->glDeleteVertexArrays(1, &VaoHandle);
		funs->glDeleteBuffers(1, &ElementsHandle);
		funs->glDeleteBuffers(1, &VboHandle);
		funs->glDeleteShader(FragHandle);
		funs->glDeleteShader(VertHandle);
		funs->glDeleteProgram(ShaderHandle);

		});
}
void QImguiWidget::QtOpenGlDevicesClean() {
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx) {
		ctx->AttribLocationTex = {};
		ctx->AttribLocationProjMtx = {};
		ctx->AttribLocationPosition = {};
		ctx->AttribLocationUV = {};
		ctx->AttribLocationColor = {};
		FontTex = {};
		ctx->VaoHandle = {};
		ctx->ElementsHandle = {};
		ctx->VboHandle = {};
		ctx->FragHandle = {};
		ctx->VertHandle = {};
		ctx->ShaderHandle = {};
		return;
	}
}
void QImguiWidget::QtOpenGlNewFarme() {
	auto buildFontTex = [this]() {
		unsigned char* pixels = 0;
		int            width, height;
		FontAtlas->GetTexDataAsRGBA32(&pixels, &width, &height);
		FontTex = this->CreateTexture(pixels, width, height, GL_RGBA);
		FontAtlas->SetTexID((ImTextureID)1);
		//qDebug() << " GL FontAtlas Texture build!  " << FontTex;
		this->setWindowTitle(QString("Texture id %1").arg((int)FontTex));

	};
	// 此处判断发生在渲染第一次执行或更新(添加)字体后
	if (!this->FontAtlas->IsBuilt()) {
		if (FontTex) {
			if (this->context()->extraFunctions()->glIsTexture(FontTex))
				this->context()->extraFunctions()->glDeleteTextures(1, &FontTex);
			FontTex = {};
		}
		//qDebug() << u8"文字更新 重建纹理  ";
		buildFontTex();
	}
	// 此处是由于 多个widget间共享 FontAtlas, 但 QT opengl 的特性(opengl窗口对象间不共享纹理ID)，因此添加
	if (!FontTex)
	{
		//qDebug() << u8"纹理ID 不存在 重建纹理  ";
		buildFontTex();
	}
}
void QImguiWidget::QtOpenGlSetClearRenderTarget() {

	auto* funs = this->context()->extraFunctions();
	funs->glViewport(0, 0, width(), height());
	funs->glClearColor(ClearColor.redF(), ClearColor.greenF(), ClearColor.blueF(), ClearColor.alphaF());
	funs->glClear(GL_COLOR_BUFFER_BIT);
}
void QImguiWidget::QtOpenGlRenderData() {
	// 选定imgui上下文
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	ImGui::SetCurrentContext(ctx->imgui);
	const ImGuiIO& io = ImGui::GetIO();
	auto* funs = this->context()->extraFunctions();
	// Backup GL state
	GLint last_active_texture;
	funs->glGetIntegerv(GL_ACTIVE_TEXTURE, &last_active_texture);
	funs->glActiveTexture(GL_TEXTURE0);
	GLint last_program;
	funs->glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture;
	funs->glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer;
	funs->glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer;
	funs->glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array;
	funs->glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src_rgb;
	funs->glGetIntegerv(GL_BLEND_SRC_RGB, &last_blend_src_rgb);
	GLint last_blend_dst_rgb;
	funs->glGetIntegerv(GL_BLEND_DST_RGB, &last_blend_dst_rgb);
	GLint last_blend_src_alpha;
	funs->glGetIntegerv(GL_BLEND_SRC_ALPHA, &last_blend_src_alpha);
	GLint last_blend_dst_alpha;
	funs->glGetIntegerv(GL_BLEND_DST_ALPHA, &last_blend_dst_alpha);
	GLint last_blend_equation_rgb;
	funs->glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha;
	funs->glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4];
	funs->glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLint last_scissor_box[4];
	funs->glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
	GLboolean last_enable_blend = funs->glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = funs->glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = funs->glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = funs->glIsEnabled(GL_SCISSOR_TEST);

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	funs->glEnable(GL_BLEND);
	funs->glBlendEquation(GL_FUNC_ADD);
	funs->glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	funs->glDisable(GL_CULL_FACE);
	funs->glDisable(GL_DEPTH_TEST);
	funs->glEnable(GL_SCISSOR_TEST);

	// Setup viewport, orthographic projection matrix
	funs->glViewport(0, 0, (GLsizei)ctx->fb_width, (GLsizei)ctx->fb_height);
	const float ortho_projection[4][4] = {
		{2.0f / io.DisplaySize.x, 0.0f, 0.0f, 0.0f},
		{0.0f, 2.0f / -io.DisplaySize.y, 0.0f, 0.0f},
		{0.0f, 0.0f, -1.0f, 0.0f},
		{-1.0f, 1.0f, 0.0f, 1.0f},
	};
	funs->glUseProgram(ctx->ShaderHandle);
	funs->glUniform1i(ctx->AttribLocationTex, 0);
	funs->glUniformMatrix4fv(ctx->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);
	funs->glBindVertexArray(ctx->VaoHandle);

	for (int n = 0; n < ctx->DrawData.count(); n++) {
		const ImDrawList* cmd_list = ctx->DrawData[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		funs->glBindBuffer(GL_ARRAY_BUFFER, ctx->VboHandle);
		funs->glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.Size * sizeof(ImDrawVert),
			(const GLvoid*)cmd_list->VtxBuffer.Data, GL_STREAM_DRAW);

		funs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ctx->ElementsHandle);
		funs->glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx),
			(const GLvoid*)cmd_list->IdxBuffer.Data, GL_STREAM_DRAW);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback) {
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else {
				funs->glBindTexture(GL_TEXTURE_2D, (GLuint)this->FontTex);
				funs->glScissor((int)pcmd->ClipRect.x, (int)(ctx->fb_height - pcmd->ClipRect.w),
					(int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				funs->glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
					sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

	// Restore modified GL state
	funs->glUseProgram(last_program);
	funs->glBindTexture(GL_TEXTURE_2D, last_texture);
	funs->glActiveTexture(last_active_texture);
	funs->glBindVertexArray(last_vertex_array);
	funs->glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	funs->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	funs->glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	funs->glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
	if (last_enable_blend)
		funs->glEnable(GL_BLEND);
	else
		funs->glDisable(GL_BLEND);
	if (last_enable_cull_face)
		funs->glEnable(GL_CULL_FACE);
	else
		funs->glDisable(GL_CULL_FACE);
	if (last_enable_depth_test)
		funs->glEnable(GL_DEPTH_TEST);
	else
		funs->glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test)
		funs->glEnable(GL_SCISSOR_TEST);
	else
		funs->glDisable(GL_SCISSOR_TEST);
	funs->glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
	funs->glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
void QImguiWidget::mousePressEvent(QMouseEvent* event) {

	auto button = event->button();
	const bool key_pressed = true;
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx->imgui) {
		ImGui::SetCurrentContext(ctx->imgui);
		auto& io = ImGui::GetIO();
		if (button & Qt::LeftButton)
			io.AddMouseButtonEvent(0, key_pressed);
		else if (button & Qt::RightButton)
			io.AddMouseButtonEvent(1, key_pressed);
		else if (button & Qt::MidButton)
			io.AddMouseButtonEvent(2, key_pressed);
	}
	else {
		return QOpenGLWidget::mousePressEvent(event);
	}
}
void QImguiWidget::mouseReleaseEvent(QMouseEvent* event) {
	auto button = event->button();
	const bool key_pressed = false;
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx->imgui) {
		ImGui::SetCurrentContext(ctx->imgui);
		auto& io = ImGui::GetIO();
		if (button & Qt::LeftButton)
			io.AddMouseButtonEvent(0, key_pressed);
		else if (button & Qt::RightButton)
			io.AddMouseButtonEvent(1, key_pressed);
		else if (button & Qt::MidButton)
			io.AddMouseButtonEvent(2, key_pressed);
	}
	else {
		return QOpenGLWidget::mouseReleaseEvent(event);
	}

}
void QImguiWidget::mouseDoubleClickEvent(QMouseEvent* event) {
    return QOpenGLWidget::mouseDoubleClickEvent(event);
}
void QImguiWidget::mouseMoveEvent(QMouseEvent* event) {
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx->imgui) {
		ImGui::SetCurrentContext(ctx->imgui);
		ImGui::GetIO().AddMousePosEvent(event->pos().x(), event->pos().y());
	}
	else {
		return QOpenGLWidget::mouseMoveEvent(event);
	}


}
void QImguiWidget::leaveEvent(QEvent* event) {
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx->imgui) {
		ImGui::SetCurrentContext(ctx->imgui);
		ImGui::GetIO().AddMousePosEvent(-FLT_MAX, -FLT_MAX);
	}
	else {
		return QOpenGLWidget::leaveEvent(event);
	}
}
#if QT_CONFIG(wheelevent)
void QImguiWidget::wheelEvent(QWheelEvent* event) {
	QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);
	if (ctx->imgui) {
		ImGui::SetCurrentContext(ctx->imgui);
		auto& io = ImGui::GetIO();
		if (event->pixelDelta().x() != 0) {
			io.AddMouseWheelEvent(event->pixelDelta().x() / (ImGui::GetTextLineHeight()), 0);
		}
		else {
			// Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
			io.AddMouseWheelEvent(event->angleDelta().x() / 120.0f, 0);
		}
		if (event->pixelDelta().y() != 0) {
			// 5 lines per unit
			io.AddMouseWheelEvent(0, event->pixelDelta().y() / (5.0 * ImGui::GetTextLineHeight()));
		}
		else {
			// Magic number of 120 comes from Qt doc on QWheelEvent::pixelDelta()
			io.AddMouseWheelEvent(0, event->angleDelta().y() / 120.0f);
		}
	}
	else {
		return QOpenGLWidget::wheelEvent(event);
	}
}
#endif

static ImGuiKey ImGui_ImplQt_QKeyToImGuiKey(int key, QKeyEvent* event)
{
	switch (key)
	{
	case Qt::Key_Tab: return ImGuiKey_Tab;
	case Qt::Key_Left: return ImGuiKey_LeftArrow;
	case Qt::Key_Right: return ImGuiKey_RightArrow;
	case Qt::Key_Up: return ImGuiKey_UpArrow;
	case Qt::Key_Down: return ImGuiKey_DownArrow;
	case Qt::Key_PageUp: return ImGuiKey_PageUp;
	case Qt::Key_PageDown: return ImGuiKey_PageDown;
	case Qt::Key_Home: return ImGuiKey_Home;
	case Qt::Key_End: return ImGuiKey_End;
	case Qt::Key_Insert: return ImGuiKey_Insert;
	case Qt::Key_Delete: return ImGuiKey_Delete;
	case Qt::Key_Backspace: return ImGuiKey_Backspace;
	case Qt::Key_Space: return ImGuiKey_Space;
	case Qt::Key_Return: return ImGuiKey_Enter;
	case Qt::Key_Escape: return ImGuiKey_Escape;
	case Qt::Key_Apostrophe: return ImGuiKey_Apostrophe;
	case Qt::Key_Comma: return ImGuiKey_Comma;
	case Qt::Key_Semicolon: return ImGuiKey_Semicolon;
	case Qt::Key_Equal: return ImGuiKey_Equal;
	case Qt::Key_BracketLeft: return ImGuiKey_LeftBracket;
	case Qt::Key_Backslash: return ImGuiKey_Backslash;
	case Qt::Key_BracketRight: return ImGuiKey_RightBracket;
	case Qt::Key_Agrave: return ImGuiKey_GraveAccent;
	case Qt::Key_CapsLock: return ImGuiKey_CapsLock;
	case Qt::Key_ScrollLock: return ImGuiKey_ScrollLock;
	case Qt::Key_NumLock: return ImGuiKey_NumLock;
	case Qt::Key_Print: return ImGuiKey_PrintScreen;
	case Qt::Key_Pause: return ImGuiKey_Pause;
	case Qt::Key_0:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad0;
		else
			return ImGuiKey_0;
	case Qt::Key_1:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad1;
		else
			return ImGuiKey_1;

	case Qt::Key_2:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad2;
		else
			return ImGuiKey_2;
	case Qt::Key_3:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad3;
		else
			return ImGuiKey_3;
	case Qt::Key_4:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad4;
		else
			return ImGuiKey_4;
	case Qt::Key_5:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad5;
		else
			return ImGuiKey_5;
	case Qt::Key_6:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad6;
		else
			return ImGuiKey_6;
	case Qt::Key_7:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad7;
		else
			return ImGuiKey_7;
	case Qt::Key_8:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad8;
		else
			return ImGuiKey_8;
	case Qt::Key_9:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_Keypad9;
		else
			return ImGuiKey_9;
	case Qt::Key_Period:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_KeypadDecimal;
		else
			return ImGuiKey_Period;
	case Qt::Key_Slash:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_KeypadDivide;
		else
			return ImGuiKey_Slash;
	case Qt::Key_Asterisk: return ImGuiKey_KeypadMultiply;

	case Qt::Key_Minus:
		if (event->modifiers() & Qt::KeypadModifier)
			return ImGuiKey_KeypadSubtract;
		else
			return ImGuiKey_Minus;
	case Qt::Key_Plus:
		return ImGuiKey_KeypadAdd;
	case Qt::Key_Enter: return ImGuiKey_KeypadEnter;
	case Qt::Key_Shift: return ImGuiKey_LeftShift;
	case Qt::Key_Control: return ImGuiKey_LeftCtrl;
	case Qt::Key_Alt: return ImGuiKey_LeftAlt;
	case Qt::Key_Super_L: return ImGuiKey_LeftSuper;
	case Qt::Key_Super_R: return ImGuiKey_RightSuper;
	case Qt::Key_Menu: return ImGuiKey_Menu;
	case Qt::Key_A: return ImGuiKey_A;
	case Qt::Key_B: return ImGuiKey_B;
	case Qt::Key_C: return ImGuiKey_C;
	case Qt::Key_D: return ImGuiKey_D;
	case Qt::Key_E: return ImGuiKey_E;
	case Qt::Key_F: return ImGuiKey_F;
	case Qt::Key_G: return ImGuiKey_G;
	case Qt::Key_H: return ImGuiKey_H;
	case Qt::Key_I: return ImGuiKey_I;
	case Qt::Key_J: return ImGuiKey_J;
	case Qt::Key_K: return ImGuiKey_K;
	case Qt::Key_L: return ImGuiKey_L;
	case Qt::Key_M: return ImGuiKey_M;
	case Qt::Key_N: return ImGuiKey_N;
	case Qt::Key_O: return ImGuiKey_O;
	case Qt::Key_P: return ImGuiKey_P;
	case Qt::Key_Q: return ImGuiKey_Q;
	case Qt::Key_R: return ImGuiKey_R;
	case Qt::Key_S: return ImGuiKey_S;
	case Qt::Key_T: return ImGuiKey_T;
	case Qt::Key_U: return ImGuiKey_U;
	case Qt::Key_V: return ImGuiKey_V;
	case Qt::Key_W: return ImGuiKey_W;
	case Qt::Key_X: return ImGuiKey_X;
	case Qt::Key_Y: return ImGuiKey_Y;
	case Qt::Key_Z: return ImGuiKey_Z;
	case Qt::Key_F1: return ImGuiKey_F1;
	case Qt::Key_F2: return ImGuiKey_F2;
	case Qt::Key_F3: return ImGuiKey_F3;
	case Qt::Key_F4: return ImGuiKey_F4;
	case Qt::Key_F5: return ImGuiKey_F5;
	case Qt::Key_F6: return ImGuiKey_F6;
	case Qt::Key_F7: return ImGuiKey_F7;
	case Qt::Key_F8: return ImGuiKey_F8;
	case Qt::Key_F9: return ImGuiKey_F9;
	case Qt::Key_F10: return ImGuiKey_F10;
	case Qt::Key_F11: return ImGuiKey_F11;
	case Qt::Key_F12: return ImGuiKey_F12;
	default: return ImGuiKey_None;
	}
}
void QImguiWidget::keyPressEvent(QKeyEvent* event) {
	if (!event->isAutoRepeat()) {
		const ImGuiKey key = ImGui_ImplQt_QKeyToImGuiKey(event->key(), event);
		QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);


		if (ctx->imgui) {
			ImGui::SetCurrentContext(ctx->imgui);
			auto& io = ImGui::GetIO();
			if (key != ImGuiKey_None)
				io.AddKeyEvent(key, true);
			if (!event->text().isEmpty())
				io.AddInputCharacterUTF16(event->text()[0].unicode());
		}
		else {
			return QOpenGLWidget::keyPressEvent(event);
		}
	}
	else
	{
		QWidget::keyPressEvent(event);
	}
}
void QImguiWidget::keyReleaseEvent(QKeyEvent* event) {
	if (!event->isAutoRepeat()) {
		const ImGuiKey key = ImGui_ImplQt_QKeyToImGuiKey(event->key(), event);
		QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);

		if (ctx->imgui) {
			ImGui::SetCurrentContext(ctx->imgui);
			if (key != ImGuiKey_None)
				ImGui::GetIO().AddKeyEvent(key, false);
		}
		else {
			return QOpenGLWidget::keyReleaseEvent(event);
		}


	}
	else
	{
		QWidget::keyReleaseEvent(event);
	}
}
void QImguiWidget::inputMethodEvent(QInputMethodEvent* event) {
	if (!event->commitString().isEmpty()) {
		QImguiWidgetImplContext* ctx = reinterpret_cast<QImguiWidgetImplContext*>(this->impl);

		if (ctx->imgui) {
			ImGui::SetCurrentContext(ctx->imgui);
			ImGui::GetIO().AddInputCharactersUTF8(event->commitString().toUtf8().constData());
		}
		else {
			return QOpenGLWidget::inputMethodEvent(event);
		}
	}
	QWidget::inputMethodEvent(event);
}
