#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <deque>
#include <cstdint>

using namespace geode::prelude;

/*
 * ZilClix Input Buffer — experimental prototype.
 *
 * This prototype performs a FIXED frame delay. It does not inspect future
 * collisions and cannot know which frame is "safe" without a real physics
 * predictor. It deliberately avoids pretending that a delayed click is an
 * automatic save.
 *
 * Important: PlayerObject pointers are only used while the current PlayLayer
 * is alive. The queue is cleared when the level ends/restarts.
 */

namespace {
    struct QueuedInput {
        PlayerObject* player = nullptr;
        PlayerButton button = PlayerButton::Jump;
        bool isPush = true;
        int framesLeft = 0;
    };

    std::deque<QueuedInput> s_queue;
    bool s_forwarding = false;
    bool s_wasInLevel = false;

    bool enabled() {
        return Mod::get()->getSettingValue<bool>("enabled");
    }

    int delayFrames() {
        auto value = Mod::get()->getSettingValue<int64_t>("delay-frames");
        if (value < 1) return 1;
        if (value > 16) return 16;
        return static_cast<int>(value);
    }

    void enqueue(PlayerObject* player, PlayerButton button, bool isPush) {
        s_queue.push_back(QueuedInput{
            player,
            button,
            isPush,
            delayFrames()
        });
    }

    void clearQueue() {
        s_queue.clear();
    }

    void dispatch(QueuedInput const& input) {
        if (!input.player) return;
        s_forwarding = true;
        if (input.isPush) {
            input.player->pushButton(input.button);
        } else {
            input.player->releaseButton(input.button);
        }
        s_forwarding = false;
    }

    void tickQueue() {
        for (auto it = s_queue.begin(); it != s_queue.end();) {
            --it->framesLeft;
            if (it->framesLeft <= 0) {
                auto event = *it;
                it = s_queue.erase(it);
                dispatch(event);
            } else {
                ++it;
            }
        }
    }
}

class $modify(ZilClixPlayerObject, PlayerObject) {
    bool pushButton(PlayerButton button) {
        if (s_forwarding || !enabled() || !PlayLayer::get()) {
            return PlayerObject::pushButton(button);
        }

        enqueue(this, button, true);
        return true;
    }

    bool releaseButton(PlayerButton button) {
        if (s_forwarding || !enabled() || !PlayLayer::get()) {
            return PlayerObject::releaseButton(button);
        }

        enqueue(this, button, false);
        return true;
    }
};

class $modify(ZilClixPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (!enabled()) {
            clearQueue();
            s_wasInLevel = false;
            return;
        }

        s_wasInLevel = true;
        tickQueue();
    }

    void resetLevel() {
        clearQueue();
        PlayLayer::resetLevel();
    }

    void onQuit() {
        clearQueue();
        PlayLayer::onQuit();
    }
};
