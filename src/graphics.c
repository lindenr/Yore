/* graphics.c */

#include "include/graphics.h"
#include "include/vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"

uint32_t bg_colour;
int forced_refresh = 0;

SDL_Surface *screen = NULL, *tiles = NULL, *glyph_col = NULL;

/* starting size */
int screenY = 720, screenX = 1000;

glyph *txt_map = NULL;
char *txt_change = NULL;

glyph *screen_map = NULL;
char *screen_change = NULL;

/* where written text and input appears */
int curs_yloc = 0, curs_xloc = 0;
/* window dimensions (in glyphs) */
int txt_h = 0, txt_w = 0;
int txt_area = 0;
/* location of the blinking cursor */
int csr_y = 0, csr_x = 0, csr_state = 1;

Vector graphs = NULL;

/* event callback functions */
void (*gr_onidle) () = NULL;
void (*gr_onresize) () = NULL;
void (*gr_onrefresh) () = NULL;

int gra_buffer (Graph gra, int yloc, int xloc)
{
	if (yloc < 0 || yloc >= gra->h || xloc < 0 || xloc >= gra->w)
		return -1;
	return gra->w * yloc + xloc;
}

int txt_buffer (int yloc, int xloc)
{
	if (yloc < 0 || yloc >= txt_h || xloc < 0 || xloc >= txt_w)
		return -1;
	return txt_w * yloc + xloc;
}

void txt_move (int yloc, int xloc)
{
	curs_yloc = yloc;
	curs_xloc = xloc;
}

void gra_movecam (Graph gra, int yloc, int xloc)
{
	gra->cy = yloc;
	gra->cx = xloc;
	
	forced_refresh = 1;
}

void gra_centcam (Graph gra, int yloc, int xloc)
{
	gra_movecam (gra, gra->cy - gra->vh/2, gra->cx - gra->vw/2);
}

void gra_baddch (Graph gra, int buf, glyph gl)
{
	if (gra->data[buf] == gl) return;
	if (gl == (gl&255)) gl |= COL_TXT_DEF;
	gra->data[buf] = gl;
	gra->change[buf] = 1;
}

void txt_baddch (int buf, glyph gl)
{
	if (gl != 0 && gl == (gl&255)) gl |= COL_TXT_DEF;
	if (txt_map[buf] == gl) return;
	txt_map[buf] = gl;
	txt_change[buf] = 1;
}

void gra_mvaddch (Graph gra, int yloc, int xloc, glyph gl)
{
	int buf = gra_buffer (gra, yloc, xloc);
	if (buf != -1)
		gra_baddch (gra, buf, gl);
}

void txt_mvaddch (int yloc, int xloc, glyph gl)
{
	int buf = txt_buffer (yloc, xloc);
	if (buf != -1)
		txt_baddch (buf, gl);
}

void txt_mvaprint (int yloc, int xloc, const char *str)
{
	int i, buf = txt_buffer (yloc, xloc), len = strlen(str);

	for (i = 0; i < len && i+buf < txt_area; ++ i)
		txt_baddch (i+buf, str[i]);
}

void txt_mvprint (int yloc, int xloc, const char *str, ...)
{
	va_list args;
	char out[1024];

	txt_move (yloc, xloc);
	va_start (args, str);
	vsprintf (out, str, args);
	va_end (args);

	txt_mvaprint (yloc, xloc, out);
}

void glyph_box (int yloc, int xloc, int height, int width,
                glyph ULCORNER, glyph URCORNER, glyph LLCORNER, glyph LRCORNER,
				glyph HLINE, glyph VLINE)
{
	int xt = width - 1, yt = height - 1;
	txt_mvaddch (yloc, xloc, ULCORNER);
	txt_mvaddch (yloc + height, xloc, LLCORNER);
	txt_mvaddch (yloc, xloc + width, URCORNER);
	txt_mvaddch (yloc + height, xloc + width, LRCORNER);
	while (xt--)
	{
		txt_mvaddch (yloc, xloc + xt + 1, HLINE);
		txt_mvaddch (yloc + height, xloc + xt + 1, HLINE);
	}
	while (yt--)
	{
		txt_mvaddch (yloc + yt + 1, xloc, VLINE);
		txt_mvaddch (yloc + yt + 1, xloc + width, VLINE);
	}
}

void txt_box (int yloc, int xloc, int height, int width)
{
	glyph_box (yloc, xloc, height, width,
	           ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER,
			   ACS_HLINE, ACS_VLINE);
}

void txt_dbox (int yloc, int xloc, int height, int width)
{
	glyph_box (yloc, xloc, height, width,
	           DCS_ULCORNER, DCS_URCORNER, DCS_LLCORNER, DCS_LRCORNER,
			   DCS_HLINE, DCS_VLINE);
}

void txt_fbox (int yloc, int xloc, int height, int width, glyph fill)
{
	int x, y;
	for (x = 1; x < width; ++x)
		for (y = 1; y < height; ++y)
			txt_mvaddch (yloc + y, xloc + x, fill);
	txt_box (yloc, xloc, height, width);
}

void csr_noblink ()
{
	csr_state = 3;
}

void csr_blink ()
{
	csr_state = 1;
}

void csr_move (int yloc, int xloc)
{
	if (yloc == csr_y && xloc == csr_x)
		return;
	txt_mark (csr_y, csr_x);
	txt_mark (yloc, xloc);
	csr_y = yloc;
	csr_x = xloc;
}

void csr_hide ()
{
	csr_state = 2;
}

void csr_show ()
{
	csr_state = 1;
}

void gra_mark (Graph gra, int yloc, int xloc)
{
	if (yloc >= gra->cy && yloc < gra->cy + gra->vh &&
	    xloc >= gra->cx && xloc < gra->cx + gra->vw)
		txt_mark (yloc - gra->cy + gra->vy, xloc - gra->cx + gra->vx);
}

void txt_mark (int yloc, int xloc)
{
	if (yloc >= 0 && yloc < txt_h &&
	    xloc >= 0 && xloc < txt_w)
		txt_change[txt_buffer(yloc, xloc)] = 1;
}

#define GL_TRD ((gl&0xF0000000)>>24)
#define GL_TGN ((gl&0x0F000000)>>20)
#define GL_TBL ((gl&0x00F00000)>>16)
#define GL_BRD ((gl&0x000F0000)>>12)
#define GL_BGN ((gl&0x0000F000)>>8)
#define GL_BBL ((gl&0x00000F00)>>4)
inline void blit_glyph (glyph gl, int yloc, int xloc)
{
	if (yloc == csr_y && xloc == csr_x && (csr_state&1))
		gl = ((gl << 12)&0xFFF00000) ^ ((gl >> 12)&0x000FFF00) ^ (gl&0xFF);
	char ch = (char) gl;
	SDL_Rect srcrect = {GLW*(ch&15), GLH*((ch>>4)&15), GLW, GLH};
	SDL_Rect dstrect = {GLW*xloc, GLH*yloc, GLW, GLH};

	SDL_FillRect (glyph_col, NULL, SDL_MapRGB (glyph_col->format, GL_BRD, GL_BGN, GL_BBL));
	SDL_BlitSurface (tiles, &srcrect, glyph_col, NULL);
	
	SDL_FillRect (screen, &dstrect, SDL_MapRGB (screen->format, GL_TRD, GL_TGN, GL_TBL));
	SDL_BlitSurface (glyph_col, NULL, screen, &dstrect);
}

void gr_refresh ()
{
	int i, x, y;

	for (i = 0; i < graphs->len; ++ i)
	{
		Graph gra = * (Graph *) v_at (graphs, i);
		if (!gra->vis)
			continue;
		for (x = 0; x < gra->vw; ++ x)
		{
			for (y = 0; y < gra->vh; ++ y)
			{
				int gr_y = y + gra->cy, gr_x = x + gra->cx;
				int txt_y = y + gra->vy, txt_x = x + gra->vx;
				int gr_c = gra_buffer (gra, gr_y, gr_x);
				int txt_c = txt_buffer (txt_y, txt_x);
				if (gr_c != -1 && txt_c != -1)
				{
					if (gra->change[gr_c] || forced_refresh ||
					    (txt_map[txt_c] == 0 && txt_change[txt_c]))
					{
						screen_map[txt_c] = gra->data[gr_c];
						screen_change[txt_c] = 1;
					}
		//			else
		//				screen_map[txt_c] = 0;
				}
				gra->change[gr_c] = 0;
			}
		}
	}

	for (x = 0; x < txt_w; ++ x)
	{
		for (y = 0; y < txt_h; ++ y)
		{
			int txt_c = txt_buffer (y, x);
			if (txt_change[txt_c] || forced_refresh)
			{
				if (txt_map[txt_c] || !screen_change[txt_c])
				{
					screen_map[txt_c] = txt_map[txt_c];
					screen_change[txt_c] = 1;
				}
			}
			txt_change[txt_c] = 0;
		}
	}

	for (x = 0; x < txt_w; ++ x)
	{
		for (y = 0; y < txt_h; ++ y)
		{
			int txt_c = txt_buffer (y, x);
			if (screen_change[txt_c] || forced_refresh)
				blit_glyph (screen_map[txt_c], y, x);
			screen_map[txt_c] = 0;
			screen_change[txt_c] = 0;
		}
	}

	if (gr_onrefresh)
		gr_onrefresh ();

	forced_refresh = 0;
	SDL_UpdateRect (screen, 0, 0, 0, 0);
}

void gr_frefresh ()
{
	forced_refresh = 1;
	gr_refresh ();
}

void txt_clear ()
{
	int i;
	//px_showboxes (); why was this here?
	for (i = 0; i < txt_area; ++ i)
		txt_baddch (i, 0);
	//gr_refresh ();
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

uint32_t end = 0;
uint32_t gr_getfullch ()
{
	gr_refresh ();

	SDL_Event event;
	//uint32_t modifier_keys = (KMOD_SHIFT | KMOD_CAPS | KMOD_NUM) << 16;
	int ticks = SDL_GetTicks ();
	if (tout_num && end <= ticks)
		end = tout_num + ticks;
	if (tout_num == 0)
		end = 0;

	while (1)
	{
		if (end && SDL_GetTicks () >= end)
		{
			end = 0;
			break;
		}
		if (!SDL_PollEvent (&event))
		{
			if (gr_onidle)
				gr_onidle ();
			gr_wait (20);
			continue;
		}

		switch (event.type)
		{
			case SDL_KEYDOWN:
			 {
				uint32_t mod = event.key.keysym.mod << 16;
				/*if ((mod & (~modifier_keys)) && (event.key.keysym.unicode != 0))
				{
					//printf ("%d %d\n", event.key.keysym.sym, event.key.keysym.unicode);
					return mod|event.key.keysym.sym;
				}*/
				if (event.key.keysym.sym == SDLK_UP)
					return mod|GRK_UP;
				if (event.key.keysym.sym == SDLK_DOWN)
					return mod|GRK_DN;
				if (event.key.keysym.sym == SDLK_LEFT)
					return mod|GRK_LF;
				if (event.key.keysym.sym == SDLK_RIGHT)
					return mod|GRK_RT;
				if (event.key.keysym.unicode == 0)
					break;
				/*if (echoing)
				{
					if (event.key.keysym.unicode >= 0x20)
						txt_mvaddch (curs_yloc-gra->cy, curs_xloc-gra->cx, event.key.keysym.unicode);
					if (curs_xloc == MAP_WIDTH-1)
					{
						curs_xloc = 0;
						++ curs_yloc;
					}
					else
						++ curs_xloc;
				}*/

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

void txt_getstr (char *out, int len)
{
	int i = 0;
	do
	{
		int tcx = curs_xloc, tcy = curs_yloc;
		csr_move (curs_yloc, curs_xloc);
		char in = gr_getch ();
		curs_yloc = tcy;
		curs_xloc = tcx;
		if (in == CH_LF || in == CH_CR) break;
		if (in == CH_BS)
		{
			if (i)
			{
				if (curs_xloc == 0)
				{
					curs_xloc = txt_w-1;
					-- curs_yloc;
				}
				else
					-- curs_xloc;
				-- i;
			}
			out[i] = 0;
			txt_mvaddch (curs_yloc, curs_xloc, ' ');
			continue;
		}
		txt_mvaddch (curs_yloc, curs_xloc, in);
		out[i] = in;
		if (i < len-1)
		{
			++ curs_xloc;
			++ i;
		}
	}
	while (1);
	out[i] = 0;
}

int txt_echo (int echo)
{
	if (echo != -1)
		echoing = (echo == 1);
	return echoing;
}

void gr_resize (int ysiz, int xsiz)
{
	int i;
	SDL_FreeSurface (screen);
	screen = SDL_SetVideoMode (xsiz, ysiz, 32, SDL_SWSURFACE | SDL_RESIZABLE);

	txt_h = ysiz/GLH;
	txt_w = xsiz/GLW;
	txt_area = txt_h * txt_w;
	
	txt_map = realloc (txt_map, sizeof(glyph) * txt_area);
	memset (txt_map, 0, sizeof(glyph) * txt_area);
	txt_change = realloc (txt_change, sizeof(char) * txt_area);
	memset (txt_change, 0, sizeof(char) * txt_area);
	screen_map = realloc (screen_map, sizeof(glyph) * txt_area);
	for (i = 0; i < txt_area; ++ i)
		screen_map[i] = ' ';
	screen_change = realloc (screen_change, sizeof(char) * txt_area);
	memset (screen_change, 0, sizeof(char) * txt_area);

	forced_refresh = 1;

	if (gr_onresize)
		gr_onresize ();
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

	GR_TRY_TILES("res/%s", TILE_FILE);
	GR_TRY_TILES("../res/%s", TILE_FILE);
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
	
	SDL_EnableUNICODE (1);
	SDL_EnableKeyRepeat (200, 30);
	SDL_WM_SetCaption ("Yore", "Yore");
	csr_hide ();

	if (!graphs)
		graphs = v_dinit (sizeof(Graph));
}

Graph gra_init (int h, int w, int vy, int vx, int vh, int vw)
{
	Graph gra;

	glyph *data = malloc (sizeof(glyph) * h * w);
	char *change = malloc (sizeof(char) * h * w);

	memset (data, 0, sizeof(glyph) * h * w);
	memset (change, 0, sizeof(char) * h * w);
	
	gra = malloc (sizeof(struct Graph));
	struct Graph from = {h, w, h * w, data, change, 0, 0, vy, vx, vh, vw, 1};
	memcpy (gra, &from, sizeof(struct Graph));

	if (!graphs)
		graphs = v_dinit (sizeof(Graph));
	v_push (graphs, &gra);
	
	return gra;
}

void gr_wait (uint32_t ms)
{
	SDL_Delay (ms);
}

uint32_t gr_getms ()
{
	return SDL_GetTicks ();
}

#define GR_NEAR 5
int gra_nearedge (Graph gra, int yloc, int xloc)
{
	yloc -= gra->cy; xloc -= gra->cx;
	return (yloc <= GR_NEAR || yloc >= (gra->vh-GR_NEAR) ||
	        xloc <= GR_NEAR || xloc >= (gra->vw-GR_NEAR));
}

