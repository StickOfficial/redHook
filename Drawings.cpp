#include "Drawings.h"
#include "NetVars.h"
#include "Misc.h"
#include "Player.h"
#include "WeaponList.h"

CDrawings::CDrawings()
{
	hFont = Interfaces.Surface->CreateFnt();
	if(!hFont)
		return;

	Interfaces.Surface->SetFontGlyphSet(hFont, "Tahoma", 14, 400, 0, 0, FONTFLAG_OUTLINE);

	Interfaces.Engine->GetScreenSize(ScreenSize.Width, ScreenSize.Height);
}

CDrawings::~CDrawings()
{
	hFont = 0;
}

void CDrawings::DrawString(int x, int y, Color clrColor, /*bool bCenter,*/ wchar_t* szText)
{
	if(!szText)
		return;

	Interfaces.Surface->DrawSetTextPos(x, y);
	Interfaces.Surface->DrawSetTextFont(hFont);
	Interfaces.Surface->DrawSetTextColor(clrColor);
	Interfaces.Surface->DrawPrintText(szText, wcslen(szText));
}

void CDrawings::DrawString(int x, int y, Color clrColor, bool bCenter, const char* szText, ...)
{
	if(!szText)
		return;

	va_list va_alist = nullptr;
	int Wide = 0, Tall = 0;
	char szBuffer[256] = { '\0' };
	wchar_t szString[128] = { '\0' };

	va_start(va_alist, szText);
	vsprintf_s(szBuffer, szText, va_alist);
	va_end(va_alist);

	MultiByteToWideChar(CP_UTF8, 0, szBuffer, -1, szString, 128);

	if(bCenter)
	{
		Interfaces.Surface->GetTextSize(hFont, szString, Wide, Tall);
		x -= Wide / 2;
	}

	Interfaces.Surface->DrawSetTextPos(x, y);
	Interfaces.Surface->DrawSetTextFont(hFont);
	Interfaces.Surface->DrawSetTextColor(clrColor);
	Interfaces.Surface->DrawPrintText(szString, strlen(szBuffer));
}

void CDrawings::DrawString(int x, int y, Color dwColor, unsigned long font, const char *pszText, ...)
{
	if (pszText == NULL)
		return;

	va_list va_alist;
	char szBuffer[1024] = { '\0' };
	wchar_t szString[1024] = { '\0' };

	va_start(va_alist, pszText);
	vsprintf_s(szBuffer, pszText, va_alist);
	va_end(va_alist);

	wsprintfW(szString, L"%S", szBuffer);

	Interfaces.Surface->DrawSetTextPos(x, y);
	Interfaces.Surface->DrawSetTextFont(font);
	Interfaces.Surface->DrawSetTextColor(dwColor);
	Interfaces.Surface->DrawPrintText(szString, wcslen(szString));
}

void CDrawings::DrawRect(int x, int y, int w, int h, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawFilledRect(x, y, x + w, y + h);
}

void CDrawings::DrawOutlineRect(int x, int y, int w, int h, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawOutlinedRect(x, y, x + w, y + h);
}

void CDrawings::DrawLine(int x, int y, int x1, int y1, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawLine(x, y, x1, y1);
}

void DrawBox(Vector3* pointList, const Color& clrColor)
{
	Vector3 vStart, vEnd;

	for(auto i = 0; i < 3; i++)
	{
		if(Hack.Misc->WorldToScreen(pointList[i], vStart))
		{
			if(Hack.Misc->WorldToScreen(pointList[i + 1], vEnd))
				Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
		}
	}

	if(Hack.Misc->WorldToScreen(pointList[0], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[3], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}

	for(int i = 4; i < 7; i++)
	{
		if(Hack.Misc->WorldToScreen(pointList[i], vStart))
		{
			if(Hack.Misc->WorldToScreen(pointList[i + 1], vEnd))
				Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
		}
	}

	if(Hack.Misc->WorldToScreen(pointList[4], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[7], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}

	if(Hack.Misc->WorldToScreen(pointList[0], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[6], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}

	if(Hack.Misc->WorldToScreen(pointList[1], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[5], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}

	if(Hack.Misc->WorldToScreen(pointList[2], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[4], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}

	if(Hack.Misc->WorldToScreen(pointList[3], vStart))
	{
		if(Hack.Misc->WorldToScreen(pointList[7], vEnd))
			Hack.Drawings->DrawLine(vStart.x, vStart.y, vEnd.x, vEnd.y, clrColor);
	}
}

void CDrawings::FrameHitbox(CPlayer* pPlayer, int iHitbox)
{
	const model_t* pModel = pPlayer->GetModel();
	if(!pModel)
		return;

	studiohdr_t* pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(pModel);
	if(!pStudioHdr)
		return;

	mstudiohitboxset_t* pSet = pStudioHdr->pHitboxSet(pPlayer->GetHitboxSet());
	if(!pSet)
		return;

	mstudiobbox_t* pBox = pSet->pHitbox(iHitbox);
	if(!pBox)
		return;

	matrix3x4 vMatrix[128];

	if(!pPlayer->SetupBones(vMatrix, 128, 0x100, Interfaces.Globals->curtime))
		return;

	Vector3 vMin = pBox->bbmin;
	Vector3 vMax = pBox->bbmax;

	Vector3 vPointList[] =
	{
		Vector3(vMin.x, vMin.y, vMin.z),
		Vector3(vMin.x, vMax.y, vMin.z),
		Vector3(vMax.x, vMax.y, vMin.z),
		Vector3(vMax.x, vMin.y, vMin.z),
		Vector3(vMax.x, vMax.y, vMax.z),
		Vector3(vMin.x, vMax.y, vMax.z),
		Vector3(vMin.x, vMin.y, vMax.z),
		Vector3(vMax.x, vMin.y, vMax.z)
	};

	Vector3 vTransformed[8];

	for(int i = 0; i < 8; i++)
		Hack.Misc->VectorTransform(vPointList[i], vMatrix[pBox->bone], vTransformed[i]);

	DrawBox(vTransformed, Color::White());
}

void drawBoneEsp(CPlayer *pBaseEntity, Color clrColor)
{

		static const tf_hitbox leftArm[] = { tf_hitbox::hand_L, tf_hitbox::lowerArm_L, tf_hitbox::upperArm_L, tf_hitbox::spine_2 };
		static const tf_hitbox rightArm[] = { tf_hitbox::hand_R, tf_hitbox::lowerArm_R, tf_hitbox::upperArm_R, tf_hitbox::spine_2 };
		static const tf_hitbox head[] = { tf_hitbox::head, tf_hitbox::spine_2, tf_hitbox::pelvis };
		static const tf_hitbox leftLeg[] = { tf_hitbox::foot_L, tf_hitbox::knee_L, tf_hitbox::pelvis };
		static const tf_hitbox rightLeg[] = { tf_hitbox::foot_R, tf_hitbox::knee_R, tf_hitbox::pelvis };

		const model_t* pModel = pBaseEntity->GetModel();
		if (!pModel)
			return;

		studiohdr_t* pStudioHdr = Interfaces.ModelInfo->GetStudiomodel(pModel);
		if (!pStudioHdr)
			return;

		static matrix3x4 BoneToWorld[128];

		if (!pBaseEntity->SetupBones(BoneToWorld, 128, 0x100, 0))
			return;

		int HitboxSetIndex = *(int *)((DWORD)pStudioHdr + 0xB0);
		mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)pStudioHdr) + HitboxSetIndex);

		auto drawBoneChain = [](const tf_hitbox *bones, int count, mstudiohitboxset_t *pSet)
		{
			Vector3 startPos, endPos;
			Vector3 startWorld, endWorld;
			Vector3 Min, Max;

			for (int i = 0; i < count; i++)
			{
				if (i != (count - 1))
				{
					if (i == 0)
					{
						tf_hitbox h = bones[0];

						auto *pBox = pSet->pHitbox((int)h);

						Hack.Misc->VectorTransform(pBox->bbmin, BoneToWorld[pBox->bone], Min);
						Hack.Misc->VectorTransform(pBox->bbmax, BoneToWorld[pBox->bone], Max);

						startPos = (Min + Max) / 2;
					}
					else
					{
						startPos = endPos;
					}


					tf_hitbox h = bones[i + 1];

					auto *pBox = pSet->pHitbox((int)h);

					Hack.Misc->VectorTransform(pBox->bbmin, BoneToWorld[pBox->bone], Min);
					Hack.Misc->VectorTransform(pBox->bbmax, BoneToWorld[pBox->bone], Max);

					endPos = (Min + Max) / 2;

					if (!Hack.Misc->WorldToScreen(startPos, startWorld) || !Hack.Misc->WorldToScreen(endPos, endWorld))
						continue;

					Hack.Drawings->DrawLine(startWorld.x, startWorld.y, endWorld.x, endWorld.y, Color(255, 255, 255, 255));
				}
			}

		};

		drawBoneChain(leftArm, 4, pSet);
		drawBoneChain(rightArm, 4, pSet);

		drawBoneChain(head, 3, pSet);

		drawBoneChain(leftLeg, 3, pSet);
		drawBoneChain(rightLeg, 3, pSet);

}


/*
void CDrawings::ThreeDeeBox(CPlayer* pPlayer, const char* szName, Color clrColor)
{
	const matrix3x4& trans = *reinterpret_cast<matrix3x4*>(pPlayer + NetVars.m_rgflCoordinateFrame);

	Vector3 min = *reinterpret_cast<Vector3*>(pPlayer + NetVars.m_Collision + 0x8);
	Vector3 max = *reinterpret_cast<Vector3*>(pPlayer + NetVars.m_Collision + 0x14);

	Vector3 pointList[] = {
		Vector3(min.x, min.y, min.z),
		Vector3(min.x, max.y, min.z),
		Vector3(max.x, max.y, min.z),
		Vector3(max.x, min.y, min.z),
		Vector3(max.x, max.y, max.z),
		Vector3(min.x, max.y, max.z),
		Vector3(min.x, min.y, max.z),
		Vector3(max.x, min.y, max.z)
	};

	Vector3 transformed[8];

	for (int i = 0; i < 8; i++)
		Hack.Misc->VectorTransform(pointList[i], trans, transformed[i]);

	Vector3 flb, brt, blb, frt, frb, brb, blt, flt;

	if (!Hack.Misc->WorldToScreen(transformed[3], flb) ||
		!Hack.Misc->WorldToScreen(transformed[0], blb) ||
		!Hack.Misc->WorldToScreen(transformed[2], frb) ||
		!Hack.Misc->WorldToScreen(transformed[6], blt) ||
		!Hack.Misc->WorldToScreen(transformed[5], brt) ||
		!Hack.Misc->WorldToScreen(transformed[4], frt) ||
		!Hack.Misc->WorldToScreen(transformed[1], brb) ||
		!Hack.Misc->WorldToScreen(transformed[7], flt))
		return;

	Vector3 arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;


	if(pPlayer->GetPlayerCond() & TFCond_Cloaked)
	{
		DrawCornerBox(x - 1, y - 1, w + 2, h + 1, 3, 5, Color::Black());
		DrawCornerBox(x, y, w, h - 1, 3, 5, Color::White());
	}
	else
	{
		DrawCornerBox(x - 1, y - 1, w + 2, h + 1, 3, 5, Color::Black());
		DrawCornerBox(x, y, w, h - 1, 3, 5, clrColor);
	}

	//-------------------------------------------------------------------------------------------------------------
	if (pPlayer->GetPlayerCond() & TFCond_Cloaked)
	{
		DrawString(x + (w / 2), y - 25, Color::White(), true, "%s", szName);
		DrawString(x + (w / 2), y - 15, Color::White(), true, "%s", pPlayer->GetClass());
		DrawString(x + (w / 2), y + h, redGreenGradiant(pPlayer->GetHealth(), pPlayer->GetMaxHealth()), true, "%i HP", pPlayer->GetHealth());
	}
	else
	{
		DrawString(x + (w / 2), y - 25, clrColor, true, "%s", szName);
		DrawString(x + (w / 2), y - 15, clrColor, true, "%s", pPlayer->GetClass());
		DrawString(x + (w / 2), y + h, redGreenGradiant(pPlayer->GetHealth(), pPlayer->GetMaxHealth()) , true, "%i HP", pPlayer->GetHealth());
	}
	//-------------------------------------------------------------------------------------------------------------
	drawBoneEsp(pPlayer, Color::White());
}
*/

void CDrawings::DrawPlayerBox(CPlayer *pEnt, const char* szName, Color dwColor, int health, int maxHealth)
{
	if (pEnt == NULL)
		return;

	Vector3 mon, nom;

	nom = pEnt->GetAbsOrigin();

	mon = nom + Vector3(0, 0, 90.f);

	Vector3 bot, top;

	if (Hack.Misc->WorldToScreen(nom, bot) && Hack.Misc->WorldToScreen(mon, top))
	{
		float h = (bot.y - top.y);

		float w = h / 3.f;

		DrawOutlineRect(top.x - w, top.y, w * 2, h, dwColor);
		DrawOutlineRect(top.x - w - 1, top.y - 1, w * 2 + 2, h + 2, Color(0, 0, 0, 255));
		DrawOutlineRect(top.x - w + 1, top.y + 1, w * 2 - 2, h - 2, Color(0, 0, 0, 255));
		DrawString(bot.x, bot.y, dwColor, true, "%s", szName);
		bot.y += 11;
		DrawString(bot.x, bot.y, dwColor, true, "%s", pEnt->GetClass());
		bot.y += 11;
		DrawString(bot.x, bot.y, redGreenGradiant(pEnt->GetHealth(), pEnt->GetMaxHealth()), true, "%i HP", pEnt->GetHealth());
		bot.y += 11;
		IClientEntity* wep = pEnt->GetActiveWeapon();
		CBaseCombatWeapon *basechar = (CBaseCombatWeapon*)wep;
		if (pEnt->GetClassNum() == TF2_Medic && wep->GetItemDefinitionIndex() == WPN_Vaccinator && basechar->GetSlot() == 1)
		{
			DrawString(bot.x, bot.y, redGreenGradiant(pEnt->GetHealth(), pEnt->GetMaxHealth()), true, "Holding Vaccinator");
			bot.y += 11;
		}

		if (gCvars.Bones)
		{
			drawBoneEsp(pEnt, Color::White());
		}	

		Color dwHealth = Color(255, 255, 255, 255);

		int first = floor(255 - (health / maxHealth * 255 * 2.));
		int second = floor(health / maxHealth * 255 * 2.);


		if (maxHealth*.5<health)
		{
			dwHealth = Color(first, 255, 25, 255);
		}
		else 
		{
			dwHealth = Color(255, second, 25, 255);
		}

		top.x -= floor(w + 6);
		DrawRect(floor(top.x), floor(top.y), 4, h, Color(0, 0, 0, 255));
		if (maxHealth<health)
		{
			dwHealth = Color(0, 255, 0, 255);
			health = maxHealth;
		}
		int hw = floor((((h)* health) / maxHealth));
		DrawRect(floor(top.x) + 1, floor(top.y + (h - hw)) + 1, 2, hw - 2, redGreenGradiant(health, maxHealth));

	}

}

void CDrawings::DrawCircle(int x, int y, int radius, int segments, Color clrColor)
{
	Interfaces.Surface->DrawSetColor(clrColor);
	Interfaces.Surface->DrawOutlinedCircle(x, y, radius, segments);
}

void CDrawings::DrawRadarPoint(Vector3 vOriginX, Vector3 vOriginY, Vector3 qAngle, CPlayer *pBaseEntity, Color dwTeamColor)
{
	CPlayer* pLocalEntity = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());

	if (pLocalEntity == NULL)
		return;

	float flDeltaX = vOriginX.x - vOriginY.x;
	float flDeltaY = vOriginX.y - vOriginY.y;

	float flAngle = qAngle.y;

	float flYaw = (flAngle) * (PI / 180.0);
	float flMainViewAngles_CosYaw = cos(flYaw);
	float flMainViewAngles_SinYaw = sin(flYaw);

	// rotate
	float x = flDeltaY * (-flMainViewAngles_CosYaw) + flDeltaX * flMainViewAngles_SinYaw;
	float y = flDeltaX * (-flMainViewAngles_CosYaw) - flDeltaY * flMainViewAngles_SinYaw;

	float flRange = 2000;

	if (fabs(x) > flRange || fabs(y) > flRange)
	{
		// clipping
		if (y>x)
		{
			if (y>-x)
			{
				x = flRange * x / y;
				y = flRange;
			}
			else
			{
				y = -flRange * y / x;
				x = -flRange;
			}
		}
		else
		{
			if (y > -x)
			{
				y = flRange * y / x;
				x = flRange;
			}
			else
			{
				x = -flRange * x / y;
				y = -flRange;
			}
		}
	}

	int	iScreenX = gCvars.x + int(x / flRange * gCvars.w);
	int iScreenY = gCvars.y + int(y / flRange * gCvars.w);


	DrawRect(iScreenX - 3, iScreenY - 3, 7, 7, Color::Black());
	DrawRect(iScreenX - 2, iScreenY - 2, 5, 5, dwTeamColor);
}
//===================================================================================
void CDrawings::DrawRadarBack(Color team)
{
	int iCenterX = gCvars.x;
	int iCenterY = gCvars.y;
	int iSize = gCvars.w;

	DrawRect(iCenterX - iSize, iCenterY - iSize, 2 * iSize + 2, 2 * iSize + 2, Color(30, 30, 30, 128));
	iSize -= 10;

	DrawRect(iCenterX, iCenterY - iSize - 11, 1, 2 * iSize + 22, team);
	DrawRect(iCenterX - iSize - 11, iCenterY, 2 * iSize + 22, 1, team);
}

void CDrawings::SpecList()
{
	CPlayer *pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());

	RECT scrn = GetViewport();
	int ayy = 0;

	// Loop through all active entitys
	for (int i = 0; i < Interfaces.Engine->GetMaxClients(); i++)
	{
		// Get the entity
		CPlayer *pEntity = Interfaces.EntList->GetClientEntity<CPlayer>(i);
		player_info_t pinfo;

		// The entity isn't some laggy peice of shit or something
		if (pEntity &&  pEntity != pLocal)
		{
			if (Interfaces.Engine->GetPlayerInfo(i, &pinfo) && !pEntity->IsDormant())
			{

				char buf[255]; 
				sprintf_s(buf, "%s | %s | %i HP", pinfo.name, pEntity->GetClass(), pEntity->GetHealth());
				RECT TextSize = GetTextSize(hFont, buf);
				Clear(scrn.right - 260, (scrn.bottom / 2) + (16 * ayy), 260, 16, Color(0, 0, 0, 140));
				if (pEntity->GetTeam() == 2)
				{
					Text(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), Color(236, 57, 57, 255), hFont, buf);
				}
				if (pEntity->GetTeam() == 3)
				{
					Text(scrn.right - TextSize.right - 4, (scrn.bottom / 2) + (16 * ayy), Color(0, 129, 236, 255), hFont, buf);
				}
				ayy++;
			}
		}
	}

	DrawOutlineRect(scrn.right - 261, (scrn.bottom / 2) - 1, 292, (16 * ayy) + 2, Color(2, 2, 15, 200));
	DrawOutlineRect(scrn.right - 260, (scrn.bottom / 2), 290, (16 * ayy), Color(30, 30, 30, 255));
}