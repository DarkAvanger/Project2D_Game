#include <math.h>
#include "Defs.h"
#include "App.h"
#include "Fade.h"
#include "Render.h"
#include "SDL/include/SDL_render.h"
#include "SDL/include/SDL_timer.h"
#include "Player.h"
#include "Window.h"
#include "Gui.h"


Fade::Fade()
{
	name.create("Fade");
	screen = { 0, 0, 1000,500 };
}

Fade::~Fade()
{}

// Load assets
bool Fade::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	loadingTex = App->gui->GetAtlas();

	return true;
}

// Update: draw background
bool Fade::Update(float dt)
{
	return true;
}


bool Fade::PostUpdate() {


	bool ret = true;
	if (current_step != fade_step::none) {
		Uint32 now = SDL_GetTicks() - start_time;
		float normalized = MIN(1.0f, (float)now / (float)total_time);

		switch (current_step)
		{
		case fade_step::fade_to_black:
		{
			if (now >= total_time)
			{

				total_time += total_time;
				start_time = SDL_GetTicks();
				current_step = fade_step::fade_from_black;

			}
		} break;

		case fade_step::fade_from_black:
		{
			normalized = 1.0f - normalized;

			if (now >= total_time) {
				current_step = fade_step::none;
			}
		} break;
		}

		SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
		SDL_RenderFillRect(App->render->renderer, &screen);
		App->render->Blit(loadingTex, 310, 250, &LoadingRect, false);
	}
	return ret;

}


bool Fade::Fade2(float time)
{
	bool ret = false;


	if (current_step == fade_step::none)
	{
	
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time * 0.5f * 1000.0f);
		ret = true;

	}

	return ret;
}
