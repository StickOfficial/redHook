#include "projectile.h"
#include "ClassId.h"
#include "Misc.h"
#include "Drawings.h"

float projectileHelper::GetProjectileSpeed(IClientEntity *wep)
{
	int id = wep->GetClientClass()->iClassID;

	float BeginCharge;
	float Charge;

	//return flSpeed;
	switch(id)
	{
	case 284:
		return 1980.0f;

	case 282:
	case 285:
		return 1100.0f;

	case 205:
		return 1450.0f;

	case 188:
		return 1940.0f;

	case 303:
		return 990.0f;

	case 194:
	{
		BeginCharge = wep->GetChargeTime();

		Charge = BeginCharge == 0.0f ? 0.0f : Interfaces.Globals->curtime - BeginCharge;

		if(Charge > 1.0f)
			Charge = 1.0f;

		return( 800 * Charge + 1800 );
	}

	default:
		return -1.0f;
	}

	return -1.0f;
}

float projectileHelper::GetProjectileGravity(IClientEntity *wep)
{

	int id = wep->GetClientClass()->iClassID;

	float BeginCharge;
	float Charge;

	switch(id)
	{
	case 197:
		BeginCharge = wep->GetChargeTime();

		Charge = BeginCharge == 0.0f ? 0.0f : Interfaces.Globals->curtime - BeginCharge;

		if(Charge > 1.0f)
			Charge = 1.0f;

		return ( ( ( 1.3 - Charge ) / 3 ) * 1000 );
		break;

	default:
		return 1.0f;
	}
}

void projectileHelper::PhysicsClipVelocity(const Vector3 &in, const Vector3 &normal, Vector3 &out, float overbounce)
{
	float backoff =  in.Dot(normal) * overbounce;

	for(int i = 0; i < 3; ++i)
	{
		float change = normal[i] * backoff;
		out[i] = in[i] - change;

		if(out[i] > -0.1f && out[i] < 0.1f)
			out[i] = 0.0f;
	}

	float adjust = out.Dot(normal);

	if(adjust < 0.0f)
		out -= ( normal * adjust );
}

bool projectileHelper::PhysicsApplyFriction(const Vector3 &in, Vector3 &out, float flSurfaceFriction, float flTickRate)
{
	static ConVar *sv_friction = Interfaces.Cvar->FindVar("sv_friction");
	static ConVar *sv_stopspeed = Interfaces.Cvar->FindVar("sv_stopspeed");

	float speed = in.Length() / flTickRate;

	if(speed < 0.1f)
		return false;

	float drop = 0.0f;

	if(flSurfaceFriction != -1.0f)
	{
		float friction = sv_friction->GetFloat() * flSurfaceFriction;
		float control = ( speed < sv_stopspeed->GetFloat() ) ? sv_stopspeed->GetFloat() : speed;
		drop += control * friction * flTickRate;
	}

	float newspeed = speed - drop;

	if(newspeed < 0.0f)
		newspeed = 0.0f;

	if(newspeed != speed)
	{
		newspeed /= speed;
		out = in * newspeed; // Vector3Scale(in, newspeed, out);
	}

	out -= in * ( 1.0f - newspeed );
	out *= flTickRate;
	return true;
}

void projectileHelper::DrawDebugArrow(const Vector3 &vecFrom, const Vector3 &vecTo, const Color color)
{
	Vector3 angRotation;
	Hack.Misc->VectorAngles(vecTo - vecFrom, angRotation);
	Vector3 vecForward, vecRight, vecUp;
	Hack.Misc->AngleVectors(angRotation, &vecForward, &vecRight, &vecUp);
	Interfaces.DebugOverlay->AddLineOverlay(vecFrom, vecTo, 0, 255, 0, true, 1.0f);
	Interfaces.DebugOverlay->AddLineOverlay(vecFrom, vecFrom - vecRight * 10.0f, 0, 255, 0, true, 1.0f);
}

float projectileHelper::GetEntityGravity(IClientEntity *ent)
{
	//static ConVar *sv_gravity = Interfaces.Cvar->FindVar("sv_gravity");

	int type = ent->GetMoveType();

	if(type == MOVETYPE_NOCLIP || type == MOVETYPE_STEP || type == MOVETYPE_FLY)
		return 0.0f;

	return 1.0f;
}

unsigned projectileHelper::PhysicsSolidMaskForEntity(IClientEntity *ent)
{
	typedef unsigned int(__thiscall *OriginalFn)( PVOID );
	return CallVirtualFunction<OriginalFn>(ent, 128)( ent );
}

Vector3 projectileHelper::PredictCorrection(IClientEntity *weapon, IClientEntity *target, Vector3 vecFrom, int qual)
{
	if (weapon == nullptr || target == nullptr)
	{
		return vecFrom;
	}


	float flSpeed = projectileHelper::GetProjectileSpeed(weapon);;

	if (flSpeed <= 0.0f)
		return vecFrom;

	static ConVar *sv_gravity = Interfaces.Cvar->FindVar("sv_gravity");
	float flLag = 0;
	bool bIsOnGround = target->GetFlags() & FL_ONGROUND;
	unsigned int mask = projectileHelper::PhysicsSolidMaskForEntity(target);
	Vector3 vecWorldGravity = Vector3(0, 0, -sv_gravity->GetFloat() * Interfaces.Globals->interval_per_tick * Interfaces.Globals->interval_per_tick);

	Vector3 vecProjGravity = Vector3(0, 0, sv_gravity->GetFloat() * projectileHelper::GetProjectileGravity(weapon) * Interfaces.Globals->interval_per_tick * Interfaces.Globals->interval_per_tick);

	// get the current velocity
	Vector3 vecVelocity = Hack.Misc->EstimateAbsVelocity(target);

	Vector3 vecProjVelocity = vecProjGravity;

	Vector3 vecStepPos = target->GetAbsOrigin();

	// we can get the collideable through the netvars
	Vector3 vecMins = target->GetCollideableMins();
	Vector3 vecMaxs = target->GetCollideableMaxs();

	// get velocity for a single tick

	Vector3 vecPredictedPos = vecStepPos;

	trace_t tr;
	Ray_t ray;

	CTraceFilter filter;

	// get the current arival time
	Vector3 vecPredictedProjVel = vecProjVelocity; // TODO: rename - this is used for gravity
	float flArrivalTime = (vecFrom - vecPredictedPos).Length() / (flSpeed)+flLag + Interfaces.Globals->interval_per_tick;
	Vector3 vecPredictedVel = vecVelocity;

	for (float flTravelTime = 0.0f; flTravelTime < flArrivalTime; flTravelTime += (Interfaces.Globals->interval_per_tick * qual))
	{

		// trace the velocity of the target
		ray.Init(vecPredictedPos, vecPredictedPos + vecPredictedVel, vecMins, vecMaxs);
		Interfaces.EngineTrace->TraceRay(ray, mask, &filter, &tr);

		if (tr.startsolid)
			break;

		if (tr.fraction != 1.0f)
			projectileHelper::PhysicsClipVelocity(vecPredictedVel, tr.plane.normal, vecPredictedVel, 1.0f);

		vecPredictedPos = tr.endpos;

		ray.Init(vecPredictedPos, vecPredictedPos + vecWorldGravity, vecMins, vecMaxs);
		Interfaces.EngineTrace->TraceRay(ray, mask, &filter, &tr);

		vecPredictedVel += vecPredictedProjVel;

		if (tr.fraction == 1.0f)
		{
			bIsOnGround = false;
			vecPredictedVel += vecWorldGravity;
		}
		else if (!bIsOnGround)
		{
			float surfaceFriction = 1.0f;
			Interfaces.Phys->GetPhysicsProperties(tr.surface.surfaceProps, NULL, NULL, &surfaceFriction, NULL);

			if (!projectileHelper::PhysicsApplyFriction(vecPredictedVel, vecPredictedVel, surfaceFriction, Interfaces.Globals->interval_per_tick))
				break;
		}

		flArrivalTime = (vecFrom - vecPredictedPos).Length() / (flSpeed)+flLag + Interfaces.Globals->interval_per_tick;

		if (vecPredictedVel.Length() > flSpeed)
		{
			break;
		}
	}

	projectileHelper::DrawDebugArrow(vecStepPos, vecPredictedPos, Color(0, 255, 0, 255));
	return vecStepPos - vecPredictedPos;
}

void projectileHelper::PredictPath(IClientEntity *target)
{

	static ConVar *sv_gravity = Interfaces.Cvar->FindVar("sv_gravity");

	Vector3 vecVelocity = Hack.Misc->EstimateAbsVelocity(target);

	bool bIsOnGround = target->GetFlags() & FL_ONGROUND;
	unsigned int mask = PhysicsSolidMaskForEntity(target);
	Vector3 vecWorldGravity = Vector3(0, 0, -sv_gravity->GetFloat() * GetEntityGravity(target) * Interfaces.Globals->interval_per_tick * Interfaces.Globals->interval_per_tick);
	Vector3 vecStepPos = target->GetAbsOrigin();

	// we can get the collideable through the netvars
	Vector3 vecMins = target->GetCollideableMins();
	Vector3 vecMaxs = target->GetCollideableMaxs();

	vecVelocity *= Interfaces.Globals->interval_per_tick;

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	for(int i = 0; i < 100; ++i)
	{
		ray.Init(vecStepPos, vecStepPos + vecVelocity, vecMins, vecMaxs);
		Interfaces.EngineTrace->TraceRay(ray, mask, &filter, &tr);

		if(tr.startsolid)
			break;

		if(tr.fraction != 1.0f)
			PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecVelocity, 1.0f);

		DrawDebugArrow(vecStepPos, tr.endpos, Hack.Drawings->redGreenGradiant(i, 100));
		vecStepPos = tr.endpos;

		ray.Init(vecStepPos, vecStepPos + vecWorldGravity, vecMins, vecMaxs);
		Interfaces.EngineTrace->TraceRay(ray, mask, &filter, &tr);

		if(tr.fraction == 1.0f)
		{
			bIsOnGround = false;
			vecVelocity += vecWorldGravity;
		}
		else if(!bIsOnGround)
		{
			float surfaceFriction = 1.0f;
			Interfaces.Phys->GetPhysicsProperties(tr.surface.surfaceProps, NULL, NULL, &surfaceFriction, NULL);

			if(!PhysicsApplyFriction(vecVelocity, vecVelocity, surfaceFriction, Interfaces.Globals->interval_per_tick))
				break;
		}
	}
}
