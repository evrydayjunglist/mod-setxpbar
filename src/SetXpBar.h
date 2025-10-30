#ifndef MODULE_SETXPBAR_H
#define MODULE_SETXPBAR_H

#include "Player.h"
#include "ScriptMgr.h"
#include "WardenWin.h"
#include "Config.h"

extern bool sxp_Enabled;
extern bool sxp_UseIndividualXpFormat;

const std::string _addonPayloadSetXpBar = R"lua(
local function SetXpRate(rate)
    -- Try both command formats for compatibility
    -- First try .xp N format for Custom_XP.lua
    SendChatMessage(".xp "..rate, "SAY")
end
local xpRates = {
    {text="Experience:",isTitle=true},
    {text="x1 (Blizzlike)",func=function() SetXpRate(1) end},
    {text="x2",func=function() SetXpRate(2) end},
    {text="x3",func=function() SetXpRate(3) end},
    {text="x5",func=function() SetXpRate(5) end},
    {text="x7",func=function() SetXpRate(7) end},
    {text="x12",func=function() SetXpRate(12) end},
    {text="Custom",func=function() StaticPopup_Show("SET_XP_RATE") end}
}
local function InitializeMenu(self,level)
    if not level then return end
    for _,rate in pairs(xpRates) do
        local info=UIDropDownMenu_CreateInfo()
        info.text=rate.text
        info.func=rate.func
        info.isTitle=rate.isTitle
        UIDropDownMenu_AddButton(info,level)
    end
end
-- Create menu with XPRM name for ElvUI compatibility
local xpBarMenu=CreateFrame("Frame","XPRM",UIParent,"UIDropDownMenuTemplate")
xpBarMenu.initialize=InitializeMenu
-- Function to show menu
local function ShowXpMenu()
    EasyMenu(xpRates,xpBarMenu,"cursor",3,-3,"MENU")
end
-- Hook ElvUI's existing OnClick if available, otherwise attach to default bar
local function SetupXpBar()
    -- Check for ElvUI bar (either the frame or via DB.StatusBars)
    local elvBar = _G.ElvUI_ExperienceBar
    local elvBarHolder = _G.ElvUI_ExperienceBarHolder
    if not elvBar and _G.ElvUI then
        local DB = _G.ElvUI:GetModule("DataBars")
        if DB and DB.StatusBars and DB.StatusBars.Experience then
            elvBar = DB.StatusBars.Experience
            if elvBar and elvBar.holder then
                elvBarHolder = elvBar.holder
            end
        end
    end
    if elvBar then
        -- Hook the holder's OnMouseDown (ElvUI's primary click handler)
        if elvBarHolder then
            local originalOnClick = elvBarHolder:GetScript("OnMouseDown")
            elvBarHolder:SetScript("OnMouseDown", function(self, button)
                if button == "RightButton" then
                    ShowXpMenu()
                elseif originalOnClick then
                    originalOnClick(self, button)
                end
            end)
        end
        -- Also hook the bar's OnMouseUp for compatibility
        if elvBar then
            local originalOnMouseUp = elvBar:GetScript("OnMouseUp")
            elvBar:HookScript("OnMouseUp", function(self, button)
                if button == "RightButton" then
                    ShowXpMenu()
                elseif originalOnMouseUp then
                    originalOnMouseUp(self, button)
                end
            end)
        end
        return true
    end
    -- Fall back to default XP bar
    local defBar = _G.MainMenuExpBar
    if defBar then
        defBar:EnableMouse(true)
        defBar:SetScript("OnMouseDown", function(self, button)
            if button == "RightButton" then
                ShowXpMenu()
            end
        end)
        return true
    end
    return false
end
-- Try to setup immediately, then retry on PLAYER_ENTERING_WORLD
if not SetupXpBar() then
    local frame = CreateFrame("Frame")
    frame:RegisterEvent("PLAYER_ENTERING_WORLD")
    frame:SetScript("OnEvent", function(self)
        if SetupXpBar() then
            self:UnregisterAllEvents()
        end
    end)
end
StaticPopupDialogs["SET_XP_RATE"]={
    text="Enter custom XP rate:",
    button1="Set Rate",
    button2="Cancel",
    hasEditBox=true,
    OnAccept=function(self)
        local rate=self.editBox:GetText()
        SetXpRate(rate)
    end,
    timeout=0,
    whileDead=true,
    hideOnEscape=true,
    preferredIndex=3
}
)lua";

const std::string _addonPayloadSetXpBarIndividualXp = R"lua(
local function SetXpRate(rate)
    -- Use .xp set N format for mod-individual-xp compatibility
    SendChatMessage(".xp set "..rate, "SAY")
end
local xpRates = {
    {text="Experience:",isTitle=true},
    {text="x1 (Blizzlike)",func=function() SetXpRate(1) end},
    {text="x2",func=function() SetXpRate(2) end},
    {text="x3",func=function() SetXpRate(3) end},
    {text="x5",func=function() SetXpRate(5) end},
    {text="x7",func=function() SetXpRate(7) end},
    {text="x12",func=function() SetXpRate(12) end},
    {text="Custom",func=function() StaticPopup_Show("SET_XP_RATE") end}
}
local function InitializeMenu(self,level)
    if not level then return end
    for _,rate in pairs(xpRates) do
        local info=UIDropDownMenu_CreateInfo()
        info.text=rate.text
        info.func=rate.func
        info.isTitle=rate.isTitle
        UIDropDownMenu_AddButton(info,level)
    end
end
-- Create menu with XPRM name for ElvUI compatibility
local xpBarMenu=CreateFrame("Frame","XPRM",UIParent,"UIDropDownMenuTemplate")
xpBarMenu.initialize=InitializeMenu
-- Function to show menu
local function ShowXpMenu()
    EasyMenu(xpRates,xpBarMenu,"cursor",3,-3,"MENU")
end
-- Hook ElvUI's existing OnClick if available, otherwise attach to default bar
local function SetupXpBar()
    -- Check for ElvUI bar (either the frame or via DB.StatusBars)
    local elvBar = _G.ElvUI_ExperienceBar
    local elvBarHolder = _G.ElvUI_ExperienceBarHolder
    if not elvBar and _G.ElvUI then
        local DB = _G.ElvUI:GetModule("DataBars")
        if DB and DB.StatusBars and DB.StatusBars.Experience then
            elvBar = DB.StatusBars.Experience
            if elvBar and elvBar.holder then
                elvBarHolder = elvBar.holder
            end
        end
    end
    if elvBar then
        -- Hook the holder's OnMouseDown (ElvUI's primary click handler)
        if elvBarHolder then
            local originalOnClick = elvBarHolder:GetScript("OnMouseDown")
            elvBarHolder:SetScript("OnMouseDown", function(self, button)
                if button == "RightButton" then
                    ShowXpMenu()
                elseif originalOnClick then
                    originalOnClick(self, button)
                end
            end)
        end
        -- Also hook the bar's OnMouseUp for compatibility
        if elvBar then
            local originalOnMouseUp = elvBar:GetScript("OnMouseUp")
            elvBar:HookScript("OnMouseUp", function(self, button)
                if button == "RightButton" then
                    ShowXpMenu()
                elseif originalOnMouseUp then
                    originalOnMouseUp(self, button)
                end
            end)
        end
        return true
    end
    -- Fall back to default XP bar
    local defBar = _G.MainMenuExpBar
    if defBar then
        defBar:EnableMouse(true)
        defBar:SetScript("OnMouseDown", function(self, button)
            if button == "RightButton" then
                ShowXpMenu()
            end
        end)
        return true
    end
    return false
end
-- Try to setup immediately, then retry on PLAYER_ENTERING_WORLD
if not SetupXpBar() then
    local frame = CreateFrame("Frame")
    frame:RegisterEvent("PLAYER_ENTERING_WORLD")
    frame:SetScript("OnEvent", function(self)
        if SetupXpBar() then
            self:UnregisterAllEvents()
        end
    end)
end
StaticPopupDialogs["SET_XP_RATE"]={
    text="Enter custom XP rate:",
    button1="Set Rate",
    button2="Cancel",
    hasEditBox=true,
    OnAccept=function(self)
        local rate=self.editBox:GetText()
        SetXpRate(rate)
    end,
    timeout=0,
    whileDead=true,
    hideOnEscape=true,
    preferredIndex=3
}
)lua";

const std::string _prePayload = "wlbuf = '';";
const std::string _postPayload = "local a,b=loadstring(wlbuf)if not a then message(b)else a()end";
const std::string _midPayload = _addonPayloadSetXpBar;
const std::string _midPayloadIndividualXp = _addonPayloadSetXpBarIndividualXp;

const uint16 _prePayloadId = 9600;
const uint16 _postPayloadId = 9601;
const uint16 _tmpPayloadId = 9602;

class SetXpBarServerScript : public ServerScript
{
public:
    SetXpBarServerScript() : ServerScript("SetXpBarServerScript", { SERVERHOOK_CAN_PACKET_SEND }) {}

private:
    bool CanPacketSend(WorldSession* session, WorldPacket& packet) override;
    std::vector<std::string> GetChunks(std::string s, uint8_t chunkSize);
    void SendChunkedPayload(Warden* warden, WardenPayloadMgr* payloadMgr, std::string payload, uint32 chunkSize);
};

class SetXpBarWorldScript : public WorldScript
{
public:
    SetXpBarWorldScript() : WorldScript("SetXpBarWorldScript", { WORLDHOOK_ON_AFTER_CONFIG_LOAD }) {}

private:
    void OnAfterConfigLoad(bool reload) override;
};

#endif // MODULE_SETXPBAR_H