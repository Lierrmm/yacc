#pragma once
class AssetHandler final : public module
{
public:
	class IAsset
	{
	public:
		virtual ~IAsset() {};
		virtual game::native::XAssetType getType() { return game::native::XAssetType::ASSET_TYPE_INVALID; };
		virtual void mark(game::native::XAssetHeader /*header*/, Components::ZoneBuilder::Zone* /*builder*/) { /*ErrorTypeNotSupported(this);*/ };
		virtual void save(game::native::XAssetHeader /*header*/, Components::ZoneBuilder::Zone* /*builder*/) { /*ErrorTypeNotSupported(this);*/ };
		virtual void dump(game::native::XAssetHeader /*header*/) { /*ErrorTypeNotSupported(this);*/ };
		virtual void load(game::native::XAssetHeader* /*header*/, const std::string& /*name*/, Components::ZoneBuilder::Zone* /*builder*/) { /*ErrorTypeNotSupported(this);*/ };
	};

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

	static void ZoneMark(game::native::XAsset asset, Components::ZoneBuilder::Zone* builder);
	static void ZoneSave(game::native::XAsset asset, Components::ZoneBuilder::Zone* builder);
	static game::native::XAssetHeader FindAssetForZone(game::native::XAssetType type, const std::string& filename, Components::ZoneBuilder::Zone* builder, bool isSubAsset = true);

private:
	static thread_local int BypassState;
	static bool ShouldSearchTempAssets;

	static std::map<game::native::XAssetType, IAsset*> AssetInterfaces;

	static std::map<std::string, game::native::XAssetHeader> TemporaryAssets[game::native::XAssetType::ASSET_TYPE_COUNT];
	
	static std::map<game::native::XAssetType, utils::string::Slot<Callback>> TypeCallbacks;
	static utils::string::Signal<RestrictCallback> RestrictSignal;

	static std::map<void*, void*> Relocations;

	static std::vector<std::pair<game::native::XAssetType, std::string>> EmptyAssets;

	static game::native::XAssetHeader FindAsset(game::native::XAssetType type, const char* name);
	static game::native::XAssetHeader AddAsset(game::native::XAssetType type, game::native::XAssetHeader asset);
	static game::native::XAssetHeader FindTemporaryAsset(game::native::XAssetType type, const char* filename);
	static bool IsAssetEligible(game::native::XAssetType type, game::native::XAssetHeader asset);
	static void DB_AddXAsset(game::native::XAssetHeader asset/*,game::native::XAssetType type*/);
	static void test(game::native::XAssetType* type/*, game::native::XAssetHeader* header*/);

	static void StoreEmptyAsset(game::native::XAssetType type, const char* name);
	static void StoreEmptyAssetStub();

	static int HasThreadBypass();
	static void SetBypassState(bool value);
};