#pragma once

#include "NetworkBase.h"

#ifndef DISABLE_NETWORK

namespace OpenRCT2
{
    class NetworkServer final : public NetworkBase
    {
    public:
        static constexpr int32_t NetworkMode = NETWORK_MODE_SERVER;

    public:
        NetworkServer(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);

        void BeginServerLog();
        void AppendServerLog(const std::string& s);
        void CloseServerLog();
        bool BeginServer(uint16_t port, const std::string& address);

        void Close() override;
        void Update() override;
        void PostUpdate() override;
        void Flush() override;
        int32_t GetMode() const override;
        NetworkStats_t GetStats() const override;
        NetworkAuth GetAuthStatus() override;
        NetworkConnection* GetPlayerConnection(NetworkPlayerId_t id);

    public:
        void KickPlayer(int32_t playerId);
        void SendGameAction(const GameAction* action);
        void SendTick();

        // Packet dispatchers.
        void SendAuthStatus(NetworkConnection& connection);

        void SendToken(NetworkConnection& connection);
        void SendMap(NetworkConnection* connection = nullptr);
        void SendChat(const char* text, const std::vector<uint8_t>& playerIds = {});
        void SendPlayerInfo(int32_t playerId);
        void SendPlayerList();
        void SendPing();
        void SendPingList();
        void SendDisconnectMsg(NetworkConnection& connection, const char* msg);
        void Server_Send_GAMEINFO(NetworkConnection& connection);
        void Server_Send_SHOWERROR(NetworkConnection& connection, rct_string_id title, rct_string_id message);
        void Server_Send_GROUPLIST(NetworkConnection& connection);
        void Server_Send_EVENT_PLAYER_JOINED(const char* playerName);
        void SendEventPlayerDisconnected(const char* playerName, const char* reason);
        void SendObjectsList(NetworkConnection& connection, const std::vector<const ObjectRepositoryItem*>& objects) const;
        void SendScripts(NetworkConnection& connection) const;

    private:
        void AddClient(std::unique_ptr<ITcpSocket>&& socket);
        void RemovePlayer(std::unique_ptr<NetworkConnection>& connection);

        void SendPacketToClients(const NetworkPacket& packet, bool front, bool gameCmd);
        void DecayCooldown(NetworkPlayer* player);
        void ProcessPlayerList();
        void ProcessDisconnectedClients();

    private:
        void OnClientAuthenticated(const char* name, const std::string& keyhash, NetworkConnection& connection);
        void OnClientDisconnected(std::unique_ptr<NetworkConnection>& connection);

        void OnPacketAuthRequest(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketRequestGameState(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketHeartbeat(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketChat(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGameAction(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketPing(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketGameInfo(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketToken(NetworkConnection& connection, NetworkPacket& packet);
        void OnPacketMapRequest(NetworkConnection& connection, NetworkPacket& packet);

    private:
        void OnClientAuthenticated(NetworkConnection& connection, const char* name);

    private:
        uint32_t _lastPingTime = 0;
        std::unique_ptr<ITcpSocket> _listenSocket;
        std::unique_ptr<INetworkServerAdvertiser> _advertiser;
        std::list<std::unique_ptr<NetworkConnection>> _clients;
        std::string _serverLogPath;
        std::string _serverLogFilenameFormat = "%Y%m%d-%H%M%S.txt";
        std::ofstream _server_log_fs;
        uint16_t listening_port = 0;
    };

} // namespace OpenRCT2

#endif // DISABLE_NETWORK
