/* graphics.c */

#include "include/graphics.h"
#include "include/vector.h"
#include "include/drawing.h"

#include "SDL.h"

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

/* Screen data */
static Uint32 *gr_pixels;
static int gr_pitch;

/* Tiles data */
static uint8_t *tiles_data;

/* SDL globals */
static SDL_Window *sdlWindow;
static SDL_Renderer *sdlRenderer;
static SDL_Texture *sdlTexture;

/* all the active graphs */
static V_Graph graphs = NULL;

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

void grx_movecam (Graph gra, int zloc, int yploc, int xploc, int ct)
{
	gra->cz = zloc;
	gra->cpy = yploc;
	gra->cpx = xploc;
	if (ct)
		gra->ct = ct;
}

void grx_centcam (Graph gra, int zloc, int yloc, int xloc)
{
	grx_movecam (gra, zloc + 1 - gra->ct, yloc*gra->glh - gra->vph/2,
		xloc*gra->glw - gra->vpw/2, 0);
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

#define PIXEL_VALUE(a,b,c) (((a)<<16) | ((b)<<8) | ((c)<<0) | 0xFF000000)
#define setpixel(y,x) {*(uint32_t*) ((uint8_t*) pixels + 4*(x) + gr_pitch*(y)) = col;}
void gr_drawboxes (int py, int px, int glh, int glw, gflags f)
{
	if (f == 0)
		return;

	int ismoving = 1 & (f>>12), isattacking = 1 & (f>>17), code, i;
	uint32_t col, *pixels;
	pixels = (uint32_t *) ((uintptr_t) gr_pixels + gr_pitch*py + px*4);

	///////////// MOVING
	if (ismoving == 0) goto end_moving;
	col = PIXEL_VALUE (0xFF,0xFF,0xFF);
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
	col = PIXEL_VALUE (0xFF,0x33,0x33);
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
end_attacking:;
}

#define ST(x) ((x)*0x11) // seventeen
#define GL_TRD ST((gl&0xF0000000)>>28)
#define GL_TGN ST((gl&0x0F000000)>>24)
#define GL_TBL ST((gl&0x00F00000)>>20)
#define GL_BRD ST((gl&0x000F0000)>>16)
#define GL_BGN ST((gl&0x0000F000)>>12)
#define GL_BBL ST((gl&0x00000F00)>>8)
void blit_glyph (glyph gl, int py, int px, int glh, int glw,
	int ey, int ex, int ed, int dy, int dx) /* edges: y, x, diagonal; directions: y, x */
{
	unsigned char ch = (unsigned char) gl;

	int y, x;
	int bitloc = ch*glh*glw;
	for (y = 0; y < glh; ++ y) for (x = 0; x < glw; ++ x, ++ bitloc)
	{
		if (px + x < 0 || px + x >= gr_pw ||
			py + y < 0 || py + y >= gr_ph)
			continue;
		int d1 = dy<0?y:dy>0?GLH-y-1:127;
		int D1 = ey?d1:127;
		int d2 = dx<0?x:dx>0?GLW-x-1:127;
		int D2 = ex?d2:127;
		int D3 = ed?d1+d2:127;
		int D = D1<D2?D1:
			D2 < D3?D2:D3;
		int g = D < 3 ? 20-8*D : 0;

		uint32_t R = GL_BRD, G = GL_BGN, B = GL_BBL;
		if (tiles_data [bitloc])
			R = GL_TRD, G = GL_TGN, B = GL_TBL;
		R = R+g > 255 ? 255 : R+g;
		G = G+g > 255 ? 255 : G+g;
		B = B+g > 255 ? 255 : B+g;
		gr_pixels [px + x + gr_pw * (py+y)] = PIXEL_VALUE (R, G, B);
	}
}

void blit_glyph_2d (glyph gl, int py, int px, int glh, int glw)
{
	unsigned char ch = (unsigned char) gl;

	int y, x;
	int bitloc = ch*glh*glw;
	uint32_t fgcol = PIXEL_VALUE (GL_TRD, GL_TGN, GL_TBL),
		bgcol = PIXEL_VALUE (GL_BRD, GL_BGN, GL_BBL);
	for (y = 0; y < glh; ++ y) for (x = 0; x < glw; ++ x, ++ bitloc)
	{
		if (px + x < 0 || px + x >= gr_pw ||
			py + y < 0 || py + y >= gr_ph)
			continue;

		if (tiles_data [bitloc])
			gr_pixels [px + x + gr_pw * (py+y)] = fgcol;
		else
			gr_pixels [px + x + gr_pw * (py+y)] = bgcol;
	}
}

//static int total_reftime = 0, numrefs = -10;
void gr_refresh ()
{
	int i;
	//uint32_t asdf = gr_getms();
	for (i = 0; i < graphs->len; ++ i)
	{
		Graph gra = graphs->data[i];
		if (!gra->vis)
			continue;

		// for calculating whether to draw a glyph
		int hmax = gra->glh + abs(gra->gldy);
		int wmax = gra->glw + abs(gra->gldx);

		// for correct drawing order
		// todo change so doesn't iterate over whole graph
		int grx_c;
		int z, y, x, dz, dy, dx;
		//int Y = gra->gldy < 0, X = gra->gldx < 0;
		//if (Y && X)
			dz = 0, dy = 0, dx = 1, grx_c = gra->cz * gra->A;
		/*else if (Y && (!X))
			dz = 0, dy = 2*gra->w, dx = -1, grx_c = gra->w - 1;
		else if ((!Y) && X)
			dz = 2*gra->A, dy = -2*gra->w, dx = 1, grx_c = gra->A - gra->w;
		else
			dz = 2*gra->A, dy = 0, dx = -1, grx_c = gra->A - 1;*/
		int sny = (gra->gldy > 0) - (gra->gldy < 0);
		int snx = (gra->gldx > 0) - (gra->gldx < 0);
		for (z = gra->cz; z < gra->cz+gra->ct && z < gra->t; ++ z, grx_c += dz)
			for (y = 0; y < gra->h; ++ y, grx_c += dy)
				for (x = 0; x < gra->w; ++ x, grx_c += dx)
		{
			int z = grx_c/gra->A, y = (grx_c%gra->A)/gra->w, x = grx_c%gra->w;
			if (z < 0 || z >= gra->t || y < 0 || y >= gra->h || x < 0 || x >= gra->w)
				continue;
			glyph gl = gra->data[grx_c];
			if (gra->csr_state && grx_c == gra->csr_b)
				gl = 0x000FFF00 | (gl&0xFF);
			if (!gl)
				continue;
			if (z < gra->t-2 && z < gra->cz+gra->ct-1 && gra->data[grx_c + gra->A]) // could get rid of this?
				continue;
			int ipy = y*gra->glh - gra->cpy + (z-gra->cz)*gra->gldy;
			int ipx = x*gra->glw - gra->cpx + (z-gra->cz)*gra->gldx;
			if (ipy < -hmax || ipy >= gra->vph ||
				ipx < -wmax || ipx >= gra->vpw)
				continue;
			int py = ipy + gra->vpy;
			int px = ipx + gra->vpx;
			if (py < -hmax || py >= gr_ph ||
				px < -wmax || px >= gr_pw)
				continue;
			if ((gl&0x000FFF00) != 0x000FFF00)
				gl += (z - gra->cz)*0x00011100;
			if (gra->t == 1)
				blit_glyph_2d (gl, py, px, gra->glh, gra->glw);
			else
			{
				int ey, ex;
				ey = (gra->gldy < 0 && y > 0 && gra->data[grx_c-gra->w] == 0) ||
					(gra->gldy > 0 && y < gra->h-1 && gra->data[grx_c+gra->w] == 0);
				ex = (gra->gldx < 0 && x > 0 && gra->data[grx_c-1] == 0) ||
					(gra->gldx > 0 && x < gra->w-1 && gra->data[grx_c+1] == 0);
				int ed = (y && x && y < gra->h-1 && x < gra->w-1 && gra->data[grx_c+snx+sny*gra->w] == 0);
				if (ey || ex || ed)
					blit_glyph (gl, py, px, gra->glh, gra->glw, ey, ex, ed, gra->gldy, gra->gldx);
				else
					blit_glyph_2d (gl, py, px, gra->glh, gra->glw);
			}
			/* TODO draw boxes: something like
				gflags f = gr_flags[gr_c];
				if (f)
					gr_drawboxes (y, x, h, f); */
		}
#ifdef DEBUG_REFRESH_TIME
		fprintf(stderr, "%ums ", gr_getms() - asdf);
#endif
	}

	//fprintf(stderr, "visible: %d\n", gr_c);
	//fprintf(stderr, "drawn: %d\n", drawn);
	if (gr_onrefresh)
		gr_onrefresh ();
#ifdef DEBUG_REFRESH_TIME
	fprintf(stderr, "| onrefresh: %ums", gr_getms() - asdf);
#endif

	SDL_UpdateTexture (sdlTexture, NULL, gr_pixels, gr_pitch);
#ifdef DEBUG_REFRESH_TIME
	fprintf(stderr, "| ud texture: %ums", gr_getms() - asdf);
#endif
	SDL_RenderClear (sdlRenderer);
	SDL_RenderCopy (sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent (sdlRenderer);

#ifdef DEBUG_REFRESH_TIME
	fprintf(stderr, "| total: %ums\n", gr_getms() - asdf);
#endif
	//fprintf(stderr, "| total: %ums\n", gr_getms() - asdf);
	//if ((numrefs++) >= 0)
	//	total_reftime += gr_getms() - asdf;
	memclr (gr_pixels, gr_pitch * gr_ph);
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
#ifdef FAKE_INPUT
static char fake_input[] = " L\rllllllllllllllllllllllllhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhllllllllllllllllllllllllllllllllllllllllllllllllllhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh S";
static int input_cur = 0;
#endif
char gr_getch_aux (int text, int tout_num, int get)
{
#ifdef FAKE_INPUT
	gr_refresh ();
	if (fake_input[input_cur])
		++ input_cur;
	return fake_input[input_cur-1];
#endif
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
//#include <math.h>
void gr_resize (int ph, int pw)
{
	gr_ph = ph;
	gr_pw = pw;
	
	gr_pitch = sizeof (Uint32) * gr_pw;
	gr_pixels = realloc (gr_pixels, gr_pitch * gr_ph);
	memclr (gr_pixels, gr_pitch * gr_ph);

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
	
  success:;
	SDL_Surface *tiles = SDL_ConvertSurfaceFormat (temp, SDL_GetWindowPixelFormat (sdlWindow), 0);
	SDL_FreeSurface (temp);
	uint32_t gl_fgcolour = SDL_MapRGB (tiles->format, 253, 253, 253);
	tiles_data = malloc (sizeof (*tiles_data) * 256 * GLW * GLH);
	memclr (tiles_data, sizeof (*tiles_data) * 256 * GLW * GLH);
	int i;
	if (SDL_MUSTLOCK (tiles))
		SDL_LockSurface (tiles);
	for (i = 0; i < 256 * GLW * GLH; ++ i)
	{
		unsigned char ch = i/(GLW*GLH);
		int y = (ch/16) * GLH + (i/GLW)%GLH;
		int x = (ch%16) * GLW + i%GLW;
		tiles_data[i] =
			(* (uint32_t *) ((uint8_t *) tiles->pixels + tiles->pitch * y + 4*x) == gl_fgcolour);
	}
	if (SDL_MUSTLOCK (tiles))
		SDL_UnlockSurface (tiles);
	SDL_FreeSurface (tiles);
}

void gr_cleanup ()
{
	SDL_StopTextInput();
	SDL_DestroyRenderer (sdlRenderer);
	SDL_DestroyWindow (sdlWindow);
	SDL_Quit ();
	//fprintf (stderr, "average refresh time: %fms, %dms over %d frames\n",
	//	((float)total_reftime)/numrefs, total_reftime, numrefs);
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
	sdlTexture = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, pw, ph);

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
		0, 0, 0, ct, // cam px loc stats
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
		if (gra == graphs->data[i])
		{
			free (gra->data);
			free (gra->flags);
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

