#include "pico/stdlib.h"
#include "libraries/pico_graphics/pico_graphics.hpp"
#include "libraries/gfx_pack/gfx_pack.hpp"
#include <time.h>

extern "C" {
	#include "../vpet/vpet.h"
	void vpet_init();
	void vpet_run();

	void vpet_tick_second();
	void vpet_tick_animation();
	void vpet_tick_button_press();
	void vpet_tick_60fps();
}
using namespace pimoroni;

// ----------------------------------------------------------------------------

extern uint8_t vpet_screen_buffer[PET_SCREEN_W / 8][PET_SCREEN_H];
uint8_t vpet_screen_buffer_copy[PET_SCREEN_W / 8][PET_SCREEN_H]; // Used for detecting when an update is required

uint16_t key_down = 0, key_new = 0, key_last = 0, key_new_or_repeat = 0;
int key_repeat = 0;
extern int second_ticks;

const uint8_t strawberry_yogurt[] = {
0, 0, 0, 0, 0, 0, 0, 1, 240, 30, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 14, 48, 37, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 49, 192, 241, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 31, 206, 1, 216, 64, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 16, 48, 1, 254, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 208, 0, 251, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 32, 3, 255, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 24, 64, 3, 238, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 255, 247, 128, 3, 120, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 127, 255, 255, 224, 1, 224, 31, 224, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 255, 255, 0, 7, 255, 248, 0, 0, 0, 0, 0, 0, 0, 7, 255, 255, 255, 255, 224, 31, 255, 255, 128, 0, 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 255, 255, 255, 255, 252, 0, 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 255, 255, 255, 255, 252, 0, 0, 0, 0, 0, 0, 31, 255, 255, 255, 255, 255, 255, 255, 255, 254, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 31, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 48, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 40, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 200, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 206, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 1, 1, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 246, 127, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 1, 191, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 255, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 253, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 239, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 0, 0, 0, 0, 0, 127, 31, 0, 0, 0, 0, 0, 0, 0, 0, 254, 2, 0, 0, 0, 0, 126, 31, 0, 0, 0, 0, 0, 0, 0, 0, 254, 5, 0, 0, 0, 0, 122, 31, 0, 0, 0, 0, 0, 0, 0, 0, 254, 57, 0, 0, 0, 0, 62, 63, 0, 0, 0, 0, 0, 0, 0, 0, 254, 64, 128, 0, 0, 0, 28, 63, 0, 0, 0, 0, 0, 0, 0, 0, 252, 72, 64, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 252, 60, 32, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, 0, 252, 117, 144, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 248, 254, 224, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 248, 255, 128, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 240, 191, 128, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 240, 251, 128, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 248, 255, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 248, 254, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 252, 108, 0, 0, 0, 0, 0, 255, 0, 0, 0, 0, 0, 0, 0, 0, 252, 120, 0, 0, 0, 0, 0, 127, 255, 255, 255, 255, 255, 255, 255, 255, 252, 48, 0, 0, 0, 0, 0, 127, 255, 255, 255, 255, 255, 255, 255, 255, 248, 0, 0, 0, 0, 0, 0, 63, 255, 255, 255, 255, 255, 255, 255, 255, 248, 0, 0, 0, 0, 0, 0, 15, 255, 255, 255, 255, 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 3, 143, 255, 255, 255, 255, 255, 255, 255, 240, 0, 0, 0, 0, 0, 0, 0, 3, 255, 255, 255, 0, 15, 255, 248, 0, 0, 0, 0, 0, 0, 0, 0, 0, 63, 255, 252, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 255, 224, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// ----------------------------------------------------------------------------


ST7567 st7567(128, 64, GfxPack::gfx_pack_pins);
PicoGraphics_Pen1Bit graphics(st7567.width, st7567.height, nullptr);
RGBLED backlight_rgb(GfxPack::BL_R, GfxPack::BL_G, GfxPack::BL_B, Polarity::ACTIVE_HIGH);
Button button_a(GfxPack::A);
Button button_b(GfxPack::B);
Button button_c(GfxPack::C);
Button button_d(GfxPack::D);
Button button_e(GfxPack::E);

// HSV Conversion expects float inputs in the range of 0.00-1.00 for each channel
// Outputs are rgb in the range 0-255 for each channel
void from_hsv(float h, float s, float v, uint8_t &r, uint8_t &g, uint8_t &b) {
  float i = floor(h * 6.0f);
  float f = h * 6.0f - i;
  v *= 255.0f;
  uint8_t p = v * (1.0f - s);
  uint8_t q = v * (1.0f - f * s);
  uint8_t t = v * (1.0f - (1.0f - f) * s);

  switch (int(i) % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
  }
}

int main() {
	sleep_ms(100);

	st7567.set_backlight(64); // 0 to 255
	memset(vpet_screen_buffer, 0, sizeof(vpet_screen_buffer));
	memset(vpet_screen_buffer_copy, 0, sizeof(vpet_screen_buffer_copy));

	vpet_init();
	RandomSeed();

	float hue = 0.0f;
	bool screen_needs_update = true;
	time_t last_time = time(NULL);
	while(true) {  
		//graphics.set_pen(0);
		//graphics.clear();

		uint8_t *fb = (uint8_t *)graphics.frame_buffer;
		memcpy(fb, strawberry_yogurt, sizeof(strawberry_yogurt));

		// --------------------------------------------------------------
		// Run virtual pet for one frame

		key_last = key_down;
		key_down = 0;
		if(button_a.raw())
			key_down |= VPET_KEY_PREV;
		if(button_b.raw())
			key_down |= VPET_KEY_NEXT;
		if(button_c.raw())
			key_down |= VPET_KEY_C;
		if(button_d.raw())
			key_down |= VPET_KEY_B;
		if(button_e.raw())
			key_down |= VPET_KEY_A;
		key_new = key_down & (~key_last);
		key_new_or_repeat = key_new;

		if((key_down&(VPET_KEY_PREV|VPET_KEY_NEXT)) ==
		(key_last&(VPET_KEY_PREV|VPET_KEY_NEXT)) ) {
			key_repeat++;
			if(key_repeat > 15) {
				key_repeat = 12;
				key_new_or_repeat |= key_down & (VPET_KEY_PREV|VPET_KEY_NEXT);
			}
		} else {
			key_repeat = 0;
		}

		// --------------------------------------------------------------
		// Run for one tick

		// Tick from button presses
		if(key_new_or_repeat) {
			vpet_tick_button_press();
		}

		// Tick every frame
		vpet_tick_60fps();

		// Tick very half second or full second
		second_ticks++;
		if(second_ticks == 30 || second_ticks == 60)
			vpet_tick_animation();
		if(second_ticks >= 60) {
			second_ticks = 0;
		}
		time_t now = time(NULL);
		if(now != last_time) {
			vpet_tick_second();
			last_time = now;
		}

		// --------------------------------------------------------------

		for(int y=0; y<PET_SCREEN_H; y++) {
			for(int x=0; x<PET_SCREEN_W/8; x++) {
				if(vpet_screen_buffer_copy[x][y] != vpet_screen_buffer[x][y]) {
					screen_needs_update = true;
					fb[(128/8)*(y+16) + x+4] = vpet_screen_buffer[x][y];
				}
			}
		}

		// Update screen screen
		backlight_rgb.set_rgb(255, 255, 255);
		//backlight_rgb.set_hsv(hue, 1.0f, 1.0f);
		//hue += 0.002;
		if(screen_needs_update) {
			st7567.update(&graphics);
			screen_needs_update = false;
		}
		sleep_ms(1000/60);
	}
	return 0;
}
