#include "SDK.h"

class CPlayer : public IClientEntity
{
public:
	bool IsValid();
	bool IsValid(CPlayer* pPlayer);

	Vector3 GetOrigin();
	Vector3 GetEyePosition();
	Vector3 GetEyeAngles();
	Vector3 GetVelocity();
	Vector3 GetAnglesRotation();
	Vector3 GetBonePosition(int bone);
	Vector3 GetHitboxPosition(int hitbox);
	Vector3 GetHitboxPositionForAimbot(int hitbox);
	bool GetHitboxBound(int hitbox, Vector3& vMin, Vector3& vMax);
	byte GetLifeState();
	int GetFlags();
	int GetHealth();
	int GetMaxHealth();
	int GetTeam();
	int GetHitboxSet();
	int GetUserId();
	int GetClassNum();
	int GetPlayerCond();
	int FOV();
	const char* GetClass();
	IClientEntity* GetActiveWeapon();
	bool CanAutostrafe();
	const char* GetModelName();
};