
#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <deque>

using namespace geode::prelude;

// Input Buffer basico para Geometry Dash 2.2081
// Retrasa las pulsaciones y liberaciones por ticks de fisica.

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

    static void clearQueue() {
        inputQueue.clear();
    }

}

class $modify(InputBufferGameLayer, GJBaseGameLayer) {

    void handleButton(bool down, int button, bool isPlayer1) {

        // Evitar capturar de nuevo los inputs que el mod envia.
        if (InputBuffer::injectingInput) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        // Solo capturar los clics del jugador 1 durante gameplay.
        auto* playLayer = GameManager::get()->m_playLayer;

        if (playLayer != this || button != 1 || !isPlayer1) {
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
            return;
        }

        // Guardar el input para ejecutarlo despues.
        InputBuffer::inputQueue.push_back({
            down,
            button,
            isPlayer1,
            InputBuffer::BUFFER_TICKS
        });

        // No enviar el input inmediatamente.
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {

        // Ejecutar la logica original del juego.
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);

        auto* playLayer = GameManager::get()->m_playLayer;

        if (playLayer != this) {
            return;
        }

        // Reducir el contador de cada input pendiente.
        for (auto& input : InputBuffer::inputQueue) {
            if (input.ticksLeft > 0) {
                input.ticksLeft--;
            }
        }

        // Ejecutar los inputs que ya han cumplido el retraso.
        while (
            !InputBuffer::inputQueue.empty() &&
            InputBuffer::inputQueue.front().ticksLeft <= 0
        ) {

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
