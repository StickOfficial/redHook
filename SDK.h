#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <conio.h>
#include <cstdlib>
#include <cstdio>
#include <future>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <sys\stat.h>
#include <algorithm>

#include "VTHook.h"
#include "Vector.h"
#include "dt_recv.h"
#include "prediction.h"
#include "DynamicNetvars.h"
#include "CBaseAPI.h"
#include "IConVar.h"
#include "crc32.h"
#include "netpacket.h"
#include "network.h"
#include "CUtlVector.h"

#include "steam/SteamTypes.h"
#include "steam\\Steamworks.h"
#include "valvePlatform.h"

#define PI 3.14159265358979323846f
#define RADPI 57.295779513082f
#define DEG2RAD(x)((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)((float)(x) * (float)(180.0f / (float)(PI)))
#define SAFE_RESET( x ) if( x ) { x.reset( ); x = nullptr; }
#define SAFE_DELETE( x ) if( x ) { delete x; x = nullptr; }
#define IsInRange(x, a, b) (x >= a && x <= b)
#define GetBits(x) (IsInRange(x, '0', '9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xA))
#define GetByte(x) (GetBits(x[0]) << 4 | GetBits(x[1]))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))

#define streql(x,y) strcmp( (x) , (y) ) == 0
#define MakePtr( Type, dwBase, dwOffset ) ( ( Type )( DWORD( dwBase ) + (DWORD)( dwOffset ) ) )

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

#define DotProduct(x,y)         ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])   
#define VectorSubtract(a,b,c)   ((c)[0]=(a)[0]-(b)[0],(c)[1]=(a)[1]-(b)[1],(c)[2]=(a)[2]-(b)[2])   
#define VectorAdd(a,b,c)        ((c)[0]=(a)[0]+(b)[0],(c)[1]=(a)[1]+(b)[1],(c)[2]=(a)[2]+(b)[2])   
#define VectorCopy(a,b)         ((b)[0]=(a)[0],(b)[1]=(a)[1],(b)[2]=(a)[2])   
#define VectorScale(v, s, o)    ((o)[0]=(v)[0]*(s),(o)[1]=(v)[1]*(s),(o)[2]=(v)[2]*(s))   
#define VectorMA(v, s, b, o)    ((o)[0]=(v)[0]+(b)[0]*(s),(o)[1]=(v)[1]+(b)[1]*(s),(o)[2]=(v)[2]+(b)[2]*(s))   
#define VectorCmp(a,b)          (((a)[0]==(b)[0])&&((a)[1]==(b)[1])&&((a)[2]==(b)[2]))

class model_t;
class CGameTrace;
class CUserCmd;
class CPlayer;
class CNetVars;
class CDrawings;
class CMisc;
class CAimbot;
class CVars;

typedef CGameTrace trace_t;

#pragma optimize("gsy", on)
#pragma warning(disable : 4244)
#pragma comment(lib, "steamclient.lib")
#pragma comment(lib, "tier1.lib")

typedef float matrix3x4[3][4];
typedef unsigned short ModelInstanceHandle_t;
typedef unsigned long HFont;
typedef int OverlayText_t;

typedef void* (*CreateInterfaceFn)(const char* szInterface, int* pReturnCode);
typedef void(__cdecl* MsgFn)(char const* pMsg, va_list);
typedef bool(__thiscall* CreateMoveFn)(void*, float, CUserCmd*);
typedef void(__thiscall* CHLCreateMoveFn)(void*, int, float, bool);
typedef void(__thiscall* PaintTraverseFn)(void*, unsigned int, bool, bool);

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

typedef void(__thiscall* FrameStageNotifyFn)(void*, ClientFrameStage_t);

template <typename Fn> __forceinline Fn CallVirtualFunction(void* pClassBase, int nFunctionIndex)
{
	return (Fn)((PDWORD)*(PDWORD*)pClassBase)[nFunctionIndex];
}

enum FontDrawType_t
{
	FONT_DRAW_DEFAULT = 0,
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,
	FONT_DRAW_TYPE_COUNT = 2,
};

enum FontFlags_t
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};

enum PlayerControls_t
{
	IN_ATTACK = (1 << 0),
	IN_JUMP = (1 << 1),
	IN_DUCK = (1 << 2),
	IN_FORWARD = (1 << 3),
	IN_BACK = (1 << 4),
	IN_USE = (1 << 5),
	IN_CANCEL = (1 << 6),
	IN_LEFT = (1 << 7),
	IN_RIGHT = (1 << 8),
	IN_MOVELEFT = (1 << 9),
	IN_MOVERIGHT = (1 << 10),
	IN_ATTACK2 = (1 << 11),
	IN_RUN = (1 << 12),
	IN_RELOAD = (1 << 13),
	IN_ALT1 = (1 << 14),
	IN_ALT2 = (1 << 15),
	IN_SCORE = (1 << 16),
	IN_SPEED = (1 << 17),
	IN_WALK = (1 << 18),
	IN_ZOOM = (1 << 19),
	IN_WEAPON1 = (1 << 20),
	IN_WEAPON2 = (1 << 21),
	IN_BULLRUSH = (1 << 22),
	IN_GRENADE1 = (1 << 23),
	IN_GRENADE2 = (1 << 24),
	IN_ATTACK3 = (1 << 25),
};

enum PlayerStates_t
{
	FL_ONGROUND = (1 << 0),
	FL_DUCKING = (1 << 1),
	FL_WATERJUMP = (1 << 2),
	FL_ONTRAIN = (1 << 3),
	FL_INRAIN = (1 << 4),
	FL_FROZEN = (1 << 5),
	FL_ATCONTROLS = (1 << 6),
	FL_CLIENT = (1 << 7),
	FL_FAKECLIENT = (1 << 8),
	FL_INWATER = (1 << 9),
};

enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

enum SurfaceFlags_t
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4),
};

enum MoveType_t
{
	MOVETYPE_NONE = 0,
	MOVETYPE_ISOMETRIC,
	MOVETYPE_WALK,
	MOVETYPE_STEP,
	MOVETYPE_FLY,
	MOVETYPE_FLYGRAVITY,
	MOVETYPE_VPHYSICS,
	MOVETYPE_PUSH,
	MOVETYPE_NOCLIP,
	MOVETYPE_LADDER,
	MOVETYPE_OBSERVER,
	MOVETYPE_CUSTOM,

	MOVETYPE_LAST = MOVETYPE_CUSTOM,

	MOVETYPE_MAX_BITS = 4,
};

enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_MULTIPASS = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
};

enum TFConditions_t
{
	TFCond_Slowed = (1 << 0),
	TFCond_Zoomed = (1 << 1),
	TFCond_Disguising = (1 << 2),
	TFCond_Disguised = (1 << 3),
	TFCond_Cloaked = (1 << 4),
	TFCond_Ubercharged = (1 << 5),
	TFCond_TeleportedGlow = (1 << 6),
	TFCond_Taunting = (1 << 7),
	TFCond_UberchargeFading = (1 << 8),
	TFCond_CloakFlicker = (1 << 9),
	TFCond_Teleporting = (1 << 10),
	TFCond_Kritzkrieged = (1 << 11),
	TFCond_TmpDamageBonus = (1 << 12),
	TFCond_DeadRingered = (1 << 13),
	TFCond_Bonked = (1 << 14),
	TFCond_Stunned = (1 << 15),
	TFCond_Buffed = (1 << 16),
	TFCond_Charging = (1 << 17),
	TFCond_DemoBuff = (1 << 18),
	TFCond_CritCola = (1 << 19),
	TFCond_InHealRadius = (1 << 20),
	TFCond_Healing = (1 << 21),
	TFCond_OnFire = (1 << 22),
	TFCond_Overhealed = (1 << 23),
	TFCond_Jarated = (1 << 24),
	TFCond_Bleeding = (1 << 25),
	TFCond_DefenseBuffed = (1 << 26),
	TFCond_Milked = (1 << 27),
	TFCond_MegaHeal = (1 << 28),
	TFCond_RegenBuffed = (1 << 29),
	TFCond_MarkedForDeath = (1 << 30),
	TFCond_NoHealingDamageBuff = (1 << 31),

	TFCondEx_SpeedBuffAlly = (1 << 0),
	TFCondEx_HalloweenCritCandy = (1 << 1),
	TFCondEx_CritCanteen = (1 << 2),
	TFCondEx_CritDemoCharge = (1 << 3),
	TFCondEx_CritHype = (1 << 4),
	TFCondEx_CritOnFirstBlood = (1 << 5),
	TFCondEx_CritOnWin = (1 << 6),
	TFCondEx_CritOnFlagCapture = (1 << 7),
	TFCondEx_CritOnKill = (1 << 8),
	TFCondEx_RestrictToMelee = (1 << 9),
	TFCondEx_DefenseBuffNoCritBlock = (1 << 10),
	TFCondEx_Reprogrammed = (1 << 11),
	TFCondEx_PyroCrits = (1 << 12),
	TFCondEx_PyroHeal = (1 << 13),
	TFCondEx_FocusBuff = (1 << 14),
	TFCondEx_DisguisedRemoved = (1 << 15),
	TFCondEx_MarkedForDeathSilent = (1 << 16),
	TFCondEx_DisguisedAsDispenser = (1 << 17),
	TFCondEx_Sapped = (1 << 18),
	TFCondEx_UberchargedHidden = (1 << 19),
	TFCondEx_UberchargedCanteen = (1 << 20),
	TFCondEx_HalloweenBombHead = (1 << 21),
	TFCondEx_HalloweenThriller = (1 << 22),
	TFCondEx_BulletCharge = (1 << 26),
	TFCondEx_ExplosiveCharge = (1 << 27),
	TFCondEx_FireCharge = (1 << 28),
	TFCondEx_BulletResistance = (1 << 29),
	TFCondEx_ExplosiveResistance = (1 << 30),
	TFCondEx_FireResistance = (1 << 31),

	TFCondEx2_Stealthed = (1 << 0),
	TFCondEx2_MedigunDebuff = (1 << 1),
	TFCondEx2_StealthedUserBuffFade = (1 << 2),
	TFCondEx2_BulletImmune = (1 << 3),
	TFCondEx2_BlastImmune = (1 << 4),
	TFCondEx2_FireImmune = (1 << 5),
	TFCondEx2_PreventDeath = (1 << 6),
	TFCondEx2_MVMBotRadiowave = (1 << 7),
	TFCondEx2_HalloweenSpeedBoost = (1 << 8),
	TFCondEx2_HalloweenQuickHeal = (1 << 9),
	TFCondEx2_HalloweenGiant = (1 << 10),
	TFCondEx2_HalloweenTiny = (1 << 11),
	TFCondEx2_HalloweenInHell = (1 << 12),
	TFCondEx2_HalloweenGhostMode = (1 << 13),
	TFCondEx2_Parachute = (1 << 16),
	TFCondEx2_BlastJumping = (1 << 17),

	TFCond_MiniCrits = (TFCond_Buffed | TFCond_CritCola),
	TFCond_IgnoreStates = (TFCond_Ubercharged | TFCond_Bonked),
	TFCondEx_IgnoreStates = (TFCondEx_PyroHeal)
};

enum TFClasses_t
{
	TF2_None = 0,
	TF2_Scout,
	TF2_Sniper,
	TF2_Soldier,
	TF2_Demoman,
	TF2_Medic,
	TF2_Heavy,
	TF2_Pyro,
	TF2_Spy,
	TF2_Engineer
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE
};

enum LifeStates_t
{
	LIFE_ALIVE = 0,
	LIFE_DYING,
	LIFE_DEAD,
	LIFE_RESPAWNABLE,
	LIFE_DISCARDBODY
};

struct mstudiobbox_t
{
	int bone;
	int group;
	Vector3 bbmin;
	Vector3 bbmax;
	int szhitboxnameindex;
	int unused[8];
};

struct mstudiohitboxset_t
{
	int sznameindex;
	inline char *const pszName(void) const { return ((char *)this) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t *pHitbox(int i) const { return (mstudiobbox_t *)(((BYTE *)this) + hitboxindex) + i; };
};

struct studiohdr_t
{
	unsigned char		pad00[12];
	char				name[64];
	unsigned char		pad01[80];
	int					numbones;
	int					boneindex;
	unsigned char		pad02[12];
	int					hitboxsetindex;
	unsigned char		pad03[228];

	mstudiohitboxset_t	*pHitboxSet(int i) const
	{
		return (mstudiohitboxset_t *)(((unsigned char *)this) + hitboxsetindex) + i;
	};

	mstudiobbox_t *pHitbox(int i, int set) const
	{
		const mstudiohitboxset_t *s = pHitboxSet(set);
		if(!s)
			return 0;

		return s->pHitbox(i);
	};

	int			iHitboxCount(int set) const
	{
		const mstudiohitboxset_t *s = pHitboxSet(set);
		if(!s)
			return 0;

		return s->numhitboxes;
	};
};

typedef struct player_info_s
{
	char			name[32];
	int				userID;
	char			guid[33];
	unsigned long	friendsID;
	char			friendsName[32];
	bool			fakeplayer;
	bool			ishltv;
	unsigned long	customFiles[4];
	unsigned char	filesDownloaded;
} player_info_t;

class CUserCmd
{
public:

	virtual ~CUserCmd() {};
	int command_number;
	int tick_count;
	Vector3 viewangles;
	float forwardmove;
	float sidemove;
	float upmove;
	int	buttons;
	byte impulse;
	int weaponselect;
	int weaponsubtype;
	int random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
};

class ClientClass
{
public:
	BYTE _chPadding[8];
	char* szName;
	RecvTable* rtTable;
	ClientClass* pNextClass;
	int iClassID;
};

class IBaseClientDLL
{
public:
	ClientClass* GetAllClasses()
	{
		typedef ClientClass* (__thiscall* GetAllClassesFn)(void*);
		return CallVirtualFunction<GetAllClassesFn>(this, 8)(this);
	}
};

class IClientMode
{
public:
};

class IVEngineClient
{
public:
	void GetScreenSize(int& iWidth, int& iHeight)
	{
		typedef void(__thiscall* GetScreenSizeFn)(void*, int&, int&);
		return CallVirtualFunction<GetScreenSizeFn>(this, 5)(this, iWidth, iHeight);
	}

	void ClientCmd(const char* szCommandString)
	{
		typedef void(__thiscall* ClientCmdFn)(void*, const char*);
		return CallVirtualFunction<ClientCmdFn>(this, 7)(this, szCommandString);
	}

	bool GetPlayerInfo(int iIndex, player_info_t* pInfo)
	{
		typedef bool(__thiscall* GetPlayerInfoFn)(void*, int, player_info_t*);
		return CallVirtualFunction<GetPlayerInfoFn>(this, 8)(this, iIndex, pInfo);
	}

	bool Con_IsVisible()
	{
		typedef bool(__thiscall* Con_IsVisibleFn)(void*);
		return CallVirtualFunction<Con_IsVisibleFn>(this, 11)(this);
	}

	int GetLocalPlayer()
	{
		typedef int(__thiscall* GetLocalPlayerFn)(void*);
		return CallVirtualFunction<GetLocalPlayerFn>(this, 12)(this);
	}

	float Time()
	{
		typedef float(__thiscall* TimeFn)(void*);
		return CallVirtualFunction<TimeFn>(this, 14)(this);
	}

	void GetViewAngles(Vector3 &vAngles)
	{
		typedef void(__thiscall* GetViewAnglesFn)(void*, Vector3&);
		return CallVirtualFunction< GetViewAnglesFn >(this, 19)(this, vAngles);
	}

	void SetViewAngles(Vector3 &vAngles)
	{
		typedef void(__thiscall* SetViewAnglesFn)(void*, Vector3&);
		return CallVirtualFunction< SetViewAnglesFn >(this, 20)(this, vAngles);
	}

	int GetMaxClients()
	{
		typedef int(__thiscall* GetMaxClientsFn)(void*);
		return CallVirtualFunction<GetMaxClientsFn>(this, 21)(this);
	}

	bool IsInGame()
	{
		typedef bool(__thiscall* IsInGameFn)(void*);
		return CallVirtualFunction<IsInGameFn>(this, 26)(this);
	}

	bool IsConnected()
	{
		typedef bool(__thiscall* IsConnectedFn)(void*);
		return CallVirtualFunction<IsConnectedFn>(this, 27)(this);
	}

	bool IsDrawingLoadingImage(void)
	{
		typedef bool(__thiscall* OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 28)(this);
	}

	const matrix3x4& WorldToScreenMatrix()
	{
		typedef const matrix3x4& (__thiscall* GetMatrixFn)(void*);
		return CallVirtualFunction<GetMatrixFn>(this, 36)(this);
	}

	bool IsTakingScreenshot()
	{
		typedef bool(__thiscall* IsTakingScreenshotFn)(void*);
		return CallVirtualFunction<IsTakingScreenshotFn>(this, 85)(this);
	}

	void ClientCmd_Unrestricted(const char* szCommandString)
	{
		typedef void(__thiscall* ClientCmd_UnrestrictedFn)(void*, const char*);
		return CallVirtualFunction<ClientCmd_UnrestrictedFn>(this, 106)(this, szCommandString);
	}

	void ServerCmdKeyValues(PVOID keyval)
	{
		typedef void(__thiscall * erverCmdKeyValuesFn)(PVOID, PVOID);
		return CallVirtualFunction<erverCmdKeyValuesFn>(this, 127)(this, keyval);
	}

	CNetChan *GetNetChannelInfo(void)
	{
		typedef CNetChan *(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 72)(this);
	}
};

class IClientEntity
{
public:
	void* GetClientRenderable()
	{
		return reinterpret_cast<void*>(this + 0x4);
	}

	void* GetClientNetworkable()
	{
		return reinterpret_cast<void*>(this + 0x8);
	}

	Vector3& GetAbsOrigin()
	{
		typedef Vector3& (__thiscall* GetAbsOriginFn)(void*);
		return CallVirtualFunction<GetAbsOriginFn>(this, 9)(this);
	}

	Vector3& GetAbsAngles()
	{
		typedef Vector3& (__thiscall* GetAbsAnglesFn)(void*);
		return CallVirtualFunction<GetAbsAnglesFn>(this, 10)(this);
	}

	Vector3 GetVecOrigin()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BaseEntity", "m_vecOrigin");
	}



	void GetWorldSpaceCenter(Vector3& vWorldSpaceCenter)
	{
		Vector3 vMin, vMax;
		this->GetRenderBounds(vMin, vMax);
		vWorldSpaceCenter = this->GetAbsOrigin();
		vWorldSpaceCenter.z += (vMin.z + vMax.z) / 2;
	}

	ClientClass* GetClientClass()
	{
		typedef ClientClass* (__thiscall* GetClientClassFn)(void*);
		return CallVirtualFunction<GetClientClassFn>(this->GetClientNetworkable(), 2)(this->GetClientNetworkable());
	}

	bool IsDormant()
	{
		typedef bool(__thiscall* IsDormantFn)(void*);
		return CallVirtualFunction<IsDormantFn>(this->GetClientNetworkable(), 8)(this->GetClientNetworkable());
	}

	int GetIndex()
	{
		typedef int(__thiscall* GetIndexFn)(void*);
		return CallVirtualFunction<GetIndexFn>(this->GetClientNetworkable(), 9)(this->GetClientNetworkable());
	}

	model_t* GetModel()
	{
		typedef model_t* (__thiscall* GetModelFn)(void*);
		return CallVirtualFunction<GetModelFn>(this->GetClientRenderable(), 9)(this->GetClientRenderable());
	}

	bool SetupBones(matrix3x4 *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime)
	{
		typedef bool(__thiscall* SetupBonesFn)(void*, matrix3x4*, int, int, float);
		return CallVirtualFunction<SetupBonesFn>(this->GetClientRenderable(), 16)(this->GetClientRenderable(), pBoneToWorldOut, nMaxBones, boneMask, currentTime);
	}

	void GetRenderBounds(Vector3& vMins, Vector3& vMaxs)
	{
		typedef void(__thiscall* GetRenderBoundsFn)(void*, Vector3&, Vector3&);
		CallVirtualFunction<GetRenderBoundsFn>(this->GetClientRenderable(), 20)(this->GetClientRenderable(), vMins, vMaxs);
	}

	template <typename t>
	inline t* cast_to()
	{
		return reinterpret_cast<t*>(this);
	}

	MoveType_t GetMoveType()
	{
		DYNVAR_RETURN(MoveType_t, this, "DT_BaseEntity", "movetype");
	}

	float GetChargeDamage()
	{
		DYNVAR_RETURN(float, this, "DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
	}

	int GetTickBase()
	{
		DYNVAR_RETURN(int, this, "DT_BasePlayer", "localdata", "m_nTickBase");
	}

	float GetNextAttack()
	{
		DYNVAR_RETURN(float, this, "DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	}

	float GetLastFireTime()
	{
		DYNVAR_RETURN(float, this, "DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
	}

	Vector3 GetPunchAngles()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngle");
	}

	int GetItemDefinitionIndex()
	{
		DYNVAR_RETURN(int, this, "DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
	}

	float GetChargeTime()
	{
		DYNVAR_RETURN(float, this, "DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime");
	}

	int GetFlags()
	{
		DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_fFlags");
	}

	Vector3 GetCollideableMins()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BaseEntity", "m_Collision", "m_vecMins");
	}

	Vector3 GetCollideableMaxs()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs");
	}

	Vector3 Velocity()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BasePlayer", "m_vecVelocity[0]");
	}

	Vector3 GetViewOffset()
	{
		DYNVAR_RETURN(Vector3, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	}

	Vector3 GetViewPos()
	{
		return GetAbsOrigin() + GetViewOffset();
	}

	int GetTeam()
	{
		DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
	}

	void SetGlowEnabled(bool t)
	{
		DYNVAR(n, bool, "DT_TFPlayer", "m_bGlowEnabled");
		return n.setValue(this, t);
	}

	byte GetLifeState()
	{
		DYNVAR_RETURN(byte, this, "DT_BasePlayer", "m_lifeState");
	}

	int GetHitboxSet()
	{
		DYNVAR_RETURN(int, this, "DT_BaseAnimating", "m_nHitboxSet");
	}

};

class IClientEntityList
{
public:
	template <typename t>
	t* GetClientEntity(int index)
	{
		typedef t*(__thiscall* GetClientEntityFn)(void*, int);
		return CallVirtualFunction<GetClientEntityFn>(this, 3)(this, index);
	}

	IClientEntity* GetClientEntityFromHandle(DWORD handle)
	{
		typedef IClientEntity* (__thiscall* GetClientEntityFromHandleFn)(void*, int);
		return CallVirtualFunction<GetClientEntityFromHandleFn>(this, 4)(this, handle);
	}

	int GetHighestEntityIndex()
	{
		typedef int(__thiscall* GetHighestEntityIndexFn)(void*);
		return CallVirtualFunction<GetHighestEntityIndexFn>(this, 6)(this);
	}
};

class Color
{
public:
	Color() : R(0), G(0), B(0), A(0)
	{
	}

	Color(int r, int g, int b, int a) : R(r), G(g), B(b), A(a)
	{
	}

	Color(int r, int g, int b) : R(r), G(g), B(b), A(255)
	{
	}

	static Color White(void){ return Color(249, 249, 249, 255); }
	static Color Black(void) { return Color(0, 0, 0, 255); }
	static Color Red(void) { return Color(255, 0, 0, 255); }
	static Color Green(void) { return Color(0, 255, 0, 255); }
	static Color Blue(void) { return Color(0, 0, 255, 255); }
	static Color Yellow(void) { return Color(255, 255, 0, 255); }

	int r(void) { return R; }
	int g(void) { return G; }
	int b(void) { return B; }
	int a(void) { return A; }

	Color& operator = (Color& c)
	{
		R = c.r();
		G = c.g();
		B = c.b();
		A = c.a();
		return *this;
	}

private:
	int R, G, B, A;
};

class fColor
{
public:
	fColor() : R(0.0f), G(0.0f), B(0.0f), A(0.0f)
	{
	}

	fColor(float r, float g, float b, float a) : R(r), G(g), B(b), A(a)
	{
	}

	fColor(float r, float g, float b) : R(r), G(g), B(b), A(1.0f)
	{
	}

	static fColor White(void){ return fColor(1.0f, 1.0f, 1.0f, 1.0f); }
	static fColor Black(void) { return fColor(0.0f, 0.0f, 0.0f, 1.0f); }
	static fColor Red(void) { return fColor(1.0f, 0.0f, 0.0f, 1.0f); }
	static fColor Green(void) { return fColor(0.0f, 1.0f, 0.0f, 1.0f); }
	static fColor Blue(void) { return fColor(0.0f, 0.0f, 1.0f, 1.0f); }
	static fColor Yellow(void) { return fColor(1.0f, 1.0f, 0.0f, 1.0f); }

	float r(void) { return R; }
	float g(void) { return G; }
	float b(void) { return B; }
	float a(void) { return A; }

	fColor& operator = (fColor& c)
	{
		R = c.r();
		G = c.g();
		B = c.b();
		A = c.a();
		return *this;
	}

private:
	float R, G, B, A;
};

class IPanel
{
public:
	const char* GetName(int panel)
	{
		typedef const char* (__thiscall* GetNameFn)(void*, int);
		return CallVirtualFunction<GetNameFn>(this, 36)(this, panel);
	}

	void SetMouseInputEnabled(unsigned int panel, bool state)
	{
		return CallVirtualFunction<void(__thiscall *)(PVOID, int, bool)>(this, 32)(this, panel, state);
	}
};

class ISurface
{
public:
	void DrawSetColor(Color clr)
	{
		typedef void(__thiscall* DrawSetColorFn)(void*, Color);
		CallVirtualFunction<DrawSetColorFn>(this, 11)(this, clr);
	}

	void DrawFilledRect(int x, int y, int w, int h)
	{
		typedef void(__thiscall* DrawFilledRectFn)(void*, int, int, int, int);
		CallVirtualFunction<DrawFilledRectFn>(this, 12)(this, x, y, w, h);
	}

	void DrawOutlinedRect(int x, int y, int w, int h)
	{
		typedef void(__thiscall* DrawOutlinedRectFn)(void*, int, int, int, int);
		CallVirtualFunction<DrawOutlinedRectFn>(this, 14)(this, x, y, w, h);
	}

	void DrawLine(int x0, int y0, int x1, int y1)
	{
		typedef void(__thiscall* DrawLineFn)(void*, int, int, int, int);
		CallVirtualFunction<DrawLineFn>(this, 15)(this, x0, y0, x1, y1);
	}

	void DrawPolyLine(int* x, int* y, int n)
	{
		typedef void(__thiscall* DrawPolyLineFn)(void*, int*, int*, int);
		CallVirtualFunction<DrawPolyLineFn>(this, 16)(this, x, y, n);
	}

	void DrawSetTextFont(unsigned long index)
	{
		typedef void(__thiscall* DrawSetTextFontFn)(void*, unsigned long);
		CallVirtualFunction<DrawSetTextFontFn>(this, 17)(this, index);
	}

	void DrawSetTextColor(Color clr)
	{
		typedef void(__thiscall* DrawSetTextColorFn)(void*, Color);
		CallVirtualFunction<DrawSetTextColorFn>(this, 19)(this, clr);
	}

	void DrawSetTextPos(int x, int y)
	{
		typedef void(__thiscall* DrawSetTextPosFn)(void*, int, int);
		CallVirtualFunction<DrawSetTextPosFn>(this, 20)(this, x, y);
	}

	void DrawPrintText(wchar_t* text, int length, FontDrawType_t drawtype = FontDrawType_t::FONT_DRAW_DEFAULT)
	{
		typedef void(__thiscall* DrawPrintTextFn)(void*, wchar_t*, int, FontDrawType_t);
		CallVirtualFunction<DrawPrintTextFn>(this, 22)(this, text, length, drawtype);
	}

	unsigned long CreateFnt()
	{
		typedef unsigned long(__thiscall* CreateFntFn)(void*);
		return CallVirtualFunction<CreateFntFn>(this, 66)(this);
	}

	void SetFontGlyphSet(unsigned long font, const char* windowsFontName, int tall, int weight, int blur, int scanlines, int flags, int nRangeMin = 0, int nRangeMax = 0)
	{
		typedef void(__thiscall* SetFontGlyphSetFn)(void*, unsigned long, const char*, int, int, int, int, int, int, int);
		CallVirtualFunction<SetFontGlyphSetFn>(this, 67)(this, font, windowsFontName, tall, weight, blur, scanlines, flags, nRangeMin, nRangeMax);
	}

	void GetTextSize(unsigned long font, const wchar_t* text, int& wide, int& tall)
	{
		typedef void(__thiscall* GetTextSizeFn)(void*, unsigned long, const wchar_t*, int&, int&);
		CallVirtualFunction<GetTextSizeFn>(this, 75)(this, font, text, wide, tall);
	}

	void GetCursorPosition(int &x, int &y)
	{
		typedef void(__thiscall* OriginalFn)(void*, int &, int &);
		return CallVirtualFunction<OriginalFn>(this, 96)(this, x, y);
	}

	void DrawOutlinedCircle(int x, int y, int radius, int segments)
	{
		typedef void(__thiscall *OriginalFn)(PVOID, int, int, int, int);
		return CallVirtualFunction<OriginalFn>(this, 99)(this, x, y, radius, segments);
	}

	void UnlockCursor()
	{
		return CallVirtualFunction<void(__thiscall *)(PVOID)>(this, 61)(this);
	}
	void LockCursor()
	{
		return CallVirtualFunction<void(__thiscall *)(PVOID)>(this, 62)(this);
	}

	void SetCursorAlwaysVis(bool vis)
	{
		return CallVirtualFunction<void(__thiscall *)(PVOID, bool)>(this, 52)(this, vis);
	}

};

struct csurface_t
{
	const char		*name;
	short			surfaceProps;
	unsigned short	flags;
};

struct cplane_t
{
	Vector3	normal;
	float	dist;
	BYTE	type;
	BYTE	signbits;
	BYTE	pad[2];
};

class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientEntity *pEntity, int contentsMask) = 0;
	virtual TraceType_t    GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		if (pEntityHandle->GetIndex() <= 32)
			return false;

		if (pEntityHandle->GetClientClass()->iClassID == 64 || pEntityHandle->GetClientClass()->iClassID == 55 || pEntityHandle->GetClientClass()->iClassID == 225 || pEntityHandle->GetClientClass()->iClassID == 180 || pEntityHandle->GetClientClass()->iClassID == 200 || pEntityHandle->GetClientClass()->iClassID == 86 || pEntityHandle->GetClientClass()->iClassID == 89)
			return false;

		return true;
	}
	virtual TraceType_t    GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
};

class CTraceEntity : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return (pEntityHandle == pHit);
	}
	virtual TraceType_t	GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}
	void* pHit;
};

class CTraceFilterSkipTwoEntities : public ITraceFilter
{
public:
	CTraceFilterSkipTwoEntities(void* pPassEnt1, void* pPassEnt2)
	{
		passentity1 = pPassEnt1;
		passentity2 = pPassEnt2;
	}

	virtual bool ShouldHitEntity(void* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == passentity1 || pEntityHandle == passentity2);
	}

	virtual TraceType_t GetTraceType() const
	{
		return  TRACE_EVERYTHING_FILTER_PROPS;
	}

	void* passentity1;
	void* passentity2;
};

class __declspec(align(16))VectorAligned : public Vector3
{
public:
	VectorAligned& operator=(const Vector3 &vOther)
	{
		Set(vOther.x, vOther.y, vOther.z);
		return *this;
	}
	float w;
};

struct Ray_t
{
	VectorAligned   m_Start;
	VectorAligned   m_Delta;
	VectorAligned   m_StartOffset;
	VectorAligned   m_Extents;

	bool    m_IsRay;
	bool    m_IsSwept;

	void Init(Vector3& start, Vector3& end)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents.Set();
		m_IsRay = true;

		m_StartOffset.Set();
		m_Start = start;
	}

	void Init(Vector3& start, Vector3& end, Vector3& mins, Vector3& maxs)
	{
		m_Delta = end - start;

		m_IsSwept = (m_Delta.LengthSqr() != 0);

		m_Extents = maxs - mins;
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSqr() < 1e-6);

		m_StartOffset = mins + maxs;
		m_StartOffset *= 0.5f;
		m_Start = start - m_StartOffset;
		m_StartOffset *= -1.0f;
	}
};

class CBaseTrace
{
public:
	bool IsDispSurface(void)				{ return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void)		{ return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void)		{ return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void)			{ return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void)			{ return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:
	Vector3			startpos;
	Vector3			endpos;
	cplane_t		plane;

	float			fraction;

	int				contents;
	unsigned short	dispFlags;

	bool			allsolid;
	bool			startsolid;

	CBaseTrace() {}

private:
	CBaseTrace(const CBaseTrace& vOther);
};

class CBaseCombatCharacter : public IClientEntity
{
public:
	void UpdateGlowEffect()
	{
		typedef void(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 226)(this);
	}
	void DestroyGlowEffect()
	{
		typedef void(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 227)(this);
	}
};

class CGameTrace : public CBaseTrace
{
public:
	bool DidHitWorld() const;

	bool DidHitNonWorldEntity() const;

	int GetEntityIndex() const;

	bool DidHit() const
	{
		return fraction < 1 || allsolid || startsolid;
	}

public:
	float			fractionleftsolid;
	csurface_t		surface;

	int				hitgroup;

	short			physicsbone;

	IClientEntity*	m_pEnt;
	int				hitbox;

	CGameTrace() {}
	CGameTrace(const CGameTrace& vOther);
};

class IEngineTrace
{
public:
	void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, trace_t* pTrace)//5
	{
		typedef void(__thiscall* TraceRayFn)(void*, const Ray_t&, unsigned int, ITraceFilter*, trace_t*);
		return CallVirtualFunction<TraceRayFn>(this, 4)(this, ray, fMask, pTraceFilter, pTrace);
	}
};

class IVModelInfoClient
{
public:
	model_t* GetModel(int index)
	{
		typedef model_t*(__thiscall* GetModelFn)(void*, int);
		return CallVirtualFunction<GetModelFn>(this, 1)(this, index);
	}

	int	GetModelIndex(const char* name)
	{
		typedef int(__thiscall* GetModelIndexFn)(void*, const char*);
		return CallVirtualFunction< GetModelIndexFn >(this, 2)(this, name);
	}

	const char* GetModelName(const model_t* model)
	{
		typedef const char* (__thiscall* GetModelNameFn)(void*, const model_t*);
		return CallVirtualFunction< GetModelNameFn >(this, 3)(this, model);
	}

	studiohdr_t* GetStudiomodel(const model_t* mod)
	{
		typedef studiohdr_t* (__thiscall* GetStudiomodelFn)(void*, const model_t*);
		return CallVirtualFunction< GetStudiomodelFn >(this, 28)(this, mod);
	}
};

class CGlobalVarsBase
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxclients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
};

class IPlayerInfoManager
{
public:
	CGlobalVarsBase* GetGlobalVars()
	{
		typedef CGlobalVarsBase*(__thiscall* GetGlobalVarsFn)(void*);
		return CallVirtualFunction<GetGlobalVarsFn>(this, 1)(this);
	}
};

enum class tf_hitbox : int
{
	head = 0,
	pelvis = 1,
	spine_0 = 2,
	spine_1 = 3,
	spine_2 = 4,
	spine_3 = 5,
	upperArm_L = 6,
	lowerArm_L = 7,
	hand_L = 8,
	upperArm_R = 9,
	lowerArm_R = 10,
	hand_R = 11,
	hip_L = 12,
	knee_L = 13,
	foot_L = 14,
	hip_R = 15,
	knee_R = 16,
	foot_R = 17,
};

struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};

struct dlight_t
{
	int     flags;
	Vector3 origin;
	float   radius;
	ColorRGBExp32   color;
	float   die;
	float   decay;
	float   minlight;
	int     key;
	int     style;
	Vector3 m_Direction;
	float   m_InnerAngle;
	float   m_OuterAngle;
	float GetRadius() const
	{
		return radius;
	}
	float GetRadiusSquared() const
	{
		return radius * radius;
	}
	float IsRadiusGreaterThanZero() const
	{
		return radius > 0.0f;
	}
};

class IVEffects
{
public:
	dlight_t* CL_AllocDlight(int key)
	{
		typedef dlight_t*(__thiscall* CL_AllocDlightFn)(PVOID, int);
		return CallVirtualFunction<CL_AllocDlightFn>(this, 4)(this, key);
	}
	dlight_t* CL_AllocElight(int key)
	{
		typedef dlight_t*(__thiscall* CL_AllocElightFn)(PVOID, int);
		return CallVirtualFunction<CL_AllocElightFn>(this, 5)(this, key);
	}
};

class CBaseCombatWeapon : public IClientEntity
{
public:

	int GetMaxClip1()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 318)(this);
	}

	int GetMaxClip2()
	{
		typedef int(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 319)(this);
	}

	int GetSlot()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 327)(this);
	}

	char *GetName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 329)(this);
	}

	char *GetPrintName()
	{
		typedef char *(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 330)(this);
	}

	Vector3 &GetBulletSpread()
	{
		typedef Vector3 &(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 286)(this);
	}

	int getWeaponID()
	{
		typedef int(__thiscall *OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 422)(this);
	}

	bool DoSwingTrace(trace_t &trace)
	{
		typedef bool(__thiscall * OriginalFn)(PVOID, trace_t &);
		return CallVirtualFunction<OriginalFn>(this, 445)(this, trace);
	}

	bool CalcIsAttackCriticalHelper()
	{
		typedef bool(__thiscall * OriginalFn)(PVOID);
		return CallVirtualFunction<OriginalFn>(this, 389)(this);
	}
};
class IAppSystem
{
public:
	// Here's where the app systems get to learn about each other
	virtual bool Connect(CreateInterfaceFn factory) = 0;
	virtual void Disconnect() = 0;

	// Here's where systems can access other interfaces implemented by this object
	// Returns NULL if it doesn't implement the requested interface
	virtual void *QueryInterface(const char *pInterfaceName) = 0;

	// Init, shutdown
	virtual int Init() = 0;
	virtual void Shutdown() = 0;
};
//===================================================================================================
class ICvar : public IAppSystem
{
public:
	// Allocate a unique DLL identifier
	virtual int AllocateDLLIdentifier() = 0;

	// Register, unregister commands
	virtual void			RegisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommand(ConCommandBase *pCommandBase) = 0;
	virtual void			UnregisterConCommands(int id) = 0;

	// If there is a +<varname> <value> on the command line, this returns the value.
	// Otherwise, it returns NULL.
	virtual const char*		GetCommandLineValue(const char *pVariableName) = 0;

	// Try to find the cvar pointer by name
	virtual ConCommandBase *FindCommandBase(const char *name) = 0;
	virtual const ConCommandBase *FindCommandBase(const char *name) const = 0;
	virtual ConVar			*FindVar(const char *var_name) = 0;
	virtual const ConVar	*FindVar(const char *var_name) const = 0;
	virtual ConCommand		*FindCommand(const char *name) = 0;
	virtual const ConCommand *FindCommand(const char *name) const = 0;

	// Get first ConCommandBase to allow iteration
	virtual ConCommandBase	*GetCommands(void) = 0;
	virtual const ConCommandBase *GetCommands(void) const = 0;

	// Install a global change callback (to be called when any convar changes)
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(ConVar *var, const char *pOldString, float flOldValue) = 0;

	// Install a console printer
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& clr, const char *pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char *pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char *pFormat, ...) const = 0;

	// Reverts cvars which contain a specific flag
	virtual void			RevertFlaggedConVars(int nFlag) = 0;

	// Method allowing the engine ICvarQuery interface to take over
	// A little hacky, owing to the fact the engine is loaded
	// well after ICVar, so we can't use the standard connect pattern
	virtual void			InstallCVarQuery(void *pQuery) = 0;

#if defined( _X360 )
	virtual void			PublishToVXConsole() = 0;
#endif
	virtual bool			IsMaterialThreadSetAllowed() const = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, const char *pValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, int nValue) = 0;
	virtual void			QueueMaterialThreadSetValue(ConVar *pConVar, float flValue) = 0;
	virtual bool			HasQueuedMaterialThreadConVarSets() const = 0;
	virtual int				ProcessQueuedMaterialThreadConVarSets() = 0;

protected:	class ICVarIteratorInternal;
public:
	/// Iteration over all cvars.
	/// (THIS IS A SLOW OPERATION AND YOU SHOULD AVOID IT.)
	/// usage:
	/// { ICVar::Iterator iter(g_pCVar);
	///   for ( iter.SetFirst() ; iter.IsValid() ; iter.Next() )
	///   {
	///       ConCommandBase *cmd = iter.Get();
	///   }
	/// }
	/// The Iterator class actually wraps the internal factory methods
	/// so you don't need to worry about new/delete -- scope takes care
	//  of it.
	/// We need an iterator like this because we can't simply return a
	/// pointer to the internal data type that contains the cvars --
	/// it's a custom, protected class with unusual semantics and is
	/// prone to change.
	class Iterator
	{
	public:
		inline Iterator(ICvar *icvar);
		inline ~Iterator(void);
		inline void		SetFirst(void);
		inline void		Next(void);
		inline bool		IsValid(void);
		inline ConCommandBase *Get(void);
	private:
		ICVarIteratorInternal *m_pIter;
	};

protected:
	// internals for  ICVarIterator
	class ICVarIteratorInternal
	{
	public:
		// warning: delete called on 'ICvar::ICVarIteratorInternal' that is abstract but has non-virtual destructor [-Wdelete-non-virtual-dtor]
		virtual ~ICVarIteratorInternal() {}
		virtual void		SetFirst(void) = 0;
		virtual void		Next(void) = 0;
		virtual	bool		IsValid(void) = 0;
		virtual ConCommandBase *Get(void) = 0;
	};

	virtual ICVarIteratorInternal	*FactoryInternalIterator(void) = 0;
	friend class Iterator;
};
class IPhysicsSurfaceProps
{
public:

	virtual ~IPhysicsSurfaceProps(void) {}

	// parses a text file containing surface prop keys

	virtual int		ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;

	// current number of entries in the database

	virtual int		SurfacePropCount(void) const = 0;

	virtual int		GetSurfaceIndex(const char *pSurfacePropName) const = 0;

	virtual void	GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;

	virtual /*surfacedata_t*/ void *GetSurfaceData(int surfaceDataIndex) = 0;

	virtual const char		*GetString(unsigned short stringTableIndex) const = 0;

	virtual const char		*GetPropName(int surfaceDataIndex) const = 0;

	// sets the global index table for world materials

	virtual void	SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient

	virtual void	GetPhysicsParameters(int surfaceDataIndex, /*surfacephysicsparams_t*/ void *pParamsOut) const = 0;

};

class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;
	virtual void AddBoxOverlay(const Vector3& origin, const Vector3& mins, const Vector3& max, Vector3 const& orientation, int r, int g, int b, int a, float duration) = 0;
	virtual void AddTriangleOverlay(const Vector3& p1, const Vector3& p2, const Vector3& p3, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddLineOverlay(const Vector3& origin, const Vector3& dest, int r, int g, int b, bool noDepthTest, float duration) = 0;
	virtual void AddTextOverlay(const Vector3& origin, float duration, const char *format, ...) = 0;
	virtual void AddTextOverlay(const Vector3& origin, int line_offset, float duration, const char *format, ...) = 0;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char *text) = 0;
	virtual void AddSweptBoxOverlay(const Vector3& start, const Vector3& end, const Vector3& mins, const Vector3& max, const Vector3 & angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void AddGridOverlay(const Vector3& origin) = 0;
	virtual int ScreenPosition(const Vector3& point, Vector3& screen) = 0;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector3& screen) = 0;

	virtual OverlayText_t *GetFirst(void) = 0;
	virtual OverlayText_t *GetNext(OverlayText_t *current) = 0;
	virtual void ClearDeadOverlays(void) = 0;
	virtual void ClearAllOverlays() = 0;

	virtual void AddTextOverlayRGB(const Vector3& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char *format, ...) = 0;
	virtual void AddTextOverlayRGB(const Vector3& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) = 0;

	virtual void AddLineOverlayAlpha(const Vector3& origin, const Vector3& dest, int r, int g, int b, int a, bool noDepthTest, float duration) = 0;
	virtual void AddBoxOverlay2(const Vector3& origin, const Vector3& mins, const Vector3& max, Vector3 const& orientation, const Color& faceColor, const Color& edgeColor, float duration) = 0;

private:
	inline void AddTextOverlay(const Vector3& origin, int line_offset, float duration, int r, int g, int b, int a, const char *format, ...) {} /* catch improper use of bad interface. Needed because '0' duration can be resolved by compiler to NULL format string (i.e., compiles but calls wrong function) */
};

class CInput
{
public:
	CUserCmd *GetUserCmd(int seq)
	{
		typedef CUserCmd *(__thiscall * OriginalFn)(PVOID, int);
		return CallVirtualFunction<OriginalFn>(this, 8)(this, seq);
	}
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	CRC32_t m_crc;
};

//--------------------------------------

class CControlVariables
{
public:

	bool ABEnabled = false;
	bool AutoShoot = false;
	int Hitbox = 0;
	int AimKey = 0;
	int FOV = 0;
	int Smoothness = 0;
	bool Silent = false;
	bool igncloak = true;
	bool removedisg = true;
	bool projpred = false;
	int projqual = 0;
	bool trigger = false;
	int key = NULL;
	bool button = false;

	//////////////////////////////////////////////

	bool Players = false;
	bool Bones = false;
	bool Objects = false;
	bool TraceLines = false;
	bool fov = false;
	Color lel;

	//////////////////////////////////////////////

	bool radar = false;
	int x = 0;
	int y = 0;
	int w = 0;

	//////////////////////////////////////////////

	bool Bhop = false;
	bool perfecthop = false;
	bool imperfecthop = false;
	bool autostrafe = false;
	bool thirdperson = false;
	bool chatsay = false;
	bool fakelag = false;
	bool fakelagadaptive = false;
	int fakelagcons = 0;
	bool airstuck = false;
	int airstuckkey = 0;
	bool airblast = false;

	//////////////////////////////////////////////

	bool FakeCenter = false;

};

extern CControlVariables gCvars;

class Hack_
{
public:
	std::shared_ptr<CNetVars> NetVars = nullptr;
	std::shared_ptr<CDrawings> Drawings = nullptr;
	std::shared_ptr<CMisc> Misc = nullptr;
	std::shared_ptr<CAimbot> Aimbot = nullptr;
	std::shared_ptr<CVars> CVars = nullptr;
public:
	std::shared_ptr<CHook> ClientModeHook = nullptr;
	std::shared_ptr<CHook> PanelHook = nullptr;
	std::shared_ptr<CHook> Client = nullptr;
};

class Interfaces_
{
public:
	IBaseClientDLL* Client = nullptr;
	IClientMode* ClientMode = nullptr;
	IClientEntityList* EntList = nullptr;
	IVEngineClient* Engine = nullptr;
	IPanel* Panel = nullptr;
	ISurface* Surface = nullptr;
	CGlobalVarsBase* Globals = nullptr;
	IPlayerInfoManager* PlayerInfoManager = nullptr;
	IEngineTrace* EngineTrace = nullptr;
	IVModelInfoClient* ModelInfo = nullptr;
	IVEffects* pEffects = nullptr;
	CPrediction* Prediction = nullptr;
	IGameMovement* GameMovement = nullptr;
	ICvar* Cvar = nullptr;
	IPhysicsSurfaceProps* Phys = nullptr;
	ISteamClient017* steamclient = nullptr;
	ISteamFriends002* steamfriends = nullptr;
	ISteamUser017* steamuser = nullptr;
	ISteamUserStats011* steamuserStats = nullptr;
	IVDebugOverlay* DebugOverlay = nullptr;
	CInput* Input = nullptr;
};

class NetVars_
{
public:
	DWORD m_rgflCoordinateFrame = 0;
	DWORD m_Collision = 0;
	DWORD m_vecOrigin = 0;
	DWORD m_vecViewOffset = 0;
	DWORD m_vecVelocity = 0;
	DWORD m_angRotation = 0;
	DWORD m_lifeState = 0;
	DWORD m_fFlags = 0;
	DWORD m_iHealth = 0;
	DWORD m_iTeamNum = 0;
	DWORD m_hActiveWeapon = 0;
	DWORD m_nHitboxSet = 0;
	DWORD m_PlayerClass = 0;
	DWORD m_iClass = 0;
	DWORD m_bGlowEnabled = 0;
	DWORD m_nForceTauntCam = 0;
	DWORD m_nPlayerCond = 0;
	DWORD m_bReadyToBackstab = 0;
	DWORD m_iDefaultFOV = 0;
	DWORD m_Shared = 0;
	DWORD m_iObjHealth = 0;
	DWORD m_iUpgradeLevel = 0;
	DWORD m_clrRender1 = 0;
	DWORD m_clrRender2 = 0;
	DWORD m_nTickBase = 0;
	DWORD m_iFOV = 0;
	DWORD m_vecAngles = 0;
	DWORD m_flChargedDamage = 0;
	DWORD m_flChargeBeginTime = 0;
	DWORD deadflag = 0;
	DWORD m_flHeadScale = 0;
	DWORD m_angEyeAngles = 0;
};

class ScreenSize_
{
public:
	int Width = 0;
	int Height = 0;
};

extern Hack_ Hack;
extern Interfaces_ Interfaces;
extern NetVars_ NetVars;
extern ScreenSize_ ScreenSize;

inline float flGetDistance(Vector3 vOrigin, Vector3 vLocalOrigin)
{
	Vector3 vDelta = vOrigin - vLocalOrigin;

	float m_fDistance = sqrt(vDelta.Length());

	if (m_fDistance < 1.0f)
		return 1.0f;

	return m_fDistance;
}

