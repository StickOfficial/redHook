#include "SDK.h"
#include "NetVars.h"
#include "Misc.h"
#include "Player.h"
#include "Aimbot.h"
#include "SDK.h"
#include "World Objects.h"
#include "Memes.h"

Vector3 qLastTickAngles = Vector3(0.0f, 0.0f, 0.0f);
DWORD dwWriteUserCmd;
DWORD dwPureLoc = NULL;

bool __fastcall Hooked_WriteUserCmdDeltaToBuffer(void* ecx, void* edx, DWORD* buf, int from, int to, bool isnewcommand)
{
		typedef void(__cdecl *WriteUserCmdFn)(DWORD*, CUserCmd *, CUserCmd*);
		static WriteUserCmdFn CallWriteUserCmd = (WriteUserCmdFn)dwWriteUserCmd;

		CUserCmd nullcmd, *pFrom, *pTo;

		if (from == -1)
		{
			pFrom = &nullcmd;
		}
		else
		{
			pFrom = Interfaces.Input->GetUserCmd(from);

			if (!pFrom)
				pFrom = &nullcmd;
		}

		pTo = Interfaces.Input->GetUserCmd(to);

		if (!pTo)
			pTo = &nullcmd;

		CallWriteUserCmd(buf, pTo, pFrom);

		if (*(bool*)((DWORD)buf + 0x10))
			return false;

		if (gCvars.airstuck)
		{
			return false;
		}
		else
		{
			return true;
		}	
}

bool __fastcall Hooked_CreateMove(void* ecx, void* edx, float frametime, CUserCmd* pCmd)
{
		if (pCmd->command_number == 0)
			return false;

		CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
		if (!pLocal)
			return false;

		if (pLocal->GetLifeState() != LIFE_ALIVE)
			return false;

		if (gCvars.ABEnabled)
		{
			Hack.Aimbot->Main(pCmd);
		}

		if (gCvars.radar)
		{
			Hack.Drawings->Update(pCmd);
		}

		if (gCvars.Bhop)
		{
				static bool bLastJumped = false;
				static bool bShouldFake = false;

				if (!bLastJumped && bShouldFake)
				{
					bShouldFake = false;
					pCmd->buttons |= IN_JUMP;
				}

				else if (pCmd->buttons & IN_JUMP)
				{
					if (pLocal->GetFlags() & FL_ONGROUND)
					{
						bLastJumped = true;
						bShouldFake = true;
					}
					else
					{
						pCmd->buttons &= ~IN_JUMP;
						bLastJumped = false;
					}
				}
				else
				{
					bLastJumped = false;
					bShouldFake = false;
				}
		}

		Vector3 pAngle = pCmd->viewangles;

		if (gCvars.trigger)
		{
			if (gCvars.AimKey == 1)
			{
				if (GetAsyncKeyState(VK_LSHIFT) && Hack.Misc->bCanHit(pCmd))
					pCmd->buttons |= IN_ATTACK;
			}
			else if (gCvars.AimKey == 2)
			{
				if (GetAsyncKeyState(0x06) && Hack.Misc->bCanHit(pCmd))
					pCmd->buttons |= IN_ATTACK;
			}
			else if (gCvars.AimKey == 3)
			{
				if (GetAsyncKeyState(0x05) && Hack.Misc->bCanHit(pCmd))
					pCmd->buttons |= IN_ATTACK;
			}
			else if (gCvars.AimKey == 4)
			{
				if (GetAsyncKeyState(VK_LBUTTON) && Hack.Misc->bCanHit(pCmd))
					pCmd->buttons |= IN_ATTACK;
			}
		}

		if (gCvars.Bhop && gCvars.autostrafe)
		{
			static float move = 400.f;

			Vector3 vel = Hack.Misc->EstimateAbsVelocity(pLocal);

			float s_move = move * (vel.Length() / 1000.f);

			if (pCmd->buttons & IN_JUMP)
			{
				pCmd->forwardmove = move * 0.015f;
				pCmd->sidemove += (float)(((pCmd->tick_count % 2) * 2) - 1) * s_move;

				if (pCmd->mousedx)
					pCmd->sidemove = (float)Hack.Misc->Clamp(pCmd->mousedx, -1, 1) * s_move;

				static float strafe = pCmd->viewangles.y;

				float rt = pCmd->viewangles.y, rotation;

				rotation = strafe - rt;

				if (rotation < -Interfaces.Globals->interval_per_tick)
					pCmd->sidemove = -s_move;

				if (rotation > Interfaces.Globals->interval_per_tick)
					pCmd->sidemove = s_move;

				strafe = rt;
			}
		}

		static bool bThird = false;
		if (gCvars.thirdperson && !bThird)
		{
			ConVar* pThirdCamYaw = Interfaces.Cvar->FindVar("cam_idealyaw");
				Interfaces.Engine->ClientCmd_Unrestricted("thirdperson");

			if (pThirdCamYaw)
			{
				pThirdCamYaw->SetValue(0);
			}

			bThird = true;
		}
		else if (!gCvars.thirdperson && bThird)
		{
			Interfaces.Engine->ClientCmd_Unrestricted("firstperson");
			bThird = false;
		}


		if (gCvars.FakeCenter)
		{
			Vector3 angle = pCmd->viewangles;

			Vector3 oldAngle = pCmd->viewangles;
			float oldForward = pCmd->forwardmove;
			float oldSideMove = pCmd->sidemove;

			if (pCmd->buttons & IN_ATTACK || pCmd->buttons & IN_ATTACK2)
			{

			}
			else
			{

				angle.x = -3256;

			}

			Hack.Misc->AngleNormalize(angle);
			Hack.Misc->ClampAngles(angle);

			pCmd->viewangles = angle;

			Hack.Misc->CorrectMovement(angle, pCmd, oldForward, oldSideMove);

		}

		for (int i = 1; i <= Interfaces.EntList->GetHighestEntityIndex(); i++)
		{
			IClientEntity *pEnt = Interfaces.EntList->GetClientEntity<IClientEntity>(i);
			if (pEnt == NULL)
				continue;

			if (pEnt->IsDormant())
				continue;

			int EntityTeam = *MakePtr(int*, pEnt, NetVars.m_iTeamNum);
			int LocalTeam = *MakePtr(int*, pLocal, NetVars.m_iTeamNum);

			Color clrTeam = Color(255, 255, 255, 255);
			if (EntityTeam == 2)
				clrTeam = Color(236, 57, 57, 255);
			else if (EntityTeam == 3)
				clrTeam = Color(0, 129, 236, 255);

			if (pLocal->GetIndex() == pEnt->GetIndex())
				continue;

			if (EntityTeam == LocalTeam)
				continue;

			if (pEnt == pLocal)
				continue;

			IClientEntity *wep = pLocal->GetActiveWeapon();

			if (gCvars.airblast)
			{
				if (wep->GetItemDefinitionIndex() == pyroweapons::WPN_Flamethrower || pyroweapons::WPN_NewFlamethrower || pyroweapons::WPN_Degreaser || pyroweapons::WPN_Backburner || pyroweapons::WPN_FestiveFlamethrower)
				{
					if (!strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_SentryRocket") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Arrow") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_HealingBolt") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_EnergyBall") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Flare") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Jar") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_JarMilk") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Cleaver") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Throwable") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_Rocket") || !strcmp(pEnt->GetClientClass()->szName, "CTFGrenadePipebombProjectile") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_ThrowableRepel") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_ThrowableBrick") || !strcmp(pEnt->GetClientClass()->szName, "CTFProjectile_ThrowableBreadMonster"))
					{

						IClientEntity* closest = 0;
						float closest_dist = 0.0f;

						Vector3 eyepos = pLocal->GetEyePosition();
						Vector3 center;
						pEnt->GetWorldSpaceCenter(center);

						Vector3 vecSpeed = Hack.Misc->EstimateAbsVelocity(pEnt);
						CNetChan *pNet = Interfaces.Engine->GetNetChannelInfo();
						if (pNet)
						{
							float lInterp = (Interfaces.Cvar->FindVar("cl_interp_ratio")->GetFloat() / Interfaces.Cvar->FindVar("cl_updaterate")->GetFloat()) * Interfaces.Globals->interval_per_tick;
							float scale = lInterp + pNet->GetAvgLatency(FLOW_OUTGOING) + pNet->GetAvgLatency(FLOW_INCOMING);
							VectorMA(center, scale, vecSpeed, center);
						}

						float dist = flGetDistance(center, eyepos);
						if (dist <= 15 && Hack.Aimbot->GetFOV(pLocal->GetAbsAngles(), eyepos, center) <= 30)
						{
							/*
							Vector3 tr = (center - pLocal->GetEyePosition());
							Vector3 angles;
							Hack.Misc->VectorAngles(tr, angles);
							Hack.Misc->ClampAngles(angles);
							pCmd->viewangles = angles;
							*/
							pCmd->buttons |= IN_ATTACK2;
						}
					}
				}
			}		
		}

		qLastTickAngles = pCmd->viewangles;

	return false;
}


void __fastcall Hooked_CHLCreateMove(void* ecx, void* edx, int sequence_number, float input_sample_time, bool active)
{
	Hack.Client->GetMethod<CHLCreateMoveFn>(21)(ecx, sequence_number, input_sample_time, active);

	CUserCmd *pCommand = Interfaces.Input->GetUserCmd(sequence_number);
	if (pCommand)
	{

		Vector3 oldView = pCommand->viewangles;
		float oldSidemove = pCommand->sidemove;
		float oldForwardmove = pCommand->forwardmove;

		CPlayer* local = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());

		int cmEBP = 0;
		__asm mov cmEBP, ebp;
		bool *bSendPacket = (bool *)(*(char **)cmEBP - 0x1);

		static int ticks = 0;
		int ticksMax = 16;

		if (gCvars.fakelag)
		{

			if (pCommand->buttons & IN_ATTACK)
			{
				*bSendPacket = true;
				return;
			}

			if (ticks >= ticksMax)
			{
				*bSendPacket = true;
				ticks = 0;
			}
			else
			{
				if (gCvars.fakelagadaptive)
				{
					int packetsToChoke;
					if (local->GetVelocity().Length() > 0.f)
					{
						packetsToChoke = (int)((64.f / Interfaces.Globals->interval_per_tick) / local->GetVelocity().Length()) + 1;
						if (packetsToChoke >= 15)
							packetsToChoke = 14;
						if (packetsToChoke < gCvars.fakelagcons)
							packetsToChoke = gCvars.fakelagcons;
					}
					else
						packetsToChoke = 0;

					*bSendPacket = ticks < 14 - packetsToChoke;
				}
				else
					*bSendPacket = ticks < 14 - gCvars.fakelagcons;
			}

			ticks++;
		}
		else
		{
			*bSendPacket = true;
		}

		/*
		static int iSpeedCounter = 0; //Setup a global counter.
		static float step = 0;
		//If I'm pressing MOUSE4 and the counter was not 0.
		if (iSpeedCounter > 0 && local->GetPlayerCond())
		{
			iSpeedCounter--; //Decrement the counter.
			//pLocalEntity->SetSimulationTime(pLocalEntity->GetSimulationTime() + step);
			pCommand->tick_count--; //Normalize tick_count.
			__asm {
				push eax; //Preserve EAX to the stack.
				mov eax, dword ptr ss : [ebp]; //Move EBP in to EAX.
				mov eax, [eax]; //Derefrence the base pointer.
				lea eax, [eax + 0x4]; //Load the return address in to EAX.
				sub[eax], 0x5; //Make it return to -5 where it would normally.
				pop eax; //Restore EAX
			}
		}
		else
		{
			auto counter = 7;
			iSpeedCounter = counter; //We want to run this 7 times.
		}
		*/


		CVerifiedUserCmd *pSafeCommand = (CVerifiedUserCmd *)(*(DWORD *)(Interfaces.Input + 0xF8) + (sizeof(CVerifiedUserCmd) * (sequence_number % 90)));
		if (pSafeCommand)
		{
			pSafeCommand->m_cmd = *pCommand;
			pSafeCommand->m_crc = CRC32_ProcessSingleBuffer(pCommand, sizeof(pCommand));
		}	
	}
			
}


bool bMeme = false;

void __fastcall Hooked_PaintTraverse(void* ecx, void* edx, unsigned int VGUIPanel, bool forcerepaint, bool allowforce)
{
		Hack.PanelHook->GetMethod<PaintTraverseFn>(41)(ecx, VGUIPanel, forcerepaint, allowforce);

		static unsigned int panel;
		if (panel == 0)
		{

			std::string szPanelName = Interfaces.Panel->GetName(VGUIPanel);
			if (szPanelName.find("FocusOverlayPanel") != std::string::npos)
			{
				panel = VGUIPanel;
			}	
		}

		if (panel && VGUIPanel == panel)
		{

			int iNewWidth, iNewHeight;
			Interfaces.Engine->GetScreenSize(iNewWidth, iNewHeight);
			if (ScreenSize.Width != iNewWidth || ScreenSize.Height != iNewHeight)
			{
				Interfaces.Engine->GetScreenSize(ScreenSize.Width, ScreenSize.Height);
			}

			if (Interfaces.Engine->IsTakingScreenshot() || GetAsyncKeyState(VK_F12) || GetAsyncKeyState(VK_SNAPSHOT))
			{
				return;
			}

			Hack.Drawings->DrawString(5, 0, Color::Red(), false, "reDHook by reD");
			Hack.Drawings->DrawString(5, 10, Color::Red(), false, "Developer Build");
			Hack.Drawings->DrawString(5, 20, Color::Green(), false, "Build: %s ", __DATE__);

			if (Interfaces.Engine->IsInGame() && Interfaces.Engine->IsConnected())
			{

				CPlayer* pLocal = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());
				if (!pLocal)
					return;

				if (*(PDWORD)dwPureLoc)
				{
					*(PDWORD)dwPureLoc = NULL;
				}

				if (gCvars.chatsay)
				{
					Hack.Misc->ChatSpamReports();
				}

				if (gCvars.radar)
				{
					Hack.Drawings->RadarBG();
				}

				for (int i = 1; i <= Interfaces.Engine->GetMaxClients(); i++)
				{
					CPlayer* pPlayer = Interfaces.EntList->GetClientEntity<CPlayer>(i);
					CPlayer* AimBotTarget = Interfaces.EntList->GetClientEntity<CPlayer>(Hack.Aimbot->m_iBestIndex);

					if (!pPlayer->IsValid())
						continue;

					if (pPlayer == pLocal)
						continue;

					if (pLocal->GetTeam() == pPlayer->GetTeam())
						continue;

					Color clrTeam = Color(255, 255, 255, 255);

					if (pPlayer->GetTeam() == 2)
						clrTeam = Color(236, 57, 57, 255);
					else if (pPlayer->GetTeam() == 3)
						clrTeam = Color(0, 129, 236, 255);

					player_info_t info;
					if (!Interfaces.Engine->GetPlayerInfo(i, &info))
						continue;

					if (gCvars.Players)
					{
						if (pPlayer == AimBotTarget)
						{
							Hack.Drawings->DrawPlayerBox(pPlayer, info.name, Color::Green(), pPlayer->GetHealth(), pPlayer->GetMaxHealth());
						}
						else
						{
							if (pPlayer->GetPlayerCond() & TFCond_Cloaked)
							{
								Hack.Drawings->DrawPlayerBox(pPlayer, info.name, Color::White(), pPlayer->GetHealth(), pPlayer->GetMaxHealth());
							}
							else
							{
								Hack.Drawings->DrawPlayerBox(pPlayer, info.name, clrTeam, pPlayer->GetHealth(), pPlayer->GetMaxHealth());
							}
						}
					}

					if (gCvars.TraceLines)
					{
						Vector3 space_ent;
						Vector3 entcenter;

						pPlayer->GetWorldSpaceCenter(entcenter);

						if (Hack.Misc->WorldToScreen(entcenter, space_ent))
						{
							Hack.Drawings->DrawLine(ScreenSize.Width / 2, ScreenSize.Height / 2, space_ent.x, space_ent.y, clrTeam);
						}
					}

					if (gCvars.radar)
					{
						Hack.Drawings->Radar(pLocal);
					}

					if (gCvars.removedisg)
					{
						*MakePtr(int*, pPlayer, NetVars.m_nPlayerCond) &= ~TFCond_Disguised;
					}

				}

				for (int i = 1; i <= Interfaces.EntList->GetHighestEntityIndex(); i++)
				{
					IClientEntity *pEnt = Interfaces.EntList->GetClientEntity<IClientEntity>(i);
					if (pEnt == NULL)
						continue;

					if (pEnt->IsDormant())
						continue;

					int EntityTeam = *MakePtr(int*, pEnt, NetVars.m_iTeamNum);
					int LocalTeam = *MakePtr(int*, pLocal, NetVars.m_iTeamNum);

					Color clrTeam = Color(255, 255, 255, 255);
					if (EntityTeam == 2)
						clrTeam = Color(236, 57, 57, 255);
					else if (EntityTeam == 3)
						clrTeam = Color(0, 129, 236, 255);

					if (pLocal->GetIndex() == pEnt->GetIndex())
						continue;

					if (EntityTeam == LocalTeam)
						continue;

					if (pEnt == pLocal)
						continue;

					CPlayer* pEntity = (CPlayer*)pEnt;

					int id = pEnt->GetClientClass()->iClassID;

					char* name = pEnt->GetClientClass()->szName;

					Vector3 center;

					pEnt->GetWorldSpaceCenter(center);

					Vector3 a;

					if (Hack.Misc->WorldToScreen(center, a))
					{
						if (gCvars.Objects)
						{
							if (id == 26)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Intel");
							if (id == 88)
							{
								CObjectSentryGun* pSentryGun = reinterpret_cast<CObjectSentryGun*>(pEnt);

								if (pSentryGun == NULL)
									return;

								if (!pSentryGun->GetLevel())
									return;

								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Sentry: %i", pSentryGun->GetLevel());
								a.y += 11;

								if (pSentryGun->IsSapped())
								{
									Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "**SAPPED**");
									a.y += 11;
								}
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), true, "HP: %i", pSentryGun->GetHealth());
								a.y += 11;
							}

							if (id == 86)
							{
								CObjectDispenser* pDispenser = reinterpret_cast<CObjectDispenser*>(pEnt);

								if (pDispenser == NULL)
									return;

								if (!pDispenser->GetLevel())
									return;

								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Dispenser: %i", pDispenser->GetLevel());
								a.y += 11;

								if (pDispenser->IsSapped())
								{
									Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "**SAPPED**");
									a.y += 11;
								}
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), true, "HP: %i", pDispenser->GetHealth());
								a.y += 11;
							}

							if (id == 89)
							{
								CObjectTeleporter* pTeleporter = reinterpret_cast<CObjectTeleporter*>(pEnt);

								if (pTeleporter == NULL)
									return;

								if (!pTeleporter->GetLevel())
									return;

								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Teleporter: %i", pTeleporter->GetLevel());
								a.y += 11;

								if (pTeleporter->IsSapped())
								{
									Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "**SAPPED**");
									a.y += 11;
								}
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), true, "HP: %i", pTeleporter->GetHealth());
								a.y += 11;
							}

							if (id == 254)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Rocket");
							if (id == 247)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Energy Ball");
							if (id == 249)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Flare");
							if (id == 250)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Hook");
							if (id == 245)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Arrow");
							if (id == 251)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Bolt");
							if (id == 212)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Pipe/Sticky");
							if (id == 301)
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, true, "Stunt Ball");

							const char* pszModelName = pEntity->GetModelName();
							if (streql(pszModelName, "models/items/medkit_small.mdl") || streql(pszModelName, "models/items/medkit_small_bday.mdl") || streql(pszModelName, "models/props_halloween/halloween_medkit_small.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), false, "HP [Small]");
							}

							if (streql(pszModelName, "models/items/ammopack_small.mdl") || streql(pszModelName, "models/items/ammopack_small_bday.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, false, "Ammo [Small]");
							}

							if (streql(pszModelName, "models/items/medkit_medium.mdl") || streql(pszModelName, "models/items/medkit_medium_bday.mdl") || streql(pszModelName, "models/props_halloween/halloween_medkit_medium.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), false, "HP [Medium]");
							}

							if (streql(pszModelName, "models/items/ammopack_medium.mdl") || streql(pszModelName, "models/items/ammopack_medium_bday.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, false, "Ammo [Medium]");
							}

							if (streql(pszModelName, "models/items/medkit_large.mdl") || streql(pszModelName, "models/items/medkit_large_bday.mdl") || streql(pszModelName, "models/props_halloween/halloween_medkit_large.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), false, "HP [Large]");
							}

							if (streql(pszModelName, "models/items/ammopack_large.mdl") || streql(pszModelName, "models/items/ammopack_large_bday.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, false, "Ammo [Large]");
							}

							if (streql(pszModelName, "models/items/plate.mdl") || streql(pszModelName, "models/items/plate_steak.mdl") || streql(pszModelName, "models/items/plate_robo_sandwich.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, clrTeam, false, "Sandwich");
							}

							if (streql(pszModelName, "models/items/currencypack_small.mdl") || streql(pszModelName, "models/items/currencypack_medium.mdl") || streql(pszModelName, "models/items/currencypack_large.mdl"))
							{
								Hack.Drawings->DrawString(a.x, a.y, Color::Green(), false, "Money");
							}
						}
					}
				}

				if (gCvars.fov)
				{
					float pxR = tan(DEG2RAD(gCvars.FOV)) / tan(DEG2RAD(106.3f) / 2) * (ScreenSize.Width / 2);
					if (pLocal->GetPlayerCond() & TFCond_Zoomed)
					{
						Hack.Drawings->DrawCircle(ScreenSize.Width / 2, ScreenSize.Height / 2, pxR * 5, 50, Color::White());
					}
					else
					{
						Hack.Drawings->DrawCircle(ScreenSize.Width / 2, ScreenSize.Height / 2, pxR, 50, Color::White());
					}

				}


				if (GetAsyncKeyState(VK_INSERT) & 0x1)
				{
					bMenuOpen = !bMenuOpen;
				}

				if (bMenuOpen)
				{
					bMeme = true;
					HackMenu->Draw(600, 450, ScreenSize.Width, ScreenSize.Height);
					Interfaces.Panel->SetMouseInputEnabled(VGUIPanel, true);
				}
				else if (bMeme)
				{
					bMeme = false;
					Interfaces.Panel->SetMouseInputEnabled(VGUIPanel, false);
					gConfig->Save();
				}

			}
		}
}

void __fastcall Hooked_FrameStageNotify(void* _this, void* _edx, ClientFrameStage_t stage)
{

	CPlayer* local = Interfaces.EntList->GetClientEntity<CPlayer>(Interfaces.Engine->GetLocalPlayer());

	if (Interfaces.Engine->IsInGame() && stage == FRAME_RENDER_START)
	{
		if (gCvars.thirdperson)
		{
			*(Vector3*)((DWORD)local + NetVars.deadflag) = qLastTickAngles;
		}
	}

	if (Interfaces.Engine->IsInGame() && stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		for (int i = 1; i <= Interfaces.Engine->GetMaxClients(); i++)
		{
			CPlayer* pPlayer = Interfaces.EntList->GetClientEntity<CPlayer>(i);
			player_info_t info;

			if (!Interfaces.Engine->GetPlayerInfo(i, &info))
				continue;

			if (!pPlayer->IsValid())
				continue;

			if (pPlayer->GetIndex() == Interfaces.Engine->GetLocalPlayer())
				continue;

			if (local->GetTeam() == pPlayer->GetTeam())
				continue;

			Vector3 pEyeAngles = pPlayer->GetEyeAngles();

			if (pEyeAngles.x >= 90)
				pEyeAngles.x = -89.f;
			else if (pEyeAngles.x <= -90)
				pEyeAngles.x = 89.f;

			pPlayer->GetEyeAngles() = pEyeAngles;
		}
	}

	Hack.Client->GetMethod<FrameStageNotifyFn>(35)(_this, stage);
}

inline DWORD WINAPI killCvars(LPVOID param)
{
	ConCommandBase *base = Interfaces.Cvar->GetCommands();

	int count = 0;

	while (base)
	{

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_CHEAT)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_CHEAT;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_REPLICATED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_REPLICATED;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_PROTECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_PROTECTED;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_SPONLY)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_SPONLY;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_HIDDEN)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_HIDDEN;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_DEVELOPMENTONLY)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_DEVELOPMENTONLY;

		if (base->m_nFlags & (int)ConvarFlags::FCVAR_NOT_CONNECTED)
			base->m_nFlags &= ~(int)ConvarFlags::FCVAR_NOT_CONNECTED;

		if (ConVar *c = dynamic_cast<ConVar *>(base))
		{
			c->m_bHasMax = false;
			c->m_bHasMin = false;
		}

		base = base->m_pNext;
		count++;
	}

	Interfaces.Cvar->ConsolePrintf("%d cvars have been unprotected.\n", count);

	return 0;
}

void Init(HMODULE hModule)
{

	CreateInterfaceFn ClientFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("client.dll"), "CreateInterface"));
	CreateInterfaceFn EngineFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("engine.dll"), "CreateInterface"));
	CreateInterfaceFn ServerFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("server.dll"), "CreateInterface"));
	CreateInterfaceFn VGUI2Factory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vgui2.dll"), "CreateInterface"));
	CreateInterfaceFn MatSurfaceFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vguimatsurface.dll"), "CreateInterface"));
	CreateInterfaceFn MatSystemFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("materialsystem.dll"), "CreateInterface"));
	CreateInterfaceFn CVarFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "CreateInterface"));
	CreateInterfaceFn PhysFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("vphysics.dll"), "CreateInterface"));
	CreateInterfaceFn SteamFactory = reinterpret_cast<CreateInterfaceFn>(GetProcAddress(GetModuleHandleA("SteamClient.dll"), "CreateInterface"));

	Interfaces.Client = static_cast<IBaseClientDLL*>(ClientFactory("VClient017", nullptr));
	Interfaces.EntList = static_cast<IClientEntityList*>(ClientFactory("VClientEntityList003", nullptr));
	Interfaces.Engine = static_cast<IVEngineClient*>(EngineFactory("VEngineClient013", nullptr));
	Interfaces.EngineTrace = static_cast<IEngineTrace*>(EngineFactory("EngineTraceClient003", nullptr));
	Interfaces.ModelInfo = static_cast<IVModelInfoClient*>(EngineFactory("VModelInfoClient006", nullptr));
	Interfaces.Panel = static_cast<IPanel*>(VGUI2Factory("VGUI_Panel009", nullptr));
	Interfaces.Surface = static_cast<ISurface*>(MatSurfaceFactory("VGUI_Surface030", nullptr));
	Interfaces.PlayerInfoManager = static_cast<IPlayerInfoManager*>(ServerFactory("PlayerInfoManager002", nullptr));
	Interfaces.pEffects = static_cast<IVEffects*>(EngineFactory("VEngineEffects001", nullptr));
	Interfaces.Prediction = static_cast<CPrediction*>(ClientFactory("VClientPrediction001", nullptr));
	Interfaces.GameMovement = static_cast<IGameMovement*>(ClientFactory("GameMovement001", nullptr));
	Interfaces.Cvar = static_cast<ICvar*>(CVarFactory("VEngineCvar004", nullptr));
	Interfaces.Phys = static_cast<IPhysicsSurfaceProps*>(PhysFactory("VPhysicsSurfaceProps001", nullptr));
	Interfaces.steamclient = static_cast<ISteamClient017*>(SteamFactory("SteamClient017", nullptr));
	Interfaces.DebugOverlay = static_cast<IVDebugOverlay*>(EngineFactory("VDebugOverlay003", nullptr));

	HSteamPipe hNewPipe = Interfaces.steamclient->CreateSteamPipe();
	HSteamUser hNewUser = Interfaces.steamclient->ConnectToGlobalUser(hNewPipe);

	Interfaces.steamfriends = reinterpret_cast<ISteamFriends002 *>(Interfaces.steamclient->GetISteamFriends(hNewUser, hNewPipe, STEAMFRIENDS_INTERFACE_VERSION_002));
	Interfaces.steamuser = reinterpret_cast<ISteamUser017 *>(Interfaces.steamclient->GetISteamUser(hNewUser, hNewPipe, STEAMUSER_INTERFACE_VERSION_017));
	Interfaces.steamuserStats = reinterpret_cast<ISteamUserStats011 *>(Interfaces.steamclient->GetISteamUserStats(hNewUser, hNewPipe, STEAMUSERSTATS_INTERFACE_VERSION_011));

	void* pClientTable = reinterpret_cast<void*>(Hack.Misc->FindSignature("client.dll", "8B 0D ? ? ? ? 8B 02 D9 05"));
	Interfaces.ClientMode = **reinterpret_cast<IClientMode***>(reinterpret_cast<DWORD>(pClientTable) + 2);

	Interfaces.Globals = Interfaces.PlayerInfoManager->GetGlobalVars();

	dwWriteUserCmd = Hack.Misc->FindSignature("client.dll","55 8B EC 83 EC 08 53 8B 5D 0C 56 8B 75 10");
	dwPureLoc = *reinterpret_cast<PDWORD>(Hack.Misc->FindSignature("engine.dll","A1 ? ? ? ? 56 33 F6 85 C0") + 0x1);

	//-------------------------------------------------------------------------------------------------------------------//
	Hack.NetVars = std::shared_ptr<CNetVars>(new CNetVars());
	Hack.Drawings = std::shared_ptr<CDrawings>(new CDrawings());
	Hack.Misc = std::shared_ptr<CMisc>(new CMisc());
	Hack.Aimbot = std::shared_ptr<CAimbot>(new CAimbot());
	//-------------------------------------------------------------------------------------------------------------------//
	Hack.Misc->InitNetVars();
	gConfig->Load();
	Interfaces.Cvar->FindVar("cl_software_cursor")->SetValue(0);
	ConVar* cvar_test = new ConVar("ayylmao", "1", 0, "ayylmao");
	Interfaces.Cvar->RegisterConCommand(cvar_test);
	ConVar_Register(0);
	Interfaces.Cvar->ConsolePrintf("+-----+ +----+ +----+   +    + +-----+ +-----+ +   ++\n");
	Interfaces.Cvar->ConsolePrintf("|     | |      |     +  |    | |     | |     | |   +\n");
	Interfaces.Cvar->ConsolePrintf("|     | |      |     |  |    | |     | |     | | ++\n");
	Interfaces.Cvar->ConsolePrintf("+-----+ +----+ |     |  +----+ |     | |     | ++\n");
	Interfaces.Cvar->ConsolePrintf("++      |      |     |  |    | |     | |     | | ++\n");
	Interfaces.Cvar->ConsolePrintf("| ++    |      |     +  |    | |     | |     | |   +\n");
	Interfaces.Cvar->ConsolePrintf("+   ++  +----+ +----+   +    + +-----+ +-----+ +   ++\n");
	CSteamID localID = Interfaces.steamuser->GetSteamID();
	Interfaces.Cvar->ConsolePrintf("Name:        %s\n", Interfaces.steamfriends->GetPersonaName());
	Interfaces.Cvar->ConsolePrintf("SteamID:     %s\n", localID.Render());
	CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)killCvars, nullptr, 0, nullptr);
	//-------------------------------------------------------------------------------------------------------------------//
	Hack.ClientModeHook = std::shared_ptr<CHook>(new CHook(Interfaces.ClientMode));
	Hack.PanelHook = std::shared_ptr<CHook>(new CHook(Interfaces.Panel));
	Hack.Client = std::shared_ptr<CHook>(new CHook(Interfaces.Client));

	Hack.Client->HookMethod(&Hooked_FrameStageNotify, 35);
	Hack.Client->HookMethod(&Hooked_CHLCreateMove, 21);
	DWORD dwInputPointer = (gBaseAPI.dwFindPattern((DWORD)Hack.Client->GetMethod<DWORD>(21), ((DWORD)Hack.Client->GetMethod<DWORD>(21)) + 0x100, "8B 0D")) + (0x2);
	Interfaces.Input = **(CInput***)dwInputPointer;
	Hack.Client->HookMethod(&Hooked_WriteUserCmdDeltaToBuffer, 23);
	Hack.ClientModeHook->HookMethod(&Hooked_CreateMove, 21);
	Hack.PanelHook->HookMethod(&Hooked_PaintTraverse, 41);

	//-------------------------------------------------------------------------------------------------------------------//

	while(!GetAsyncKeyState(VK_F11))
		std::this_thread::sleep_for(std::chrono::milliseconds(101));

	SAFE_RESET(Hack.NetVars);
	SAFE_RESET(Hack.Drawings);
	SAFE_RESET(Hack.Misc);

	SAFE_RESET(Hack.ClientModeHook);
	SAFE_RESET(Hack.PanelHook);

	std::this_thread::sleep_for(std::chrono::seconds(1));
	FreeLibraryAndExitThread(hModule, NULL);
}

bool __stdcall DllMain(HINSTANCE hDLLInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		gBaseAPI.BaseUponModule((HMODULE)hDLLInstance);
		DisableThreadLibraryCalls(hDLLInstance);
		CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Init), hDLLInstance, 0, nullptr);
	}

	return true;
}
