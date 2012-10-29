/* graphics.c */
/* Map size: 100 glyphs down, 300 across */

#include "include/SDL/SDL_image.h"
#include "include/graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "resources/t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"

Uint32 bg_colour;

SDL_Surface *screen = NULL, *tiles = NULL;
glyph gr_map[MAP_HEIGHT*MAP_WIDTH] = {0,};

int curs_yloc = 0, curs_xloc = 0;
int cam_yloc = 0, cam_xloc = 0;
int glnumy, glnumx;
char change[MAP_TILES] = {0,};

int to_buffer (int yloc, int xloc)
{
	return MAP_WIDTH*yloc + xloc;
}

void gr_move (int yloc, int xloc)
{
	curs_yloc = yloc;
	curs_xloc = xloc;
}

void gr_movecam (int yloc, int xloc)
{
	cam_yloc = yloc;
	cam_xloc = xloc;

	if (cam_yloc <= 0)
		cam_yloc = 0;

	if (cam_yloc >= MAP_HEIGHT-glnumy)
		cam_yloc = MAP_HEIGHT-glnumy;

	if (cam_xloc <= 0)
		cam_xloc = 0;

	if (cam_xloc >= MAP_WIDTH-glnumx)
		cam_xloc = MAP_WIDTH-glnumx;
	
	gr_frefresh ();
}

void gr_baddch (int buf, glyph gl)
{
	if (gr_map[buf] == gl) return;
	gr_map[buf] = gl;
	change[buf] = 1;
}

void gr_addch (glyph gl)
{
	gr_baddch (to_buffer(curs_yloc, curs_xloc), gl);
}

void gr_mvaddch (int yloc, int xloc, glyph gl)
{
	gr_baddch (to_buffer(yloc, xloc), gl);
}

void gr_vprintc(const char *str, va_list args)
{
	char buffer[1000];
	uint32_t i;
	uint32_t length;
	uint32_t buf = to_buffer(curs_yloc, curs_xloc);

	vsprintf(buffer, str, args);
	length = strlen(buffer);
	if (length + buf >= MAP_TILES)
		length = MAP_TILES - buf;
	for (i = 0; i < length; ++i)
	{
		gr_baddch (buf+i, buffer[i]);
	}
}

void gr_printc(const char *str, ...)
{
	va_list args;

	va_start(args, str);
	gr_vprintc(str, args);
	va_end(args);
}

void gr_mvprintc(int y, int x, const char *str, ...)
{
	va_list args;

	gr_move(y, x);
	va_start(args, str);
	gr_vprintc(str, args);
	va_end(args);
}

inline void blit_glyph (glyph gl, int yloc, int xloc)
{
	char ch = (char)gl;
	SDL_Rect srcrect = {GLW*(ch&15), GLH*((ch>>4)&15), GLW, GLH};
	SDL_Rect dstrect = {GLW*xloc, GLH*yloc, GLW, GLH};

	SDL_FillRect (screen, &dstrect, bg_colour);
	SDL_BlitSurface (tiles, &srcrect, screen, &dstrect);
}

void gr_refresh ()
{
	int x, y, changed_total, cur_rect;
	SDL_Rect rects[100];

	changed_total = 0;
	for (x = 0; x < glnumx; ++ x)
	{
		for (y = 0; y < glnumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;
			if (change[to_buffer(gly, glx)])
				++ changed_total;
		}
	}

	if (!changed_total) return; /* Nothing to do. */

	cur_rect = 0;

	for (x = 0; x < glnumx; ++ x)
	{
		for (y = 0; y < glnumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;
			if (change[to_buffer(gly, glx)])
			{
				blit_glyph (gr_map[to_buffer(gly, glx)], y, x);
				if (changed_total < 100)
				{
					rects[cur_rect].x = GLW*x;
					rects[cur_rect].y = GLH*y;
					rects[cur_rect].w = GLW;
					rects[cur_rect].h = GLH;
					++ cur_rect;
				}
			}
			change[to_buffer(gly, glx)] = 0;
		}
	}

	if (changed_total < 100)
		SDL_UpdateRects (screen, changed_total, rects);
	else
		SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_frefresh ()
{
	int x, y;

	for (x = 0; x < glnumx; ++ x)
	{
		for (y = 0; y < glnumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;
			blit_glyph (gr_map[to_buffer(gly, glx)], y, x);
			change[to_buffer(gly, glx)] = 0;
		}
	}
	SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_clear ()
{
	int i;
	for (i = 0; i < MAP_TILES; ++ i)
		gr_baddch (i, ' ');
	gr_refresh();
}

int echoing = 1;

char gr_getch ()
{
	SDL_Event event;
	
	while (1)
	{
		SDL_WaitEvent (&event);
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				if (!echoing)
				{
					if (event.key.keysym.sym == SDLK_UP)
						return GRK_UP;
					if (event.key.keysym.sym == SDLK_DOWN)
						return GRK_DN;
					if (event.key.keysym.sym == SDLK_LEFT)
						return GRK_LF;
					if (event.key.keysym.sym == SDLK_RIGHT)
						return GRK_RT;
				}
				if (event.key.keysym.unicode == 0)
					break;
				if (echoing)
				{
					if (event.key.keysym.unicode >= 0x20)
						gr_mvaddch (curs_yloc, curs_xloc, event.key.keysym.unicode);
					if (curs_xloc == MAP_WIDTH-1)
					{
						curs_xloc = 0;
						++ curs_yloc;
					}
					else
						++ curs_xloc;
					gr_refresh();
				}
				return event.key.keysym.unicode;
			}
			
			case SDL_QUIT:
				exit(0);
			
			default:
				break;
		}
	}
	panic ("gr_getch()");
	return 0;
}

void gr_getstr (char *out)
{
	int i = 0;
	do
	{
		char in = gr_getch ();
		if (in == 0) /* Modifier (eg shift) */
		{
			if (curs_xloc == 0)
			{
				curs_xloc = MAP_WIDTH-1;
				-- curs_yloc;
			}
			else
				-- curs_xloc;
			continue;
		}
		if (in == CH_LF || in == CH_CR) break;
		if (in == CH_BS)
		{
			if (curs_xloc == 0)
			{
				curs_xloc = MAP_WIDTH-1;
				-- curs_yloc;
			}
			else
				-- curs_xloc;
			if (i)
			{
				if (curs_xloc == 0)
				{
					curs_xloc = MAP_WIDTH-1;
					-- curs_yloc;
				}
				else
					-- curs_xloc;
			}
			if (i)
				-- i;
			out[i] = 0;
			gr_addch(' ');
			gr_refresh();
			continue;
		}
		out[i] = in;
		++ i;
	}
	while (1);
	out[i] = 0;
}

void gr_echo ()
{
	echoing = 1;
}

void gr_noecho ()
{
	echoing = 0;
}

Uint32
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xff000000,
	gmask = 0x00ff0000,
	bmask = 0x0000ff00,
	amask = 0x000000ff;
#else
	rmask = 0x000000ff,
	gmask = 0x0000ff00,
	bmask = 0x00ff0000,
	amask = 0xff000000;
#endif

#define GR_TRY_TILES(a,b) sprintf(filepath, a, b); tiles = SDL_LoadBMP (filepath); if (tiles) return;
void gr_load_tiles ()
{
	char filepath[100] = "";
	GR_TRY_TILES("resources/%s", TILE_FILE);
	GR_TRY_TILES("%s", TILE_FILE);
	GR_TRY_TILES("../%s", TILE_FILE);

	fprintf (stderr, "Error loading tileset: %s\n", SDL_GetError ());
	exit (1);
}

void gr_init ()
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error initialising SDL: %s\n", SDL_GetError ());
		exit (1);
	}
	
	atexit (SDL_Quit);
	
	screen = SDL_SetVideoMode (1280, 804, 32, SDL_SWSURFACE);
	if (screen == NULL)
	{
		fprintf (stderr, "Error initialising video mode: %s\n", SDL_GetError ());
		exit (1);
	}
	
	gr_load_tiles ();
	SDL_SetColorKey (tiles, SDL_SRCCOLORKEY, SDL_MapRGB (tiles->format, 255, 0, 255));
	
	glnumy = screen->h / GLH;
	glnumx = screen->w / GLW;

	/* Finish housekeeping */
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (200, 30);
}

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void wait_ms (unsigned ms)
{
	Sleep (ms);
}
#else
#include <unistd.h>

void wait_ms (unsigned ms)
{
	usleep (ms);
}
#endif
