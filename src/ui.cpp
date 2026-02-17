#include "UI.h"
#include "settings.h"

void UI::Register()
{
    if (!SKSEMenuFramework::IsInstalled()) {
        SKSE::log::error("UI: SKSEMenuFramework not installed, skipping UI registration.");
        return;
    }

    SKSEMenuFramework::Model::Internal::key = "Configurable Stamina";

    SKSEMenuFramework::AddSectionItem("Features",        UI::Features::Render);
    SKSEMenuFramework::AddSectionItem("Player Costs",   UI::Costs::Render);
    SKSEMenuFramework::AddSectionItem("NPC Costs",      UI::NPCCosts::Render);
    SKSEMenuFramework::AddSectionItem("Animation Event", UI::Animation::Render);

    strncpy_s(UI::Animation::EventBuffer, Configuration::Animation::sAttackEvent.c_str(), sizeof(UI::Animation::EventBuffer) - 1);

    SKSE::log::info("UI: registered.");
}

// ---- Features ----

void __stdcall UI::Features::Render()
{
    ImGuiMCP::Text("Toggle optional punishment behaviours.");
    ImGuiMCP::Separator();

    ImGuiMCPComponents::ToggleButton("Stagger Punishment", &Configuration::Features::bStaggerPunishment);
    ImGuiMCP::SameLine();
    ImGuiMCP::TextDisabled("Stagger the player when attacking with no stamina.");

    ImGuiMCPComponents::ToggleButton("Regen Penalty",      &Configuration::Features::bRegenPenalty);
    ImGuiMCP::SameLine();
    ImGuiMCP::TextDisabled("Delay stamina regen on failed attacks.");

    ImGuiMCP::Separator();
    ImGuiMCP::Text("NPC Settings");
    ImGuiMCP::Separator();

    ImGuiMCPComponents::ToggleButton("Enable NPCs (RESTART NEEDED TO APPLY CHANGES)", &Configuration::Features::bEnableNPCs);
    ImGuiMCP::SameLine();
    ImGuiMCP::TextDisabled("Apply stamina cost to all NPCs.");

    ImGuiMCPComponents::ToggleButton("NPC Separate Scaling", &Configuration::Features::bNPCUseSeparateScaling);
    ImGuiMCP::SameLine();
    ImGuiMCP::TextDisabled("Use separate cost table for NPCs.");

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##Features")) {
        Settings::Save();
    }
}

// ---- Player Costs ----

void __stdcall UI::Costs::Render()
{
    ImGuiMCP::Text("Stamina cost per weapon type.");
    ImGuiMCP::Separator();

    ImGuiMCP::InputFloat("Default",                  &Configuration::NPCCosts::fDefault,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Sword",                    &Configuration::NPCCosts::f1HSword,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("War Axe",                  &Configuration::NPCCosts::f1HAxe,      0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Mace",                     &Configuration::NPCCosts::f1HMace,     0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Dagger",                   &Configuration::NPCCosts::f1HDagger,   0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Greatswords",              &Configuration::NPCCosts::f2HSword,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Battleaxes & Warhammers",  &Configuration::NPCCosts::f2HAxe,      0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Hand to Hand",             &Configuration::NPCCosts::fHandToHand, 0.0f, 0.0f, "%.1f");

    ImGuiMCP::Separator();
    ImGuiMCP::Text("Regen Penalty Settings");
    ImGuiMCP::Separator();

    ImGuiMCP::InputFloat("Penalty Per Fail (Seconds)", &Configuration::Penalty::fRegenPenaltyPerFail, 0.0f, 0.0f, "%.2f");
    ImGuiMCP::InputInt("Max Failed Stacks", &Configuration::Penalty::iMaxFailedAttacks);

    ImGuiMCP::Separator();
    ImGuiMCP::Text("Keyword-based Costs");
    ImGuiMCP::Separator();

    ImGuiMCPComponents::ToggleButton("Sync with NPC", &Configuration::Keywords::bSyncWithNPC);
    ImGuiMCP::SameLine();
    ImGuiMCP::TextDisabled("Apply player keywords to NPCs.");

    ImGuiMCP::Separator();

    int removeIndex = -1;
    for (int i = 0; i < static_cast<int>(Configuration::Keywords::Entries.size()); i++) {
        auto& [keyword, cost] = Configuration::Keywords::Entries[i];

        char keyBuf[128] = {};
        strncpy_s(keyBuf, keyword.c_str(), sizeof(keyBuf) - 1);

        ImGuiMCP::PushID(i);

        ImGuiMCP::SetNextItemWidth(300.0f);
        if (ImGuiMCP::InputText("##kw", keyBuf, sizeof(keyBuf))) {
            keyword = keyBuf;
        }

        ImGuiMCP::SameLine();
        ImGuiMCP::SetNextItemWidth(100.0f);
        ImGuiMCP::InputFloat("##cost", &cost, 0.0f, 0.0f, "%.1f");

        ImGuiMCP::SameLine();
        if (ImGuiMCP::Button("Remove")) {
            removeIndex = i;
        }

        ImGuiMCP::PopID();
    }

    if (removeIndex >= 0) {
        Configuration::Keywords::Entries.erase(
            Configuration::Keywords::Entries.begin() + removeIndex);
    }

    ImGuiMCP::Separator();
    ImGuiMCP::Text("Add new keyword:");

    ImGuiMCP::SetNextItemWidth(300.0f);
    ImGuiMCP::InputText("##newkw", UI::Keywords::NewKeyword, sizeof(UI::Keywords::NewKeyword));
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(100.0f);
    ImGuiMCP::InputFloat("##newcost", &UI::Keywords::NewCost, 0.0f, 0.0f, "%.1f");
    ImGuiMCP::SameLine();

    if (ImGuiMCP::Button("Add")) {
        if (UI::Keywords::NewKeyword[0] != '\0') {
            Configuration::Keywords::Entries.emplace_back(
                std::string(UI::Keywords::NewKeyword),
                UI::Keywords::NewCost);
            UI::Keywords::NewKeyword[0] = '\0';
            UI::Keywords::NewCost = 10.0f;
        }
    }

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##Costs")) {
        Settings::Save();
    }
}

// ---- NPC Costs ----

void __stdcall UI::NPCCosts::Render()
{
    const bool disabled = !Configuration::Features::bNPCUseSeparateScaling;
    
    if (disabled) {
        ImGuiMCP::BeginDisabled();
        ImGuiMCP::TextDisabled("Enable 'NPC Separate Scaling' in Features to edit these values.");
    } else {
        ImGuiMCP::Text("NPC stamina costs (multiplied by fMultiplier).");
    }
    
    ImGuiMCP::Separator();

    ImGuiMCP::InputFloat("Multiplier",               &Configuration::NPCCosts::fMultiplier, 0.0f, 0.0f, "%.2f");
    ImGuiMCP::InputFloat("Default",                  &Configuration::NPCCosts::fDefault,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Sword",                    &Configuration::NPCCosts::f1HSword,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("War Axe",                  &Configuration::NPCCosts::f1HAxe,      0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Mace",                     &Configuration::NPCCosts::f1HMace,     0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Dagger",                   &Configuration::NPCCosts::f1HDagger,   0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Greatswords",              &Configuration::NPCCosts::f2HSword,    0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Battleaxes & Warhammers",  &Configuration::NPCCosts::f2HAxe,      0.0f, 0.0f, "%.1f");
    ImGuiMCP::InputFloat("Hand to Hand",             &Configuration::NPCCosts::fHandToHand, 0.0f, 0.0f, "%.1f");

    ImGuiMCP::Separator();
    ImGuiMCP::Text("Keyword Costs");
    ImGuiMCP::Separator();

    const bool synced = Configuration::Keywords::bSyncWithNPC;
    
    if (synced) {
        ImGuiMCP::BeginDisabled();
        ImGuiMCP::TextDisabled("Using synced player keywords (read-only).");
        ImGuiMCP::Separator();
        
        for (int i = 0; i < static_cast<int>(Configuration::Keywords::Entries.size()); i++) {
            const auto& [keyword, cost] = Configuration::Keywords::Entries[i];
            ImGuiMCP::Text("%s: %.1f", keyword.c_str(), cost);
        }
        
        ImGuiMCP::EndDisabled();
    } else {
        ImGuiMCP::Text("NPC-specific keywords (editable).");
        ImGuiMCP::Separator();
        
        int removeIndex = -1;
        for (int i = 0; i < static_cast<int>(Configuration::Keywords::NPCEntries.size()); i++) {
            auto& [keyword, cost] = Configuration::Keywords::NPCEntries[i];

            char keyBuf[128] = {};
            strncpy_s(keyBuf, keyword.c_str(), sizeof(keyBuf) - 1);

            ImGuiMCP::PushID(i + 1000);

            ImGuiMCP::SetNextItemWidth(300.0f);
            if (ImGuiMCP::InputText("##npckw", keyBuf, sizeof(keyBuf))) {
                keyword = keyBuf;
            }

            ImGuiMCP::SameLine();
            ImGuiMCP::SetNextItemWidth(100.0f);
            ImGuiMCP::InputFloat("##npccost", &cost, 0.0f, 0.0f, "%.1f");

            ImGuiMCP::SameLine();
            if (ImGuiMCP::Button("Remove")) {
                removeIndex = i;
            }

            ImGuiMCP::PopID();
        }

        if (removeIndex >= 0) {
            Configuration::Keywords::NPCEntries.erase(
                Configuration::Keywords::NPCEntries.begin() + removeIndex);
        }

        ImGuiMCP::Separator();
        ImGuiMCP::Text("Add new keyword:");

        ImGuiMCP::SetNextItemWidth(300.0f);
        ImGuiMCP::InputText("##newnpckw", UI::Keywords::NewNPCKeyword, sizeof(UI::Keywords::NewNPCKeyword));
        ImGuiMCP::SameLine();
        ImGuiMCP::SetNextItemWidth(100.0f);
        ImGuiMCP::InputFloat("##newnpccost", &UI::Keywords::NewNPCCost, 0.0f, 0.0f, "%.1f");
        ImGuiMCP::SameLine();

        if (ImGuiMCP::Button("Add##NPC")) {
            if (UI::Keywords::NewNPCKeyword[0] != '\0') {
                Configuration::Keywords::NPCEntries.emplace_back(
                    std::string(UI::Keywords::NewNPCKeyword),
                    UI::Keywords::NewNPCCost);
                UI::Keywords::NewNPCKeyword[0] = '\0';
                UI::Keywords::NewNPCCost = 10.0f;
            }
        }
    }

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##NPCCosts")) {
        Settings::Save();
    }
    
    if (disabled) {
        ImGuiMCP::EndDisabled();
    }
}

// ---- Animation ----

void __stdcall UI::Animation::Render()
{
    ImGuiMCP::Text("Animation event that triggers stamina cost.");
    ImGuiMCP::TextDisabled("Default: MCO_AttackInitiate");
    ImGuiMCP::Separator();

    ImGuiMCP::SetNextItemWidth(300.0f);
    if (ImGuiMCP::InputText("Attack Event", UI::Animation::EventBuffer, sizeof(UI::Animation::EventBuffer))) {
        Configuration::Animation::sAttackEvent = UI::Animation::EventBuffer;
    }

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##Animation")) {
        Settings::Save();
    }
}