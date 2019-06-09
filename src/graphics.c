/* graphics.c */

#include "include/graphics.h"
#include "include/vector.h"
#include "include/drawing.h"

#include "SDL.h"

#ifdef GR_DRAW_GPU
#include <GL/glew.h>
#endif

#if defined(SSE4_1)
#include <smmintrin.h>
#endif

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

static SDL_Window *sdlWindow;

#ifdef GR_DRAW_GPU
static SDL_GLContext glContext;
static GLuint glShader;
static GLuint grTiles, grData;
static uint32_t *gr_render_data;
static int gr_render_h, gr_render_w, gr_render_t;
static GLchar gr_shader_log[1024];
static GLint success;
#endif

#ifdef GR_DRAW_CPU
static SDL_Renderer *sdlRenderer;
static SDL_Texture *sdlTexture;
static Uint32 *gr_pixels;
static int gr_pitch;
static uint8_t *tiles_data;
#endif

/* all the active graphs */
static V_Graph graphs = NULL;

/* for text copying */
#define BUFFER_LEN 1024
static char temp_buffer[BUFFER_LEN];

/* timing parameters for held keys */
static uint32_t gr_kinitdelay = 180, gr_kdelay = 0;

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
static uint32_t debug_getch_time = 0;
#endif
#ifdef DEBUG_REFRESH_TIME
static uint32_t debug_ref_time = 0;
static uint32_t debug_ref_times[20];
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

/*struct outCol
{
	char a[8];
};*/
void grx_baddch (Graph gra, int buf, glyph gl)
{
#ifdef GR_DRAW_CPU
	/* what do you actually want to draw? */
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	/* update current to new */
	gra->data[buf] = gl;
#endif
#ifdef GR_DRAW_GPU
	if (!gl)
		return;
	int x = buf%gra->w;// + (gra->vpx/8) - gra->cpx/8;
	int y = (buf/gra->w)%gra->h;// + (gra->vpy/12) - gra->cpy/12;
	int z = 0;
	grx_mvaddch (gra, z, y, x, gl);
#endif
}

#if 0
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
#endif

/*void grx_mvaddchcol (Graph gra, int zloc, int yloc, int xloc, char ch, char bgr, char bgg, char bgb,
	char fgr, char fgg, char fgb)
{
	int x = xloc + (gra->vpx/8) - gra->cpx/8;
	int y = yloc + (gra->vpy/12) - gra->cpy/12;
	if (x >= 0 && x < 150 && y >= 0 && y < 50)
		*(struct outCol*)&gr_render_data[2*(x+150*y)] = (struct outCol)
			{{ch, bgr, bgg, bgb, fgr, fgg, fgb, 0}};
}*/

void grx_mvaddch (Graph gra, int zloc, int yloc, int xloc, glyph gl)
{
#ifdef GR_DRAW_GPU
	if (!gl)
		return;
	if (gl > 0 && gl < 256)
		gl |= gra->def & 0xFFFFFF00;
	int x = xloc + (gra->vpx/8) - gra->cpx/8;
	int y = yloc + (gra->vpy/12) - gra->cpy/12;
	int z = zloc - gra->cz;
	if (gra->t == 1)
		z = 7;
	if (x >= 0 && x < gr_render_w && y >= 0 && y < gr_render_h && z >= 0 && z < gr_render_t)
		gr_render_data[x+gr_render_w*(y + gr_render_h*z)] = gl;
	/*	*(struct outCol*)&gr_render_data[2*(x+150*y)] = (struct outCol){{
			gl&0xff,
			0x11*(0xf&(gl>>8)),  0x11*(0xf&(gl>>12)), 0x11*(0xf&(gl>>16)),
			0x11*(0xf&(gl>>20)), 0x11*(0xf&(gl>>24)), 0x11*(0xf&(gl>>28)),
			0}};*/
#endif
#ifdef GR_DRAW_CPU
	int buf = grx_index (gra, zloc, yloc, xloc);
	if (buf != -1)
		grx_baddch (gra, buf, gl);
#endif
}

void grx_mvaprint (Graph gra, int zloc, int yloc, int xloc, const char *str)
{
	int i, buf = grx_index (gra, zloc, yloc, xloc), len = strlen (str);

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
	int diag = st.err*2 > -st.dy && st.err*2 < st.dx;
	bres_iter (&st);
	while (!st.done)
	{
		if (diag)
			grx_mvaddch (gra, zloc, st.cy, st.cx, (st.sx == st.sy ? '\\' : '/') | gl);
		else
			grx_mvaddch (gra, zloc, st.cy, st.cx, (st.dx > st.dy ? ACS_HLINE : ACS_VLINE) | gl);
		diag = st.err*2 > -st.dy && st.err*2 < st.dx;
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

#ifdef GR_DRAW_CPU

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

#define ST(x) ((x)*0x11 + lighten*13) // seventeen
#define GL_TRD ST((gl&0xF0000000)>>28)
#define GL_TGN ST((gl&0x0F000000)>>24)
#define GL_TBL ST((gl&0x00F00000)>>20)
#define GL_BRD ST((gl&0x000F0000)>>16)
#define GL_BGN ST((gl&0x0000F000)>>12)
#define GL_BBL ST((gl&0x00000F00)>>8)
void blit_glyph (glyph gl, int py, int px, int glh, int glw, int lighten,
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

int min (int a, int b) { return a < b ? a : b; }
int max (int a, int b) { return a > b ? a : b; }

void blit_glyph_2d (glyph gl, int py, int px, int glh, int glw, int lighten)
{
	unsigned char ch = (unsigned char) gl;

	int y, x;
	int bitloc = ch*glh*glw;
	uint32_t R = GL_TRD, G = GL_TGN, B = GL_TBL;
	R = R > 255 ? 255 : R;
	G = G > 255 ? 255 : G;
	B = B > 255 ? 255 : B;
	uint32_t fgcol = PIXEL_VALUE (R, G, B);
	R = GL_BRD, G = GL_BGN, B = GL_BBL;
	R = R > 255 ? 255 : R;
	G = G > 255 ? 255 : G;
	B = B > 255 ? 255 : B;
	uint32_t bgcol = PIXEL_VALUE (R, G, B);
    int xlo = max(-px, 0);
    int xhi = min(glw, gr_pw - px);
    int ylo = max(-py, 0);
    int yhi = min(glh, gr_ph - py);
#if defined(SSE4_1)
    __m128i fg = _mm_set1_epi32(fgcol);
    __m128i bg = _mm_set1_epi32(bgcol);
    for (y = ylo; y < yhi; ++ y) {
        uint8_t *tiles_row = tiles_data + bitloc + glw * y;
        uint32_t *gr_row = gr_pixels + gr_pw * (py+y) + px;
        for (x = xlo; x+8 <= xhi; x += 8)
        {
            __m128i mask8 = _mm_loadl_epi64((void *) (& tiles_row [x]));
            __m128i mask32_lo = _mm_cvtepi8_epi32(mask8);
            __m128i mask32_hi = _mm_cvtepi8_epi32(_mm_srli_epi64(mask8, 32));
            __m128i data32_lo = _mm_blendv_epi8(bg, fg, mask32_lo);
            __m128i data32_hi = _mm_blendv_epi8(bg, fg, mask32_hi);
            _mm_storeu_si128((void *) (& gr_row [x]), data32_lo);
            _mm_storeu_si128((void *) (& gr_row [x + 4]), data32_hi);
        }
        for (; x < xhi; ++ x)
        {
            gr_row [x] = tiles_row [x] ? fgcol : bgcol;
        }
    }
#else
    for (y = ylo; y < yhi; ++ y) {
        uint8_t *tiles_row = tiles_data + bitloc + glw * y;
        uint32_t *gr_row = gr_pixels + gr_pw * (py+y) + px;
        for (x = xlo; x < xhi; ++ x)
        {
            gr_row [x] = tiles_row [x] ? fgcol : bgcol;
        }
    }
#endif
}

//static int total_reftime = 0, numrefs = -10;
void gr_refresh ()
{
	int i;
#ifdef DEBUG_REFRESH_TIME
//	debug_ref_time = gr_getms();
#endif
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
		z = gra->cz;
		if (z < 0)
			z = 0;
		for (; z < gra->cz+gra->ct && z < gra->t; ++ z, grx_c += dz)
			for (y = 0; y < gra->h; ++ y, grx_c += dy)
				for (x = 0; x < gra->w; ++ x, grx_c += dx)
		{
			int z = grx_c/gra->A, y = (grx_c%gra->A)/gra->w, x = grx_c%gra->w;
			if (z < 0 || z >= gra->t || y < 0 || y >= gra->h || x < 0 || x >= gra->w)
				continue;
			glyph gl = gra->data[grx_c];
			if (!gl)
				continue;
			if (gra->csr_state && grx_c == gra->csr_b)
				gl = 0x000FFF00 | (gl&0xFF);
			//if (z < gra->t-2 && z < gra->cz+gra->ct-1 && gra->data[grx_c + gra->A]) // could get rid of this?
			//	continue;
			int ipy = y*gra->glh - gra->cpy + (z-gra->cz-3)*gra->gldy;
			int ipx = x*gra->glw - gra->cpx + (z-gra->cz-3)*gra->gldx;
			if (ipy < -hmax || ipy >= gra->vph ||
				ipx < -wmax || ipx >= gra->vpw)
				continue;
			int py = ipy + gra->vpy;
			int px = ipx + gra->vpx;
			if (py < -hmax || py >= gr_ph ||
				px < -wmax || px >= gr_pw)
				continue;
			int lighten = 0;
			if ((gl&0x000FFF00) != 0x000FFF00)
				lighten = z - gra->cz;
			if (gra->t == 1)
				blit_glyph_2d (gl, py, px, gra->glh, gra->glw, 0);
			else
			{
				int ey, ex;
				ey = (gra->gldy < 0 && y > 0 && gra->data[grx_c-gra->w] == 0) ||
					(gra->gldy > 0 && y < gra->h-1 && gra->data[grx_c+gra->w] == 0);
				ex = (gra->gldx < 0 && x > 0 && gra->data[grx_c-1] == 0) ||
					(gra->gldx > 0 && x < gra->w-1 && gra->data[grx_c+1] == 0);
				int ed = (y && x && y < gra->h-1 && x < gra->w-1 && gra->data[grx_c+snx+sny*gra->w] == 0);
				if (ey || ex || ed)
					blit_glyph (gl, py, px, gra->glh, gra->glw, lighten, ey, ex, ed, gra->gldy, gra->gldx);
				else
					blit_glyph_2d (gl, py, px, gra->glh, gra->glw, lighten);
			}
			gflags f = gra->flags[grx_c];
			if (f)
				gr_drawboxes (py, px, gra->glh, gra->glw, f);
		}
#ifdef DEBUG_REFRESH_TIME
//		fprintf(stderr, "%ums ", gr_getms() - debug_ref_time);
#endif
	}

	//fprintf(stderr, "visible: %d\n", gr_c);
	//fprintf(stderr, "drawn: %d\n", drawn);
	if (gr_onrefresh)
		gr_onrefresh ();
#ifdef DEBUG_REFRESH_TIME
//	fprintf(stderr, "| onrefresh: %ums", gr_getms() - debug_ref_time);
#endif

	SDL_UpdateTexture (sdlTexture, NULL, gr_pixels, gr_pitch);
#ifdef DEBUG_REFRESH_TIME
//	fprintf(stderr, "| ud texture: %ums", gr_getms() - debug_ref_time);
#endif
#ifdef DEBUG_GETCH_TIME
	fprintf(stderr, "Time since last getch: %dms\n", gr_getms() - debug_getch_time);
#endif
	SDL_RenderClear (sdlRenderer);
	SDL_RenderCopy (sdlRenderer, sdlTexture, NULL, NULL);
	SDL_RenderPresent (sdlRenderer);

#ifdef DEBUG_REFRESH_TIME
	uint32_t time = gr_getms () - debug_ref_time;
	//fprintf(stderr, "| total: %ums\n", gr_getms() - debug_ref_time);
	if (time < 20)
		debug_ref_times[time]++;
	debug_ref_time = gr_getms ();
#endif
	//fprintf(stderr, "| total: %ums\n", gr_getms() - debug_ref_time);
	//if ((numrefs++) >= 0)
	//	total_reftime += gr_getms() - debug_ref_time;
	memclr (gr_pixels, gr_pitch * gr_ph);
}
#endif

#ifdef GR_DRAW_GPU
void gr_refresh ()
{
	//uint32_t asdf = gr_getms();
	glActiveTexture (GL_TEXTURE1);
	glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage3D (GL_TEXTURE_3D, 0, GL_RGBA, gr_render_w, gr_render_h, gr_render_t, 0, GL_RGBA, GL_UNSIGNED_BYTE, gr_render_data);
	//fprintf(stderr, "%d ", gr_getms()-asdf);

	/*glValidateProgram (glShader);
	glGetProgramiv (glShader, GL_VALIDATE_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog (glShader, sizeof(gr_shader_log), NULL, gr_shader_log);
		fprintf (stderr, "Invalid shader program: '%s'\n", gr_shader_log);
		exit (1);
	}*/
	//fprintf(stderr, "%d ", gr_getms()-asdf);

	glDrawArrays (GL_TRIANGLES, 0, 6);
	//fprintf(stderr, "%d ", gr_getms()-asdf);
	SDL_GL_SwapWindow (sdlWindow);
	//fprintf(stderr, "%d\n", gr_getms()-asdf);

#ifdef DEBUG_GETCH_TIME
	fprintf(stderr, "Time since last getch: %dms\n", gr_getms() - debug_getch_time);
#endif
#ifdef DEBUG_REFRESH_TIME
	uint32_t time = gr_getms() - debug_ref_time;
	//fprintf(stderr, "Time since last refresh: %dms\n", time);
	if (time < 20)
		debug_ref_times[time]++;
	debug_ref_time = gr_getms ();
#endif
}
#endif

void gr_clear ()
{
#ifdef GR_DRAW_GPU
	memset (gr_render_data, 0, gr_render_w*gr_render_h*gr_render_t*sizeof(*gr_render_data));
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
				debug_getch_time = ticks;
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
			debug_getch_time = ticks;
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
	
#ifdef GR_DRAW_CPU
	gr_pitch = sizeof (Uint32) * gr_pw;
	gr_pixels = realloc (gr_pixels, gr_pitch * gr_ph);
	memclr (gr_pixels, gr_pitch * gr_ph);
#endif

	if (gr_onresize)
		gr_onresize ();
}

#define GR_TRY_TILES(a,b)\
snprintf (filepath, 100, a, b);\
temp = SDL_LoadBMP (filepath);\
if (temp)\
	goto success;\
printf("Couldn't find tiles at %s.\n", filepath)

SDL_Surface *gr_load_file ()
{
	char filepath[100] = "";
	void *temp = NULL;

	GR_TRY_TILES("res/%s", TILE_FILE);
	GR_TRY_TILES("../res/%s", TILE_FILE);
	GR_TRY_TILES("%s", TILE_FILE);

	fprintf (stderr, "Error loading tileset: %s\n", SDL_GetError ());
	exit (1);
	
  success:;
	return temp;
}

#ifdef GR_DRAW_CPU
void gr_load_tiles ()
{
	SDL_Surface *temp = gr_load_file ();
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
			(* (uint32_t *) ((uint8_t *) tiles->pixels + tiles->pitch * y + 4*x) == gl_fgcolour ? -1 : 0);
	}
	if (SDL_MUSTLOCK (tiles))
		SDL_UnlockSurface (tiles);
	SDL_FreeSurface (tiles);
}
#endif
#ifdef GR_DRAW_GPU
void gr_load_tiles ()
{
	SDL_Surface *tiles = gr_load_file();

	glGenTextures (1, &grTiles);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, grTiles);

	int Mode = GL_RGB;

	if(tiles->format->BytesPerPixel == 4) {
		Mode = GL_RGBA;
	}

	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D (GL_TEXTURE_2D, 0, Mode, tiles->w, tiles->h, 0, Mode, GL_UNSIGNED_BYTE, tiles->pixels);
	SDL_FreeSurface (tiles);
}
#endif

void gr_cleanup ()
{
	SDL_StopTextInput();
#ifdef GR_DRAW_CPU
	SDL_DestroyRenderer (sdlRenderer);
#endif
#ifdef GR_DRAW_GPU
	SDL_GL_DeleteContext (glContext);
#endif
	SDL_DestroyWindow (sdlWindow);
	SDL_Quit ();
	//fprintf (stderr, "average refresh time: %fms, %dms over %d frames\n",
	//	((float)total_reftime)/numrefs, total_reftime, numrefs);
	int i, total = 0, sum = 0;
	for (i = 0; i < 20; ++ i)
	{
		if (debug_ref_times[i])
			printf ("%d ms: %u\n", i, debug_ref_times[i]);
		total += debug_ref_times[i];
		sum += debug_ref_times[i]*i;
	}
	printf ("Total frames: %d\n", total);
	printf ("Avg delay:    %f\n", sum/(float)total);
}

#ifdef GR_DRAW_GPU
static float vertices[6][2] =
{
	{ -1.f, -1.f },
	{  1.f, -1.f },
	{  1.f,  1.f },
	{  1.f,  1.f },
	{ -1.f,  1.f },
	{ -1.f, -1.f }
};

GLuint shader_init (const char *filename, GLenum sh_type)
{
	const GLchar *sh_src[1];
	char *sh_src_aux = malloc (10000);
	FILE *sh_file = fopen (filename, "r");
	size_t len = fread (sh_src_aux, 1, 10000, sh_file);
	fclose (sh_file);
	if (len == 10000)
	{
		fprintf (stderr, "Shader %s too large!\n", filename);
		exit (1);
	}
	else if (len == 0)
	{
		fprintf (stderr, "Shader %s empty!\n", filename);
		exit (1);
	}
	sh_src_aux[len-1] = 0;
	sh_src[0] = sh_src_aux;
	GLuint shader = glCreateShader (sh_type);
	glShaderSource (shader, 1, sh_src, NULL);
	glCompileShader (shader);
	glGetShaderiv (shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog (shader, sizeof(gr_shader_log), NULL, gr_shader_log);
		fprintf (stderr, "Error compiling shader (%s): %s\n", filename, gr_shader_log);
		exit (1);
	}
	free (sh_src_aux);
	return shader;
}
#endif

void gr_init (int ph, int pw)
{
	if (SDL_Init (SDL_INIT_VIDEO) < 0)
	{
		fprintf (stderr, "Error initialising SDL: %s\n", SDL_GetError ());
		exit (1);
	}

	atexit (gr_cleanup);
	sdlWindow = SDL_CreateWindow ("Yore", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		pw, ph,
#ifdef GR_DRAW_GPU
		SDL_WINDOW_OPENGL |
#endif
		0);
	if (sdlWindow == NULL)
	{
		fprintf (stderr, "SDL error: window is NULL\n");
		exit (1);
	}

#ifdef GR_DRAW_GPU
	glContext = SDL_GL_CreateContext (sdlWindow);
	if (glContext == NULL)
	{
		fprintf (stderr, "SDL error: GL context is NULL\n");
		exit (1);
	}

	glewInit ();
	SDL_GL_SetSwapInterval (0);

	/* triangles to render */
	GLuint vertex_buffer = 0;
	glGenBuffers (1, &vertex_buffer);
	glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData (GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnable (GL_TEXTURE_3D);

	gr_load_tiles ();
	glGenTextures (1, &grData);
	glActiveTexture (GL_TEXTURE1);
	glBindTexture (GL_TEXTURE_3D, grData);

	/* shader stuff */
	GLuint vertex_shader = shader_init ("shaders/shader.vert", GL_VERTEX_SHADER);
	GLuint fragment_shader = shader_init ("shaders/shader.frag", GL_FRAGMENT_SHADER);

	glShader = glCreateProgram ();
	glAttachShader (glShader, vertex_shader);
	glAttachShader (glShader, fragment_shader);

	glLinkProgram (glShader);
	glGetProgramiv (glShader, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog (glShader, sizeof(gr_shader_log), NULL, gr_shader_log);
		fprintf (stderr, "Error linking shaders: '%s'\n", gr_shader_log);
		exit (1);
	}

	glUseProgram (glShader);

	glUniform1i (glGetUniformLocation (glShader, "tiles"), 0);
	glUniform1i (glGetUniformLocation (glShader, "gr_data"), 1);

	GLint gl_pos_loc = glGetAttribLocation (glShader, "pos");
	glEnableVertexAttribArray (gl_pos_loc);
	glVertexAttribPointer (gl_pos_loc, 2, GL_FLOAT, GL_FALSE,
		sizeof(vertices[0]), (void*) 0);

	glClearColor (0,0,0,1);
	glClear (GL_COLOR_BUFFER_BIT);

	gr_render_w = pw/8+2;
	gr_render_h = ph/12+2;
	gr_render_t = 8;
	gr_render_data = malloc (gr_render_w*gr_render_h*gr_render_t*sizeof(*gr_render_data));
	gr_clear ();
#endif

#ifdef GR_DRAW_CPU
	sdlRenderer = SDL_CreateRenderer (sdlWindow, -1, SDL_RENDERER_ACCELERATED);// | SDL_RENDERER_PRESENTVSYNC);
	if (sdlRenderer == NULL)
	{
		fprintf (stderr, "SDL error: renderer is NULL\n");
		exit (1);
	}
	SDL_SetRenderDrawColor (sdlRenderer, 0, 0, 0, 255);
	SDL_RenderClear (sdlRenderer);
	SDL_SetRenderDrawBlendMode (sdlRenderer, SDL_BLENDMODE_NONE);
	sdlTexture = SDL_CreateTexture (sdlRenderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, pw, ph);
	gr_load_tiles ();
#endif
	SDL_StartTextInput ();

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

