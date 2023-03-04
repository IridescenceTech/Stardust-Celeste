#pragma once
#include <cmath>

namespace Stardust_Celeste::Math {

    inline float toRadians(float degrees) {
        return degrees * 3.14159f / 180.0f;
    }

    inline float toDegrees(float radians) {
        return radians * 180.0f / 3.14159f;
    }

    inline int sign(float value) {
        return (value > 0) - (value < 0);
    }

    inline float _min(float value, float minimum) {
		return (value < minimum) ? minimum : value;
	}

	inline float _max(float value, float maximum) {
		return (value > maximum) ? maximum : value;
	}

	inline float clamp(float value, float minimum, float maximum) {
		return (value > minimum) ? (value < maximum) ? value : maximum : minimum;
	}

    inline float lerp(float a, float b, float f) {
        return a + f * (b - a);
    }

    inline float smoothstep(float edge0, float edge1, float x) {
        x = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
        return x * x * (3 - 2 * x);
    }
}