/* graphics.c */

#include "include/graphics.h"
#include "include/vector.h"
#include "include/drawing.h"

#include <stdio.h>
#include <stdarg.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"
/*
int BOXPOS[BOX_NUM][2] = {
	{0, 0},
	{0, 0},
	{2, 0},
	{0, 2}
};

int BOXCOL[BOX_NUM][3] = {
	{0, 0, 0},
	{150, 0, 0},
	{0, 100, 200},
	{255, 90, 50}
};*/

int forced_refresh = 0;

SDL_Window *sdlWindow;
SDL_Renderer *sdlRenderer;
SDL_Surface *tiles, *screen, *glyph_col;
SDL_Texture *sdlTexture;

/* starting and current size */
int screenY = 0, screenX = 0;

glyph *gr_map = NULL;
gflags *gr_flags = NULL;

/* window dimensions (in glyphs) */
int gr_h = 0, gr_w = 0; //screenY/GLH, gr_w = screenX/GLW;
int gr_area = 0;

Vector graphs = NULL;

/* event callback functions */
void (*gr_onidle) () = NULL;
void (*gr_onresize) () = NULL;
void (*gr_onrefresh) () = NULL;
void (*gr_quit) () = NULL;

int gra_buffer (Graph gra, int yloc, int xloc)
{
	if (yloc < 0 || yloc >= gra->h || xloc < 0 || xloc >= gra->w)
		return -1;
	return gra->w * yloc + xloc;
}

int gr_buffer (int yloc, int xloc)
{
	if (yloc < 0 || yloc >= gr_h || xloc < 0 || xloc >= gr_w)
		return -1;
	return gr_w * yloc + xloc;
}

void gr_ext (glyph *out, char *in, glyph def)
{
	int i;
	for (i = 0; in[i]; ++ i)
		out[i] = in[i] | (def & 0xFFFFFF00);
	out[i] = 0;
}

void gra_movecam (Graph gra, int yloc, int xloc)
{
	gra->cy = yloc;
	gra->cx = xloc;
	
	forced_refresh = 1;
}

void gra_centcam (Graph gra, int yloc, int xloc)
{
	gra_movecam (gra, yloc - gra->vh/2, xloc - gra->vw/2);
}

void gra_baddch (Graph gra, int buf, glyph gl)
{
	/* what do you actually want to draw? */
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	/* only care about the changed flag if there are other pixels */
	if (gra->flags[buf])
		gra->flags[buf] = 1;
	/* if the same then don't bother */
	if (gra->data[buf] == gl) return;
	/* update current to new */
	gra->data[buf] = gl;
	/* transparent */
	if (gl == 0)
	{
		int gra_y = buf/gra->w, gra_x = buf%gra->w;
		int gr_y = gra_y - gra->cy + gra->vy, gr_x = gra_x - gra->cx + gra->vx;
		int gr_c = gr_buffer (gr_y, gr_x);
		gr_flags[gr_c] |= 1;
	}
}

void gra_bgaddch (Graph gra, int buf, glyph gl)
{
	/* what do you actually want to draw? */
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	if ((gl&COL_BG_MASK) == 0)
		gl |= gra->data[buf] & COL_BG_MASK;
	/* only care about the changed flag if there are other pixels */
	if (gra->flags[buf])
		gra->flags[buf] = 1;
	/* if the same then don't bother */
	if (gra->data[buf] == gl) return;
	/* update current to new */
	gra->data[buf] = gl;
	/* transparent */
	if (gl == 0)
	{
		int gra_y = buf/gra->w, gra_x = buf%gra->w;
		int gr_y = gra_y - gra->cy + gra->vy, gr_x = gra_x - gra->cx + gra->vx;
		int gr_c = gr_buffer (gr_y, gr_x);
		gr_flags[gr_c] |= 1;
	}
}

void gra_mvaddch (Graph gra, int yloc, int xloc, glyph gl)
{
	int buf = gra_buffer (gra, yloc, xloc);
	if (buf != -1)
		gra_baddch (gra, buf, gl);
}

void gra_mvaprint (Graph gra, int yloc, int xloc, const char *str)
{
	int i, buf = gra_buffer (gra, yloc, xloc), len = strlen(str);

	for (i = 0; i < len && i+buf > 0 && i+buf < gra->a; ++ i)
		gra_baddch (gra, i+buf, str[i]);
}

void gra_mvprint (Graph gra, int yloc, int xloc, const char *str, ...)
{
	va_list args;
	char out[1024];

	va_start (args, str);
	vsnprintf (out, 1024, str, args);
	va_end (args);

	gra_mvaprint (gra, yloc, xloc, out);
}

void gra_cprint (Graph gra, int yloc, const char *str, ...)
{
	va_list args;
	char out[1024];

	va_start (args, str);
	int len = vsnprintf (out, 1024, str, args);
	va_end (args);

	int xloc = (gra->w - len)/2;
	gra_mvaprint (gra, yloc, xloc, out);
}

void gra_mvaprintex (Graph gra, int yloc, int xloc, const glyph *str)
{
	int i, buf = gra_buffer (gra, yloc, xloc);

	for (i = 0; str[i] && i+buf > 0 && i+buf < gra->a; ++ i)
		gra_baddch (gra, i+buf, str[i]);
}

#include "include/rand.h"
void gra_drawline (Graph gra, int fy, int fx, int ty, int tx, glyph gl)
{
	struct BresState st;
	bres_init (&st, fy, fx, ty, tx);
	bres_iter (&st);
	while (!st.done)
	{
		if (st.err*2 > -st.dy && st.err*2 < st.dx)
			gra_mvaddch (gra, st.cy, st.cx, (st.sx == st.sy ? '\\' : '/') | gl);
				//COL_BG(11,0,0) | COL_TXT(15,15,0));
		else
			gra_mvaddch (gra, st.cy, st.cx, (st.dx > st.dy ? ACS_HLINE : ACS_VLINE) | gl);
				//COL_BG(11,0,0) | COL_TXT(15,15,0));
		bres_iter (&st);
	}
}

void gra_drawdisc (Graph gra, int x, int y, int rad, glyph gl)
{
	int i, j;
	int weight = rad * (rad + 1);
	for (i = -rad; i <= rad; ++ i) for (j = -rad; j <= rad; ++ j)
	{
		if (i*i + j*j > weight)
			continue;
		int sat = i*i + j*j;
		int drop = (sat*4) / weight;
		gra_mvaddch (gra, i + x, j + y, gl | COL_BG_BLUE (10 + rn(2) + rn(2) + rn(2) - drop));
	}
}

void glyph_box (Graph gra, int yloc, int xloc, int height, int width,
                glyph ULCORNER, glyph URCORNER, glyph LLCORNER, glyph LRCORNER,
				glyph HLINE, glyph VLINE)
{
	int xt = width - 1, yt = height - 1;
	gra_mvaddch (gra, yloc, xloc, ULCORNER);
	gra_mvaddch (gra, yloc + height, xloc, LLCORNER);
	gra_mvaddch (gra, yloc, xloc + width, URCORNER);
	gra_mvaddch (gra, yloc + height, xloc + width, LRCORNER);
	while (xt--)
	{
		gra_mvaddch (gra, yloc, xloc + xt + 1, HLINE);
		gra_mvaddch (gra, yloc + height, xloc + xt + 1, HLINE);
	}
	while (yt--)
	{
		gra_mvaddch (gra, yloc + yt + 1, xloc, VLINE);
		gra_mvaddch (gra, yloc + yt + 1, xloc + width, VLINE);
	}
}

void gra_box (Graph gra, int yloc, int xloc, int height, int width)
{
	glyph_box (gra, yloc, xloc, height, width,
	           ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER,
			   ACS_HLINE, ACS_VLINE);
}

void gra_dbox (Graph gra, int yloc, int xloc, int height, int width)
{
	glyph_box (gra, yloc, xloc, height, width,
	           DCS_ULCORNER, DCS_URCORNER, DCS_LLCORNER, DCS_LRCORNER,
			   DCS_HLINE, DCS_VLINE);
}

void gra_fbox (Graph gra, int yloc, int xloc, int height, int width, glyph gl)
{
	int x, y;
	for (x = 1; x < width; ++x)
		for (y = 1; y < height; ++y)
			gra_mvaddch (gra, yloc + y, xloc + x, gl);
	gra_box (gra, yloc, xloc, height, width);
}

void gra_csolid (Graph gra)
{
	gra->csr_state = 3;
}

void gra_cblink (Graph gra)
{
	gra->csr_state = 1;
}

void gra_cmove (Graph gra, int yloc, int xloc)
{
	if (yloc == gra->csr_y && xloc == gra->csr_x)
		return;
	gra_mark (gra, gra->csr_y, gra->csr_x);
	gra_mark (gra, yloc, xloc);
	gra->csr_y = yloc;
	gra->csr_x = xloc;
}

void gra_chide (Graph gra)
{
	gra->csr_state = 2;
}

void gra_cshow (Graph gra)
{
	gra->csr_state = 1;
}

void gra_hide (Graph gra)
{
	gra->vis = 0;
	forced_refresh = 1;
}

void gra_show (Graph gra)
{
	gra->vis = 1;
	forced_refresh = 1;
}

void gra_mark (Graph gra, int yloc, int xloc)
{
	gra->flags[gra_buffer(gra, yloc, xloc)] |= 1;
}
/*
void gra_bsetbox (Graph gra, int b, gflags flags)
{
	gra->flags[b] = 1|flags;
}*/

#define setpixel(y,x) {*(uint32_t*) ((uint8_t*) pixels + 4*(x) + screen->pitch*(y)) = col;}
void gr_drawboxes (int y, int x, gflags f)
{
	if (SDL_MUSTLOCK (screen)) // TODO draw all boxes under one lock
		SDL_LockSurface (screen);
	/*int type;
	for (type = 1; type < 8; ++ type)
	{
		if (!(f&(1<<type))) continue;
		int py = y * GLH + BOXPOS[type][0], px = x * GLW + BOXPOS[type][1];
		int r = BOXCOL[type][0], g = BOXCOL[type][1], b = BOXCOL[type][2];
		uint32_t *pixels = (uint32_t *) ((uintptr_t) screen->pixels + py*screen->pitch + px*4);
		uint32_t col = SDL_MapRGB (screen->format, r, g, b);
		setpixel(0,0); setpixel(0,1); setpixel(1,0); setpixel(1,1);
	}*/

	int ismoving = 1 & (f>>12), isattacking = 1 & (f>>17), code, px, py, i;
	uint32_t col, *pixels;
	py = y * GLH, px = x * GLW;
	pixels = (uint32_t *) ((uintptr_t) screen->pixels + py*screen->pitch + px*4);

	///////////// MOVING
	if (ismoving == 0) goto end_moving;
	col = SDL_MapRGB (screen->format, 0xFF,0xFF,0xFF);
	code = (f>>8)&15;
	switch (code)
	{
	case 1: for (i = 0;       i < GLW;   ++ i) setpixel(0,     i); break;
	case 2: for (i = 1+GLW/2; i < GLW;   ++ i) setpixel(0,     i);
			for (i = 0;       i < GLH/2; ++ i) setpixel(i, GLW-1); break;
	case 5: for (i = 0;       i < GLH;   ++ i) setpixel(i, GLW-1); break;
	case 8: for (i = 1+GLW/2; i < GLW;   ++ i) setpixel(GLH-1, i);
			for (i = 1+GLH/2; i < GLH;   ++ i) setpixel(i, GLW-1); break;
	case 7: for (i = 0;       i < GLW;   ++ i) setpixel(GLH-1, i); break;
	case 6: for (i = 0;       i < GLW/2; ++ i) setpixel(GLH-1, i);
			for (i = 1+GLH/2; i < GLH;   ++ i) setpixel(i,     0); break;
	case 3: for (i = 0;       i < GLH;   ++ i) setpixel(i,     0); break;
	case 0: for (i = 0;       i < GLW/2; ++ i) setpixel(0,     i);
			for (i = 0;       i < GLH/2; ++ i) setpixel(i,     0); break;
	}
end_moving:

	///////////// ATTACKING
	if (isattacking == 0) goto end_attacking;
	col = SDL_MapRGB (screen->format, 0xFF,0x33,0x33);
	code = (f>>13)&15;
	switch (code)
	{
	case 1: for (i = 0;       i < GLW;   ++ i) setpixel(0,     i); break;
	case 2: for (i = 1+GLW/2; i < GLW;   ++ i) setpixel(0,     i);
			for (i = 0;       i < GLH/2; ++ i) setpixel(i, GLW-1); break;
	case 5: for (i = 0;       i < GLH;   ++ i) setpixel(i, GLW-1); break;
	case 8: for (i = 1+GLW/2; i < GLW;   ++ i) setpixel(GLH-1, i);
			for (i = 1+GLH/2; i < GLH;   ++ i) setpixel(i, GLW-1); break;
	case 7: for (i = 0;       i < GLW;   ++ i) setpixel(GLH-1, i); break;
	case 6: for (i = 0;       i < GLW/2; ++ i) setpixel(GLH-1, i);
			for (i = 1+GLH/2; i < GLH;   ++ i) setpixel(i,     0); break;
	case 3: for (i = 0;       i < GLH;   ++ i) setpixel(i,     0); break;
	case 0: for (i = 0;       i < GLW/2; ++ i) setpixel(0,     i);
			for (i = 0;       i < GLH/2; ++ i) setpixel(i,     0); break;
	}
end_attacking:

	if (SDL_MUSTLOCK (screen))
		SDL_UnlockSurface (screen);
}

#define GL_TRD ((gl&0xF0000000)>>24)
#define GL_TGN ((gl&0x0F000000)>>20)
#define GL_TBL ((gl&0x00F00000)>>16)
#define GL_BRD ((gl&0x000F0000)>>12)
#define GL_BGN ((gl&0x0000F000)>>8)
#define GL_BBL ((gl&0x00000F00)>>4)
void blit_glyph (glyph gl, int yloc, int xloc)
{
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

		for (y = 0; y < gra->vh; ++ y)
		{
			for (x = 0; x < gra->vw; ++ x)
			{
				int gra_y = y + gra->cy, gra_x = x + gra->cx;
				int gr_y = y + gra->vy, gr_x = x + gra->vx;
				int gra_c = gra_buffer (gra, gra_y, gra_x);
				int gr_c = gr_buffer (gr_y, gr_x);
				if (gra_c != -1 && gr_c != -1)
				{
					if ((gra->flags[gra_c]&1) || forced_refresh || gr_flags[gr_c] ||
						gra->old[gra_c] != gra->data[gra_c])
					{
						glyph gl = gra->data[gra_c];
						if (gra->csr_state && gra->csr_y == gra_y && gra->csr_x == gra_x)
						{
							gr_map[gr_c] = gra_glinvert (gra, gl);
							gr_flags[gr_c] = 1|gra->flags[gra_c];
						}
						else if (gl)
						{
							gr_map[gr_c] = gl;
							gr_flags[gr_c] = 1|gra->flags[gra_c];
						}
						else
							gr_flags[gr_c] |= 1;
						gra->old[gra_c] = gl;
					}
				}
				gra->flags[gra_c] &= (~1);
			}
		}
	}

#ifdef DEBUG_REFRESH_TIME
	uint32_t asdf = gr_getms();
	//fprintf(stderr, "time: %ums\n", (asdf=gr_getms()));
#endif
	int gr_c = 0;
	int drawn = 0;
	int lmost = gr_w-1, rmost = 0, umost = gr_h-1, dmost = 0;
	for (y = 0; y < gr_h; ++ y)
	{
		for (x = 0; x < gr_w; ++ x, ++ gr_c)
		{
			if (gr_flags[gr_c] || forced_refresh)
			{
				if (umost > y)
					umost = y;
				if (dmost < y)
					dmost = y;
				if (lmost > x)
					lmost = x;
				if (rmost < x)
					rmost = x;
				++ drawn;
				blit_glyph (gr_map[gr_c], y, x);
				gflags f = gr_flags[gr_c];
				if (f)
					gr_drawboxes (y, x, f);
				gr_flags[gr_c] = 0;
			}
			gr_map[gr_c] = 0;
		}
	}

	//fprintf(stderr, "visible: %d\n", gr_c);
	//fprintf(stderr, "drawn: %d\n", drawn);
	if (gr_onrefresh)
		gr_onrefresh ();
	if (drawn || forced_refresh)
	{
		if (1 || forced_refresh)
			SDL_UpdateTexture (sdlTexture, NULL, screen->pixels, screenX * sizeof(Uint32));
		else
		{
			//int asdf = gr_getms();
			SDL_Rect rect = {lmost * GLW, umost * GLH,
				(rmost + 1 - lmost) * GLW, (dmost + 1 - umost) * GLH};
			void *pixels;
			int pitch;
			SDL_LockTexture (sdlTexture, &rect, &pixels, &pitch);
			for (y = 0; y < rect.h; ++ y)
				memcpy (pixels + pitch*y, screen->pixels + 4*rect.x + screen->pitch*(y+rect.y), 4*rect.w);
			SDL_UnlockTexture (sdlTexture);
			//printf ("area %d took %d (%d %d %d %d)\n", rect.w * rect.h, gr_getms() - asdf,
			//	lmost, rmost, umost, dmost);
		}

		SDL_RenderClear (sdlRenderer);
		SDL_RenderCopy (sdlRenderer, sdlTexture, NULL, NULL);
		SDL_RenderPresent (sdlRenderer);
	}
	forced_refresh = 0;
#ifdef DEBUG_REFRESH_TIME
	fprintf(stderr, "time: %ums\n\n", gr_getms() - asdf);
#endif
}

void gr_frefresh ()
{
	forced_refresh = 1;
	gr_refresh ();
}

void gra_clear (Graph gra)
{
	int i;
	if (!gra)
		return;
	
	for (i = 0; i < gra->a; ++ i)
		gra_baddch (gra, i, 0);
}

glyph gra_glinvert (Graph gra, glyph gl)
{
	if (gl == 0)
		gl = gra->def & 0xFFFFFF00;
	return ((gl << 12)&0xFFF00000) ^ ((gl >> 12)&0x000FFF00) ^ (gl&0xFF);
}

void gra_invert (Graph gra, int yloc, int xloc)
{
	int b = gra_buffer(gra, yloc, xloc);
	gra->data[b] = gra_glinvert (gra, gra->data[b]);
	gra->flags[b] |= 1;
}

int tout_num = 0;
void gr_tout (int t)
{
	tout_num = t;
}

int echoing = 1;

int gr_inputcode (SDL_Keycode code)
{
	return (code >= 32 && code < 128);
}

int gr_inputch (char in)
{
	return (in >= 32 && in < 128);
}

uint32_t gr_kinitdelay = 180, gr_kdelay = 40;
uint32_t end = 0, key_fire_ms = 0;
char cur_key_down = 0;
int num_keys_down = 0;

#ifdef DEBUG_REFRESH_TIME
uint32_t lastref = 0;
#endif

// TODO: is text parameter necessary? Could just check if it is an input code
char gr_getch_aux (int text)
{
	uint32_t ticks = gr_getms ();
#ifdef DEBUG_REFRESH_TIME
	//fprintf(stderr, "Time since last getch: %d\n", ticks - lastref);
	//lastref = ticks;
#endif
	gr_refresh ();

	if (tout_num && end <= ticks)
		end = tout_num + ticks;
	else if (tout_num == 0)
		end = 0;
	SDL_Event sdlEvent;
	while (1)
	{
		ticks = gr_getms ();
		if (end && ticks >= end)
		{
			end = 0;
			break;
		}

		if (!SDL_PollEvent (&sdlEvent))
		{
			if (cur_key_down && ticks >= key_fire_ms)
			{
				key_fire_ms = ticks + gr_kdelay;
				return cur_key_down;
			}
			if (gr_onidle)
				gr_onidle ();
			gr_wait (10);
			continue;
		}

		char input_key = 0;
		SDL_Keycode code;
		switch (sdlEvent.type)
		{
			case SDL_TEXTINPUT:
				input_key = sdlEvent.text.text[0];
				break;
			case SDL_KEYDOWN:
				if (sdlEvent.key.repeat)
					break;
				code = sdlEvent.key.keysym.sym;
				if (gr_inputcode(code))
				{
					++ num_keys_down;
					if (num_keys_down == 1 && (!text) &&
					    (sdlEvent.key.keysym.mod & (KMOD_LCTRL | KMOD_RCTRL)))
					{
						input_key = GR_CTRL(code);
						break;
					}
				}
				if (code == SDLK_UP)
					input_key = GRK_UP;
				else if (code == SDLK_DOWN)
					input_key = GRK_DN;
				else if (code == SDLK_LEFT)
					input_key = GRK_LF;
				else if (code == SDLK_RIGHT)
					input_key = GRK_RT;
				else if (code == SDLK_RETURN)
					input_key = CH_LF;
				else if (code == SDLK_BACKSPACE)
					input_key = CH_BS;
				else if (code == SDLK_ESCAPE)
					input_key = CH_ESC;
				break;

			case SDL_KEYUP:
				code = sdlEvent.key.keysym.sym;
				if (gr_inputcode(code))
					-- num_keys_down;
				if (!num_keys_down)
					cur_key_down = 0;
				break;

			/*case SDL_VIDEORESIZE:
			{
				// TODO: something nice about window resizing
				gr_resize (event.resize.h, event.resize.w);
				break;
			}*/

			case SDL_WINDOWEVENT:
				if (sdlEvent.window.event == SDL_WINDOWEVENT_EXPOSED)
					gr_refresh ();
				break;
			
			case SDL_QUIT:
				if (!gr_quit)
					exit(0);
				/* this is allowed to return - can be interpreted as a hint
				 * to quit soon */
				gr_quit (); 
				break;
			
			default:
				break;
		}
		if (input_key && input_key == cur_key_down)
			continue;
		else if (input_key)
		{
			cur_key_down = input_key;
			key_fire_ms = gr_getms () + gr_kinitdelay;
			return input_key;
		}
	}
	return GRK_EOF;
}

char gr_getch ()
{
	return gr_getch_aux (0);
}

char gr_getch_text ()
{
	return gr_getch_aux (1);
}

void gra_getstr (Graph gra, int yloc, int xloc, char *out, int len)
{
	int i = 0;
	gra->csr_state = 2;
	while (1)
	{
		gra_cmove (gra, yloc, xloc);
		char in = gr_getch_text ();
		if (in == CH_LF || in == CH_CR) break;
		else if (in == CH_BS)
		{
			if (i)
			{
				-- xloc;
				-- i;
			}
			out[i] = 0;
			gra_mvaddch (gra, yloc, xloc, ' ');
			continue;
		}
		/* watershed - put non-input-char handling above here */
		else if (!gr_inputch (in)) continue;
		else if (i < len-1)
		{
			gra_mvaddch (gra, yloc, xloc, in);
			out[i] = in;
			++ xloc;
			++ i;
		}
	}
	gra->csr_state = 0;
	out[i] = 0;
}

void gr_resize (int ysiz, int xsiz)
{
	int i;
	//SDL_FreeSurface (screen);
	//screen = SDL_SetVideoMode (xsiz, ysiz, 32, SDL_SWSURFACE | SDL_RESIZABLE);

	screenY = ysiz;
	screenX = xsiz;
	gr_h = ysiz/GLH;
	gr_w = xsiz/GLW;
	gr_area = gr_h * gr_w;
	
	gr_map = realloc (gr_map, sizeof(glyph) * gr_area);
	for (i = 0; i < gr_area; ++ i)
		gr_map[i] = ' ';
	gr_flags = realloc (gr_flags, sizeof(gflags) * gr_area);
	memset (gr_flags, 0, sizeof(gflags) * gr_area);

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
snprintf (filepath, 100, a, b);\
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
	tiles = SDL_ConvertSurfaceFormat (temp, SDL_GetWindowPixelFormat (sdlWindow), 0);
	SDL_FreeSurface (temp);
	SDL_SetColorKey (tiles, SDL_TRUE, SDL_MapRGB (tiles->format, 255, 0, 255));
}

void gr_cleanup ()
{
	SDL_StopTextInput();
	SDL_DestroyRenderer (sdlRenderer);
	SDL_DestroyWindow (sdlWindow);
	SDL_Quit ();
}

void gr_init (int screenYstart, int screenXstart)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error initialising SDL: %s\n", SDL_GetError ());
		exit (1);
	}

	atexit (gr_cleanup);
	sdlWindow = SDL_CreateWindow ("Yore", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screenXstart, screenYstart, 0);
	if (sdlWindow == NULL)
	{
		fprintf (stderr, "SDL error: window is NULL\n");
		exit (1);
	}

	sdlRenderer = SDL_CreateRenderer (sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (sdlRenderer == NULL)
	{
		fprintf (stderr, "SDL error: renderer is NULL\n");
		exit (1);
	}
	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear (sdlRenderer);
	SDL_SetRenderDrawBlendMode (sdlRenderer, SDL_BLENDMODE_NONE);
	SDL_StartTextInput();
	sdlTexture = SDL_CreateTexture (sdlRenderer, SDL_GetWindowPixelFormat (sdlWindow), SDL_TEXTUREACCESS_STREAMING, screenXstart, screenYstart);
	screen = SDL_CreateRGBSurface (0, screenXstart, screenYstart, 32, 0, 0, 0, 0);
	glyph_col = SDL_CreateRGBSurface (0, GLW, GLH, 32, 0, 0, 0, 0);
	SDL_SetColorKey (glyph_col, SDL_TRUE, SDL_MapRGB (glyph_col->format, 255, 255, 255));

	gr_load_tiles ();
	gr_resize (screenYstart, screenXstart);
}

Graph gra_init (int h, int w, int vy, int vx, int vh, int vw)
{
	Graph gra;
	int a = h*w;

	glyph *data = malloc (sizeof(glyph) * a);
	glyph *old = malloc (sizeof(glyph) * a);
	gflags *flags = malloc (sizeof(gflags) * a);

	memset (data, 0, sizeof(glyph) * a);
	memset (old, 0, sizeof(glyph) * a);
	memset (flags, 0, sizeof(gflags) * a);
	
	gra = malloc (sizeof(struct Graph));
	struct Graph from = {h, w, a, data, old, flags, 0, 0, vy, vx, vh, vw, 1, 0, 0, 0, COL_TXT_DEF};
	memcpy (gra, &from, sizeof(struct Graph));

	if (!graphs)
		graphs = v_dinit (sizeof(Graph));
	v_push (graphs, &gra);
	
	return gra;
}

void gra_free (Graph gra)
{
	if (!graphs)
		return;
	
	int i;
	for (i = 0; i < graphs->len; ++ i)
	{
		if (gra == *(Graph*)v_at(graphs, i))
		{
			free (gra->data);
			free (gra->old);
			free (gra->flags);
			free (gra);
			v_rem (graphs, i);
			forced_refresh = 1;
			break;
		}
	}
}

void gr_wait (uint32_t ms)
{
	SDL_Delay (ms);
}

uint32_t gr_getms ()
{
	return SDL_GetTicks ();
}

#define GR_NEARX 15
#define GR_NEARY 10
int gra_nearedge (Graph gra, int yloc, int xloc)
{
	yloc -= gra->cy; xloc -= gra->cx;
	return (yloc <= GR_NEARY || yloc >= (gra->vh-GR_NEARY) ||
	        xloc <= GR_NEARX || xloc >= (gra->vw-GR_NEARX));
}

