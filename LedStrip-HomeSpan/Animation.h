#ifndef ANIMATION_H
#define ANIMATION_H

#include <cmath>
#include <FastLED.h>


CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v);

struct Animation {
    virtual void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) = 0;
    virtual Animation* clone() const = 0;
    virtual ~Animation() {};
};

namespace Animations {

struct BeatingRed: Animation {
    void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) override {
        for (uint16_t i = 0; i < led_count; i++) {
            colors[i] = CRGB{ (uint8_t) (time_ms * 255 / 1000), 0, 0};
        }
    }

    Animation* clone() const {
        return new BeatingRed(*this);
    }
};

struct StaticColor: Animation {
    CRGB color;

    StaticColor(CRGB color): color(color) {}

    void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) override {
        for (uint16_t i = 0; i < led_count; i++) {
            colors[i] = color;
        }
    }

    Animation* clone() const {
        return new StaticColor(*this);
    }
};

struct Rainbow: Animation {
    void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) override {
        for(uint16_t i = 0; i < led_count; i++) {
            float value = (time_ms >> 7) + (i << 0);
            value = value * 359 / 256;
            colors[i] = hsvToRgb(((uint32_t) value) % 360, 255, 255);
        }
    }

    Animation* clone() const {
        return new Rainbow(*this);
    }
};

struct Connecting: Animation {
    void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) override {
        float c = 40.0f; // wave celerity in led/s
        int wavelength = 40; // wavelength in leds;
        float k = 2.0 * PI / wavelength; // magnitude of the wave vector
        float pulsation = 2 * PI * c / wavelength; // wave pulsation in rad/s

        float t = time_ms/1000.0f; // time elapsed in seconds
        float half_cycle_time = led_count / c;
        // make sure t is in the time interval of a valid cycle
        while (t > 2 * half_cycle_time) t -= 2 * half_cycle_time;
        // bouncing effect when half a cycle is reached
        if (t > half_cycle_time) {
            t = 2 * half_cycle_time - t;
        }

        for(uint16_t x = 0; x < led_count; x++) {
            float waveStart = -PI/(k*c) + x/c;
            float waveEnd = PI/(k*c) + x/c;

            if (t > waveStart && t < waveEnd) {
                float value = std::cos(pulsation * t - k * x) / 2.0f + 0.5f;
                colors[x] = CRGB{ (uint8_t) (value * 255), 0, 0 };
            } else {
                colors[x] = CRGB::Black;
            }
        }
    }

    Animation* clone() const {
        return new Connecting(*this);
    }
};

struct ConnectionSuccess: Animation {
    void render(uint32_t time_ms, CRGB *colors, uint16_t led_count) override {
        CRGB col = CRGB::Black;
        if (time_ms < 1000) {
            col = CRGB{0, (uint8_t) (time_ms * 255 / 1000), 0 };
        }
        else if (time_ms < 2000) {
            col = CRGB{0, (uint8_t) ((2000 - time_ms) * 255 / 1000), 0 };
        }

        for(uint16_t i = 0; i < led_count; i++) {
            colors[i] = col;
        }
    }

    Animation* clone() const {
        return new ConnectionSuccess(*this);
    }
};

}

// Converts a color from HSV to RGB.
// h is hue, as a number between 0 and 360.
// s is the saturation, as a number between 0 and 255.
// v is the value, as a number between 0 and 255.
CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return { r, g, b };
}

#endif

