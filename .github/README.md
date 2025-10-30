# SetXpBar Module

The `mod-setxpbar` module for AzerothCore enhances the [Individual XP Module](https://github.com/azerothcore/mod-individual-xp) by injecting a client-side script that allows players to control their experience gain rate by **right-clicking their XP bar**. This module eliminates the need for players to manually install an [addon](https://github.com/brian8544/SetXpBar) or know specific commands, providing a seamless way to customize their leveling experience, whether they prefer a faster pace or the classic Blizzlike progression. The idea for this module is inspired by this exact feature found on large World of Warcraft private servers, such as [Warmane](https://www.warmane.com/).

## Requirements
- [mod-individual-xp](https://github.com/azerothcore/mod-individual-xp) module installed and configured.
- OR [Config_rates](https://github.com/Day36512/Config_Rates)
- An **unpatched World of Warcraft 3.3.5 client**. RCE-Patched clients using tools like [RCEPatcher](https://github.com/stoneharry/RCEPatcher) will prevent the script injection from working.

## Installation
1. Clone this repository into your AzerothCore `modules` directory
2. Re-run CMake and rebuild your AzerothCore server to include the `mod-setxpbar` module.
3. Copy the `mod_setxpbar.conf.dist` file to your server's configuration directory.

## Demo
![GIF](https://i.imgur.com/gctjFVh.gif)

## Copyright
License: GPL 2.0
