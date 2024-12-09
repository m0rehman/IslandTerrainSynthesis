#pragma once
#include "ofMain.h"
#include "AreaShape.h"

class TerrainGenerator {
public:
    TerrainGenerator();
    
    void setup(int numVertices = 8, float baseRadius = 400.0f, float maxDistortion = 100.0f);
    vector<vector<float>> generateTerrain(int width, int height);
    void drawTerrain(int width, int height);
    void drawDebug(int width, int height);
    
    // Noise generation parameters
    int numOctaves = 4;
    float baseNoiseScale = 0.01;
    float baseAmplitude = 0.5;
    float biasStrength = 3.0;
    
    // Random offsets for noise variation
    float offsetX = ofRandom(1000);
    float offsetY = ofRandom(1000);
    
private:
    // Main shapes that define the terrain
    AreaShape islandShape;
    AreaShape mountainShape1;
    AreaShape mountainShape2;
    ofVec2f rangeDirection;
    
    // Cache for generated terrain
    vector<vector<float>> cachedHeightmap;
    bool needsUpdate;
    
    float calculateNoise(float x, float y);
    float calculateBias(float x, float y, float centerX, float centerY);
    float calculateMountainNoise(float x, float y);
};
