#include "trig.h"

s32 atan2(s32 dy, s32 dx) {
    s32 coeff_1 = inttof32(DEGREES_IN_CIRCLE>>3);
    s32 coeff_2 = 3 * coeff_1;
    s32 abs_dy = abs(dy) + 1; // kludge to prevent 0/0 condition

    s32 angle;
    if (dx >= 0) {
        s32 r = divf32((dx - abs_dy), (dx + abs_dy));
        angle = coeff_1 - mulf32(coeff_1, r);
    } else {
        s32 r = divf32((dx + abs_dy), (abs_dy - dx));
        angle = coeff_2 - mulf32(coeff_1, r);
    }
    if (dy < 0) {
        angle = -angle; //negate if in quad III or IV
    }

    angle = f32toint(angle) + (DEGREES_IN_CIRCLE>>2);
    if (angle < 0) {
        angle += DEGREES_IN_CIRCLE;
    }
    return angle;
}
