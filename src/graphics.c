/* graphics.c */

#include "include/graphics.h"
#include "include/vector.h"
#include "include/drawing.h"

#include <stdio.h>
#include <stdarg.h>

#define XSTRINGIFY(x) STRINGIFY(x)
#define STRINGIFY(x) #x
#define TILE_FILE "t"XSTRINGIFY(GLW)"x"XSTRINGIFY(GLH)".bmp"

/* window dimensions (in pixels) */
int gr_ph = 0, gr_pw = 0;

/* event callback functions */
void (*gr_onidle) () = NULL;
void (*gr_onresize) () = NULL;
void (*gr_onrefresh) () = NULL;
void (*gr_quit) () = NULL;

/* The following static variables are for internal use */

/* SDL globals */
static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;
static SDL_Surface *tiles, *screen, *glyph_col;
static SDL_Texture *sdlTexture;

/* all the active graphs */
static Vector graphs = NULL;

/* for text copying */
#define BUFFER_LEN 1024
static char temp_buffer[BUFFER_LEN];

/* timing parameters for held keys */
static uint32_t gr_kinitdelay = 180, gr_kdelay = 40;

/* state for keys being held down */
static uint32_t key_fire_ms = 0;
static char cur_key_down = 0;
static int num_keys_down = 0;

/* state for key timeout */
static uint32_t end = 0;

/* state for animation-skipping */
static char peeked = GRK_EOF;
static int gr_skip_anim = 0;

#ifdef DEBUG_GETCH_TIME
static uint32_t lastref = 0;
#endif

int grx_index (Graph gra, int zloc, int yloc, int xloc)
{
	if (zloc < 0 || zloc >= gra->t ||
		yloc < 0 || yloc >= gra->h ||
		xloc < 0 || xloc >= gra->w)
		return -1;
	return gra->A * zloc + gra->w * yloc + xloc;
}

void gr_ext (glyph *out, char *in, glyph def)
{
	int i;
	for (i = 0; in[i]; ++ i)
		out[i] = in[i] | (def & 0xFFFFFF00);
	out[i] = 0;
}

void grx_movecam (Graph gra, int zloc, int yloc, int xloc, int ct)
{
	gra->cz = zloc;
	gra->cy = yloc;
	gra->cx = xloc;
	if (ct)
		gra->ct = ct;
}

void grx_centcam (Graph gra, int zloc, int yloc, int xloc)
{
	grx_movecam (gra, zloc + 1 - gra->ct, yloc - (gra->vph/gra->glh)/2,
		xloc - (gra->vpw/gra->glw)/2, gra->ct);
}

void grx_baddch (Graph gra, int buf, glyph gl)
{
	/* what do you actually want to draw? */
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	/* update current to new */
	gra->data[buf] = gl;
}

void grx_bgaddch (Graph gra, int buf, glyph gl)
{
	/* what do you actually want to draw? */
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	if ((gl&COL_BG_MASK) == 0)
		gl |= gra->data[buf] & COL_BG_MASK;
	/* update current to new */
	gra->data[buf] = gl;
}

void grx_mvaddch (Graph gra, int zloc, int yloc, int xloc, glyph gl)
{
	int buf = grx_index (gra, zloc, yloc, xloc);
	if (buf != -1)
		grx_baddch (gra, buf, gl);
}

void grx_mvaprint (Graph gra, int zloc, int yloc, int xloc, const char *str)
{
	int i, buf = grx_index (gra, zloc, yloc, xloc), len = strlen(str);

	for (i = 0; i < len && i+buf > 0 && i+buf < gra->v; ++ i)
		grx_baddch (gra, i+buf, str[i]);
}

void grx_mvprint (Graph gra, int zloc, int yloc, int xloc, const char *str, ...)
{
	va_list args;

	va_start (args, str);
	vsnprintf (temp_buffer, BUFFER_LEN, str, args);
	va_end (args);

	grx_mvaprint (gra, zloc, yloc, xloc, temp_buffer);
}

void grx_cprint (Graph gra, int zloc, int yloc, const char *str, ...)
{
	va_list args;

	va_start (args, str);
	int len = vsnprintf (temp_buffer, BUFFER_LEN, str, args);
	va_end (args);

	int xloc = (gra->w - len)/2;
	grx_mvaprint (gra, zloc, yloc, xloc, temp_buffer);
}

void grx_mvaprintex (Graph gra, int zloc, int yloc, int xloc, const glyph *str)
{
	int i, buf = grx_index (gra, zloc, yloc, xloc);

	for (i = 0; str[i] && i+buf > 0 && i+buf < gra->v; ++ i)
		grx_baddch (gra, i+buf, str[i]);
}

#include "include/rand.h"
void grx_drawline (Graph gra, int zloc, int fy, int fx, int ty, int tx, glyph gl)
{
	struct BresState st;
	bres_init (&st, fy, fx, ty, tx);
	bres_iter (&st);
	while (!st.done)
	{
		if (st.err*2 > -st.dy && st.err*2 < st.dx)
			grx_mvaddch (gra, zloc, st.cy, st.cx, (st.sx == st.sy ? '\\' : '/') | gl);
		else
			grx_mvaddch (gra, zloc, st.cy, st.cx, (st.dx > st.dy ? ACS_HLINE : ACS_VLINE) | gl);
		bres_iter (&st);
	}
}

void grx_drawdisc (Graph gra, int z, int y, int x, int rad, glyph gl)
{
	int i, j;
	int weight = rad * (rad + 1);
	for (i = -rad; i <= rad; ++ i) for (j = -rad; j <= rad; ++ j)
	{
		if (i*i + j*j > weight)
			continue;
		int sat = i*i + j*j;
		int drop = (sat*4) / weight;
		grx_mvaddch (gra, z, j + y, i + x, gl | COL_BG_BLUE (10 + rn(2) + rn(2) + rn(2) - drop));
	}
}

void grx_box_aux (Graph gra, int zloc, int yloc, int xloc, int height, int width,
	glyph ULCORNER, glyph URCORNER, glyph LLCORNER, glyph LRCORNER,
	glyph HLINE, glyph VLINE)
{
	int xt = width - 1, yt = height - 1;
	grx_mvaddch (gra, zloc, yloc, xloc, ULCORNER);
	grx_mvaddch (gra, zloc, yloc + height, xloc, LLCORNER);
	grx_mvaddch (gra, zloc, yloc, xloc + width, URCORNER);
	grx_mvaddch (gra, zloc, yloc + height, xloc + width, LRCORNER);
	while (xt--)
	{
		grx_mvaddch (gra, zloc, yloc, xloc + xt + 1, HLINE);
		grx_mvaddch (gra, zloc, yloc + height, xloc + xt + 1, HLINE);
	}
	while (yt--)
	{
		grx_mvaddch (gra, zloc, yloc + yt + 1, xloc, VLINE);
		grx_mvaddch (gra, zloc, yloc + yt + 1, xloc + width, VLINE);
	}
}

void grx_box (Graph gra, int zloc, int yloc, int xloc, int height, int width)
{
	grx_box_aux (gra, zloc, yloc, xloc, height, width,
		ACS_ULCORNER, ACS_URCORNER, ACS_LLCORNER, ACS_LRCORNER,
		ACS_HLINE, ACS_VLINE);
}

void grx_dbox (Graph gra, int zloc, int yloc, int xloc, int height, int width)
{
	grx_box_aux (gra, zloc, yloc, xloc, height, width,
		DCS_ULCORNER, DCS_URCORNER, DCS_LLCORNER, DCS_LRCORNER,
		DCS_HLINE, DCS_VLINE);
}

void grx_fbox (Graph gra, int zloc, int yloc, int xloc, int height, int width, glyph gl)
{
	int x, y;
	for (x = 1; x < width; ++x)
		for (y = 1; y < height; ++y)
			grx_mvaddch (gra, zloc, yloc + y, xloc + x, gl);
	grx_box (gra, zloc, yloc, xloc, height, width);
}

void grx_csolid (Graph gra)
{
	gra->csr_state = 3;
}

void grx_cblink (Graph gra)
{
	gra->csr_state = 1;
}

void grx_cmove (Graph gra, int zloc, int yloc, int xloc)
{
	int b = grx_index (gra, zloc, yloc, xloc);
	if (b != -1)
		gra->csr_b = b;
}

void grx_chide (Graph gra)
{
	gra->csr_state = 2;
}

void grx_cshow (Graph gra)
{
	gra->csr_state = 1;
}

void grx_hide (Graph gra)
{
	gra->vis = 0;
}

void grx_show (Graph gra)
{
	gra->vis = 1;
}

#define setpixel(y,x) {*(uint32_t*) ((uint8_t*) pixels + 4*(x) + screen->pitch*(y)) = col;}
void gr_drawboxes (int py, int px, int glh, int glw, gflags f)
{
	if (f == 0)
		return;

	if (SDL_MUSTLOCK (screen)) // TODO draw everything under one lock
		SDL_LockSurface (screen);

	int ismoving = 1 & (f>>12), isattacking = 1 & (f>>17), code, i;
	uint32_t col, *pixels;
	pixels = (uint32_t *) ((uintptr_t) screen->pixels + py*screen->pitch + px*4);

	///////////// MOVING
	if (ismoving == 0) goto end_moving;
	col = SDL_MapRGB (screen->format, 0xFF,0xFF,0xFF);
	code = (f>>8)&15;
	switch (code)
	{
	case 1: for (i = 0;       i < glw;   ++ i) setpixel(0,     i); break;
	case 2: for (i = 1+glw/2; i < glw;   ++ i) setpixel(0,     i);
			for (i = 0;       i < glh/2; ++ i) setpixel(i, glw-1); break;
	case 5: for (i = 0;       i < glh;   ++ i) setpixel(i, glw-1); break;
	case 8: for (i = 1+glw/2; i < glw;   ++ i) setpixel(glh-1, i);
			for (i = 1+glh/2; i < glh;   ++ i) setpixel(i, glw-1); break;
	case 7: for (i = 0;       i < glw;   ++ i) setpixel(glh-1, i); break;
	case 6: for (i = 0;       i < glw/2; ++ i) setpixel(glh-1, i);
			for (i = 1+glh/2; i < glh;   ++ i) setpixel(i,     0); break;
	case 3: for (i = 0;       i < glh;   ++ i) setpixel(i,     0); break;
	case 0: for (i = 0;       i < glw/2; ++ i) setpixel(0,     i);
			for (i = 0;       i < glh/2; ++ i) setpixel(i,     0); break;
	}
end_moving:

	///////////// ATTACKING
	if (isattacking == 0) goto end_attacking;
	col = SDL_MapRGB (screen->format, 0xFF,0x33,0x33);
	code = (f>>13)&15;
	switch (code)
	{
	case 1: for (i = 0;       i < glw;   ++ i) setpixel(0,     i); break;
	case 2: for (i = 1+glw/2; i < glw;   ++ i) setpixel(0,     i);
			for (i = 0;       i < glh/2; ++ i) setpixel(i, glw-1); break;
	case 5: for (i = 0;       i < glh;   ++ i) setpixel(i, glw-1); break;
	case 8: for (i = 1+glw/2; i < glw;   ++ i) setpixel(glh-1, i);
			for (i = 1+glh/2; i < glh;   ++ i) setpixel(i, glw-1); break;
	case 7: for (i = 0;       i < glw;   ++ i) setpixel(glh-1, i); break;
	case 6: for (i = 0;       i < glw/2; ++ i) setpixel(glh-1, i);
			for (i = 1+glh/2; i < glh;   ++ i) setpixel(i,     0); break;
	case 3: for (i = 0;       i < glh;   ++ i) setpixel(i,     0); break;
	case 0: for (i = 0;       i < glw/2; ++ i) setpixel(0,     i);
			for (i = 0;       i < glh/2; ++ i) setpixel(i,     0); break;
	}
end_attacking:

	if (SDL_MUSTLOCK (screen))
		SDL_UnlockSurface (screen);
}

#define ST(x) ((x)*0x11) // seventeen
#define GL_TRD ST((gl&0xF0000000)>>28)
#define GL_TGN ST((gl&0x0F000000)>>24)
#define GL_TBL ST((gl&0x00F00000)>>20)
#define GL_BRD ST((gl&0x000F0000)>>16)
#define GL_BGN ST((gl&0x0000F000)>>12)
#define GL_BBL ST((gl&0x00000F00)>>8)
//#define BGS(y,x,h,w,c) do {bgsrect = (SDL_Rect) {(GLW+gr_xspace)*xloc - HX*z - gr_xspace + (x), (GLH+gr_yspace)*yloc - HY*z - gr_yspace + (y), (w), (h)}; SDL_FillRect (screen, &bgsrect, SDL_MapRGB (screen->format, (c), (c), (c)));} while (0)
void blit_glyph (glyph gl, int py, int px, int glh, int glw,
	int ey, int ex, int ed, int dy, int dx) /* edges: y, x, diagonal; directions: y, x */
{
	unsigned char ch = (unsigned char) gl;
	SDL_Rect srcrect = {glw*(ch&15), glh*((ch>>4)&15), glw, glh};
	SDL_Rect dstrect = {px, py, glw, glh};
	//SDL_Rect bgsrect;

	if (SDL_MUSTLOCK (glyph_col)) // TODO draw everything under one lock
		SDL_LockSurface (glyph_col);
	int y, x;
	for (y = 0; y < glh; ++ y) for (x = 0; x < glw; ++ x)
	{
		int d1 = dy<0?y:dy>0?GLH-y:127;
		int D1 = ey?d1:127;
		int d2 = dx<0?x:dx>0?GLW-x:127;
		int D2 = ex?d2:127;
		int D3 = ed?d1+d2:127;
		int D = D1<D2?D1:
			D2 < D3?D2:D3;
		int g = D < 3 ? 20-8*D : 0;
		Uint8 R = GL_BRD+g > 255 ? 255 : GL_BRD+g;
		Uint8 G = GL_BGN+g > 255 ? 255 : GL_BGN+g;
		Uint8 B = GL_BBL+g > 255 ? 255 : GL_BBL+g;
		*(uint32_t*) ((uint8_t*) glyph_col->pixels + 4*(x) + glyph_col->pitch*(y)) =
			SDL_MapRGB (glyph_col->format, R, G, B);
	}
	if (SDL_MUSTLOCK (glyph_col))
		SDL_UnlockSurface (glyph_col);
	//SDL_FillRect (glyph_col, NULL, SDL_MapRGB (glyph_col->format, GL_BRD, GL_BGN, GL_BBL));
	SDL_BlitSurface (tiles, &srcrect, glyph_col, NULL);

	//BGS (0, 0, GLH+2*gr_xspace, GLW+2*gr_yspace, 0);
	SDL_FillRect (screen, &dstrect, SDL_MapRGB (screen->format, GL_TRD, GL_TGN, GL_TBL));
	SDL_BlitSurface (glyph_col, NULL, screen, &dstrect);
}

void gr_refresh ()
{
	int i;

#ifdef DEBUG_REFRESH_TIME
	uint32_t asdf = gr_getms();
	//fprintf(stderr, "time: %ums\n", (asdf=gr_getms()));
#endif
	SDL_FillRect (screen, NULL, SDL_MapRGB (screen->format, 0, 0, 0));
	for (i = 0; i < graphs->len; ++ i)
	{
		Graph gra = * (Graph *) v_at (graphs, i);
		if (!gra->vis)
			continue;

		int grx_c;
		int hmax = gra->glh + abs(gra->gldy);
		int wmax = gra->glw + abs(gra->gldx);
		for (grx_c = 0; grx_c < gra->v; ++ grx_c) // TODO better
		{
			glyph gl = gra->data[grx_c];
			if (gra->csr_state && grx_c == gra->csr_b)
			{
				gl = 0x000FFF00 | (gl&0xFF);
			}
			if (!gl)
				continue;
			int z = grx_c/gra->A, y = (grx_c%gra->A)/gra->w, x = grx_c%gra->w;
			if (z < gra->t-1 && gra->data[grx_c + gra->A])
				continue;
			if (z < gra->cz || z >= gra->cz + gra->ct)
				continue;
			int ipy = (y-gra->cy)*gra->glh + (z-gra->cz)*gra->gldy;
			int ipx = (x-gra->cx)*gra->glw + (z-gra->cz)*gra->gldx;
			if (ipy < -hmax || ipy >= gra->vph ||
				ipx < -wmax || ipx >= gra->vpw)
				continue;
			int py = ipy + gra->vpy;
			int px = ipx + gra->vpx;
			if (py < -hmax || py >= gr_ph ||
				px < -wmax || px >= gr_pw)
				continue;
			int ey = (y && gra->data[grx_c-gra->w] == 0);
			int ex = (x && gra->data[grx_c-1] == 0);
			int ed = (y && x && gra->data[grx_c-1-gra->w] == 0);
			SDL_Rect dstrect = {px - gra->gldx, py - gra->gldy, gra->glw, gra->glh};
			SDL_FillRect (screen, &dstrect, SDL_MapRGB (screen->format, 0, 0, 0));
			blit_glyph (gl, py, px, gra->glh, gra->glw, ey, ex, ed, gra->gldy, gra->gldx);
			/* TODO draw boxes: something like
				gflags f = gr_flags[gr_c];
				if (f)
					gr_drawboxes (y, x, h, f); */
		}
	}

	//fprintf(stderr, "visible: %d\n", gr_c);
	//fprintf(stderr, "drawn: %d\n", drawn);
	if (gr_onrefresh)
		gr_onrefresh ();

	SDL_UpdateTexture (sdlTexture, NULL, screen->pixels, gr_pw * sizeof(Uint32));
	SDL_RenderCopy (sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent (sdlRenderer);

#ifdef DEBUG_REFRESH_TIME
	fprintf(stderr, "time: %ums\n\n", gr_getms() - asdf);
#endif
}

void grx_clear (Graph gra)
{
	int i;
	if (!gra)
		return;
	
	for (i = 0; i < gra->v; ++ i)
		grx_baddch (gra, i, 0);
}

glyph grx_glinvert (Graph gra, glyph gl)
{
	if (gl == 0)
		gl = gra->def & 0xFFFFFF00;
	return ((gl << 12)&0xFFF00000) | ((gl >> 12)&0x000FFF00) | (gl&0xFF);
}

void grx_invert (Graph gra, int zloc, int yloc, int xloc)
{
	int i = grx_index (gra, zloc, yloc, xloc);
	gra->data[i] = grx_glinvert (gra, gra->data[i]);
}

int gr_inputcode (SDL_Keycode code)
{
	return (code >= 32 && code < 128);
}

int gr_inputch (char in)
{
	return (in >= 32 && in < 128);
}

// TODO: is text parameter necessary? Could just check if it is an input code
char gr_getch_aux (int text, int tout_num, int get)
{
	uint32_t ticks = gr_getms ();
#ifdef DEBUG_GETCH_TIME
	fprintf(stderr, "Time since last getch: %dms\n", ticks - lastref);
#endif
	gr_refresh ();

	if (peeked != GRK_EOF)
	{
		char ret = peeked;
		if (get)
		{
			peeked = GRK_EOF;
			gr_skip_anim = 0;
		}
		return ret;
	}

	if (tout_num > 0 && end <= ticks)
		end = tout_num + ticks;
	else if (tout_num <= 0)
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
			if (tout_num < 0)
				return GRK_EOF;
			if (cur_key_down && ticks >= key_fire_ms)
			{
				key_fire_ms = ticks + gr_kdelay;
				#ifdef DEBUG_GETCH_TIME
				lastref = ticks;
				#endif
				return cur_key_down;
			}
			if (gr_onidle)
				gr_onidle ();
			gr_wait (10, 0);
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
					input_key = GRK_LT;
				else if (code == SDLK_RIGHT)
					input_key = GRK_RT;
				else if (code == SDLK_RETURN)
					input_key = GRK_RET;
				else if (code == SDLK_BACKSPACE)
					input_key = GRK_BS;
				else if (code == SDLK_ESCAPE)
					input_key = GRK_ESC;
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
			ticks = gr_getms ();
			key_fire_ms = ticks + gr_kinitdelay;
			#ifdef DEBUG_GETCH_TIME
			lastref = ticks;
			#endif
			if (!get)
				peeked = input_key;
			return input_key;
		}
	}
	return GRK_EOF;
}

char gr_getch ()
{
	return gr_getch_aux (0, 0, 1);
}

char gr_getch_text ()
{
	return gr_getch_aux (1, 0, 1);
}

char gr_getch_int (int t)
{
	return gr_getch_aux (0, t, 1);
}

void grx_getstr (Graph gra, int zloc, int yloc, int xloc, char *out, int len)
{
	int i = 0;
	gra->csr_state = 2;
	while (1)
	{
		grx_cmove (gra, zloc, yloc, xloc);
		char in = gr_getch_text ();
		if (in == GRK_RET) break;
		else if (in == GRK_BS)
		{
			if (i)
			{
				-- xloc;
				-- i;
			}
			out[i] = 0;
			grx_mvaddch (gra, zloc, yloc, xloc, ' ');
			continue;
		}
		/* watershed - put non-input-char handling above here */
		else if (!gr_inputch (in)) continue;
		else if (i < len-1)
		{
			grx_mvaddch (gra, zloc, yloc, xloc, in);
			out[i] = in;
			++ xloc;
			++ i;
		}
	}
	gra->csr_state = 0;
	out[i] = 0;
}
#include <math.h>
void gr_resize (int ph, int pw)
{
	//int i;
	//SDL_FreeSurface (screen);
	//screen = SDL_SetVideoMode (xsiz, ysiz, 32, SDL_SWSURFACE | SDL_RESIZABLE);

	gr_ph = ph;
	gr_pw = pw;
	
	if (gr_onresize)
		gr_onresize ();
}

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
	SDL_SetColorKey (tiles, SDL_TRUE, SDL_MapRGB (tiles->format, 253, 0, 253));
}

void gr_cleanup ()
{
	SDL_StopTextInput();
	SDL_DestroyRenderer (sdlRenderer);
	SDL_DestroyWindow (sdlWindow);
	SDL_Quit ();
}

void gr_init (int ph, int pw)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error initialising SDL: %s\n", SDL_GetError ());
		exit (1);
	}

	atexit (gr_cleanup);
	sdlWindow = SDL_CreateWindow ("Yore", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		pw, ph, 0);
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
	sdlTexture = SDL_CreateTexture (sdlRenderer, SDL_GetWindowPixelFormat (sdlWindow), SDL_TEXTUREACCESS_STREAMING, pw, ph);
	screen = SDL_CreateRGBSurface (0, pw, ph, 32, 0, 0, 0, 0);
	glyph_col = SDL_CreateRGBSurface (0, GLW, GLH, 32, 0, 0, 0, 0);
	SDL_SetColorKey (glyph_col, SDL_TRUE, SDL_MapRGB (glyph_col->format, 253, 253, 253));

	gr_load_tiles ();
	gr_resize (ph, pw);
}

Graph grx_init (int t, int h, int w,
	int glh, int glw, int gldy, int gldx,
	int vpy, int vpx, int vph, int vpw, int ct)
{
	Graph gra;
	int v = t*h*w;

	glyph *data = malloc (sizeof(glyph) * v);
	gflags *flags = malloc (sizeof(gflags) * v);

	memset (data, 0, sizeof(glyph) * v);
	memset (flags, 0, sizeof(gflags) * v);
	
	gra = malloc (sizeof(struct Graph));
	*gra = (struct Graph)
	{
		t, h, w, h*w, v, // internal glyph stats
		glh, glw, gldy, gldx, // glyph display stats
		data, /*old,*/ flags, // internal data
		0, 0, 0, ct, // cam glyph loc stats
		vpy, vpx, vph, vpw, // view pixel stats
		1, // graph visibility
		0, 0, // cursor location and state
		COL_TXT_DEF // default glyph
	};

	if (!graphs)
		graphs = v_dinit (sizeof(Graph));
	v_push (graphs, &gra);
	
	return gra;
}

Graph gra_cinit (int h, int w)
{
	return grx_init (1, h, w, GLH, GLW, 0, 0,
		(gr_ph - h*GLH)/2, (gr_pw - w*GLW)/2, h*GLH, w*GLW, 1);
}

Graph gra_init (int h, int w, int vy, int vx, int vh, int vw)
{
	return grx_init (1, h, w, GLH, GLW, 0, 0,
		vy*GLH, vx*GLW, vh*GLH, vw*GLW, 1);
}

void grx_free (Graph gra)
{
	if (!graphs)
		return;
	
	int i;
	for (i = 0; i < graphs->len; ++ i)
	{
		if (gra == *(Graph*)v_at(graphs, i))
		{
			free (gra->data);
			free (gra);
			v_rem (graphs, i);
			break;
		}
	}
}

char gr_wait (uint32_t ms, int interrupt)
{
	if (!ms)
		return GRK_EOF;
	if (!interrupt)
	{
		SDL_Delay (ms);
		return GRK_EOF;
	}
	if (gr_skip_anim)
	{
		/* don't want to hang */
		SDL_Delay (1);
		return GRK_EOF;
	}
	char out = gr_getch_aux (0, ms, 0);
	if (out != GRK_EOF)
		gr_skip_anim = 1;
	return out;
}

uint32_t gr_getms ()
{
	return SDL_GetTicks ();
}

#define GR_NEARX 15
#define GR_NEARY 10
int grx_nearedge (Graph gra, int yloc, int xloc)
{
	/*yloc -= gra->cy; xloc -= gra->cx;
	return (yloc <= GR_NEARY || yloc >= (gra->vh-GR_NEARY) ||
	        xloc <= GR_NEARX || xloc >= (gra->vw-GR_NEARX));*/
	return 0; // TODO
}

