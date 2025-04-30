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
    bool verbose = sConfigMgr->GetOption<bool>("SetXpBar.Verbose", false);

    auto chunks = GetChunks(payload, chunkSize);
    std::string debugPayload;

    if (!payloadMgr->GetPayloadById(_prePayloadId))
        payloadMgr->RegisterPayload(_prePayload, _prePayloadId);

    payloadMgr->QueuePayload(_prePayloadId);
    warden->ForceChecks();

    if (verbose)
        LOG_INFO("module", "SetXpBar: Sent pre-payload '%s'.", _prePayload.c_str());

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

        if (verbose)
            LOG_INFO("module", "SetXpBar: Sent mid-payload '%s'.", smallPayload.c_str());
    }

    if (!payloadMgr->GetPayloadById(_postPayloadId))
        payloadMgr->RegisterPayload(_postPayload, _postPayloadId);

    payloadMgr->QueuePayload(_postPayloadId);
    warden->ForceChecks();

    if (verbose)
    {
        LOG_INFO("module", "SetXpBar: Sent post-payload '%s'.", _postPayload.c_str());
        LOG_INFO("module", "SetXpBar: Reassembled payload: '%s'.", debugPayload.c_str());
    }
}

bool SetXpBarServerScript::CanPacketSend(WorldSession* session, WorldPacket& packet)
{
    if (!sConfigMgr->GetOption<bool>("SetXpBar.Enable", false))
    {
        LOG_INFO("module", "SetXpBar: Module disabled in configuration (opcode %u)", packet.GetOpcode());
        return true;
    }

    WardenWin* warden = (WardenWin*)session->GetWarden();
    if (!warden || !warden->IsInitialized())
    {
        LOG_INFO("module", "SetXpBar: Warden not initialized (opcode %u, initialized: %s)", packet.GetOpcode(), warden ? (warden->IsInitialized() ? "true" : "false") : "null");
        return true;
    }

    auto payloadMgr = warden->GetPayloadMgr();
    if (!payloadMgr)
    {
        LOG_INFO("module", "SetXpBar: PayloadMgr not available (opcode %u)", packet.GetOpcode());
        return true;
    }

    if (packet.GetOpcode() == SMSG_LOGIN_VERIFY_WORLD)
    {
        payloadMgr->ClearQueuedPayloads();
        payloadMgr->UnregisterPayload(_prePayloadId);
        payloadMgr->UnregisterPayload(_postPayloadId);
        payloadMgr->UnregisterPayload(_tmpPayloadId);

        SendChunkedPayload(warden, payloadMgr, _midPayload, 64);
        LOG_INFO("module", "SetXpBar: SetXpBar.lua payload queued for in-game (%u bytes)", _midPayload.size());
    }

    return true;
}

void AddSetXpBarScripts()
{
    new SetXpBarServerScript();
}