#pragma once

class music final : public module
{
public:
	void post_load() override;

	static void Replace(const std::string& sound, const char* file);

private:
	static std::unordered_map<std::string, const char*> SoundAliasList;
	static game::native::XAssetHeader ModifyAliases(game::native::XAssetType type, const std::string& filename);
};