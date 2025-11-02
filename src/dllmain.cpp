#include "dllmain.hpp"
#include "Create.hpp"

ModFunction void Initialize(AmethystContext& ctx, const Amethyst::Mod& mod) 
{
    // Initialize Amethyst mod backend
    Amethyst::InitializeAmethystMod(ctx, mod);
    Create::Initialize();
}