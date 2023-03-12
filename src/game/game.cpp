#include <std_include.hpp>
#include "game.hpp"
#include "dvars.hpp"

namespace game
{
	namespace native
	{
		decltype(longjmp)* _longjmp;

		bool I_islower(int c)
		{
			return c >= 'a' && c <= 'z';
		}

		bool I_isupper(int c)
		{
			return c >= 'A' && c <= 'Z';
		}

		dvar_t* Dvar_FindVar(const char* name)
		{
			return game::native::Dvar_FindMalleableVar(name);
		}

		void dvar_set_value_dirty(game::native::dvar_t* dvar, bool value)
		{
			if (!dvar)
			{
				return;
			}

			dvar->current.enabled = value;
			dvar->latched.enabled = value;
			//_dvar->modified = false;
		}

		// custom functions
		inline dvar_t* Dvar_RegisterBool(const char* dvar_name, const char* description, std::int32_t default_value, std::uint16_t flags) {
			return Dvar_RegisterBool_r(dvar_name, dvar_type::DVAR_TYPE_BOOL, flags, description, default_value, 0, 0, 0, 0, 0);
		}

		inline dvar_t* Dvar_RegisterFloat(const char* dvarName, const char* description, float defaultValue, float minValue, float maxValue, std::uint16_t flags) {
			return Dvar_RegisterFloat_r(dvarName, defaultValue, minValue, maxValue, flags, description);
		}

		inline dvar_t* Dvar_RegisterEnum(const char* dvar_name, const char* description, std::int32_t default_value, std::int32_t enum_size, const char** enum_data, std::uint16_t flags) {
			return Dvar_RegisterEnum_r(dvar_name, dvar_type::DVAR_TYPE_ENUM, flags, description, default_value, 0, 0, 0, enum_size, enum_data);
		}
		
		void R_AddCmdDrawTextASM(const char* text, int max_chars, void* font, float x, float y, float x_scale, float y_scale, float rotation, const float* color, int style)
		{
			const static uint32_t R_AddCmdDrawText_func = 0x5D6700;
			__asm
			{
				push	style;
				sub     esp, 14h;

				fld		rotation;
				fstp[esp + 10h];

				fld		y_scale;
				fstp[esp + 0Ch];

				fld		x_scale;
				fstp[esp + 8];

				fld		y;
				fstp[esp + 4];

				fld		x;
				fstp[esp];

				push	font;
				push	max_chars;
				push	text;
				mov		ecx, [color];

				call	R_AddCmdDrawText_func;
				add		esp, 24h;
			}
		}

		void ConDraw_Box(float* color, float x, float y, float width, float height)
		{
			const static uint32_t ConDraw_Box_func = 0x45B820;
			__asm
			{
				pushad;
				mov		esi, [color];
				sub		esp, 10h;

				fld		height;
				fstp[esp + 0Ch];

				fld		width;
				fstp[esp + 8h];

				fld		y;
				fstp[esp + 4h];

				fld		x;
				fstp[esp];

				call	ConDraw_Box_func;
				add		esp, 10h;
				popad;
			}
		}

		namespace glob
		{
			// general
			std::string loaded_modules;
			std::string loaded_libaries;

			// movement
			bool lpmove_check_jump = false;
			vec3_t lpmove_velocity = {};
			vec3_t lpmove_origin = {};
			vec3_t lpmove_angles = {};
			vec3_t lpmove_camera_origin = {};

			// ui / devgui
			bool loaded_main_menu = false;
			bool mainmenu_fade_done = false;

			gui_t gui = {};
			std::string changelog_html_body;

			// renderer
			void* d3d9_device = nullptr;

			// debug collision
			bool debug_collision_initialized = false;
			int  debug_collision_rendered_brush_amount = 0;
			int  debug_collision_rendered_planes_amount = 0;
			int  debug_collision_rendered_planes_counter = 0;

			std::string	r_drawCollision_materialList_string;

			// frametime
			int lpmove_server_time = 0;
			int lpmove_server_time_old = 0;
			int lpmove_server_frame_time = 0;
			int lpmove_pml_frame_time = 0;

			// misc
			int q3_last_projectile_weapon_used = 0; // ENUM Q3WeaponNames :: this var holds the last proj. weapon that got fired
		}
	}

	launcher::mode mode = launcher::mode::none;

	launcher::mode get_mode()
	{
		if (mode == launcher::mode::none)
		{
			throw std::runtime_error("Launcher mode not valid. Something must be wrong.");
		}

		return mode;
	}

	bool is_mp()
	{
		return get_mode() == launcher::mode::multiplayer;
	}

	bool is_sp()
	{
		return get_mode() == launcher::mode::singleplayer;
	}

	void initialize(const launcher::mode _mode)
	{
		mode = _mode;
		return;
		//dvars::initialize();

		//native::Cmd_AddCommandInternal = native::Cmd_AddCommandInternal_t(SELECT_VALUE(0x558820, 0x545DF0));
		//native::Cmd_RemoveCommand = native::Cmd_RemoveCommand_t(SELECT_VALUE(0x443A30, 0x545E20));

		//native::Cbuf_AddText = native::Cbuf_AddText_t(SELECT_VALUE(0x457C90, 0x545680));
		////native::Conbuf_AppendText = native::Conbuf_AppendText_t(SELECT_VALUE(0x4C84E0, 0x5CF610));

		//native::Com_Error = native::Com_Error_t(SELECT_VALUE(0x425540, 0x555450));
		//native::Com_Filter = native::Com_Filter_t(SELECT_VALUE(0x44EFF0, 0x5B7C30));

		//native::DB_LoadXAssets = native::DB_LoadXAssets_t(SELECT_VALUE(0x48A8E0, 0x4CD020));
		//native::DB_FindXAssetHeader = native::DB_FindXAssetHeader_t(SELECT_VALUE(0x4FF000, 0x4CA620));
		//native::DB_IsXAssetDefault = native::DB_IsXAssetDefault_t(SELECT_VALUE(0x4868E0, 0x4CA800));
		//native::DB_EnumXAssets = native::DB_EnumXAssets_t(SELECT_VALUE(0x50A0D0, 0x4CA2D0));
		//native::DB_GetXAssetName = native::DB_GetXAssetName_t(SELECT_VALUE(0x438100, 0x4B7C10));

		//native::Dvar_RegisterBool = native::Dvar_RegisterBool_t(SELECT_VALUE(0x4914D0, 0x5BE9F0));
		//native::Dvar_RegisterFloat = native::Dvar_RegisterFloat_t(SELECT_VALUE(0x4F9CC0, 0x5BEA80));
		//native::Dvar_RegisterInt = native::Dvar_RegisterInt_t(SELECT_VALUE(0x48CD40, 0x5BEA40));
		//native::Dvar_RegisterString = native::Dvar_RegisterString_t(SELECT_VALUE(0x5197F0, 0x5BEC90));

		//native::Dvar_SetIntByName = native::Dvar_SetIntByName_t(SELECT_VALUE(0x5396B0, 0x5BF560));
		//native::Dvar_SetFromStringByName = native::Dvar_SetFromStringByName_t(SELECT_VALUE(0x4DD090, 0x5BF740));
		//native::Dvar_SetString = native::Dvar_SetString_t(SELECT_VALUE(0x540570, 0x5BF3E0));

		//native::Dvar_ForEach = native::Dvar_ForEach_t(SELECT_VALUE(0x536720, 0x5BFFB0));

		//native::Dvar_DisplayableValue = native::Dvar_DisplayableValue_t(SELECT_VALUE(0x4AB1D0, 0x5BD260));
		//native::Dvar_DisplayableLatchedValue = native::Dvar_DisplayableLatchedValue_t(SELECT_VALUE(0x464F50, 0x5BD290));

		//native::G_RunFrame = native::G_RunFrame_t(SELECT_VALUE(0x52EAA0, 0x50CB70));
		//native::G_GetWeaponForName = native::G_GetWeaponForName_t(SELECT_VALUE(0x495E40, 0x531070));

		//native::MSG_ReadData = native::MSG_ReadData_t(SELECT_VALUE(0, 0x5592A0));

		//native::MT_AllocIndex = native::MT_AllocIndex_t(SELECT_VALUE(0x4B9610, 0x562080));

		//native::FindVariable = native::FindVariable_t(SELECT_VALUE(0x4C4E70, 0x5651F0));
		//native::RemoveRefToValue = native::RemoveRefToValue_t(SELECT_VALUE(0x477EA0, 0x565730));
		//native::GetEntityFieldValue = native::GetEntityFieldValue_t(SELECT_VALUE(0x530E30, 0x56AF20));

		//native::SL_GetStringOfSize = native::SL_GetStringOfSize_t(SELECT_VALUE(0x4E13F0, 0x564650));

		//native::Scr_AddEntityNum = native::Scr_AddEntityNum_t(SELECT_VALUE(0x0, 0x56ABC0));
		//native::Scr_AddString = native::Scr_AddString_t(SELECT_VALUE(0x4A5600, 0x56AC00));
		//native::Scr_AddInt = native::Scr_AddInt_t(SELECT_VALUE(0x42DE20, 0x56AA20));
		//native::Scr_AddFloat = native::Scr_AddFloat_t(SELECT_VALUE(0x5349D0, 0x56AA70));

		//native::Scr_Notify = native::Scr_Notify_t(SELECT_VALUE(0x4895B0, 0x52B190));
		//native::Scr_NotifyLevel = native::Scr_NotifyLevel_t(SELECT_VALUE(0x445E10, 0x56B6B0));
		//native::Scr_GetNumParam = native::Scr_GetNumParam_t(SELECT_VALUE(0x4C6FE0, 0x56AA10));
		//native::Scr_GetString = native::Scr_GetString_t(SELECT_VALUE(0x497530, 0x56A3D0));
		//native::Scr_CastString = native::Scr_CastString_t(SELECT_VALUE(0x447CE0, 0x566EE0));
		//native::Scr_ErrorInternal = native::Scr_ErrorInternal_t(SELECT_VALUE(0x42B910, 0x568FD0));
		//native::Scr_LoadScript = native::Scr_LoadScript_t(SELECT_VALUE(0x4D45D0, 0x561CC0));
		//native::Scr_GetFunctionHandle = native::Scr_GetFunctionHandle_t(SELECT_VALUE(0x51DD50, 0x5618A0));
		//native::Scr_ExecThread = native::Scr_ExecThread_t(SELECT_VALUE(0x4FC590, 0x56E240));
		//native::Scr_FreeThread = native::Scr_FreeThread_t(SELECT_VALUE(0x51FD90, 0x569E20));
		//native::Scr_RegisterFunction = native::Scr_RegisterFunction_t(SELECT_VALUE(0x49E190, 0x561520));

		//native::GetObjectType = native::GetObjectType_t(SELECT_VALUE(0x4D8FE0, 0x565C60));

		//native::Sys_Error = native::Sys_Error_t(SELECT_VALUE(0x490D90, 0x5CC3B0));
		//native::Sys_Milliseconds = native::Sys_Milliseconds_t(SELECT_VALUE(0x4A1610, 0x5CE740));
		//native::Sys_Sleep = native::Sys_Sleep_t(SELECT_VALUE(0x438600, 0x55F460));
		//native::Sys_FreeFileList = native::Sys_FreeFileList_t(SELECT_VALUE(0x486380, 0x5C4F90));
		//native::Sys_MessageBox = native::Sys_MessageBox_t(SELECT_VALUE(0x4664D0, 0x5CD180));

		//native::PMem_AllocFromSource_NoDebug = native::PMem_AllocFromSource_NoDebug_t(SELECT_VALUE(0x449E50, 0x5C15C0));

		//native::Hunk_AllocateTempMemoryHighInternal = native::Hunk_AllocateTempMemoryHighInternal_t(SELECT_VALUE(0x517870, 0x5B6C60));
		//native::Hunk_FreeTempMemory = native::Hunk_FreeTempMemory_t(SELECT_VALUE(0x434A40, 0x5B6F90));

		//native::Hunk_UserDestroy = native::Hunk_UserDestroy_t(SELECT_VALUE(0x50FE20, 0x5B7520));

		//native::VM_Notify = native::VM_Notify_t(SELECT_VALUE(0x610200, 0x569720));

		//native::BG_NetDataChecksum = native::BG_NetDataChecksum_t(SELECT_VALUE(0x0, 0x41BB20));
		//native::BG_GetMaxSprintTime = native::BG_GetMaxSprintTime_t(SELECT_VALUE(0x4FAA50, 0x41BA70));

		//native::LiveStorage_GetPersistentDataDefVersion = native::LiveStorage_GetPersistentDataDefVersion_t(SELECT_VALUE(0x0, 0x548D60));

		//native::LiveStorage_GetPersistentDataDefFormatChecksum = native::LiveStorage_GetPersistentDataDefFormatChecksum_t(SELECT_VALUE(0x0, 0x548D80));

		//native::SV_DirectConnect = native::SV_DirectConnect_t(SELECT_VALUE(0x0, 0x572750));
		//native::SV_ClientEnterWorld = native::SV_ClientEnterWorld_t(SELECT_VALUE(0x0, 0x571100));
		//native::SV_Cmd_TokenizeString = native::SV_Cmd_TokenizeString_t(SELECT_VALUE(0x0, 0x545D40));
		//native::SV_Cmd_EndTokenizedString = native::SV_Cmd_EndTokenizedString_t(SELECT_VALUE(0x0, 0x545D70));
		//native::SV_SpawnServer = native::SV_SpawnServer_t(SELECT_VALUE(0x0, 0x575020));
		//native::SV_GetConfigstring = native::SV_GetConfigstring_t(SELECT_VALUE(0x4C6E30, 0x573D50));
		//native::mp::SV_GameSendServerCommand = native::mp::SV_GameSendServerCommand_t(0x573220);
		//native::mp::SV_GetGuid = native::mp::SV_GetGuid_t(0x573990);

		//native::sp::IsServerRunning = native::sp::IsServerRunning_t(0x45D310);
		//native::sp::SV_GameSendServerCommand = native::sp::SV_GameSendServerCommand_t(0x402130);

		//native::sp::TeleportPlayer = native::sp::TeleportPlayer_t(0x4CCEE0);

		//native::XUIDToString = native::XUIDToString_t(SELECT_VALUE(0x4FAA30, 0x55CC20));

		//native::SEH_LocalizeTextMessage = native::SEH_LocalizeTextMessage_t(SELECT_VALUE(0x41EA20, 0x57E240));
		//native::SEH_GetLanguageName = native::SEH_GetLanguageName_t(SELECT_VALUE(0x533650, 0x57E5A0));

		//native::CM_TransformedCapsuleTrace = native::CM_TransformedCapsuleTrace_t(SELECT_VALUE(0x4F9B80, 0x541340));

		//native::PM_WeaponUseAmmo = native::PM_WeaponUseAmmo_t(SELECT_VALUE(0x463F80, 0x42E930));
		//native::PM_playerTrace = native::PM_playerTrace_t(SELECT_VALUE(0x4CE600, 0x421F00));
		//native::PM_trace = native::PM_trace_t(SELECT_VALUE(0x544BF0, 0x41CEB0));

		//native::Jump_ClearState = native::Jump_ClearState_t(SELECT_VALUE(0x514CE0, 0x4160F0));

		//native::Vec3Normalize = native::Vec3Normalize_t(SELECT_VALUE(0x536A20, 0x415EE0));

		//native::Cmd_ExecuteSingleCommand = native::Cmd_ExecuteSingleCommand_t(SELECT_VALUE(0x4D6960, 0x5462B0));

		//native::Com_Quit_f = native::Com_Quit_f_t(SELECT_VALUE(0x4F48B0, 0x5556B0));

		//native::FS_Printf = native::FS_Printf_t(SELECT_VALUE(0x421E90, 0x5AF7C0));
		//native::FS_ReadFile = native::FS_ReadFile_t(SELECT_VALUE(0x4D8DF0, 0x5B1FB0));
		//native::FS_CreatePath = native::FS_CreatePath_t(SELECT_VALUE(0x4F5AB0, 0x5AF060));
		//native::FS_HandleForFile = native::FS_HandleForFile_t(SELECT_VALUE(0x46B1C0, 0x5AEE50));
		//native::FS_FCloseFile = native::FS_FCloseFile_t(SELECT_VALUE(0x415160, 0x5AF170));
		//native::FS_ListFilteredFiles = native::FS_ListFilteredFiles_t(SELECT_VALUE(0x41D910, 0x5AFF10));

		//native::player_die = native::player_die_t(SELECT_VALUE(0x0, 0x503460));

		//native::LargeLocalResetToMark = native::LargeLocalResetToMark_t(SELECT_VALUE(0x524350, 0x5B7150));

		//native::Win_LocalizeRef = native::Win_LocalizeRef_t(SELECT_VALUE(0x49D7E0, 0x5CBE90));

		//native::Key_KeynumToString = native::Key_KeynumToString_t(SELECT_VALUE(0x4BB000, 0x48C080));

		//native::_longjmp = reinterpret_cast<decltype(longjmp)*>(SELECT_VALUE(0x73AC20, 0x7363BC));

		//native::sv_cmd_args = reinterpret_cast<native::CmdArgs*>(SELECT_VALUE(0x1757218, 0x1CAA998));
		//native::cmd_args = reinterpret_cast<native::CmdArgs*>(SELECT_VALUE(0x1750750, 0x1C978D0));

		//native::scrVarGlob = reinterpret_cast<short*>(SELECT_VALUE(0x19AFC80, 0x1E72180));
		//native::scrMemTreePub = reinterpret_cast<char**>(SELECT_VALUE(0x196FB00, 0x1E32000));
		//native::scrMemTreeGlob = reinterpret_cast<char*>(SELECT_VALUE(0x186DA00, 0x1D6FF00));

		//native::scr_function_stack = reinterpret_cast<native::function_stack_t*>(SELECT_VALUE(0x1BF255C, 0x20B4A5C));
		//native::scr_VarPub = reinterpret_cast<native::scrVarPub_t*>(SELECT_VALUE(0x0, 0x208E188));
		//native::scr_VmPub = reinterpret_cast<native::scrVmPub_t*>(SELECT_VALUE(0x1BF2580, 0x20B4A80));

		//native::g_script_error_level = reinterpret_cast<int*>(SELECT_VALUE(0x1BEFCFC, 0x20B21FC));
		//native::g_script_error = reinterpret_cast<jmp_buf*>(SELECT_VALUE(0x1BF1D18, 0x20B4218));

		//native::g_classMap = reinterpret_cast<native::scr_classStruct_t*>(SELECT_VALUE(0x92D140, 0x8B4300));

		//native::svs_clientCount = reinterpret_cast<int*>(SELECT_VALUE(0x0, 0x4B5CF8C));

		//native::levelEntityId = reinterpret_cast<unsigned int*>(SELECT_VALUE(0x1BCBCA4, 0x208E1A4));

		//native::mp::svs_clients = reinterpret_cast<native::mp::client_t*>(0x4B5CF90);

		//native::g_entities = reinterpret_cast<native::gentity_s*>(0x1A66E28);
		//native::sp::g_entities = reinterpret_cast<native::sp::gentity_s*>(0x1197AD8);

		//native::sp::g_clients = reinterpret_cast<native::sp::gclient_s*>(0x1381D48);

		//native::mp::level = reinterpret_cast<native::level_locals_t*>(0x1C6D4D8);

		//native::deferredQueue = reinterpret_cast<native::DeferredQueue*>(SELECT_VALUE(0x0, 0x1D55438));

		//native::com_codeTimeScale = reinterpret_cast<float*>(SELECT_VALUE(0x1769F1C, 0x1CEF554));

		//native::com_frameTime = reinterpret_cast<int*>(SELECT_VALUE(0x0, 0x1CF0B88));

		//native::s_criticalSection = reinterpret_cast<RTL_CRITICAL_SECTION*>(SELECT_VALUE(0x1CD5638, 0x5A91048));

		//native::logfile = reinterpret_cast<int*>(SELECT_VALUE(0x176B534, 0x1CF0B78));

		//native::fs_searchpaths = reinterpret_cast<native::searchpath_s**>(SELECT_VALUE(0x1C2FE78, 0x59BA858));
		//native::fs_gamedir = reinterpret_cast<char*>(SELECT_VALUE(0x1C2B220, 0x59A98F8));
		//native::fsh = reinterpret_cast<native::fileHandleData_t*>(SELECT_VALUE(0x1C2B540, 0x59B5F20));
		//native::fs_numServerIwds = reinterpret_cast<int*>(SELECT_VALUE(0x1C2FE84, 0x59BA864));
		//native::fs_serverIwds = reinterpret_cast<int*>(SELECT_VALUE(0x0, 0x59AAB18));
		//native::fs_iwdFileCount = reinterpret_cast<int*>(SELECT_VALUE(0x1C2fE7C, 0x59BA85C));
		//native::com_fileAccessed = reinterpret_cast<int*>(SELECT_VALUE(0x1C2B328, 0x59A9A04));

		//native::threadId = reinterpret_cast<unsigned int(*)[native::THREAD_CONTEXT_COUNT]>(SELECT_VALUE(0x18576C8, 0x1D6E448));

		//native::initialized_0 = reinterpret_cast<int*>(SELECT_VALUE(0x1CE1CA0, 0x5AA3058));
		//native::sys_timeBase = reinterpret_cast<int*>(SELECT_VALUE(0x1CE1C98, 0x5AA3050));
		//native::sys_counterBase = reinterpret_cast<unsigned __int64*>(SELECT_VALUE(0x1CE1C90, 0x5AA3048));

		//native::db_hashCritSect = reinterpret_cast<native::FastCriticalSection*>(SELECT_VALUE(0xFA9E7C, 0x18596E4));

		//native::g_assetNames = reinterpret_cast<const char**>(SELECT_VALUE(0x92A688, 0x8AAB30));

		//native::command_whitelist = reinterpret_cast<const char**>(SELECT_VALUE(0x929FA0, 0x8AA3B8));
	}
}
