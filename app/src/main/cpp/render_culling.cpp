#include "render_culling.h"

#include <atomic>

namespace {

std::atomic<bool> enabled{true};

}

namespace render_culling {

void Install() {
    // No external hooks.
    // Render Culling is kept as a local/UI setting.
}

void SetEnabled(bool value) {
    enabled.store(value, std::memory_order_relaxed);
}

bool IsEnabled() {
    return enabled.load(std::memory_order_relaxed);
}

} // namespace render_culling
