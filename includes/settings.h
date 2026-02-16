#pragma once

#include <utility>
#include <string_view>

namespace Settings
{
    // Stamina costs per weapon type
    inline constexpr float kCost_Default    = 15.0f;
    inline constexpr float kCost_1HSword    = 12.0f;
    inline constexpr float kCost_1HAxe      = 13.0f;
    inline constexpr float kCost_1HMace     = 14.0f;
    inline constexpr float kCost_1HDagger   = 8.0f;
    inline constexpr float kCost_2HSword    = 20.0f;
    inline constexpr float kCost_2HAxe      = 22.0f;
    inline constexpr float kCost_HandToHand = 6.0f;

    // Punishment
    inline constexpr float kRegenPenaltyPerFail = 0.2f;
    inline constexpr int   kMaxFailedAttacks     = 10;

    // Feature toggles
    inline constexpr bool kEnableStaminaCost   = true;
    inline constexpr bool kEnableStaggerPunish = true;
    inline constexpr bool kEnableRegenPenalty  = true;

    // OCF keyword costs — add entries here for modded weapons
    inline constexpr std::pair<std::string_view, float> kOCFCosts[] = {
        { "OCF_WeapTypeKatana1H",   10.0f },
        { "OCF_WeapTypeSpear1H",    16.0f },
        { "OCF_WeapTypeHalberd2H",  24.0f },
        { "OCF_WeapTypeQtrStaff2H", 18.0f },
        { "OCF_WeapTypeWhip1H",     10.0f },
    };
}