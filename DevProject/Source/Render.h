#ifndef __RENDER_H__
#define __RENDER_H__

#include "SDL/include/SDL.h"
#include "Point.h"
#include "Module.h"
#include "Timer.h"

class Render : public Module
{
public:

	Render();

	// Destructor
	virtual ~Render();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load / Save
	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	// Utils
	void SetViewPort(const SDL_Rect& rect);
	void ResetViewPort();
	iPoint ScreenToWorld(int x, int y) const;

	// Draw & Blit
	bool Blit(SDL_Texture* texture, int x, int y, const SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX,SDL_RendererFlip flip = SDL_FLIP_NONE,float scale_ = 1)const;
	//bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true) const;
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true, float scale_ = 1.0) const;
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true) const;


	// Set background color
	void SetBackgroundColor(SDL_Color color);
	void ResetCamera(){
		camera.x = 0;
		camera.y = 0;
	}
	void CameraShake(float power);

	bool DoCameraShake = false;

	float power = 0;
	float Time_Doing_Shake;
	Timer CameraShake_Time;


public:

	SDL_Renderer*	renderer;
	SDL_Rect		camera;
	SDL_Rect		viewport;
	SDL_Color		background;
};

#endif // __RENDER_H__