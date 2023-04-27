#pragma once

#define WEAK __declspec(selectany)

// *
// Version
#define YACC_BUILDVERSION_DATE __DATE__
#define YACC_BUILDSTRING "YACC :: build :: %s", __DATE__

// *
// Gui
#define GET_GGUI game::native::glob::gui
#define GGUI_READY game::native::glob::gui.imgui_initialized
#define GGUI_READY_DVARS game::native::glob::gui.dvars_initialized
#define GGUI_ANY_MENUS_OPEN game::native::glob::gui.any_menus_open

#define FONT_SMALL_DEV  "fonts/smallDevFont"
#define FONT_BIG_DEV    "fonts/bigDevFont"
#define FONT_CONSOLE    "fonts/consoleFont"
#define FONT_BIG        "fonts/bigFont"
#define FONT_SMALL      "fonts/smallFont"
#define FONT_BOLD       "fonts/boldFont"
#define FONT_NORMAL     "fonts/normalFont"
#define FONT_EXTRA_BIG  "fonts/extraBigFont"
#define FONT_OBJECTIVE  "fonts/objectiveFont"

#define HORIZONTAL_ALIGN_SUBLEFT		0	// left edge of a 4:3 screen (safe area not included)
#define HORIZONTAL_ALIGN_LEFT			1	// left viewable (safe area) edge
#define HORIZONTAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define HORIZONTAL_ALIGN_RIGHT			3	// right viewable (safe area) edge
#define HORIZONTAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define HORIZONTAL_ALIGN_NOSCALE		5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define HORIZONTAL_ALIGN_TO640			6	// scales a real-screen resolution x down into the 0 - 640 range
#define HORIZONTAL_ALIGN_CENTER_SAFEAREA 7	// center of the safearea

#define VERTICAL_ALIGN_SUBTOP			0	// top edge of the 4:3 screen (safe area not included)
#define VERTICAL_ALIGN_TOP				1	// top viewable (safe area) edge
#define VERTICAL_ALIGN_CENTER			2	// center of the screen (reticle)
#define VERTICAL_ALIGN_BOTTOM			3	// bottom viewable (safe area) edge
#define VERTICAL_ALIGN_FULLSCREEN		4	// disregards safe area
#define VERTICAL_ALIGN_NOSCALE			5	// uses exact parameters - neither adjusts for safe area nor scales for screen size
#define VERTICAL_ALIGN_TO480			6	// scales a real-screen resolution y down into the 0 - 480 range
#define VERTICAL_ALIGN_CENTER_SAFEAREA	7	// center of the save area

#define HORIZONTAL_APPLY_LEFT				0
#define HORIZONTAL_APPLY_CENTER				1
#define HORIZONTAL_APPLY_RIGHT				2
#define HORIZONTAL_APPLY_FULLSCREEN			3
#define HORIZONTAL_APPLY_NONE				4
#define HORIZONTAL_APPLY_TO640				5
#define HORIZONTAL_APPLY_CENTER_SAFEAREA	6
#define HORIZONTAL_APPLY_CONSOLE_SPECIAL	7

#define VERTICAL_APPLY_TOP					0
#define VERTICAL_APPLY_CENTER				1
#define VERTICAL_APPLY_BOTTOM				2
#define VERTICAL_APPLY_FULLSCREEN			3
#define VERTICAL_APPLY_NONE					4
#define VERTICAL_APPLY_TO640				5
#define VERTICAL_APPLY_CENTER_SAFEAREA		6

#define KEYCATCHER_NONE	0x0
#define KEYCATCHER_TAB	0x9
#define KEYCATCHER_ENTER 0xD
#define KEYCATCHER_ESCAPE 0x1B
#define KEYCATCHER_SPACE 0x20
#define KEYCATCHER_GRAVE 0x60
#define KEYCATCHER_TILDE 0x7E
#define KEYCATCHER_BACKSPACE 0x7F
#define KEYCATCHER_ASCII_FIRST 0x80
#define KEYCATCHER_ASCII_181 0x80
#define KEYCATCHER_ASCII_191 0x81
#define KEYCATCHER_ASCII_223 0x82
#define KEYCATCHER_ASCII_224 0x83
#define KEYCATCHER_ASCII_225 0x84
#define KEYCATCHER_ASCII_228 0x85
#define KEYCATCHER_ASCII_229 0x86
#define KEYCATCHER_ASCII_230 0x87
#define KEYCATCHER_ASCII_231 0x88
#define KEYCATCHER_ASCII_232 0x89
#define KEYCATCHER_ASCII_233 0x8A
#define KEYCATCHER_ASCII_236 0x8B
#define KEYCATCHER_ASCII_241 0x8C
#define KEYCATCHER_ASCII_242 0x8D
#define KEYCATCHER_ASCII_243 0x8E
#define KEYCATCHER_ASCII_246 0x8F
#define KEYCATCHER_ASCII_248 0x90
#define KEYCATCHER_ASCII_249 0x91
#define KEYCATCHER_ASCII_250 0x92
#define KEYCATCHER_ASCII_252 0x93
#define KEYCATCHER_END_ASCII_CHARS 0x94
#define KEYCATCHER_COMMAND 0x96
#define KEYCATCHER_CAPSLOCK 0x97
#define KEYCATCHER_POWER 0x98
#define KEYCATCHER_PAUSE 0x99
#define KEYCATCHER_UPARROW 0x9A
#define KEYCATCHER_DOWNARROW 0x9B
#define KEYCATCHER_LEFTARROW 0x9C
#define KEYCATCHER_RIGHTARROW 0x9D
#define KEYCATCHER_ALT 0x9E
#define KEYCATCHER_CTRL 0x9F
#define KEYCATCHER_SHIFT 0xA0
#define KEYCATCHER_INS 0xA1
#define KEYCATCHER_DEL 0xA2
#define KEYCATCHER_PGDN 0xA3
#define KEYCATCHER_PGUP 0xA4
#define KEYCATCHER_HOME 0xA5
#define KEYCATCHER_END 0xA6
#define KEYCATCHER_F1 0xA7
#define KEYCATCHER_F2 0xA8
#define KEYCATCHER_F3 0xA9
#define KEYCATCHER_F4 0xAA
#define KEYCATCHER_F5 0xAB
#define KEYCATCHER_F6 0xAC
#define KEYCATCHER_F7 0xAD
#define KEYCATCHER_F8 0xAE
#define KEYCATCHER_F9 0xAF
#define KEYCATCHER_F10 0xB0
#define KEYCATCHER_F11 0xB1
#define KEYCATCHER_F12 0xB2
#define KEYCATCHER_F13 0xB3
#define KEYCATCHER_F14 0xB4
#define KEYCATCHER_F15 0xB5
#define KEYCATCHER_KP_HOME 0xB6
#define KEYCATCHER_KP_UPARROW 0xB7
#define KEYCATCHER_KP_PGUP 0xB8
#define KEYCATCHER_KP_LEFTARROW 0xB9
#define KEYCATCHER_KP_5 0xBA
#define KEYCATCHER_KP_RIGHTARROW 0xBB
#define KEYCATCHER_KP_END 0xBC
#define KEYCATCHER_KP_DOWNARROW 0xBD
#define KEYCATCHER_KP_PGDN 0xBE
#define KEYCATCHER_KP_ENTER 0xBF
#define KEYCATCHER_KP_INS 0xC0
#define KEYCATCHER_KP_DEL 0xC1
#define KEYCATCHER_KP_SLASH 0xC2
#define KEYCATCHER_KP_MINUS 0xC3
#define KEYCATCHER_KP_PLUS 0xC4
#define KEYCATCHER_KP_NUMLOCK 0xC5
#define KEYCATCHER_KP_STAR 0xC6
#define KEYCATCHER_KP_EQUALS 0xC7
#define KEYCATCHER_MOUSE1 0xC8
#define KEYCATCHER_MOUSE2 0xC9
#define KEYCATCHER_MOUSE3 0xCA
#define KEYCATCHER_MOUSE4 0xCB
#define KEYCATCHER_MOUSE5 0xCC
#define KEYCATCHER_MWHEELDOWN 0xCD
#define KEYCATCHER_MWHEELUP 0xCE
#define KEYCATCHER_AUX1 0xCF
#define KEYCATCHER_AUX2 0xD0
#define KEYCATCHER_AUX3 0xD1
#define KEYCATCHER_AUX4 0xD2
#define KEYCATCHER_AUX5 0xD3
#define KEYCATCHER_AUX6 0xD4
#define KEYCATCHER_AUX7 0xD5
#define KEYCATCHER_AUX8 0xD6
#define KEYCATCHER_AUX9 0xD7
#define KEYCATCHER_AUX10 0xD8
#define KEYCATCHER_AUX11 0xD9
#define KEYCATCHER_AUX12 0xDA
#define KEYCATCHER_AUX13 0xDB
#define KEYCATCHER_AUX14 0xDC
#define KEYCATCHER_AUX15 0xDD
#define KEYCATCHER_AUX16 0xDE
#define KEYCATCHER_LAST_KEY 0xDF

#define fs_loadStack *(int*)(0xCB12ABC)

namespace game
{
	namespace native
	{
		typedef int(__cdecl* DB_GetXAssetSizeHandler_t)();
		typedef const char* (*DB_GetXAssetNameHandler_t)(XAssetHeader* asset);

		WEAK symbol < void()> Sys_ShowConsole{  0x574DC0 };
		WEAK symbol < void (const char* message)> Conbuf_AppendText{  0x574E40 };
		WEAK symbol < void* (LocalClientNum_t localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{  0x4F4660 };
		WEAK symbol <dvar_t* (const char* dvarName)> Dvar_FindMalleableVar{  0x565C80 };
		WEAK symbol <Font_s* (const char* fontName, int fontSize)> R_RegisterFont{  0x5D1B20 };
		WEAK symbol <Material* (const char* materialName, int unk)> Material_RegisterHandle{ 0x5D26E0 };
		WEAK symbol <void()> Sys_InitMainThread{  0x505D70 };
		WEAK symbol <bool()> Sys_IsMainThread{  0x506110 };
		WEAK symbol <void()> RegisterDvars{  0x53C220 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_bool, std::uint16_t flags, const char* description, std::int32_t default_value, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t null4, std::int32_t null5)> Dvar_RegisterBool_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type typeInt, std::uint16_t flags, const char* description, std::int32_t defaultValue, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t minValue, std::int32_t maxValue)> Dvar_RegisterInt_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_enum, std::uint16_t flags, const char* description, std::int32_t default_index, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t enumSize, const char** enum_data)> Dvar_RegisterEnum_r{  0x5667F0 };
		//WEAK symbol <dvar_t* (const char* dvar_name, float default_value, float min_value, float max_value, std::uint16_t flags, const char* description)> Dvar_RegisterFloat_r{  0x566B70 };
		WEAK symbol<dvar_t* (const char* name, dvar_type type, unsigned short flags, DvarValue value, DvarLimits limits, const char* desc)> Cvar_Register{ 0x566B70 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_float, std::uint16_t flags, const char* description, float x, std::int32_t null0, std::int32_t null1, std::int32_t null2, float min, float max)> Dvar_RegisterFloat_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_float2, std::uint16_t flags, const char* description, float x, float y, std::int32_t null1, std::int32_t null2, float min, float max)> Dvar_RegisterVec2_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_float3, std::uint16_t flags, const char* description, float x, float y, float z, std::int32_t null2, float min, float max)> Dvar_RegisterVec3_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_float4, std::uint16_t flags, const char* description, float x, float y, float z, float w, float min, float max)> Dvar_RegisterVec4_r{  0x5667F0 };
		WEAK symbol <dvar_t* (const char* dvar_name, dvar_type type_string, std::uint16_t flags, const char* description, const char* default_value, std::int32_t null1, std::int32_t null2, std::int32_t null3, std::int32_t null4, std::int32_t null5)> Dvar_RegisterString_r{  0x5667F0 };

		WEAK symbol <void(int, const char*, char)> Com_PrintMessage{  0x4F75E0 };
		WEAK symbol <void(const char* text, int maxChars, Font_s* font, float x, float y, float xScale, float yScale, float rotation, const float* color, int style)> R_AddCmdDrawText{  0x5D6700 };
		WEAK symbol <void()> Key_SetCatcher{  0x464A80 };
		WEAK symbol <const char*()> Sys_DefaultInstallPath{  0x56D480 };
		WEAK symbol <void(const char* map)> LoadMapLoadScreenInternal{  0x466C00 };
		//WEAK symbol <dvar_t*(const char* dvarName, const char* string, int arg3)> Dvar_SetFromStringByName{ 0x567830 };
		WEAK symbol <bool(const char* address, netadr_t* adr)> NET_StringToAdr{ 0x503A80 };
		WEAK symbol <const char* (const char* pszInputBuffer, const char* pszMessageType, msgLocErrType_t errType)> SEH_LocalizeTextMessage{ 0x533730 };
		WEAK symbol <void(const char* var_name, const char* var_value)> Cvar_SetCommand{ 0x5678A0 };

		WEAK symbol<const char* (const char* mode)> GetModeName {  0x53E3E0 };
		WEAK symbol<const char* (const char* map)> GetMapName {  0x53E310 };

		WEAK symbol<XAssetHeader(XAssetType type, const char* name)> DB_FindXAssetHeader {  0x484A50 };
		WEAK symbol<int(XAssetType type, const char* name)> DB_IsXAssetDefault {  0x484D80 };
		WEAK symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{  0x484600 };
		WEAK symbol<void(XZoneInfo* zoneInfo, unsigned int zone_count, int sync)> DB_LoadXAssets{  0x485790 };
		WEAK symbol<void(int level, const char* error, ...)> Com_Error {  0x4F7ED0 };
		WEAK symbol<int(const char* a1, int* a2, int a3)> FS_FOpenFileRead{  0x556020 };
		WEAK symbol<int(char* Buffer, int ElementCount, int a3)> FS_Read{  0x5567E0 };
		WEAK symbol<char*(const char* file, bool forceEnglish)> SE_Load{  0x534AF0 };
		WEAK symbol<char*(bool forceEnglish)> SE_LoadLanguage{  0x534D70 };

		WEAK symbol<void()> R_BeginRemoteScreenUpdate{  0x5D74A0 };
		WEAK symbol<void()> R_EndRemoteScreenUpdate{  0x5D74F0 };
		WEAK symbol<void()> DB_SyncXAssets{  0x485600 };

		WEAK symbol<int()> Sys_Milliseconds {  0x573650 };
		WEAK symbol<DWORD()> Sys_SuspendOtherThreads{  0x506150 };
		WEAK symbol<unsigned int(const char* string, int user, int size)> SL_GetString{ 0x512DA0};
		WEAK symbol<script_t*(int length)> Script_Alloc{  0x41C2F0 };
		WEAK symbol<void(script_t* script)> Script_SetupTokens{  0x41FDF0 };
		WEAK symbol<int(char* buffer)> Script_CleanString{  0x56B510 };
		WEAK symbol<void(int, const char*, ...)> PC_SourceError{  0x54F970 };
		WEAK symbol<int(int handle, pc_token_s* pc_token)> PC_ReadTokenHandle{  0x41FC60 };  //maybe requires some asm
		WEAK symbol<menuDef_t* (UiContext* dc, const char* name)> Menus_FindByName{  0x546880 };
		WEAK symbol<bool(UiContext* dc, menuDef_t* menu)> Menu_IsVisible{  0x54ED30 }; //maybe requires some asm
		WEAK symbol<MenuList* (const char* menuFile)> UI_LoadMenus{  0x552D70 };
		WEAK symbol<void(UiContext* dc, MenuList* menuList)> UI_AddMenuList{  0x54F5D0 }; //maybe require some asm
		WEAK symbol<MenuList* (const char* a2)> sub_552E10{  0x552E10 };
		WEAK symbol<char* (int localClientNum, char* Destination)> CL_GetClientName{  0x471550 }; //failed to load .menu
		WEAK symbol<gentity_s* ()> SV_AddTestClient{  0x527F30 };
		WEAK symbol<void(unsigned int id)> RemoveRefToObject{  0x514A40 };
		WEAK symbol<void(const float* vectorValue)> RemoveRefToVector{  0x514BD0 };
		WEAK symbol<void(const int stringValue)> SL_RemoveRefToString{ 0x513450 };
		WEAK symbol<void(unsigned int, unsigned int, VariableValue*)> VM_Notify{ 0x51C760 };
		WEAK symbol<char*(int a1, const char* filename)> LoadModdableRawfile{ 0x511FF0 };
		WEAK symbol<void()> DB_BeginRecoverLostDevice{ 0x484EC0 };
		WEAK symbol<void()> DB_EndRecoverLostDevice{ 0x484F50 };
		WEAK symbol<void(int clientNum, int menuNum)> UI_SetActiveMenu{ 0x543B90 };
		WEAK symbol<void(dvar_t* dvar, float value, int source)> Dvar_SetFloat{ 0x5670E0 };
		WEAK symbol<void(int x, int y, const char* text)> SCR_DrawSmallStringExt{ 0x4701C0 };
		WEAK symbol<bool(const char* query, const char* match_to_text, int match_text_len)> Con_IsAutoCompleteMatch{ 0x45BC70 };

		WEAK symbol<char*()> Con_TokenizeInput{ 0x45B630 };
		WEAK symbol<void()> Con_CancelAutoComplete{ 0x45CD90 };
		WEAK symbol<void(/*int a1*/)> Con_DrawInputPrompt{ 0x45C7E0 }; // may take 1 argument
		WEAK symbol<void()> Cmd_EndTokenizedString{ 0x4F4470 };

		WEAK symbol<void(const char*)> ConDrawInput_IncrMatchCounter{ 0x45BD20 };
		WEAK symbol<bool()> Con_AnySpaceAfterCommand{ 0x45B6B0 };
		WEAK symbol<void(const char*)> ConDrawInput_DetailedDvarMatch{ 0x45C270 };
		WEAK symbol<void(const char*)> ConDrawInput_DetailedCmdMatch{ 0x45C640 };


		//WEAK symbol<void(const char*)> ConDrawInput_DvarMatch{ 0x45BD90 };
		WEAK symbol<void(const char*)> ConDrawInput_CmdMatch{ 0x45C710 };
		WEAK symbol<void(float x, float y, float width, float height)> Con_DrawOutputScrollBar{ 0x45DB70 };
		WEAK symbol<void(float x, float y)> Con_DrawOutputText{ 0x45DCF0 };

		WEAK symbol<void(int a1, char* text)> Con_DrawAutoCompleteChoice_func{ 0x45C760 };
		WEAK symbol<bool(DWORD lpAddress)> sub_55EBB0{ 0x55EBB0 };
		WEAK symbol<int(int)> Scr_ExecThread{ 0x51D560 };

		namespace network 
		{
			WEAK symbol<const char*(netadr_t adr)> NET_AdrToString{ 0x502540 };
			WEAK symbol<bool(netsrc_t sock, size_t len, const char* format, netadr_t adr)> Sys_SendPacket { 0x503680 };
			WEAK symbol<int> numIP{ 0xCC0FA9C };
			WEAK symbol<netIP_t> localIP{ 0xCC0FAA0 };
		}
			

		// symbols
		WEAK symbol<bool> CL_IsCgameInitialized{  0xC578F6 };
		WEAK symbol<HANDLE> databaseCompletedEvent { 0x14E09A4 };
		WEAK symbol<HANDLE> databaseCompletedEvent2 { 0x14E09E4 };
		
		WEAK symbol<UiContext> ui_context{  0xCAE6200 };
		WEAK symbol<UiContext> ui_cg_dc{  0x73EFA8 };
		WEAK symbol<CmdArgs> cmd_args{  0x1408B40 };
		WEAK symbol<CmdArgs> sv_cmd_args{  0x142B408 };
		WEAK symbol<cmd_function_s*> cmd_ptr{  0x1408B3C };

		WEAK symbol<localization_t> localization{  0xCC0C7D4 };
		WEAK symbol<cg_s> cgs{  0x746338 };
		WEAK symbol<client_t> svs_clients{ 0x1CB7C8C };

		WEAK symbol<const char> serverMode{  0x742928 };
		WEAK symbol<const char> serverMap {  0xCADE158 };

		WEAK symbol<const char*> g_assetNames{  0x71E838 };
		WEAK symbol<netadr_t> serverAddress{  0x8ECCF0 };

		WEAK symbol<XAssetHeader> DB_XAssetPool{  0x71E5D8 };
		WEAK symbol<unsigned int> g_poolSize{  0x71E398 };

		WEAK symbol<const char*> zone_mod{  0xCC9450C };
		WEAK symbol<const char*> zone_ui_mp{  0xCC944FC };
		WEAK symbol<const char*> zone_localized_code_post_gfx_mp{  0xCC94504 };
		WEAK symbol<const char*> zone_code_post_gfx_mp{  0xCC944F8 };
		WEAK symbol<const char*> zone_common_mp{  0xCC94500 };
		WEAK symbol<const char*> zone_localized_common_mp{  0xCC94508 };
		WEAK symbol<DB_GetXAssetSizeHandler_t> DB_GetXAssetSizeHandlers{  0x71EA08 };
		WEAK symbol<DB_GetXAssetNameHandler_t> DB_GetXAssetNameHandlers{  0x71E8C8 };
		WEAK symbol<HANDLE> database_handle{  0x14E09A4 };
		WEAK symbol<DB_LoadData> g_load{  0xE2C4C8 };
		WEAK symbol<int> sv{  0x17F47C8 };
		WEAK symbol<source_t*> sourceFiles{  0x72FF90 };
		WEAK symbol<keywordHash_t*> menuParseKeywordHash{  0xCB0D110 };
		WEAK symbol<searchpath_t*> fs_searchpaths{  0xD5E44D8 };
		WEAK symbol<scrVmPub_t> gScrVmPub{  0x178C058 };
		WEAK symbol<scrVarPub_t> gScrVarPub{  0x15C2610 };
		WEAK symbol<scrVarGlob_t> scr_VarGlob {  0x168B820 };
		WEAK symbol<GfxCmdBufState> gfxCmdBufState{  0xD4FBAF0 };
		WEAK symbol<IDirect3DDevice9*> dx_ptr{  0xCC91808 };
		WEAK symbol<gentity_s> g_entities{ 0x1280500 };
		WEAK symbol<GfxScene> scene{ 0xCF07600 };

		WEAK symbol<int> gameTypeCount{ 0xCAE981C };
	}
}