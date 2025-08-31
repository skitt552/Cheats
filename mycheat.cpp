#include <windows.h>
#include <d3d9.h>

// ======= Minimal ImGui STUBS =======
// (Replace with real ImGui later)
struct ImVec2 { float x, y; ImVec2(float _x=0, float _y=0):x(_x),y(_y){} };
struct ImVec4 { float x,y,z,w; ImVec4(float _x=0,float _y=0,float _z=0,float _w=0):x(_x),y(_y),z(_z),w(_w){} };
struct ImDrawList {
    void AddRect(ImVec2, ImVec2, int) {}
    void AddLine(ImVec2, ImVec2, int, float=1.0f) {}
    void AddCircle(ImVec2, float, int, int=64, float=1.0f) {}
    void AddCircleFilled(ImVec2, float, int) {}
};
struct ImGuiIO { ImVec2 DisplaySize; };
namespace ImGui {
    inline ImGuiIO& GetIO() { static ImGuiIO io; return io; }
    inline ImDrawList* GetBackgroundDrawList() { static ImDrawList d; return &d; }
    inline void Begin(const char*) {}
    inline void End() {}
    inline void Checkbox(const char*, bool*) {}
    inline void SliderFloat(const char*, float*, float, float) {}
    inline void ColorEdit4(const char*, float*) {}
}
// ===================================

// ==========================
// Configurable Settings
// ==========================
struct Config {
    bool aimbot = true;
    float aimFov = 120.0f;
    float aimSmooth = 5.0f;

    bool espBox = true;
    bool espSkeleton = true;
    ImVec4 espColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);

    bool radar = true;
    bool triggerbot = false;
} config;

bool menuOpen = true;

// ==========================
// Render Placeholders
// ==========================
void RenderESP(ImDrawList* drawList) {
    if (config.espBox)
        drawList->AddRect(ImVec2(200,200), ImVec2(260,320), 0xFFFFFFFF);

    if (config.espSkeleton)
        drawList->AddLine(ImVec2(230,200), ImVec2(230,320), 0xFFFFFFFF, 2.0f);
}

void RenderRadar(ImDrawList* drawList) {
    if (!config.radar) return;

    ImVec2 radarPos = ImVec2(100, 100);
    float radarSize = 120;
    drawList->AddRect(radarPos, ImVec2(radarPos.x + radarSize, radarPos.y + radarSize), 0xFFFFFFFF);
    drawList->AddCircleFilled(ImVec2(radarPos.x + radarSize/2, radarPos.y + radarSize/2), 3, 0xFFFF0000);
}

void RenderAimbotFov(ImDrawList* drawList, ImVec2 screenSize) {
    if (!config.aimbot) return;
    ImVec2 center = ImVec2(screenSize.x/2, screenSize.y/2);
    drawList->AddCircle(center, config.aimFov, 0xFF00FF00, 64, 2.0f);
}

// ==========================
// Cheat Menu
// ==========================
void RenderMenu() {
    if (!menuOpen) return;

    ImGui::Begin("Cheat Menu");

    ImGui::Checkbox("Aimbot", &config.aimbot);
    if (config.aimbot) {
        ImGui::SliderFloat("FOV", &config.aimFov, 10.0f, 360.0f);
        ImGui::SliderFloat("Smooth", &config.aimSmooth, 1.0f, 20.0f);
    }

    ImGui::Checkbox("Box ESP", &config.espBox);
    ImGui::Checkbox("Skeleton ESP", &config.espSkeleton);
    ImGui::ColorEdit4("ESP Color", (float*)&config.espColor);

    ImGui::Checkbox("2D Radar", &config.radar);
    ImGui::Checkbox("Triggerbot", &config.triggerbot);

    ImGui::End();
}

// ==========================
// Main Hack Thread
// ==========================
DWORD WINAPI HackThread(HMODULE hModule) {
    while (true) {
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            menuOpen = !menuOpen;
        }

        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2(800, 600); // fake screen size

        ImDrawList* drawList = ImGui::GetBackgroundDrawList();

        RenderESP(drawList);
        RenderRadar(drawList);
        RenderAimbotFov(drawList, io.DisplaySize);

        if (menuOpen) RenderMenu();

        Sleep(16);
    }

    FreeLibraryAndExitThread(hModule, 0);
    return 0;
}

// ==========================
// DLL Entry Point
// ==========================
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
    if (dwReason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr);
    }
    return TRUE;
}