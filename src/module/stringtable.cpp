#include <std_include.hpp>
#include <loader/module_loader.hpp>
#include <game/game.hpp>

#include "stringtable.hpp"
#include "console.hpp"

#include <utils/string.hpp>
#include <utils/memory.hpp>
#include <utils/hook.hpp>
#include <utils/csv.hpp>
#include "asset_handler.hpp"


void stringtable::FFile::read()
{
	char* _buffer = nullptr;
	int size = game::native::FS_ReadFile(this->filePath.data(), &_buffer);

	this->buffer.clear();

	if (size >= 0)
	{
		this->buffer.append(_buffer, size);
		game::native::FS_FreeFile(_buffer);
	}
}

std::unordered_map<std::string, game::native::StringTable*> stringtable::StringTableMap;

game::native::StringTable* stringtable::LoadObject(std::string filename)
{
	utils::memory::allocator* allocator = utils::memory::get_allocator();

	filename = utils::string::to_lower(filename);

	game::native::StringTable* table = nullptr;
	stringtable::FFile rawTable(filename);

	if (rawTable.exists())
	{
		utils::CSV parsedTable(rawTable.getBuffer(), false, false);
		table = allocator->allocate<game::native::StringTable>();

		if (table)
		{
			table->name = allocator->duplicate_string(filename);
			table->columnCount = parsedTable.getColumns();
			table->rowCount = parsedTable.getRows();

			table->values = allocator->allocate_array<game::native::StringTableCell>(table->columnCount * table->rowCount);

			if (!table->values)
			{
				return nullptr;
			}

			for (int i = 0; i < table->rowCount; ++i)
			{
				for (int j = 0; j < table->columnCount; ++j)
				{
					std::string value = parsedTable.getElementAt(i, j);

					game::native::StringTableCell* cell = &table->values[i * table->columnCount + j];
					cell->hash = game::native::StringTable_HashString(value.data());
					cell->string = allocator->duplicate_string(value);
				}
			}

			stringtable::StringTableMap[filename] = table;
		}
	}
	else
	{
		stringtable::StringTableMap[filename] = nullptr;
	}

	return table;
}

void stringtable::post_load()
{
	AssetHandler::OnFind(game::native::XAssetType::ASSET_TYPE_STRINGTABLE, [](game::native::XAssetType, const std::string& _filename)
	{
		game::native::XAssetHeader header = { nullptr };

		std::string filename = utils::string::to_lower(_filename);

		if (stringtable::StringTableMap.find(filename) != stringtable::StringTableMap.end())
		{
			header.stringTable = stringtable::StringTableMap[filename];
		}
		else
		{
			header.stringTable = stringtable::LoadObject(filename);
		}

		return header;
	});
}

REGISTER_MODULE(stringtable);