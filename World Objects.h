//===================================================================================
#include "SDK.h"
//===================================================================================
class CObject : public IClientEntity
{
public:
	inline int GetLevel()
	{
		DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iUpgradeLevel");
	}

	inline bool IsSapped()
	{
		DYNVAR_RETURN(bool, this, "DT_BaseObject", "m_bHasSapper");
	}

	inline bool IsBuilding()
	{
		DYNVAR_RETURN(bool, this, "DT_BaseObject", "m_bBuilding");
	}
    
	inline float GetPercentageConstructed()
	{
		if (IsBuilding())
		{
			DYNVAR_RETURN(float, this, "DT_BaseObject", "m_flPercentageConstructed");
		}			
		else
		{
			return 0;
		}		
	}
	
	inline int GetHealth()
	{
		DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iHealth");
	}

	inline int GetUpgradedMetal()
	{
		DYNVAR_RETURN(int, this, "DT_BaseObject", "m_iUpgradeMetal");
	}

	inline int GetState()
	{
		DYNVAR_RETURN(int, this, "DT_ObjectSentrygun", "m_iState");
	}

	inline int GetTeamNum()
	{
		DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
	}
};
//===================================================================================
class CObjectDispenser : public CObject
{
public:
	inline int GetMetalReserve()
	{
		DYNVAR_RETURN(int, this, "DT_ObjectDispenser", "m_iAmmoMetal");
	}
};
//===================================================================================
class CObjectSentryGun : public CObject
{
public:
	inline int GetRocket()
	{
		if (GetLevel() == 3)
		{
			DYNVAR_RETURN(int, this, "DT_ObjectSentrygun", "m_iAmmoRockets");
		}
		return NULL;
	}

	inline int GetAmmo()
	{
		DYNVAR_RETURN(int, this, "DT_ObjectSentrygun", "m_iAmmoShells");
	}

	inline int IsControlled()
	{
		DYNVAR_RETURN(bool, this, "DT_ObjectSentrygun", "m_bPlayerControlled");
	}


	inline char* GetStateString()
	{
		switch( GetState() )
		{
		case 1:
			{
				return "Idle";
			}
		case 2:
			{
				return "Attacking";
			}
		case 3:
			{
				return "Upgrading";
			}
		}
		return "Unknown";
	}
};
//===================================================================================
class CObjectTeleporter : public CObject
{
public:
	inline char* GetStateString()
	{
		switch( GetState() )
		{
		case 1:
			{
				return "Idle";
			}
		case 2:
			{
				return "Active";
			}
		case 4:
			{
				return "Teleporting";
			}
		case 6:
			{
				return "Charging";
			}
		}

		return "Unknown";
	}
};
//===================================================================================
CObject *ToBaseObject( IClientEntity *pBaseEntity )
{
	return reinterpret_cast< CObject *>( pBaseEntity );
}
//===================================================================================