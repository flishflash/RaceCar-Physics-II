#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"

 VehicleInfo car;

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
	car;
	// Car properties ----------------------------------------
	car.chassis_size.Set(2, 1, 6);
	car.chassis_offset.Set(0, 1, 0);
	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.0f;
	float wheel_radius = 1.2f;
	float wheel_width = 1.0f;
	float suspensionRestLength = 0.5f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width * (wheel_width*2), connection_height, half_length);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = true;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width * (wheel_width*2), connection_height, half_length);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = true;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width * (wheel_width*2), connection_height, -half_length);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width * (wheel_width*2), connection_height, -half_length);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 12, 10);
	
	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = 0.0f;
	brake = 10.0f;

	if (!win && !lose)
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			acceleration = MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			if (turn < TURN_DEGREES)
				turn += TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			if (turn > -TURN_DEGREES)
				turn -= TURN_DEGREES;
		}

		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			acceleration = -MAX_ACCELERATION;
		}

		if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT)
		{
			brake = BRAKE_POWER;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT && App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		{
			acceleration = MAX_ACCELERATION * 3;
		}

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			vehicle->Reset(0);
		}

		if (App->input->GetKey(SDL_SCANCODE_M) == KEY_REPEAT)
		{
			if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_DOWN) car.mass += 10.0f;
			if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_DOWN) car.mass -= 10.0f;
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN && vehicle->vehicle->getWheelInfo(0).m_raycastInfo.m_groundObject != NULL)
		{
			vehicle->Push(0.0f, 10000.0f, 0.0f);
		}

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && forces == true)
		{
			car.suspensionStiffness = 0.0f;
			car.suspensionCompression = 0.0f;
			car.suspensionDamping = 0.0f;
			car.maxSuspensionTravelCm = 0.0f;
			car.frictionSlip = 0.0f;
			car.maxSuspensionForce = 0.0f;
			forces = false;
		}
		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN && forces == false)
		{
			car.suspensionStiffness = 15.88f;
			car.suspensionCompression = 0.83f;
			car.suspensionDamping = 0.88f;
			car.maxSuspensionTravelCm = 1000.0f;
			car.frictionSlip = 50.5;
			car.maxSuspensionForce = 6000.0f;
			forces = true;
		}

		vehicle->ApplyEngineForce(acceleration);
		vehicle->Turn(turn);
		vehicle->Brake(brake);

		vehicle->Render();
	}
	char title[80];
	int segundos = (App->tiempo/1000) - ((timesup-120));

	if (segundos >= timesup || lose==true)
	{
		sprintf_s(title, "TimesUp!! YOU LOSE!!!");
		App->window->SetTitle(title);
		lose = true;
	}
	else if (win)
	{
		sprintf_s(title, "YOU WIN!!", segundos);
		App->window->SetTitle(title);
	}
	else
	{
		sprintf_s(title, "Liga Roquete || Time: %d s", segundos);
		App->window->SetTitle(title);
	}

	if(lose)
	{
		App->physics->debug = true;
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			lose = false; segundos = timesup;	timesup += 120;
		}
	}
	if (win)
	{
		App->physics->debug = true;
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN)
		{
			win = false; segundos = 0;	timesup += 120;
			vehicle->SetPos(0, 5, 0);
			vehicle->Reset(0);
		}
		App->scene_intro->Ball->SetPos(0, 0, 30);
	}

	return UPDATE_CONTINUE;
}
void ModulePlayer::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
}



