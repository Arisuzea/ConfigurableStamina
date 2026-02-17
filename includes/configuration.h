#pragma once
#include <string>
#include <vector>
#include <utility>

namespace Configuration
{
    namespace Features
    {
        inline bool bStaggerPunishment = true;
        inline bool bRegenPenalty      = true;
        
        inline bool bEnableNPCs           = true;
        inline bool bNPCUseSeparateScaling = false;
    }

    namespace Costs
    {
        inline float fDefault    = 15.0f;
        inline float f1HSword    = 12.0f;
        inline float f1HAxe      = 13.0f;
        inline float f1HMace     = 14.0f;
        inline float f1HDagger   = 8.0f;
        inline float f2HSword    = 20.0f;
        inline float f2HAxe      = 22.0f;
        inline float f2HMace     = 21.0f;
        inline float fHandToHand = 6.0f;
    }

    namespace NPCCosts
    {
        inline float fMultiplier = 1.0f;
        
        inline float fDefault    = 15.0f;
        inline float f1HSword    = 12.0f;
        inline float f1HAxe      = 13.0f;
        inline float f1HMace     = 14.0f;
        inline float f1HDagger   = 8.0f;
        inline float f2HSword    = 20.0f;
        inline float f2HAxe      = 22.0f;
        inline float f2HMace     = 21.0f;
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
            { "OCF_WeapTypeQtrStaff2H", 18.0f },
        };
        
        inline std::vector<std::pair<std::string, float>> NPCEntries = {
            { "OCF_WeapTypeKatana1H",   10.0f },
            { "OCF_WeapTypeKatana2H",   20.0f },
            { "OCF_WeapTypeSpear1H",    16.0f },
            { "OCF_WeapTypeSpear2H",    23.0f },
            { "OCF_WeapTypeHalberd2H",  24.0f },
            { "OCF_WeapTypeQtrStaff2H", 18.0f },
        };
    }

    namespace Animation
    {
        inline std::string sAttackEvent = "MCO_AttackInitiate";
    }
}