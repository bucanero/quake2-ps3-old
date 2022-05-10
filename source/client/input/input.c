/*
 * Copyright (C) 2010 Yamagi Burmeister
 * Copyright (C) 1997-2005 Id Software, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * =======================================================================
 *
 * This is the Quake II's input system backend for PS3's dualshock 3.
 *
 * =======================================================================
 */

#include "header/input.h"
#include "../../client/header/client.h"

#include <io/pad.h>

// ----

// Maximal mouse move per frame
#define MOUSE_MAX 3000

// Minimal mouse move per frame
#define MOUSE_MIN 40

// ----

// These are used to communicate the events collected by
// IN_Update() called at the beginning of a frame to the
// actual movement functions called at a later time.
static float mouse_x, mouse_y;
static int back_button_id = -1;
// static int sdl_back_button = SDL_CONTROLLER_BUTTON_BACK;
static float joystick_yaw, joystick_pitch;
static float joystick_forwardmove, joystick_sidemove;
static float joystick_up;
static qboolean mlooking;

// The last time input events were processed.
// Used throughout the client.
int sys_frame_time;

// the joystick altselector that turns K_JOYX into K_JOYX_ALT
// is pressed
qboolean joy_altselector_pressed = false;

// Console Variables
cvar_t *freelook;
cvar_t *lookstrafe;
cvar_t *m_forward;
cvar_t *m_pitch;
cvar_t *m_side;
cvar_t *m_up;
cvar_t *m_yaw;
cvar_t *sensitivity;

static cvar_t *exponential_speedup;
static cvar_t *in_grab;
static cvar_t *m_filter;
static cvar_t *windowed_mouse;

// ----


qboolean show_haptic;

// Joystick sensitivity
cvar_t *joy_yawsensitivity;
cvar_t *joy_pitchsensitivity;
cvar_t *joy_forwardsensitivity;
cvar_t *joy_sidesensitivity;
cvar_t *joy_upsensitivity;
cvar_t *joy_expo;

// Joystick haptic
static cvar_t *joy_haptic_magnitude;

void IN_Init(void)
{
	ioPadInit(7);
	cvar_t *in_sdlbackbutton;
	int nummappings;
	char controllerdb[MAX_OSPATH] = {0};
	Com_Printf("------- input initialization -------\n");

	mouse_x = mouse_y = 0;
	joystick_yaw = joystick_pitch = joystick_forwardmove = joystick_sidemove = 0;

	exponential_speedup = Cvar_Get("exponential_speedup", "0", CVAR_ARCHIVE);
	freelook = Cvar_Get("freelook", "1", CVAR_ARCHIVE);
	in_grab = Cvar_Get("in_grab", "2", CVAR_ARCHIVE);
	lookstrafe = Cvar_Get("lookstrafe", "0", CVAR_ARCHIVE);
	m_filter = Cvar_Get("m_filter", "0", CVAR_ARCHIVE);
	m_up = Cvar_Get("m_up", "1", CVAR_ARCHIVE);
	m_forward = Cvar_Get("m_forward", "1", CVAR_ARCHIVE);
	m_pitch = Cvar_Get("m_pitch", "0.022", CVAR_ARCHIVE);
	m_side = Cvar_Get("m_side", "0.8", CVAR_ARCHIVE);
	m_yaw = Cvar_Get("m_yaw", "0.022", CVAR_ARCHIVE);
	sensitivity = Cvar_Get("sensitivity", "3", CVAR_ARCHIVE);

	joy_haptic_magnitude = Cvar_Get("joy_haptic_magnitude", "0.0", CVAR_ARCHIVE);

	joy_yawsensitivity = Cvar_Get("joy_yawsensitivity", "1.0", CVAR_ARCHIVE);
	joy_pitchsensitivity = Cvar_Get("joy_pitchsensitivity", "1.0", CVAR_ARCHIVE);
	joy_forwardsensitivity = Cvar_Get("joy_forwardsensitivity", "1.0", CVAR_ARCHIVE);
	joy_sidesensitivity = Cvar_Get("joy_sidesensitivity", "1.0", CVAR_ARCHIVE);
	joy_upsensitivity = Cvar_Get("joy_upsensitivity", "1.0", CVAR_ARCHIVE);
	joy_expo = Cvar_Get("joy_expo", "2.0", CVAR_ARCHIVE);

	// joy_axis_leftx = Cvar_Get("joy_axis_leftx", "sidemove", CVAR_ARCHIVE);
	// joy_axis_lefty = Cvar_Get("joy_axis_lefty", "forwardmove", CVAR_ARCHIVE);
	// joy_axis_rightx = Cvar_Get("joy_axis_rightx", "yaw", CVAR_ARCHIVE);
	// joy_axis_righty = Cvar_Get("joy_axis_righty", "pitch", CVAR_ARCHIVE);
	// joy_axis_triggerleft = Cvar_Get("joy_axis_triggerleft", "triggerleft", CVAR_ARCHIVE);
	// joy_axis_triggerright = Cvar_Get("joy_axis_triggerright", "triggerright", CVAR_ARCHIVE);

	// joy_axis_leftx_threshold = Cvar_Get("joy_axis_leftx_threshold", "0.15", CVAR_ARCHIVE);
	// joy_axis_lefty_threshold = Cvar_Get("joy_axis_lefty_threshold", "0.15", CVAR_ARCHIVE);
	// joy_axis_rightx_threshold = Cvar_Get("joy_axis_rightx_threshold", "0.15", CVAR_ARCHIVE);
	// joy_axis_righty_threshold = Cvar_Get("joy_axis_righty_threshold", "0.15", CVAR_ARCHIVE);
	// joy_axis_triggerleft_threshold = Cvar_Get("joy_axis_triggerleft_threshold", "0.15", CVAR_ARCHIVE);
	// joy_axis_triggerright_threshold = Cvar_Get("joy_axis_triggerright_threshold", "0.15", CVAR_ARCHIVE);

	windowed_mouse = Cvar_Get("windowed_mouse", "1", CVAR_USERINFO | CVAR_ARCHIVE);

	#if 0
	in_sdlbackbutton = Cvar_Get("in_sdlbackbutton", "0", CVAR_ARCHIVE);
	if (in_sdlbackbutton)
	{
		switch ((int)in_sdlbackbutton->value)
		{
			case 1:
				sdl_back_button = SDL_CONTROLLER_BUTTON_START;
				break;
			case 2:
				sdl_back_button = SDL_CONTROLLER_BUTTON_GUIDE;
				break;
			default:
				sdl_back_button = SDL_CONTROLLER_BUTTON_BACK;
		}
	}

	Cmd_AddCommand("+mlook", IN_MLookDown);
	Cmd_AddCommand("-mlook", IN_MLookUp);

	Cmd_AddCommand("+joyaltselector", IN_JoyAltSelectorDown);
	Cmd_AddCommand("-joyaltselector", IN_JoyAltSelectorUp);

	SDL_StartTextInput();

	/* Joystick init */
	if (!SDL_WasInit(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) && false)
	{
		if (SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC) == -1)
		{
			Com_Printf ("Couldn't init SDL joystick: %s.\n", SDL_GetError ());
		}
		else
		{
			Com_Printf ("%i joysticks were found.\n", SDL_NumJoysticks());

			if (SDL_NumJoysticks() > 0)
			{
				for (const char* rawPath = FS_GetNextRawPath(NULL); rawPath != NULL; rawPath = FS_GetNextRawPath(rawPath))
				{
					snprintf(controllerdb, MAX_OSPATH, "%s/gamecontrollerdb.txt", rawPath);
					nummappings = SDL_GameControllerAddMappingsFromFile(controllerdb);
					if (nummappings > 0)
						Com_Printf ("%d mappings loaded from gamecontrollerdb.txt\n", nummappings);
				}

				for (int i = 0; i < SDL_NumJoysticks(); i++) {
					joystick = SDL_JoystickOpen(i);

					Com_Printf ("The name of the joystick is '%s'\n", SDL_JoystickName(joystick));
					Com_Printf ("Number of Axes: %d\n", SDL_JoystickNumAxes(joystick));
					Com_Printf ("Number of Buttons: %d\n", SDL_JoystickNumButtons(joystick));
					Com_Printf ("Number of Balls: %d\n", SDL_JoystickNumBalls(joystick));
					Com_Printf ("Number of Hats: %d\n", SDL_JoystickNumHats(joystick));

					joystick_haptic = SDL_HapticOpenFromJoystick(joystick);

					if (joystick_haptic == NULL)
					{
						Com_Printf("Most likely joystick isn't haptic.\n");
					}
					else
					{
						IN_Haptic_Effects_Info();
					}

					if(SDL_IsGameController(i))
					{
						SDL_GameControllerButtonBind backBind;
						controller = SDL_GameControllerOpen(i);

						Com_Printf ("Controller settings: %s\n", SDL_GameControllerMapping(controller));
						Com_Printf ("Controller axis: \n");
						Com_Printf (" * leftx = %s\n", joy_axis_leftx->string);
						Com_Printf (" * lefty = %s\n", joy_axis_lefty->string);
						Com_Printf (" * rightx = %s\n", joy_axis_rightx->string);
						Com_Printf (" * righty = %s\n", joy_axis_righty->string);
						Com_Printf (" * triggerleft = %s\n", joy_axis_triggerleft->string);
						Com_Printf (" * triggerright = %s\n", joy_axis_triggerright->string);

						Com_Printf ("Controller thresholds: \n");
						Com_Printf (" * leftx = %f\n", joy_axis_leftx_threshold->value);
						Com_Printf (" * lefty = %f\n", joy_axis_lefty_threshold->value);
						Com_Printf (" * rightx = %f\n", joy_axis_rightx_threshold->value);
						Com_Printf (" * righty = %f\n", joy_axis_righty_threshold->value);
						Com_Printf (" * triggerleft = %f\n", joy_axis_triggerleft_threshold->value);
						Com_Printf (" * triggerright = %f\n", joy_axis_triggerright_threshold->value);

						backBind = SDL_GameControllerGetBindForButton(controller, sdl_back_button);

						if (backBind.bindType == SDL_CONTROLLER_BINDTYPE_BUTTON)
						{
							back_button_id = backBind.value.button;
							Com_Printf ("\nBack button JOY%d will be unbindable.\n", back_button_id+1);
						}

						break;
					}
					else
					{
						char joystick_guid[256] = {0};

						SDL_JoystickGUID guid;
						guid = SDL_JoystickGetDeviceGUID(i);

						SDL_JoystickGetGUIDString(guid, joystick_guid, 255);

						Com_Printf ("To use joystick as game controller please set SDL_GAMECONTROLLERCONFIG:\n");
						Com_Printf ("e.g.: SDL_GAMECONTROLLERCONFIG='%s,%s,leftx:a0,lefty:a1,rightx:a2,righty:a3,back:b1,...\n", joystick_guid, SDL_JoystickName(joystick));
					}
				}
			}
			else
			{
				joystick_haptic = SDL_HapticOpenFromMouse();

				if (joystick_haptic == NULL)
				{
					Com_Printf("Most likely mouse isn't haptic.\n");
				}
				else
				{
					IN_Haptic_Effects_Info();
				}
			}
		}
	}
	#endif
	Com_Printf("------------------------------------\n\n");
}

void IN_Move(usercmd_t *cmd)
{
}

void IN_Shutdown(void)
{
}

#define SIMPLE_MAP(PAD_KEY, QKEY) \
if (last_paddata[i].PAD_KEY != paddata.PAD_KEY)\
{\
	Key_Event(QKEY, paddata.PAD_KEY, true);\
}

void IN_Update(void)
{
	padInfo padinfo;
	padData paddata;

	static padData last_paddata[MAX_PADS];

	ioPadGetInfo(&padinfo);
	for(int i = 0; i < MAX_PADS; ++i)
	{
		if(padinfo.status[i])
		{
			ioPadGetData(i, &paddata);

			// padData diff = IN_comparePadData(last_paddata[i], paddata);

			SIMPLE_MAP(BTN_START, K_ESCAPE)
			SIMPLE_MAP(BTN_CROSS, K_ENTER)

			SIMPLE_MAP(BTN_UP, K_UPARROW)
			SIMPLE_MAP(BTN_DOWN, K_DOWNARROW)
			SIMPLE_MAP(BTN_LEFT, K_LEFTARROW)
			SIMPLE_MAP(BTN_RIGHT, K_RIGHTARROW)

			SIMPLE_MAP(BTN_R1, K_CTRL)

			// SIMPLE_MAP(BTN_TRIANGLE, K_SC_W)
			if (last_paddata[i].BTN_TRIANGLE != paddata.BTN_TRIANGLE)
			{
				Key_Event(K_MOUSE3, paddata.BTN_TRIANGLE, false);
			}

			//SIMPLE_MAP(BTN_R2, K_CTRL)
			
			last_paddata[i] = paddata;
		}
	}
}

void In_FlushQueue(void)
{
	Key_MarkAllUp();
}

void
Haptic_Feedback(char *name, int effect_volume, int effect_duration,
			   int effect_begin, int effect_end,
			   int effect_attack, int effect_fade,
			   int effect_x, int effect_y, int effect_z)
{
}