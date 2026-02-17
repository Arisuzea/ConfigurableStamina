#include "staminaonattack.h"
#include "configuration.h"

namespace StaminaOnAttack
{
    // ---- Cost calculation ----
    static int _failedAttackCount = 0;
    static float GetPlayerStaminaCost(RE::PlayerCharacter* a_player)
    {
        auto* rightObj = a_player->GetEquippedObject(false);
        auto* leftObj  = a_player->GetEquippedObject(true);

        auto* weap = rightObj ? rightObj->As<RE::TESObjectWEAP>() : nullptr;
        if (!weap)
            weap = leftObj ? leftObj->As<RE::TESObjectWEAP>() : nullptr;

        if (!weap)
            return Configuration::Costs::fHandToHand;

        auto* keywordForm = weap->As<RE::BGSKeywordForm>();
        if (keywordForm) {
            for (const auto& [keyword, cost] : Configuration::Keywords::Entries) {
                if (keywordForm->HasKeywordString(keyword)) {
                    SKSE::log::info("StaminaOnAttack: keyword '{}' matched, cost = {:.2f}", keyword, cost);
                    return cost;
                }
            }
        }

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

    static float GetNPCStaminaCost(RE::Actor* a_actor)
    {
        auto* rightObj = a_actor->GetEquippedObject(false);
        auto* leftObj  = a_actor->GetEquippedObject(true);

        auto* weap = rightObj ? rightObj->As<RE::TESObjectWEAP>() : nullptr;
        if (!weap)
            weap = leftObj ? leftObj->As<RE::TESObjectWEAP>() : nullptr;

        const bool useSeparate = Configuration::Features::bNPCUseSeparateScaling;

        if (!weap)
            return useSeparate ? Configuration::NPCCosts::fHandToHand : Configuration::Costs::fHandToHand;

        auto* keywordForm = weap->As<RE::BGSKeywordForm>();
        if (keywordForm) {
            const auto& keywordList = Configuration::Keywords::bSyncWithNPC
                ? Configuration::Keywords::Entries
                : Configuration::Keywords::NPCEntries;

            for (const auto& [keyword, cost] : keywordList) {
                if (keywordForm->HasKeywordString(keyword))
                    return cost;
            }
        }

        if (useSeparate) {
            float baseCost = Configuration::NPCCosts::fDefault;
            switch (weap->GetWeaponType()) {
            case RE::WEAPON_TYPE::kOneHandSword:    baseCost = Configuration::NPCCosts::f1HSword; break;
            case RE::WEAPON_TYPE::kOneHandAxe:      baseCost = Configuration::NPCCosts::f1HAxe; break;
            case RE::WEAPON_TYPE::kOneHandMace:     baseCost = Configuration::NPCCosts::f1HMace; break;
            case RE::WEAPON_TYPE::kOneHandDagger:   baseCost = Configuration::NPCCosts::f1HDagger; break;
            case RE::WEAPON_TYPE::kTwoHandSword:    baseCost = Configuration::NPCCosts::f2HSword; break;
            case RE::WEAPON_TYPE::kTwoHandAxe:      baseCost = Configuration::NPCCosts::f2HAxe; break;
            case RE::WEAPON_TYPE::kHandToHandMelee: baseCost = Configuration::NPCCosts::fHandToHand; break;
            default: break;
            }
            return baseCost * Configuration::NPCCosts::fMultiplier;
        } else {
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
    }

    // ---- Shared event handler ----

    static void HandleAnimEvent(const RE::BSAnimationGraphEvent* a_event)
    {
        if (!a_event || !a_event->holder)
            return;

        if (a_event->tag != std::string_view(Configuration::Animation::sAttackEvent))
            return;

        auto* actor = const_cast<RE::TESObjectREFR*>(a_event->holder)->As<RE::Actor>();
        if (!actor)
            return;

        // ---- Player ----
        if (actor->IsPlayerRef()) {
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player || RE::PlayerCharacter::IsGodMode())
                return;

            const float cost    = GetPlayerStaminaCost(player);
            const float stamina = player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);

            SKSE::log::info("StaminaOnAttack: stamina = {:.2f}, cost = {:.2f}, failCount = {}",
                stamina, cost, _failedAttackCount);

            if (stamina < cost) {
                if (stamina > 0.0f) {
                    player->AsActorValueOwner()->RestoreActorValue(
                        RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -stamina);
                    SKSE::log::info("StaminaOnAttack: player below cost, drained remaining {:.2f}", stamina);
                }

                if (Configuration::Features::bRegenPenalty) {
                    if (_failedAttackCount < Configuration::Penalty::iMaxFailedAttacks)
                        _failedAttackCount++;

                    const float currentDelay = player->GetRegenDelay(RE::ActorValue::kStamina);
                    const float penalty      = Configuration::Penalty::fRegenPenaltyPerFail * _failedAttackCount;
                    player->UpdateRegenDelay(RE::ActorValue::kStamina, currentDelay + penalty);

                    SKSE::log::info("StaminaOnAttack: regen penalty applied, failCount = {}, penalty = {:.2f}",
                        _failedAttackCount, penalty);
                }

                if (Configuration::Features::bStaggerPunishment) {
                    player->NotifyAnimationGraph("attackStop");
                    player->NotifyAnimationGraph("StaggerStart");
                    SKSE::log::info("StaminaOnAttack: player staggered.");
                }

                return;
            }

            _failedAttackCount = 0;
            player->AsActorValueOwner()->RestoreActorValue(
                RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -cost);

            SKSE::log::info("StaminaOnAttack: player stamina drained, new = {:.2f}",
                player->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina));

            return;
        }

        // ---- NPC ----
        if (!Configuration::Features::bEnableNPCs)
            return;

        const float cost    = GetNPCStaminaCost(actor);
        const float stamina = actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);

        SKSE::log::info("StaminaOnAttack: NPC stamina = {:.2f}, cost = {:.2f}", stamina, cost);

        if (stamina >= cost) {
            actor->AsActorValueOwner()->RestoreActorValue(
                RE::ACTOR_VALUE_MODIFIER::kDamage, RE::ActorValue::kStamina, -cost);

            SKSE::log::info("StaminaOnAttack: NPC stamina drained, new = {:.2f}",
                actor->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina));
        }
    }


    // ---- Hooks ----
    struct CharacterHook
    {
        static RE::BSEventNotifyControl ProcessEvent(
            RE::BSTEventSink<RE::BSAnimationGraphEvent>*   a_sink,
            const RE::BSAnimationGraphEvent*               a_event,
            RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_source)
        {
            HandleAnimEvent(a_event);
            return _ProcessEvent(a_sink, a_event, a_source);
        }

        static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;

        static void Install()
        {
            REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_Character[2] };
            _ProcessEvent = vtbl.write_vfunc(0x1, ProcessEvent);
            SKSE::log::info("StaminaOnAttack: Character hook installed.");
        }
    };

    struct PlayerCharacterHook
    {
        static RE::BSEventNotifyControl ProcessEvent(
            RE::BSTEventSink<RE::BSAnimationGraphEvent>*   a_sink,
            const RE::BSAnimationGraphEvent*               a_event,
            RE::BSTEventSource<RE::BSAnimationGraphEvent>* a_source)
        {
            HandleAnimEvent(a_event);
            return _ProcessEvent(a_sink, a_event, a_source);
        }

        static inline REL::Relocation<decltype(ProcessEvent)> _ProcessEvent;

        static void Install()
        {
            REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_PlayerCharacter[2] };
            _ProcessEvent = vtbl.write_vfunc(0x1, ProcessEvent);
            SKSE::log::info("StaminaOnAttack: PlayerCharacter hook installed.");
        }
    };

    void Install()
    {
        CharacterHook::Install();
        PlayerCharacterHook::Install();
    }
}