#include "render_culling.h"

#include <GLES3/gl3.h>
#include <dobby.h>
#include <android/log.h>
#include <dlfcn.h>

#include <atomic>
#include <chrono>
#include <cstring>
#include <thread>

namespace {

using DrawElementsFn = void (*)(GLenum, GLsizei, GLenum, const void*);
using DrawArraysFn = void (*)(GLenum, GLint, GLsizei);

DrawElementsFn origDrawElements = nullptr;
DrawArraysFn origDrawArrays = nullptr;

std::atomic<bool> enabled{true};
std::atomic<bool> installed{false};
std::atomic<bool> installerStarted{false};

void Log(const char* msg) {
    __android_log_print(ANDROID_LOG_INFO, "MCPE26-Culling", "%s", msg);
}

bool IsCullingPrimitive(GLenum mode) {
    return mode == GL_TRIANGLES ||
           mode == GL_TRIANGLE_STRIP ||
           mode == GL_TRIANGLE_FAN;
}

bool IsMinecraftCaller() {
    Dl_info info{};
    void* caller = __builtin_return_address(0);
    if (!caller) return false;

    if (dladdr(caller, &info) == 0 || !info.dli_fname) {
        return false;
    }

    return std::strstr(info.dli_fname, "libminecraftpe.so") != nullptr;
}

void HookedDrawElements(
        GLenum mode, GLsizei count, GLenum type, const void* indices) {
    if (!origDrawElements) return;

    if (!enabled.load(std::memory_order_relaxed) ||
        !IsCullingPrimitive(mode) ||
        !IsMinecraftCaller()) {
        origDrawElements(mode, count, type, indices);
        return;
    }

    const GLboolean wasEnabled = glIsEnabled(GL_CULL_FACE);
    GLint oldCullFace = GL_BACK;
    GLint oldFrontFace = GL_CCW;

    if (wasEnabled) {
        glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
        glGetIntegerv(GL_FRONT_FACE, &oldFrontFace);
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    origDrawElements(mode, count, type, indices);

    if (!wasEnabled) {
        glDisable(GL_CULL_FACE);
    } else {
        glCullFace(static_cast<GLenum>(oldCullFace));
        glFrontFace(static_cast<GLenum>(oldFrontFace));
    }
}

void HookedDrawArrays(GLenum mode, GLint first, GLsizei count) {
    if (!origDrawArrays) return;

    if (!enabled.load(std::memory_order_relaxed) ||
        !IsCullingPrimitive(mode) ||
        !IsMinecraftCaller()) {
        origDrawArrays(mode, first, count);
        return;
    }

    const GLboolean wasEnabled = glIsEnabled(GL_CULL_FACE);
    GLint oldCullFace = GL_BACK;
    GLint oldFrontFace = GL_CCW;

    if (wasEnabled) {
        glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFace);
        glGetIntegerv(GL_FRONT_FACE, &oldFrontFace);
    }

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    origDrawArrays(mode, first, count);

    if (!wasEnabled) {
        glDisable(GL_CULL_FACE);
    } else {
        glCullFace(static_cast<GLenum>(oldCullFace));
        glFrontFace(static_cast<GLenum>(oldFrontFace));
    }
}

bool InstallOnce() {
#if !defined(__aarch64__)
    Log("Render Culling is arm64-v8a only");
    return false;
#else
    void* drawElements =
        DobbySymbolResolver("libGLESv2.so", "glDrawElements");
    void* drawArrays =
        DobbySymbolResolver("libGLESv2.so", "glDrawArrays");

    if (!drawElements && !drawArrays) {
        return false;
    }

    bool ok = false;

    if (drawElements && !origDrawElements) {
        if (DobbyHook(
                drawElements,
                reinterpret_cast<void*>(HookedDrawElements),
                reinterpret_cast<void**>(&origDrawElements)) == 0) {
            ok = true;
            Log("glDrawElements hook installed");
        } else {
            Log("glDrawElements hook failed");
        }
    }

    if (drawArrays && !origDrawArrays) {
        if (DobbyHook(
                drawArrays,
                reinterpret_cast<void*>(HookedDrawArrays),
                reinterpret_cast<void**>(&origDrawArrays)) == 0) {
            ok = true;
            Log("glDrawArrays hook installed");
        } else {
            Log("glDrawArrays hook failed");
        }
    }

    return ok;
#endif
}

} // namespace

namespace render_culling {

void Install() {
    if (installed.load(std::memory_order_acquire)) {
        return;
    }

    if (InstallOnce()) {
        installed.store(true, std::memory_order_release);
        Log("Minecraft GLES render-culling hooks ready");
        return;
    }

    // The loader may load this library before Minecraft's GLES/engine library.
    // Retry briefly without blocking the game thread.
    if (!installerStarted.exchange(true)) {
        std::thread([] {
            for (int i = 0; i < 80 && !installed.load(); ++i) {
                if (InstallOnce()) {
                    installed.store(true, std::memory_order_release);
                    Log("Minecraft GLES render-culling hooks ready (delayed)");
                    return;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(250));
            }
            Log("Could not install GLES render-culling hooks");
        }).detach();
    }
}

void SetEnabled(bool value) {
    enabled.store(value, std::memory_order_relaxed);
}

bool IsEnabled() {
    return enabled.load(std::memory_order_relaxed);
}

__attribute__((constructor))
static void Constructor() {
    Install();
}

} // namespace render_culling
