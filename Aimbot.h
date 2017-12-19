#include "SDK.h"

class CPlayer;
class CAimbot
{
public:
	CAimbot();
	~CAimbot();

	void			Main(CUserCmd* pCmd);
	float			GetFOV(const Vector3& viewangles, const Vector3& vStart, const Vector3& vEnd);
	int				m_iBestIndex;
	void			AimAtHitbox(IClientEntity* ent, int hitbox, CUserCmd* cmd);

private:
	void			DropTarget(void);
	void			GetBestTarget(void);
	bool			Valid(int index);
	void			AimAt(Vector3 origin, Vector3 target, CUserCmd* cmd);

private:
	CUserCmd*		m_pCmd;
	CPlayer*		m_pLocal;
	Vector3			m_vEnd;
	float			m_flBestTarget;
	int				m_iTimeCounter;
	int				m_iWeaponId;
	float			time, speed, distance;
	Vector3         predPos, currPos;
};