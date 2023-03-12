@echo off
git submodule update --init --recursive
tools\premake5 %* vs2022 --copy-to="G:\SteamLibrary\steamapps\common\Call of Duty 4"
