#pragma once
#include "API/SKSEMenuFramework.h"
#include "configuration.h"

namespace UI
{
    void Register();

    namespace Features
    {
        void __stdcall Render();
    }

    namespace Costs
    {
        void __stdcall Render();
    }

    namespace Keywords
    {
        inline char  NewKeyword[128] = {};
        inline float NewCost         = 10.0f;
        void __stdcall Render();
    }

    namespace Animation
    {
        inline char EventBuffer[128] = {};
        void __stdcall Render();
    }
}