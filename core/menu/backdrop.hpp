

#include "../../dependencies/common_includes.hpp"
#include "../../dependencies/Vector2D.h"
//#include <vector>

class Dot;


extern std::vector<Dot*> Dots;

class Dot
{
public:
	Vector2D RelativePosition;
	Vector2D Velocity;
	
	Dot(Vector2D p, Vector2D v)
	{
		RelativePosition = p;
		Velocity = v;
	}
	virtual void Draw()
	{
		//146 / 255.f, 97 / 255.f, 201 / 255.f, 255 / 255.f
		auto red = config_system.item.clr_theme[0] * 255 ;
		auto green = config_system.item.clr_theme[1] * 255;
		auto blue = config_system.item.clr_theme[2] * 255;
		interfaces::surface->set_drawing_color(red, green, blue, 20);
		
		auto t = std::find(Dots.begin(), Dots.end(), this);
		if (t == Dots.end()) return;
		for (auto i = t; i != Dots.end(); i++)
		{
			if ((*i) == this) continue;
			Vector2D Pos = RelativePosition;
			float Dist = Pos.DistTo((*i)->RelativePosition);
			if (Dist < 128)
			{
				Vector2D Dir = ((*i)->RelativePosition - RelativePosition).Normalized();
				//	g_pSurface->DrawSetColor(Color(69, 140, 230, 255));
				interfaces::surface->draw_line(Pos.x, Pos.y, (Pos + Dir * Dist).x, (Pos + Dir * Dist).y);

			}
		}
	}

	
};

namespace Drop
{
	void DrawBackDrop();

}
