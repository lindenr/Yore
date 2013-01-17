/* graphics.c */
/* Map size: 100 glyphs down, 300 across */

#include "include/graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"

Uint32 bg_colour;

SDL_Surface *screen = NULL, *tiles = NULL, *glyph_col = NULL;
glyph gr_map[MAP_HEIGHT*MAP_WIDTH] = {0,};
glyph *gr_txt;

int curs_yloc = 0, curs_xloc = 0;
int cam_yloc = 0, cam_xloc = 0;
int glnumy, glnumx;
char gr_change[MAP_TILES] = {0,};
char *txt_change;
int mode = TMODE;

void gr_mode (int m)
{
	mode = m;

	if (m == GMODE && ((m&NOREF) == 0))
		gr_frefresh();
}

int to_buffer (int yloc, int xloc)
{
	if (mode == TMODE) return glnumx*yloc + xloc;
	return MAP_WIDTH*yloc + xloc;
}

int map_buffer (int yloc, int xloc)
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
	/* You can't move the camera in text-mode */
	if (mode == TMODE) return;

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

void gr_centcam (int yloc, int xloc)
{
	gr_movecam (yloc - glnumy/2, xloc - glnumx/2);
}

void gr_baddch (int buf, glyph gl)
{
	glyph *map;
	char *change;

	if (mode == TMODE)
	{
		map = gr_txt;
		change = txt_change;
	}
	else
	{
		map = gr_map;
		change = gr_change;
	}

	if (map[buf] == gl) return;
	if (gl < 256) gl |= COL_TXT_DEF;
	map[buf] = gl;
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

void gr_vprintc (const char *str, va_list args)
{
	char buffer[1000];
	uint32_t i;
	uint32_t length;
	uint32_t buf = to_buffer (curs_yloc, curs_xloc);

	vsprintf (buffer, str, args);
	length = strlen(buffer);
	if (length + buf >= MAP_TILES)
		length = MAP_TILES - buf;
	for (i = 0; i < length; ++i)
	{
		gr_baddch (buf+i, buffer[i]);
	}
}

void gr_printc (const char *str, ...)
{
	va_list args;

	va_start (args, str);
	gr_vprintc (str, args);
	va_end (args);
}

void gr_mvprintc (int y, int x, const char *str, ...)
{
	va_list args;

	gr_move (y, x);
	va_start (args, str);
	gr_vprintc (str, args);
	va_end (args);
}

void gr_setline (int line, glyph gl)
{
	/* You can't set whole lines outside of text mode */
	if (mode == GMODE) return;

	int i;
	for (i = 0; i < glnumx; ++ i)
		gr_mvaddch (line, i, gl);
}

#define GL_TRD ((gl&0xF0000000)>>24)
#define GL_TGN ((gl&0x0F000000)>>20)
#define GL_TBL ((gl&0x00F00000)>>16)
#define GL_BRD ((gl&0x000F0000)>>12)
#define GL_BGN ((gl&0x0000F000)>>8)
#define GL_BBL ((gl&0x00000F00)>>4)
inline void blit_glyph (glyph gl, int yloc, int xloc)
{
	char ch = (char)gl;
	SDL_Rect srcrect = {GLW*(ch&15), GLH*((ch>>4)&15), GLW, GLH};
	SDL_Rect dstrect = {GLW*xloc, GLH*yloc, GLW, GLH};

	SDL_FillRect (glyph_col, NULL, SDL_MapRGB (glyph_col->format, GL_BRD, GL_BGN, GL_BBL));
	SDL_BlitSurface (tiles, &srcrect, glyph_col, NULL);
	
	SDL_FillRect (screen, &dstrect, SDL_MapRGB (screen->format, GL_TRD, GL_TGN, GL_TBL));
	SDL_BlitSurface (glyph_col, NULL, screen, &dstrect);
}

void gr_trefresh ()
{
	int w, changed_total, cur_rect;
	SDL_Rect rects[100];

	changed_total = 0;
	for (w = 0; w < glnumx*glnumy; ++ w)
		if (txt_change[w])
			++ changed_total;

	if (!changed_total) return; /* Nothing to do. */

	cur_rect = 0;

	for (w = 0; w < glnumx*glnumy; ++ w)
	{
		if (txt_change[w])
		{
			blit_glyph (gr_txt[w], w/glnumx, w%glnumx);
			if (changed_total < 100)
			{
				rects[cur_rect].x = GLW*(w%glnumx);
				rects[cur_rect].y = GLH*(w/glnumx);
				rects[cur_rect].w = GLW;
				rects[cur_rect].h = GLH;
				++ cur_rect;
			}
		}
		txt_change[w] = 0;
	}
	if (changed_total < 100)
		SDL_UpdateRects (screen, changed_total, rects);
	else
		SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_refresh ()
{
	if (mode == TMODE)
	{
		gr_trefresh ();
		return;
	}
	int x, y, changed_total, cur_rect;
	SDL_Rect rects[100];

	changed_total = 0;
	for (x = 0; x < glnumx; ++ x)
	{
		for (y = 0; y < glnumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;

			if (gr_change[to_buffer(gly, glx)])
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
			if (gr_change[to_buffer(gly, glx)])
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
			gr_change[to_buffer(gly, glx)] = 0;
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
			gr_change[to_buffer(gly, glx)] = 0;
		}
	}
	SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_mvforce (int yloc, int xloc)
{
	int w = to_buffer (yloc, xloc);
	if (mode == TMODE)
		txt_change[w] = 1;
	else
		gr_change[w] = 1;
}

void gr_tclear ()
{
	int i;
	for (i = 0; i < glnumy*glnumx; ++ i)
		gr_baddch (i, ' ');
	gr_refresh ();
}

void gr_clear ()
{
	if (mode == TMODE)
	{
		gr_tclear ();
		return;
	}
	int i;
	for (i = 0; i < MAP_TILES; ++ i)
		gr_baddch (i, ' ');
	gr_refresh ();
}

int tout_num = 0;
void gr_tout (int t)
{
	tout_num = t;
}

int echoing = 1;

int gr_equiv (uint32_t key1, uint32_t key2)
{
	return 1;
}

uint32_t gr_getfullch ()
{
	SDL_Event event;
	clock_t end = 0;
	uint32_t modifier_keys = (KMOD_SHIFT | KMOD_CAPS | KMOD_NUM) << 16;
	if (tout_num)
		end = tout_num * CLOCKS_PER_SEC / 1000 + clock();

	while (1)
	{
		if (end && clock() >= end) break;
		if (!SDL_PollEvent (&event))
		{
			gr_wait (20);
			continue;
		}

		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				uint32_t mod = event.key.keysym.mod << 16;
				if (mod & (~modifier_keys) && (event.key.keysym.unicode != 0))
				{
					//printf ("%d %d\n", event.key.keysym.sym, event.key.keysym.unicode);
					return mod|event.key.keysym.sym;
				}
				if (!echoing)
				{
					if (event.key.keysym.sym == SDLK_UP)
						return mod|GRK_UP;
					if (event.key.keysym.sym == SDLK_DOWN)
						return mod|GRK_DN;
					if (event.key.keysym.sym == SDLK_LEFT)
						return mod|GRK_LF;
					if (event.key.keysym.sym == SDLK_RIGHT)
						return mod|GRK_RT;
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
				}

				return mod|event.key.keysym.unicode;
			}
			
			case SDL_QUIT:
				exit(0);
			
			default:
				break;
		}
	}
	return EOF;
}

char gr_getch ()
{
	return (char)(gr_getfullch () & 0xFF);
}

void gr_getstr (char *out, int len)
{
	int i = 0;
	do
	{
		char in = gr_getch ();
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
			gr_addch (' ');
			gr_refresh ();
			continue;
		}
		if (in == 0 || i >= len) /* Modifier (eg shift) or too long */
		{
			if (curs_xloc == 0)
			{
				curs_xloc = MAP_WIDTH-1;
				-- curs_yloc;
			}
			else
				-- curs_xloc;
			gr_addch (' ');
			gr_refresh ();
			continue;
		}
		gr_refresh();
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

#define GR_TRY_TILES(a,b) sprintf(filepath, a, b); temp = SDL_LoadBMP (filepath); if (temp) goto success; printf("Couldn't find tiles at %s.\n", filepath)
void gr_load_tiles ()
{
	char filepath[100] = "";
	void *temp = NULL;

	GR_TRY_TILES("resources/%s", TILE_FILE);
	GR_TRY_TILES("../resources/%s", TILE_FILE);
	GR_TRY_TILES("%s", TILE_FILE);

	fprintf (stderr, "Error loading tileset: %s\n", SDL_GetError ());
	exit (1);
	
  success:
	tiles = SDL_DisplayFormat (temp);
	SDL_FreeSurface(temp);
}

void gr_cleanup ()
{
	SDL_FreeSurface (screen);
	SDL_Quit ();
}

void gr_init ()
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error initialising SDL: %s\n", SDL_GetError ());
		exit (1);
	}
	
	atexit (gr_cleanup);
	
	screen = SDL_SetVideoMode (640, 480, 32, SDL_SWSURFACE);
	if (screen == NULL)
	{
		fprintf (stderr, "Error initialising video mode: %s\n", SDL_GetError ());
		exit (1);
	}
	
	gr_load_tiles ();
	SDL_SetColorKey (tiles, SDL_SRCCOLORKEY, SDL_MapRGB (tiles->format, 255, 0, 255));
	
	void *tmp = SDL_CreateRGBSurface (SDL_SWSURFACE, GLW, GLH, 32, rmask, gmask, bmask, amask);
	glyph_col = SDL_DisplayFormat (tmp);
	SDL_FreeSurface (tmp);
	SDL_SetColorKey (glyph_col, SDL_SRCCOLORKEY, SDL_MapRGB (glyph_col->format, 255, 255, 255));
	
	glnumy = screen->h / GLH;
	glnumx = screen->w / GLW - 5;
	
	gr_txt = malloc (sizeof(glyph) * glnumy * glnumx);
	memset (gr_txt, 0, sizeof(glyph) * glnumy * glnumx);
	txt_change = malloc (glnumy * glnumx);
	memset (txt_change, 0, glnumy * glnumx);

	/* Finish housekeeping */
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (200, 30);
	SDL_WM_SetCaption ("Yore", "Yore");
}

void gr_wait (int ms)
{
	SDL_Delay (ms);
}

#define GR_NEAR 5
int gr_nearedge (int yloc, int xloc)
{
	yloc -= cam_yloc; xloc -= cam_xloc;
	return (yloc <= GR_NEAR || yloc >= (glnumy-GR_NEAR) ||
	        xloc <= GR_NEAR || xloc >= (glnumx-GR_NEAR));
}
