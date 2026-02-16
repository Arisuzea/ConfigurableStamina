#pragma once

#include <SKSE/SKSE.h>
#include <RE/Skyrim.h>

namespace StaminaOnAttack
{
    void RegisterSink();

    class AnimEventSink : public RE::BSTEventSink<RE::BSAnimationGraphEvent>
    {
    public:
        static AnimEventSink* GetSingleton();

        RE::BSEventNotifyControl ProcessEvent(
            const RE::BSAnimationGraphEvent* a_event,
            RE::BSTEventSource<RE::BSAnimationGraphEvent>*) override;

    private:
        AnimEventSink() = default;
        int _failedAttackCount = 0;
    };
}