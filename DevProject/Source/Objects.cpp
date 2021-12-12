#include "Objects.h"
#include "EntityManager.h"
#include "Collisions.h"
#include "Map.h"
#include "Log.h"

Objects::Objects(iPoint pos, OBJECT_TYPE objType) : Entity(ENTITY_TYPE::OBJECT_ENT) { name.create("Objects"); }

Objects::~Objects() {}

Objects* Objects::CreateObject(iPoint pos, OBJECT_TYPE objType) {

	Objects* Object = nullptr;
	Object = new Objects(pos, objType);

	App->entity_manager->entities_list.add(Object);
	return Object;

}