#include "SDK.h"

class CPlayer;
class CDrawings
{
public:
	CDrawings();
	~CDrawings();

	void	DrawString(int x, int y, Color clrColor, /*bool center,*/ wchar_t* szText);
	void	DrawString(int x, int y, Color clrColor, bool bCenter, const char* szText, ...);
	void    DrawString(int x, int y, Color dwColor, unsigned long font, const char *pszText, ...);
	void	DrawRect(int x, int y, int w, int h, Color clrColor);
	void	DrawOutlineRect(int x, int y, int w, int h, Color clrColor);
	void	DrawLine(int x0, int y0, int x1, int y1, Color clrColor);
	void    FrameHitbox(CPlayer* pPlayer, int iHitbox);
	void    DrawCircle(int x, int y, int radius, int segments, Color clrColor);
	void    DrawRadarBack(Color team);
	void    DrawRadarPoint(Vector3 vOriginX, Vector3 vOriginY, Vector3 qAngle, CPlayer *pBaseEntity, Color dwTeamColor);
	void	DrawPlayerBox(CPlayer *pEnt, const char* szName, Color dwColor, int health, int maxHealth);

	CUserCmd*		m_cmd;
	Vector2			m_position;
	Vector2			m_size;
	IClientEntity*	m_player;

	void Update(CUserCmd* pCmd)
	{
		m_cmd = pCmd;

		if (!m_cmd) {
			return;
		}

		int index = Interfaces.Engine->GetLocalPlayer();

		if (!index) {
			return;
		}

		m_player = Interfaces.EntList->GetClientEntity<IClientEntity>(index);
	}

	void RadarBG()
	{
		m_position.x = gCvars.x;
		m_position.y = gCvars.y;
		m_size.x = m_size.y = gCvars.w;

		Hack.Drawings->DrawRect(m_position.x, m_position.y, m_size.x, m_size.y, Color(50, 50, 50, 80));
		Hack.Drawings->DrawOutlineRect(m_position.x, m_position.y, m_size.x, m_size.y, Color::Black());
		Hack.Drawings->DrawRect(m_position.x + m_size.x / 2, m_position.y, 1, m_size.y, Color::Black());
		Hack.Drawings->DrawRect(m_position.x, m_position.y + m_size.y / 2, m_size.x, 1, Color::Black());
	}

	void Radar(IClientEntity *local)
	{

		m_position.x = gCvars.x;
		m_position.y = gCvars.y;
		m_size.x = m_size.y = gCvars.w;

		Vector3 lposition = local->GetAbsOrigin();

		int playerCount = Interfaces.Engine->GetMaxClients();

		for (int i = 1; i <= playerCount; i++)
		{
			IClientEntity* entity = Interfaces.EntList->GetClientEntity<IClientEntity>(i);

			if (!entity)
				continue;

			if (entity == local)
				continue;

			if (entity->GetTeam() == local->GetTeam())
				continue;

			if (entity->GetLifeState() == LIFE_DEAD)
				continue;

			Vector3 eposition = entity->GetAbsOrigin();

			if (eposition.IsZero())
				continue;

			float radar_x = lposition.x - eposition.x;
			float radar_y = lposition.y - eposition.y;

			float angle = DEG2RAD(m_cmd->viewangles.y);

			float rotate_x = radar_y * cos(angle) - radar_x * sin(angle);
			float rotate_y = radar_x * cos(angle) + radar_y * sin(angle);

			float final_x = (m_position.x + (m_size.x / 2)) + rotate_x / 10;
			float final_y = (m_position.y + (m_size.y / 2)) + rotate_y / 10;

			if (final_x < m_position.x)
				final_x = m_position.x;
			if (final_x > m_position.x + m_size.x)
				final_x = m_position.x + m_size.x - 1;
			if (final_y < m_position.y)
				final_y = m_position.y;
			if (final_y > m_position.y + m_size.y)
				final_y = m_position.y + m_size.y - 1;

			Color dot_color = Color::White();
			if (entity->GetTeam() == 2)
				dot_color = Color(236, 57, 57, 255);
			else if (entity->GetTeam() == 3)
				dot_color = Color(0, 129, 236, 255);
			Hack.Drawings->DrawRect(final_x, final_y, 4, 4, dot_color);
		}
	}

	RECT GetViewport()
	{
		RECT Viewport = { 0, 0, 0, 0 };
		int w, h;
		Interfaces.Engine->GetScreenSize(w, h);
		Viewport.right = w; Viewport.bottom = h;
		return Viewport;
	}

	void Clear(int x, int y, int w, int h, Color color)
	{
		Interfaces.Surface->DrawSetColor(color);
		Interfaces.Surface->DrawFilledRect(x, y, x + w, y + h);
	}

	RECT GetTextSize(DWORD font, const char* text)
	{
		size_t origsize = strlen(text) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

		RECT rect; int x, y;
		Interfaces.Surface->GetTextSize(font, wcstring, x, y);
		rect.left = x; rect.bottom = y;
		rect.right = x;
		return rect;
	}

	void Text(int x, int y, Color color, DWORD font, const char* text)
	{
		size_t origsize = strlen(text) + 1;
		const size_t newsize = 100;
		size_t convertedChars = 0;
		wchar_t wcstring[newsize];
		mbstowcs_s(&convertedChars, wcstring, origsize, text, _TRUNCATE);

		Interfaces.Surface->DrawSetTextFont(font);

		Interfaces.Surface->DrawSetTextColor(color);
		Interfaces.Surface->DrawSetTextPos(x, y);
		Interfaces.Surface->DrawPrintText(wcstring, wcslen(wcstring));
		return;
	}

	void GradientV(int x, int y, int w, int h, Color c1, Color c2)
	{
		Clear(x, y, w, h, c1);
		BYTE first = c2.r();
		BYTE second = c2.g();
		BYTE third = c2.b();
		for (int i = 0; i < h; i++)
		{
			float fi = i, fh = h;
			float a = fi / fh;
			DWORD ia = a * 255;
			Clear(x, y + i, w, 1, Color(first, second, third, ia));
		}
	}

	void DrawStringA(HFont font, int x, int y, Color color, const char* msg, ...)
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, msg);
		_vsnprintf(buf, sizeof(buf), msg, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int Wid, Hit;

		Interfaces.Surface->GetTextSize(font, wbuf, Wid, Hit);
		Interfaces.Surface->DrawSetTextFont(font);
		Interfaces.Surface->DrawSetTextColor(color);
		Interfaces.Surface->DrawSetTextPos(x - Wid / 2, y - Hit / 2);
		Interfaces.Surface->DrawPrintText(wbuf, wcslen(wbuf));
	}

	void DrawStringB(HFont font, int x, int y, Color color, const char* msg, ...)
	{
		va_list va_alist;
		char buf[1024];
		va_start(va_alist, msg);
		_vsnprintf(buf, sizeof(buf), msg, va_alist);
		va_end(va_alist);
		wchar_t wbuf[1024];
		MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

		int Wid, Hit;

		Interfaces.Surface->GetTextSize(font, wbuf, Wid, Hit);
		Interfaces.Surface->DrawSetTextFont(font);
		Interfaces.Surface->DrawSetTextColor(color);
		Interfaces.Surface->DrawSetTextPos(x, y - Hit / 2);
		Interfaces.Surface->DrawPrintText(wbuf, wcslen(wbuf));
	}

	inline Color redGreenGradiant(float percent)
	{
		if (percent < 0 || percent > 1) { return Color(0, 255, 0, 255); }

		int r = 0, g = 0;
		if (percent < 0.5)
		{
			r = 255;
			g = (int)(255 * percent / 0.5);
		}
		else
		{
			g = 255;
			r = 255 - (int)(255 * (percent - 0.5) / 0.5);
		}
		return Color(r, g, 0, 255);
	}

	Color redGreenGradiant(int i, int max)
	{
		float percent = float(i) / float(max);
		return redGreenGradiant(percent);
	}

	void SpecList();

	HFont	hFont;

};