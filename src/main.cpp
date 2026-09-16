#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <algorithm>

using namespace geode::prelude;

class $modify(CompletionDuckPlayLayer, PlayLayer) {
    struct Fields {
        float originalMusicVolume = 1.0f;
        float quietMusicVolume = 0.3f;
        float phaseTime = 0.0f;
        int fadePhase = 0; // 0: idle, 1: fading down, 2: holding, 3: fading up
    };

    void setMusicVolume(float volume) {
        auto* engine = FMODAudioEngine::sharedEngine();
        if (engine && engine->m_backgroundMusicChannel) {
            engine->m_backgroundMusicChannel->setVolume(volume);
        }
    }

    static float smoothStep(float value) {
        value = std::clamp(value, 0.0f, 1.0f);
        return value * value * (3.0f - 2.0f * value);
    }

    void restoreMusicVolumeImmediately() {
        if (m_fields->fadePhase == 0) {
            return;
        }

        this->unschedule(schedule_selector(CompletionDuckPlayLayer::updateDuck));
        this->setMusicVolume(m_fields->originalMusicVolume);
        m_fields->fadePhase = 0;
        m_fields->phaseTime = 0.0f;
    }

    void beginMusicDuck() {
        auto* engine = FMODAudioEngine::sharedEngine();
        if (!engine || !engine->m_backgroundMusicChannel) {
            return;
        }

        auto* music = engine->m_backgroundMusicChannel;
        float currentVolume = 1.0f;
        if (music->getVolume(&currentVolume) != FMOD_OK) {
            return;
        }

        m_fields->originalMusicVolume = currentVolume;
        auto percent = Mod::get()->getSettingValue<int64_t>("music-percent");
        m_fields->quietMusicVolume = currentVolume *
            (static_cast<float>(percent) / 100.0f);
        m_fields->phaseTime = 0.0f;
        m_fields->fadePhase = 1;

        this->unschedule(schedule_selector(CompletionDuckPlayLayer::updateDuck));
        this->schedule(schedule_selector(CompletionDuckPlayLayer::updateDuck));
    }

    void updateDuck(float delta) {
        m_fields->phaseTime += delta;

        if (m_fields->fadePhase == 1) {
            auto duration = static_cast<float>(
                Mod::get()->getSettingValue<double>("fade-down-seconds")
            );
            auto progress = smoothStep(m_fields->phaseTime / duration);
            auto volume = m_fields->originalMusicVolume +
                (m_fields->quietMusicVolume - m_fields->originalMusicVolume) * progress;
            this->setMusicVolume(volume);

            if (m_fields->phaseTime >= duration) {
                this->setMusicVolume(m_fields->quietMusicVolume);
                m_fields->fadePhase = 2;
                m_fields->phaseTime = 0.0f;
            }
            return;
        }

        if (m_fields->fadePhase == 2) {
            this->setMusicVolume(m_fields->quietMusicVolume);
            auto duration = static_cast<float>(
                Mod::get()->getSettingValue<double>("hold-seconds")
            );
            if (m_fields->phaseTime >= duration) {
                m_fields->fadePhase = 3;
                m_fields->phaseTime = 0.0f;
            }
            return;
        }

        if (m_fields->fadePhase == 3) {
            auto duration = static_cast<float>(
                Mod::get()->getSettingValue<double>("fade-up-seconds")
            );
            auto progress = smoothStep(m_fields->phaseTime / duration);
            auto volume = m_fields->quietMusicVolume +
                (m_fields->originalMusicVolume - m_fields->quietMusicVolume) * progress;
            this->setMusicVolume(volume);

            if (m_fields->phaseTime >= duration) {
                this->setMusicVolume(m_fields->originalMusicVolume);
                m_fields->fadePhase = 0;
                m_fields->phaseTime = 0.0f;
                this->unschedule(
                    schedule_selector(CompletionDuckPlayLayer::updateDuck)
                );
            }
        }
    }

    void levelComplete() {
        // Let Geometry Dash start endStart.ogg normally, then lower only the
        // background-music group with a smooth curve. This keeps the jingle at
        // its normal volume while avoiding an abrupt volume jump.
        PlayLayer::levelComplete();
        this->beginMusicDuck();
    }

    void resetLevel() {
        this->restoreMusicVolumeImmediately();
        PlayLayer::resetLevel();
    }

    void onExit() {
        this->restoreMusicVolumeImmediately();
        PlayLayer::onExit();
    }
};
