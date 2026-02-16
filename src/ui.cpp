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
    SKSEMenuFramework::AddSectionItem("Weapon Costs",    UI::Costs::Render);
    SKSEMenuFramework::AddSectionItem("Keyword Costs",   UI::Keywords::Render);
    SKSEMenuFramework::AddSectionItem("Animation Event", UI::Animation::Render);

    // Pre-populate the animation event buffer from current config
    strncpy_s(UI::Animation::EventBuffer, Configuration::Animation::sAttackEvent.c_str(), sizeof(UI::Animation::EventBuffer) - 1);

    SKSE::log::info("UI: registered.");
}

// ---- Features ---------------------------------------------------------------

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
    if (ImGuiMCP::Button("Save##Features")) {
        Settings::Save();
    }
}

// ---- Costs ------------------------------------------------------------------

void __stdcall UI::Costs::Render()
{
    ImGuiMCP::Text("Stamina cost per weapon type.");
    ImGuiMCP::Separator();

    ImGuiMCP::SliderFloat("Default",        &Configuration::Costs::fDefault,    0.0f, 50.0f);
    ImGuiMCP::SliderFloat("1H Sword",       &Configuration::Costs::f1HSword,    0.0f, 50.0f);
    ImGuiMCP::SliderFloat("1H Axe",         &Configuration::Costs::f1HAxe,      0.0f, 50.0f);
    ImGuiMCP::SliderFloat("1H Mace",        &Configuration::Costs::f1HMace,     0.0f, 50.0f);
    ImGuiMCP::SliderFloat("Dagger",         &Configuration::Costs::f1HDagger,   0.0f, 50.0f);
    ImGuiMCP::SliderFloat("2H Sword",       &Configuration::Costs::f2HSword,    0.0f, 50.0f);
    ImGuiMCP::SliderFloat("2H Axe",         &Configuration::Costs::f2HAxe,      0.0f, 50.0f);
    ImGuiMCP::SliderFloat("Hand to Hand",   &Configuration::Costs::fHandToHand, 0.0f, 50.0f);

    ImGuiMCP::Separator();
    ImGuiMCP::Text("Regen Penalty Settings");
    ImGuiMCP::Separator();

    ImGuiMCP::SliderFloat("Penalty Per Fail",    &Configuration::Penalty::fRegenPenaltyPerFail, 0.0f, 10.0f);
    ImGuiMCP::SliderInt(  "Max Failed Stacks",   &Configuration::Penalty::iMaxFailedAttacks,    1,    20);

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##Costs")) {
        Settings::Save();
    }
}

// ---- Keywords ---------------------------------------------------------------

void __stdcall UI::Keywords::Render()
{
    ImGuiMCP::Text("Keyword-based stamina costs. Checked before weapon type.");
    ImGuiMCP::Separator();

    // Existing entries
    int removeIndex = -1;
    for (int i = 0; i < static_cast<int>(Configuration::Keywords::Entries.size()); i++) {
        auto& [keyword, cost] = Configuration::Keywords::Entries[i];

        // Keyword text input
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
    ImGuiMCP::Text("Add new entry:");

    ImGuiMCP::SetNextItemWidth(300.0f);
    ImGuiMCP::InputText("##newkw",   UI::Keywords::NewKeyword, sizeof(UI::Keywords::NewKeyword));
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
            UI::Keywords::NewCost       = 10.0f;
        }
    }

    ImGuiMCP::Separator();
    if (ImGuiMCP::Button("Save##Keywords")) {
        Settings::Save();
    }
}

// ---- Animation --------------------------------------------------------------

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