#include "SDK.h"

class CPlayer;
class CMisc
{
public:
	CMisc();
	~CMisc();

	//ehh??
	bool IsVisible(Vector3& vecAbsStart, Vector3& vecAbsEnd, IClientEntity* pLocal, IClientEntity* pBaseEnt);
	bool WorldToScreen(Vector3 &vOrigin, Vector3 &vScreen);

	//NetVars
	void InitNetVars();

	//Other
	DWORD FindSignature(const char* szModuleName, char* szPattern);

	//Triggerbot
	bool bCanHit(CUserCmd* cmd);

	//Math stuff
	float Clamp(float v, float mmin, float mmax);
	float Deg2Rad(float x);
	float Rad2Deg(float x);
	void SinCos(float x, float* s, float* c);
	void SinCos(float x, float &s, float &c);
	void AngleVectors(const Vector3& angles, Vector3* forward);
	void AngleVectors(const Vector3& angles, Vector3* forward, Vector3* right, Vector3* up);
	void VectorAngles(const Vector3& forward, Vector3& angles);
	float VectorNormalize(Vector3& v);
	void AngleNormalize(Vector3& v);
	void ClampAngles(Vector3& v);
	void VectorTransform(const Vector3& vSome, const matrix3x4& vMatrix, Vector3& vOut);
	void NormalizeAngles(Vector3& angle);
	//k

	DWORD iWeaponID;
	DWORD bHasMeleeWeapon;
	DWORD bHasFlameThrower;

	DWORD bGlowEnabled;
	DWORD iTeam;
	DWORD iLifeState;
	DWORD iHealth;
	DWORD iClass;
	DWORD bReadyToBackstab;
	DWORD rgflCoordinateFrame;
	DWORD collision;
	DWORD movetype;

	DWORD iWeaponSlot;
	DWORD iPlayerCond;
	DWORD iPlayerCondEx;
	DWORD iFlags;
	DWORD chName;
	DWORD iFov;

	DWORD vecViewOffset;
	DWORD vecVelocity;
	DWORD vecBaseVelocity;
	DWORD vecPunchAngle;
	DWORD angRotation;

	DWORD flChargedDamage;
	DWORD nTickBase;
	DWORD flNextPrimaryAttack;
	DWORD flLastFireTime;
	DWORD flChargeBeginTime;
	DWORD flAnimTime;
	DWORD flSimulationTime;

	DWORD hActiveWeapon;
	DWORD hOwner;
	DWORD bLowered;

	DWORD iItemDefinitionIndex;

	DWORD iRoundState;

	//Pasted Stuff
	Vector3 EstimateAbsVelocity(CPlayer *ent);
	Vector3 EstimateAbsVelocity(IClientEntity *ent);
	void CorrectMovement(Vector3 vOldAngles, CUserCmd* pCmd, float fOldForward, float fOldSidemove);
	void runSimulation(CPrediction *pred, int curr_cmd, float curTime, CUserCmd *cmd, CPlayer *pBaseEnt);
	void safeRunSimulation(CPrediction *pred, CUserCmd *pCommand, CPlayer *pBaseEnt);
	void safeRunSimulation(CPrediction *pred, CPlayer *pBaseEnt);
	void ChatSpamReports();

};