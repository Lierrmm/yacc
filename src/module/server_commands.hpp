#pragma once

class servercommands final : public module
{
public:

	void post_load() override;
	static void OnCommand(std::int32_t cmd, std::function<bool(command::params)> callback);

private:
	static std::unordered_map<std::int32_t, std::function<bool(command::params)>> Commands;

	static bool OnServerCommand();
	static void CG_DeployServerCommand_Stub();
};