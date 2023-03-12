#pragma once

namespace game
{
	namespace native
	{
		typedef float vec_t;
		typedef vec_t vec2_t[2];
		typedef vec_t vec3_t[3];
		typedef vec_t vec4_t[4];

		struct GfxDrawSurfFields// Size=0x8
		{
			unsigned long unused : 1;// Offset=0x0 Size=0x8 BitOffset=0x3f BitSize=0x1
			unsigned long primarySortKey : 6;// Offset=0x0 Size=0x8 BitOffset=0x39 BitSize=0x6
			unsigned long surfType : 4;// Offset=0x0 Size=0x8 BitOffset=0x35 BitSize=0x4
			unsigned long viewModelRender : 1;// Offset=0x0 Size=0x8 BitOffset=0x34 BitSize=0x1
			unsigned long sceneLightIndex : 8;// Offset=0x0 Size=0x8 BitOffset=0x2c BitSize=0x8
			unsigned long useHeroLighting : 1;// Offset=0x0 Size=0x8 BitOffset=0x2b BitSize=0x1
			unsigned long prepass : 2;// Offset=0x0 Size=0x8 BitOffset=0x29 BitSize=0x2
			unsigned long materialSortedIndex : 12;// Offset=0x0 Size=0x8 BitOffset=0x1d BitSize=0xc
			unsigned long customIndex : 5;// Offset=0x0 Size=0x8 BitOffset=0x18 BitSize=0x5
			unsigned long hasGfxEntIndex : 1;// Offset=0x0 Size=0x8 BitOffset=0x17 BitSize=0x1
			unsigned long reflectionProbeIndex : 8;// Offset=0x0 Size=0x8 BitOffset=0xf BitSize=0x8
			unsigned long objectId : 15;// Offset=0x0 Size=0x8 BitOffset=0x0 BitSize=0xf
		};

		union GfxDrawSurf// Size=0x8
		{
			struct GfxDrawSurfFields fields;// Offset=0x0 Size=0x8
			unsigned long packed;// Offset=0x0 Size=0x8
		};

		struct MaterialInfo// Size=0x18
		{
			char* name;// Offset=0x0 Size=0x4
			unsigned char gameFlags;// Offset=0x4 Size=0x1
			unsigned char sortKey;// Offset=0x5 Size=0x1
			unsigned char textureAtlasRowCount;// Offset=0x6 Size=0x1
			unsigned char textureAtlasColumnCount;// Offset=0x7 Size=0x1
			GfxDrawSurf drawSurf;// Offset=0x8 Size=0x8
			unsigned int surfaceTypeBits;// Offset=0x10 Size=0x4
		};

		struct MaterialPass// Size=0x6c
		{
			void* vertexDecl;// Offset=0x0 Size=0x4
			void* vertexShaderArray[21];// Offset=0x4 Size=0x54
			void* vertexShader;// Offset=0x58 Size=0x4
			void* pixelShader;// Offset=0x5c Size=0x4
			unsigned char perPrimArgCount;// Offset=0x60 Size=0x1
			unsigned char perObjArgCount;// Offset=0x61 Size=0x1
			unsigned char stableArgCount;// Offset=0x62 Size=0x1
			unsigned char customSamplerFlags;// Offset=0x63 Size=0x1
			unsigned char precompiledIndex;// Offset=0x64 Size=0x1
			unsigned char __align0[3];// Offset=0x65 Size=0x3
			void* args;// Offset=0x68 Size=0x4
		};

		struct MaterialTechnique// Size=0x74
		{
			char* name;// Offset=0x0 Size=0x4
			unsigned short flags;// Offset=0x4 Size=0x2
			unsigned short passCount;// Offset=0x6 Size=0x2
			MaterialPass passArray[1];// Offset=0x8 Size=0x6c
		};

		struct MaterialTechniqueSet// Size=0xa0
		{
			char* name;// Offset=0x0 Size=0x4
			unsigned char worldVertFormat;// Offset=0x4 Size=0x1
			unsigned char unused[2];// Offset=0x5 Size=0x2
			unsigned char __align0[1];// Offset=0x7 Size=0x1
			MaterialTechniqueSet* remappedTechniqueSet;// Offset=0x8 Size=0x4
			MaterialTechnique* techniques[37];// Offset=0xc Size=0x94
		};

		struct Material// Size=0x58
		{
			MaterialInfo info;// Offset=0x0 Size=0x18
			unsigned char stateBitsEntry[37];// Offset=0x18 Size=0x25
			unsigned char textureCount;// Offset=0x3d Size=0x1
			unsigned char constantCount;// Offset=0x3e Size=0x1
			unsigned char stateBitsCount;// Offset=0x3f Size=0x1
			unsigned char stateFlags;// Offset=0x40 Size=0x1
			unsigned char cameraRegion;// Offset=0x41 Size=0x1
			unsigned char materialType;// Offset=0x42 Size=0x1
			unsigned char layerCount;// Offset=0x43 Size=0x1
			MaterialTechniqueSet* techniqueSet;// Offset=0x44 Size=0x4
			void* textureTable;// Offset=0x48 Size=0x4
			void* constantTable;// Offset=0x4c Size=0x4
			void* stateBitsTable;// Offset=0x50 Size=0x4
			char** subMaterials;// Offset=0x54 Size=0x4
		};

		struct Font_s// Size=0x18
		{
			char* fontName;// Offset=0x0 Size=0x4
			int pixelHeight;// Offset=0x4 Size=0x4
			int glyphCount;// Offset=0x8 Size=0x4
			struct Material* material;// Offset=0xc Size=0x4
			struct Material* glowMaterial;// Offset=0x10 Size=0x4
			struct Glyph* glyphs;// Offset=0x14 Size=0x4
		};

		struct ScreenPlacement// Size=0x6c
		{
			vec2_t scaleVirtualToReal;// Offset=0x0 Size=0x8
			vec2_t scaleVirtualToFull;// Offset=0x8 Size=0x8
			vec2_t scaleRealToVirtual;// Offset=0x10 Size=0x8
			vec2_t realViewportPosition;// Offset=0x18 Size=0x8
			vec2_t realViewportSize;// Offset=0x20 Size=0x8
			vec2_t virtualViewableMin;// Offset=0x28 Size=0x8
			vec2_t virtualViewableMax;// Offset=0x30 Size=0x8
			vec2_t realViewableMin;// Offset=0x38 Size=0x8
			vec2_t realViewableMax;// Offset=0x40 Size=0x8
			vec2_t virtualAdjustableMin;// Offset=0x48 Size=0x8
			vec2_t virtualAdjustableMax;// Offset=0x50 Size=0x8
			vec2_t realAdjustableMin;// Offset=0x58 Size=0x8
			vec2_t realAdjustableMax;// Offset=0x60 Size=0x8
			vec_t subScreenLeft;// Offset=0x68 Size=0x4
		};

		struct cursor_t
		{
			float x;
			float y;
		};

		enum UILocalVarType
		{
			UILOCALVAR_INT = 0x0,
			UILOCALVAR_FLOAT = 0x1,
			UILOCALVAR_STRING = 0x2,
		};

		union $B42A88463653BDCDFC5664844B4491DA
		{
			int integer;
			float value;
			const char* string;
		};

		struct UILocalVar
		{
			UILocalVarType type;
			const char* name;
			$B42A88463653BDCDFC5664844B4491DA u;
		};

		struct UILocalVarContext
		{
			UILocalVar table[256];
		};

		struct rectDef_s
		{
			float x;
			float y;
			float w;
			float h;
			int horzAlign;
			int vertAlign;
		};

		struct windowDef_t
		{
			const char* name;
			rectDef_s rect;
			rectDef_s rectClient;
			const char* group;
			int style;
			int border;
			int ownerDraw;
			int ownerDrawFlags;
			float borderSize;
			int staticFlags;
			int dynamicFlags[1];
			int nextTime;
			float foreColor[4];
			float backColor[4];
			float borderColor[4];
			float outlineColor[4];
			Material* background;
		};

		union operandInternalDataUnion
		{
			int intVal;
			float floatVal;
			const char* string;
		};

		enum expDataType
		{
			VAL_INT = 0x0,
			VAL_FLOAT = 0x1,
			VAL_STRING = 0x2,
		};

		struct Operand
		{
			expDataType dataType;
			operandInternalDataUnion internals;
		};

		union entryInternalData
		{
			int operationEnum;
			Operand operand;
		};

		struct expressionEntry
		{
			int type;
			entryInternalData data;
		};

		struct statement_s
		{
			int numEntries;
			expressionEntry** entries;
		};

		struct ItemKeyHandler
		{
			int key;
			const char* action;
			ItemKeyHandler* next;
		};

		struct columnInfo_s
		{
			int pos;
			int width;
			int maxChars;
			int alignment;
		};

		struct listBoxDef_s
		{
			int mousePos;
			int startPos[1];
			int endPos[1];
			int drawPadding;
			float elementWidth;
			float elementHeight;
			int elementStyle;
			int numColumns;
			columnInfo_s columnInfo[16];
			const char* doubleClick;
			int notselectable;
			int noScrollBars;
			int usePaging;
			float selectBorder[4];
			float disableColor[4];
			Material* selectIcon;
		};

		struct editFieldDef_s
		{
			float minVal;
			float maxVal;
			float defVal;
			float range;
			int maxChars;
			int maxCharsGotoNext;
			int maxPaintChars;
			int paintOffset;
		};

		struct multiDef_s
		{
			const char* dvarList[32];
			const char* dvarStr[32];
			float dvarValue[32];
			int count;
			int strDef;
		};

		union itemDefData_t
		{
			listBoxDef_s* listBox;
			editFieldDef_s* editField;
			multiDef_s* multi;
			const char* enumDvarName;
			void* data;
		};

		struct itemDef_s;

		struct menuDef_t
		{
			windowDef_t window;
			const char* font;
			int fullScreen;
			int itemCount;
			int fontIndex;
			int cursorItem[1];
			int fadeCycle;
			float fadeClamp;
			float fadeAmount;
			float fadeInAmount;
			float blurRadius;
			const char* onOpen;
			const char* onClose;
			const char* onESC;
			ItemKeyHandler* onKey;
			statement_s visibleExp;
			const char* allowedBinding;
			const char* soundName; //soundloop
			int imageTrack;
			float focusColor[4];
			float disableColor[4];
			statement_s rectXExp;
			statement_s rectYExp;
			itemDef_s** items;
		};

		struct itemDef_s
		{
			windowDef_t window;
			rectDef_s textRect[1];
			int type;
			int dataType;
			int alignment;
			int fontEnum;
			int textAlignMode;
			float textalignx;
			float textaligny;
			float textscale;
			int textStyle;
			int gameMsgWindowIndex;
			int gameMsgWindowMode;
			const char* text;
			int textSavegame; //original name: itemFlags
			menuDef_t* parent;
			const char* mouseEnterText;
			const char* mouseExitText;
			const char* mouseEnter;
			const char* mouseExit;
			const char* action;
			const char* onAccept;
			const char* onFocus;
			const char* leaveFocus;
			const char* dvar;
			const char* dvarTest;
			ItemKeyHandler* onKey;
			const char* enableDvar;
			int dvarFlags;
			void* focusSound;
			int feeder; //float feeder; //original name: special
			int cursorPos[1];
			itemDefData_t typeData;
			int imageTrack;
			statement_s visibleExp;
			statement_s textExp;
			statement_s materialExp;
			statement_s rectXExp;
			statement_s rectYExp;
			statement_s rectWExp;
			statement_s rectHExp;
			statement_s forecolorAExp;
		};

		struct UiContext
		{
			int localClientNum;
			float bias;
			int realTime;
			int frameTime;
			cursor_t cursor;
			int isCursorVisible;
			int screenWidth;
			int screenHeight;
			float screenAspect;
			float FPS;
			float blurRadiusOut;
			menuDef_t* Menus[640];
			int menuCount;
			menuDef_t* menuStack[16];
			int openMenuCount;
			UILocalVarContext localVars;
		};

		enum keyNum_t
		{
			K_NONE = 0x0,
			K_FIRSTGAMEPADBUTTON_RANGE_1 = 0x1,
			K_BUTTON_A = 0x1,
			K_BUTTON_B = 0x2,
			K_BUTTON_X = 0x3,
			K_BUTTON_Y = 0x4,
			K_BUTTON_LSHLDR = 0x5,
			K_BUTTON_RSHLDR = 0x6,
			K_LASTGAMEPADBUTTON_RANGE_1 = 0x6,
			K_BS = 0x8,
			K_TAB = 0x9,
			K_ENTER = 0xD,
			K_FIRSTGAMEPADBUTTON_RANGE_2 = 0xE,
			K_BUTTON_START = 0xE,
			K_BUTTON_BACK = 0xF,
			K_BUTTON_LSTICK = 0x10,
			K_BUTTON_RSTICK = 0x11,
			K_BUTTON_LTRIG = 0x12,
			K_BUTTON_RTRIG = 0x13,
			K_DPAD_UP = 0x14,
			K_FIRSTDPAD = 0x14,
			K_DPAD_DOWN = 0x15,
			K_DPAD_LEFT = 0x16,
			K_DPAD_RIGHT = 0x17,
			K_BUTTON_LSTICK_ALTIMAGE2 = 0x10,
			K_BUTTON_RSTICK_ALTIMAGE2 = 0x11,
			K_BUTTON_LSTICK_ALTIMAGE = 0xBC,
			K_BUTTON_RSTICK_ALTIMAGE = 0xBD,
			K_LASTDPAD = 0x17,
			K_LASTGAMEPADBUTTON_RANGE_2 = 0x17,
			K_ESCAPE = 0x1B,
			K_FIRSTGAMEPADBUTTON_RANGE_3 = 0x1C,
			K_APAD_UP = 0x1C,
			K_FIRSTAPAD = 0x1C,
			K_APAD_DOWN = 0x1D,
			K_APAD_LEFT = 0x1E,
			K_APAD_RIGHT = 0x1F,
			K_LASTAPAD = 0x1F,
			K_LASTGAMEPADBUTTON_RANGE_3 = 0x1F,
			K_SPACE = 0x20,
			K_GRAVE = 0x60,
			K_TILDE = 0x7E,
			K_BACKSPACE = 0x7F,
			K_ASCII_FIRST = 0x80,
			K_ASCII_181 = 0x80,
			K_ASCII_191 = 0x81,
			K_ASCII_223 = 0x82,
			K_ASCII_224 = 0x83,
			K_ASCII_225 = 0x84,
			K_ASCII_228 = 0x85,
			K_ASCII_229 = 0x86,
			K_ASCII_230 = 0x87,
			K_ASCII_231 = 0x88,
			K_ASCII_232 = 0x89,
			K_ASCII_233 = 0x8A,
			K_ASCII_236 = 0x8B,
			K_ASCII_241 = 0x8C,
			K_ASCII_242 = 0x8D,
			K_ASCII_243 = 0x8E,
			K_ASCII_246 = 0x8F,
			K_ASCII_248 = 0x90,
			K_ASCII_249 = 0x91,
			K_ASCII_250 = 0x92,
			K_ASCII_252 = 0x93,
			K_END_ASCII_CHARS = 0x94,
			K_COMMAND = 0x96,
			K_CAPSLOCK = 0x97,
			K_POWER = 0x98,
			K_PAUSE = 0x99,
			K_UPARROW = 0x9A,
			K_DOWNARROW = 0x9B,
			K_LEFTARROW = 0x9C,
			K_RIGHTARROW = 0x9D,
			K_ALT = 0x9E,
			K_CTRL = 0x9F,
			K_SHIFT = 0xA0,
			K_INS = 0xA1,
			K_DEL = 0xA2,
			K_PGDN = 0xA3,
			K_PGUP = 0xA4,
			K_HOME = 0xA5,
			K_END = 0xA6,
			K_F1 = 0xA7,
			K_F2 = 0xA8,
			K_F3 = 0xA9,
			K_F4 = 0xAA,
			K_F5 = 0xAB,
			K_F6 = 0xAC,
			K_F7 = 0xAD,
			K_F8 = 0xAE,
			K_F9 = 0xAF,
			K_F10 = 0xB0,
			K_F11 = 0xB1,
			K_F12 = 0xB2,
			K_F13 = 0xB3,
			K_F14 = 0xB4,
			K_F15 = 0xB5,
			K_KP_HOME = 0xB6,
			K_KP_UPARROW = 0xB7,
			K_KP_PGUP = 0xB8,
			K_KP_LEFTARROW = 0xB9,
			K_KP_5 = 0xBA,
			K_KP_RIGHTARROW = 0xBB,
			K_KP_END = 0xBC,
			K_KP_DOWNARROW = 0xBD,
			K_KP_PGDN = 0xBE,
			K_KP_ENTER = 0xBF,
			K_KP_INS = 0xC0,
			K_KP_DEL = 0xC1,
			K_KP_SLASH = 0xC2,
			K_KP_MINUS = 0xC3,
			K_KP_PLUS = 0xC4,
			K_KP_NUMLOCK = 0xC5,
			K_KP_STAR = 0xC6,
			K_KP_EQUALS = 0xC7,
			K_MOUSE1 = 0xC8,
			K_MOUSE2 = 0xC9,
			K_MOUSE3 = 0xCA,
			K_MOUSE4 = 0xCB,
			K_MOUSE5 = 0xCC,
			K_MWHEELDOWN = 0xCD,
			K_MWHEELUP = 0xCE,
			K_AUX1 = 0xCF,
			K_AUX2 = 0xD0,
			K_AUX3 = 0xD1,
			K_AUX4 = 0xD2,
			K_AUX5 = 0xD3,
			K_AUX6 = 0xD4,
			K_AUX7 = 0xD5,
			K_AUX8 = 0xD6,
			K_AUX9 = 0xD7,
			K_AUX10 = 0xD8,
			K_AUX11 = 0xD9,
			K_AUX12 = 0xDA,
			K_AUX13 = 0xDB,
			K_AUX14 = 0xDC,
			K_AUX15 = 0xDD,
			K_AUX16 = 0xDE,
			K_LAST_KEY = 0xDF
		};

		struct gui_menus_t
		{
			bool menustate;
			bool mouse_ignores_menustate;
			bool was_open;
			bool hk_is_clicked;
			bool hk_is_down;
			bool one_time_init;
			bool got_layout_from_menu;
			float position[2];
			float size[2];
			int horzAlign;
			int vertAlign;
		};

		struct gui_t
		{
			bool imgui_initialized;
			bool dvars_initialized;
			bool any_menus_open;
			gui_menus_t menus[4];
		};

		// MW3

		enum bdLobbyErrorCode : uint32_t
		{
			BD_NO_ERROR = 0x0,
			BD_TOO_MANY_TASKS = 0x1,
			BD_NOT_CONNECTED = 0x2,
			BD_SEND_FAILED = 0x3,
			BD_HANDLE_TASK_FAILED = 0x4,
			BD_START_TASK_FAILED = 0x5,
			BD_RESULT_EXCEEDS_BUFFER_SIZE = 0x64,
			BD_ACCESS_DENIED = 0x65,
			BD_EXCEPTION_IN_DB = 0x66,
			BD_MALFORMED_TASK_HEADER = 0x67,
			BD_INVALID_ROW = 0x68,
			BD_EMPTY_ARG_LIST = 0x69,
			BD_PARAM_PARSE_ERROR = 0x6A,
			BD_PARAM_MISMATCHED_TYPE = 0x6B,
			BD_SERVICE_NOT_AVAILABLE = 0x6C,
			BD_CONNECTION_RESET = 0x6D,
			BD_INVALID_USER_ID = 0x6E,
			BD_LOBBY_PROTOCOL_VERSION_FAILURE = 0x6F,
			BD_LOBBY_INTERNAL_FAILURE = 0x70,
			BD_LOBBY_PROTOCOL_ERROR = 0x71,
			BD_LOBBY_FAILED_TO_DECODE_UTF8 = 0x72,
			BD_LOBBY_ASCII_EXPECTED = 0x73,
			BD_ASYNCHRONOUS_ERROR = 0xC8,
			BD_STREAMING_COMPLETE = 0xC9,
			BD_MEMBER_NO_PROPOSAL = 0x12C,
			BD_TEAMNAME_ALREADY_EXISTS = 0x12D,
			BD_MAX_TEAM_MEMBERSHIPS_LIMITED = 0x12E,
			BD_MAX_TEAM_OWNERSHIPS_LIMITED = 0x12F,
			BD_NOT_A_TEAM_MEMBER = 0x130,
			BD_INVALID_TEAM_ID = 0x131,
			BD_INVALID_TEAM_NAME = 0x132,
			BD_NOT_A_TEAM_OWNER = 0x133,
			BD_NOT_AN_ADMIN_OR_OWNER = 0x134,
			BD_MEMBER_PROPOSAL_EXISTS = 0x135,
			BD_MEMBER_EXISTS = 0x136,
			BD_TEAM_FULL = 0x137,
			BD_VULGAR_TEAM_NAME = 0x138,
			BD_TEAM_USERID_BANNED = 0x139,
			BD_TEAM_EMPTY = 0x13A,
			BD_INVALID_TEAM_PROFILE_QUERY_ID = 0x13B,
			BD_TEAMNAME_TOO_SHORT = 0x13C,
			BD_UNIQUE_PROFILE_DATA_EXISTS_ALREADY = 0x13D,
			BD_INVALID_LEADERBOARD_ID = 0x190,
			BD_INVALID_STATS_SET = 0x191,
			BD_EMPTY_STATS_SET_IGNORED = 0x193,
			BD_NO_DIRECT_ACCESS_TO_ARBITRATED_LBS = 0x194,
			BD_STATS_WRITE_PERMISSION_DENIED = 0x195,
			BD_STATS_WRITE_TYPE_DATA_TYPE_MISMATCH = 0x196,
			BD_NO_STATS_FOR_USER = 0x197,
			BD_INVALID_ACCESS_TO_UNRANKED_LB = 0x198,
			BD_INVALID_EXTERNAL_TITLE_ID = 0x199,
			BD_DIFFERENT_LEADERBOARD_SCHEMAS = 0x19A,
			BD_TOO_MANY_LEADERBOARDS_REQUESTED = 0x19B,
			BD_ENTITLEMENTS_ERROR = 0x19C,
			BD_ENTITLEMENTS_INVALID_TITLEID = 0x19D,
			BD_ENTITLEMENTS_INVALID_LEADERBOARDID = 0x19E,
			BD_ENTITLEMENTS_INVALID_GET_MODE_FOR_TITLE = 0x19F,
			BD_ENTITLEMENTS_URL_CONNECTION_ERROR = 0x1A0,
			BD_ENTITLEMENTS_CONFIG_ERROR = 0x1A1,
			BD_ENTITLEMENTS_NAMED_PARENT_ERROR = 0x1A2,
			BD_ENTITLEMENTS_NAMED_KEY_ERROR = 0x1A3,
			BD_TOO_MANY_ENTITY_IDS_REQUESTED = 0x1A4,
			BD_STATS_READ_FAILED = 0x1A5,
			BD_INVALID_TITLE_ID = 0x1F4,
			BD_MESSAGING_INVALID_MAIL_ID = 0x258,
			BD_SELF_BLOCK_NOT_ALLOWED = 0x259,
			BD_GLOBAL_MESSAGE_ACCESS_DENIED = 0x25A,
			BD_GLOBAL_MESSAGES_USER_LIMIT_EXCEEDED = 0x25B,
			BD_MESSAGING_SENDER_DOES_NOT_EXIST = 0x25C,
			BD_AUTH_NO_ERROR = 0x2BC,
			BD_AUTH_BAD_REQUEST = 0x2BD,
			BD_AUTH_SERVER_CONFIG_ERROR = 0x2BE,
			BD_AUTH_BAD_TITLE_ID = 0x2BF,
			BD_AUTH_BAD_ACCOUNT = 0x2C0,
			BD_AUTH_ILLEGAL_OPERATION = 0x2C1,
			BD_AUTH_INCORRECT_LICENSE_CODE = 0x2C2,
			BD_AUTH_CREATE_USERNAME_EXISTS = 0x2C3,
			BD_AUTH_CREATE_USERNAME_ILLEGAL = 0x2C4,
			BD_AUTH_CREATE_USERNAME_VULGAR = 0x2C5,
			BD_AUTH_CREATE_MAX_ACC_EXCEEDED = 0x2C6,
			BD_AUTH_MIGRATE_NOT_SUPPORTED = 0x2C7,
			BD_AUTH_TITLE_DISABLED = 0x2C8,
			BD_AUTH_ACCOUNT_EXPIRED = 0x2C9,
			BD_AUTH_ACCOUNT_LOCKED = 0x2CA,
			BD_AUTH_UNKNOWN_ERROR = 0x2CB,
			BD_AUTH_INCORRECT_PASSWORD = 0x2CC,
			BD_AUTH_IP_NOT_IN_ALLOWED_RANGE = 0x2CD,
			BD_AUTH_WII_TOKEN_VERIFICATION_FAILED = 0x2CE,
			BD_AUTH_WII_AUTHENTICATION_FAILED = 0x2CF,
			BD_AUTH_IP_KEY_LIMIT_REACHED = 0x2D0,
			BD_AUTH_INVALID_GSPID = 0x2D1,
			BD_AUTH_INVALID_IP_RANGE_ID = 0x2D2,
			BD_AUTH_3DS_TOKEN_VERIFICATION_FAILED = 0x2D1,
			BD_AUTH_3DS_AUTHENTICATION_FAILED = 0x2D2,
			BD_AUTH_STEAM_APP_ID_MISMATCH = 0x2D3,
			BD_AUTH_ABACCOUNTS_APP_ID_MISMATCH = 0x2D4,
			BD_AUTH_CODO_USERNAME_NOT_SET = 0x2D5,
			BD_AUTH_WIIU_TOKEN_VERIFICATION_FAILED = 0x2D6,
			BD_AUTH_WIIU_AUTHENTICATION_FAILED = 0x2D7,
			BD_AUTH_CODO_USERNAME_NOT_BASE64 = 0x2D8,
			BD_AUTH_CODO_USERNAME_NOT_UTF8 = 0x2D9,
			BD_AUTH_TENCENT_TICKET_EXPIRED = 0x2DA,
			BD_AUTH_PS3_SERVICE_ID_MISMATCH = 0x2DB,
			BD_AUTH_CODOID_NOT_WHITELISTED = 0x2DC,
			BD_AUTH_PLATFORM_TOKEN_ERROR = 0x2DD,
			BD_AUTH_JSON_FORMAT_ERROR = 0x2DE,
			BD_AUTH_REPLY_CONTENT_ERROR = 0x2DF,
			BD_AUTH_THIRD_PARTY_TOKEN_EXPIRED = 0x2E0,
			BD_AUTH_CONTINUING = 0x2E1,
			BD_AUTH_PLATFORM_DEVICE_ID_ERROR = 0x2E4,
			BD_NO_PROFILE_INFO_EXISTS = 0x320,
			BD_FRIENDSHIP_NOT_REQUSTED = 0x384,
			BD_NOT_A_FRIEND = 0x385,
			BD_SELF_FRIENDSHIP_NOT_ALLOWED = 0x387,
			BD_FRIENDSHIP_EXISTS = 0x388,
			BD_PENDING_FRIENDSHIP_EXISTS = 0x389,
			BD_USERID_BANNED = 0x38A,
			BD_FRIENDS_FULL = 0x38C,
			BD_FRIENDS_NO_RICH_PRESENCE = 0x38D,
			BD_RICH_PRESENCE_TOO_LARGE = 0x38E,
			BD_NO_FILE = 0x3E8,
			BD_PERMISSION_DENIED = 0x3E9,
			BD_FILESIZE_LIMIT_EXCEEDED = 0x3EA,
			BD_FILENAME_MAX_LENGTH_EXCEEDED = 0x3EB,
			BD_EXTERNAL_STORAGE_SERVICE_ERROR = 0x3EC,
			BD_CHANNEL_DOES_NOT_EXIST = 0x44D,
			BD_CHANNEL_ALREADY_SUBSCRIBED = 0x44E,
			BD_CHANNEL_NOT_SUBSCRIBED = 0x44F,
			BD_CHANNEL_FULL = 0x450,
			BD_CHANNEL_SUBSCRIPTIONS_FULL = 0x451,
			BD_CHANNEL_NO_SELF_WHISPERING = 0x452,
			BD_CHANNEL_ADMIN_REQUIRED = 0x453,
			BD_CHANNEL_TARGET_NOT_SUBSCRIBED = 0x454,
			BD_CHANNEL_REQUIRES_PASSWORD = 0x455,
			BD_CHANNEL_TARGET_IS_SELF = 0x456,
			BD_CHANNEL_PUBLIC_BAN_NOT_ALLOWED = 0x457,
			BD_CHANNEL_USER_BANNED = 0x458,
			BD_CHANNEL_PUBLIC_PASSWORD_NOT_ALLOWED = 0x459,
			BD_CHANNEL_PUBLIC_KICK_NOT_ALLOWED = 0x45A,
			BD_CHANNEL_MUTED = 0x45B,
			BD_EVENT_DESC_TRUNCATED = 0x4B0,
			BD_CONTENT_UNLOCK_UNKNOWN_ERROR = 0x514,
			BD_UNLOCK_KEY_INVALID = 0x515,
			BD_UNLOCK_KEY_ALREADY_USED_UP = 0x516,
			BD_SHARED_UNLOCK_LIMIT_REACHED = 0x517,
			BD_DIFFERENT_HARDWARE_ID = 0x518,
			BD_INVALID_CONTENT_OWNER = 0x519,
			BD_CONTENT_UNLOCK_INVALID_USER = 0x51A,
			BD_CONTENT_UNLOCK_INVALID_CATEGORY = 0x51B,
			BD_KEY_ARCHIVE_INVALID_WRITE_TYPE = 0x5DC,
			BD_KEY_ARCHIVE_EXCEEDED_MAX_IDS_PER_REQUEST = 0x5DD,
			BD_BANDWIDTH_TEST_TRY_AGAIN = 0x712,
			BD_BANDWIDTH_TEST_STILL_IN_PROGRESS = 0x713,
			BD_BANDWIDTH_TEST_NOT_PROGRESS = 0x714,
			BD_BANDWIDTH_TEST_SOCKET_ERROR = 0x715,
			BD_INVALID_SESSION_NONCE = 0x76D,
			BD_ARBITRATION_FAILURE = 0x76F,
			BD_ARBITRATION_USER_NOT_REGISTERED = 0x771,
			BD_ARBITRATION_NOT_CONFIGURED = 0x772,
			BD_CONTENTSTREAMING_FILE_NOT_AVAILABLE = 0x7D0,
			BD_CONTENTSTREAMING_STORAGE_SPACE_EXCEEDED = 0x7D1,
			BD_CONTENTSTREAMING_NUM_FILES_EXCEEDED = 0x7D2,
			BD_CONTENTSTREAMING_UPLOAD_BANDWIDTH_EXCEEDED = 0x7D3,
			BD_CONTENTSTREAMING_FILENAME_MAX_LENGTH_EXCEEDED = 0x7D4,
			BD_CONTENTSTREAMING_MAX_THUMB_DATA_SIZE_EXCEEDED = 0x7D5,
			BD_CONTENTSTREAMING_DOWNLOAD_BANDWIDTH_EXCEEDED = 0x7D6,
			BD_CONTENTSTREAMING_NOT_ENOUGH_DOWNLOAD_BUFFER_SPACE = 0x7D7,
			BD_CONTENTSTREAMING_SERVER_NOT_CONFIGURED = 0x7D8,
			BD_CONTENTSTREAMING_INVALID_APPLE_RECEIPT = 0x7DA,
			BD_CONTENTSTREAMING_APPLE_STORE_NOT_AVAILABLE = 0x7DB,
			BD_CONTENTSTREAMING_APPLE_RECEIPT_FILENAME_MISMATCH = 0x7DC,
			BD_CONTENTSTREAMING_HTTP_ERROR = 0x7E4,
			BD_CONTENTSTREAMING_FAILED_TO_START_HTTP = 0x7E5,
			BD_CONTENTSTREAMING_LOCALE_INVALID = 0x7E6,
			BD_CONTENTSTREAMING_LOCALE_MISSING = 0x7E7,
			BD_VOTERANK_ERROR_EMPTY_RATING_SUBMISSION = 0x7EE,
			BD_VOTERANK_ERROR_MAX_VOTES_EXCEEDED = 0x7EF,
			BD_VOTERANK_ERROR_INVALID_RATING = 0x7F0,
			BD_MAX_NUM_TAGS_EXCEEDED = 0x82A,
			BD_TAGGED_COLLECTION_DOES_NOT_EXIST = 0x82B,
			BD_EMPTY_TAG_ARRAY = 0x82C,
			BD_INVALID_QUERY_ID = 0x834,
			BD_NO_ENTRY_TO_UPDATE = 0x835,
			BD_SESSION_INVITE_EXISTS = 0x836,
			BD_INVALID_SESSION_ID = 0x837,
			BD_ATTACHMENT_TOO_LARGE = 0x838,
			BD_INVALID_GROUP_ID = 0xAF0,
			BD_MAIL_INVALID_MAIL_ID_ERROR = 0xB55,
			BD_UCD_SERVICE_ERROR = 0xC80,
			BD_UCD_SERVICE_DISABLED = 0xC81,
			BD_UCD_UNINTIALIZED_ERROR = 0xC82,
			BD_UCD_ACCOUNT_ALREADY_REGISTERED = 0xC83,
			BD_UCD_ACCOUNT_NOT_REGISTERED = 0xC84,
			BD_UCD_AUTH_ATTEMPT_FAILED = 0xC85,
			BD_UCD_ACCOUNT_LINKING_ERROR = 0xC86,
			BD_UCD_ENCRYPTION_ERROR = 0xC87,
			BD_UCD_ACCOUNT_DATA_INVALID = 0xC88,
			BD_UCD_ACCOUNT_DATA_INVALID_FIRSTNAME = 0xC89,
			BD_UCD_ACCOUNT_DATA_INVALID_LASTNAME = 0xC8A,
			BD_UCD_ACCOUNT_DATA_INVALID_DOB = 0xC8B,
			BD_UCD_ACCOUNT_DATA_INVALID_EMAIL = 0xC8C,
			BD_UCD_ACCOUNT_DATA_INVALID_COUNTRY = 0xC8D,
			BD_UCD_ACCOUNT_DATA_INVALID_POSTCODE = 0xC8E,
			BD_UCD_ACCOUNT_DATA_INVALID_PASSWORD = 0xC8F,
			BD_UCD_ACCOUNT_NAME_ALREADY_RESISTERED = 0xC94,
			BD_UCD_ACCOUNT_EMAIL_ALREADY_RESISTERED = 0xC95,
			BD_UCD_GUEST_ACCOUNT_AUTH_CONFLICT = 0xC96,
			BD_TWITCH_SERVICE_ERROR = 0xC1D,
			BD_TWITCH_ACCOUNT_ALREADY_LINKED = 0xC1E,
			BD_TWITCH_NO_LINKED_ACCOUNT = 0xC1F,
			BD_YOUTUBE_SERVICE_ERROR = 0xCE5,
			BD_YOUTUBE_SERVICE_COMMUNICATION_ERROR = 0xCE6,
			BD_YOUTUBE_USER_DENIED_AUTHORIZATION = 0xCE7,
			BD_YOUTUBE_AUTH_MAX_TIME_EXCEEDED = 0xCE8,
			BD_YOUTUBE_USER_UNAUTHORIZED = 0xCE9,
			BD_YOUTUBE_UPLOAD_MAX_TIME_EXCEEDED = 0xCEA,
			BD_YOUTUBE_DUPLICATE_UPLOAD = 0xCEB,
			BD_YOUTUBE_FAILED_UPLOAD = 0xCEC,
			BD_YOUTUBE_ACCOUNT_ALREADY_REGISTERED = 0xCED,
			BD_YOUTUBE_ACCOUNT_NOT_REGISTERED = 0xCEE,
			BD_YOUTUBE_CONTENT_SERVER_ERROR = 0xCEF,
			BD_YOUTUBE_UPLOAD_DOES_NOT_EXIST = 0xCF0,
			BD_YOUTUBE_NO_LINKED_ACCOUNT = 0xCF1,
			BD_YOUTUBE_DEVELOPER_TAGS_INVALID = 0xCF2,
			BD_TWITTER_AUTH_ATTEMPT_FAILED = 0xDAD,
			BD_TWITTER_AUTH_TOKEN_INVALID = 0xDAE,
			BD_TWITTER_UPDATE_LIMIT_REACHED = 0xDAF,
			BD_TWITTER_UNAVAILABLE = 0xDB0,
			BD_TWITTER_ERROR = 0xDB1,
			BD_TWITTER_TIMED_OUT = 0xDB2,
			BD_TWITTER_DISABLED_FOR_USER = 0xDB3,
			BD_TWITTER_ACCOUNT_AMBIGUOUS = 0xDB4,
			BD_TWITTER_MAXIMUM_ACCOUNTS_REACHED = 0xDB5,
			BD_TWITTER_ACCOUNT_NOT_REGISTERED = 0xDB6,
			BD_TWITTER_DUPLICATE_STATUS = 0xDB7,
			BD_TWITTER_ACCOUNT_ALREADY_REGISTERED = 0xE1C,
			BD_FACEBOOK_AUTH_ATTEMPT_FAILED = 0xE11,
			BD_FACEBOOK_AUTH_TOKEN_INVALID = 0xE12,
			BD_FACEBOOK_PHOTO_DOES_NOT_EXIST = 0xE13,
			BD_FACEBOOK_PHOTO_INVALID = 0xE14,
			BD_FACEBOOK_PHOTO_ALBUM_FULL = 0xE15,
			BD_FACEBOOK_UNAVAILABLE = 0xE16,
			BD_FACEBOOK_ERROR = 0xE17,
			BD_FACEBOOK_TIMED_OUT = 0xE18,
			BD_FACEBOOK_DISABLED_FOR_USER = 0xE19,
			BD_FACEBOOK_ACCOUNT_AMBIGUOUS = 0xE1A,
			BD_FACEBOOK_MAXIMUM_ACCOUNTS_REACHED = 0xE1B,
			BD_FACEBOOK_INVALID_NUM_PICTURES_REQUESTED = 0xE1C,
			BD_FACEBOOK_VIDEO_DOES_NOT_EXIST = 0xE1D,
			BD_FACEBOOK_ACCOUNT_ALREADY_REGISTERED = 0xE1E,
			BD_APNS_INVALID_PAYLOAD = 0xE74,
			BD_APNS_INVALID_TOKEN_LENGTH_ERROR = 0xE76,
			BD_MAX_CONSOLEID_LENGTH_EXCEEDED = 0xEE1,
			BD_MAX_WHITELIST_LENGTH_EXCEEDED = 0xEE2,
			BD_USERGROUP_NAME_ALREADY_EXISTS = 0x1770,
			BD_INVALID_USERGROUP_ID = 0x1771,
			BD_USER_ALREADY_IN_USERGROUP = 0x1772,
			BD_USER_NOT_IN_USERGROUP = 0x1773,
			BD_INVALID_USERGROUP_MEMBER_TYPE = 0x1774,
			BD_TOO_MANY_MEMBERS_REQUESTED = 0x1775,
			BD_USERGROUP_NAME_TOO_SHORT = 0x1776,
			BD_RICH_PRESENCE_DATA_TOO_LARGE = 0x1A90,
			BD_RICH_PRESENCE_TOO_MANY_USERS = 0x1A91,
			BD_PRESENCE_DATA_TOO_LARGE = 0x283C,
			BD_PRESENCE_TOO_MANY_USERS = 0x283D,
			BD_USER_LOGGED_IN_OTHER_TITLE = 0x283E,
			BD_USER_NOT_LOGGED_IN = 0x283F,
			BD_SUBSCRIPTION_TOO_MANY_USERS = 0x1B58,
			BD_SUBSCRIPTION_TICKET_PARSE_ERROR = 0x1B59,
			BD_CODO_ID_INVALID_DATA = 0x1BBC,
			BD_INVALID_MESSAGE_FORMAT = 0x1BBD,
			BD_TLOG_TOO_MANY_MESSAGES = 0x1BBE,
			BD_CODO_ID_NOT_IN_WHITELIST = 0x1BBF,
			BD_TLOG_MESSAGE_TRANSFORMATION_ERROR = 0x1BC0,
			BD_REWARDS_NOT_ENABLED = 0x1BC1,
			BD_MARKETPLACE_ERROR = 0x1F40,
			BD_MARKETPLACE_RESOURCE_NOT_FOUND = 0x1F41,
			BD_MARKETPLACE_INVALID_CURRENCY = 0x1F42,
			BD_MARKETPLACE_INVALID_PARAMETER = 0x1F43,
			BD_MARKETPLACE_RESOURCE_CONFLICT = 0x1F44,
			BD_MARKETPLACE_STORAGE_ERROR = 0x1F45,
			BD_MARKETPLACE_INTEGRITY_ERROR = 0x1F46,
			BD_MARKETPLACE_INSUFFICIENT_FUNDS_ERROR = 0x1F47,
			BD_MARKETPLACE_MMP_SERVICE_ERROR = 0x1F48,
			BD_MARKETPLACE_PRECONDITION_REQUIRED = 0x1F49,
			BD_MARKETPLACE_ITEM_MULTIPLE_PURCHASE_ERROR = 0x1F4A,
			BD_MARKETPLACE_MISSING_REQUIRED_ENTITLEMENT = 0x1F4B,
			BD_MARKETPLACE_VALIDATION_ERROR = 0x1F4C,
			BD_MARKETPLACE_TENCENT_PAYMENT_ERROR = 0x1F4D,
			BD_MARKETPLACE_SKU_NOT_COUPON_ENABLED_ERROR = 0x1F4E,
			BD_LEAGUE_INVALID_TEAM_SIZE = 0x1FA4,
			BD_LEAGUE_INVALID_TEAM = 0x1FA5,
			BD_LEAGUE_INVALID_SUBDIVISION = 0x1FA6,
			BD_LEAGUE_INVALID_LEAGUE = 0x1FA7,
			BD_LEAGUE_TOO_MANY_RESULTS_REQUESTED = 0x1FA8,
			BD_LEAGUE_METADATA_TOO_LARGE = 0x1FA9,
			BD_LEAGUE_TEAM_ICON_TOO_LARGE = 0x1FAA,
			BD_LEAGUE_TEAM_NAME_TOO_LONG = 0x1FAB,
			BD_LEAGUE_ARRAY_SIZE_MISMATCH = 0x1FAC,
			BD_LEAGUE_SUBDIVISION_MISMATCH = 0x2008,
			BD_LEAGUE_INVALID_WRITE_TYPE = 0x2009,
			BD_LEAGUE_INVALID_STATS_DATA = 0x200A,
			BD_LEAGUE_SUBDIVISION_UNRANKED = 0x200B,
			BD_LEAGUE_CROSS_TEAM_STATS_WRITE_PREVENTED = 0x200C,
			BD_LEAGUE_INVALID_STATS_SEASON = 0x200D,
			BD_COMMERCE_ERROR = 0x206C,
			BD_COMMERCE_RESOURCE_NOT_FOUND = 0x206D,
			BD_COMMERCE_STORAGE_INVALID_PARAMETER = 0x206E,
			BD_COMMERCE_APPLICATION_INVALID_PARAMETER = 0x206F,
			BD_COMMERCE_RESOURCE_CONFLICT = 0x2070,
			BD_COMMERCE_STORAGE_ERROR = 0x2071,
			BD_COMMERCE_INTEGRITY_ERROR = 0x2072,
			BD_COMMERCE_MMP_SERVICE_ERROR = 0x2073,
			BD_COMMERCE_PERMISSION_DENIED = 0x2074,
			BD_COMMERCE_INSUFFICIENT_FUNDS_ERROR = 0x2075,
			BD_COMMERCE_UNKNOWN_CURRENCY = 0x2076,
			BD_COMMERCE_INVALID_RECEIPT = 0x2077,
			BD_COMMERCE_RECEIPT_USED = 0x2078,
			BD_COMMERCE_TRANSACTION_ALREADY_APPLIED = 0x2079,
			BD_COMMERCE_INVALID_CURRENCY_TYPE = 0x207A,
			BD_CONNECTION_COUNTER_ERROR = 0x20D0,
			BD_LINKED_ACCOUNTS_INVALID_CONTEXT = 0x2198,
			BD_LINKED_ACCOUNTS_INVALID_PLATFORM = 0x2199,
			BD_LINKED_ACCOUNTS_LINKED_ACCOUNTS_FETCH_ERROR = 0x219A,
			BD_LINKED_ACCOUNTS_INVALID_ACCOUNT = 0x219B,
			BD_GMSG_INVALID_CATEGORY_ID = 0x27D8,
			BD_GMSG_CATEGORY_MEMBERSHIPS_LIMIT = 0x27D9,
			BD_GMSG_NONMEMBER_POST_DISALLOWED = 0x27DA,
			BD_GMSG_CATEGORY_DISALLOWS_CLIENT_TYPE = 0x27DB,
			BD_GMSG_PAYLOAD_TOO_BIG = 0x27DC,
			BD_GMSG_MEMBER_POST_DISALLOWED = 0x27DD,
			BD_GMSG_OVERLOADED = 0x27DE,
			BD_GMSG_USER_PERCATEGORY_POST_RATE_EXCEEDED = 0x27DF,
			BD_GMSG_USER_GLOBAL_POST_RATE_EXCEEDED = 0x27E0,
			BD_GMSG_GROUP_POST_RATE_EXCEEDED = 0x27E1,
			BD_MAX_ERROR_CODE = 0x27E2,
		};

		enum bdNATType : uint8_t
		{
			BD_NAT_UNKNOWN = 0x0,
			BD_NAT_OPEN = 0x1,
			BD_NAT_MODERATE = 0x2,
			BD_NAT_STRICT = 0x3,
		};

		enum
		{
			THREAD_CONTEXT_MAIN = 0x0,
			THREAD_CONTEXT_BACKEND = 0x1,
			THREAD_CONTEXT_WORKER0 = 0x2,
			THREAD_CONTEXT_WORKER1 = 0x3,
			THREAD_CONTEXT_WORKER2 = 0x4,
			THREAD_CONTEXT_WORKER3 = 0x5,
			THREAD_CONTEXT_WORKER4 = 0x6,
			THREAD_CONTEXT_WORKER5 = 0x7,
			THREAD_CONTEXT_WORKER6 = 0x8,
			THREAD_CONTEXT_WORKER7 = 0x9,
			THREAD_CONTEXT_SERVER = 0xA,
			THREAD_CONTEXT_TRACE_COUNT = 0xB,
			THREAD_CONTEXT_TRACE_LAST = 0xA,
			THREAD_CONTEXT_CINEMATIC = 0xB,
			THREAD_CONTEXT_DATABASE = 0xC,
			THREAD_CONTEXT_STREAM = 0xD,
			THREAD_CONTEXT_SNDSTREAMPACKETCALLBACK = 0xE,
			THREAD_CONTEXT_STATS_WRITE = 0xF,
			THREAD_CONTEXT_COUNT = 0x10,
		};

#pragma pack(push, 1)
		struct bdAuthTicket
		{
			unsigned int m_magicNumber;
			char m_type;
			unsigned int m_titleID;
			unsigned int m_timeIssued;
			unsigned int m_timeExpires;
			unsigned __int64 m_licenseID;
			unsigned __int64 m_userID;
			char m_username[64];
			char m_sessionKey[24];
			char m_usingHashMagicNumber[3];
			char m_hash[4];
		};
#pragma pack(pop)

		enum XAssetType
		{
			ASSET_TYPE_SCRIPTFILE = 0x27,
			ASSET_TYPE_COUNT = 0x2E,
		};

		struct ScriptFile
		{
			const char* name;
			int compressedLen;
			int len;
			int bytecodeLen;
			const char* buffer;
			unsigned char* bytecode;
		};

		struct RawFile
		{
			const char* name;
			int compressedLen;
			int len;
			const char* buffer;
		};

		union XAssetHeader
		{
			void* data;
			ScriptFile* scriptfile;
			RawFile* rawfile;
		};

		struct XAsset
		{
			int type;
			XAssetHeader header;
		};

		struct XAssetEntry
		{
			XAsset asset;
			char zoneIndex;
			volatile char inuseMask;
			bool printedMissingAsset;
			unsigned __int16 nextHash;
			unsigned __int16 nextOverride;
		};

		struct TempPriority
		{
			void* threadHandle;
			int oldPriority;
		};

		struct FastCriticalSection
		{
			volatile long readCount;
			volatile long writeCount;
			TempPriority tempPriority;
		};

		enum errorParm_t
		{
			ERR_FATAL = 0x0,
			ERR_DROP = 0x1,
			ERR_SERVERDISCONNECT = 0x2,
			ERR_DISCONNECT = 0x3,
			ERR_SCRIPT = 0x4,
			ERR_SCRIPT_DROP = 0x5,
			ERR_LOCALIZATION = 0x6,
			ERR_MAPLOADERRORSUMMARY = 0x7
		};

		enum svscmd_type
		{
			SV_CMD_CAN_IGNORE,
			SV_CMD_RELIABLE,
		};

		enum LocalClientNum_t
		{
			LOCAL_CLIENT_0 = 0,
			LOCAL_CLIENT_1 = 1,
			LOCAL_CLIENT_2 = 2,
			LOCAL_CLIENT_3 = 3,
			LOCAL_CLIENT_LAST = 3,
			LOCAL_CLIENT_COUNT = 4,
		};

		enum fsMode_t
		{
			FS_READ,
			FS_WRITE,
			FS_APPEND,
			FS_APPEND_SYNC,
		};

		enum msgLocErrType_t
		{
			LOCMSG_SAFE,
			LOCMSG_NOERR,
		};

		enum PMem_Source
		{
			PMEM_SOURCE_EXTERNAL,
			PMEM_SOURCE_DATABASE,
			PMEM_SOURCE_MOVIE,
			PMEM_SOURCE_SCRIPT,
		};

		enum FsListBehavior_e
		{
			FS_LIST_PURE_ONLY,
			FS_LIST_ALL,
		};

		struct cmd_function_s
		{
			cmd_function_s* next;
			const char* name;
			const char* autoCompleteDir;
			const char* autoCompleteExt;
			void (__cdecl *function)();
			int flags;
		};

		struct CmdArgs
		{
			int nesting;
			int localClientNum[8];
			int controllerIndex[8];
			int argc[8];
			const char** argv[8];
		};

		struct msg_t
		{
			int overflowed;
			int readOnly;
			char* data;
			char* splitData;
			int maxsize;
			int cursize;
			int splitSize;
			int readcount;
			int bit;
			int lastEntityRef;
		};

		struct XZoneInfo
		{
			const char* name;
			int allocFlags;
			int freeFlags;
		};

		struct scr_entref_t
		{
			unsigned __int16 entnum;
			unsigned __int16 classnum;
		};

		typedef void(*BuiltinFunction)();
		typedef void(*BuiltinMethod)(scr_entref_t);

		typedef unsigned __int16 scr_string_t;

		enum
		{
			VAR_UNDEFINED = 0x0,
			VAR_BEGIN_REF = 0x1,
			VAR_POINTER = 0x1,
			VAR_STRING = 0x2,
			VAR_ISTRING = 0x3,
			VAR_VECTOR = 0x4,
			VAR_END_REF = 0x5,
			VAR_FLOAT = 0x5,
			VAR_INTEGER = 0x6,
			VAR_CODEPOS = 0x7,
			VAR_PRECODEPOS = 0x8,
			VAR_FUNCTION = 0x9,
			VAR_BUILTIN_FUNCTION = 0xA,
			VAR_BUILTIN_METHOD = 0xB,
			VAR_STACK = 0xC,
			VAR_ANIMATION = 0xD,
			VAR_PRE_ANIMATION = 0xE,
			VAR_THREAD = 0xF,
			VAR_NOTIFY_THREAD = 0x10,
			VAR_TIME_THREAD = 0x11,
			VAR_CHILD_THREAD = 0x12,
			VAR_OBJECT = 0x13,
			VAR_DEAD_ENTITY = 0x14,
			VAR_ENTITY = 0x15,
			VAR_ARRAY = 0x16,
			VAR_DEAD_THREAD = 0x17,
			VAR_COUNT = 0x18,
			VAR_FREE = 0x18,
			VAR_THREAD_LIST = 0x19,
			VAR_ENDON_LIST = 0x1A,
			VAR_TOTAL_COUNT = 0x1B,
		};

		struct VariableStackBuffer
		{
			const char* pos;
			unsigned __int16 size;
			unsigned __int16 bufLen;
			unsigned __int16 localId;
			char time;
			char buf[1];
		};

		union VariableUnion
		{
			int intValue;
			float floatValue;
			unsigned int stringValue;
			const float* vectorValue;
			const char* codePosValue;
			unsigned int pointerValue;
			VariableStackBuffer* stackValue;
			unsigned int entityId;
		};

		struct VariableValue
		{
			VariableUnion u;
			int type;
		};

		struct function_stack_t
		{
			const char* pos;
			unsigned int localId;
			unsigned int localVarCount;
			VariableValue* top;
			VariableValue* startTop;
		};

		struct function_frame_t
		{
			function_stack_t fs;
			int topType;
		};

		struct scrVmPub_t
		{
			unsigned int* localVars;
			VariableValue* maxStack;
			int function_count;
			function_frame_t* function_frame;
			VariableValue* top;
			unsigned int inparamcount;
			unsigned int outparamcount;
			unsigned int breakpointOutparamcount;
			bool showError;
			function_frame_t function_frame_start[32];
			VariableValue stack[2048];
		};

		struct HunkUser
		{
			HunkUser* current;
			HunkUser* next;
			int maxSize;
			int end;
			int pos;
			const char* name;
			bool fixed;
			int type;
			unsigned char buf[1];
		};

		static_assert(sizeof(HunkUser) == 0x24);

		struct scrVarPub_t
		{
			const char* fieldBuffer;
			bool evaluate;
			unsigned int time;
			unsigned int timeArrayId;
			unsigned int pauseArrayId;
			unsigned int notifyArrayId;
			unsigned int objectStackId;
			unsigned int levelId;
			unsigned int gameId;
			unsigned int animId;
			unsigned int freeEntList;
			unsigned int tempVariable;
			unsigned int numScriptValues[2];
			bool bInited;
			bool abort;
			unsigned __int16 savecount;
			unsigned __int16 savecountMark;
			unsigned int entId;
			unsigned int entFieldName;
			unsigned int checksum;
			HunkUser* programHunkUser;
			HunkUser* canonicalStringHunkUser;
			const char* programBuffer;
			unsigned __int16 saveIdMap[36864];
			unsigned __int16 saveIdMapRev[36864];
		};

		static_assert(sizeof(scrVarPub_t) == 0x24058);

		struct scr_classStruct_t
		{
			unsigned __int16 id;
			unsigned __int16 entArrayId;
			char charId;
			const char* name;
		};

		enum dvar_flags : std::uint16_t
		{
			none = 0x0,
			saved = 0x1,
			user_info = 0x2, // sent to server on connect or change
			server_info = 0x4, // sent in response to front end requests
			replicated = 0x8,
			write_protected = 0x10,
			latched = 0x20,
			read_only = 0x40,
			cheat_protected = 0x80,
			temp = 0x100,
			no_restart = 0x400, // do not clear when a cvar_restart is issued
			user_created = 0x4000, // created by a set command
		};

		enum dvar_type : std::int8_t
		{
			DVAR_TYPE_BOOL = 0x0,
			DVAR_TYPE_FLOAT = 0x1,
			DVAR_TYPE_FLOAT_2 = 0x2,
			DVAR_TYPE_FLOAT_3 = 0x3,
			DVAR_TYPE_FLOAT_4 = 0x4,
			DVAR_TYPE_INT = 0x5,
			DVAR_TYPE_ENUM = 0x6,
			DVAR_TYPE_STRING = 0x7,
			DVAR_TYPE_COLOR = 0x8,
			DVAR_TYPE_COUNT = 0x9,
		};

		union DvarValue
		{
			bool enabled;
			int integer;
			unsigned int unsignedInt;
			float value;
			float vector[4];
			const char* string;
			char color[4];
		};

		static_assert(sizeof(DvarValue) == 0x10);

		struct enum_limit
		{
			int stringCount;
			const char** strings;
		};

		struct int_limit
		{
			int min;
			int max;
		};

		struct float_limit
		{
			float min;
			float max;
		};

		union DvarLimits
		{
			enum_limit enumeration;
			int_limit integer;
			float_limit value;
			float_limit vector;
		};

		enum DvarFlags : std::uint32_t
		{
			DVAR_FLAG_NONE = 0,
			DVAR_FLAG_SAVED = 0x1,
			DVAR_FLAG_LATCHED = 0x2,
			DVAR_FLAG_CHEAT = 0x4,
			DVAR_FLAG_REPLICATED = 0x8,
			DVAR_FLAG_WRITE = 0x800,
			DVAR_FLAG_READ = 0x2000,
		};

		struct dvar_t
		{
			const char* name;
			unsigned int flags;
			char type;
			bool modified;
			DvarValue current;
			DvarValue latched;
			DvarValue reset;
			DvarLimits domain;
			bool (*domainFunc)(dvar_t*, DvarValue);
			dvar_t* hashNext;
		};

		struct DvarDumpInfo
		{
			int count;
			int channel;
			const char* match;
		};

		struct Bounds
		{
			float midPoint[3];
			float halfSize[3];
		};

		enum TraceHitType
		{
			TRACE_HITTYPE_NONE = 0x0,
			TRACE_HITTYPE_ENTITY = 0x1,
			TRACE_HITTYPE_DYNENT_MODEL = 0x2,
			TRACE_HITTYPE_DYNENT_BRUSH = 0x3,
			TRACE_HITTYPE_GLASS = 0x4,
		};

		struct trace_t
		{
			float fraction;
			float normal[3];
			int surfaceFlags;
			int contents;
			const char* material;
			TraceHitType hitType;
			unsigned __int16 hitId;
			unsigned __int16 modelIndex;
			unsigned __int16 partName;
			unsigned __int16 partGroup;
			bool allsolid;
			bool startsolid;
			bool walkable;
		};

		static_assert(sizeof(trace_t) == 0x2C);

		struct pml_t
		{
			float forward[3];
			float right[3];
			float up[3];
			float frametime;
			int msec;
			int walking;
			int groundPlane;
			int almostGroundPlane;
			trace_t groundTrace;
			float impactSpeed;
			float previous_origin[3];
			float previous_velocity[3];
			unsigned int holdrand;
		};

		static_assert(sizeof(pml_t) == 0x84);

		struct usercmd_s
		{
			int serverTime;
			int buttons;
			int angles[3];
			unsigned int weapon;
			unsigned int offHand;
			char forwardmove;
			char rightmove;
			unsigned __int16 airburstMarkDistance;
			unsigned __int16 meleeChargeEnt;
			unsigned char meleeChargeDist;
			char selectedLoc[2];
			unsigned char selectedLocAngle;
			char remoteControlAngles[2];
			char remoteControlMove[3];
		};

		static_assert(sizeof(usercmd_s) == 0x2C);

		enum PlayerHandIndex
		{
			WEAPON_HAND_RIGHT = 0,
			WEAPON_HAND_LEFT = 1,
			NUM_WEAPON_HANDS = 2,
			WEAPON_HAND_DEFAULT = 0,
		};

		struct Weapon_s
		{
			unsigned int weaponIdx : 8;
			unsigned int weaponVariation : 4;
			unsigned int weaponScopes : 3;
			unsigned int weaponUnderBarrels : 2;
			unsigned int weaponOthers : 4;
			unsigned int scopeVariation : 3;
			unsigned int padding : 8;
		};

		union Weapon
		{
			Weapon_s __s0;
			unsigned int data;
		};

		static_assert(sizeof(Weapon) == 4);

		struct weaponParms
		{
			float forward[3];
			float right[3];
			float up[3];
			float muzzleTrace[3];
			float gunForward[3];
			Weapon weapon;
			bool isAlternate;
			const void* weapDef;
			const void* weapCompleteDef;
		};

		static_assert(sizeof(weaponParms) == 0x4C);

		enum hitLocation_t
		{
			HITLOC_NONE = 0x0,
			HITLOC_HELMET = 0x1,
			HITLOC_HEAD = 0x2,
			HITLOC_NECK = 0x3,
			HITLOC_TORSO_UPR = 0x4,
			HITLOC_TORSO_LWR = 0x5,
			HITLOC_R_ARM_UPR = 0x6,
			HITLOC_L_ARM_UPR = 0x7,
			HITLOC_R_ARM_LWR = 0x8,
			HITLOC_L_ARM_LWR = 0x9,
			HITLOC_R_HAND = 0xA,
			HITLOC_L_HAND = 0xB,
			HITLOC_R_LEG_UPR = 0xC,
			HITLOC_L_LEG_UPR = 0xD,
			HITLOC_R_LEG_LWR = 0xE,
			HITLOC_L_LEG_LWR = 0xF,
			HITLOC_R_FOOT = 0x10,
			HITLOC_L_FOOT = 0x11,
			HITLOC_GUN = 0x12,
			HITLOC_SHIELD = 0x13,

			HITLOC_NUM = 0x14,
		};

		enum ViewLockTypes
		{
			PLAYERVIEWLOCK_NONE = 0x0,
			PLAYERVIEWLOCK_FULL = 0x1,
			PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
			PLAYERVIEWLOCKCOUNT = 0x3,
		};

		enum
		{
			PMF_PRONE = 0x1,
			PMF_DUCKED = 0x2,
			PMF_MANTLE = 0x4,
			PMF_LADDER = 0x8,
			PMF_SIGHT_AIMING = 0x10,
			PMF_BACKWARDS_RUN = 0x20,
			PMF_WALKING = 0x40,
			PMF_TIME_HARDLANDING = 0x80,
			PMF_TIME_KNOCKBACK = 0x100,
			PMF_PRONEMOVE_OVERRIDDEN = 0x200,
			PMF_RESPAWNED = 0x400,
			PMF_FROZEN = 0x800,
			PMF_LADDER_FALL = 0x1000,
			PMF_JUMPING = 0x2000,
			PMF_SPRINTING = 0x4000,
			PMF_SHELLSHOCKED = 0x8000,
			PMF_MELEE_CHARGE = 0x10000,
			PMF_NO_SPRINT = 0x20000,
			PMF_NO_JUMP = 0x40000,
			PMF_REMOTE_CONTROLLING = 0x80000,
			PMF_ANIM_SCRIPTED = 0x100000,
			PMF_DIVING = 0x400000,
		};

		struct playerState_s
		{
			int commandTime;
			int pm_type;
			int pm_time;
			int pm_flags;
			int otherFlags;
			int linkFlags;
			int bobCycle;
			float origin[3];
			float velocity[3];
			int grenadeTimeLeft;
			int throwbackGrenadeOwner;
			int throwbackGrenadeTimeLeft;
			unsigned int throwbackWeapon;
			int remoteEyesEnt;
			int remoteEyesTagname;
			int remoteControlEnt;
			int remoteTurretEnt;
			int foliageSoundTime;
			int gravity;
			int speed;
			int delta_angles;
			int groundEntityNum;
			float vLadderVec[3];
			int jumpTime;
			float jumpOriginZ;
			unsigned char __pad0[0x470];
			unsigned int perks[0x2];
			unsigned int perkSlots[0x9];
			unsigned char __pad1[0x2DF4];
		};

		static_assert(sizeof(playerState_s) == 0x330C);

		struct pmove_t
		{
			playerState_s* ps;
			usercmd_s cmd;
			usercmd_s oldcmd;
			int tracemask;
			int numtouch;
			int touchents[32];
			Bounds bounds;
			float speed;
			int proneChange;
			float maxSprintTimeMultiplier;
			bool mantleStarted;
			float mantleEndPos[3];
			int mantleDuration;
			int viewChangeTime;
			float viewChange;
			float fTorsoPitch;
			float fWaistPitch;
			int remoteTurretFireTime;
			int lastUpdateCMDServerTime;
			unsigned char handler;
		};

		static_assert(sizeof(pmove_t) == 0x138);

		enum sessionState_t
		{
			SESS_STATE_PLAYING = 0x0,
			SESS_STATE_DEAD = 0x1,
			SESS_STATE_SPECTATOR = 0x2,
			SESS_STATE_INTERMISSION = 0x3,
		};

		enum clientConnected_t
		{
			CON_DISCONNECTED = 0x0,
			CON_CONNECTING = 0x1,
			CON_CONNECTED = 0x2,
		};

		struct clientSession_t
		{
			sessionState_t sessionState;
			int forceSpectatorClient;
			int killCamEntity;
			int killCamLookAtEntity;
			int status_icon;
			int archiveTime;
			int score;
			int deaths;
			int kills;
			int assists;
			unsigned __int16 scriptPersId;
			clientConnected_t connected;
			unsigned char __pad0[0x290];
		};

		static_assert(sizeof(clientSession_t) == 0x2C0);

		struct gclient_s
		{
			playerState_s ps;
			clientSession_t sess;
			int flags;
			unsigned char __pad0[0x3B0];
		};

		static_assert(sizeof(gclient_s) == 0x3980);

		enum
		{
			FL_GODMODE = 0x1,
			FL_DEMI_GODMODE = 0x2,
			FL_NOTARGET = 0x4,
			FL_NO_KNOCKBACK = 0x8,
			FL_NO_RADIUS_DAMAGE = 0x10,
			FL_SUPPORTS_LINKTO = 0x1000,
			FL_NO_AUTO_ANIM_UPDATE = 0x2000,
			FL_GRENADE_TOUCH_DAMAGE = 0x4000,
			FL_STABLE_MISSILES = 0x20000,
			FL_REPEAT_ANIM_UPDATE = 0x40000,
			FL_VEHICLE_TARGET = 0x80000,
			FL_GROUND_ENT = 0x100000,
			FL_CURSOR_HINT = 0x200000,
			FL_MISSILE_ATTRACTOR = 0x800000,
			FL_WEAPON_BEING_GRABBED = 0x1000000,
			FL_DELETE = 0x2000000,
			FL_BOUNCE = 0x4000000,
			FL_MOVER_SLIDE = 0x8000000
		};

		enum entityType
		{
			ET_GENERAL,
			ET_PLAYER,
			ET_ITEM,
			ET_MISSILE,
			ET_INVISIBLE,
			ET_SCRIPTMOVER,
			ET_SOUND_BLEND,
			ET_PRIMARY_LIGHT,
			ET_TURRET,
			ET_VEHICLE,
			ET_VEHICLE_COLLMAP,
			ET_VEHICLE_CORPSE,
			ET_VEHICLE_SPAWNER,
			ET_ACTOR,
			ET_ACTOR_SPAWNER,
			ET_ACTOR_CORPSE,
		};

		struct entityState_s
		{
			int number;
		};

		struct EntHandle
		{
			unsigned __int16 number;
			unsigned __int16 infoIndex;
		};

		struct gentity_s
		{
			entityState_s s;
			unsigned char __pad0[0x154];
			gclient_s* client; // 0x158
			unsigned char __pad1[0x28];
			int flags;
			int eventTime;
			int clipmask;
			int processedFrame;
			EntHandle parent;
			int nextthink;
			int health;
			int maxHealth;
			int damage;
			int count;
			unsigned char __pad2[0xC8];
		};

		static_assert(sizeof(gentity_s) == 0x274);

		struct missileFireParms
		{
			gentity_s* target;
			float targetPosOrOffset[3];
			float autoDetonateTime;
			bool lockon;
			bool topFire;
		};

		static_assert(sizeof(missileFireParms) == 0x18);

		enum clientState_t
		{
			CS_FREE = 0,
			CS_ZOMBIE = 1,
			CS_RECONNECTING = 2,
			CS_CONNECTED = 3,
			CS_CLIENTLOADING = 4,
			CS_ACTIVE = 5,
		};

		enum netsrc_t
		{
			NS_CLIENT1 = 0x0,
			NS_CLIENT2 = 0x1,
			NS_CLIENT3 = 0x2,
			NS_CLIENT4 = 0x3,
			NS_MAXCLIENTS = 0x4,
			NS_SERVER = 0x4,
			NS_PACKET = 0x5,
			NS_INVALID_NETSRC = 0x6,
		};

		enum netadrtype_t
		{
			NA_BOT = 0x0,
			NA_BAD = 0x1,
			NA_LOOPBACK = 0x2,
			NA_BROADCAST = 0x3,
			NA_IP = 0x4,
		};

		struct netadr_s
		{
			netadrtype_t type;
			unsigned char ip[4];
			unsigned __int16 port;
			unsigned char ipx[10];
			unsigned int addrHandleIndex;
		};

		static_assert(sizeof(netadr_s) == 24);

		struct DeferredMsg
		{
			netadr_s addr;
			unsigned char data[1262];
			int datalen;
		};

		static_assert(sizeof(DeferredMsg) == 0x50C);

		struct DeferredQueue
		{
			DeferredMsg msgs[16];
			volatile long get;
			volatile long send;
		};

		static_assert(sizeof(DeferredQueue) == 0x50C8);

		struct netProfileInfo_t // Unused
		{
			unsigned char __pad0[0x5E0];
		};

		struct netchan_t
		{
			int outgoingSequence;
			netsrc_t sock;
			int dropped;
			int incomingSequence;
			netadr_s remoteAddress;
			int qport;
			int fragmentSequence;
			int fragmentLength;
			unsigned char* fragmentBuffer;
			int fragmentBufferSize;
			int unsentFragments;
			int unsentFragmentStart;
			int unsentLength;
			unsigned char* unsentBuffer;
			int unsentBufferSize;
			netProfileInfo_t prof;
		};

		static_assert(sizeof(netchan_t) == 0x630);

		struct clientHeader_t
		{
			clientState_t state;
			int sendAsActive;
			int deltaMessage;
			int rateDealyed;
			int hasAckedBaselineData;
			int hugeSnapshotSent;
			netchan_t netchan;
			float predictedOrigin[3];
			int predictedOriginServerTime;
			int migrationState;
			float predictedVehicleOrigin[3];
			int predictedVehicleServerTime;
		};

		static_assert(sizeof(clientHeader_t) == 0x66C);

		enum objectiveState_t
		{
			OBJST_EMPTY = 0x0,
			OBJST_ACTIVE = 0x1,
			OBJST_INVISIBLE = 0x2,
			OBJST_DONE = 0x3,
			OBJST_CURRENT = 0x4,
			OBJST_FAILED = 0x5,
			OBJST_NUMSTATES = 0x6,
		};

		struct objective_t
		{
			objectiveState_t state;
			float origin[3];
			int entNum;
			int teamNum;
			int clientNum;
			int invertVisibilityByClientNum;
			int icon;
		};

		static_assert(sizeof(objective_t) == 0x24);

		struct level_locals_t
		{
			gclient_s* clients;
			gentity_s* gentities;
			int num_entities;
			gentity_s* firstFreeEnt;
			gentity_s* lastFreeEnt;
			void* turrets;
			int initializing;
			int clientIsSpawning;
			objective_t objectives[32];
			int maxclients;
			int framenum;
			int time;
			unsigned char __pad0[0x2BD4];
		};

		static_assert(sizeof(level_locals_t) == 0x3080);

		enum CriticalSection
		{
			CRITSECT_CONSOLE = 0x0,
			CRITSECT_FATAL_ERROR = 0x12,
			CRITSECT_CBUF = 0x1F,
			CRITSECT_COUNT = 0x27,
		};

		enum ScopedCriticalSectionType
		{
			SCOPED_CRITSECT_NORMAL = 0x0,
			SCOPED_CRITSECT_DISABLED = 0x1,
			SCOPED_CRITSECT_RELEASE = 0x2,
			SCOPED_CRITSECT_TRY = 0x3,
		};

		struct fileInIwd_s
		{
			unsigned int pos;
			char* name;
			fileInIwd_s* next;
		};

		struct iwd_t
		{
			char iwdFilename[256];
			char iwdBasename[256];
			char iwdGamename[256];
			char* handle;
			int checksum;
			int pure_checksum;
			volatile int hasOpenFile;
			int numfiles;
			char referenced;
			unsigned int hashSize;
			fileInIwd_s** hashTable;
			fileInIwd_s* buildBuffer;
		};

		struct directory_t
		{
			char path[256];
			char gamedir[256];
		};

		struct searchpath_s
		{
			searchpath_s* next;
			iwd_t* iwd;
			directory_t* dir;
			int bLocalized;
			int ignore;
			int ignorePureCheck;
			int language;
		};

		enum FsThread
		{
			FS_THREAD_MAIN = 0x0,
			FS_THREAD_STREAM = 0x1,
			FS_THREAD_DATABASE = 0x2,
			FS_THREAD_BACKEND = 0x3,
			FS_THREAD_SERVER = 0x4,
			FS_THREAD_COUNT = 0x5,
			FS_THREAD_INVALID = 0x6,
		};

		union qfile_gus
		{
			_iobuf* o;
			char* z;
		};

		struct qfile_us
		{
			qfile_gus file;
			int iwdIsClone;
		};

		struct fileHandleData_t
		{
			qfile_us handleFiles;
			int handleSync;
			int fileSize;
			int zipFilePos;
			iwd_t* zipFile;
			int streamed;
			char name[256];
		};

		static_assert(sizeof(fileHandleData_t) == 0x11C);

		struct field_t
		{
			int cursor;
			int scroll;
			int drawWidth;
			int widthInPixels;
			float charHeight;
			int fixedSize;
			char buffer[256];
		};

		enum LocSelInputState
		{
			LOC_SEL_INPUT_NONE = 0x0,
			LOC_SEL_INPUT_CONFIRM = 0x1,
			LOC_SEL_INPUT_CANCEL = 0x2,
		};

		struct KeyState
		{
			int down;
			int repeats;
			int binding;
		};

		struct PlayerKeyState
		{
			field_t chatField;
			int chat_team;
			int overstrikeMode;
			int anyKeyDown;
			KeyState keys[256];
			LocSelInputState locSelInputState;
		};

		static_assert(sizeof(PlayerKeyState) == 0xD28);

		namespace mp
		{
			enum ConfigString
			{
				CS_EFFECT_NAMES = 0x9DC,
			};

			struct client_t
			{
				clientHeader_t header;
				const char* dropReason; // 0x66C
				char userinfo[1024]; // 0x670
				unsigned char __pad0[0x209B8];
				gentity_s* gentity; // 0x21428
				char name[16]; // 0x2142C
				unsigned char __pad1[0x20876];
				unsigned __int16 scriptId; // 0x41CB2
				int bIsTestClient; // 0x41CB4
				int serverId; // 0x41CB8
				unsigned char __pad2[0x369DC];
			};

			static_assert(sizeof(client_t) == 0x78698);
		}

		namespace sp
		{
			enum ConfigString
			{
				CS_EFFECT_NAMES = 0xB8F,
			};

			struct usercmd_s
			{
				int serverTime;
				int buttons;
				int angles[3];
				Weapon weapon;
				Weapon offHand;
				char forwardmove;
				char rightmove;
				unsigned char upmove;
				unsigned char downmove;
				char pitchmove;
				char yawmove;
				float gunPitch;
				float gunYaw;
				float gunXOfs;
				float gunYOfs;
				float gunZOfs;
				short meleeChargeYaw;
				unsigned char meleeChargeDist;
				char selectedLoc[2];
				unsigned char selectedLocAngle;
				char remoteControlAngles[2];
			};

			static_assert(sizeof(usercmd_s) == 0x40);

			struct clientPersistent_t
			{
				usercmd_s cmd;
				usercmd_s oldcmd;
				int maxHealth;
				float moveSpeedScaleMultiplier;
				int motionTrackerEnabled;
				char playerName[0x20];
			};

			static_assert(sizeof(clientPersistent_t) == 0xAC);

			struct playerState_s
			{
				int commandTime;
				int pm_type;
				int pm_time;
				int pm_flags;
				int otherFlags;
				int linkFlags;
				int bobCycle;
				float origin[3];
				float velocity[3];
				unsigned char __pad0[0xAD24];
			};

			static_assert(sizeof(playerState_s) == 0xAD58);

			struct gclient_s
			{
				playerState_s ps;
				clientPersistent_t pers;
				int flags;
				unsigned char __pad1[0x2BC];
			};

			static_assert(sizeof(gclient_s) == 0xB0C4);

			struct entityState_s
			{
				int eType;
				unsigned char __pad0[0x80];
				int number;
				unsigned char __pad1[0x28];
			};

			static_assert(sizeof(entityState_s) == 0xB0);

			struct entityShared_t
			{
				unsigned char isLinked;
				unsigned char modelType;
				unsigned char svFlags;
				unsigned char eventType;
				unsigned char isInUse;
				Bounds box;
				int contents;
				Bounds absBox;
				float currentOrigin[3];
				float currentAngles[3];
				EntHandle ownerNum;
				int eventTime;
			};

			static_assert(sizeof(entityShared_t) == 0x5C);

			struct gentity_s
			{
				entityState_s s;
				entityShared_t r;
				gclient_s* client; // 0x10C
				unsigned char __pad0[0x2C];
				int flags;
				int clipmask;
				int processedFrame;
				EntHandle parent;
				int nextthink;
				int health;
				int maxHealth;
				int damage;
				unsigned char __pad1[0x114];
			};

			static_assert(sizeof(gentity_s) == 0x270);
		}
	}
}
