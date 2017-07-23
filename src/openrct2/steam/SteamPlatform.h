#pragma once 

#include "core/steam_api.h"

#ifdef __cplusplus 

class SteamPlatform
{
private:
    bool _steamAvailable = false;
    bool _gameServer = false;
    void *_steamApiMod = nullptr;

public:
    SteamPlatform();

    bool Initialize();
    void Shutdown();
    void Update();
    bool IsAvailable() const;

    bool InitializeServer(const char *host, uint16 port);

    ISteamUser* User() const;
    ISteamNetworking* Networking() const;

    CSteamID SteamID() const;
};

extern SteamPlatform *gSteamPlatform;

#endif 

extern "C" 
{
    bool steamplatform_init();
    void steamplatform_update();
    void steamplatform_shutdown();
    bool steamplatform_available();
}