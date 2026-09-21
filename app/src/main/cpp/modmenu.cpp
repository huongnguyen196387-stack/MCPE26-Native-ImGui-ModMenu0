#include "modmenu.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "render_culling.h"
#include <GLES3/gl3.h>
#include <android/log.h>
#include <algorithm>

namespace {
bool initialized = false;
bool visible = true;
bool fps = true;
bool compact = false;
bool demo = false;
bool featureA = false;
bool featureB = false;

// Performance panel state.
bool performancePanel = true;
bool lowGraphics = false;
bool showStats = true;
bool frameLimiter = false;
bool renderCulling = true;
int targetFps = 60;

int width = 1;
int height = 1;

void Log(const char* s) {
    __android_log_print(ANDROID_LOG_INFO, "MCPE26-ImGui", "%s", s);
}
}

namespace modmenu {

void Init() {
    if (initialized) return;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    ImGui::StyleColorsDark();

    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 12.0f;
    s.ChildRounding = 8.0f;
    s.FrameRounding = 7.0f;
    s.PopupRounding = 8.0f;
    s.ScrollbarRounding = 7.0f;
    s.GrabRounding = 7.0f;

    ImGui_ImplOpenGL3_Init("#version 300 es");
    initialized = true;
    Log("ImGui initialized");
}

void Resize(int w, int h) {
    width = std::max(1, w);
    height = std::max(1, h);
    glViewport(0, 0, width, height);
    if (initialized) ImGui::GetIO().DisplaySize = ImVec2((float)width, (float)height);
}

void Touch(int action, float x, float y) {
    if (!initialized) return;
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent(x, y);
    if (action == 0) io.AddMouseButtonEvent(0, true);
    if (action == 1) io.AddMouseButtonEvent(0, false);
}

void Render() {
    if (!initialized) return;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    if (visible) {
        ImGui::SetNextWindowSize(
            compact ? ImVec2(380, 0) : ImVec2(470, 0),
            ImGuiCond_FirstUseEver
        );

        if (ImGui::Begin("MCPE 26 | Native ImGui Menu",
                         &visible, ImGuiWindowFlags_NoCollapse)) {
            ImGui::TextUnformatted("Clean build / UI test");
            ImGui::Separator();

            ImGui::Checkbox("Feature A", &featureA);
            ImGui::Checkbox("Feature B", &featureB);
            ImGui::Checkbox("FPS counter", &fps);
            ImGui::Checkbox("Compact layout", &compact);
            ImGui::Checkbox("ImGui demo", &demo);

            ImGui::Spacing();
            if (ImGui::CollapsingHeader("Performance",
                                        ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Checkbox("Low graphics mode", &lowGraphics);
                if (ImGui::Checkbox("Render Culling", &renderCulling)) {
                    render_culling::SetEnabled(renderCulling);
                }
                ImGui::Checkbox("Show performance stats", &showStats);
                ImGui::Checkbox("Frame limiter", &frameLimiter);

                const char* fpsItems[] = {"30 FPS", "45 FPS", "60 FPS", "90 FPS", "120 FPS"};
                int fpsIndex = 2;
                if (targetFps == 30) fpsIndex = 0;
                else if (targetFps == 45) fpsIndex = 1;
                else if (targetFps == 90) fpsIndex = 3;
                else if (targetFps == 120) fpsIndex = 4;

                if (ImGui::Combo("Target FPS", &fpsIndex,
                                 fpsItems, IM_ARRAYSIZE(fpsItems))) {
                    const int values[] = {30, 45, 60, 90, 120};
                    targetFps = values[fpsIndex];
                }

                ImGui::Separator();
                ImGui::Text("Current FPS: %.1f", ImGui::GetIO().Framerate);
                ImGui::Text("Target: %d FPS", targetFps);
                ImGui::Text("Renderer: OpenGL ES 3");
                ImGui::Text("ABI: arm64-v8a");

                if (lowGraphics) {
                    ImGui::TextUnformatted("Low graphics: ON");
                } else {
                    ImGui::TextUnformatted("Low graphics: OFF");
                }

                if (renderCulling) {
                    ImGui::TextUnformatted(
                        "Render Culling: UI setting enabled");
                } else {
                    ImGui::TextUnformatted(
                        "Render Culling: UI setting disabled");
                }

                if (frameLimiter) {
                    ImGui::TextUnformatted(
                        "Frame limiter: UI setting enabled");
                } else {
                    ImGui::TextUnformatted(
                        "Frame limiter: UI setting disabled");
                }
            }

            ImGui::Spacing();
            ImGui::Text("Feature A: %s", featureA ? "ON" : "OFF");
            ImGui::Text("Feature B: %s", featureB ? "ON" : "OFF");
            ImGui::Text("Display: %dx%d", width, height);

            if (ImGui::Button("Hide menu", ImVec2(-1, 0))) visible = false;
        }
        ImGui::End();
    } else {
        ImGui::SetNextWindowPos(ImVec2(18, 18), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.72f);
        if (ImGui::Begin("##open", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings)) {
            if (ImGui::Button("OPEN MENU")) visible = true;
        }
        ImGui::End();
    }

    if (fps && showStats) {
        ImGui::SetNextWindowBgAlpha(0.45f);
        if (ImGui::Begin("##fps", nullptr,
            ImGuiWindowFlags_NoDecoration |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav)) {
            ImGui::Text("FPS %.1f", ImGui::GetIO().Framerate);
        }
        ImGui::End();
    }

    if (demo) ImGui::ShowDemoWindow(&demo);

    ImGui::Render();
    glClearColor(0.035f, 0.045f, 0.060f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

}
