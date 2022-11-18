#include "ImPushButton.h"
ImPushButton::ImPushButton(QWidget* parent) : QImguiWidget(parent) {}
void ImPushButton::OnImguiInitialized() {

}

void ImPushButton::QImguiBegin() {
    if (/*this->parent()->inherits("QImguiWidget")*/0) {
        return;
    }
    else
    {
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin((u8"###" + this->objectName()).toUtf8().constData(), 0, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNavFocus);
    }
}
void ImPushButton::QImguiContent() { 
    ImGui::Button("TEST BUTTON");
}
void ImPushButton::QImguiEnd() {
    if (/*this->parent()->inherits("QImguiWidget")*/0) {
        return;
    }
    else
    {
        ImGui::End();
        ImGui::PopStyleVar(3);
    }
}
