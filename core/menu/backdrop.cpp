#include "backdrop.hpp"
#include "../../dependencies/common_includes.hpp"
#include "../../dependencies/Vector2D.h"

std::vector<Dot*> Dots;

void Drop::DrawBackDrop()
{


	static int width, height;
	interfaces::engine->get_screen_size(width, height);

	interfaces::surface->set_drawing_color(1.f * 0, 1.f * 0, 1.f* 0 , 195);
	interfaces::surface->draw_filled_rectangle(0, 0, width, height);
	int s = rand() % 24;

	if (s == 0)
		Dots.push_back(new Dot(Vector2D(rand() % (int)width, -16), Vector2D((rand() % 7) - 3, rand() % 3 + 1)));
	else if (s == 1)
		Dots.push_back(new Dot(Vector2D(rand() % (int)width, (int)height + 16), Vector2D((rand() % 7) - 3, -1 * (rand() % 3 + 1))));

	for (auto i = Dots.begin(); i < Dots.end();)
	{
		if ((*i)->RelativePosition.y < -20 || (*i)->RelativePosition.y > (height + 20) || (*i)->RelativePosition.x < -20 || (*i)->RelativePosition.x > (width + 20))
		{
			delete (*i);
			i = Dots.erase(i);
		}
		else
		{
			(*i)->RelativePosition = (*i)->RelativePosition + (*i)->Velocity * (0.3);
			i++;
		}
	}
	for (auto i = Dots.begin(); i < Dots.end(); i++)
		(*i)->Draw();
}
