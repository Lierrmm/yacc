#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include <module/assets/zone_builder.hpp>

#include <module/setup/console.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/memory.hpp>

#include "asset_handler.hpp"

thread_local int AssetHandler::BypassState = 0;
bool AssetHandler::ShouldSearchTempAssets = false;
std::map <game::native::XAssetType, AssetHandler::IAsset*> AssetHandler::AssetInterfaces;
std::map<game::native::XAssetType, utils::string::Slot<AssetHandler::Callback>> AssetHandler::TypeCallbacks;
utils::string::Signal<AssetHandler::RestrictCallback> AssetHandler::RestrictSignal;
std::map<void*, void*> AssetHandler::Relocations;
utils::hook::detour find_xasset_header_hook;
utils::hook::detour add_xasset_hook;

std::vector<std::pair<game::native::XAssetType, std::string>> AssetHandler::EmptyAssets;

std::map<std::string, game::native::XAssetHeader> AssetHandler::TemporaryAssets[game::native::XAssetType::ASSET_TYPE_COUNT];

void AssetHandler::ClearTemporaryAssets()
{
	for (int i = 0; i < game::native::XAssetType::ASSET_TYPE_COUNT; ++i)
	{
		AssetHandler::TemporaryAssets[i].clear();
	}
}

void AssetHandler::StoreTemporaryAsset(game::native::XAssetType type, game::native::XAssetHeader asset)
{
	AssetHandler::TemporaryAssets[type][game::native::DB_GetXAssetNameHandlers[type](&asset)] = asset;
}

game::native::XAssetHeader AssetHandler::FindAsset(game::native::XAssetType type, const char* name)
{
	if (AssetHandler::HasThreadBypass() > 0)
	{
		checkTempAssets:
		if (AssetHandler::ShouldSearchTempAssets)
		{
			auto tempAsset = AssetHandler::FindTemporaryAsset(type, name);
			if (tempAsset.data != nullptr) return tempAsset;
			else goto finishOriginal;
		}
		else goto finishOriginal;
	}
	else
	{
		game::native::XAssetHeader customAsset = { nullptr };

		if (name)
		{
			// Allow call DB_FindXAssetHeader within the hook
			AssetHandler::SetBypassState(true);

			if (AssetHandler::TypeCallbacks.find(type) != AssetHandler::TypeCallbacks.end())
			{
				customAsset = AssetHandler::TypeCallbacks[type](type, name);
			}

			// Disallow calling DB_FindXAssetHeader ;)
			AssetHandler::SetBypassState(false);

			if (customAsset.data != nullptr) return customAsset;
			else goto checkTempAssets;
		}
		else goto checkTempAssets;
	}

finishOriginal:
	auto originalHeader = find_xasset_header_hook.invoke<game::native::XAssetHeader>(type, name);

	return originalHeader;
}



bool AssetHandler::IsAssetEligible(game::native::XAssetType type, game::native::XAssetHeader asset)
{
	const char* name = game::native::DB_GetXAssetNameHandlers[type](&asset);
	if (!name) return false;

	for (auto i = AssetHandler::EmptyAssets.begin(); i != AssetHandler::EmptyAssets.end();)
	{
		if (i->first == type && i->second == name)
		{
			i = AssetHandler::EmptyAssets.erase(i);
		}
		else
		{
			++i;
		}
	}

	bool restrict = false;
	AssetHandler::RestrictSignal(type, asset, name, &restrict);

	return (!restrict);
}


game::native::XAssetHeader AssetHandler::AddAsset(game::native::XAssetType type, game::native::XAssetHeader asset)
{
	if (!AssetHandler::IsAssetEligible(type, asset))
		return {};

	return add_xasset_hook.invoke<game::native::XAssetHeader>(asset);
}

void AssetHandler::DB_AddXAsset(game::native::XAssetHeader asset/*, game::native::XAssetType type*/)
{
	uint32_t result{};
	// move eax register into result
	// eax contains the asset type
	__asm
	{
		mov result, eax
	}

	if (result == NULL)
		return;

	game::native::XAssetType _type = (game::native::XAssetType)result;
	AssetHandler::AddAsset(_type, asset);
}

void AssetHandler::test(game::native::XAssetType* type/*, game::native::XAssetHeader* header*/)
{
	uint32_t result{};
	__asm
	{
		mov eax, [esp + 30h]
		mov result, eax
		pop eax
	}

	game::native::XAssetHeader header = *(game::native::XAssetHeader*)result;
	auto fakeType = *type;

	__asm
	{
		push eax
		pushad
		push[header]
		push[fakeType]
		call AssetHandler::IsAssetEligible
		add esp, 8h

		mov[esp + 18h], eax
		popad
		pop eax

		test al, al
		jz doNotLoad

		mov     ebp, esp
		and esp, 0FFFFFFF8h
		sub     esp, 88Ch
		push    ebx
		push    esi
		push    edi
		mov     edi, [ebp + 8]
		//mov     eax, [edi]
		mov ecx, 484FF0h
		jmp ecx

		doNotLoad :
		mov     ebp, esp
		and esp, 0FFFFFFF8h
		sub     esp, 88Ch
		push    ebx
		push    esi
		push    edi
		mov     edi, [ebp + 8]
		//mov     eax, [edi]
		retn
	}
}

game::native::XAssetHeader AssetHandler::FindTemporaryAsset(game::native::XAssetType type, const char* filename)
{
	game::native::XAssetHeader header = { nullptr };
	if (type >= game::native::XAssetType::ASSET_TYPE_COUNT) return header;

	auto tempPool = &AssetHandler::TemporaryAssets[type];
	auto entry = tempPool->find(filename);
	if (entry != tempPool->end())
	{
		header = { entry->second };
	}

	return header;
}

int AssetHandler::HasThreadBypass()
{
	return AssetHandler::BypassState > 0;
}

void AssetHandler::SetBypassState(bool value)
{
	AssetHandler::BypassState += (value ? 1 : -1);
	if (AssetHandler::BypassState < 0)
	{
		AssetHandler::BypassState = 0;
		console::error("Bypass state is below 0!");
	}
}

void AssetHandler::ResetBypassState()
{
	if (AssetHandler::HasThreadBypass())
	{
		AssetHandler::BypassState = 0;
	}
}

void AssetHandler::OnFind(game::native::XAssetType type, utils::string::Slot<AssetHandler::Callback> callback)
{
	AssetHandler::TypeCallbacks[type] = callback;
}

void AssetHandler::OnLoad(utils::string::Slot<AssetHandler::RestrictCallback> callback)
{
	AssetHandler::RestrictSignal.connect(callback);
}

void AssetHandler::ClearRelocations()
{
	AssetHandler::Relocations.clear();
}

void AssetHandler::Relocate(void* start, void* to, DWORD size)
{
	for (DWORD i = 0; i < size; i += 4)
	{
		AssetHandler::Relocations[reinterpret_cast<char*>(start) + i] = reinterpret_cast<char*>(to) + i;
	}
}

void AssetHandler::ZoneSave(game::native::XAsset asset, Components::ZoneBuilder::Zone* builder)
{
	if (AssetHandler::AssetInterfaces.find(asset.type) != AssetHandler::AssetInterfaces.end())
	{
		AssetHandler::AssetInterfaces[asset.type]->save(asset.header, builder);
	}
	else
	{
		console::error("No interface for type '%s'!", game::native::DB_GetXAssetTypeName(asset.type));
	}
}

void AssetHandler::ZoneMark(game::native::XAsset asset, Components::ZoneBuilder::Zone* builder)
{
	if (AssetHandler::AssetInterfaces.find(asset.type) != AssetHandler::AssetInterfaces.end())
	{
		AssetHandler::AssetInterfaces[asset.type]->mark(asset.header, builder);
	}
	else
	{
		console::error("No interface for type '%s'!", game::native::DB_GetXAssetTypeName(asset.type));
	}
}

game::native::XAssetHeader AssetHandler::FindAssetForZone(game::native::XAssetType type, const std::string& filename, Components::ZoneBuilder::Zone* builder, bool isSubAsset)
{
	Components::ZoneBuilder::Zone::AssetRecursionMarker _(builder);

	game::native::XAssetHeader header = { nullptr };
	if (type >= game::native::XAssetType::ASSET_TYPE_COUNT) return header;

	auto tempPool = &AssetHandler::TemporaryAssets[type];
	auto entry = tempPool->find(filename);
	if (entry != tempPool->end())
	{
		return { entry->second };
	}

	if (AssetHandler::AssetInterfaces.find(type) != AssetHandler::AssetInterfaces.end())
	{
		AssetHandler::AssetInterfaces[type]->load(&header, filename, builder);

		if (header.data)
		{
			AssetHandler::StoreTemporaryAsset(type, header);
		}
	}

	if (!header.data && isSubAsset)
	{
		header = Components::ZoneBuilder::GetEmptyAssetIfCommon(type, filename, builder);
	}

	if (!header.data)
	{
		header = game::native::DB_FindXAssetHeader(type, filename.data());
		if (header.data) AssetHandler::StoreTemporaryAsset(type, header);
	}

	return header;
}

game::native::XAssetHeader AssetHandler::FindOriginalAsset(game::native::XAssetType type, const char* filename)
{
	AssetHandler::SetBypassState(true);
	game::native::XAssetHeader header = game::native::DB_FindXAssetHeader(type, filename);
	AssetHandler::SetBypassState(false);

	return header;
}

void AssetHandler::StoreEmptyAsset(game::native::XAssetType type, const char* name)
{
	AssetHandler::EmptyAssets.push_back({ type, name });
}

void AssetHandler::ExposeTemporaryAssets(bool expose)
{
	AssetHandler::ShouldSearchTempAssets = expose;
}

void AssetHandler::post_load()
{
	AssetHandler::ClearTemporaryAssets();

	find_xasset_header_hook.create(game::native::DB_FindXAssetHeader, AssetHandler::FindAsset);
	
	//add_xasset_hook.create(0x485330, AssetHandler::DB_AddXAsset);
}

void AssetHandler::pre_destroy()
{
	AssetHandler::ClearTemporaryAssets();

	for (auto i = AssetHandler::AssetInterfaces.begin(); i != AssetHandler::AssetInterfaces.end(); ++i)
	{
		delete i->second;
	}

	AssetHandler::Relocations.clear();
	AssetHandler::AssetInterfaces.clear();
	AssetHandler::RestrictSignal.clear();
	AssetHandler::TypeCallbacks.clear();
}

REGISTER_MODULE(AssetHandler);