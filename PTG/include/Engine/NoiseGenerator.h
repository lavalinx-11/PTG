#pragma once


class NoiseGenerator {
private:
	int seed;
	int octaves; // Number of noise layers to combine
	float baseFrequency; // Controls the frequency of the noise
	float amplitude; // Controls the amplitude (height) of the noise
	float lacunarity; // Frequency multiplier between octaves
	float persistence; // Amplitude multiplier between octaves

	float SingleNoise(float x, float y) const;
public:

	NoiseGenerator(int seed = 1337);
	float GetHeight(float x, float y) const;
	float SetFrequency(float freq) { baseFrequency = freq; };
	float SetAmplitude(float amp) { amplitude = amp; };
	void SetSeed(int s) { seed = s; };
	void SetOctaves(int o) { octaves = o; };
	void SetLacunarity(float l) { lacunarity = l; };
	void SetPersistence(float p) { persistence = p; };
};