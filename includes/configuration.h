#pragma once
#include <string>
#include <vector>
#include <utility>
#include <chrono>

namespace Configuration
{
    namespace Features
    {
        inline bool bStaggerPunishment     = false;
        inline bool bRegenPenalty          = true;
        inline bool bEnableNPCs            = true;
        inline bool bNPCUseSeparateScaling = false;
    }

    namespace Costs
    {
        inline float fDefault    = 10.0f;
        inline float f1HSword    = 12.0f;
        inline float f1HAxe      = 13.0f;
        inline float f1HMace     = 14.0f;
        inline float f1HDagger   = 8.0f;
        inline float f2HSword    = 20.0f;
        inline float f2HAxe      = 22.0f;
        inline float fHandToHand = 6.0f;
    }

    namespace NPCCosts
    {
        inline float fMultiplier = 1.0f;
        inline float fDefault    = 10.0f;
        inline float f1HSword    = 12.0f;
        inline float f1HAxe      = 13.0f;
        inline float f1HMace     = 14.0f;
        inline float f1HDagger   = 8.0f;
        inline float f2HSword    = 20.0f;
        inline float f2HAxe      = 22.0f;
        inline float fHandToHand = 6.0f;
    }

    namespace Penalty
    {
        inline float fRegenPenaltyPerFail = 0.2f;
        inline int   iMaxFailedAttacks    = 10;
    }

    namespace Keywords
    {
        inline bool bSyncWithNPC = true;

        inline std::vector<std::pair<std::string, float>> Entries = {
            { "OCF_WeapTypeKatana1H",   10.0f },
            { "OCF_WeapTypeKatana2H",   20.0f },
            { "OCF_WeapTypeSpear1H",    16.0f },
            { "OCF_WeapTypeSpear2H",    23.0f },
            { "OCF_WeapTypeHalberd2H",  24.0f },
            { "OCF_WeapTypeQuarterStaff2H", 18.0f },
        };

        inline std::vector<std::pair<std::string, float>> NPCEntries = {
            { "OCF_WeapTypeKatana1H",   10.0f },
            { "OCF_WeapTypeKatana2H",   20.0f },
            { "OCF_WeapTypeSpear1H",    16.0f },
            { "OCF_WeapTypeSpear2H",    23.0f },
            { "OCF_WeapTypeHalberd2H",  24.0f },
            { "OCF_WeapTypeQuarterStaff2H", 18.0f },
        };
    }

    namespace Animation
    {
        inline std::vector<std::string> AttackEvents = {
            "MCO_AttackInitiate"
        };
        inline float fAttackCooldown = 0.5f;

        inline bool bEventTrackerEnabled = false;
        inline std::vector<std::pair<std::string, std::chrono::steady_clock::time_point>> RecentEvents;
        static constexpr size_t kMaxTrackedEvents = 20;
        static constexpr float kEventTrackerTimeout = 10.0f;
    }
}