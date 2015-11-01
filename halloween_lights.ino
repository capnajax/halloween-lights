#include <Adafruit_NeoPixel.h>

#include <stdlib.h>

#define NUM_LIGHTS 8
#define EYE_WIDTH 3
#define EYE_SPACE 0
#define NUM_LEDS 240

#define DECISION_INTERVAL 250

#define WINK_PROBABILITY 2
#define BLINK_PROBABILITY 10
#define MAX_RED_BRIGHTNESS 128
#define MIN_RED_BRIGHTNESS 96
#define RED_PROBABILITY 10
#define RED_DURATION 5

// the blue-green eyes
#define MAX_BLUE 110
#define MIN_BLUE 30
#define MAX_GREEN 192
#define MIN_GREEN 0
#define MAX_RED 45
#define MIN_RED 0

int lights[] = {47, 72, 100, 120, 139, 160, 190, 218};

uint32_t last_decision,
		 colors[NUM_LIGHTS];

uint8_t redTime = 0;

bool winking[NUM_LIGHTS][2];


Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, 5, NEO_GRB + NEO_KHZ800);


uint8_t randInRange(uint8_t max, uint8_t min) {
	return ( rand() % (max-min) ) + min;
}

uint32_t setColor(int light) {
	
	// choose some random colour between blue and green with saturation up to 50% and brightness between MAX_BRIGHTNESS and MIN_BRIGHTNESS
	uint8_t r = randInRange(MAX_RED, MIN_RED),
			g = randInRange(MAX_GREEN, MIN_GREEN),
			b = randInRange(MAX_BLUE, MIN_BLUE);

	colors[light] = strip.Color(r, g, b);

}

void normalPattern() {
	uint32_t eyeColor[2];
	for(int light = 0; light < NUM_LIGHTS; light++) {
		for(int eye = 0; eye < 2; eye++) {
			eyeColor[eye] = winking[light][eye] ? 0 : colors[light];
		}
		for(int i = 0; i < EYE_WIDTH; i++) {
			strip.setPixelColor(lights[light] + i, eyeColor[0]);
			strip.setPixelColor(lights[light] + EYE_WIDTH + EYE_SPACE + i, eyeColor[1]);
		}
	}
	strip.show();
}

void blacken() {
	for(int i = 0; i < NUM_LEDS; i++) {
		strip.setPixelColor(i, 0, 0, 0);
	}
	strip.show();
}

void redden() {
	for(int light = 0; light < NUM_LIGHTS; light++) {
		uint8_t brightness = rand() % (MAX_RED_BRIGHTNESS - MIN_RED_BRIGHTNESS) + MIN_RED_BRIGHTNESS;
		for(int i = 0; i < EYE_WIDTH; i++) {
			strip.setPixelColor(lights[light] + i, brightness, 0, 0);
			strip.setPixelColor(lights[light] + EYE_WIDTH + EYE_SPACE + i, brightness, 0, 0);
		}
	}
	strip.show();
}

void setup() {

	strip.begin();
	strip.show();

}

void loop() {

	delay(DECISION_INTERVAL);
	redTime++;

	// if already is red, give the eyes a color.
	if(redTime == RED_DURATION) {
		// at the end of the red interval -- set the LEDs to black to put some space between the red and the colors.
		blacken();
	} else if(redTime == RED_DURATION+1) {
		// now set the colors.
		for(int i = 0; i < NUM_LIGHTS; i++) {
			setColor(i);
		}
	} else if(redTime < RED_DURATION) {
		if(1 == redTime) {
			// because when red was set to zero, we actually blackened to put some space between the red and the black.
			redden();
		}
		// I don't need to continue, we're in the midst of red time, so, just wait for the next cycle.
		return;
	}

	// cancel all blinks and winks
	for(int i = 0; i < NUM_LIGHTS; i++) {
		winking[i][0] = false;
		winking[i][1] = false;
	}

	// should I go red?
	if((rand()&0xFF) < RED_PROBABILITY) {
		redTime = 0;
		blacken();
		return;
	}

	// if we make it this far, we're in the normal patten and the eye colors have been determined. Now
	// we make the eyes wink and blink.

	// set up winks and blinks

	for(int light = 0; light < NUM_LIGHTS; light++) {

		if((rand()&0xFF) < BLINK_PROBABILITY) {
			winking[light][0] = true;
			winking[light][1] = true;

		} else {

			for(int i = 0; i < 1; i++) {
				winking[light][i] = ((rand()&0xFF) < WINK_PROBABILITY);
			}

		}
	}

	normalPattern();
}
