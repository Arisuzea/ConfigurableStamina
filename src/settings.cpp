#include "pch.h"
#include "settings.h"
#include "configuration.h"
#include "SimpleIni.h"

namespace Settings
{
    static constexpr auto INI_PATH = "Data/SKSE/Plugins/ConfigurableStamina.ini";

    void Load()
    {
        CSimpleIniA ini;
        ini.SetUnicode();

        if (ini.LoadFile(INI_PATH) < 0) {
            SKSE::log::info("Settings: no INI found, writing defaults.");
            Save();
            return;
        }

        // Features
        Configuration::Features::bStaggerPunishment = ini.GetBoolValue("Features", "bStaggerPunishment", Configuration::Features::bStaggerPunishment);
        Configuration::Features::bRegenPenalty      = ini.GetBoolValue("Features", "bRegenPenalty",      Configuration::Features::bRegenPenalty);

        // Costs
        Configuration::Costs::fDefault    = static_cast<float>(ini.GetDoubleValue("Costs", "fDefault",    Configuration::Costs::fDefault));
        Configuration::Costs::f1HSword    = static_cast<float>(ini.GetDoubleValue("Costs", "f1HSword",    Configuration::Costs::f1HSword));
        Configuration::Costs::f1HAxe      = static_cast<float>(ini.GetDoubleValue("Costs", "f1HAxe",      Configuration::Costs::f1HAxe));
        Configuration::Costs::f1HMace     = static_cast<float>(ini.GetDoubleValue("Costs", "f1HMace",     Configuration::Costs::f1HMace));
        Configuration::Costs::f1HDagger   = static_cast<float>(ini.GetDoubleValue("Costs", "f1HDagger",   Configuration::Costs::f1HDagger));
        Configuration::Costs::f2HSword    = static_cast<float>(ini.GetDoubleValue("Costs", "f2HSword",    Configuration::Costs::f2HSword));
        Configuration::Costs::f2HAxe      = static_cast<float>(ini.GetDoubleValue("Costs", "f2HAxe",      Configuration::Costs::f2HAxe));
        Configuration::Costs::fHandToHand = static_cast<float>(ini.GetDoubleValue("Costs", "fHandToHand", Configuration::Costs::fHandToHand));

        // Penalty
        Configuration::Penalty::fRegenPenaltyPerFail = static_cast<float>(ini.GetDoubleValue("Penalty", "fRegenPenaltyPerFail", Configuration::Penalty::fRegenPenaltyPerFail));
        Configuration::Penalty::iMaxFailedAttacks    = static_cast<int>(ini.GetLongValue("Penalty", "iMaxFailedAttacks", Configuration::Penalty::iMaxFailedAttacks));

        // Animation
        Configuration::Animation::sAttackEvent = ini.GetValue("Animation", "sAttackEvent", Configuration::Animation::sAttackEvent.c_str());

        // Keywords — serialized as Keyword0, Cost0, Keyword1, Cost1 ...
        Configuration::Keywords::Entries.clear();
        int i = 0;
        while (true) {
            auto keyName  = std::format("Keyword{}", i);
            auto costName = std::format("Cost{}", i);

            const char* keyword = ini.GetValue("Keywords", keyName.c_str(), nullptr);
            if (!keyword) break;

            float cost = static_cast<float>(ini.GetDoubleValue("Keywords", costName.c_str(), 10.0));
            Configuration::Keywords::Entries.emplace_back(keyword, cost);
            i++;
        }

        SKSE::log::info("Settings: loaded from '{}'.", INI_PATH);
    }

    void Save()
    {
        CSimpleIniA ini;
        ini.SetUnicode();

        // Features
        ini.SetBoolValue("Features", "bStaggerPunishment", Configuration::Features::bStaggerPunishment, nullptr);
        ini.SetBoolValue("Features", "bRegenPenalty",      Configuration::Features::bRegenPenalty,      nullptr);

        // Costs
        ini.SetDoubleValue("Costs", "fDefault",    Configuration::Costs::fDefault,    nullptr);
        ini.SetDoubleValue("Costs", "f1HSword",    Configuration::Costs::f1HSword,    nullptr);
        ini.SetDoubleValue("Costs", "f1HAxe",      Configuration::Costs::f1HAxe,      nullptr);
        ini.SetDoubleValue("Costs", "f1HMace",     Configuration::Costs::f1HMace,     nullptr);
        ini.SetDoubleValue("Costs", "f1HDagger",   Configuration::Costs::f1HDagger,   nullptr);
        ini.SetDoubleValue("Costs", "f2HSword",    Configuration::Costs::f2HSword,    nullptr);
        ini.SetDoubleValue("Costs", "f2HAxe",      Configuration::Costs::f2HAxe,      nullptr);
        ini.SetDoubleValue("Costs", "fHandToHand", Configuration::Costs::fHandToHand, nullptr);

        // Penalty
        ini.SetDoubleValue("Penalty", "fRegenPenaltyPerFail", Configuration::Penalty::fRegenPenaltyPerFail, nullptr);
        ini.SetLongValue(  "Penalty", "iMaxFailedAttacks",    Configuration::Penalty::iMaxFailedAttacks,    nullptr);

        // Animation
        ini.SetValue("Animation", "sAttackEvent", Configuration::Animation::sAttackEvent.c_str(), nullptr);

        // Keywords — clear old entries first
        ini.Delete("Keywords", nullptr);
        for (int i = 0; i < static_cast<int>(Configuration::Keywords::Entries.size()); i++) {
            const auto& [keyword, cost] = Configuration::Keywords::Entries[i];
            ini.SetValue(      "Keywords", std::format("Keyword{}", i).c_str(), keyword.c_str(),          nullptr);
            ini.SetDoubleValue("Keywords", std::format("Cost{}", i).c_str(),    static_cast<double>(cost), nullptr);
        }

        if (ini.SaveFile(INI_PATH) < 0) {
            SKSE::log::error("Settings: failed to save to '{}'.", INI_PATH);
        } else {
            SKSE::log::info("Settings: saved to '{}'.", INI_PATH);
        }
    }
}