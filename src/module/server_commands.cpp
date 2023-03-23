#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include "command.hpp"
#include "server_commands.hpp"

#include <utils/hook.hpp>

std::unordered_map<std::int32_t, std::function<bool(command::params)>> servercommands::Commands;

void servercommands::OnCommand(std::int32_t cmd, std::function<bool(command::params)> callback)
{
	servercommands::Commands.insert_or_assign(cmd, std::move(callback));
}

bool servercommands::OnServerCommand()
{
	command::params params;

	for (const auto& [id, callback] : servercommands::Commands)
	{
		if (params.length() >= 1)
		{
			return callback(params);
		}
	}

	return false;
}

__declspec(naked) void servercommands::CG_DeployServerCommand_Stub()
{
	__asm
	{
		push eax
		pushad
		call servercommands::OnServerCommand
		mov[esp + 20h], eax
		popad
		pop eax

		test al, al
		jnz jumpback

		cmp eax, ebx
		jle error

		mov edx, dword ptr[edx * 4 + 1408BA4h]
		mov eax, [edx]

		push 449330h
		retn

		error :
		push 44932Bh
			retn

			jumpback :
		push 4493B5h
			retn
	}
}

void servercommands::post_load()
{
	// Server command receive hook
	utils::hook(0x44931C, servercommands::CG_DeployServerCommand_Stub).install()->quick();
}


REGISTER_MODULE(servercommands);