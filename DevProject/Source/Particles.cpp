#include "App.h"
#include "Textures.h"
#include "Render.h"
#include "Particles.h"
#include "Player.h"
#include "Scene.h"
#include "Collisions.h"

#include "SDL/include/SDL_timer.h"


Particles::Particles() {

	name.create("Particles");
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i) 
		active[i] = nullptr;
}

Particles::~Particles() {}

bool Particles::Start() {


	pugi::xml_parse_result result = ParticleDocument.load_file("Particle_Settings.xml");
	if (result == NULL)
		LOG("The xml file containing the player tileset fails. Pugi error: %s", result.description());



	Animation_node = ParticleDocument.child("config").child("Particle_Pushbacks").child("Blood");
	App->scene->Player->LoadPushbacks(Animation_node, Blood.Anim);
	Animation_node = ParticleDocument.child("config").child("Particle_Pushbacks").child("Player_Kunai");
	App->scene->Player->LoadPushbacks(Animation_node, Player_Shoot.Anim);
	Animation_node = ParticleDocument.child("config").child("Particle_Pushbacks").child("Enemy_Kunai");
	App->scene->Player->LoadPushbacks(Animation_node, Enemy_Shoot.Anim);
	Animation_node = ParticleDocument.child("config").child("Particle_Pushbacks").child("Player_Shot_Kunai");
	App->scene->Player->LoadPushbacks(Animation_node, Player_Shoot_Beam.Anim);
	Animation_node = ParticleDocument.child("config").child("Particle_Pushbacks").child("Explosion");
	App->scene->Player->LoadPushbacks(Animation_node, Plasma_Explosion.Anim);

	Particle_1 = App->tex->Load("maps/Particles.png");


	Player_Shoot.Sprites = Blood.Sprites = Enemy_Shoot.Sprites = Plasma_Explosion.Sprites = Player_Shoot_Beam.Sprites = Particle_1;
	Enemy_Shoot.Life = 600;
	Player_Shoot.Life = 3200;
	Player_Shoot.collider = App->collisions->AddCollider({0,0,80,10},COLLIDER_PLAYER_BULLET,this);
	Enemy_Shoot.collider = App->collisions->AddCollider({ 0,0,50,10 }, COLLIDER_ENEMY_BULLET, this);
	return true;

}

bool Particles::CleanUp() {

	LOG("Unloading Particles");

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {

		if (active[i] != nullptr) {
			App->tex->UnLoad(active[i]->Sprites);
			delete active[i];
			active[i] = nullptr;
		}
	}

	return true;
}

bool Particles::Update(float dt) {


	bool ret = true;
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {

		Particle *p = active[i];

		if (p == nullptr) 
			continue;
		

		if (p->Update() == false) {

			delete p;
 			active[i] = nullptr;
		}

		else if (SDL_GetTicks() >= p->Born) {

			App->render->Blit(p->Sprites, p->Position.x, p->Position.y, &(p->Anim.GetCurrentFrame(dt)),1,0,0,0,p->Flip,p->scale);

			if (!p->fx_played)
				p->fx_played = true;
			
		}
	}

	return ret;
}

void Particles::AddParticle(const Particle& particle, int x, int y, COLLIDER_TYPE collider_type, fPoint speed,float scale, SDL_RendererFlip Flip, Uint32 delay) {

	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i) {
		if (active[i] == nullptr) {

			Particle* p = new Particle(particle);
			p->Born = SDL_GetTicks() + delay;
			p->Position.x = x;
			p->Position.y = y;
			p->Sprites = particle.Sprites;
			p->Speed.x = speed.x * App->GetDT();
			p->Speed.y = speed.y * App->GetDT();
			p->Flip = Flip;
			p->scale = scale;
			if (collider_type != COLLIDER_NONE && p->collider==nullptr) {
					p->collider = App->collisions->AddCollider(p->Anim.GetCurrentFrame(App->GetDT()), collider_type, this);
			}
			active[i] = p;
			break;

		}
	}
}

void Particles::OnCollision(Collider* c1, Collider* c2)
{
	for (uint i = 0; i < MAX_ACTIVE_PARTICLES; ++i)
	{
		if (active[i] != nullptr && active[i]->collider == c1) {

			if (c2->type == COLLIDER_STATIC) {

				delete active[i];
				active[i] = nullptr;

				break;
			}
		}
	}
}

Particle::Particle() {

	Position.SetToZero();
	Speed.SetToZero();
}

Particle::Particle(const Particle &p) :
	Anim(p.Anim), Position(p.Position), Speed(p.Speed),
	fx(p.fx), Born(p.Born), Life(p.Life) {}

Particle::~Particle()
{
	if (collider != nullptr)
		collider->to_delete = true;
}

bool Particle::Update() {

	bool ret = true;

	if (Life > 0) {

		if (((int)SDL_GetTicks() - (int)Born) > (int)Life)
			return false;
	}
	else
		if (Anim.Finished())
			ret = false;

	if (collider != nullptr)
		ret = true;

	if (SDL_GetTicks() >= Born) {

		Position.x += Speed.x;
		Position.y += Speed.y;

		if (collider != nullptr)
 			collider->SetPos(Position.x, Position.y);
	}

	return ret;

}