#include "staminaonattack.h"
#include "configuration.h"

namespace StaminaOnAttack
{
    static float GetStaminaCost(RE::PlayerCharacter* a_player)
    {
        auto* rightObj = a_player->GetEquippedObject(false);
        auto* leftObj  = a_player->GetEquippedObject(true);

        auto* weap = rightObj ? rightObj->As<RE::TESObjectWEAP>() : nullptr;
        if (!weap)
            weap = leftObj ? leftObj->As<RE::TESObjectWEAP>() : nullptr;

        if (!weap)
            return Configuration::Costs::fHandToHand;

        // OCF keyword check first
        auto* keywordForm = weap->As<RE::BGSKeywordForm>();
        if (keywordForm) {
            for (const auto& [keyword, cost] : Configuration::Keywords::Entries) {
                if (keywordForm->HasKeywordString(keyword)) {
                    SKSE::log::info("StaminaOnAttack: OCF keyword '{}' matched, cost = {:.2f}", keyword, cost);
                    return cost;
                }
            }
        }

        // Vanilla weapon type fallback
        switch (weap->GetWeaponType()) {
        case RE::WEAPON_TYPE::kOneHandSword:    return Configuration::Costs::f1HSword;
        case RE::WEAPON_TYPE::kOneHandAxe:      return Configuration::Costs::f1HAxe;
        case RE::WEAPON_TYPE::kOneHandMace:     return Configuration::Costs::f1HMace;
        case RE::WEAPON_TYPE::kOneHandDagger:   return Configuration::Costs::f1HDagger;
        case RE::WEAPON_TYPE::kTwoHandSword:    return Configuration::Costs::f2HSword;
        case RE::WEAPON_TYPE::kTwoHandAxe:      return Configuration::Costs::f2HAxe;
        case RE::WEAPON_TYPE::kHandToHandMelee: return Configuration::Costs::fHandToHand;
        default:                                return Configuration::Costs::fDefault;
        }
    }

    AnimEventSink* AnimEventSink::GetSingleton()
    {
        static AnimEventSink singleton;
        return &singleton;
    }

    RE::BSEventNotifyControl AnimEventSink::ProcessEvent(
        const RE::BSAnimationGraphEvent* a_event,
        RE::BSTEventSource<RE::BSAnimationGraphEvent>*)
    {
        if (!a_event) return RE::BSEventNotifyControl::kContinue;

        if (a_event->tag != std::string_view(Configuration::Animation::sAttackEvent))
            return RE::BSEventNotifyControl::kContinue;

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player || RE::PlayerCharacter::IsGodMode())
            return RE::BSEventNotifyControl::kContinue;

        const float cost    = GetStaminaCost(player);
        const float stamina = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);

        SKSE::log::info("StaminaOnAttack: stamina = {:.2f}, cost = {:.2f}, failCount = {}",
            stamina, cost, _failedAttackCount);

        if (stamina < cost) {
            if (Configuration::Features::bRegenPenalty) {
                if (_failedAttackCount < Configuration::Penalty::iMaxFailedAttacks)
                    _failedAttackCount++;

                const float currentDelay = player->GetRegenDelay(RE::ActorValue::kStamina);
                const float penalty      = Configuration::Penalty::fRegenPenaltyPerFail * _failedAttackCount;
                player->UpdateRegenDelay(RE::ActorValue::kStamina, currentDelay + penalty);

                SKSE::log::info("StaminaOnAttack: regen penalty applied, failCount = {}, penalty = {:.2f}",
                    _failedAttackCount, penalty);
            }

            player->NotifyAnimationGraph("attackStop");

            if (Configuration::Features::bStaggerPunishment) {
                SKSE::log::info("StaminaOnAttack: stagger triggered.");
                player->NotifyAnimationGraph("StaggerStart");
            }

            return RE::BSEventNotifyControl::kContinue;
        }

        // Successful attack — reset and drain
        _failedAttackCount = 0;

        player->AsActorValueOwner()->RestoreActorValue(
            RE::ACTOR_VALUE_MODIFIER::kDamage,
            RE::ActorValue::kStamina,
            -cost);

        SKSE::log::info("StaminaOnAttack: stamina drained, new value = {:.2f}",
            player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina));

        return RE::BSEventNotifyControl::kContinue;
    }

    void RegisterSink()
    {
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {
            SKSE::log::error("StaminaOnAttack: player is null, sink not registered.");
            return;
        }

        player->RemoveAnimationGraphEventSink(AnimEventSink::GetSingleton());
        player->AddAnimationGraphEventSink(AnimEventSink::GetSingleton());
        SKSE::log::info("StaminaOnAttack: sink registered.");
    }
}