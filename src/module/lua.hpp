#pragma once

class lua_engine final : public module
{
public:
	void post_load() override;
	void post_unpack() override;

	void load_script(const std::string& name);
	void load_scripts(const std::string& script_dir);
	void loadLuaScripts();

	struct Engine
	{
		Engine() { }

		void Conbuf_AppendText(const char* message) { game::native::Conbuf_AppendText_ASM(message); }
		const char* mapname = game::native::UI_GetCurrentMapName();
		const char* gametype = game::native::UI_GetCurrentGameType();
		float GetDvarFloat(const char* name) 
		{ 
			auto dvar = game::native::Dvar_FindVar(name);
			
			if (dvar) return dvar->current.value;
			return NULL;
		}
	};
private:
};
