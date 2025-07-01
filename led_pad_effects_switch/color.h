#ifndef COLOR_H
#define COLOR_H

#include <math.h>

struct Color {
  int r;
  int g;
  int b;
};

struct ColorHSV {
  int h;
  int s;
  int v;
	float hF() { return mapf(h, 0, 360, 0, 1); }
	float sF() { return mapf(s, 0, 100, 0, 1); }
	float vF() { return mapf(s, 0, 100, 0, 1); }
};

Color hsvToRgb(float h, float s, float v) {
	float r = 0, g = 0, b = 0;
	
	int i = floor(h * 6);	
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);

	switch(i % 6){
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
	}

  Color color;
	color.r = round(r * 255);
	color.g = round(g * 255);
	color.b = round(b * 255);
  return color;
}

#endif