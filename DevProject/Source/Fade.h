#ifndef __MODULEFADE_H__
#define __MODULEFADE_H__

#include "Defs.h"
#include "Log.h"
#include "Point.h"
#include "Module.h"
#include "App.h"

struct SDL_Rect;
class UI_Element;

class Fade : public Module
{
public:
	Fade();
	~Fade();

	bool Start();
	bool Update(float dt);
	bool PostUpdate();
	bool Fade2(float time = 2.0f);

	SDL_Texture* loadingTex;

	SDL_Rect LoadingRect = { 724,903,277,41 };

	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;
private:
	Uint32 start_time = 0;
	Uint32 total_time = 0;
	SDL_Rect screen;
};

#endif //__MODULEFADETOBLACK_