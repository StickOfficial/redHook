#include "Misc.h"
#include "NetVars.h"
#include "Player.h"
#include "CBaseAPI.h"
#include "Drawings.h"

CMisc::CMisc()
{
}

CMisc::~CMisc()
{
}

bool CMisc::IsVisible(Vector3& vecAbsStart, Vector3& vecAbsEnd, IClientEntity* pLocal, IClientEntity* pEntity)
{
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	ray.Init(vecAbsStart, vecAbsEnd);
	Interfaces.EngineTrace->TraceRay(ray, 0x200400B, &filter, &tr);

	if (tr.m_pEnt == NULL)
		if (tr.fraction == 1.0f)
			return true;

	return (tr.m_pEnt->GetIndex() == pEntity->GetIndex());
}

bool CMisc::WorldToScreen(Vector3 &vOrigin, Vector3 &vScreen)
{
	const matrix3x4& worldToScreen = Interfaces.Engine->WorldToScreenMatrix();
	float w = worldToScreen[3][0] * vOrigin[0] + worldToScreen[3][1] * vOrigin[1] + worldToScreen[3][2] * vOrigin[2] + worldToScreen[3][3];
	vScreen.z = 0;

	if(w > 0.01)
	{
		float inverseWidth = 1 / w;

		vScreen.x = (ScreenSize.Width / 2) + (0.5 * ((worldToScreen[0][0] * vOrigin[0] + worldToScreen[0][1] * vOrigin[1] + worldToScreen[0][2] * vOrigin[2] + worldToScreen[0][3]) * inverseWidth) * ScreenSize.Width + 0.5);
		vScreen.y = (ScreenSize.Height / 2) - (0.5 * ((worldToScreen[1][0] * vOrigin[0] + worldToScreen[1][1] * vOrigin[1] + worldToScreen[1][2] * vOrigin[2] + worldToScreen[1][3]) * inverseWidth) * ScreenSize.Height + 0.5);

		return true;
	}

	return false;
}

void CMisc::InitNetVars()
{
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_CollisionGroup", &NetVars.m_rgflCoordinateFrame);
	NetVars.m_rgflCoordinateFrame -= 0x4C;

	Hack.NetVars->GetOffset("DT_BaseEntity", "m_Collision", &NetVars.m_Collision);
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_vecOrigin", &NetVars.m_vecOrigin);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_vecViewOffset[0]", &NetVars.m_vecViewOffset);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_vecVelocity[0]", &NetVars.m_vecVelocity);
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_angRotation", &NetVars.m_angRotation);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_lifeState", &NetVars.m_lifeState);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_fFlags", &NetVars.m_fFlags);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_iHealth", &NetVars.m_iHealth);
	Hack.NetVars->GetOffset("DT_BaseEntity", "m_iTeamNum", &NetVars.m_iTeamNum);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_hActiveWeapon", &NetVars.m_hActiveWeapon);
	Hack.NetVars->GetOffset("DT_BaseAnimating", "m_nHitboxSet", &NetVars.m_nHitboxSet);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_PlayerClass", &NetVars.m_PlayerClass);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_iClass", &NetVars.m_iClass);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_bGlowEnabled", &NetVars.m_bGlowEnabled);
	Hack.NetVars->GetOffset("DT_TFWeaponKnife", "m_bReadyToBackstab", &NetVars.m_bReadyToBackstab);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_nPlayerCond", &NetVars.m_nPlayerCond);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_nForceTauntCam", &NetVars.m_nForceTauntCam);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_iDefaultFOV", &NetVars.m_iDefaultFOV);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_Shared", &NetVars.m_Shared);
	Hack.NetVars->GetOffset("DT_TFPlayer", "m_clrRender", &NetVars.m_clrRender1);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_clrRender", &NetVars.m_clrRender2);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_nTickBase", &NetVars.m_nTickBase);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_iFOV", &NetVars.m_iFOV);
	Hack.NetVars->GetOffset("DT_BasePlayer", "m_vecAngles", &NetVars.m_vecAngles);
	Hack.NetVars->GetOffset("DT_TFSniperRifle", "m_flChargedDamage", &NetVars.m_flChargedDamage);
	Hack.NetVars->GetOffset("DT_WeaponPipebombLauncher", "m_flChargeBeginTime", &NetVars.m_flChargeBeginTime);
	Hack.NetVars->GetOffset("DT_BaseObject", "m_iHealth", &NetVars.m_iObjHealth);
	Hack.NetVars->GetOffset("DT_BaseObject", "m_iUpgradeLevel", &NetVars.m_iUpgradeLevel);
	Hack.NetVars->GetOffset("DT_BasePlayer", "deadflag", &NetVars.deadflag);
	NetVars.deadflag += 0x4;
	Hack.NetVars->GetOffset("DT_TFPlayer","m_flHeadScale", &NetVars.m_flHeadScale);
	Hack.NetVars->GetOffset("DT_TFNonLocalPlayerExclusive", "m_angEyeAngles[0]", &NetVars.m_angEyeAngles);
	

	gNetvars.init();

	iHealth = gNetvars.get_offset("DT_BasePlayer", "m_iHealth");
	iLifeState = gNetvars.get_offset("DT_BasePlayer", "m_lifeState");
	iTeam = gNetvars.get_offset("DT_BaseEntity", "m_iTeamNum");
	iClass = gNetvars.get_offset("DT_TFPlayer", "m_PlayerClass", "m_iClass");
	iFlags = gNetvars.get_offset("DT_BasePlayer", "m_fFlags");
	bGlowEnabled = gNetvars.get_offset("DT_TFPlayer", "m_bGlowEnabled");
	hActiveWeapon = gNetvars.get_offset("DT_BaseCombatCharacter", "m_hActiveWeapon");
	bReadyToBackstab = gNetvars.get_offset("DT_TFWeaponKnife", "m_bReadyToBackstab");
	vecViewOffset = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
	flChargedDamage = gNetvars.get_offset("DT_TFSniperRifle", "SniperRifleLocalData", "m_flChargedDamage");
	nTickBase = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_nTickBase");
	flNextPrimaryAttack = gNetvars.get_offset("DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
	flLastFireTime = gNetvars.get_offset("DT_TFWeaponBase", "LocalActiveTFWeaponData", "m_flLastFireTime");
	vecVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecVelocity[0]");
	vecVelocity = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_vecBaseVelocity");
	iPlayerCond = gNetvars.get_offset("DT_TFPlayer", "m_Shared", "m_nPlayerCond");
	vecPunchAngle = gNetvars.get_offset("DT_BasePlayer", "localdata", "m_Local", "m_vecPunchAngle");
	hOwner = gNetvars.get_offset("DT_BaseCombatWeapon", "m_hOwner"); // do this
	movetype = gNetvars.get_offset("DT_BaseEntity", "movetype");
	collision = gNetvars.get_offset("DT_BaseEntity", "m_Collision");
	rgflCoordinateFrame = gNetvars.get_offset("DT_BaseEntity", "m_CollisionGroup") - 0x4C;
	iRoundState = gNetvars.get_offset("DT_TeamplayRoundBasedRulesProxy", "teamplayroundbased_gamerules_data", "m_iRoundState");
	iFov = gNetvars.get_offset("DT_BasePlayer", "m_iFOV");
	angRotation = gNetvars.get_offset("DT_BaseEntity", "m_angRotation");
	flChargeBeginTime = gNetvars.get_offset("DT_WeaponPipebombLauncher", "PipebombLauncherLocalData", "m_flChargeBeginTime");
	flAnimTime = gNetvars.get_offset("DT_BaseEntity", "AnimTimeMustBeFirst", "m_flAnimTime");
	flSimulationTime = gNetvars.get_offset("DT_BaseEntity", "m_flSimulationTime");
	iItemDefinitionIndex = gNetvars.get_offset("DT_EconEntity", "m_AttributeManager", "m_Item", "m_iItemDefinitionIndex");
}

DWORD FindPattern(DWORD dwAddress, DWORD dwLength, const char* szPattern)
{
	if(!dwAddress || !dwLength || !szPattern)
		return 0;

	const char* pat = szPattern;
	DWORD firstMatch = NULL;

	for(DWORD pCur = dwAddress; pCur < dwLength; pCur++)
	{
		if(!*pat)
			return firstMatch;

		if(*(PBYTE)pat == '\?' || *(BYTE*)pCur == GetByte(pat))
		{
			if(!firstMatch)
				firstMatch = pCur;

			if(!pat[2])
				return firstMatch;

			if(*(PWORD)pat == '\?\?' || *(PBYTE)pat != '\?')
				pat += 3;

			else pat += 2;
		}
		else
		{
			pat = szPattern;
			firstMatch = 0;
		}
	}

	return 0;
}

DWORD CMisc::FindSignature(const char* szModuleName, char* szPattern)
{
	HMODULE hModule = GetModuleHandleA(szModuleName);
	PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
	PIMAGE_NT_HEADERS pNTHeaders = (PIMAGE_NT_HEADERS)(((DWORD)hModule) + pDOSHeader->e_lfanew);

	return FindPattern(((DWORD)hModule) + pNTHeaders->OptionalHeader.BaseOfCode, ((DWORD)hModule) + pNTHeaders->OptionalHeader.SizeOfCode, szPattern);
}

bool CMisc::bCanHit(CUserCmd* cmd)
{
	CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
	trace_t tr;
	Ray_t ray;
	Vector3 eye = pLocal->GetEyePosition() , end, angles;

	Interfaces.Engine->GetViewAngles(angles);

	Hack.Misc->AngleVectors(angles, &end);
	float sp, sy, cp, cy;
	sy = sinf(DEG2RAD(cmd->viewangles[1])); // yaw
	cy = cosf(DEG2RAD(cmd->viewangles[1]));

	sp = sinf(DEG2RAD(cmd->viewangles[0])); // pitch
	cp = cosf(DEG2RAD(cmd->viewangles[0]));

	end.x = cp * cy;
	end.y = cp * sy;
	end.z = -sp;
	end = end * 8192.0f + eye;

	ray.Init(eye, end);
	Interfaces.EngineTrace->TraceRay(ray, 0x46004003, NULL, &tr);

	if (tr.m_pEnt->cast_to<CPlayer>()->IsValid(pLocal))
	{
		if (tr.hitgroup > 0)
			return true;
	}

	return false;
}

float CMisc::Clamp(float v, float mmin, float mmax)
{
	if(v > mmax) return mmax;
	if(v < mmin) return mmin;

	return v;
}

float CMisc::Rad2Deg(float x)
{
	return (x * (180.0f / PI));
}

float CMisc::Deg2Rad(float x)
{
	return (x * (PI / 180.0f));
}

void CMisc::SinCos(float x, float* s, float* c)
{
	//*s = sin(x);
	//*c = cos(x);

	__asm
	{
		fld dword ptr[x]
		fsincos
		mov edx, dword ptr[c]
		mov eax, dword ptr[s]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void CMisc::SinCos(float x, float &s, float &c)
{
	//s = sin(x);
	//c = cos(x);

	__asm
	{
		fld dword ptr[x]
		fsincos
		mov edx, dword ptr[c]
		mov eax, dword ptr[s]
		fstp dword ptr[edx]
		fstp dword ptr[eax]
	}
}

void CMisc::AngleVectors(const Vector3& angles, Vector3* forward)
{
	float sp, sy, cp, cy;

	SinCos(Deg2Rad(angles.x), &sp, &cp);
	SinCos(Deg2Rad(angles.y), &sy, &cy);

	if(forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}
}

void CMisc::AngleVectors(const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up)
{
	float sr, sp, sy, cr, cp, cy;

	SinCos(Deg2Rad(angles.x), &sp, &cp);
	SinCos(Deg2Rad(angles.y), &sy, &cy);
	SinCos(Deg2Rad(angles.z), &sr, &cr);

	if(forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if(right)
	{
		right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
		right->y = (-1 * sr * sp * sy + -1 * cr * cy);
		right->z = -1 * sr * cp;
	}

	if(up)
	{
		up->x = (cr * sp * cy + -sr * -sy);
		up->y = (cr * sp * sy + -sr * cy);
		up->z = cr * cp;
	}
}

void CMisc::VectorAngles(const Vector3& forward, Vector3& angles)
{
	float tmp, yaw, pitch;

	if(forward.y == 0 && forward.x == 0)
	{
		yaw = 0;

		if(forward.z > 0)
			pitch = 270;
		else
			pitch = 90;
	}
	else
	{
		yaw = Rad2Deg(atan2f(forward.y, forward.x));

		if(yaw < 0)
			yaw += 360;

		tmp = forward.Length2d();
		pitch = Rad2Deg(atan2f(-forward.z, tmp));

		if(pitch < 0)
			pitch += 360;
	}

	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

float CMisc::VectorNormalize(Vector3& v)
{
	float flLength = v.Length();

	if(!flLength)
		v.Set();
	else
		v /= flLength;

	return flLength;
}

void CMisc::AngleNormalize(Vector3& v)
{
	for(auto i = 0; i < 3; i++)
	{
		if(v[i] < -180.0f) v[i] += 360.0f;
		if(v[i] >  180.0f) v[i] -= 360.0f;
	}
}

void CMisc::NormalizeAngles(Vector3& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

void CMisc::CorrectMovement(Vector3 vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove)
{
	float deltaView;
	float f1;
	float f2;

	if (vOldAngles.y < 0.f)
		f1 = 360.0f + vOldAngles.y;
	else
		f1 = vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);

	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(DEG2RAD(deltaView)) * fOldForward + cos(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
	pCmd->sidemove = sin(DEG2RAD(deltaView)) * fOldForward + sin(DEG2RAD(deltaView + 90.f)) * fOldSidemove;
}

void CMisc::ClampAngles(Vector3& v)
{
	if(v.x > 89.0f && v.x <= 180.0f)
		v.x = 89.0f;

	if(v.x > 180.0f)
		v.x = v.x - 360.0f;

	if(v.x < -89.0f)
		v.x = -89.0f;

	v.y = fmodf(v.y + 180, 360) - 180;

	v.z = 0;
}

void CMisc::VectorTransform(const Vector3& vSome, const matrix3x4& vMatrix, Vector3& vOut)
{
	for(auto i = 0; i < 3; i++)
		vOut[i] = vSome.Dot((Vector3&)vMatrix[i]) + vMatrix[i][3];
}

Vector3 CMisc::EstimateAbsVelocity(CPlayer *ent)
{
	typedef void(__thiscall * EstimateAbsVelocityFn)(CPlayer *, Vector3 &);

	static DWORD dwFn = Hack.Misc->FindSignature("client.dll","E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;

	static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);

	EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;

	Vector3 v;

	vel(ent, v);

	return v;
}

void CMisc::runSimulation(CPrediction *pred, int curr_cmd, float curTime, CUserCmd *cmd, CPlayer *pBaseEnt)
{
	typedef void(__thiscall * runSimulationfn)(CPrediction *, int, float, CUserCmd *, CPlayer *);

	static DWORD dwRunSim = Hack.Misc->FindSignature("client.dll", "55 8B EC 51 53 56 8B 75 14 57");

	static runSimulationfn func = (runSimulationfn)dwRunSim;

	func(pred, curr_cmd, curTime, cmd, pBaseEnt);

	return;
}

void CMisc::safeRunSimulation(CPrediction *pred, CUserCmd *pCommand, CPlayer *pBaseEnt)
{
	float frameTime = Interfaces.Globals->frametime;

	float curTime = Interfaces.Globals->curtime;

	Interfaces.Globals->curtime = pBaseEnt->GetTickBase() * Interfaces.Globals->interval_per_tick;

	Interfaces.Globals->frametime = Interfaces.Globals->interval_per_tick;

	runSimulation(pred, pCommand->command_number, curTime, pCommand, pBaseEnt);

	Interfaces.Globals->frametime = frameTime;

	Interfaces.Globals->curtime = curTime;
}

void CMisc::safeRunSimulation(CPrediction *pred, CPlayer *pBaseEnt)
{
	float frameTime = Interfaces.Globals->frametime;

	float curTime = Interfaces.Globals->curtime;

	Interfaces.Globals->curtime = pBaseEnt->GetTickBase() * Interfaces.Globals->interval_per_tick;

	Interfaces.Globals->frametime = Interfaces.Globals->interval_per_tick;

	CUserCmd cmd;

	runSimulation(pred, cmd.command_number, curTime, &cmd, pBaseEnt);

	Interfaces.Globals->frametime = frameTime;

	Interfaces.Globals->curtime = curTime;

}


Vector3 CMisc::EstimateAbsVelocity(IClientEntity *ent)
{
	typedef void(__thiscall * EstimateAbsVelocityFn)(IClientEntity *, Vector3 &);

	static DWORD dwFn = Hack.Misc->FindSignature("client.dll", "E8 ? ? ? ? F3 0F 10 4D ? 8D 85 ? ? ? ? F3 0F 10 45 ? F3 0F 59 C9 56 F3 0F 59 C0 F3 0F 58 C8 0F 2F 0D ? ? ? ? 76 07") + 0x1;

	static DWORD dwEstimate = ((*(PDWORD)(dwFn)) + dwFn + 4);

	EstimateAbsVelocityFn vel = (EstimateAbsVelocityFn)dwEstimate;

	Vector3 v;

	vel(ent, v);

	return v;
}

void CMisc::ChatSpamReports()
{
	// Don't spam it too fast so you can still do stuff
	static clock_t start_t = clock();
	double timeSoFar = (double)(clock() - start_t) / CLOCKS_PER_SEC;
	if (timeSoFar < 0.5)
		return;

	// Loop through all active entitys
	std::vector < std::string > Names;

	for (int i = 0; i < Interfaces.Engine->GetMaxClients(); i++)
	{
		// Get the entity
		CPlayer* ent = Interfaces.EntList->GetClientEntity<CPlayer>(i);

		player_info_t pInfo;
		// If it's a valid entity and isn't the player

		CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());

		if (ent && pLocal->GetTeam() != ent->GetTeam())
		{

			// If entity is a player
			if (ent->GetClientClass()->iClassID == 241)
			{
				if (Interfaces.Engine->GetPlayerInfo(i, &pInfo))
				{
					if (!strstr(pInfo.name, "GOTV"))
						Names.push_back(pInfo.name);
				}
			}
		}
	}

	int randomIndex = rand() % Names.size();
	char buffer[128];
	static unsigned long int meme = 69;
	sprintf_s(buffer, "say Report for %s submitted, Report ID %lu.", Names[randomIndex].c_str(), meme);
	meme += 1;
	Interfaces.Engine->ClientCmd_Unrestricted(buffer);
	start_t = clock();
}