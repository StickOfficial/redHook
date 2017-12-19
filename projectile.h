#pragma once

#include "SDK.h"

namespace projectileHelper
{
	inline float GetProjectileSpeed(IClientEntity *wep);

	inline float GetProjectileGravity(IClientEntity *wep);

	inline void PhysicsClipVelocity(const Vector3 &in, const Vector3 &normal, Vector3 &out, float overbounce);

	inline bool PhysicsApplyFriction(const Vector3 &in, Vector3 &out, float flSurfaceFriction, float flTickRate);

	inline void DrawDebugArrow(const Vector3 &vecFrom, const Vector3 &vecTo, const Color color);

	inline float GetEntityGravity(IClientEntity *ent);

	inline unsigned int PhysicsSolidMaskForEntity(IClientEntity *ent);

	Vector3 PredictCorrection(IClientEntity *weapon, IClientEntity *target, Vector3 vecFrom, int qual);

	inline void PredictPath(IClientEntity *target);
}
