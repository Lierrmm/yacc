#pragma once
class AssetHandler final : public module
{
public:
	typedef game::native::XAssetHeader(Callback)(game::native::XAssetType type, const std::string& name);
	typedef void(RestrictCallback)(game::native::XAssetType type, game::native::XAssetHeader asset, const std::string& name, bool* restrict);

	void post_load() override;
	void pre_destroy() override;

	static void OnFind(game::native::XAssetType type, utils::string::Slot<Callback> callback);
	static void OnLoad(utils::string::Slot<AssetHandler::RestrictCallback> callback);

	static void ClearRelocations();
	static void Relocate(void* start, void* to, DWORD size = 4);

	static game::native::XAssetHeader FindOriginalAsset(game::native::XAssetType type, const char* filename);

	static void ClearTemporaryAssets();
	static void StoreTemporaryAsset(game::native::XAssetType type, game::native::XAssetHeader asset);

	static void ResetBypassState();

	static void ExposeTemporaryAssets(bool expose);

private:
	static thread_local int BypassState;
	static bool ShouldSearchTempAssets;

	static std::map<std::string, game::native::XAssetHeader> TemporaryAssets[game::native::XAssetType::ASSET_TYPE_COUNT];
	
	static std::map<game::native::XAssetType, utils::string::Slot<Callback>> TypeCallbacks;
	static utils::string::Signal<RestrictCallback> RestrictSignal;

	static std::map<void*, void*> Relocations;

	static std::vector<std::pair<game::native::XAssetType, std::string>> EmptyAssets;

	static game::native::XAssetHeader FindAsset(game::native::XAssetType type, const char* name);
	static game::native::XAssetHeader AddAsset(game::native::XAssetType type, game::native::XAssetHeader asset);
	static game::native::XAssetHeader FindTemporaryAsset(game::native::XAssetType type, const char* filename);
	static bool IsAssetEligible(game::native::XAssetType type, game::native::XAssetHeader asset);
	static void DB_AddXAsset(game::native::XAssetHeader asset);

	static void StoreEmptyAsset(game::native::XAssetType type, const char* name);
	static void StoreEmptyAssetStub();

	static int HasThreadBypass();
	static void SetBypassState(bool value);
};