#include "Aimbot.h"
#include "Misc.h"
#include "Player.h"
#include "WeaponList.h"

CAimbot::CAimbot()
{
	m_pCmd = nullptr;

	m_pLocal = nullptr;

	m_iBestIndex = -1;
	m_flBestTarget = 0.0f;

	m_iTimeCounter = 0;
}

CAimbot::~CAimbot()
{
}

void MakeVector(const Vector3& vIn, Vector3& vOut)
{
	float pitch = Hack.Misc->Deg2Rad(vIn.x);
	float yaw = Hack.Misc->Deg2Rad(vIn.y);
	float temp = cos(pitch);

	vOut.x = -temp * -cos(yaw);
	vOut.y = sin(yaw) * temp;
	vOut.z = -sin(pitch);
}

Vector3 GetHitbox(IClientEntity* ent, int hitbox)
{
	model_t* model = ent->GetModel();
	if (!model)
		return Vector3();

	studiohdr_t* hdr = Interfaces.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return Vector3();

	matrix3x4 matrix[128];
	if (!ent->SetupBones(matrix, 128, 0x100, Interfaces.Globals->curtime))
		return Vector3();

	mstudiohitboxset_t* set = hdr->pHitboxSet(ent->GetHitboxSet());
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

void CAimbot::AimAt(Vector3 origin, Vector3 target, CUserCmd* cmd) 
{
	Vector3 angles;
	Vector3 tr = (target - origin);
	Hack.Misc->VectorAngles(tr, angles);
	Hack.Misc->ClampAngles(angles);
	cmd->viewangles = angles;
}

void CAimbot::AimAtHitbox(IClientEntity* ent, int hitbox, CUserCmd* cmd)
{
	Vector3 r = ent->GetVecOrigin();
	r = GetHitbox(ent, hitbox);
	IClientEntity* local = Interfaces.EntList->GetClientEntity<IClientEntity>(Interfaces.Engine->GetLocalPlayer());
	AimAt(local->GetViewPos(), r, cmd);
}

float PlayerGravityMod(IClientEntity* player)
{
	int movetype = player->GetMoveType();
	if (movetype == MOVETYPE_FLY || movetype == MOVETYPE_NOCLIP) 
		return 0.0f;

	return 1.0f;
}

float VectorialDistanceToGround(Vector3 origin)
{
	static trace_t* ground_trace = new trace_t();
	Ray_t ray;
	Vector3 endpos = origin;
	endpos.z -= 8192;
	ray.Init(origin, endpos);
	Interfaces.EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, NULL, ground_trace);
	return 8192.0f * ground_trace->fraction;
}

float DistanceToGround(IClientEntity* ent)
{
	if (ent->GetFlags() & FL_ONGROUND) return 0;

	Vector3& origin = ent->GetVecOrigin();
	float v1 = VectorialDistanceToGround(origin + Vector3(10.0f, 10.0f, 0.0f));
	float v2 = VectorialDistanceToGround(origin + Vector3(-10.0f, 10.0f, 0.0f));
	float v3 = VectorialDistanceToGround(origin + Vector3(10.0f, -10.0f, 0.0f));
	float v4 = VectorialDistanceToGround(origin + Vector3(-10.0f, -10.0f, 0.0f));
	return MIN(v1, MIN(v2, MIN(v3, v4)));
}

Vector3 SimpleLatencyPrediction(IClientEntity* ent, int hb)
{
	if (!ent) return Vector3();
	Vector3 result;
	result = GetHitbox(ent, hb);
	float latency = Interfaces.Engine->GetNetChannelInfo()->GetLatency(FLOW_OUTGOING) +
		Interfaces.Engine->GetNetChannelInfo()->GetLatency(FLOW_INCOMING);
	Vector3 vel = Hack.Misc->EstimateAbsVelocity(ent);
	result += vel * latency;
	return result;
}

Vector3 ProjectilePrediction(IClientEntity* ent, IClientEntity* local, int hb, float speed, float gravitymod, float entgmod) {
	if (!ent) return Vector3();
	Vector3 result = SimpleLatencyPrediction(ent, hb);
	if (speed == 0.0f) return Vector3();
	float dtg = DistanceToGround(ent);
	Vector3 vel = Hack.Misc->EstimateAbsVelocity(ent);
	float medianTime = local->GetViewPos().DistTo(result) / speed;
	float range = 1.5f;
	float currenttime = medianTime - range;
	if (currenttime <= 0.0f) currenttime = 0.01f;
	float besttime = currenttime;
	float mindelta = 65536.0f;
	Vector3 bestpos = result;
	int maxsteps = 300;
	for (int steps = 0; steps < maxsteps; steps++, currenttime += ((float)(2 * range) / (float)maxsteps)) 
	{
		Vector3 curpos = result;
		curpos += vel * currenttime;
		if (dtg > 0.0f)
		{
			curpos.z -= currenttime * currenttime * 400 * entgmod;
			if (curpos.z < result.z - dtg) curpos.z = result.z - dtg;
		}
		float rockettime = local->GetViewPos().DistTo(curpos) / speed;
		if (fabs(rockettime - currenttime) < mindelta) 
		{
			besttime = currenttime;
			bestpos = curpos;
			mindelta = fabs(rockettime - currenttime);
		}
	}
	bestpos.z += (400 * besttime * besttime * gravitymod);
	return bestpos;
}

float CAimbot::GetFOV(const Vector3& viewangles, const Vector3& vStart, const Vector3& vEnd)
{
	Vector3 vAng, vAim;

	Vector3 vDir = vEnd - vStart;

	Hack.Misc->VectorNormalize(vDir);

	Hack.Misc->VectorAngles(vDir, vAng);

	MakeVector(viewangles, vAim);
	MakeVector(vAng, vAng);

	return Hack.Misc->Rad2Deg(acos(vAim.Dot(vAng)) / vAim.LengthSqr());
}

void FixMove(CUserCmd* pCmd, Vector3 m_vOldAngles, float m_fOldForward, float m_fOldSidemove) 
{
	float deltaView = pCmd->viewangles.y - m_vOldAngles.y;
	float f1;
	float f2;

	if (m_vOldAngles.y < 0.f)
		f1 = 360.0f + m_vOldAngles.y;
	else
		f1 = m_vOldAngles.y;

	if (pCmd->viewangles.y < 0.0f)
		f2 = 360.0f + pCmd->viewangles.y;
	else
		f2 = pCmd->viewangles.y;

	if (f2 < f1)
		deltaView = abs(f2 - f1);
	else
		deltaView = 360.0f - abs(f1 - f2);
	deltaView = 360.0f - deltaView;

	pCmd->forwardmove = cos(Hack.Misc->Deg2Rad(deltaView)) * m_fOldForward + cos(Hack.Misc->Deg2Rad(deltaView + 90.f)) * m_fOldSidemove;
	pCmd->sidemove = sin(Hack.Misc->Deg2Rad(deltaView)) * m_fOldForward + sin(Hack.Misc->Deg2Rad(deltaView + 90.f)) * m_fOldSidemove;
}

void CAimbot::Main(CUserCmd* pCmd)
{
	if (gCvars.Hitbox == 0)
		return;

	if (gCvars.AimKey == 0)
		return;

	m_pCmd = pCmd;
	if (!m_pCmd)
		return;

	m_pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
	if (!m_pLocal)
		return;

	if (gCvars.AimKey == 1)
	{
		if (!GetAsyncKeyState(VK_LSHIFT) || m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}
	else if (gCvars.AimKey == 2)
	{
		if (!GetAsyncKeyState(0x06) || m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}
	else if (gCvars.AimKey == 3)
	{
		if (!GetAsyncKeyState(0x05) || m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}
	else if (gCvars.AimKey == 4)
	{
		if (!GetAsyncKeyState(VK_LBUTTON) || m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}
	else
	{
		if (m_iBestIndex == -1)
		{
			m_iTimeCounter = 0;
			GetBestTarget();
		}
	}

	DropTarget();

	Vector3 m_vOldViewAngle = pCmd->viewangles;
	float m_fOldSideMove = pCmd->sidemove;
	float m_fOldForwardMove = pCmd->forwardmove;

	if (m_iBestIndex != -1)
	{
		Vector3 vAim;

		Hack.Misc->VectorAngles((m_vEnd - m_pLocal->GetEyePosition()), vAim);

		Hack.Misc->ClampAngles(vAim);

		m_pCmd->viewangles = vAim;

		if (!gCvars.Silent)
		{
			Interfaces.Engine->SetViewAngles(m_pCmd->viewangles);
		}

		if (gCvars.Silent == 0 && gCvars.Smoothness > 0.0f)
		{
			Vector3 vDelta(m_pCmd->viewangles - vAim);
			Hack.Misc->AngleNormalize(vDelta);
			vAim = m_pCmd->viewangles - vDelta / gCvars.Smoothness;
		}

		if (gCvars.AutoShoot)
		{
			m_pCmd->buttons |= IN_ATTACK;
		}

		FixMove(m_pCmd, m_vOldViewAngle, m_fOldForwardMove, m_fOldSideMove);
	}
	
}

void CAimbot::DropTarget()
{
	if (gCvars.AimKey == 1)
	{
		if(!Valid(m_iBestIndex) || !GetAsyncKeyState(VK_LSHIFT))
			m_iBestIndex = -1;
	}
	else if (gCvars.AimKey == 2)
	{
		if (!Valid(m_iBestIndex) || !GetAsyncKeyState(0x06))
			m_iBestIndex = -1;
	}
	else if (gCvars.AimKey == 3)
	{
		if (!Valid(m_iBestIndex) || !GetAsyncKeyState(0x05))
			m_iBestIndex = -1;
	}
	else if (gCvars.AimKey == 4)
	{
		if (!Valid(m_iBestIndex) || !GetAsyncKeyState(VK_LBUTTON))
			m_iBestIndex = -1;
	}
	else
	{
			m_iBestIndex = -1;
	}
}

void CAimbot::GetBestTarget()
{
	m_flBestTarget = gCvars.FOV;

	if (m_pLocal == NULL)
		return;

	if (m_pLocal->GetLifeState() != LIFE_ALIVE)
		return;

	for (int i = 1; i <= Interfaces.Engine->GetMaxClients(); i++)
	{

		if (!Valid(i))
			continue;

		float flFOV = GetFOV(m_pCmd->viewangles, m_pLocal->GetEyePosition(), m_vEnd);

		if (flFOV < gCvars.FOV)
		{
			m_flBestTarget = flFOV;
			m_iBestIndex = i;
		}

	}
}

bool CAimbot::Valid(int index)
{
	if (!m_pLocal)
		return false;

	CPlayer* pEntity = Interfaces.EntList->GetClientEntity<CPlayer>(index);
	if (pEntity == NULL)
		return false;

	if (pEntity->IsDormant())
		return false;

	if (pEntity->GetLifeState() != 0)
		return false;

	if (pEntity->GetTeam() == m_pLocal->GetTeam())
		return false;

	if (pEntity->GetPlayerCond() & TFCond_Bonked)
		return false;

	if (pEntity->GetPlayerCond() & TFCond_Ubercharged)
		return false;

	player_info_t info;
	if (!Interfaces.Engine->GetPlayerInfo(index, &info))
		return false;

	if (gCvars.igncloak)
	{
		if (pEntity->GetPlayerCond() & TFCond_Cloaked)
			return false;
	}

	IClientEntity* pBaseLocalWeapon = m_pLocal->GetActiveWeapon();
	CPlayer* pLocalWeapon = (CPlayer*)pBaseLocalWeapon;
	CBaseCombatWeapon *pBaseCharacter = (CBaseCombatWeapon*)pBaseLocalWeapon;

	if (gCvars.Hitbox == 1)
	{
		m_vEnd = pEntity->GetHitboxPositionForAimbot(1);
		SimpleLatencyPrediction(pEntity, 13);
	}
	else if (gCvars.Hitbox == 2)
	{
		m_vEnd = pEntity->GetHitboxPositionForAimbot(0);
		SimpleLatencyPrediction(pEntity, 0);
	}
	else if (gCvars.Hitbox == 3)
	{
		if (gCvars.projpred && pBaseCharacter->GetSlot() == 0)
		{
			if (pLocalWeapon->GetItemDefinitionIndex() == soldierweapons::WPN_DirectHit)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1980, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == soldierweapons::WPN_LibertyLauncher)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1540, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == soldierweapons::WPN_RocketLauncher || soldierweapons::WPN_BlackBox || soldierweapons::WPN_Original || soldierweapons::WPN_CowMangler || 1104 || soldierweapons::WPN_NewRocketLauncher || soldierweapons::WPN_FestiveRocketLauncher)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1100, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == demomanweapons::WPN_GrenadeLauncher || demomanweapons::WPN_FestiveGrenadeLauncher || 1151 || demomanweapons::WPN_NewGrenadeLauncher)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1215, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == demomanweapons::WPN_LochNLoad)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1510, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == engineerweapons::WPN_RescueRanger)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 2400, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == engineerweapons::WPN_Pomson)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1200, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == medicweapons::WPN_SyringeGun || medicweapons::WPN_Overdose || medicweapons::WPN_Blutsauger)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 1000, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == medicweapons::WPN_Crossbow)
			{
				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 1, 2400, 0, PlayerGravityMod(pEntity));
			}
			if (pLocalWeapon->GetItemDefinitionIndex() == sniperweapons::WPN_Huntsman || sniperweapons::WPN_FestiveHuntsman || 1092)
			{
				float BeginCharge = pLocalWeapon->GetChargeTime();

				float Charge = BeginCharge == 0.0f ? 0.0f : Interfaces.Globals->curtime - BeginCharge;

				if (Charge > 1.0f)
					Charge = 1.0f;

				float speed = (800 * Charge + 1800);

				m_vEnd = ProjectilePrediction(pEntity, m_pLocal, 0, speed, 0, PlayerGravityMod(pEntity));
			}

		}
		else if (gCvars.projpred && (m_pLocal->GetClassNum() == TF2_Soldier && pBaseCharacter->GetSlot() == 1))
		{
			m_vEnd = pEntity->GetHitboxPositionForAimbot(1);
		}
			
	}

	return(Hack.Misc->IsVisible(m_pLocal->GetEyePosition(), m_vEnd, m_pLocal, pEntity));
}