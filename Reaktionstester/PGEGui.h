/*  My olc::PixelGameEngine 2.0 GUI Extension
 *  It features 2d Zoom and pan functions: ScreenToWorld, WorldToScreen, GetZoom
 *  It provides six GUI Classes: Widgets, Sliders, Buttons, Labels, Pictures, InputWidgets and User Interfaces
 *  and it gives access to a very fast random number function: Lehmer32
 */


#pragma once
 // Some includes
#include "olcPixelGameEngine.h"
#include <Windows.h>
#include <vector>

// You need to override this class
class PGEGui : public olc::PixelGameEngine
{
protected:
	// some variables for the panning and zooming
	olc::vf2d panOffset, panStart, vZoomBorder;
	float fZoom;

	// it updates the pan and zoom values
	void UpdatePanAndZoom(float fElapsedTime, bool bZoomAllowed = true)
	{
		olc::vf2d vMouse = olc::vf2d(GetMouseX(), GetMouseY());

		// pan
		if (GetMouse(1).bPressed)
		{
			panStart = vMouse;
		}
		if (GetMouse(1).bHeld)
		{
			panOffset += (vMouse - panStart) / fZoom;
			panStart = vMouse;
		}

		// zoom
		olc::vf2d vMouseWorldBeforeZoom = ScreenToWorld(vMouse);
		float fAdd = 1.1f * fZoom * fElapsedTime;
		if (bZoomAllowed && GetKey(olc::Key::Q).bHeld) fZoom += fAdd; if (fZoom < vZoomBorder.x) fZoom = vZoomBorder.x;
		if (bZoomAllowed && GetKey(olc::Key::E).bHeld) fZoom -= fAdd; if (fZoom > vZoomBorder.y) fZoom = vZoomBorder.y;
		olc::vf2d vMouseWorldAfterZoom = ScreenToWorld(vMouse);
		panOffset += (vMouseWorldAfterZoom - vMouseWorldBeforeZoom);
	}

public:
	// some 2d zoom and pan functions
	inline float GetZoom()
	{
		return fZoom;
	}
	inline olc::vf2d WorldToScreen(const olc::vf2d vWorld)
	{
		return (vWorld + panOffset) * fZoom;
	}
	inline olc::vf2d ScreenToWorld(const olc::vf2d vScreen)
	{
		return vScreen / fZoom - panOffset;
	}

protected:
	// The Slider
	struct Slider;

	// A widget is the base class for every GUI object
	struct Widget
	{
		olc::vi2d pos, size;
		bool bVisible;
		int type = 0;
		Slider* sldAffected = nullptr;

		inline Widget() { pos = size = olc::vi2d(); bVisible = true; }
		inline Widget(olc::vi2d pos, olc::vi2d size)
		{
			this->pos = pos;
			this->size = size;
			this->bVisible = true;
		}
		inline void Show()
		{
			bVisible = true;
		}
		inline bool IsVisible()
		{
			return bVisible;
		}
		inline olc::vi2d Pos()
		{
			if (sldAffected == nullptr) return pos;
			return pos - sldAffected->fPanScale * sldAffected->vSliderOffset;
		}

		void SetSlider(Slider* slider)
		{
			this->sldAffected = slider;
		}

		virtual inline void Draw(PGEGui* pge) = 0;

		inline int Identify()
		{
			return type;
		}
		virtual bool is_button()
		{
			return false;
		}
		virtual void Add_Pos(olc::vi2d dpos)
		{
			pos += dpos;
		}
	};

	// A button can be pressed
	struct Button : public Widget
	{
		olc::vi2d vTextPos, vTextOffset;
		olc::vf2d vTextScale, vButtonScale;
		std::string sText;
		std::string sHint = "";
		olc::Pixel pTextCol;
		bool bDrawBG = true;

		Button()
		{
			type = 1;
			pos = size = vTextPos = vTextOffset = olc::vi2d();
			vTextScale = vButtonScale = olc::vf2d();
		}
		Button(olc::vi2d pos, olc::vi2d size, std::string sText, olc::Pixel col, Slider* sld = nullptr)
		{
			type = 1;
			this->pos = pos; this->size = size;
			this->sText = sText;
			pTextCol = col;
			sldAffected = sld;

			vButtonScale = olc::vf2d((float)size.x / 200.0f, (float)size.y / 75.0f);

			// Get the dimensions of the text size
			int nLines = 1, nLongestLine = 0, nCurrentLine = 0;
			for (char c : sText)
			{
				if (c == '\n')
				{
					nLines++;
					nCurrentLine = 0;
				}
				else
				{
					nCurrentLine++;
				}

				if (nCurrentLine > nLongestLine) nLongestLine = nCurrentLine;
			}

			float fX = 8.0f * nLongestLine;
			float fY = 8.0f * nLines;

			float fSX = (float)size.x / fX;
			float fSY = (float)size.y / fY;

			if (fSY < fSX)
			{
				vTextScale = { fSY, fSY };
				vTextOffset.x = ((float)size.x - fX * vTextScale.x) / 2.0f;
			}
			else
			{
				vTextScale = { fSX, fSX };
				vTextOffset.y = ((float)size.y - fY * vTextScale.y) / 2.0f;
			}
		}

		virtual inline void Draw(PGEGui* pge) override
		{
			if (bVisible)
			{
				if (pge->vButton.size() > 0 && bDrawBG)
				{
					// if hovered
					olc::vi2d vPos = Pos();
					if (vPos.x <= pge->GetMouseX() && pge->GetMouseX() <= vPos.x + size.x && vPos.y <= pge->GetMouseY() && pge->GetMouseY() <= vPos.y + size.y)
					{
						// if pressed or held
						if (pge->GetMouse(0).bPressed || pge->GetMouse(0).bHeld)
							pge->DrawDecal(Pos(), pge->vButton_Pressed.at(0), vButtonScale);
						// else just hovered
						else
							pge->DrawDecal(Pos(), pge->vButton_Hover.at(0), vButtonScale);
					}
					else
						pge->DrawDecal(Pos(), pge->vButton.at(0), vButtonScale);
				}

				// draws the text
				pge->DrawStringDecal(Pos() + vTextOffset, sText, pTextCol, vTextScale);
			}
		}
		virtual inline bool isPressed(PGEGui* pge)
		{
			if (pge->GetMouse(0).bReleased && bVisible)
			{
				olc::vi2d vPos = Pos();
				return (vPos.x <= pge->GetMouseX() && pge->GetMouseX() <= vPos.x + size.x &&
						vPos.y <= pge->GetMouseY() && pge->GetMouseY() <= vPos.y + size.y);
			}
			return false;
		}
		virtual bool is_button() override
		{
			return true;
		}
	};

	// A Label shows text
	struct Label : public Widget
	{
		olc::vi2d textlen;
		olc::vf2d scale;
		olc::vi2d vTextOffset = olc::vi2d();
		std::string sText;
		olc::Pixel pTextColor;

		inline void PrepareText()
		{
			textlen.x = 8 * sText.length();                         // 8 px in x-axis per char
			for (char c : sText) { if (c == '\n') textlen.y += 8; } // 8 px in y-axis per '\n'
			scale = { (float)size.x / (8.0f * (float)sText.length()), (float)size.y / 8.0f };
			if (scale.x < scale.y) scale.y = scale.x;
			else scale.x = scale.y;

			// center on y axis
			vTextOffset.y = (size.y - scale.y * textlen.y) / 2;
		}

		Label()
		{
			type = 2;
			textlen = { 0, 8 };
			scale = olc::vf2d();
			sText = std::string();
			pTextColor = olc::Pixel();
		}
		Label(olc::vi2d pos, olc::vi2d size, std::string sText, olc::Pixel textColor)
		{
			type = 2;
			textlen = { 0, 8 };
			this->pos = pos; this->size = size; pTextColor = textColor; this->sText = sText;
			PrepareText();
		}

		inline void Update_Text(const std::string& sText)
		{
			this->sText = sText;
			PrepareText();
		}

		virtual inline void Draw(PGEGui* pge) override
		{
			pge->DrawStringDecal(Pos() + vTextOffset, sText, pTextColor, scale);
		}

		virtual inline bool isPressed(PGEGui* pge)
		{
			if (pge->GetMouse(0).bReleased)
			{
				return (pos.x <= pge->GetMouseX() && pge->GetMouseX() <= pos.x + size.x &&
						pos.y <= pge->GetMouseY() && pge->GetMouseY() <= pos.y + size.y);
			}

			return false;
		}
	};

	struct InputWidget : public Widget
	{
		Label* lblText;
		olc::Decal* decBG;
		std::array<bool, 255> keys;
		std::string sText;
		bool bNumbersAllowed = true;
		bool bLettersAllowed = true;
		bool bSpecialAllowed = true;
		bool bCRLFAllowed    = true;

	public:
		InputWidget()
		{
			type = 5;
			lblText = nullptr;
			decBG = nullptr;
			keys = std::array<bool, 255>();

		}
		InputWidget(olc::vi2d pos, olc::vi2d size, olc::Pixel fg, olc::Pixel bg)
		{
			type = 5;
			this->pos = pos;
			this->size = size;
			lblText = new Label(pos, size, "", fg);
			keys = std::array<bool, 255>();

			olc::Sprite* spr = new olc::Sprite(1, 1);
			spr->SetPixel(0, 0, bg);
			decBG = new olc::Decal(spr);
		}
		~InputWidget()
		{
			delete lblText;
		}

	private:
		std::vector<char> GetPressedKeys(bool bNumbersAllowed = true, bool bLettersAllowed = true, bool bSpecialAllowed = true, bool bCRLFAllowed = true)
		{
			std::vector<char> vPressedKeys;
			std::array<bool, 255> vKeyStates{ 0 };

			bool bShift = GetAsyncKeyState(VK_SHIFT) != 0;
			bool bAltGr = GetAsyncKeyState(17) != 0;

			for (int i = 0; i < 255; i++)
			{
				short result = GetAsyncKeyState(i);

				// pressed
				if (result != 0 && !keys.at(i))
				{
					keys.at(i) = true;
					vKeyStates.at(i) = true;
				}
				if (result == 0)
				{
					keys.at(i) = false;
				}
			}
			

			// Keys A-Z
			if (bLettersAllowed)
			{
				const char nonShiftedKeys[] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z' };
				const char    shiftedKeys[] = { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
				for (int i = 65; i < 91; i++)
				{
					if (vKeyStates.at(i))
					{
						char c;
						if (bShift) c = shiftedKeys[i - 65];
						else c = nonShiftedKeys[i - 65];
						vPressedKeys.push_back(c);
					}
				}
			}
			
			if (bNumbersAllowed)
			{
				// Numbers 0-9
				const char nonShiftedNumbers[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
				const char    shiftedNumbers[] = { '=', '!', '"', '§', '$', '%', '&', '/', '(', ')' };
				const char      altedNumbers[] = { '}', '1', '2', '3', '4', '5', '6', '{', '[', ']' };
				for (int i = 48; i < 58; i++)
				{
					if (vKeyStates.at(i))
					{
						char c;
						if (bAltGr)
							c = altedNumbers[i - 48];
						else if (bShift)
							c = shiftedNumbers[i - 48];
						else
							c = nonShiftedNumbers[i - 48];
						vPressedKeys.push_back(c);
					}
				}
			}

			if (bSpecialAllowed)
			{
				// Space
				if (vKeyStates.at(32))
					vPressedKeys.push_back(' ');

				// Enter
				if (bCRLFAllowed && vKeyStates.at(13))
					vPressedKeys.push_back('\n');

				// Backspace
				if (vKeyStates.at(8))
					vPressedKeys.push_back('\b');

				const char nonShiftedKeys_0[] = {'+', ',', '-', '.', '#'};
				const char shiftedKeys_0[] = { '*', ';', '_', ':', '\'' };

				for (int i = 187; i < 192; i++)
				{
					if (vKeyStates.at(i))
					{
						char c;
						if (bShift) c = shiftedKeys_0[i - 187];
						else c = nonShiftedKeys_0[i - 187];
						vPressedKeys.push_back(c);
					}
				}
			}

			return vPressedKeys;
		}

	public:
		inline void Add_Pos(olc::vi2d dpos) override
		{
			pos += dpos;
			lblText->pos += dpos;
		}

		virtual inline void Draw(PGEGui* pge) override
		{
			// Update the Text
			const std::vector<char> vPressedKeys = GetPressedKeys(bNumbersAllowed, bLettersAllowed, bSpecialAllowed, bCRLFAllowed);
			for (char c : vPressedKeys)
			{
				if (c == '\b') // means deleting
				{
					if (sText.size() > 0) // do not delete an empty std::string
						sText.pop_back();
				}
				else
				{
					sText.push_back(c); // means writing a character
				}
			}

			// Update the Label
			lblText->Update_Text(sText);
			
			// Draw the background
			pge->DrawDecal(Pos(), decBG, size);

			// Draw the Label
			lblText->Draw(pge);
		}

		inline void reset()
		{
			sText.clear();
		}
	};

	// A Picture draws a Decal
	struct Picture : public Widget
	{
		olc::Decal* dec;
		olc::vf2d scale;

		inline Picture() { this->dec = nullptr; scale = olc::vf2d(); type = 3; }
		inline Picture(olc::vi2d pos, olc::vi2d size, olc::Decal* decPicture)
		{
			type = 3;
			this->pos = pos;
			this->size = size;
			this->dec = decPicture;
			scale = olc::vf2d((float)size.x / (float)decPicture->sprite->width, (float)size.y / (float)decPicture->sprite->height);
		}

		virtual inline void Draw(PGEGui* pge)
		{
			pge->DrawDecal(Pos(), dec, scale);
		}
	};

	// A Slider offsets Widgets
	struct Slider : public Widget
	{
		olc::vi2d panStart = olc::vi2d();
		olc::vi2d vSliderOffset;
		olc::vf2d vSliderScale, vBackgroundScale;
		olc::Decal *decBackground, *decSlider;
		size_t nMaxOffset;
		float fPanScale = 1.0f;
		bool bIsXAxis = true;
		bool bStillPressed = false;

		Slider() { type = 4; vSliderOffset = olc::vi2d(); decBackground = decSlider = nullptr; nMaxOffset = 0; }
		Slider(bool bXAxis, olc::vi2d pos, olc::vi2d size, size_t maxOffset, olc::Pixel colBackground, olc::Pixel colSlider)
		{
			type = 4;
			this->bIsXAxis = bXAxis;
			this->pos = pos;
			this->size = size;
			this->nMaxOffset = maxOffset;
			olc::vi2d vMaxSliderOffset = size;
			if (bIsXAxis) this->fPanScale = (float)nMaxOffset / (float)vMaxSliderOffset.x;
			else this->fPanScale = (float)nMaxOffset / (float)vMaxSliderOffset.y;
			// Decal Assets
			olc::Sprite *sprBackground, *sprSlider;
			sprBackground = new olc::Sprite(1, 1); sprBackground->SetPixel({ 0, 0 }, colBackground); decBackground = new olc::Decal(sprBackground);
			sprSlider     = new olc::Sprite(1, 1); sprSlider->SetPixel    ({ 0, 0 }, colSlider);     decSlider     = new olc::Decal(sprSlider);
			// Scaling
			vSliderScale = size;
			if (bXAxis) this->vSliderScale.x *= 0.2f;
			else this->vSliderScale.y *= 0.2f;
			vBackgroundScale = size;
		}
		virtual inline void Draw(PGEGui* pge) override
		{
			// Update
			Update(pge);

			// Background
			pge->DrawDecal(pos, decBackground, vBackgroundScale);
			
			// Slider
			olc::vi2d vMaxSliderOffset = size - vSliderScale;
			if (bIsXAxis)
			{
				size_t xOffset = vSliderOffset.x;
				pge->DrawDecal(pos + olc::vi2d(xOffset, 0), decSlider, vSliderScale);
			}
			else
			{
				size_t yOffset = vSliderOffset.y;
				pge->DrawDecal(pos + olc::vi2d(0, yOffset), decSlider, vSliderScale);
			}
		}
		virtual inline void Update(PGEGui* pge)
		{
			if (pge->GetMouse(0).bPressed) panStart = { pge->GetMouseX(), pge->GetMouseY() };
			olc::vi2d panDelta = olc::vi2d();
			if (pge->GetMouse(0).bHeld)
			{
				olc::vi2d mouse = olc::vi2d(pge->GetMouseX(), pge->GetMouseY());
				panDelta = (mouse - panStart);
				panStart = mouse;
			}

			// if the slider is pressed
			olc::vi2d vSliderPos = pos + vSliderOffset;
			if (pge->GetMouse(0).bHeld && ((vSliderPos.x <= pge->GetMouseX() && pge->GetMouseX() <= vSliderPos.x + size.x
				&& vSliderPos.y <= pge->GetMouseY() && pge->GetMouseY() <= vSliderPos.y + size.y) || bStillPressed))
			{
				bStillPressed = true;
				// add the panDelta to the axis
				if (bIsXAxis)
				{
					vSliderOffset.x += panDelta.x;
					if (vSliderOffset.x < 0) vSliderOffset.x = 0;
					if (vSliderOffset.x > 0.8f * size.x) vSliderOffset.x = 0.8f * size.x;
				}
				else
				{
					vSliderOffset.y += panDelta.y;
					if (vSliderOffset.y < 0) vSliderOffset.y = 0;
					if (vSliderOffset.y > 0.8f * size.y) vSliderOffset.y = 0.8f * size.y;
				}
			}
			else bStillPressed = false;
		}
	};

	// A UI makes drawing a GUI way easier
	struct UserInterface
	{
		olc::vi2d pos, size;
		Button btnClose;
		Picture ptcBackground;
		Label lblName;
		std::vector<Widget*> vWidgets;
		bool bIsVisible = true;

		inline UserInterface()
		{
			pos = size = olc::vi2d();
			lblName = Label();
			btnClose = Button();
			ptcBackground = Picture();
		}
		inline UserInterface(olc::vi2d pos, olc::vi2d size, olc::Decal* decBackground, std::string sName)
		{
			this->pos = pos;
			this->size = size;
			this->ptcBackground = Picture(pos, size, decBackground);
			this->lblName = Label(pos, olc::vi2d(0.9f * size.x, 0.1f * size.y), sName, olc::BLACK);
			this->btnClose = Button(pos + olc::vi2d(size.x - 0.1f * size.y, 0), olc::vi2d(0.1f * size.y, 0.1f * size.y), "X", olc::BLACK);
			this->btnClose.bDrawBG = false;
		}
		inline void Add(Widget* wWidget)
		{
			vWidgets.push_back(wWidget);
			vWidgets.back()->Add_Pos({ 0, btnClose.size.y });
			vWidgets.back()->Add_Pos(pos);
		}
		inline void Show() { bIsVisible = true; }
		inline void Vanish() { bIsVisible = false; }

		inline void Update(PGEGui* pge)
		{
			for (Widget* w : vWidgets)
			{
				if (w->Identify() == 4)
				{
					static_cast<Slider*>(w)->Update(pge);
				}
			}
		}
		inline void Draw(PGEGui* pge)
		{
			if (btnClose.isPressed(pge)) bIsVisible = false;

			if (bIsVisible)
			{
				// DrawBackground
				ptcBackground.Draw(pge);
				// Draw Title
				lblName.Draw(pge);
				// Draw Close Button
				btnClose.Draw(pge);

				// Widgets
				for (Widget* widget : vWidgets)
				{
					widget->Draw(pge);
				}
			}
		}
	};

protected:
	// general Button Assets
	std::vector<olc::Decal*> vButton;
	std::vector<olc::Decal*> vButton_Hover;
	std::vector<olc::Decal*> vButton_Pressed;

protected:
	// The seed for the random number function
	uint32_t nLehmer = 0;
	// The fast 32-bit random generator
	inline uint32_t Lehmer32()
	{
		nLehmer += 0xe120fc15;
		uint64_t temp;
		temp = (uint64_t)nLehmer * 0x4a39b70d;
		uint32_t m1 = (temp >> 32) ^ temp;
		temp = (uint64_t)m1 * 0x12fad5c9;
		uint32_t m2 = (temp >> 32) ^ temp;
		return m2;
	}
};