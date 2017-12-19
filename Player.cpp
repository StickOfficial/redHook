#include "Player.h"
#include "NetVars.h"
#include "Misc.h"

/*IClientEntity* CPlayer::GetBaseEntity()
{
	return reinterpret_cast<IClientEntity*>(this);
}*/

bool CPlayer::IsValid()
{
	if(!this)
		return false;

	if(this->GetHealth() < 1)
		return false;

	if(this->GetLifeState() != LIFE_ALIVE)
		return false;

	if(this->IsDormant())
		return false;

	return true;
}

bool CPlayer::IsValid(CPlayer* pPlayer)
{
	if(!this->IsValid())
		return false;

	if(this->GetIndex() == pPlayer->GetIndex())
		return false;

	if(this->GetTeam() == pPlayer->GetTeam())
		return false;

	if (this->GetPlayerCond() & TFCond_Cloaked)
		return false;

	return true;
}

Vector3 CPlayer::GetOrigin()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecOrigin);
}

Vector3 CPlayer::GetEyePosition()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecViewOffset) + GetOrigin();
}

Vector3 CPlayer::GetEyeAngles()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_angEyeAngles);
}

Vector3 CPlayer::GetVelocity()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_vecVelocity);
}

Vector3 CPlayer::GetAnglesRotation()
{
	return *reinterpret_cast<Vector3*>(this + NetVars.m_angRotation);
}

Vector3 CPlayer::GetBonePosition(int bone)
{
	matrix3x4 matrix[128];

	this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime);

	return Vector3(matrix[bone][0][3], matrix[bone][1][3], matrix[bone][2][3]);
}

Vector3 CPlayer::GetHitboxPosition(int hitbox)
{
	model_t* model = this->GetModel();
	if(!model)
		return Vector3();

	studiohdr_t* hdr = Interfaces.ModelInfo->GetStudiomodel(model);
	if(!hdr)
		return Vector3();

	matrix3x4 matrix[128];
	if(!this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime))
		return Vector3();

	mstudiohitboxset_t* set = hdr->pHitboxSet(GetHitboxSet());
	if(!set)
		return Vector3();

	mstudiobbox_t* box = set->pHitbox(hitbox);
	if(!box)
		return Vector3();

	Vector3 center = (box->bbmin + box->bbmax) * 0.5f;

	Vector3 hitboxpos;

	Hack.Misc->VectorTransform(center, matrix[box->bone], hitboxpos);

	return hitboxpos;
}

Vector3 CPlayer::GetHitboxPositionForAimbot(int hitbox)
{
	model_t* model = this->GetModel();
	if (!model)
		return Vector3();

	studiohdr_t* hdr = Interfaces.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector3();

	matrix3x4 matrix[128];
	if (!this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime))
		return Vector3();

	mstudiohitboxset_t* set = hdr->pHitboxSet(GetHitboxSet());
	if (!set)
		return Vector3();

	mstudiobbox_t* box = set->pHitbox(hitbox);
	if (!box)
		return Vector3();

	Vector3 center = (box->bbmin + box->bbmax) * 0.5f;

	Vector3 hitboxpos;

	Hack.Misc->VectorTransform(center, matrix[box->bone], hitboxpos);

	return hitboxpos;
}

bool CPlayer::GetHitboxBound(int hitbox, Vector3& vMin, Vector3& vMax)
{
	model_t* model = this->GetModel();
	if(!model)
		return false;

	studiohdr_t* pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(model);
	if(!pStudioHdr)
		return false;

	matrix3x4 matrix[128];
	if(!this->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime))
		return false;

	mstudiohitboxset_t* set = pStudioHdr->pHitboxSet(0);
	if(!set)
		return false;

	mstudiobbox_t* pbox = set->pHitbox(hitbox);

	Hack.Misc->VectorTransform(pbox->bbmin, matrix[pbox->bone], vMin);
	Hack.Misc->VectorTransform(pbox->bbmax, matrix[pbox->bone], vMax);

	return true;
}

byte CPlayer::GetLifeState()
{
	return *reinterpret_cast<byte*>(this + NetVars.m_lifeState);
}

int CPlayer::GetFlags()
{
	return *reinterpret_cast<int*>(this + NetVars.m_fFlags);
}

int CPlayer::GetHealth()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
}

int CPlayer::GetPlayerCond()
{
	return *reinterpret_cast<int*>(this + NetVars.m_nPlayerCond);
}

int CPlayer::FOV()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iDefaultFOV);
}

int CPlayer::GetMaxHealth()
{
	if(this->GetClassNum() == TF2_Scout)
		return 125;
	else if(this->GetClassNum() == TF2_Sniper)
		return 125;
	else if(this->GetClassNum() == TF2_Soldier)
		return 200;
	else if(this->GetClassNum() == TF2_Demoman)
		return 175;
	else if(this->GetClassNum() == TF2_Medic)
		return 150;
	else if(this->GetClassNum() == TF2_Heavy)
		return 300;
	else if(this->GetClassNum() == TF2_Pyro)
		return 175;
	else if(this->GetClassNum() == TF2_Spy)
		return 125;
	else if(this->GetClassNum() == TF2_Engineer)
		return 125;

	return 100;
}

int CPlayer::GetTeam()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iTeamNum);
}

int CPlayer::GetHitboxSet()
{
	return *reinterpret_cast<int*>(this + NetVars.m_nHitboxSet);
}

int CPlayer::GetUserId()
{
	player_info_t player_info;

	if(Interfaces.Engine->GetPlayerInfo(GetIndex(), &player_info))
		return player_info.userID;

	return -1;
}

int CPlayer::GetClassNum()
{
	return *reinterpret_cast<int*>(this + NetVars.m_iClass);
}

const char* CPlayer::GetClass()
{
	if(this->GetClassNum() == TF2_Scout)
		return "Scout";
	else if(this->GetClassNum() == TF2_Sniper)
		return "Sniper";
	else if(this->GetClassNum() == TF2_Soldier)
		return "Soldier";
	else if(this->GetClassNum() == TF2_Demoman)
		return "Demoman";
	else if(this->GetClassNum() == TF2_Medic)
		return "Medic";
	else if(this->GetClassNum() == TF2_Heavy)
		return "Heavy";
	else if(this->GetClassNum() == TF2_Pyro)
		return "Pyro";
	else if(this->GetClassNum() == TF2_Spy)
		return "Spy";
	else if(this->GetClassNum() == TF2_Engineer)
		return "Engineer";

	return "null";
}

IClientEntity* CPlayer::GetActiveWeapon()
{
	DWORD hWeapon = *reinterpret_cast<DWORD*>(this + NetVars.m_hActiveWeapon);

	hWeapon &= 0xFFF;

	return Interfaces.EntList->GetClientEntity<IClientEntity>(hWeapon);
}

bool CPlayer::CanAutostrafe()
{
	if(this->GetClassNum() == TF2_Scout)
		return false;

	return true;
}

const char* CPlayer::GetModelName()
{
	return Interfaces.ModelInfo->GetModelName(this->GetModel());
}
