/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <stdint.h>
#include <vector>

#include "NetworkConnection.h"
#include "NetworkPackets.h"

struct NetworkHostInfo
{
    std::string name;
    std::string description;
    std::string greeting;
    std::string providerName;
    std::string providerEmail;
    std::string providerWebsite;
};

enum class NetworkState : uint8_t
{
    NONE = 0,
    READY,
    CONNECTING,
    CONNECTED,
};

enum class NetworkMode : uint8_t
{
    NONE = 0,
    CLIENT,
    HOST,
};

class NetworkPacketDispatcher
{
public:
    NetworkPacketDispatcher() = default;

    NetworkPacketDispatcher(NetworkPacketDispatcher&& mv)
    {
        _handlers = std::move(mv._handlers);
    }

    template<typename T, typename F> void Register(F fn)
    {
        Handler handler;
        handler.type = T::Type();
        handler.fn = [fn](NetworkConnection& c, MemoryStream& data) -> void {
            DataSerialiser ds(false, data);

            T packet;
            packet.Serialise(ds);

            fn(c, packet);
        };
        _handlers.emplace_back(std::move(handler));
    }

    void Dispatch(NetworkConnection& connection, MemoryStream& data)
    {
        DataSerialiser ds(false, data);

        NetworkPacketHead head;
        head.SerialiseHead(ds);

        for (auto& handler : _handlers)
        {
            if (handler.type == head.type)
            {
                handler.fn(connection, data);
            }
        }
    }

private:
    struct Handler
    {
        int32_t type;
        std::function<void(NetworkConnection&, MemoryStream&)> fn;
    };
    std::vector<Handler> _handlers;
};

class NetworkBase
{
public:
    NetworkBase();
    virtual ~NetworkBase() = default;

    virtual bool Startup() = 0;
    virtual bool Shutdown() = 0;

    virtual void Close() = 0;
    virtual void Update() = 0;
    virtual void Flush() = 0;

    NetworkState GetState() const
    {
        return _state;
    }

    bool IsReady() const
    {
        return _state == NetworkState::READY;
    }

    bool IsConnected() const
    {
        return _state == NetworkState::CONNECTED;
    }

    bool IsConnecting() const
    {
        return _state == NetworkState::CONNECTING;
    }

    const NetworkHostInfo& GetHostInfo() const
    {
        return _hostInfo;
    }

    const char* GetGameVersion() const
    {
        return "OPENRCT2-NETWORK-NEW";
    }

protected:
    NetworkState _state = NetworkState::NONE;
    NetworkHostInfo _hostInfo;
    NetworkPacketDispatcher _dispatcher;
};
