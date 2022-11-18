#include "ImArea.h"
ImArea::ImArea(QWidget* parent) : QImguiWidget(parent) {}
void ImArea::OnImguiInitialized() {

}
void ImArea::QImguiBegin() {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin((u8"###" + this->objectName()).toUtf8().constData(), 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
}

void ImArea::QImguiEnd() {
    ImGui::End();
    ImGui::PopStyleVar(3);
}