#include "example.h"
#include <QApplication>
example::example() {
    ImGui::StyleColorsLight();
}
void example::OnImguiFrameShow() {
    ImGui::Begin("HELLO IMGUI", 0, 0);
    ImGui::End();
    ImGui::ShowDemoWindow();
}
void example::preGlWidgetChange() {
    //release image texture
}
void example::onGlWidgetChanged() {
    //create image texture

}

int main(int argc, char* argv[])
{
#if QT_VERSION >= 0x050000
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);

    static const unsigned short ranges[] = {
      0x0020, 0x00FF, // Basic Latin + Latin Supplement
      0x2000, 0x206F, // General Punctuation
      0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
      0x31F0, 0x31FF, // Katakana Phonetic Extensions
      //0xFF00, 0xFFEF, // Half-width characters
      //0xFFFD, 0xFFFD, // Invalid
      0x4e00, 0x9FAF, // CJK Ideograms
      0,
    };

    QImGuiInterface::GetFontAtlas()->AddFontFromFileTTF(u8"C:\\Windows\\Fonts\\msyh.ttc", 18.0f, 0, ranges);
    QImguiWidget w(&a);
    w.resize(950, 650);
    w.show();
    w.setFrame(example::Instance());
    return a.exec();
}
