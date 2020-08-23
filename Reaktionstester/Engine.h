#pragma once
#include "PGEGui.h"

struct Circle
{
	olc::vf2d pos;
	uint32_t radius;
	olc::Decal* pDecal;
	bool bVisible;

private:
	static olc::Sprite* GenerateSprite(const uint32_t radius, const olc::Pixel& fg, const olc::Pixel& bg)
	{
		uint32_t d = 2 * radius;
		uint32_t r2 = radius * radius;

		olc::Sprite* spr = new olc::Sprite(d, d);

		for (uint32_t y = 0; y < d; y++)
		{
			for (uint32_t x = 0; x < d; x++)
			{
				// calculate the distance to the middle point
				uint32_t dx = x - radius;
				uint32_t dy = y - radius;

				spr->SetPixel(x, y, (dx * dx + dy * dy < r2) ? fg : bg);
			}
		}

		return spr;
	}

public:

	Circle()
	{
		pos = olc::vf2d();
		radius = 0;
		pDecal = nullptr;
		bVisible = false;
	}

	Circle(olc::vf2d pos, uint32_t radius, const olc::Pixel& fg, const olc::Pixel& bg)
	{
		this->pos = pos;
		this->radius = radius;
		this->pDecal = new olc::Decal(GenerateSprite(radius, fg, bg));
		bVisible = true;
	}

	void updateSize(uint32_t radius, const olc::Pixel& fg, const olc::Pixel& bg)
	{
		delete pDecal;
		pDecal = new olc::Decal(GenerateSprite(radius, fg, bg));
	}

	bool isHit(const olc::vf2d& pos)
	{
		// calculate the square (its faster) of the distance from the middle point to pos
		olc::vf2d middle = this->pos + olc::vf2d(0.5f * pDecal->sprite->width, 0.5f * pDecal->sprite->height);
		olc::vf2d delta = (pos - middle);

		delta = delta * delta;
		float fSquare = delta.x + delta.y;

		return (fSquare <= radius * radius);
	}
};

class Engine : public PGEGui
{
private:
	// Circle stuff
	Circle circle;

	// Timing
	float fMaxTime = 1.0f;
	std::chrono::time_point<std::chrono::system_clock> tPlaced = std::chrono::system_clock::now();
	float fTimeForNextPlacement = 0.0f;

	// Results
	std::vector<float> vResults;
	uint32_t nRuns = 0;
	uint32_t nWonRuns = 0;

public:
	Engine()
	{
		sAppName = "Reaktionstester";
	}

protected:
	virtual bool OnUserCreate() override
	{
		// Create the Circle
		circle = Circle({ 100, 100 }, 100, olc::RED, olc::BLACK);

		return true;
	}

	virtual bool OnUserUpdate(float fElapsedTime) override
	{
		// halt the Engine if Key 'ESC' is pressed
		if (GetKey(olc::Key::ESCAPE).bPressed) return false;

		olc::vf2d vMouse = { (float)GetMouseX(), (float)GetMouseY() };

		fTimeForNextPlacement -= fElapsedTime;
		if (fTimeForNextPlacement < 0)
		{
			circle.bVisible = true;
			tPlaced = std::chrono::system_clock::now();
			fTimeForNextPlacement = 1000000000.0f;
		}

		// Update the circle if visible
		if (circle.bVisible)
		{
			DrawDecal(circle.pos, circle.pDecal);

			if (GetMouse(0).bPressed)
			{
				if (circle.isHit(vMouse))
				{
					// calculate the elapsed time
					uint32_t nElapsedMicros = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - tPlaced).count();
					float fElapsed = (float)nElapsedMicros / 1'000'000.0f;
					vResults.push_back(fElapsed);
					nWonRuns++;
				}

				circle.bVisible = false;
				nRuns++;

				olc::vf2d newPos = olc::vf2d(100 + Lehmer32() % (ScreenWidth() - 300), 100 + Lehmer32() % (ScreenHeight() - 300));
				circle.pos = newPos;

				// setup a cooldown
				fTimeForNextPlacement = (float)(Lehmer32() % (uint32_t)(fMaxTime * 1e6f)) / 1e6f;
			}
		}

		DrawStringDecal({ 5, 50 }, "played rounds: " + std::to_string(nRuns));
		DrawStringDecal({ 5, 60 }, "won runs: " + std::to_string(nWonRuns));

		return true;
	}

	virtual bool OnUserDestroy() override
	{
		return true;
	}
};