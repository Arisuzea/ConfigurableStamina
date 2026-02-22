# Configurable Stamina

A lightweight SKSE plugin that adds stamina costs to normal attacks. Configure everything in-game through a clean MCM-style menu using SKSE Menu Framework.

Tuned by default for **MCO (Modern Combat Overhaul)**, but compatible with any combat system, including vanilla.

## What it does

Every weapon swing deducts stamina. Run out and you face consequences. You decide the costs, the penalties, and whether NPCs follow the same rules.

## Features

- **Weapon-specific costs**  
  Tune stamina drain per weapon type: daggers are light, greatswords are heavy. For modded weapons, assign costs via keywords. Works great with [Object Categorization Framework (OCF)](https://www.nexusmods.com/skyrimspecialedition/mods/68065), which covers most modded weapons out of the box.

- **Stagger on failure** (toggleable)  
  Try to attack without enough stamina and you get staggered instead. The swing cancels and you take a brief vulnerability window.

- **Regen delay stacking** (toggleable)  
  Spamming attacks on empty stamina builds up a regeneration delay. Encourages pacing and proper spacing.

- **NPC support**  
  Optionally apply the system to NPCs. Choose whether they use the same costs as the player or separate values for different tuning.

- **Animation event based**  
  Hooks into a configurable animation event to detect attacks. Defaults to `MCO_AttackInitiate` for MCO. Use `weaponSwing` for vanilla. Adjust to match your combat framework.

## Requirements

- [SKSE64](https://skse.silverlock.org/)
- [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)

## Configuration

Adjust settings live in-game via the SKSE Menu Framework menu. Changes save automatically to the INI file.

You can also edit manually:

```
Data/SKSE/Plugins/ConfigurableStamina.ini
```

## Compatibility

- Built around MCO but works with any combat mod — just update the animation event name
- Plays nice with mods that don't hook stamina regen directly
- No ESP file, no load order impact

## Build

Clone the repository inside your current folder
```
git clone https://github.com/Arisuzea/ConfigurableStamina .
```

This plugin is built with [CLibDT](https://www.nexusmods.com/skyrimspecialedition/mods/154240).

But if you do not want to use it

SE + AE
```
xmake f -y -m release --toolchain=msvc --skyrim_se=y --skyrim_ae=y
xmake build
```

SE Only
```
xmake f -y -m release --toolchain=msvc --skyrim_se=y
xmake build
```

AE Only
```
xmake f -y -m release --toolchain=msvc --skyrim_ae=y
xmake build
```

VR Only
```
xmake f -y -m release --toolchain=msvc --skyrim_vr=y
xmake build
```
> **Note:** VR vtable offsets are unverified. The VR build exists but is untested.

## Credits

Made with [CLibDT](https://www.nexusmods.com/skyrimspecialedition/mods/154240) and [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352).

- [Normal Attacks Cost Stamina](https://www.nexusmods.com/skyrimspecialedition/mods/32996)
  For the mod idea
- [Paragon Perks](https://www.nexusmods.com/skyrimspecialedition/mods/120398)
  For the breakthrough I needed, I implemented its logic to distinct a player from NPC, saving me a day's worth of headache
