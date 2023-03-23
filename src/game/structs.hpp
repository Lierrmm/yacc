#pragma once

namespace game
{
	namespace native
	{
		typedef float vec_t;
		typedef vec_t vec2_t[2];
		typedef vec_t vec3_t[3];
		typedef vec_t vec4_t[4];
		typedef unsigned __int16 ScriptString;

		struct cmd_function_s
		{
			cmd_function_s* next;
			const char* name;
			const char* args;			//autoCompleteDir;
			const char* description;	// autoCompleteExt
			void(__cdecl* function)();
		};

		struct GfxImageFileHeader
		{
			char tag[3];
			char version;
			char format;
			char flags;
			short dimensions[3];
			int fileSizeForPicmip[4];
		};

		struct GfxVertexShaderLoadDef
		{
			unsigned int* program;
			unsigned __int16 programSize;
			unsigned __int16 loadForRenderer;
		};

		struct MaterialVertexShaderProgram
		{
			void* vs;
			GfxVertexShaderLoadDef loadDef;
		};

		struct MaterialVertexShader
		{
			const char* name;
			MaterialVertexShaderProgram prog;
		};


		struct GfxPixelShaderLoadDef
		{
			unsigned int* program;
			unsigned __int16 programSize;
			unsigned __int16 loadForRenderer;
		};

		struct MaterialPixelShaderProgram
		{
			void* ps;
			GfxPixelShaderLoadDef loadDef;
		};

		struct MaterialPixelShader
		{
			const char* name;
			MaterialPixelShaderProgram prog;
		};

		struct MaterialConstantDef
		{
			int nameHash;
			char name[12];
			vec4_t literal;
		};

		struct GfxStateBits
		{
			unsigned int loadBits[2];
		};

		struct WaterWritable
		{
			float floatTime;
		};

		struct complex_s
		{
			float real;
			float imag;
		};

		struct GfxImage;

		struct water_t
		{
			WaterWritable writable;
			complex_s* H0;
			float* wTerm;
			int M;
			int N;
			float Lx;
			float Lz;
			float gravity;
			float windvel;
			float winddir[2];
			float amplitude;
			float codeConstant[4];
			GfxImage* image;
		};

		/* MaterialTextureDef->semantic */
#define TS_2D           0x0
#define TS_FUNCTION     0x1
#define TS_COLOR_MAP    0x2
#define TS_UNUSED_1     0x3
#define TS_UNUSED_2     0x4
#define TS_NORMAL_MAP   0x5
#define TS_UNUSED_3     0x6
#define TS_UNUSED_4     0x7
#define TS_SPECULAR_MAP 0x8
#define TS_UNUSED_5     0x9
#define TS_UNUSED_6     0xA
#define TS_WATER_MAP    0xB

		union MaterialTextureDefInfo
		{
			GfxImage* image;	// MaterialTextureDef->semantic != TS_WATER_MAP
			water_t* water;		// MaterialTextureDef->semantic == TS_WATER_MAP
		};

		struct MaterialTextureDef
		{
			unsigned int nameHash;
			char nameStart;
			char nameEnd;
			char samplerState;
			char semantic;
			MaterialTextureDefInfo u;
		};

		struct GfxDrawSurfFields
		{
			unsigned __int64 objectId : 16;
			unsigned __int64 reflectionProbeIndex : 8;
			unsigned __int64 customIndex : 5;
			unsigned __int64 materialSortedIndex : 11;
			unsigned __int64 prepass : 2;
			unsigned __int64 primaryLightIndex : 8;
			unsigned __int64 surfType : 4;
			unsigned __int64 primarySortKey : 6;
			unsigned __int64 unused : 4;
		};

		union GfxDrawSurf
		{
			GfxDrawSurfFields fields;
			unsigned long long packed;
		};

		struct MaterialArgumentCodeConst
		{
			unsigned __int16 index;
			char firstRow;
			char rowCount;
		};

		union MaterialArgumentDef
		{
			const float* literalConst;
			MaterialArgumentCodeConst codeConst;
			unsigned int codeSampler;
			unsigned int nameHash;
		};

		/* MaterialShaderArgument->type */
#define MTL_ARG_MATERIAL_VERTEX_CONST	0x0
#define MTL_ARG_LITERAL_VERTEX_CONST	0x1
#define MTL_ARG_MATERIAL_PIXEL_SAMPLER	0x2
#define MTL_ARG_CODE_VERTEX_CONST	    0x3
#define MTL_ARG_CODE_PIXEL_SAMPLER	    0x4
#define MTL_ARG_CODE_PIXEL_CONST	    0x5
#define MTL_ARG_MATERIAL_PIXEL_CONST	0x6
#define MTL_ARG_LITERAL_PIXEL_CONST	    0x7

		struct MaterialShaderArgument
		{
			unsigned __int16 type;
			unsigned __int16 dest;
			MaterialArgumentDef u;
		};

		enum ShaderCodeConstants
		{
			CONST_SRC_CODE_MAYBE_DIRTY_PS_BEGIN = 0x0,
			CONST_SRC_CODE_LIGHT_POSITION = 0x0,
			CONST_SRC_CODE_LIGHT_DIFFUSE = 0x1,
			CONST_SRC_CODE_LIGHT_SPECULAR = 0x2,
			CONST_SRC_CODE_LIGHT_SPOTDIR = 0x3,
			CONST_SRC_CODE_LIGHT_SPOTFACTORS = 0x4,
			CONST_SRC_CODE_NEARPLANE_ORG = 0x5,
			CONST_SRC_CODE_NEARPLANE_DX = 0x6,
			CONST_SRC_CODE_NEARPLANE_DY = 0x7,
			CONST_SRC_CODE_SHADOW_PARMS = 0x8,
			CONST_SRC_CODE_SHADOWMAP_POLYGON_OFFSET = 0x9,
			CONST_SRC_CODE_RENDER_TARGET_SIZE = 0xA,
			CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT = 0xB,
			CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR = 0xC,
			CONST_SRC_CODE_DOF_EQUATION_SCENE = 0xD,
			CONST_SRC_CODE_DOF_LERP_SCALE = 0xE,
			CONST_SRC_CODE_DOF_LERP_BIAS = 0xF,
			CONST_SRC_CODE_DOF_ROW_DELTA = 0x10,
			CONST_SRC_CODE_PARTICLE_CLOUD_COLOR = 0x11,
			CONST_SRC_CODE_GAMETIME = 0x12,
			CONST_SRC_CODE_MAYBE_DIRTY_PS_END = 0x13,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_BEGIN = 0x13,
			CONST_SRC_CODE_PIXEL_COST_FRACS = 0x13,
			CONST_SRC_CODE_PIXEL_COST_DECODE = 0x14,
			CONST_SRC_CODE_FILTER_TAP_0 = 0x15,
			CONST_SRC_CODE_FILTER_TAP_1 = 0x16,
			CONST_SRC_CODE_FILTER_TAP_2 = 0x17,
			CONST_SRC_CODE_FILTER_TAP_3 = 0x18,
			CONST_SRC_CODE_FILTER_TAP_4 = 0x19,
			CONST_SRC_CODE_FILTER_TAP_5 = 0x1A,
			CONST_SRC_CODE_FILTER_TAP_6 = 0x1B,
			CONST_SRC_CODE_FILTER_TAP_7 = 0x1C,
			CONST_SRC_CODE_COLOR_MATRIX_R = 0x1D,
			CONST_SRC_CODE_COLOR_MATRIX_G = 0x1E,
			CONST_SRC_CODE_COLOR_MATRIX_B = 0x1F,
			CONST_SRC_CODE_ALWAYS_DIRTY_PS_END = 0x20,
			CONST_SRC_CODE_NEVER_DIRTY_PS_BEGIN = 0x20,
			CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION = 0x20,
			CONST_SRC_CODE_SHADOWMAP_SCALE = 0x21,
			CONST_SRC_CODE_ZNEAR = 0x22,
			CONST_SRC_CODE_SUN_POSITION = 0x23,
			CONST_SRC_CODE_SUN_DIFFUSE = 0x24,
			CONST_SRC_CODE_SUN_SPECULAR = 0x25,
			CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE = 0x26,
			CONST_SRC_CODE_DEBUG_BUMPMAP = 0x27,
			CONST_SRC_CODE_MATERIAL_COLOR = 0x28,
			CONST_SRC_CODE_FOG = 0x29,
			CONST_SRC_CODE_FOG_COLOR = 0x2A,
			CONST_SRC_CODE_GLOW_SETUP = 0x2B,
			CONST_SRC_CODE_GLOW_APPLY = 0x2C,
			CONST_SRC_CODE_COLOR_BIAS = 0x2D,
			CONST_SRC_CODE_COLOR_TINT_BASE = 0x2E,
			CONST_SRC_CODE_COLOR_TINT_DELTA = 0x2F,
			CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS = 0x30,
			CONST_SRC_CODE_ENVMAP_PARMS = 0x31,
			CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST = 0x32,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE = 0x33,
			CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET = 0x34,
			CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX = 0x35,
			CONST_SRC_CODE_DEPTH_FROM_CLIP = 0x36,
			CONST_SRC_CODE_CODE_MESH_ARG_0 = 0x37,
			CONST_SRC_CODE_CODE_MESH_ARG_1 = 0x38,
			CONST_SRC_CODE_CODE_MESH_ARG_LAST = 0x38,
			CONST_SRC_CODE_BASE_LIGHTING_COORDS = 0x39,
			CONST_SRC_CODE_NEVER_DIRTY_PS_END = 0x3A,
			CONST_SRC_CODE_COUNT_FLOAT4 = 0x3A,
			CONST_SRC_FIRST_CODE_MATRIX = 0x3A,
			CONST_SRC_CODE_WORLD_MATRIX = 0x3A,
			CONST_SRC_CODE_INVERSE_WORLD_MATRIX = 0x3B,
			CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX = 0x3C,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX = 0x3D,
			CONST_SRC_CODE_VIEW_MATRIX = 0x3E,
			CONST_SRC_CODE_INVERSE_VIEW_MATRIX = 0x3F,
			CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX = 0x40,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX = 0x41,
			CONST_SRC_CODE_PROJECTION_MATRIX = 0x42,
			CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX = 0x43,
			CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX = 0x44,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX = 0x45,
			CONST_SRC_CODE_WORLD_VIEW_MATRIX = 0x46,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX = 0x47,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x48,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX = 0x49,
			CONST_SRC_CODE_VIEW_PROJECTION_MATRIX = 0x4A,
			CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX = 0x4B,
			CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4C,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX = 0x4D,
			CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX = 0x4E,
			CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX = 0x4F,
			CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x50,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX = 0x51,
			CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX = 0x52,
			CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX = 0x53,
			CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x54,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX = 0x55,
			CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x56,
			CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x57,
			CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x58,
			CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX = 0x59,
			CONST_SRC_TOTAL_COUNT = 0x5A,
			CONST_SRC_NONE = 0x5B,
		};

		struct MaterialStreamRouting
		{
			char source;
			char dest;
		};

		struct MaterialVertexStreamRouting
		{
			MaterialStreamRouting data[16];
			void* decl[16];
		};

		struct MaterialVertexDeclaration
		{
			char streamCount;
			bool hasOptionalSource;
			bool isLoaded;
			MaterialVertexStreamRouting routing;
		};

#pragma pack(push, 4)
		struct MaterialPass
		{
			MaterialVertexDeclaration* vertexDecl;
			MaterialVertexShader* vertexShader;
			MaterialPixelShader* pixelShader;
			char perPrimArgCount;
			char perObjArgCount;
			char stableArgCount;
			char customSamplerFlags;
			MaterialShaderArgument* args;
		};
#pragma pack(pop)

		struct MaterialTechnique
		{
			const char* name;
			unsigned __int16 flags;
			unsigned __int16 passCount;
			MaterialPass passArray[1];	// count = passCount
		};

		/* MaterialTechniqueSet->worldVertFormat */
		enum MaterialWorldVertexFormat : char
		{
			MTL_WORLDVERT_TEX_1_NRM_1 = 0x0,
			MTL_WORLDVERT_TEX_2_NRM_1 = 0x1,
			MTL_WORLDVERT_TEX_2_NRM_2 = 0x2,
			MTL_WORLDVERT_TEX_3_NRM_1 = 0x3,
			MTL_WORLDVERT_TEX_3_NRM_2 = 0x4,
			MTL_WORLDVERT_TEX_3_NRM_3 = 0x5,
			MTL_WORLDVERT_TEX_4_NRM_1 = 0x6,
			MTL_WORLDVERT_TEX_4_NRM_2 = 0x7,
			MTL_WORLDVERT_TEX_4_NRM_3 = 0x8,
			MTL_WORLDVERT_TEX_5_NRM_1 = 0x9,
			MTL_WORLDVERT_TEX_5_NRM_2 = 0xA,
			MTL_WORLDVERT_TEX_5_NRM_3 = 0xB,
		};

		/*struct MaterialTechniqueSet // org
		{
			char *name;
			MaterialWorldVertexFormat worldVertFormat;
			MaterialTechnique *techniques[34];
		};*/

		struct MaterialTechniqueSet
		{
			char* name;
			MaterialWorldVertexFormat worldVertFormat;
			bool hasBeenUploaded;
			char unused[1];
			MaterialTechniqueSet* remappedTechniqueSet;
			MaterialTechnique* techniques[34];
		};

#pragma pack(push, 4)
		struct MaterialInfo
		{
			const char* name;
			char gameFlags;
			char sortKey;
			char textureAtlasRowCount;
			char textureAtlasColumnCount;
			GfxDrawSurf drawSurf;
			unsigned int surfaceTypeBits;
			unsigned __int16 hashIndex;
		};
#pragma pack(pop)

		enum MaterialTechniqueType
		{
			TECHNIQUE_DEPTH_PREPASS = 0x0,
			TECHNIQUE_BUILD_FLOAT_Z = 0x1,
			TECHNIQUE_BUILD_SHADOWMAP_DEPTH = 0x2,
			TECHNIQUE_BUILD_SHADOWMAP_COLOR = 0x3,
			TECHNIQUE_UNLIT = 0x4,
			TECHNIQUE_EMISSIVE = 0x5,
			TECHNIQUE_EMISSIVE_SHADOW = 0x6,
			TECHNIQUE_LIT_BEGIN = 0x7,
			TECHNIQUE_LIT = 0x7,
			TECHNIQUE_LIT_SUN = 0x8,
			TECHNIQUE_LIT_SUN_SHADOW = 0x9,
			TECHNIQUE_LIT_SPOT = 0xA,
			TECHNIQUE_LIT_SPOT_SHADOW = 0xB,
			TECHNIQUE_LIT_OMNI = 0xC,
			TECHNIQUE_LIT_OMNI_SHADOW = 0xD,
			TECHNIQUE_LIT_INSTANCED = 0xE,
			TECHNIQUE_LIT_INSTANCED_SUN = 0xF,
			TECHNIQUE_LIT_INSTANCED_SUN_SHADOW = 0x10,
			TECHNIQUE_LIT_INSTANCED_SPOT = 0x11,
			TECHNIQUE_LIT_INSTANCED_SPOT_SHADOW = 0x12,
			TECHNIQUE_LIT_INSTANCED_OMNI = 0x13,
			TECHNIQUE_LIT_INSTANCED_OMNI_SHADOW = 0x14,
			TECHNIQUE_LIT_END = 0x15,
			TECHNIQUE_LIGHT_SPOT = 0x15,
			TECHNIQUE_LIGHT_OMNI = 0x16,
			TECHNIQUE_LIGHT_SPOT_SHADOW = 0x17,
			TECHNIQUE_FAKELIGHT_NORMAL = 0x18,
			TECHNIQUE_FAKELIGHT_VIEW = 0x19,
			TECHNIQUE_SUNLIGHT_PREVIEW = 0x1A,
			TECHNIQUE_CASE_TEXTURE = 0x1B,
			TECHNIQUE_WIREFRAME_SOLID = 0x1C,
			TECHNIQUE_WIREFRAME_SHADED = 0x1D,
			TECHNIQUE_SHADOWCOOKIE_CASTER = 0x1E,
			TECHNIQUE_SHADOWCOOKIE_RECEIVER = 0x1F,
			TECHNIQUE_DEBUG_BUMPMAP = 0x20,
			TECHNIQUE_DEBUG_BUMPMAP_INSTANCED = 0x21,
			//TECHNIQUE_COUNT = 0x22
		};

		struct infoParm_t
		{
			const char* name;
			int clearSolid;
			int surfaceFlags;
			int contents;
			int toolFlags;
		};

		struct Material
		{
			MaterialInfo info;
			char stateBitsEntry[34];
			char textureCount;
			char constantCount;
			char stateBitsCount;
			char stateFlags;
			char cameraRegion;
			MaterialTechniqueSet* techniqueSet;
			MaterialTextureDef* textureTable;
			MaterialConstantDef* constantTable;
			GfxStateBits* stateBitsTable;
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

		typedef struct
		{
			uint16_t id;
			uint16_t byteCount;
		}GfxCmdHeader;

		typedef union
		{
			unsigned int packed;
			uint8_t array[4];
		}GfxColor;

		typedef struct
		{
			uint8_t* cmds;
			int usedTotal;
			int usedCritical;
			GfxCmdHeader* lastCmd;
		}GfxCmdArray;

		typedef struct //Dynamic sized structure !!
		{
			GfxCmdHeader header;
			float x;
			float y;
			//  float w;
			float rotation;
			Font_s* font;
			float xScale;
			float yScale;
			GfxColor color;
			int maxChars;
			int renderFlags;
			int cursorPos;
			char cursorLetter;
			uint8_t pad[3];
			GfxColor glowForceColor;
			int fxBirthTime;
			int fxLetterTime;
			int fxDecayStartTime;
			int fxDecayDuration;
			Material* fxMaterial;
			Material* fxMaterialGlow;
			float padding;
			char text[4]; //Dynamic sized field
		}GfxCmdDrawText2D;

		enum connstate_t
		{
			CA_DISCONNECTED = 0x0,
			CA_CINEMATIC = 0x1,
			CA_LOGO = 0x2,
			CA_CONNECTING = 0x3,
			CA_CHALLENGING = 0x4,
			CA_CONNECTED = 0x5,
			CA_SENDINGSTATS = 0x6,
			CA_LOADING = 0x7,
			CA_PRIMED = 0x8,
			CA_ACTIVE = 0x9,
		};

		struct clientUIActive_t
		{
			bool active;
			bool isRunning;
			bool cgameInitialized;
			bool cgameInitCalled;
			int keyCatchers;
			bool displayHUDWithKeycatchUI;
			connstate_t connectionState;
		};

		typedef struct
		{
			const char* language;
			const char* translations;
		}localization_t;

		enum DemoType
		{
			DEMO_TYPE_NONE = 0x0,
			DEMO_TYPE_CLIENT = 0x1,
			DEMO_TYPE_SERVER = 0x2,
		};

		enum CubemapShot
		{
			CUBEMAPSHOT_NONE = 0x0,
			CUBEMAPSHOT_RIGHT = 0x1,
			CUBEMAPSHOT_LEFT = 0x2,
			CUBEMAPSHOT_BACK = 0x3,
			CUBEMAPSHOT_FRONT = 0x4,
			CUBEMAPSHOT_UP = 0x5,
			CUBEMAPSHOT_DOWN = 0x6,
			CUBEMAPSHOT_COUNT = 0x7,
		};

		enum trType_t
		{
			TR_STATIONARY = 0x0,
			TR_INTERPOLATE = 0x1,
			TR_LINEAR = 0x2,
			TR_LINEAR_STOP = 0x3,
			TR_SINE = 0x4,
			TR_GRAVITY = 0x5,
			TR_ACCELERATE = 0x6,
			TR_DECELERATE = 0x7,
			TR_PHYSICS = 0x8,
			TR_FIRST_RAGDOLL = 0x9,
			TR_RAGDOLL = 0x9,
			TR_RAGDOLL_GRAVITY = 0xA,
			TR_RAGDOLL_INTERPOLATE = 0xB,
			TR_LAST_RAGDOLL = 0xB,
		};

		struct trajectory_t
		{
			trType_t trType;
			int trTime;
			int trDuration;
			float trBase[3];
			float trDelta[3];
		};

		struct LerpEntityStateEarthquake
		{
			float scale;
			float radius;
			int duration;
		};

		struct LerpEntityStateLoopFx
		{
			float cullDist;
			int period;
		};

		/* 807 */
		struct LerpEntityStateCustomExplode
		{
			int startTime;
		};

		/* 808 */
		struct LerpEntityStateTurret
		{
			float gunAngles[3];
		};

		/* 813 */
		struct LerpEntityStateExplosion
		{
			float innerRadius;
			float magnitude;
		};

		/* 814 */
		struct LerpEntityStateBulletHit
		{
			float start[3];
		};

		/* 815 */
		struct LerpEntityStatePrimaryLight
		{
			char colorAndExp[4];
			float intensity;
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
		};

		struct LerpEntityStatePlayer
		{
			float leanf;
			int movementDir;
		};

		struct LerpEntityStateVehicle
		{
			float bodyPitch;
			float bodyRoll;
			float steerYaw;
			int materialTime;
			float gunPitch;
			float gunYaw;
			int teamAndOwnerIndex;
		};

		struct LerpEntityStateMissile
		{
			int launchTime;
		};

		/* 818 */
		struct LerpEntityStateSoundBlend
		{
			float lerp;
		};

		struct LerpEntityStateExplosionJolt
		{
			float innerRadius;
			float impulse[3];
		};

		struct LerpEntityStatePhysicsJitter
		{
			float innerRadius;
			float minDisplacement;
			float maxDisplacement;
		};

		struct LerpEntityStateAnonymous
		{
			int data[7];
		};

		union LerpEntityStateTypeUnion
		{
			LerpEntityStateTurret turret;
			LerpEntityStateLoopFx loopFx;
			LerpEntityStatePrimaryLight primaryLight;
			LerpEntityStatePlayer player;
			LerpEntityStateVehicle vehicle;
			LerpEntityStateMissile missile;
			LerpEntityStateSoundBlend soundBlend;
			LerpEntityStateBulletHit bulletHit;
			LerpEntityStateEarthquake earthquake;
			LerpEntityStateCustomExplode customExplode;
			LerpEntityStateExplosion explosion;
			LerpEntityStateExplosionJolt explosionJolt;
			LerpEntityStatePhysicsJitter physicsJitter;
			LerpEntityStateAnonymous anonymous;
		};

		enum team_t
		{
			TEAM_FREE = 0x0,
			TEAM_AXIS = 0x1,
			TEAM_ALLIES = 0x2,
			TEAM_SPECTATOR = 0x3,
			TEAM_NUM_TEAMS = 0x4,
		};

		struct clientState_s
		{
			int clientIndex;
			team_t team;
			int modelindex;
			int attachModelIndex[6];
			int attachTagIndex[6];
			char name[16];
			float maxSprintTimeMultiplier;
			int rank;
			int prestige;
			int perks;
			int attachedVehEntNum;
			int attachedVehSlotIndex;
		};

		

		struct GfxSkinCacheEntry
		{
			unsigned int frameCount;
			int skinnedCachedOffset;
			unsigned __int16 numSkinnedVerts;
			unsigned __int16 ageCount;
		};

		struct cpose_t
		{
			unsigned __int16 lightingHandle;
			char eType;
			char eTypeUnion;
			char localClientNum;
			int cullIn;
			char isRagdoll;
			int ragdollHandle;
			int killcamRagdollHandle;
			int physObjId;
			float origin[3];
			float angles[3];
			GfxSkinCacheEntry skinCacheEntry;
			//$EAE81CC4C8A7A2F7E95AA09AC9F9F9C0 ___u12;
			char pad[0x24];
		};

		struct LerpEntityState
		{
			int eFlags;
			trajectory_t pos;
			trajectory_t apos;
			LerpEntityStateTypeUnion u;
		};

		struct entityState_s
		{
			int number;
			int eType;
			LerpEntityState lerp;
			int time2;
			int otherEntityNum;
			int attackerEntityNum;
			int groundEntityNum;
			int loopSound;
			int surfType;
			int index;
			int clientNum;
			int iHeadIcon;
			int iHeadIconTeam;
			int solid;
			unsigned int eventParm;
			int eventSequence;
			int events[4];
			int eventParms[4];
			int weapon;
			int weaponModel;
			int legsAnim;
			int torsoAnim;
			int indexUnion1;
			int indexUnion2;
			float fTorsoPitch;
			float fWaistPitch;
			unsigned int partBits[4];
		};

		struct centity_s
		{
			cpose_t pose;
			LerpEntityState currentState;
			entityState_s nextState;
			bool nextValid;
			bool bMuzzleFlash;
			bool bTrailMade;
			int previousEventSequence;
			int miscTime;
			float lightingOrigin[3];
			void* tree; // XAnimTree_s
		};

		struct playerEntity_t
		{
			float fLastWeaponPosFrac;
			int bPositionToADS;
			float vPositionLastOrg[3];
			float fLastIdleFactor;
			float vLastMoveOrg[3];
			float vLastMoveAng[3];
		};

		struct GfxDepthOfField
		{
			float viewModelStart;
			float viewModelEnd;
			float nearStart;
			float nearEnd;
			float farStart;
			float farEnd;
			float nearBlur;
			float farBlur;
		};

		struct GfxFilm
		{
			bool enabled;
			float brightness;
			float contrast;
			float desaturation;
			bool invert;
			float tintDark[3];
			float tintLight[3];
		};

		struct GfxGlow
		{
			bool enabled;
			float bloomCutoff;
			float bloomDesaturation;
			float bloomIntensity;
			float radius;
		};

		struct GfxViewport
		{
			int x;
			int y;
			int width;
			int height;
		};

		enum MapType
		{
			MAPTYPE_NONE = 0x0,
			MAPTYPE_INVALID1 = 0x1,
			MAPTYPE_INVALID2 = 0x2,
			MAPTYPE_2D = 0x3,
			MAPTYPE_3D = 0x4,
			MAPTYPE_CUBE = 0x5,
			MAPTYPE_COUNT = 0x6,
		};

		struct Picmip
		{
			char platform[2];
		};

		struct CardMemory
		{
			int platform[2];
		};

		struct GfxImageLoadDef
		{
			char levelCount;
			char flags;
			__int16 dimensions[3];
			int format;
			int resourceSize;
			char data[1];
		};

		union GfxTexture
		{
			IDirect3DBaseTexture9* basemap;
			IDirect3DTexture9* map;
			IDirect3DVolumeTexture9* volmap;
			IDirect3DCubeTexture9* cubemap;
			GfxImageLoadDef* loadDef;
			void* data;
		};

		struct GfxImage
		{
			MapType mapType;
			GfxTexture texture;
			Picmip picmip;
			bool noPicmip;
			char semantic;
			char track;
			byte field_05;
			unsigned __int16 field_06;
			CardMemory cardMemory;
			unsigned __int16 width;
			unsigned __int16 height;
			unsigned __int16 depth;
			char category;
			bool delayLoadPixels;
			const char* name;
		};

		struct __declspec(align(4)) GfxLightImage
		{
			GfxImage* image;
			char samplerState;
		};

		struct GfxLightDef
		{
			const char* name;
			GfxLightImage attenuation;
			int lmapLookupStart;
		};

		struct GfxLight
		{
			char type;
			char canUseShadowMap;
			char unused[2];
			float color[3];
			float dir[3];
			float origin[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
			unsigned int spotShadowIndex;
			GfxLightDef* def;
		};
		struct refdef_s
		{
			unsigned int x;
			unsigned int y;
			unsigned int width;
			unsigned int height;
			float tanHalfFovX;
			float tanHalfFovY;
			float vieworg[3];
			float viewaxis[3][3];
			float viewOffset[3];
			int time;
			float zNear;
			float blurRadius;
			GfxDepthOfField dof;
			GfxFilm film;
			GfxGlow glow;
			GfxLight primaryLights[255];
			GfxViewport scissorViewport;
			bool useScissorViewport;
			int localClientNum;
		};

		struct DObjAnimMat
		{
			float quat[4];
			float trans[3];
			float transWeight;
		};

		struct XSurfaceCollisionAabb
		{
			unsigned short mins[3];
			unsigned short maxs[3];
		};

		struct XSurfaceCollisionNode
		{
			XSurfaceCollisionAabb aabb;
			unsigned short childBeginIndex;
			unsigned short childCount;
		};

		struct XSurfaceCollisionLeaf
		{
			unsigned short triangleBeginIndex;
		};

		struct XSurfaceCollisionTree
		{
			float trans[3];
			float scale[3];
			unsigned int nodeCount;
			XSurfaceCollisionNode* nodes;
			unsigned int leafCount;
			XSurfaceCollisionLeaf* leafs;
		};

		struct XRigidVertList
		{
			unsigned short boneOffset;
			unsigned short vertCount;
			unsigned short triOffset;
			unsigned short triCount;
			XSurfaceCollisionTree* collisionTree;
		};

		struct XSurfaceVertexInfo
		{
			short vertCount[4];
			unsigned short* vertsBlend;
		};


		union PackedTexCoords
		{
			unsigned int packed;
		};

		union PackedUnitVec
		{
			unsigned int packed;
			char array[4];
		};

		struct GfxPackedVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			PackedTexCoords texCoord;
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct XSurface
		{
			char tileMode;
			bool deformed;
			unsigned __int16 vertCount;
			unsigned __int16 triCount;
			char zoneHandle;
			unsigned __int16 baseTriIndex;
			unsigned __int16 baseVertIndex;
			unsigned __int16* triIndices;
			XSurfaceVertexInfo vertInfo;
			GfxPackedVertex* verts0;
			unsigned int vertListCount;
			XRigidVertList* vertList;
			int partBits[4];
		};

		struct XModelLodInfo
		{
			float dist;
			unsigned __int16 numsurfs;
			unsigned __int16 surfIndex;
			int partBits[4];
			char lod;
			char smcIndexPlusOne;
			char smcAllocBits;
			char unused;
		};

		struct XModelCollTri_s
		{
			float plane[4];
			float svec[4];
			float tvec[4];
		};

		struct XModelCollSurf_s
		{
			XModelCollTri_s* collTris;
			int numCollTris;
			float mins[3];
			float maxs[3];
			int boneIdx;
			int contents;
			int surfFlags;
		};
		
		struct XBoneInfo
		{
			float bounds[2][3];
			float offset[3];
			float radiusSquared;
		};

		struct XModelHighMipBounds
		{
			float mins[3];
			float maxs[3];
		};

		struct XModelStreamInfo
		{
			XModelHighMipBounds* highMipBounds;
		};

		struct PhysMass
		{
			float centerOfMass[3];
			float momentsOfInertia[3];
			float productsOfInertia[3];
		};

		struct cplane_s
		{
			float normal[3];
			float dist;
			char type;
			char signbits;
			char pad[2];
		};

#pragma pack(push, 2)
		struct cbrushside_t
		{
			cplane_s* plane;
			unsigned int materialNum;
			__int16 firstAdjacentSideOffset;
			char edgeCount;
		};
#pragma pack(pop)

		struct BrushWrapper
		{
			float mins[3];
			int contents;
			float maxs[3];
			unsigned int numsides;
			cbrushside_t* sides;
			__int16 axialMaterialNum[2][3];
			char* baseAdjacentSide;
			__int16 firstAdjacentSideOffsets[2][3];
			char edgeCount[2][3];
			int totalEdgeCount;
			cplane_s* planes;
		};

		struct PhysGeomInfo
		{
			BrushWrapper* brush;
			int type;
			float orientation[3][3];
			float offset[3];
			float halfLengths[3];
		};

		struct PhysGeomList
		{
			unsigned int count;
			PhysGeomInfo* geoms;
			PhysMass mass;
		};

#pragma pack(push, 4)
		struct PhysPreset
		{
			const char* name;
			int type;
			float mass;
			float bounce;
			float friction;
			float bulletForceScale;
			float explosiveForceScale;
			const char* sndAliasPrefix;
			float piecesSpreadFraction;
			float piecesUpwardVelocity;
			char tempDefaultToCylinder;
		};
#pragma pack(pop)

		struct XModel
		{
			const char* name;
			char numBones;
			char numRootBones;
			unsigned char numsurfs;
			char lodRampType;
			unsigned __int16* boneNames;
			char* parentList;
			__int16* quats;
			float* trans;
			char* partClassification;
			DObjAnimMat* baseMat;
			XSurface* surfs;
			Material** materialHandles;
			XModelLodInfo lodInfo[4];
			XModelCollSurf_s* collSurfs;
			int numCollSurfs;
			int contents;
			XBoneInfo* boneInfo;
			float radius;
			float mins[3];
			float maxs[3];
			__int16 numLods;
			__int16 collLod;
			XModelStreamInfo streamInfo;
			int memUsage;
			char flags;
			bool bad;
			PhysPreset* physPreset;
			PhysGeomList* physGeoms;
		};

		union XAnimIndices
		{
			char* _1;
			unsigned __int16* _2;
			void* data;
		};

		union XAnimDynamicFrames
		{
			char(*_1)[3];
			unsigned __int16(*_2)[3];
		};

		union XAnimDynamicIndices
		{
			char _1[1];
			unsigned __int16 _2[1];
		};

#pragma pack(push, 4)
		struct XAnimPartTransFrames
		{
			float mins[3];
			float size[3];
			XAnimDynamicFrames frames;
			XAnimDynamicIndices indices;
		};

		union XAnimPartTransData
		{
			XAnimPartTransFrames frames;
			float frame0[3];
		};

		struct XAnimPartTrans
		{
			unsigned __int16 size;
			char smallTrans;
			__declspec(align(2)) XAnimPartTransData u;
		};

		struct XAnimDeltaPartQuatDataFrames
		{
			__int16(*frames)[2];
			XAnimDynamicIndices indices;
		};

		union XAnimDeltaPartQuatData
		{
			XAnimDeltaPartQuatDataFrames frames;
			__int16 frame0[2];
		};

		struct XAnimDeltaPartQuat
		{
			unsigned __int16 size;
			__declspec(align(4)) XAnimDeltaPartQuatData u;
		};

		struct XAnimDeltaPart
		{
			XAnimPartTrans* trans;
			XAnimDeltaPartQuat* quat;
		};

		struct XAnimNotifyInfo
		{
			ScriptString name;
			float time;
		};

#ifdef __cplusplus
		enum XAnimPartType
		{
			PART_TYPE_NO_QUAT = 0x0,
			PART_TYPE_SIMPLE_QUAT = 0x1,
			PART_TYPE_NORMAL_QUAT = 0x2,
			PART_TYPE_PRECISION_QUAT = 0x3,
			PART_TYPE_SIMPLE_QUAT_NO_SIZE = 0x4,
			PART_TYPE_NORMAL_QUAT_NO_SIZE = 0x5,
			PART_TYPE_PRECISION_QUAT_NO_SIZE = 0x6,
			PART_TYPE_SMALL_TRANS = 0x7,
			PART_TYPE_TRANS = 0x8,
			PART_TYPE_TRANS_NO_SIZE = 0x9,
			PART_TYPE_NO_TRANS = 0xA,
			PART_TYPE_ALL = 0xB,
			PART_TYPE_COUNT = 0xC,
		};
#endif

#pragma pack(pop)

		struct XAnimParts
		{
			const char* name;
			unsigned __int16 dataByteCount;
			unsigned __int16 dataShortCount;
			unsigned __int16 dataIntCount;
			unsigned __int16 randomDataByteCount;
			unsigned __int16 randomDataIntCount;
			unsigned __int16 numframes;
			char bLoop;
			char bDelta;
			char boneCount[12];
			char notifyCount;
			char assetType;
			unsigned int randomDataShortCount;
			unsigned int indexCount;
			float framerate;
			float frequency;
			ScriptString* names;
			char* dataByte;
			__int16* dataShort;
			int* dataInt;
			__int16* randomDataShort;
			char* randomDataByte;
			int* randomDataInt;
			XAnimIndices indices;
			XAnimNotifyInfo* notify;
			XAnimDeltaPart* deltaPart;
		};

		struct score_t
		{
			int client;
			int score;
			int ping;
			int deaths;
			int team;
			int kills;
			int rank;
			int assists;
			Material* hStatusIcon;
			Material* hRankIcon;
		};

		enum InvalidCmdHintType
		{
			INVALID_CMD_NONE = 0x0,
			INVALID_CMD_NO_AMMO_BULLETS = 0x1,
			INVALID_CMD_NO_AMMO_FRAG_GRENADE = 0x2,
			INVALID_CMD_NO_AMMO_SPECIAL_GRENADE = 0x3,
			INVALID_CMD_NO_AMMO_FLASH_GRENADE = 0x4,
			INVALID_CMD_STAND_BLOCKED = 0x5,
			INVALID_CMD_CROUCH_BLOCKED = 0x6,
			INVALID_CMD_TARGET_TOO_CLOSE = 0x7,
			INVALID_CMD_LOCKON_REQUIRED = 0x8,
			INVALID_CMD_NOT_ENOUGH_CLEARANCE = 0x9,
		};

		struct viewDamage_t
		{
			int time;
			int duration;
			float yaw;
		};

		struct $F6DFD6D87F75480A1EF1906639406DF5
		{
			int time;
			int duration;
		};

		struct shellshock_t
		{
			void* parms; //shellshock_parms_t
			int startTime;
			int duration;
			int loopEndTime;
			float sensitivity;
			float viewDelta[2];
			int hasSavedScreen;
		};

		struct SprintState
		{
			int sprintButtonUpRequired;
			int sprintDelay;
			int lastSprintStart;
			int lastSprintEnd;
			int sprintStartMaxLength;
		};

		struct animScriptCommand_t
		{
			__int16 bodyPart[2];
			__int16 animIndex[2];
			__int16 animDuration[2];
			void* soundAlias;
		};

		struct animScriptCondition_t
		{
			int index;
			unsigned int value[2];
		};

		struct animScriptItem_t
		{
			int numConditions;
			animScriptCondition_t conditions[10];
			int numCommands;
			animScriptCommand_t commands[8];
		};

		struct animScript_t
		{
			int numItems;
			animScriptItem_t* items[128];
		};

		struct __declspec(align(8)) animation_s
		{
			char name[64];
			int initialLerp;
			float moveSpeed;
			int duration;
			int nameHash;
			int flags;
			__int64 movetype;
			int noteType;
		};

		struct scr_animtree_t
		{
			void* anims; //XAnim_s
		};

		struct $0867E0FC4F8157A276DAB76B1612E229
		{
			scr_animtree_t tree;
			int torso; //scr_anim_s
			int legs; //scr_anim_s
			int turning; //scr_anim_s
		};

		struct __declspec(align(8)) animScriptData_t
		{
			animation_s animations[512];
			unsigned int numAnimations;
			animScript_t scriptAnims[1][43];
			animScript_t scriptCannedAnims[1][43];
			animScript_t scriptStateChange[1][1];
			animScript_t scriptEvents[21];
			animScriptItem_t scriptItems[2048];
			int numScriptItems;
			scr_animtree_t animTree;
			unsigned __int16 torsoAnim;
			unsigned __int16 legsAnim;
			unsigned __int16 turningAnim;
			void* (__cdecl* soundAlias)(const char*);
			int(__cdecl* playSoundAlias)(int, void*);
		};

		struct lerpFrame_t
		{
			float yawAngle;
			int yawing;
			float pitchAngle;
			int pitching;
			int animationNumber;
			void* animation; //animation_s
			int animationTime;
			float oldFramePos[3];
			float animSpeedScale;
			int oldFrameSnapshotTime;
		};

		struct clientControllers_t
		{
			float angles[6][3];
			float tag_origin_angles[3];
			float tag_origin_offset[3];
		};

		struct __declspec(align(4)) clientInfo_t
		{
			int infoValid;
			int nextValid;
			int clientNum;
			char name[16];
			team_t team;
			team_t oldteam;
			int rank;
			int prestige;
			int perks;
			int score;
			int location;
			int health;
			char model[64];
			char attachModelNames[6][64];
			char attachTagNames[6][64];
			lerpFrame_t legs;
			lerpFrame_t torso;
			float lerpMoveDir;
			float lerpLean;
			float playerAngles[3];
			int leftHandGun;
			int dobjDirty;
			clientControllers_t control;
			unsigned int clientConditions[10][2];
			void* pXAnimTree; //XAnimTree_s
			int iDObjWeapon;
			char weaponModel;
			int stanceTransitionTime;
			int turnAnimEndTime;
			char turnAnimType;
			int attachedVehEntNum;
			int attachedVehSlotIndex;
			bool hideWeapon;
			bool usingKnife;
		};

		struct bgs_t
		{
			animScriptData_t animScriptData;
			$0867E0FC4F8157A276DAB76B1612E229 generic_human;
			int time;
			int latestSnapshotTime;
			int frametime;
			int anim_user;
			XModel* (__cdecl* GetXModel)(const char*);
			void* CreateDObj; //void(__cdecl* CreateDObj)(DObjModel_s*, unsigned __int16, XAnimTree_s*, int, int, clientInfo_t*);
			unsigned __int16 AttachWeapon; //unsigned __int16 (__cdecl *AttachWeapon)(DObjModel_s *, unsigned __int16, clientInfo_t *);
			void* DObj; //DObj_s *(__cdecl *GetDObj)(int, int);
			void(__cdecl* SafeDObjFree)(int, int);
			void* (__cdecl* AllocXAnim)(int);
			clientInfo_t clientinfo[64];
		};

		struct visionSetVars_t
		{
			bool glowEnable;
			float glowBloomCutoff;
			float glowBloomDesaturation;
			float glowBloomIntensity0;
			float glowBloomIntensity1;
			float glowRadius0;
			float glowRadius1;
			float glowSkyBleedIntensity0;
			float glowSkyBleedIntensity1;
			bool filmEnable;
			float filmBrightness;
			float filmContrast;
			float filmDesaturation;
			bool filmInvert;
			float filmLightTint[3];
			float filmDarkTint[3];
		};

		enum visionSetLerpStyle_t
		{
			VISIONSETLERP_UNDEFINED = 0x0,
			VISIONSETLERP_NONE = 0x1,
			VISIONSETLERP_TO_LINEAR = 0x2,
			VISIONSETLERP_TO_SMOOTH = 0x3,
			VISIONSETLERP_BACKFORTH_LINEAR = 0x4,
			VISIONSETLERP_BACKFORTH_SMOOTH = 0x5,
		};

		struct visionSetLerpData_t
		{
			int timeStart;
			int timeDuration;
			visionSetLerpStyle_t style;
		};

		struct $BE9F66374A020A9809EEAF403416A176
		{
			float aimSpreadScale;
		};

		struct hudElemSoundInfo_t
		{
			int lastPlayedTime;
		};

		enum OffhandSecondaryClass
		{
			PLAYER_OFFHAND_SECONDARY_SMOKE = 0x0,
			PLAYER_OFFHAND_SECONDARY_FLASH = 0x1,
			PLAYER_OFFHAND_SECONDARIES_TOTAL = 0x2,
		};

		enum ViewLockTypes
		{
			PLAYERVIEWLOCK_NONE = 0x0,
			PLAYERVIEWLOCK_FULL = 0x1,
			PLAYERVIEWLOCK_WEAPONJITTER = 0x2,
			PLAYERVIEWLOCKCOUNT = 0x3,
		};

		enum ActionSlotType
		{
			ACTIONSLOTTYPE_DONOTHING = 0x0,
			ACTIONSLOTTYPE_SPECIFYWEAPON = 0x1,
			ACTIONSLOTTYPE_ALTWEAPONTOGGLE = 0x2,
			ACTIONSLOTTYPE_NIGHTVISION = 0x3,
			ACTIONSLOTTYPECOUNT = 0x4,
		};

		/* 827 */
		struct MantleState
		{
			float yaw;
			int timer;
			int transIndex;
			int flags;
		};

		struct ActionSlotParam_SpecifyWeapon
		{
			unsigned int index;
		};

		struct ActionSlotParam
		{
			ActionSlotParam_SpecifyWeapon specifyWeapon;
		};

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
			int icon;
		};

		enum he_type_t
		{
			HE_TYPE_FREE = 0x0,
			HE_TYPE_TEXT = 0x1,
			HE_TYPE_VALUE = 0x2,
			HE_TYPE_PLAYERNAME = 0x3,
			HE_TYPE_MAPNAME = 0x4,
			HE_TYPE_GAMETYPE = 0x5,
			HE_TYPE_MATERIAL = 0x6,
			HE_TYPE_TIMER_DOWN = 0x7,
			HE_TYPE_TIMER_UP = 0x8,
			HE_TYPE_TENTHS_TIMER_DOWN = 0x9,
			HE_TYPE_TENTHS_TIMER_UP = 0xA,
			HE_TYPE_CLOCK_DOWN = 0xB,
			HE_TYPE_CLOCK_UP = 0xC,
			HE_TYPE_WAYPOINT = 0xD,
			HE_TYPE_COUNT = 0xE,
		};

		struct $C96EA5EC2ACBB9C0BF22693F316ACC67
		{
			char r;
			char g;
			char b;
			char a;
		};

		union hudelem_color_t
		{
			$C96EA5EC2ACBB9C0BF22693F316ACC67 __s0;
			int rgba;
		};

		struct hudelem_s
		{
			he_type_t type;
			float x;
			float y;
			float z;
			int targetEntNum;
			float fontScale;
			int font;
			int alignOrg;
			int alignScreen;
			hudelem_color_t color;
			hudelem_color_t fromColor;
			int fadeStartTime;
			int fadeTime;
			int label;
			int width;
			int height;
			int materialIndex;
			int offscreenMaterialIdx;
			int fromWidth;
			int fromHeight;
			int scaleStartTime;
			int scaleTime;
			float fromX;
			float fromY;
			int fromAlignOrg;
			int fromAlignScreen;
			int moveStartTime;
			int moveTime;
			int time;
			int duration;
			float value;
			int text;
			float sort;
			hudelem_color_t glowColor;
			int fxBirthTime;
			int fxLetterTime;
			int fxDecayStartTime;
			int fxDecayDuration;
			int soundID;
			int flags;
		};

		struct $15067B6A14D88D7E1E730369692C3A81
		{
			hudelem_s current[31];
			hudelem_s archival[31];
		};

		struct playerState_s
		{
			int commandTime;
			int pm_type;
			int bobCycle;
			int pm_flags;
			int weapFlags;
			int otherFlags;
			int pm_time;
			float origin[3];
			float velocity[3];
			float oldVelocity[2];
			int weaponTime;
			int weaponDelay;
			int grenadeTimeLeft;
			int throwBackGrenadeOwner;
			int throwBackGrenadeTimeLeft;
			int weaponRestrictKickTime;
			int foliageSoundTime;
			int gravity;
			float leanf;
			int speed;
			float delta_angles[3];
			int groundEntityNum;
			float vLadderVec[3];
			int jumpTime;
			float jumpOriginZ;
			int legsTimer;
			int legsAnim;
			int torsoTimer;
			int torsoAnim;
			int legsAnimDuration;
			int torsoAnimDuration;
			int damageTimer;
			int damageDuration;
			int flinchYawAnim;
			int movementDir;
			int eFlags;
			int eventSequence;
			int events[4];
			unsigned int eventParms[4];
			int oldEventSequence;
			int clientNum;
			int offHandIndex;
			OffhandSecondaryClass offhandSecondary;
			unsigned int weapon;
			int weaponstate;
			unsigned int weaponShotCount;
			float fWeaponPosFrac;
			int adsDelayTime;
			int spreadOverride;
			int spreadOverrideState;
			int viewmodelIndex;
			float viewangles[3];
			int viewHeightTarget;
			float viewHeightCurrent;
			int viewHeightLerpTime;
			int viewHeightLerpTarget;
			int viewHeightLerpDown;
			float viewAngleClampBase[2];
			float viewAngleClampRange[2];
			int damageEvent;
			int damageYaw;
			int damagePitch;
			int damageCount;
			int stats[5];
			int ammo[128];
			int ammoclip[128];
			unsigned int weapons[4];
			unsigned int weaponold[4];
			unsigned int weaponrechamber[4];
			float proneDirection;
			float proneDirectionPitch;
			float proneTorsoPitch;
			ViewLockTypes viewlocked;
			int viewlocked_entNum;
			int cursorHint;
			int cursorHintString;
			int cursorHintEntIndex;
			int iCompassPlayerInfo;
			int radarEnabled;
			int locationSelectionInfo;
			SprintState sprintState;
			float fTorsoPitch;
			float fWaistPitch;
			float holdBreathScale;
			int holdBreathTimer;
			float moveSpeedScaleMultiplier;
			MantleState mantleState;
			float meleeChargeYaw;
			int meleeChargeDist;
			int meleeChargeTime;
			int perks;
			ActionSlotType actionSlotType[4];
			ActionSlotParam actionSlotParam[4];
			int entityEventSequence;
			int weapAnim;
			float aimSpreadScale;
			int shellshockIndex;
			int shellshockTime;
			int shellshockDuration;
			float dofNearStart;
			float dofNearEnd;
			float dofFarStart;
			float dofFarEnd;
			float dofNearBlur;
			float dofFarBlur;
			float dofViewmodelStart;
			float dofViewmodelEnd;
			int hudElemLastAssignedSoundID;
			objective_t objective[16];
			char weaponmodels[128];
			int deltaTime;
			int killCamEntity;
			$15067B6A14D88D7E1E730369692C3A81 hud;
		};

		struct snapshot_s
		{
			int snapFlags;
			int ping;
			int serverTime;
			playerState_s ps;
			int numEntities;
			int numClients;
			entityState_s entities[512];
			clientState_s clients[64];
			int serverCommandSequence;
		};

		struct cg_s
		{
			int clientNum;
			int localClientNum;
			DemoType demoType;
			CubemapShot cubemapShot;
			int cubemapSize;
			int renderScreen;
			int latestSnapshotNum;
			int latestSnapshotTime;
			snapshot_s* snap;
			snapshot_s* nextSnap;
			snapshot_s activeSnapshots[2];
			float frameInterpolation;
			int frametime;
			int time;
			int oldTime;
			int physicsTime;
			int mapRestart;
			int renderingThirdPerson;
			playerState_s predictedPlayerState;
			centity_s predictedPlayerEntity;
			playerEntity_t playerEntity;
			int predictedErrorTime;
			float predictedError[3];
			float landChange;
			int landTime;
			float heightToCeiling;
			refdef_s refdef;
			float refdefViewAngles[3];
			float lastVieworg[3];
			float swayViewAngles[3];
			float swayAngles[3];
			float swayOffset[3];
			int iEntityLastType[1024];
			XModel* pEntityLastXModel[1024];
			float zoomSensitivity;
			bool isLoading;
			char objectiveText[1024];
			char scriptMainMenu[256];
			int scoresRequestTime;
			int numScores;
			int teamScores[4];
			int teamPings[4];
			int teamPlayers[4];
			score_t scores[64];
			int scoreLimit;
			int showScores;
			int scoreFadeTime;
			int scoresTop;
			int scoresOffBottom;
			int scoresBottom;
			int drawHud;
			int crosshairClientNum;
			int crosshairClientLastTime;
			int crosshairClientStartTime;
			int identifyClientNum;
			int cursorHintIcon;
			int cursorHintTime;
			int cursorHintFade;
			int cursorHintString;
			int lastClipFlashTime;
			InvalidCmdHintType invalidCmdHintType;
			int invalidCmdHintTime;
			int lastHealthPulseTime;
			int lastHealthLerpDelay;
			int lastHealthClient;
			float lastHealth;
			float healthOverlayFromAlpha;
			float healthOverlayToAlpha;
			int healthOverlayPulseTime;
			int healthOverlayPulseDuration;
			int healthOverlayPulsePhase;
			bool healthOverlayHurt;
			int healthOverlayLastHitTime;
			float healthOverlayOldHealth;
			int healthOverlayPulseIndex;
			int proneBlockedEndTime;
			int lastStance;
			int lastStanceChangeTime;
			int lastStanceFlashTime;
			int voiceTime;
			unsigned int weaponSelect;
			int weaponSelectTime;
			unsigned int weaponLatestPrimaryIdx;
			int prevViewmodelWeapon;
			int equippedOffHand;
			viewDamage_t viewDamage[8];
			int damageTime;
			float damageX;
			float damageY;
			float damageValue;
			float viewFade;
			int weapIdleTime;
			int nomarks;
			int v_dmg_time;
			float v_dmg_pitch;
			float v_dmg_roll;
			float fBobCycle;
			float xyspeed;
			float kickAVel[3];
			float kickAngles[3];
			float offsetAngles[3];
			float gunPitch;
			float gunYaw;
			float gunXOfs;
			float gunYOfs;
			float gunZOfs;
			float vGunOffset[3];
			float vGunSpeed[3];
			float viewModelAxis[4][3];
			float rumbleScale;
			float compassNorthYaw;
			float compassNorth[2];
			Material* compassMapMaterial;
			float compassMapUpperLeft[2];
			float compassMapWorldSize[2];
			int compassFadeTime;
			int healthFadeTime;
			int ammoFadeTime;
			int stanceFadeTime;
			int sprintFadeTime;
			int offhandFadeTime;
			int offhandFlashTime;
			shellshock_t shellshock;
			$F6DFD6D87F75480A1EF1906639406DF5 testShock;
			int holdBreathTime;
			int holdBreathInTime;
			int holdBreathDelay;
			float holdBreathFrac;
			float radarProgress;		// correct offset
			float selectedLocation[2];
			SprintState sprintStates;	// 5 ints
			int _unk01;
			int _unk02;
			bgs_t bgs;	// first name root
			cpose_t viewModelPose;
			visionSetVars_t visionSetPreLoaded[4];
			char visionSetPreLoadedName[4][64];
			visionSetVars_t visionSetFrom[2];
			visionSetVars_t visionSetTo[2];
			visionSetVars_t visionSetCurrent[2];
			visionSetLerpData_t visionSetLerpData[2];
			char visionNameNaked[64];
			char visionNameNight[64];
			int extraButtons;
			int lastActionSlotTime;
			bool playerTeleported;
			int stepViewStart;
			float stepViewChange;
			$BE9F66374A020A9809EEAF403416A176 lastFrame;
			hudElemSoundInfo_t hudElemSound[32];
			int vehicleFrame;
		}; // should be right

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

		enum netadrtype_t {
			NA_BAD = 0,					// an address lookup failed
			NA_BOT = 0,
			NA_LOOPBACK = 2,
			NA_BROADCAST = 3,
			NA_IP = 4,
			NA_IP6 = 5,
			NA_TCP = 6,
			NA_TCP6 = 7,
			NA_MULTICAST6 = 8,
			NA_UNSPEC = 9,
			NA_DOWN = 10,
		};

		struct netadr_t
		{
			netadrtype_t type;
			unsigned char ip[4];
			unsigned __int16 port;
			char ipx[10];
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
			const char* binding;
		};

		struct PlayerKeyState
		{
			/*field_t chatField;*/
			int chat_team;			// 0x8F1DB8
			int overstrikeMode;
			int anyKeyDown;
			KeyState keys[256];
			LocSelInputState locSelInputState;
		};

		enum XAssetType
		{
			ASSET_TYPE_XMODELPIECES = 0x0,
			ASSET_TYPE_PHYSPRESET = 0x1,
			ASSET_TYPE_XANIMPARTS = 0x2,
			ASSET_TYPE_XMODEL = 0x3,
			ASSET_TYPE_MATERIAL = 0x4,
			ASSET_TYPE_TECHNIQUE_SET = 0x5,
			ASSET_TYPE_IMAGE = 0x6,
			ASSET_TYPE_SOUND = 0x7,
			ASSET_TYPE_SOUND_CURVE = 0x8,
			ASSET_TYPE_LOADED_SOUND = 0x9,
			ASSET_TYPE_CLIPMAP = 0xA,
			ASSET_TYPE_CLIPMAP_PVS = 0xB,
			ASSET_TYPE_COMWORLD = 0xC,
			ASSET_TYPE_GAMEWORLD_SP = 0xD,
			ASSET_TYPE_GAMEWORLD_MP = 0xE,
			ASSET_TYPE_MAP_ENTS = 0xF,
			ASSET_TYPE_GFXWORLD = 0x10,
			ASSET_TYPE_LIGHT_DEF = 0x11,
			ASSET_TYPE_UI_MAP = 0x12,
			ASSET_TYPE_FONT = 0x13,
			ASSET_TYPE_MENULIST = 0x14,
			ASSET_TYPE_MENU = 0x15,
			ASSET_TYPE_LOCALIZE_ENTRY = 0x16,
			ASSET_TYPE_WEAPON = 0x17,
			ASSET_TYPE_SNDDRIVER_GLOBALS = 0x18,
			ASSET_TYPE_FX = 0x19,
			ASSET_TYPE_IMPACT_FX = 0x1A,
			ASSET_TYPE_AITYPE = 0x1B,
			ASSET_TYPE_MPTYPE = 0x1C,
			ASSET_TYPE_CHARACTER = 0x1D,
			ASSET_TYPE_XMODELALIAS = 0x1E,
			ASSET_TYPE_RAWFILE = 0x1F,
			ASSET_TYPE_STRINGTABLE = 0x20,
			ASSET_TYPE_COUNT = 0x21,
			ASSET_TYPE_STRING = 0x21,
			ASSET_TYPE_ASSETLIST = 0x22,
			ASSET_TYPE_INVALID = -1,
		};

#pragma pack(push, 16)
		struct cbrush_t
		{
			float mins[3];
			int contents;
			float maxs[3];
			unsigned int numsides;
			cbrushside_t* sides;
			__int16 axialMaterialNum[2][3];
			char* baseAdjacentSide;
			__int16 firstAdjacentSideOffsets[2][3];
			char edgeCount[2][3];
			__int16 colorCounter;
			__int16 cmBrushIndex;
			//float distFromCam;
			__int16 cmSubmodelIndex;
			bool isSubmodel;
			bool pad;
		};
#pragma pack(pop)

		struct cStaticModelWritable
		{
			unsigned __int16 nextModelInWorldSector;
		};

		struct cStaticModel_s
		{
			cStaticModelWritable writable;
			XModel* xmodel;
			float origin[3];
			float invScaledAxis[3][3];
			float absmin[3];
			float absmax[3];
		};

		struct dmaterial_t
		{
			char material[64];
			int surfaceFlags;
			int contentFlags;
		};

		struct cNode_t
		{
			cplane_s* plane;
			__int16 children[2];
		};

#pragma pack(push, 4)
		struct cLeaf_t
		{
			unsigned __int16 firstCollAabbIndex;
			unsigned __int16 collAabbCount;
			int brushContents;
			int terrainContents;
			float mins[3];
			float maxs[3];
			int leafBrushNode;
			__int16 cluster;
		};
#pragma pack(pop)

		struct cLeafBrushNodeLeaf_t
		{
			unsigned __int16* brushes;
		};

		struct cLeafBrushNodeChildren_t
		{
			float dist;
			float range;
			unsigned __int16 childOffset[2];
		};

		union cLeafBrushNodeData_t
		{
			cLeafBrushNodeLeaf_t leaf;
			cLeafBrushNodeChildren_t children;
		};

		struct cLeafBrushNode_s
		{
			char axis;
			__int16 leafBrushCount;
			int contents;
			cLeafBrushNodeData_t data;
		};

		struct CollisionBorder
		{
			float distEq[3];
			float zBase;
			float zSlope;
			float start;
			float length;
		};

		struct CollisionPartition
		{
			char triCount;
			char borderCount;
			int firstTri;
			CollisionBorder* borders;
		};

		union CollisionAabbTreeIndex
		{
			int firstChildIndex;
			int partitionIndex;
		};

		struct CollisionAabbTree
		{
			float origin[3];
			float halfSize[3];
			unsigned __int16 materialIndex;
			unsigned __int16 childCount;
			CollisionAabbTreeIndex u;
		};

		struct cmodel_t
		{
			float mins[3];
			float maxs[3];
			float radius;
			cLeaf_t leaf;
		};

		struct Bounds
		{
			vec3_t midPoint;
			vec3_t halfSize;
		};

		struct TriggerModel
		{
			int contents;
			unsigned __int16 hullCount;
			unsigned __int16 firstHull;
		};

		/* 2376 */
		struct TriggerHull
		{
			Bounds bounds;
			int contents;
			unsigned __int16 slabCount;
			unsigned __int16 firstSlab;
		};

		struct TriggerSlab
		{
			float dir[3];
			float midPoint;
			float halfSize;
		};

		/* 2378 */
		struct MapTriggers
		{
			unsigned int count;
			TriggerModel* models;
			unsigned int hullCount;
			TriggerHull* hulls;
			unsigned int slabCount;
			TriggerSlab* slabs;
		};

		struct MapEnts
		{
			const char* name;
			char* entityString;
			int numEntityChars;
			MapTriggers trigger;
			// this goes on for a while but we don't need any of it
		};

		struct GfxPlacement
		{
			float quat[4];
			float origin[3];
		};

		struct FxEffectDef_Placeholder
		{
			const char* name;
		};

		struct DynEntityDef
		{
			int type;
			GfxPlacement pose;
			XModel* xModel;
			unsigned __int16 brushModel;
			unsigned __int16 physicsBrushModel;
			FxEffectDef_Placeholder* destroyFx;
			/*XModelPieces*/ void* destroyPieces;
			PhysPreset* physPreset;
			int health;
			PhysMass mass;
			int contents;
		};

		struct clipMap_t
		{
			const char* name;
			int isInUse;
			int planeCount;
			cplane_s* planes;
			unsigned int numStaticModels;
			cStaticModel_s* staticModelList;
			unsigned int numMaterials;
			dmaterial_t* materials;
			unsigned int numBrushSides;
			cbrushside_t* brushsides;
			unsigned int numBrushEdges;
			char* brushEdges;
			unsigned int numNodes;
			cNode_t* nodes;
			unsigned int numLeafs;
			cLeaf_t* leafs;
			unsigned int leafbrushNodesCount;
			cLeafBrushNode_s* leafbrushNodes;
			unsigned int numLeafBrushes;
			unsigned __int16* leafbrushes;
			unsigned int numLeafSurfaces;
			unsigned int* leafsurfaces;
			unsigned int vertCount;
			float(*verts)[3];
			int triCount;
			unsigned __int16* triIndices;
			char* triEdgeIsWalkable;
			int borderCount;
			CollisionBorder* borders;
			int partitionCount;
			CollisionPartition* partitions;
			int aabbTreeCount;
			CollisionAabbTree* aabbTrees;
			unsigned int numSubModels;
			cmodel_t* cmodels;
			unsigned __int16 numBrushes;
			cbrush_t* brushes;
			int numClusters;
			int clusterBytes;
			char* visibility;
			int vised;
			MapEnts* mapEnts;
			cbrush_t* box_brush;
			cmodel_t box_model;
			unsigned __int16 dynEntCount[2];
			DynEntityDef* dynEntDefList[2];
			/*DynEntityPose*/ void* dynEntPoseList[2];
			/*DynEntityClient*/ void* dynEntClientList[2];
			/*DynEntityColl*/ void* dynEntCollList[2];
			unsigned int checksum;
		};

		struct RawFile
		{
			const char* name;
			int len;
			const char* buffer;
		};

		struct ComPrimaryLight
		{
			char type;
			char canUseShadowMap;
			char exponent;
			char unused;
			float color[3];
			float dir[3];
			float origin[3];
			float radius;
			float cosHalfFovOuter;
			float cosHalfFovInner;
			float cosHalfFovExpanded;
			float rotationLimit;
			float translationLimit;
			const char* defName;
		};

		struct ComWorld
		{
			const char* name;
			int isInUse;
			unsigned int primaryLightCount;
			ComPrimaryLight* primaryLights;
		};

		struct pathnode_t;

		/* 875 */
		struct pathnode_transient_t
		{
			int iSearchFrame;
			pathnode_t* pNextOpen;
			pathnode_t* pPrevOpen;
			pathnode_t* pParent;
			float fCost;
			float fHeuristic;
			float costFactor;
		};

		struct pathlink_s
		{
			float fDist;
			unsigned __int16 nodeNum;
			char disconnectCount;
			char negotiationLink;
			char ubBadPlaceCount[4];
		};

		enum nodeType
		{
			NODE_BADNODE = 0x0,
			NODE_PATHNODE = 0x1,
			NODE_COVER_STAND = 0x2,
			NODE_COVER_CROUCH = 0x3,
			NODE_COVER_CROUCH_WINDOW = 0x4,
			NODE_COVER_PRONE = 0x5,
			NODE_COVER_RIGHT = 0x6,
			NODE_COVER_LEFT = 0x7,
			NODE_COVER_WIDE_RIGHT = 0x8,
			NODE_COVER_WIDE_LEFT = 0x9,
			NODE_CONCEALMENT_STAND = 0xA,
			NODE_CONCEALMENT_CROUCH = 0xB,
			NODE_CONCEALMENT_PRONE = 0xC,
			NODE_REACQUIRE = 0xD,
			NODE_BALCONY = 0xE,
			NODE_SCRIPTED = 0xF,
			NODE_NEGOTIATION_BEGIN = 0x10,
			NODE_NEGOTIATION_END = 0x11,
			NODE_TURRET = 0x12,
			NODE_GUARD = 0x13,
			NODE_NUMTYPES = 0x14,
			NODE_DONTLINK = 0x14,
		};

		/* 872 */
		struct pathnode_constant_t
		{
			nodeType type;
			unsigned __int16 spawnflags;
			unsigned __int16 targetname;
			unsigned __int16 script_linkName;
			unsigned __int16 script_noteworthy;
			unsigned __int16 target;
			unsigned __int16 animscript;
			int animscriptfunc;
			float vOrigin[3];
			float fAngle;
			float forward[2];
			float fRadius;
			float minUseDistSq;
			__int16 wOverlapNode[2];
			__int16 wChainId;
			__int16 wChainDepth;
			__int16 wChainParent;
			unsigned __int16 totalLinkCount;
			pathlink_s* Links;
		};

		/* 873 */
		struct pathnode_dynamic_t
		{
			void* pOwner;
			int iFreeTime;
			int iValidTime[3];
			int inPlayerLOSTime;
			__int16 wLinkCount;
			__int16 wOverlapCount;
			__int16 turretEntNumber;
			__int16 userCount;
		};


		/* 874 */
		struct pathnode_t
		{
			pathnode_constant_t constant;
			pathnode_dynamic_t dynamic;
			pathnode_transient_t transient;
		};

		/* 876 */
		struct pathbasenode_t
		{
			float vOrigin[3];
			unsigned int type;
		};

		/* 878 */
		struct pathnode_tree_nodes_t
		{
			int nodeCount;
			unsigned __int16* nodes;
		};

		struct pathnode_tree_t;

		/* 879 */
		union pathnode_tree_info_t
		{
			pathnode_tree_t* child[2];
			pathnode_tree_nodes_t s;
		};

		/* 877 */
		struct pathnode_tree_t
		{
			int axis;
			float dist;
			pathnode_tree_info_t u;
		};

		/* 880 */
		struct PathData
		{
			unsigned int nodeCount;
			pathnode_t* nodes;
			pathbasenode_t* basenodes;
			unsigned int chainNodeCount;
			unsigned __int16* chainNodeForNode;
			unsigned __int16* nodeForChainNode;
			int visBytes;
			char* pathVis;
			int nodeTreeCount;
			pathnode_tree_t* nodeTree;
		};

		/* 881 */
		struct GameWorldSp
		{
			const char* name;
			PathData path;
		};

		struct RGBA_COLOR
		{
			float r, g, b, a;
		};

		struct sunflare_t
		{
			char hasValidData;
			Material* spriteMaterial;
			Material* flareMaterial;
			float spriteSize;
			float flareMinSize;
			float flareMinDot;
			float flareMaxSize;
			float flareMaxDot;
			float flareMaxAlpha;
			int flareFadeInTime;
			int flareFadeOutTime;
			float blindMinDot;
			float blindMaxDot;
			float blindMaxDarken;
			int blindFadeInTime;
			int blindFadeOutTime;
			float glareMinDot;
			float glareMaxDot;
			float glareMaxLighten;
			int glareFadeInTime;
			int glareFadeOutTime;
			float sunFxPosition[3];
		};

		struct XModelDrawInfo
		{
			unsigned __int16 lod;
			unsigned __int16 surfId;
		};

		struct GfxSceneDynModel
		{
			XModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct BModelDrawInfo
		{
			unsigned __int16 surfId;
		};

		struct GfxSceneDynBrush
		{
			BModelDrawInfo info;
			unsigned __int16 dynEntId;
		};

		struct GfxShadowGeometry
		{
			unsigned __int16 surfaceCount;
			unsigned __int16 smodelCount;
			unsigned __int16* sortedSurfIndex;
			unsigned __int16* smodelIndex;
		};

		struct GfxLightRegionAxis
		{
			float dir[3];
			float midPoint;
			float halfSize;
		};

		struct GfxLightRegionHull
		{
			float kdopMidPoint[9];
			float kdopHalfSize[9];
			unsigned int axisCount;
			GfxLightRegionAxis* axis;
		};

		struct GfxLightRegion
		{
			unsigned int hullCount;
			GfxLightRegionHull* hulls;
		};

		struct GfxStaticModelInst
		{
			float mins[3];
			float maxs[3];
			GfxColor groundLighting;
		};

		struct srfTriangles_t
		{
			int vertexLayerData;
			int firstVertex;
			unsigned __int16 vertexCount;
			unsigned __int16 triCount;
			int baseIndex;
		};

		struct GfxSurface
		{
			srfTriangles_t tris;
			Material* material;
			char lightmapIndex;
			char reflectionProbeIndex;
			char primaryLightIndex;
			char flags;
			float bounds[2][3];
		};

		struct GfxCullGroup
		{
			float mins[3];
			float maxs[3];
			int surfaceCount;
			int startSurfIndex;
		};

		struct GfxPackedPlacement
		{
			float origin[3];
			vec3_t axis[3];
			float scale;
		};

		struct __declspec(align(4)) GfxStaticModelDrawInst
		{
			float cullDist;
			GfxPackedPlacement placement;
			XModel* model;
			unsigned __int16 smodelCacheIndex[4];
			char reflectionProbeIndex;
			char primaryLightIndex;
			unsigned __int16 lightingHandle;
			char flags;
		};

		struct GfxWorldDpvsStatic
		{
			unsigned int smodelCount;
			unsigned int staticSurfaceCount;
			unsigned int staticSurfaceCountNoDecal;
			unsigned int litSurfsBegin;
			unsigned int litSurfsEnd;
			unsigned int decalSurfsBegin;
			unsigned int decalSurfsEnd;
			unsigned int emissiveSurfsBegin;
			unsigned int emissiveSurfsEnd;
			unsigned int smodelVisDataCount;
			unsigned int surfaceVisDataCount;
			char* smodelVisData[3];
			char* surfaceVisData[3];
			unsigned int* lodData;
			unsigned __int16* sortedSurfIndex;
			GfxStaticModelInst* smodelInsts;
			GfxSurface* surfaces;
			GfxCullGroup* cullGroups;
			GfxStaticModelDrawInst* smodelDrawInsts;
			GfxDrawSurf* surfaceMaterials;
			unsigned int* surfaceCastsSunShadow;
			volatile int usageCount;
		};

		struct GfxWorldDpvsDynamic
		{
			unsigned int dynEntClientWordCount[2];
			unsigned int dynEntClientCount[2];
			unsigned int* dynEntCellBits[2];
			char* dynEntVisData[2][3];
		};

		struct GfxWorldStreamInfo
		{
			int aabbTreeCount;
			// 			GfxStreamingAabbTree *aabbTrees;
			// 			int leafRefCount;
			// 			int *leafRefs;
		};

		struct GfxWorldVertex
		{
			float xyz[3];
			float binormalSign;
			GfxColor color;
			float texCoord[2];
			float lmapCoord[2];
			PackedUnitVec normal;
			PackedUnitVec tangent;
		};

		struct GfxStreamingAabbTree
		{
			unsigned __int16 firstItem;
			unsigned __int16 itemCount;
			unsigned __int16 firstChild;
			unsigned __int16 childCount;
			float mins[3];
			float maxs[3];
		};

		struct GfxWorldVertexData
		{
			GfxWorldVertex* vertices;
			void/*IDirect3DVertexBuffer9*/* worldVb;
		};

		struct GfxWorldVertexLayerData
		{
			char* data;
			void/*IDirect3DVertexBuffer9*/* layerVb;
		};

		struct SunLightParseParams
		{
			char name[64];
			float ambientScale;
			float ambientColor[3];
			float diffuseFraction;
			float sunLight;
			float sunColor[3];
			float diffuseColor[3];
			char diffuseColorHasBeenSet;
			float angles[3];
		};

		struct GfxReflectionProbe
		{
			float origin[3];
			GfxImage* reflectionImage;
		};

		struct DpvsPlane
		{
			float coeffs[4];
			char side[3];
			char pad;
		};

		struct GfxPortal;

		struct GfxPortalWritable
		{
			char isQueued;
			char isAncestor;
			char recursionDepth;
			char hullPointCount;
			float(*hullPoints)[2];
			GfxPortal* queuedParent;
		};

		struct GfxWorldDpvsPlanes
		{
			int cellCount;
			cplane_s* planes;
			unsigned __int16* nodes;
			unsigned int* sceneEntCellBits;
		};

		struct GfxCell;

		struct GfxPortal
		{
			GfxPortalWritable writable;
			DpvsPlane plane;
			GfxCell* cell;
			float(*vertices)[3];
			char vertexCount;
			float hullAxis[2][3];
		};

		struct GfxCell
		{
			float mins[3];
			float maxs[3];
			int aabbTreeCount;
			void* aabbTree;
			int portalCount;
			GfxPortal* portals;
			int cullGroupCount;
			int* cullGroups;
			char reflectionProbeCount;
			char* reflectionProbes;
		};

		struct GfxLightGridEntry
		{
			unsigned __int16 colorsIndex;
			char primaryLightIndex;
			char needsTrace;
		};

		struct GfxLightGridColors
		{
			char rgb[56][3];
		};

		struct GfxLightGrid
		{
			char hasLightRegions;
			unsigned int sunPrimaryLightIndex;
			unsigned __int16 mins[3];
			unsigned __int16 maxs[3];
			unsigned int rowAxis;
			unsigned int colAxis;
			unsigned __int16* rowDataStart;
			unsigned int rawRowDataSize;
			char* rawRowData;
			unsigned int entryCount;
			GfxLightGridEntry* entries;
			unsigned int colorCount;
			GfxLightGridColors* colors;
		};

		struct GfxBrushModelWritable
		{
			float mins[3];
			float maxs[3];
		};

		struct __declspec(align(4)) GfxBrushModel
		{
			GfxBrushModelWritable writable;
			float bounds[2][3];
			unsigned __int16 surfaceCount;
			unsigned __int16 startSurfIndex;
			unsigned __int16 surfaceCountNoDecal;
		};

		struct MaterialMemory
		{
			Material* material;
			int memory;
		};

		struct GfxWorld
		{
			const char* name;
			const char* baseName;
			int planeCount;
			int nodeCount;
			int indexCount;
			unsigned __int16* indices;
			int surfaceCount;
			GfxWorldStreamInfo streamInfo;
			int skySurfCount;
			int* skyStartSurfs;
			GfxImage* skyImage;
			char skySamplerState;
			unsigned int vertexCount;
			GfxWorldVertexData vd;
			unsigned int vertexLayerDataSize;
			GfxWorldVertexLayerData vld;
			SunLightParseParams sunParse;
			GfxLight* sunLight;
			float sunColorFromBsp[3];
			unsigned int sunPrimaryLightIndex;
			unsigned int primaryLightCount;
			int cullGroupCount;
			unsigned int reflectionProbeCount;
			GfxReflectionProbe* reflectionProbes;
			GfxTexture* reflectionProbeTextures;
			GfxWorldDpvsPlanes dpvsPlanes;
			int cellBitsCount;
			GfxCell* cells;
			int lightmapCount;
			/*GfxLightmapArray**/void* lightmaps;
			GfxLightGrid lightGrid;
			GfxTexture* lightmapPrimaryTextures;
			GfxTexture* lightmapSecondaryTextures;
			int modelCount;
			GfxBrushModel* models;
			float mins[3];
			float maxs[3];
			unsigned int checksum;
			int materialMemoryCount;
			MaterialMemory* materialMemory;
			sunflare_t sun;
			float outdoorLookupMatrix[4][4];
			GfxImage* outdoorImage;
			unsigned int* cellCasterBits;
			GfxSceneDynModel* sceneDynModel;
			GfxSceneDynBrush* sceneDynBrush;
			unsigned int* primaryLightEntityShadowVis;
			unsigned int* primaryLightDynEntShadowVis[2];
			char* nonSunPrimaryLightForModelDynEnt;
			GfxShadowGeometry* shadowGeom;
			GfxLightRegion* lightRegion;
			GfxWorldDpvsStatic dpvs;
			GfxWorldDpvsDynamic dpvsDyn;
		};

		typedef struct MenuList
		{
			const char* name;
			int menuCount;
			menuDef_t** menus;
		} MenuList;

		struct LocalizeEntry
		{
			const char* value;
			const char* name;
		};

		struct StringTableCell
		{
			const char* string;
			int hash;
		};

		typedef struct
		{
			const char* name;
			int columnCount;
			int rowCount;
			StringTableCell* values;
		}StringTable;

		typedef enum
		{
			SASYS_UI = 0x0,
			SASYS_CGAME = 0x1,
			SASYS_GAME = 0x2,
			SASYS_COUNT = 0x3,
		}snd_alias_system_t;


		typedef struct _AILSOUNDINFO
		{
			signed int format;
			const void* data_ptr;
			unsigned int data_len;
			unsigned int rate;
			signed int bits;
			signed int channels;
			unsigned int samples;
			unsigned int block_size;
			const void* initial_ptr;
		}AILSOUNDINFO;

		typedef struct MssSound_s
		{
			AILSOUNDINFO ailInfo;
			void* data;
		}MssSound_t;

		typedef struct LoadedSound_s
		{
			const char* name;
			MssSound_t sounds;
		}LoadedSound_t;


		typedef struct SoundFileInfo_s
		{
			const char* sndfilename;
			MssSound_t ailsoundinfo;
		}SoundFileInfo_t;


		typedef struct StreamFileNamePacked_s
		{
			unsigned __int64 offset;
			unsigned __int64 length;
		}StreamFileNamePacked_t;

		typedef struct StreamFileNameRaw_s
		{
			const char* dir;
			const char* name;
		}StreamFileNameRaw_t;

		typedef union StreamFileInfo_s
		{
			StreamFileNameRaw_t raw;
			StreamFileNamePacked_t packed;
		}StreamFileInfo_t;

		typedef struct StreamFileName_s
		{
			StreamFileInfo_t info;
			unsigned __int16 isLocalized;
			unsigned __int16 fileIndex;
		}StreamFileName_t;

		typedef struct StreamedSound_s
		{
			StreamFileName_t filename;
			unsigned int totalMsec;
		}StreamedSound_t;



		typedef union SoundFileRef_s
		{
			LoadedSound_t* loadSnd;
			StreamedSound_t streamSnd;
		}SoundFileRef_t;


		typedef struct SoundFile_s
		{
			byte type;
			byte pad2[3];
			SoundFileRef_t sound;
			byte exists;
			byte pad[3];
		}SoundFile_t;

		typedef struct SndCurve_s
		{
			const char* filename;
			int knotCount;
			float knots[8][2];
		}SndCurve_t;

		typedef enum
		{
			SAT_UNKNOWN = 0x0,
			SAT_LOADED = 0x1,
			SAT_STREAMED = 0x2,
			SAT_PRIMED = 0x3,
			SAT_COUNT = 0x4,
		}snd_alias_type_t;

		typedef struct SpeakerLevels_s
		{
			int speaker;
			int numLevels;
			float levels[2];
		}SpeakerLevels_t;

		typedef struct ChannelMap_s
		{
			int entryCount;	// how many entries are used
			SpeakerLevels_t speakers[6];
		}ChannelMap_t;

		typedef struct SpeakerMap_s
		{
			byte isDefault;
			byte pad[3];
			const char* name;
			ChannelMap_t channelMaps[2][2];
		}SpeakerMap_t;

		typedef const struct snd_alias_s
		{
			const char* aliasName;
			const char* subtitle;
			const char* secondaryAliasName;
			const char* chainAliasName;
			SoundFile_t* soundFile;
			int sequence;
			float volMin;
			float volMax;
			float pitchMin;
			float pitchMax;
			float distMin;
			float distMax;
			int flags;
			float slavePercentage;
			float probability;
			float lfePercentage;
			float centerPercentage;
			int startDelay;
			SndCurve_t* volumeFalloffCurve;
			float envelopMin;
			float envelopMax;
			float envelopPercentage;
			SpeakerMap_t* speakerMap;
		}snd_alias_t;

		typedef struct snd_alias_list_s
		{
			const char* aliasName;
			snd_alias_t* head; //Can be multiple
			int count;
		}snd_alias_list_t;

		union XAssetHeader
		{
			void* data;
			// 			XModelPieces *xmodelPieces;
			PhysPreset* physPreset;
			XAnimParts* parts;
			XModel* model;
			Material* material;
			// 			MaterialPixelShader *pixelShader;
			// 			MaterialVertexShader *vertexShader;
			MaterialTechniqueSet* techniqueSet;
			GfxImage* image;
			snd_alias_list_t *sound;
			// 			SndCurve *sndCurve;
			clipMap_t* clipMap;
			ComWorld* comWorld;
			GameWorldSp* gameWorldSp;
			// 			GameWorldMp *gameWorldMp;
			MapEnts* mapEnts;
			GfxWorld* gfxWorld;
			GfxLightDef* lightDef;
			Font_s *font;
			MenuList* menuList;
			menuDef_t *menu;
			LocalizeEntry *localize;
			// 			WeaponDef *weapon;
			// 			SndDriverGlobals *sndDriverGlobals;
			/*FxEffectDef**/void* fx;
			// 			FxImpactTable *impactFx;
			RawFile* rawfile;
			StringTable *stringTable;
		};

		struct XAsset
		{
			XAssetType type;
			XAssetHeader header;
		};

		struct XAssetEntry
		{
			XAsset asset;
			byte zoneIndex;
			bool inuse;
			unsigned __int16 nextHash;
			unsigned __int16 nextOverride;
			unsigned __int16 usageFrame;
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

		struct CmdArgs
		{
			int nesting;
			int localClientNum[8];
			int controllerIndex[8];
			int argc[8];
			const char** argv[8];
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

		union DvarLimits
		{
			struct
			{
				int stringCount;
				const char** strings;
			} enumeration;

			struct
			{
				int min;
				int max;
			} integer;

			struct
			{
				float min;
				float max;
			} value;

			struct
			{
				float min;
				float max;
			} vector;
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
			const char* description;
			dvar_flags flags;
			dvar_type type;
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

		enum DB_FILE_EXISTS_PATH
		{
			DB_PATH_ZONE = 0,
			DB_PATH_MAIN = 1,
			DB_PATH_USERMAPS = 2
		};

		enum XZONE_FLAGS
		{
			XZONE_ZERO = 0x0,

			XZONE_LOC_POST_GFX = 0x0,
			XZONE_LOC_POST_GFX_FREE = 0x0,

			XZONE_LOC_COMMON = 0x1,
			XZONE_LOC_COMMON_FREE = 0x0,

			XZONE_POST_GFX = 0x2,
			XZONE_POST_GFX_FREE = 0x0,

			XZONE_COMMON = 0x4,
			XZONE_COMMON_FREE = 0x0,

			XZONE_UI = 0x8,
			XZONE_UI_FREE = 0x0,
			// .. below

			XZONE_MAP = 0x8,
			XZONE_MAP_FREE = 0x8,

			XZONE_MOD = 0x10, //(0x10)
			XZONE_MOD_FREE = 0x0,

			XZONE_DEBUG = 0x40, //(0x40)
			XZONE_DEBUG_FREE = 0x40, //(0x40)

			XZONE_LOAD = 0x20, //(0x20)
			XZONE_LOAD_FREE = 0x60, //(0x60)

			XZONE_UI_FREE_INGAME = XZONE_LOAD_FREE | XZONE_UI //(0x68)
		};

		struct XZoneInfo
		{
			const char* name;
			int allocFlags;
			int freeFlags;
		};

		enum
		{
			THREAD_CONTEXT_MAIN,
			THREAD_CONTEXT_BACKEND,
			THREAD_CONTEXT_WORKER0,
			THREAD_CONTEXT_WORKER1,
			THREAD_CONTEXT_SERVER,
			THREAD_CONTEXT_CINEMATIC,
			THREAD_CONTEXT_DATABASE,
			THREAD_CONTEXT_STREAM,
			THREAD_CONTEXT_STATS_WRITE,
			THREAD_CONTEXT_MJPEG,
			THREAD_CONTEXT_COUNT,
		};

		union XAssetEntryPoolEntry
		{
			XAssetEntry entry;
			union XAssetEntryPoolEntry* next;
		};

		struct XBlock
		{
			char* data;
			unsigned int size;
		};

		struct DB_LoadData
		{
			void* f;
			const char* filename;
			XBlock* blocks;
			int outstandingReads;
			struct _OVERLAPPED overlapped;
			int stream[13];
			char* compressBufferStart;
			char* compressBufferEnd;
			void(__cdecl* interrupt_)();
			int allocType;
		};

		struct EntHandle
		{
			unsigned __int16 number;
			unsigned __int16 infoIndex;
		};

		struct entityShared_t
		{
			char linked;
			char bmodel;
			char svFlags;
			int clientMask[2];
			char inuse;
			int broadcastTime;
			float mins[3];
			float maxs[3];
			int contents;
			float absmin[3];
			float absmax[3];
			float currentOrigin[3];
			float currentAngles[3];
			EntHandle ownerNum;
			int eventTime;
		};

		struct gentity_s;

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

		struct usercmd_s
		{
			int serverTime;
			int buttons;
			int angles[3];
			char weapon;
			char offHandIndex;
			char forwardmove; //buttonsVertical
			char rightmove; //buttonsHorizontal
			float meleeChargeYaw;
			char meleeChargeDist;
			char selectedLocation[2];
		};

		struct playerTeamState_t
		{
			int location;
		};

		struct clientSession_t
		{
			sessionState_t sessionState;
			int forceSpectatorClient;
			int killCamEntity;
			int status_icon;
			int archiveTime;
			int score;
			int deaths;
			int kills;
			int assists;
			unsigned __int16 scriptPersId;
			clientConnected_t connected;
			usercmd_s cmd;
			usercmd_s oldcmd;
			int localClient;
			int predictItemPickup;
			char newnetname[16];
			int maxHealth;
			int enterTime;
			playerTeamState_t teamState;
			int voteCount;
			int teamVoteCount;
			float moveSpeedScaleMultiplier;
			int viewmodelIndex;
			int noSpectate;
			int teamInfo;
			clientState_s cs;
			int psOffsetTime;
		};

		struct gclient_s
		{
			playerState_s ps;
			clientSession_t sess;
			int spectatorClient;
			int noclip;
			int ufo;
			int bFrozen;
			int lastCmdTime;
			int buttons;
			int oldbuttons;
			int latched_buttons;
			int buttonsSinceLastFrame;
			float oldOrigin[3];
			float fGunPitch;
			float fGunYaw;
			int damage_blood;
			float damage_from[3];
			int damage_fromWorld;
			int accurateCount;
			int accuracy_shots;
			int accuracy_hits;
			int inactivityTime;
			int inactivityWarning;
			int lastVoiceTime;
			int switchTeamTime;
			float currentAimSpreadScale;
			gentity_s* persistantPowerup;
			int portalID;
			int dropWeaponTime;
			int sniperRifleFiredTime;
			float sniperRifleMuzzleYaw;
			int PCSpecialPickedUpCount;
			EntHandle useHoldEntity;
			int useHoldTime;
			int useButtonDone;
			int iLastCompassPlayerInfoEnt;
			int compassPingTime;
			int damageTime;
			float v_dmg_roll;
			float v_dmg_pitch;
			float swayViewAngles[3];
			float swayOffset[3];
			float swayAngles[3];
			float vLastMoveAng[3];
			float fLastIdleFactor;
			float vGunOffset[3];
			float vGunSpeed[3];
			int weapIdleTime;
			int lastServerTime;
			int lastSpawnTime;
			unsigned int lastWeapon;
			bool previouslyFiring;
			bool previouslyUsingNightVision;
			bool previouslySprinting;
			int hasRadar;
			int lastStand;
			int lastStandTime;
		};

		struct turretInfo_s
		{
			int inuse;
			int flags;
			int fireTime;
			float arcmin[2];
			float arcmax[2];
			float dropPitch;
			int stance;
			int prevStance;
			int fireSndDelay;
			float userOrigin[3];
			float playerSpread;
			float pitchCap;
			int triggerDown;
			char fireSnd;
			char fireSndPlayer;
			char stopSnd;
			char stopSndPlayer;
		};

		struct item_ent_t
		{
			int ammoCount;
			int clipAmmoCount;
			int index;
		};

		struct __declspec(align(4)) trigger_ent_t
		{
			int threshold;
			int accumulate;
			int timestamp;
			int singleUserEntIndex;
			bool requireLookAt;
		};

		struct mover_ent_t
		{
			float decelTime;
			float aDecelTime;
			float speed;
			float aSpeed;
			float midTime;
			float aMidTime;
			float pos1[3];
			float pos2[3];
			float pos3[3];
			float apos1[3];
			float apos2[3];
			float apos3[3];
		};

		struct corpse_ent_t
		{
			int deathAnimStartTime;
		};

		enum MissileStage
		{
			MISSILESTAGE_SOFTLAUNCH = 0x0,
			MISSILESTAGE_ASCENT = 0x1,
			MISSILESTAGE_DESCENT = 0x2,
		};

		enum MissileFlightMode
		{
			MISSILEFLIGHTMODE_TOP = 0x0,
			MISSILEFLIGHTMODE_DIRECT = 0x1,
		};

		struct missile_ent_t
		{
			float time;
			int timeOfBirth;
			float travelDist;
			float surfaceNormal[3];
			team_t team;
			float curvature[3];
			float targetOffset[3];
			MissileStage stage;
			MissileFlightMode flightMode;
		};

		union $4C7580D783CB81EAAF8D9BB4061D1D71
		{
			item_ent_t item[2];
			trigger_ent_t trigger;
			mover_ent_t mover;
			corpse_ent_t corpse;
			missile_ent_t missile;
		};

		struct tagInfo_s
		{
			gentity_s* parent;
			gentity_s* next;
			unsigned __int16 name;
			int index;
			float axis[4][3];
			float parentInvAxis[4][3];
		};

		struct gentity_s
		{
			entityState_s s;
			entityShared_t r;
			gclient_s* client;
			turretInfo_s* pTurretInfo;
			void* scr_vehicle;
			unsigned __int16 model;
			char physicsObject;
			char takedamage;
			char active;
			char nopickup;
			char handler;
			char team;
			unsigned __int16 classname;
			unsigned __int16 target;
			unsigned __int16 targetname;
			unsigned int attachIgnoreCollision;
			int spawnflags;
			int flags;
			int eventTime;
			int freeAfterEvent;
			int unlinkAfterEvent;
			int clipmask;
			int processedFrame;
			EntHandle parent;
			int nextthink;
			int health;
			int maxHealth;
			int damage;
			int count;
			gentity_s* chain;
			$4C7580D783CB81EAAF8D9BB4061D1D71 ___u30;
			EntHandle missileTargetEnt;
			tagInfo_s* tagInfo;
			gentity_s* tagChildren;
			unsigned __int16 attachModelNames[19];
			unsigned __int16 attachTagNames[19];
			int useCount;
			gentity_s* nextFree;
		};


		//* Custom
		enum itemTextStyle
		{
			ITEM_TEXTSTYLE_NORMAL = 0,   // normal text
			ITEM_TEXTSTYLE_SHADOWED = 3,   // drop shadow ( need a color for this )
			ITEM_TEXTSTYLE_SHADOWEDMORE = 6,   // drop shadow ( need a color for this )
			ITEM_TEXTSTYLE_BORDERED = 7,   // border (stroke)
			ITEM_TEXTSTYLE_BORDEREDMORE = 8,   // more border :P
			ITEM_TEXTSTYLE_MONOSPACE = 128,
			ITEM_TEXTSTYLE_MONOSPACESHADOWED = 132,
		};

		enum XFILE_BLOCK_TYPES
		{
			XFILE_BLOCK_TEMP = 0x0,
			XFILE_BLOCK_PHYSICAL = 0x1,
			XFILE_BLOCK_RUNTIME = 0x2,
			XFILE_BLOCK_VIRTUAL = 0x3,
			XFILE_BLOCK_LARGE = 0x4,

			// Those are probably incorrect
			XFILE_BLOCK_CALLBACK,
			XFILE_BLOCK_VERTEX,
			XFILE_BLOCK_INDEX,

			MAX_XFILE_COUNT,

			XFILE_BLOCK_INVALID = -1
		};

		struct XFile
		{
			unsigned int size;
			unsigned int externalSize;
			unsigned int blockSize[MAX_XFILE_COUNT];
		};

		#define FS_GENERAL_REF	0x01
		#define FS_UI_REF		0x02
		#define FS_CGAME_REF	0x04
		#define FS_QAGAME_REF	0x08

		#define MAX_ZPATH	256
		#define	MAX_SEARCH_PATHS	4096
		#define MAX_FILEHASH_SIZE	1024
		#define MAX_FILE_HANDLES 64


		typedef struct fileInPack_s {
			unsigned long		pos;		// file info position in zip
			char* name;		// name of the file
			struct	fileInPack_s* next;		// next file in the hash
		} fileInPack_t;

		typedef struct {	//Verified
			char		pakFilename[256];	// c:\quake3\baseq3\pak0.pk3
			char		pakBasename[256];	// pak0
			char		pakGamename[256];	// baseq3
			void*		handle;						// handle to zip file +0x300
			int			checksum;					// regular checksum
			int			pure_checksum;				// checksum for pure
			int			hasOpenFile;
			int			numfiles;					// number of files in pk3
			int			referenced;					// referenced file flags
			int			hashSize;					// hash table size (power of 2)		+0x318
			fileInPack_t** hashTable;					// hash table	+0x31c
			fileInPack_t* buildBuffer;				// buffer with the filenames etc. +0x320
		} pack_t;

		typedef struct {	//Verified
			char		path[256];		// c:\quake3
			char		gamedir[256];	// baseq3
		} directory_t;


		typedef struct searchpath_s {	//Verified
			struct searchpath_s* next;
			pack_t* pack;		// only one of pack / dir will be non NULL
			directory_t* dir;
			bool	localized;
			int		val_2;
			int		val_3;
			int		langIndex;
		} searchpath_t;


#define MAX_TOKEN 1024
#define MAX_TOKENLENGTH 1024

		typedef struct punctuation_s
		{
			char* p;						//punctuation character(s)
			int n;							//punctuation indication
			struct punctuation_s* next;		//next punctuation
		} punctuation_t;

		typedef struct token_s
		{
			char string[MAX_TOKEN];			//available token
			int type;						//last read token type
			int subtype;					//last read token sub type
			unsigned long int intvalue;	//integer value
			long double floatvalue;			//floating point value
			char* whitespace_p;				//start of white space before token
			char* endwhitespace_p;			//start of white space before token
			int line;						//line the token was on
			int linescrossed;				//lines crossed in white space
			struct token_s* next;			//next token in chain
		} token_t;

		typedef struct script_s
		{
			char filename[64];				//file name of the script
			char* buffer;					//buffer containing the script
			char* script_p;					//current pointer in the script
			char* end_p;					//pointer to the end of the script
			char* lastscript_p;				//script pointer before reading token
			char* whitespace_p;				//begin of the white space
			char* endwhitespace_p;			//end of the white space
			int length;						//length of the script in bytes
			int line;						//current line in script
			int lastline;					//line before reading token
			int tokenavailable;				//set by UnreadLastToken
			int flags;						//several script flags
			punctuation_t* punctuations;	//the punctuations used in the script
			punctuation_t** punctuationtable;
			token_t token;					//available token
			struct script_s* next;			//next script in a chain
		} script_t;

		typedef struct define_s
		{
			char* name;							//define name
			int flags;							//define flags
			int builtin;						// > 0 if builtin define
			int numparms;						//number of define parameters
			token_t* parms;						//define parameters
			token_t* tokens;					//macro tokens (possibly containing parm tokens)
			struct define_s* next;				//next defined macro in a list
			struct define_s* hashnext;			//next define in the hash chain
		} define_t;

		typedef struct indent_s
		{
			int type;								//indent type
			int skip;								//true if skipping current indent
			script_t* script;						//script the indent was in
			struct indent_s* next;					//next indent on the indent stack
		} indent_t;

		typedef struct source_s
		{
			char filename[64];					//file name of the script
			char includepath[64];					//path to include files
			punctuation_t* punctuations;			//punctuations to use
			script_t* scriptstack;					//stack with scripts of the source
			token_t* tokens;						//tokens to read first
			define_t* defines;						//list with macro definitions
			define_t** definehash;					//hash chain with defines
			indent_t* indentstack;					//stack with indents
			int skip;								// > 0 if skipping conditional code
			token_t token;							//last read token
		} source_t;

		typedef struct pc_token_s
		{
			int type;
			int subtype;
			int intvalue;
			float floatvalue;
			char string[MAX_TOKENLENGTH];
		} pc_token_t;

		typedef struct keywordHash_s
		{
			char* keyword;
			bool(*func)(menuDef_t* item, int handle);
		} keywordHash_t;
	}
}
