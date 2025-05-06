#include "SetXpBar.h"
#include "Config.h"

std::vector<std::string> SetXpBarServerScript::GetChunks(std::string s, uint8_t chunkSize)
{
    std::vector<std::string> chunks;
    for (uint32_t i = 0; i < s.size(); i += chunkSize)
        chunks.push_back(s.substr(i, chunkSize));
    return chunks;
}

void SetXpBarServerScript::SendChunkedPayload(Warden* warden, WardenPayloadMgr* payloadMgr, std::string payload, uint32 chunkSize)
{
    auto chunks = GetChunks(payload, chunkSize);
    std::string debugPayload;

    if (!payloadMgr->GetPayloadById(_prePayloadId))
        payloadMgr->RegisterPayload(_prePayload, _prePayloadId);

    payloadMgr->QueuePayload(_prePayloadId);
    warden->ForceChecks();

    for (auto const& chunk : chunks)
    {
        std::string escapedChunk = chunk;
        size_t pos = 0;
        while ((pos = escapedChunk.find("]]", pos)) != std::string::npos)
            escapedChunk.replace(pos, 2, "] ]");

        auto smallPayload = "wlbuf = wlbuf .. [[" + escapedChunk + "]];";
        debugPayload += escapedChunk;

        payloadMgr->RegisterPayload(smallPayload, _tmpPayloadId, true);
        payloadMgr->QueuePayload(_tmpPayloadId);
        warden->ForceChecks();
    }

    if (!payloadMgr->GetPayloadById(_postPayloadId))
        payloadMgr->RegisterPayload(_postPayload, _postPayloadId);

    payloadMgr->QueuePayload(_postPayloadId);
    warden->ForceChecks();
}

bool SetXpBarServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (!sConfigMgr->GetOption<bool>("SetXpBar.Enable", false))
    {
        return true;
    }

    WardenWin* warden = (WardenWin*)session->GetWarden();
    if (!warden || !warden->IsInitialized())
    {
        return true;
    }

    auto payloadMgr = warden->GetPayloadMgr();
    if (!payloadMgr)
    {
        return true;
    }

    if (packet.GetOpcode() == SMSG_LOGIN_VERIFY_WORLD)
    {
        payloadMgr->ClearQueuedPayloads();
        payloadMgr->UnregisterPayload(_prePayloadId);
        payloadMgr->UnregisterPayload(_postPayloadId);
        payloadMgr->UnregisterPayload(_tmpPayloadId);

        SendChunkedPayload(warden, payloadMgr, _midPayload, 128);
    }

    return true;
}

void AddSetXpBarScripts()
{
    new SetXpBarServerScript();
}
