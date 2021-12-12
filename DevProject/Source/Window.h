#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "Module.h"

struct SDL_Window;
struct SDL_Surface;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before quitting
	bool CleanUp();

	// Change title
	void SetTitle(const char* new_title);

	// Retrive window size
	void GetWindowSize(uint& width, uint& height) const;

	// Retrieve window scale
	uint GetScale() const;

	uint GetWindowHeight() const {
		return height;
	}
	uint GetWindowWidth() const {
		return width;
	}

public:
	SDL_Window* window;

	SDL_Surface* screen_surface;

private:
	SString	title;
	uint		width;
	uint		height;
	uint		scale;
};

#endif // __WINDOW_H__