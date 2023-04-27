#pragma once

#include "structs.hpp"
#include "launcher/launcher.hpp"

#define SERVER_CD_KEY "YACC-CD-Key"

// *
// Common
#define FF_LOAD_ADDON_MENU		true
#define FF_ADDON_MENU_NAME		"yacc"

#define BASEGAME "yacc"

//#define CUSTOM_MASTER_IP "150.230.121.10"
#define CUSTOM_MASTER_IP "192.168.50.100"
#define CUSTOM_MASTER_PORT 20810

//#define USING_CUSTOM_MASTER // unset this to use default masterserver

namespace game
{
	template <typename T>
	class symbol
	{
	public:
		symbol(const size_t mp_address) : mp_object_(reinterpret_cast<T*>(mp_address))
		{
		}

		T* get() const
		{
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
		T* mp_object_;
	};

	namespace native
	{
		// console
		extern ConDrawInputGlob* conDrawInputGlob;
		extern Console* con;
		extern field_t* g_consoleField;
		extern bool* Key_IsCatcherActive;
		extern float* con_matchtxtColor_currentDvar;
		extern float* con_matchtxtColor_currentValue;
		extern float* con_matchtxtColor_defaultValue;
		extern float* con_matchtxtColor_dvarDescription;
		extern float* con_matchtxtColor_domainDescription;
		extern bool* extvar_con_ignoreMatchPrefixOnly;

		extern ScreenPlacement* scrPlaceFull;
		extern ScreenPlacement* scrPlace;

		extern PlayerKeyState* playerKeys;
		extern clientUIActive_t* clientUI;

		static DWORD* con_font_ptr = (DWORD*)(0xC52E94);

		typedef void(__cdecl* ConDrawInput_DvarMatch_t)(const char*);
		extern ConDrawInput_DvarMatch_t ConDrawInput_DvarMatch;

		extern void AddBaseDrawConsoleTextCmd(int char_count /*eax*/, const float* color_float /*ecx*/, const char* text_pool, int pool_size, int first_char, game::native::Font_s* font, float x, float y, float x_scale, float y_scale, int style);
		extern void Dvar_ForEachName(void(__cdecl* func)(const char*));
		extern void Cmd_ForEachXO(void(__cdecl* callback)(const char*));
		extern char* Cmd_Argv(int arg_index);
		extern void Con_DrawAutoCompleteChoice(int is_dvar_cmd, char* text);
		extern void Cmd_ForEach_PassCmd(void(__cdecl* callback)(cmd_function_s*));
		extern void Con_DrawMessageWindowOldToNew(DWORD* msg_window /*esi*/, int local_client_num, int x_pos, int y_pos, int char_height, int horz_align, int vert_align, int mode, Font_s* font, const float* color, int text_style, float msgwnd_scale, int text_align_mode);


		extern dvar_t* Dvar_FindVar(const char* name);
		extern void dvar_set_value_dirty(dvar_t* dvar, bool value);
		extern void ConDraw_Box(float* color, float x, float y, float width, float height);
		extern inline dvar_t* Dvar_RegisterBool(const char* dvar_name, const char* description, std::int32_t default_value, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterInt(const char* dvar_name, const char* description, std::int32_t default_value, std::int32_t min_value, std::int32_t max_value, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterFloat(const char* dvarName, const char* description, float defaultValue, float minValue, float maxValue, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterEnum(const char* dvar_name, const char* description, std::int32_t default_value, std::int32_t enum_size, const char** enum_data, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterVec2(const char* dvar_name, const char* description, float x, float y, float min_value, float max_value, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterVec3(const char* dvar_name, const char* description, float x, float y, float z, float min_value, float max_value, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterVec4(const char* dvar_name, const char* description, float x, float y, float z, float w, float min_value, float max_value, std::uint16_t flags);
		extern inline dvar_t* Dvar_RegisterString(const char* dvar_name, const char* description, const char* default_value, std::uint16_t flags);
		
		extern void Cbuf_AddText(const char* text /*eax*/, int local_client_num /*ecx*/);
		extern void Cmd_AddCommand(const char* name, void(*callback)(), cmd_function_s* data, char);
		extern void Cmd_AddCommand(const char* name, const char* args, const char* description, void(*callback)(), cmd_function_s* data, char);
		extern void FS_DisplayPath(int bLanguageCull /*eax*/);

		extern char* Com_Parse(const char** data_p);

		extern void UI_DrawText(const ScreenPlacement* scrPlace, const char* text, int maxChars, Font_s* font, float ix, float iy, int horzAlign, int vertAlign, float scale, const float* color, int style);
		extern int UI_TextHeight(Font_s* font, float scale);
		extern int UI_TextWidth(const char* text, int maxChars, Font_s* font, float scale);
		extern int R_TextHeight(Font_s* font);
		extern int R_TextWidth(const char* text /*<eax*/, int maxChars, Font_s* font);
		extern void _R_AddCmdDrawText(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, const float* color, int style);

		extern void CL_DrawStretchPicPhysical(float x, float y, float w, float h, float s0, float t0, float s1, float t1, float* color, game::native::Material* material);
		extern void R_AddCmdDrawStretchPic(game::native::Material* material, float x, float y, float w, float h, float null1, float null2, float null3, float null4, float* color);

		extern int String_Parse(const char** p /*eax*/, char* out_str, int len);
		extern void Menus_OpenByName(const char* menu_name, game::native::UiContext* ui_dc);
		extern void Menus_CloseByName(const char* menu_name, game::native::UiContext* ui_dc);
		extern void Menus_CloseAll(game::native::UiContext* ui_dc);
		
		extern void R_AddCmdDrawTextASM(const char* text, int max_chars, void* font, float x, float y, float x_scale, float y_scale, float rotation, const float* color, int style);
		extern void draw_text_with_engine(float x, float y, float scale_x, float scale_y, const float* color, const char* text);

		extern const char* UI_SafeTranslateString(const char* name);

		extern const char* UI_GetCurrentMapName();
		extern const char* UI_GetCurrentGameType();

		extern const char* Win_GetLanguage();

		extern unsigned int R_HashString(const char* string);
		extern void Conbuf_AppendText_ASM(const char* string);
		extern scr_entref_t __cdecl Scr_GetEntityIdRef(unsigned int entId);


		extern float Vec3Normalize(vec3_t& vec);
		extern void Vec2UnpackTexCoords(const PackedTexCoords in, vec2_t* out);
		extern void MatrixVecMultiply(const float(&mulMat)[3][3], const vec3_t& mulVec, vec3_t& solution);
		extern void Vec3UnpackUnitVec(PackedUnitVec in, vec3_t& out);

		extern XAssetHeader db_realloc_xasset_pool(XAssetType type, unsigned int new_size);

		extern bool DB_FileExists(const char* file_name, game::native::DB_FILE_EXISTS_PATH source);

		extern const char* DB_GetXAssetTypeName(XAssetType type);
		extern XAssetEntry* DB_FindXAssetEntry(XAssetType type, const char* name);

		extern void FS_FreeFile(void* buf);
		extern int FS_ReadFile(const char* path, char** buffer);
		extern int StringTable_HashString(const char* string);
		extern const char* TableLookup(StringTable* stringtable, int row, int column);

		extern bool SV_Loaded();
		extern void FreeMemory(void* Block);

		extern void Image_Release(game::native::GfxImage* image);
		extern void Image_Setup(GfxImage* image, unsigned int width, unsigned int height, unsigned int depth, unsigned int flags, _D3DFORMAT format);
		extern void unzClose(char* Block);

		extern void __cdecl Scr_AddString(const char* value);
		extern void Scr_Notify(gentity_s* ent, unsigned short constString, unsigned int numArgs);

		extern bool Sys_IsDatabaseReady();
		extern bool Sys_IsDatabaseReady2();

		extern void R_AddDebugBounds(float* color, Bounds* b);
		extern void R_AddDebugBounds(float* color, Bounds* b, const float(*quat)[4]);
		extern void R_AddDebugString(float* /*color*/, float* /*pos*/, float /*scale*/, const char* /*string*/);

		extern float Vec3SqrDistance(const float v1[3], const float v2[3]);
		
		namespace wrappers
		{
			const char* SL_ConvertToString(scr_string_t stringValue);
			void Scr_ShutdownAllocNode();
		}

		namespace network
		{
			extern void SockadrToNetadr(sockaddr* from, netadr_t* addr);
			extern bool NET_CompareAdr(netadr_t a, netadr_t b);
			extern void NetadrToSockadr(netadr_t* a, sockaddr* s);
			extern bool NET_IsLocalAddress(netadr_t adr);
		}

		constexpr auto MAX_QPATH = 64;
		constexpr auto MAX_OSPATH = 256;

		// Angle indexes
		constexpr auto PITCH = 0; // up / down
		constexpr auto YAW = 1; // left / right
		constexpr auto ROLL = 2; // fall over

		// From Quake III, to match game's assembly
		template <typename T, typename R>
		constexpr auto VectorScale(T v, R s, T out) { out[0] = v[0] * s; out[1] = v[1] * s; out[2] = v[2] * s; }

		//int Vec4Compare(const float* a, const float* b);

		bool I_islower(int c);
		bool I_isupper(int c);
	}
	void initialize(launcher::mode mode);
}

#include "symbols.hpp"