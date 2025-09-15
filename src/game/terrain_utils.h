#ifndef _TERRAIN_UTILS_H
#define _TERRAIN_UTILS_H
#include "core/mb.h"

extern unsigned char g_perlinPermutationTable[256];
extern float g_perlinGradientTable[256 * 3];

#define PERLIN_CLAMP(x) ((unsigned char)(x))
#define PERLIN_PERMUTATION(x) g_perlinPermutationTable[PERLIN_CLAMP(x)]
#define PERLIN_GRADIENT(x) &g_perlinGradientTable[3 * PERLIN_PERMUTATION(x)]

glm::vec4 perlin_octave(float frequency, float gain, int octaves_count, const glm::vec4& in);
glm::vec4 perlin(float factor, const glm::vec4& in);

#endif // !_TERRAIN_UTILS_H
