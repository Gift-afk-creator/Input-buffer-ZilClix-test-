#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <deque>

using namespace geode::prelude;

// Experimental input buffer for Geometry Dash 2.2081.
// Delays button press/release events by a fixed number of game ticks.
// This does NOT predict landings or automatically rescue missed inputs.

namespace InputBuffer {
    constexpr int BUFFER_TICKS = 4;

    struct BufferedInput {
        bool down;
        int button;
        bool isPlayer1;
        int ticksLeft;
    };

    static std::deque<BufferedInput> inputQueue;
    static bool injectingInput = false;
}

class $modify(InputBufferGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        if (InputBuffer::injectingInput) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        auto* playLayer = GameManager::get()->m_playLayer;

        // Compare pointer addresses through void* because the pointer types differ.
        if (reinterpret_cast<void*>(playLayer) != reinterpret_cast<void*>(this)
            || button != 1 || !isPlayer1) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        InputBuffer::inputQueue.push_back({
            down,
            button,
            isPlayer1,
            InputBuffer::BUFFER_TICKS
        });
        // The captured event is intentionally not sent immediately.
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);

        auto* playLayer = GameManager::get()->m_playLayer;
        if (reinterpret_cast<void*>(playLayer) != reinterpret_cast<void*>(this)) {
            return;
        }

        for (auto& input : InputBuffer::inputQueue) {
            if (input.ticksLeft > 0) {
                --input.ticksLeft;
            }
        }

        while (!InputBuffer::inputQueue.empty()
            && InputBuffer::inputQueue.front().ticksLeft <= 0) {
            auto input = InputBuffer::inputQueue.front();
            InputBuffer::inputQueue.pop_front();

            InputBuffer::injectingInput = true;
            GJBaseGameLayer::handleButton(
                input.down,
                input.button,
                input.isPlayer1
            );
            InputBuffer::injectingInput = false;
        }
    }
};

