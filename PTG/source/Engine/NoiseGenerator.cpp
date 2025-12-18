#include "Engine/NoiseGenerator.h"
#include <cmath>
NoiseGenerator::NoiseGenerator(int seed_)
    : seed(seed_)
    , octaves(5)
    , baseFrequency(0.09f)
    , lacunarity(2.0f)
    , persistence(0.6f)
    , amplitude(3.3f)
{}

// Hash function to generate pseudo-random values
static float Hash(int x, int y, int seed) { 
	int h = x * 374761393 + y * 668265263 + seed * 31;
    h = (h ^ (h >> 13)) * 1274126177;
    return (h & 0x7fffffff) / float(0x7fffffff);
}

// Basic value noise
float NoiseGenerator::SingleNoise(float x, float z) const { 
    int xi = static_cast<int>(std::floor(x));
    int zi = static_cast<int>(std::floor(z));

    float n = Hash(xi, zi, seed);
    return n * 2.0f - 1.0f; // [-1, 1]
}

// Fractal Brownian Motion 
float NoiseGenerator::GetHeight(float x, float z) const { 
    float height = 0.0f;
    float frequency = baseFrequency;
    float amplitudeAcc = 1.0f;

    for (int i = 0; i < octaves; i++) {
        height += SingleNoise(x * frequency, z * frequency) * amplitudeAcc;

        frequency *= lacunarity;
        amplitudeAcc *= persistence;
    }
   
    return height * amplitude;
}