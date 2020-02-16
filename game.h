#pragma once

#include "physics.h"
#include "render.h"
#include "sprite.h"
#include "world.h"
#include "terrain.h"

#include "inventory.h"


struct Character
{
	// recolor?
	Sprite* sprite;

	int anim;
	int frame;
	float pos[3];
	float dir;
};

struct TalkBox;

struct SpriteReq
{
	int mount;
	int action;
	int armor;
	int helmet;
	int shield;
	int weapon;
};

struct Human : Character
{
	char name[16];
	char desc[16];

	int level;

	int max_xp; // to next level = C1 * pow(C2,C3*level)
	int cur_xp; // 0..max_xp-1

	int pr; // 0-transparent <0-evil >0-good ...

	int max_hp;
	int cur_hp;

	int max_mp;
	int cur_mp;

	int max_speed;
	int cur_speed;

	int max_power;
	int cur_power;

	// 0-6
	int prot_hit;
	int prot_fire;
	int nutr_vits; 
	int nutr_mins;
	int nutr_prots;
	int nutr_fats;
	int nutr_carbs;
	int nutr_water;

	// -------------

	uint64_t action_stamp;

	bool SetActionNone(uint64_t stamp);
	bool SetActionAttack(uint64_t stamp);
	bool SetActionFall(uint64_t stamp);
	bool SetActionStand(uint64_t stamp);
	bool SetActionDead(uint64_t stamp);

	bool SetWeapon(int w);
	bool SetShield(int s);
	bool SetHelmet(int h);
	bool SetArmor(int a);
	bool SetMount(int m);

	TalkBox* talk_box;

	SpriteReq req;
};

struct Game
{
	// terrain & world are global
	// World* world;
	// Terrain* terrain;

	uint64_t stamp;

	int font_size[2];
	int render_size[2];

	//bool player_hit; // helper for detecting clicks on the player sprite
	bool show_keyb; // activated together with talk_box by clicking on character
	int keyb_hide;  // show / hide animator (vertical position)

	bool show_buts; // true only if no popup is visible
	int bars_pos; // used to hide buts (0..7)

	// time relaxated KEY_UP/DOWN emulation by KEY_PRESSes
	uint64_t PressStamp;
	int PressKey;

	int TalkBox_blink;
	int KeybAutoRepCap;
	char KeybAutoRepChar;
	uint64_t KeybAuroRepDelayStamp;

	int keyb_pos[2];
	int keyb_mul;
	uint8_t keyb_key[32]; // simulated key presses by touch/mouse

	int water;
	float prev_yaw;

	Renderer* renderer;
	Physics* physics;

	Human player;
	Inventory inventory;

	int npcs;
	Character* npc;

	void StartContact(int id, int x, int y, int b);
	void MoveContact(int id, int x, int y);
	void EndContact(int id, int x, int y);

	int GetContact(int id);

	// accumulated input state
	struct Input 
	{
		uint8_t key[32]; // keyb state

		// we split touch input to multiple separate mice with left button only
		struct Contact
		{
			enum
			{
				NONE,
				KEYBCAP,
				PLAYER,
				TORQUE, // can be abs (right mouse but) or (timer touch on margin)
				FORCE
			};

			int action;

			int drag;    // which button initiated drag and is still down since then OR ZERO if there is no contact!!!
			int pos[2];  // mouse pos
			int drag_from[2]; // where drag has started

			int keyb_cap; // if touch starts at some cap
			bool player_hit; // if touch started at player/talkbox
			int margin; // -1: if touch started at left margin, +1 : if touch started at right margin, 0 otherwise
			float start_yaw; // absolute by mouse
		};

		Contact contact[4]; // 0:mouse, 1:primary_touch 2:secondary_touch 3:unused

		uint8_t but; // real mouse buttons currently down
		int wheel;   // relative mouse wheel (only from real mouse)
		int size[2]; // window size (in pixels)
		bool jump;
		bool shot;

		bool IsKeyDown(int k)
		{
			return (key[k >> 3] & (1 << (k & 7))) != 0;
		}

		void ClearKey(int k)
		{
			key[k >> 3] &= ~(1 << (k & 7));
		}
	};

	Input input;

	enum GAME_KEYB
	{
		KEYB_DOWN,
		KEYB_UP,
		KEYB_CHAR,
		KEYB_PRESS, // non-char terminal input with modifiers
	};

	enum GAME_MOUSE
	{
		MOUSE_MOVE,
		MOUSE_LEFT_BUT_DOWN,
		MOUSE_LEFT_BUT_UP,
		MOUSE_RIGHT_BUT_DOWN,
		MOUSE_RIGHT_BUT_UP,
		MOUSE_MIDDLE_BUT_DOWN,
		MOUSE_MIDDLE_BUT_UP,
		MOUSE_WHEEL_DOWN,
		MOUSE_WHEEL_UP
	};

	enum GAME_TOUCH
	{
		TOUCH_MOVE,
		TOUCH_BEGIN,
		TOUCH_END,
		TOUCH_CANCEL
	};

	// just accumulates input
	void OnKeyb(GAME_KEYB keyb, int key);
	void OnMouse(GAME_MOUSE mouse, int x, int y);
	void OnTouch(GAME_TOUCH touch, int id, int x, int y);
	void OnFocus(bool set);
	void OnSize(int w, int h, int fw, int fh);

	// update physics with accumulated input then render state to output
	void Render(uint64_t _stamp, AnsiCell* ptr, int width, int height);
	void ScreenToCell(int p[2]) const;
};

Game* CreateGame(int water, float pos[3], float yaw, float dir, uint64_t stamp);
void DeleteGame(Game* g);

void LoadSprites();
void FreeSprites();