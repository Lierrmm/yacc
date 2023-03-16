#pragma once

#include "structs.hpp"
#include "launcher/launcher.hpp"

#define SELECT_VALUE(sp, mp) (game::is_sp() ? (sp) : (mp))

#define SERVER_CD_KEY "YACC-CD-Key"

// *
// Common
#define FF_LOAD_ADDON_MENU		false
#define FF_ADDON_MENU_NAME		"xcommon_yacc_menu"

namespace game
{
	bool is_mp();
	bool is_sp();

	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t sp_address, const size_t mp_address)
			: sp_object_(reinterpret_cast<T*>(sp_address))
			, mp_object_(reinterpret_cast<T*>(mp_address))
		{
		}

		T* get() const
		{
			if (is_sp())
			{
				return sp_object_;
			}

			return mp_object_;
		}

		operator T* () const
		{
			return this->get();
		}

		T* operator->() const
		{
			return this->get();
		}

	private:
		T* sp_object_;
		T* mp_object_;
	};

	namespace native
	{
		extern dvar_t* Dvar_FindVar(const char* name);
		extern void dvar_set_value_dirty(dvar_t* dvar, bool value);
		extern void ConDraw_Box(float* color, float x, float y, float width, float height);
		extern inline dvar_t* Dvar_RegisterBool(const char* dvar_name, const char* description, std::int32_t default_value, std::uint16_t flags);
		//extern inline dvar_t* Dvar_RegisterFloat(const char* dvarName, const char* description, float defaultValue, float minValue, float maxValue, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterEnum(const char* dvar_name, const char* description, std::int32_t default_value, std::int32_t enum_size, const char** enum_data, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterVec2(const char* dvar_name, const char* description, float x, float y, float min_value, float max_value, std::uint16_t flags);
		extern void Cbuf_AddText(const char* text /*eax*/, int local_client_num /*ecx*/);
		extern void Cmd_AddCommand(const char* name, void(*callback)(), cmd_function_s* data, char);
		extern void Cmd_AddCommand(const char* name, const char* args, const char* description, void(*callback)(), cmd_function_s* data, char);
		extern void FS_DisplayPath(int bLanguageCull /*eax*/);

		extern void UI_DrawText(const ScreenPlacement* scrPlace, const char* text, int maxChars, Font_s* font, float ix, float iy, int horzAlign, int vertAlign, float scale, const float* color, int style);
		extern int UI_TextHeight(Font_s* font, float scale);
		extern int UI_TextWidth(const char* text, int maxChars, Font_s* font, float scale);
		extern int R_TextHeight(Font_s* font);

		extern int String_Parse(const char** p /*eax*/, char* out_str, int len);
		extern void Menus_OpenByName(const char* menu_name, game::native::UiContext* ui_dc);
		extern void Menus_CloseByName(const char* menu_name, game::native::UiContext* ui_dc);
		extern void Menus_CloseAll(game::native::UiContext* ui_dc);

		extern const char* UI_SafeTranslateString(const char* name);

		extern const char* UI_GetCurrentMapName();
		extern const char* UI_GetCurrentGameType();

		extern const char* Win_GetLanguage();

		extern unsigned int R_HashString(const char* string);
		extern void Conbuf_AppendText_ASM(const char* string);

		extern float Vec3Normalize(vec3_t& vec);
		extern void Vec2UnpackTexCoords(const PackedTexCoords in, vec2_t* out);
		extern void MatrixVecMultiply(const float(&mulMat)[3][3], const vec3_t& mulVec, vec3_t& solution);
		extern void Vec3UnpackUnitVec(PackedUnitVec in, vec3_t& out);

		extern XAssetHeader db_realloc_xasset_pool(XAssetType type, unsigned int new_size);

		extern bool DB_FileExists(const char* file_name, game::native::DB_FILE_EXISTS_PATH source);

		namespace glob
		{
			// general
			extern std::string loaded_modules;
			extern std::string loaded_libaries;

			// movement
			extern bool lpmove_check_jump; // if Jumped in Check_Jump, reset after x frames in PmoveSingle

			extern vec3_t lpmove_velocity;	// grab local player velocity
			extern vec3_t lpmove_origin;		// grab local player origin
			extern vec3_t lpmove_angles;		// grab local player angles
			extern vec3_t lpmove_camera_origin; // grab local camera origin

			// ui / devgui
			extern bool loaded_main_menu;
			extern bool mainmenu_fade_done;

			extern gui_t gui;
			extern std::string changelog_html_body;

			// renderer
			extern void* d3d9_device;

			// debug collision
			extern bool debug_collision_initialized; // debug collision was used
			extern int  debug_collision_rendered_brush_amount; // total amount of brushes used for calculations of planes 
			extern int  debug_collision_rendered_planes_amount;	// total amount of planes rendered 
			extern int  debug_collision_rendered_planes_counter; // total amount of planes rendered used to count while drawing

			extern std::string	r_drawCollision_materialList_string;

			// Frametime
			extern int lpmove_server_time;
			extern int lpmove_server_time_old;
			extern int lpmove_server_frame_time;
			extern int lpmove_pml_frame_time;

			// Misc
			extern int q3_last_projectile_weapon_used; // ENUM Q3WeaponNames :: this var holds the last proj. weapon that got fired
		}

		// Global Definitions & Functions
		constexpr auto JUMP_LAND_SLOWDOWN_TIME = 1800;

		constexpr auto MAX_QPATH = 64;
		constexpr auto MAX_OSPATH = 256;

		// Angle indexes
		constexpr auto PITCH = 0; // up / down
		constexpr auto YAW = 1; // left / right
		constexpr auto ROLL = 2; // fall over

		// From Quake III, to match game's assembly
		template <typename T, typename R>
		constexpr auto VectorScale(T v, R s, T out) { out[0] = v[0] * s; out[1] = v[1] * s; out[2] = v[2] * s; }

		int Vec4Compare(const float* a, const float* b);

		bool I_islower(int c);
		bool I_isupper(int c);
	}
	void initialize(launcher::mode mode);
}

#include "symbols.hpp"