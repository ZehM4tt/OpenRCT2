/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../network/network.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(PlayerSetGroupAction, GAME_COMMAND_SET_PLAYER_GROUP, GameActionResult)
{
private:
    NetworkPlayerId_t _playerId{ -1 };
    NetworkGroupId_t _groupId{ std::numeric_limits<NetworkGroupId_t>::max() };

public:
    PlayerSetGroupAction()
    {
    }

    PlayerSetGroupAction(NetworkPlayerId_t playerId, NetworkGroupId_t groupId)
        : _playerId(playerId)
        , _groupId(groupId)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_playerId) << DS_TAG(_groupId);
    }
    GameActionResult::Ptr Query() const override
    {
        return network_set_player_group(GetPlayerId(), _playerId, _groupId, false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return network_set_player_group(GetPlayerId(), _playerId, _groupId, true);
    }

private:
    NetworkPlayerId_t GetPlayerId() const
    {
        NetworkPlayerId_t res = GetPlayer();
        // If no round trip happened yet the player id is unassigned, we result the local player id instead.
        if (res == -1 && !(GetFlags() & GAME_COMMAND_FLAG_NETWORKED))
        {
            res = network_get_current_player_id();
        }
        return res;
    }
};
