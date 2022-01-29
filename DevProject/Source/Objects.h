#ifndef _OBJECTS_
#define _OBJECTS_

#include "Entity.h"
#include "Animation.h"

enum OBJECT_TYPE {

	PUSHOFF,
	PUSHON,
	WIN,
	BLINK_BLOCK

};

class Objects : public Entity
{

public:

	Objects(iPoint pos, OBJECT_TYPE objType);
	~Objects();
	

public:

	Objects* CreateObject(iPoint pos, OBJECT_TYPE objType);

	void LoadPushbacks(pugi::xml_node node, Animation& animation);

public:

	OBJECT_TYPE type;
	SDL_Texture *Obj_tex = nullptr;
	Animation pushoff_anim;

};

#endif