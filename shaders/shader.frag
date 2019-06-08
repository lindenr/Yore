#version 110
uniform sampler2D tiles;
uniform sampler3D gr_data;
const float pw = 1200.0, ph = 640.0;
const float GLW = 8.0, GLH = 12.0;
const float tw = float(int(pw/GLW)) + 2.0, th = float(int(ph/GLH)) + 2.0, tt = 8.0;
const float data_pw = tw*GLW, data_ph = th*GLH;

int byte (float f)
{
	return int(f*255.0 + 0.5);
}

float g_m (float f)
{
	int b = byte (f);
	return float(b/16)/15.0;
}

float g_l (float f)
{
	int b = byte (f);
	return float(b - 16*(b/16))/15.0;
}

void main ()
{
	vec4 v = vec4(0.0, 0.0, 0.0, 0.0);
	float px = gl_FragCoord.x, py = ph - gl_FragCoord.y;
	float offset = 0.5;
	float x, y, layer;
	int ch = 0, X, Y, XX, YY;
	for (layer = tt - 0.5; layer >= 0.0; layer -= 1.0, offset = layer)
	{
		x = gl_FragCoord.x + offset - 0.5;
		y = ph - gl_FragCoord.y + 2.0*(offset - 0.5);
		v = texture3D(gr_data, vec3(x/data_pw, y/data_ph, layer/tt));
		if (v.x != 0.0)
			break;
	}
	if (layer < 0.0)
	{
		gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);
		return;
	}
	ch = int(v.x*256.0);
	X = int(x);
	Y = int(y);
	XX = X - int(GLW)*(X/int(GLW));
	YY = Y - int(GLH)*(Y/int(GLH));
	//ch = X/8 + 16*(Y/12);
	//int chX = 40;
	//int chY = 72;
	int chX = int(GLW)*(ch - 16*(ch/16));
	int chY = int(GLH)*(ch/16);
	bool fg = texture2D(tiles, vec2((float(XX+chX)+0.5)/GLW/16.0, (float(YY+chY)+0.5)/GLH/16.0)).y != 0.0;
	vec3 fgcol = vec3(g_m(v.w), g_l(v.w), g_m(v.z));
	vec3 bgcol = vec3(g_l(v.z), g_m(v.y), g_l(v.y));
	bool L1 = texture3D(gr_data, vec3((x-1.0)/data_pw, y/data_ph, layer/tt)).x == 0.0;
	bool L2 = L1 && XX == 0;
	bool U1 = texture3D(gr_data, vec3(x/data_pw, (y-1.0)/data_ph, layer/tt)).x == 0.0;
	bool U2 = U1 && YY == 0;
	bool UL1 = texture3D(gr_data, vec3((x-1.0)/data_pw, (y-1.0)/data_ph, layer/tt)).x == 0.0;
	bool UL2 = UL1 && XX == 0 && YY == 0;
	float height_bonus = offset * 13.0 / 255.0 +
		float(offset > 1.0) * (10.0/255.0) * (float(L1||U1||UL1) + float(L2||U2||UL2));
	vec3 extra1 = vec3(height_bonus, height_bonus, height_bonus);
	vec3 col;
	if (fg)
		col = fgcol;
	else
		col = bgcol;
	gl_FragColor = vec4(col + extra1, 1.0);
}

