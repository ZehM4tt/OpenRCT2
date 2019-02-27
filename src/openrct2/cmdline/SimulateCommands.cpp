/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Context.h"
#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/Console.hpp"
#include "../network/network.h"
#include "../platform/platform.h"
#include "../world/Sprite.h"
#include "CommandLine.hpp"

#include <cstdlib>
#include <memory>

using namespace OpenRCT2;

static exitcode_t HandleSimulate(CommandLineArgEnumerator* argEnumerator);

const CommandLineCommand CommandLine::SimulateCommands[]{ // Main commands
                                                          DefineCommand("", "<ticks>", nullptr, HandleSimulate), CommandTableEnd
};

static exitcode_t HandleSimulate(CommandLineArgEnumerator* argEnumerator)
{
    const char** argv = (const char**)argEnumerator->GetArguments() + argEnumerator->GetIndex();
    int32_t argc = argEnumerator->GetCount() - argEnumerator->GetIndex();

    if (argc < 2)
    {
        Console::Error::WriteLine("Missing arguments <sv6-file> <ticks>.");
        return EXITCODE_FAIL;
    }

    core_init();

    const char* inputPath = argv[0];
    uint32_t ticks = atol(argv[1]);

    gOpenRCT2Headless = true;

#ifndef DISABLE_NETWORK
    gNetworkStart = NETWORK_MODE::HOST;
#endif

    std::unique_ptr<IContext> context(CreateContext());
    if (context->Initialise())
    {
        if (!context->LoadParkFromFile(inputPath))
        {
            return EXITCODE_FAIL;
        }

        Console::WriteLine("Running %d ticks...", ticks);
        for (uint32_t i = 0; i < ticks; i++)
        {
            context->GetGameState()->UpdateLogic();
        }
        Console::WriteLine("Completed: %s", sprite_checksum());
    }
    else
    {
        Console::Error::WriteLine("Context initialization failed.");
        return EXITCODE_FAIL;
    }

    return EXITCODE_OK;
}
