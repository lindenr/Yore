/* graphics.c */
/* Map size: 100 glyphs down, 300 across */

#include "include/graphics.h"
#include "include/timer.h"
#include "include/panel.h"
#include "include/pixel.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"

uint32_t bg_colour;

SDL_Surface *screen = NULL, *tiles = NULL, *glyph_col = NULL;

/* starting size */
int screenY = 780, screenX = 1000;

glyph gr_map[MAP_HEIGHT*MAP_WIDTH] = {0,};
char gr_change[MAP_TILES] = {0,};

glyph *txt_map = NULL;
char *txt_change = NULL;

int curs_yloc = 0, curs_xloc = 0;
int cam_yloc = 0, cam_xloc = 0;
int snumy = 10, snumx = 1;
int pnumy = 0, pnumx = 0;

int gr_buffer (int yloc, int xloc)
{
	return MAP_WIDTH*yloc + xloc;
}

int txt_buffer (int yloc, int xloc)
{
	return snumx*yloc + xloc;
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

	if (cam_yloc >= MAP_HEIGHT-pnumy)
		cam_yloc = MAP_HEIGHT-pnumy;

	if (cam_xloc <= 0)
		cam_xloc = 0;

	if (cam_xloc >= MAP_WIDTH-pnumx)
		cam_xloc = MAP_WIDTH-pnumx;
	
	gr_frefresh ();
}

void gr_centcam (int yloc, int xloc)
{
	gr_movecam (yloc - pnumy/2, xloc - pnumx/2);
}

void gr_baddch (int buf, glyph gl)
{
	if (gr_map[buf] == gl) return;
	if (gl == (gl&255)) gl |= COL_TXT_DEF;
	gr_map[buf] = gl;
	gr_change[buf] = 1;
}

void txt_baddch (int buf, glyph gl)
{
	if (gl != 0 && gl == (gl&255)) gl |= COL_TXT_DEF;
	if (txt_map[buf] == gl) return;
	txt_map[buf] = gl;
	txt_change[buf] = 1;
}

void gr_addch (glyph gl)
{
	int buf = gr_buffer (curs_yloc, curs_xloc);
	gr_baddch (buf, gl);
}

void gr_mvaddch (int yloc, int xloc, glyph gl)
{
	int buf = gr_buffer (yloc, xloc);
	gr_baddch (buf, gl);
}

void txt_mvaddch (int yloc, int xloc, glyph gl)
{
	int buf = txt_buffer (yloc, xloc);
	txt_baddch (buf, gl);
}

void txt_mvaprint (int yloc, int xloc, const char *str)
{
	int i, buf = txt_buffer (yloc, xloc), len = strlen(str);

	for (i = 0; i < len && i+buf < TXT_TILES; ++ i)
		txt_baddch (i+buf, str[i]);
}

void txt_mvprint (int yloc, int xloc, const char *str, ...)
{
	va_list args;
	char out[1024];

	gr_move (yloc, xloc);
	va_start (args, str);
	vsprintf (out, str, args);
	va_end (args);

	txt_mvaprint (yloc, xloc, out);
}

void txt_box (int yloc, int xloc, int height, int width)
{
	int xt = width - 1, yt = height - 1;
	txt_mvaddch (yloc, xloc, ACS_ULCORNER);
	txt_mvaddch (yloc + height, xloc, ACS_LLCORNER);
	txt_mvaddch (yloc, xloc + width, ACS_URCORNER);
	txt_mvaddch (yloc + height, xloc + width, ACS_LRCORNER);
	while (xt--)
	{
		txt_mvaddch (yloc, xloc + xt + 1, ACS_HLINE);
		txt_mvaddch (yloc + height, xloc + xt + 1, ACS_HLINE);
	}
	while (yt--)
	{
		txt_mvaddch (yloc + yt + 1, xloc, ACS_VLINE);
		txt_mvaddch (yloc + yt + 1, xloc + width, ACS_VLINE);
	}
}

void txt_fbox (int yloc, int xloc, int height, int width, glyph fill)
{
	int x, y;
	for (x = 1; x < width; ++x)
		for (y = 1; y < height; ++y)
			txt_mvaddch (yloc + y, xloc + x, fill);
	txt_box (yloc, xloc, height, width);
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

#define CURCHANGED \
if (changed_total < 100) \
{\
rects[cur_rect].x = GLW*x;\
rects[cur_rect].y = GLH*y;\
rects[cur_rect].w = GLW;\
rects[cur_rect].h = GLH;\
++ cur_rect;\
}

void gr_refresh ()
{
	int x, y, changed_total, cur_rect;
	SDL_Rect rects[100];

	changed_total = 0;
	for (x = 0; x < snumx; ++ x)
	{
		for (y = 0; y < snumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;
			int txtbuf = txt_buffer (y, x), grbuf = gr_buffer (gly, glx);

			if (txt_change[txtbuf] || (gr_change[grbuf] && (!txt_map[txtbuf])))
				++ changed_total;
		}
	}

	if (!changed_total) return; /* Nothing to do. */

	cur_rect = 0;

	for (x = 0; x < snumx; ++ x)
	{
		for (y = 0; y < snumy; ++ y)
		{
			int gly = y + cam_yloc, glx = x + cam_xloc;
			int txtbuf = txt_buffer (y, x), grbuf = gr_buffer (gly, glx);
			if (txt_change[txtbuf] && txt_map[txtbuf])
			{
				blit_glyph (txt_map[txtbuf], y, x);
				CURCHANGED;
				txt_change[txtbuf] = 0;
			}
			if (txt_map[txtbuf])
				continue;
			if (gr_change[grbuf] || txt_change[txtbuf])
			{
				blit_glyph (gr_map[grbuf], y, x);
				CURCHANGED;
				gr_change[grbuf] = 0;
			}
		}
	}

	px_showboxes ();

	if (changed_total < 100)
		SDL_UpdateRects (screen, changed_total, rects);
	else
		SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_frefresh ()
{
	int x, y;

	for (x = 0; x < snumx; ++ x)
	{
		for (y = 0; y < snumy; ++ y)
		{
			int txtbuf = txt_buffer (y, x);
			if (txt_map[txtbuf])
			{
				blit_glyph (txt_map[txtbuf], y, x);
				txt_change[txtbuf] = 0;
				continue;
			}
			int gly = y + cam_yloc, glx = x + cam_xloc;
			int grbuf = gr_buffer (gly, glx);
			blit_glyph (gr_map[grbuf], y, x);
			gr_change[grbuf] = 0;
		}
	}
	px_showboxes ();
	SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_clear ()
{
	int i;
	px_showboxes ();
	for (i = 0; i < MAP_TILES; ++ i)
		gr_baddch (i, ' ');
	for (i = 0; i < TXT_TILES; ++ i)
		txt_baddch (i, 0);
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
	uint32_t mod1 = key1 >> 16, mod2 = key2 >> 16;
	//printf ("%x %x\n", mod1, mod2);

	/* control */
	if (((mod1 & KMOD_CTRL) != 0) != ((mod2 & KMOD_CTRL) != 0))
		return 0;

	return 1;
}

uint32_t gr_getfullch ()
{
	gr_refresh ();

	SDL_Event event;
	uint32_t end = 0;
	uint32_t modifier_keys = (KMOD_SHIFT | KMOD_CAPS | KMOD_NUM) << 16;
	if (tout_num)
		end = tout_num + SDL_GetTicks ();

	while (1)
	{
		if ((end) && (SDL_GetTicks () >= end)) break;
		if (!SDL_PollEvent (&event))
		{
			t_idle ();
			gr_wait (20);
			continue;
		}

		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				uint32_t mod = event.key.keysym.mod << 16;
				if ((mod & (~modifier_keys)) && (event.key.keysym.unicode != 0))
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
						txt_mvaddch (curs_yloc-cam_yloc, curs_xloc-cam_xloc, event.key.keysym.unicode);
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

			case SDL_VIDEORESIZE:
			{
				// TODO: something nice about window resizing
				gr_resize (event.resize.h, event.resize.w);
				break;
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
			txt_mvaddch (curs_yloc, curs_xloc, ' ');
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
			txt_mvaddch (curs_yloc, curs_xloc, ' ');
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

void gr_resize (int ysiz, int xsiz)
{
	SDL_FreeSurface (screen);
	screen = SDL_SetVideoMode (xsiz, ysiz, 32, SDL_SWSURFACE | SDL_RESIZABLE);

	int chy = ysiz/GLH - snumy, chx = xsiz/GLW - snumx;
	snumy += chy;
	snumx += chx;
	pnumy += chy;
	pnumx += chx;
	
	txt_map = realloc (txt_map, sizeof(glyph) * snumy * snumx);
	memset (txt_map, 0, sizeof(glyph) * snumy * snumx);
	txt_change = realloc (txt_change, snumy * snumx);
	memset (txt_change, 0, snumy * snumx);

	p_init ();
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

#define GR_TRY_TILES(a,b)\
sprintf (filepath, a, b);\
temp = SDL_LoadBMP (filepath);\
if (temp)\
	goto success;\
printf("Couldn't find tiles at %s.\n", filepath)

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
	
	screen = SDL_SetVideoMode (screenX, screenY, 32, SDL_SWSURFACE | SDL_RESIZABLE);
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

	gr_resize (screenY, screenX);
	
	/* Finish housekeeping */
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (200, 30);
	SDL_WM_SetCaption ("Yore", "Yore");
}

void gr_wait (uint32_t ms)
{
	SDL_Delay (ms);
}

#define GR_NEAR 5
int gr_nearedge (int yloc, int xloc)
{
	yloc -= cam_yloc; xloc -= cam_xloc;
	return (yloc <= GR_NEAR || yloc >= (pnumy-GR_NEAR) ||
	        xloc <= GR_NEAR || xloc >= (pnumx-GR_NEAR));
}

