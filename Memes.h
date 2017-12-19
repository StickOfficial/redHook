#include "Drawings.h"
#include "SDK.h"
#include "WeaponList.h"

class GConfig
{
public:
	void Save();
	void Load();
private:
	void Read(char*, char*, char*&);
	void ReadInt(char*, char*, int&);
	void ReadBool(char*, char*, bool&);
	void ReadFloat(char*, char*, float&);
	void Write(char*, char*, char*);
	void WriteInt(char*, char*, int);
	void WriteFloat(char*, char*, float);
	void WriteMode(int);
	char szFile[MAX_PATH];

}; GConfig* gConfig = new GConfig;

void GConfig::Save()
{
	TCHAR szPath[MAX_PATH];
	char szFolder[MAX_PATH];

	sprintf(szFolder, "C:\\reDHook\\", szPath);
	sprintf(szFile, "C:\\reDHook\\reDHook.ini", szPath);

	CreateDirectoryA(szFolder, NULL);

	WriteInt("Aimbot", "Enabled", gCvars.ABEnabled);
	WriteInt("Aimbot", "FOV", gCvars.FOV);
	WriteInt("Aimbot", "AimKey", gCvars.AimKey);
	WriteInt("Aimbot", "Pos", gCvars.Hitbox);
	WriteInt("Aimbot", "Smoothness", gCvars.Smoothness);
	WriteInt("Aimbot", "Silent", gCvars.Silent);
	WriteInt("Aimbot", "AutoShoot", gCvars.AutoShoot);
	WriteInt("Aimbot", "Ignore Cloak", gCvars.igncloak);
	WriteInt("Aimbot", "Remove Disguise", gCvars.removedisg);
	WriteInt("Aimbot", "Projectile Prediction", gCvars.projpred);
	WriteInt("Aimbot", "Fake Center", gCvars.FakeCenter);
	WriteInt("Aimbot", "Trigger Bot", gCvars.trigger);

	WriteInt("Visuals", "Draw Players", gCvars.Players);
	WriteInt("Visuals", "Draw Bones", gCvars.Bones);
	WriteInt("Visuals", "Draw Objects", gCvars.Objects);
	WriteInt("Visuals", "Draw FOV", gCvars.fov);
	WriteInt("Visuals", "Lines To Players", gCvars.TraceLines);

	WriteInt("Radar", "Draw Radar", gCvars.radar);
	WriteInt("Radar", "Radar X", gCvars.x);
	WriteInt("Radar", "Radar Y", gCvars.y);
	WriteInt("Radar", "Radar W", gCvars.w);

	WriteInt("Misc", "Bunny Hop", gCvars.Bhop);
	WriteInt("Misc", "Auto Strafe", gCvars.autostrafe);
	WriteInt("Misc", "ThirdPerson", gCvars.thirdperson);
	WriteInt("Misc", "Report Spam", gCvars.chatsay);
	WriteInt("Misc", "Fake Lag", gCvars.fakelag);
	WriteInt("Misc", "Fake Lag Adaptive", gCvars.fakelagadaptive);
	WriteInt("Misc", "Lag Var", gCvars.fakelagcons);
	WriteInt("Misc", "Airstuck", gCvars.airstuck);
	WriteInt("Misc", "Auto Airblast", gCvars.airblast);
}

void GConfig::Load()
{
	TCHAR szPath[MAX_PATH];
	char szFolder[MAX_PATH];

	sprintf(szFolder, "C:\\reDHook\\", szPath);
	sprintf(szFile, "C:\\reDHook\\reDHook.ini", szPath);

	CreateDirectoryA(szFolder, NULL);

	ReadBool("Aimbot", "Enabled", gCvars.ABEnabled);
	ReadInt("Aimbot", "FOV", gCvars.FOV);
	ReadInt("Aimbot", "AimKey", gCvars.AimKey);
	ReadInt("Aimbot", "Pos", gCvars.Hitbox);
	ReadInt("Aimbot", "Smoothness", gCvars.Smoothness);
	ReadBool("Aimbot", "Silent", gCvars.Silent);
	ReadBool("Aimbot", "AutoShoot", gCvars.AutoShoot);
	ReadBool("Aimbot", "Ignore Cloak", gCvars.igncloak);
	ReadBool("Aimbot", "Remove Disguise", gCvars.removedisg);
	ReadBool("Aimbot", "Projectile Prediction", gCvars.projpred);
	ReadBool("Aimbot", "[AA]Fake Center", gCvars.FakeCenter);
	ReadBool("Aimbot", "Trigger Bot", gCvars.trigger);

	ReadBool("Visuals", "Draw Players", gCvars.Players);
	ReadBool("Visuals", "Draw Bones", gCvars.Bones);
	ReadBool("Visuals", "Draw Objects", gCvars.Objects);
	ReadBool("Visuals", "Draw FOV", gCvars.fov);
	ReadBool("Visuals", "Lines To Players", gCvars.TraceLines);

	ReadBool("Radar", "Draw Radar", gCvars.radar);
	ReadInt("Radar", "Radar X", gCvars.x);
	ReadInt("Radar", "Radar Y", gCvars.y);
	ReadInt("Radar", "Radar W", gCvars.w);

	ReadBool("Misc", "Bunny Hop", gCvars.Bhop);
	ReadBool("Misc", "Auto Strafe", gCvars.autostrafe);
	ReadBool("Misc", "ThirdPerson", gCvars.thirdperson);
	ReadBool("Misc", "Report Spam", gCvars.chatsay);
	ReadBool("Misc", "Fake Lag", gCvars.fakelag);
	ReadBool("Misc", "Fake Lag Adaptive", gCvars.fakelagadaptive);
	ReadInt("Misc", "Lag Var", gCvars.fakelagcons);
	ReadBool("Misc", "Airstuck", gCvars.airstuck);
	ReadBool("Misc", "Auto Airblast", gCvars.airblast);
}

void GConfig::Read(char* szSection, char* szKey, char* &szData)
{
	char szTemp[128];
	if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile))
	{
		char szString[128];
		sprintf(szString, szTemp);

		szData = szString;
	}
}

void GConfig::ReadInt(char* szSection, char* szKey, int &iData)
{
	char szTemp[128];
	if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile))
	{
		char szString[128];
		sprintf(szString, szTemp);

		iData = std::stoi(szString);
	}
}

void GConfig::ReadBool(char* szSection, char* szKey, bool &bData)
{
	char szTemp[128];
	if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile))
	{
		char szString[128];
		sprintf(szString, szTemp);

		bData = !!std::stoi(szString);
	}
}

void GConfig::ReadFloat(char* szSection, char* szKey, float &fData)
{
	char szTemp[128];
	if (GetPrivateProfileStringA(szSection, szKey, "", szTemp, 128, szFile))
	{
		char szString[128];
		sprintf(szString, szTemp);

		fData = std::stof(szString);
	}
}

void GConfig::Write(char* szSection, char* szKey, char* szValue)
{
	WritePrivateProfileStringA(szSection, szKey, szValue, szFile);
}

void GConfig::WriteInt(char* szSection, char* szKey, int szValue)
{
	WritePrivateProfileStringA(szSection, szKey, std::to_string(szValue).c_str(), szFile);
}

void GConfig::WriteFloat(char* szSection, char* szKey, float szValue)
{
	WritePrivateProfileStringA(szSection, szKey, std::to_string(szValue).c_str(), szFile);
}

class CMenu
{
public:
	void Draw(int, int, int, int);

private:
	int iMouseX, iMouseY, iWidth, iHeight, iScreenWidth, iScreenHeight, iTab = 1;
	bool bClicked, bMouseD = false;
	char* title = "";
	int tick = 0;
	bool waiting = false;
	int CPWidth = 168, CPHeight = 179;
	void GetMousePos();
	void GetClicked();
	bool Holding(int, int, int, int);
	bool Hovering(int, int, int, int);
	int MouseXPos(int, int, int, int);
	bool Clicked(int, int, int, int);
	void DrawRectM(int, int, int, int, Color);
	void DrawOutlinedRectM(int, int, int, int, Color);
	void DrawStringM(HFont, int, int, Color, const char*, ...);
	void DrawStringM2(HFont, int, int, Color, const char*);
	void DrawSlider(const char*, int, int, int, int, int, int &);
	void DrawButton(const char*, int, int, int, bool &);
	void DrawDropdown(const char*, int, int, int, int, const char**, int, int &);
	void DrawTab(const char*, int);
	void DrawKey(int, int, int, int);
	void DrawBackground();
	void DrawClose();
	void DrawButtons();

}; CMenu *HackMenu = new CMenu;

bool bMenuOpen = false;
const char* key[4] = { "Left Shift", "Mouse4", "Mouse5", "Mouse1"};
const char* key1[3] = { "Left Shift", "Mouse4", "Mouse5" };
const char* hitbox[3] = { "Body", "Head", "Proj Pred"};

void CMenu::Draw(int iMenuWidth, int iMenuHeight, int iSWidth, int iSHeight)
{
	iWidth = iMenuWidth;
	iHeight = iMenuHeight;
	iScreenWidth = iSWidth;
	iScreenHeight = iSHeight;

	GetMousePos();
	GetClicked();
	DrawBackground();
	DrawButtons();
}

void CMenu::GetMousePos()
{
	Interfaces.Surface->GetCursorPosition(iMouseX, iMouseY);
}

void CMenu::GetClicked()
{
	if (GetAsyncKeyState(VK_LBUTTON))
	{
		bClicked = false;
		bMouseD = true;
	}
	else
		if (bMouseD)
		{
			bClicked = true;
			bMouseD = false;
		}
		else
			bClicked = false;
}

bool CMenu::Holding(int iXStart, int iYStart, int iXEnd, int iYEnd)
{
	if (GetAsyncKeyState(VK_LBUTTON))
		if (Hovering(iXStart, iYStart, iXEnd, iYEnd))
			return true;

	return false;
}


bool CMenu::Hovering(int iXStart, int iYStart, int iXEnd, int iYEnd)
{
	int iRealXStart = ((iScreenWidth / 2) - (iWidth / 2)) + iXStart;
	int iRealYStart = ((iScreenHeight / 2) - (iHeight / 2)) + iYStart;
	int iRealXEnd = iRealXStart + iXEnd;
	int iRealYEnd = iRealYStart + iYEnd;

	if (iMouseX >= iRealXStart && iRealXEnd >= iMouseX)
		if (iMouseY >= iRealYStart && iRealYEnd >= iMouseY)
			return true;

	return false;
}

int CMenu::MouseXPos(int iXStart, int iYStart, int iXEnd, int iYEnd)
{
	int iRealXStart = ((iScreenWidth / 2) - (iWidth / 2)) + iXStart;
	return iMouseX - iRealXStart;
}

bool CMenu::Clicked(int iXStart, int iYStart, int iXEnd, int iYEnd)
{
	if (bClicked)
		if (Hovering(iXStart, iYStart, iXEnd, iYEnd))
			return true;

	return false;
}

void CMenu::DrawRectM(int iXStart, int iYStart, int iRWidth, int iRHeight, Color colDrawColor)
{
	int iRealXStart = ((iScreenWidth / 2) - (iWidth / 2)) + iXStart;
	int iRealYStart = ((iScreenHeight / 2) - (iHeight / 2)) + iYStart;

	Hack.Drawings->DrawRect(iRealXStart, iRealYStart, iRWidth, iRHeight, colDrawColor);
}

void CMenu::DrawOutlinedRectM(int iXStart, int iYStart, int iRWidth, int iRHeight, Color colDrawColor)
{
	int iRealXStart = ((iScreenWidth / 2) - (iWidth / 2)) + iXStart;
	int iRealYStart = ((iScreenHeight / 2) - (iHeight / 2)) + iYStart;

	Hack.Drawings->DrawOutlineRect(iRealXStart, iRealYStart, iRWidth, iRHeight, colDrawColor);
}

void CMenu::DrawStringM(HFont hfFont, int iX, int iY, Color colDrawColor, const char* msg, ...)
{
	int iRealX = ((iScreenWidth / 2) - (iWidth / 2)) + iX;
	int iRealY = ((iScreenHeight / 2) - (iHeight / 2)) + iY;

	Hack.Drawings->DrawStringA(hfFont, iRealX, iRealY, colDrawColor, msg);
}

void CMenu::DrawStringM2(HFont hfFont, int iX, int iY, Color colDrawColor, const char* msg)
{
	int iRealX = ((iScreenWidth / 2) - (iWidth / 2)) + iX;
	int iRealY = ((iScreenHeight / 2) - (iHeight / 2)) + iY;

	Hack.Drawings->DrawStringB(hfFont, iRealX, iRealY, colDrawColor, msg);
}

void CMenu::DrawButton(const char* strText, int iTabIndex, int iX, int iY, bool &bVar)
{
	if (iTab == iTabIndex)
	{
		if (!bVar)
			DrawOutlinedRectM(iX, iY, 20, 20, Color(220, 24, 24, 255));
		else
			DrawRectM(iX, iY, 20, 20, Color(220, 24, 24, 255));

		DrawStringM2(Hack.Drawings->hFont, iX + 28, iY + 10, Color(255, 255, 255, 255), strText);

		if (Clicked(iX, iY, 20, 20))
		{
			bVar = !bVar;
		}
	}
}

int WhichKeyIsPressed()
{
	for (int i = 1; i < 255; ++i)
	{
		if (GetAsyncKeyState(i) & 0x8000)
			return i;
	}
}

void CMenu::DrawKey(int tabindex, int iX, int iY, int kek)
{
	if (iTab == tabindex)
	{
		auto bHover = Hovering(iX, iY, 60, 20);
		static auto bState = false;

		DrawOutlinedRectM(iX, iY, 60, 20, Color(220, 24, 24, 255));
		DrawRectM(iX + 1, iY + 1, 60 - 2, 20 - 2, Color(27, 27, 27));

		if (bHover && Clicked(iX, iY, 60, 20))
		{
			bState = !bState;
		}

		if (bState && GetAsyncKeyState(WhichKeyIsPressed()))
		{
			bState = false;
			kek = WhichKeyIsPressed();
		}

		char pszBuffer[0x20];
		sprintf_s(pszBuffer, "%.f", gCvars.key);

		DrawStringM(Hack.Drawings->hFont, iX + 40 / 2 + 5, iY + 20 / 2, Color(255, 255, 255), "%.f", std::string(pszBuffer));
		DrawStringM(Hack.Drawings->hFont, iX + 40 / 2, iY + 20 / 2, Color(255, 255, 255), title);
	}
}

void CMenu::DrawSlider(const char* strText, int iTabIndex, int iX, int iY, int iWidth, int iMax, int &iVar)
{
	if (iTab == iTabIndex)
	{
		DrawOutlinedRectM(iX, iY, iWidth, 20, Color(220, 24, 24, 255));
		DrawRectM(iX, iY, iVar * ((float)iWidth / (float)(iMax)), 20, Color(220, 24, 24, 255));

		std::string s = std::to_string(iVar);
		char const *pchar = s.c_str();

		DrawStringM2(Hack.Drawings->hFont, iX + 12, iY + 9, Color(255, 255, 255, 255), strText);
		DrawStringM2(Hack.Drawings->hFont, iX + ((float)iWidth / (float)2) - 3, iY + 10, Color(255, 255, 255, 255), pchar);

		if (Holding(iX, iY, iWidth, 20))
		{
			iVar = MouseXPos(iX, iY, iWidth, 20) / ((float)iWidth / (float)(iMax));
		}		
	}
}

void CMenu::DrawDropdown(const char* strText, int iTabIndex, int iX, int iY, int iAdd, const char** chTable, int iAmount, int &iVar)
{
	if (iTab == iTabIndex)
	{

		DrawOutlinedRectM(iX - 50, iY, 100, iAdd, Color(220, 24, 24, 255));

		DrawOutlinedRectM(iX - 50, iY, 100, iAdd + (iAmount * iAdd) + 1, Color(220, 24, 24, 255));

		DrawStringM(Hack.Drawings->hFont, iX, iY + 10, Color(255, 255, 255, 255), strText);


		for (int iVarIndex = 1; iVarIndex < iAmount + 1; iVarIndex++)
		{
			const char* strMeme = chTable[iVarIndex - 1];

			if (Hovering(iX - 50, iY + iVarIndex * iAdd, 100, iAdd))
				DrawRectM(iX - 49, iY + iVarIndex * iAdd, 98, iAdd, Color(255, 255, 255, 125));

			if (Holding(iX - 50, iY + iVarIndex * iAdd, 100, iAdd))
			{
				iVar = iVarIndex;
			}			

			DrawStringM(Hack.Drawings->hFont, iX, iY + (iAdd / 2) + (iVarIndex * iAdd), iVar == iVarIndex ? Color(125, 125, 125, 255) : Color(255, 255, 255, 255), strMeme);
		}
	}
}

void CMenu::DrawTab(const char* strText, int iTabIndex)
{
	int iBaseX = (iWidth / 4) * (iTabIndex - 1);
	int iBaseW = iWidth / 4;

	DrawRectM(iBaseX, 24, iBaseW, 28, iTab == iTabIndex ? Color(200, 5, 5, 255) : Color(220, 24, 24, 255));
	DrawStringM(Hack.Drawings->hFont, iBaseX + (iBaseW / 2), 38, Color(255, 255, 255, 255), strText);

	if (Hovering(iBaseX, 24, iBaseW, 28))
		DrawRectM(iBaseX, 24, iBaseW, 28, Color(255, 255, 255, 50));

	if (Clicked(iBaseX, 24, iBaseW, 28))
		iTab = iTabIndex;
}

void CMenu::DrawBackground()
{
	DrawRectM(0, 0, iWidth, iHeight, Color(0, 0, 0, 200));
	DrawRectM(0, 0, iWidth, 24, Color(220, 24, 24, 255));
	DrawStringM(Hack.Drawings->hFont, iWidth / 2, 12, Color(255, 255, 255, 255), "reDHook");
	DrawOutlinedRectM(0, 0, iWidth, iHeight, Color(220, 24, 24, 255));
}

void CMenu::DrawClose()
{
	DrawRectM(iWidth - 24, 0, 24, 24, Color(255, 0, 0, 255));
	DrawStringM(Hack.Drawings->hFont, iWidth - 12, 12, Color(255, 255, 255, 255), "X");

	if (Clicked(iWidth - 24, 0, 24, 24))
		bMenuOpen = false;
}

void CMenu::DrawButtons()
{
	int y = 58;
	int add = 22;

	DrawTab("Aimbot", 1);
	DrawTab("Visuals", 2);
	DrawTab("Misc", 3);

	DrawButton("Aimbot", 1, 8, y, gCvars.ABEnabled);
	DrawSlider("FOV", 1, 8, y += add, 180, 180, gCvars.FOV);
	DrawDropdown("Key", 1, iWidth - 160, 58, add, key, 4, gCvars.AimKey);
	DrawDropdown("Hitbox", 1, iWidth - 58, 58, add, hitbox, 3, gCvars.Hitbox);
	DrawButton("Silent", 1, 8, y += add, gCvars.Silent);
	DrawSlider("Smoothness", 1, 8, y += add, 180, 10, gCvars.Smoothness);
	DrawButton("AutoShoot", 1, 8, y += add, gCvars.AutoShoot);
	DrawButton("Ignore Cloak", 1, 8, y += add, gCvars.igncloak);
	DrawButton("Remove Disguise", 1, 8, y += add, gCvars.removedisg);
	DrawButton("Projectile Prediction", 1, 8, y += add, gCvars.projpred);
	DrawButton("[AA]Fake Center", 1, 8, y += add, gCvars.FakeCenter);
	DrawButton("Trigger Bot", 1, 8, y += add, gCvars.trigger);

	y = 58;
	DrawButton("Draw Players", 2, 8, y, gCvars.Players);
	DrawButton("Draw Bones", 2, 8, y += add, gCvars.Bones);
	DrawButton("Draw Objects", 2, 8, y += add, gCvars.Objects);
	DrawButton("Draw FOV", 2, 8, y += add, gCvars.fov);
	DrawButton("Lines To Players", 2, 8, y += add, gCvars.TraceLines);

	y = 58;
	DrawButton("Bunny Hop", 3, 8, y, gCvars.Bhop);
	DrawButton("Auto Strafe", 3, 8, y += add, gCvars.autostrafe);
	DrawButton("ThirdPerson", 3, 8, y += add, gCvars.thirdperson);
	DrawButton("Report Spam", 3, 8, y += add, gCvars.chatsay);
	DrawButton("Auto Airblast", 3, 8, y += add, gCvars.airblast);
	DrawButton("Fake Lag", 3, 8, y += add, gCvars.fakelag);
	if (gCvars.fakelag)
	{
		DrawButton("Fake Lag Adaptive", 3, 12, y += add, gCvars.fakelagadaptive);
		DrawSlider("Fake Lag Var", 3, 12, y += add, 180, 15, gCvars.fakelagcons);
	}
	DrawButton("AirStuck", 3, 8, y += add, gCvars.airstuck);
	DrawDropdown("AStuck Key", 3, iWidth - 58, 58, add, key1, 3, gCvars.airstuckkey);
}


