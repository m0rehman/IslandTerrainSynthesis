#include "TerrainGenerator.h"

TerrainGenerator::TerrainGenerator() {}

void TerrainGenerator::setup(int numVertices, float baseRadius, float maxDistortion) {
    // Set up main island shape
    islandShape.setup(numVertices, baseRadius * 0.45f, maxDistortion * 1.2, ofVec2f(0,0));
    
    // Create random direction for mountain range
    float rangeAngle = ofRandom(TWO_PI);
    rangeDirection = ofVec2f(cos(rangeAngle), sin(rangeAngle));
    
    // Position mountain ranges along the direction
    float distance1 = ofRandom(baseRadius * 0.01, baseRadius * 0.1);
    float distance2 = ofRandom(baseRadius * 0.05, baseRadius * 0.15);
    
    ofVec2f pos1 = rangeDirection * distance1;
    ofVec2f pos2 = rangeDirection * -distance2;
    
    // Set up mountain shapes
    mountainShape1.setup(12, baseRadius * 0.2083f, maxDistortion * 0.25, pos1);
    mountainShape2.setup(6, baseRadius * 0.15f, maxDistortion * 0.1333f, pos2);
    needsUpdate = true;
}

vector<vector<float>> TerrainGenerator::generateTerrain(int width, int height) {
    vector<vector<float>> heightmap(height, vector<float>(width, 0.0f));
    float centerX = width / 2.0;
    float centerY = height / 2.0;
    
    // Generate terrain height for each point
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float shapeX = x - centerX;
            float shapeY = y - centerY;
            
            // Generate base terrain
            float noiseValue = calculateNoise(x, y);
            float bias = calculateBias(shapeX, shapeY, centerX, centerY);
            float adjustedBias = (bias * 1.2f) - 0.1f;
            float baseHeight = noiseValue * pow(ofClamp(adjustedBias, 0.0f, 1.0f), biasStrength);
            
            // Add mountain ranges
            float mountainBias = max(
                mountainShape1.getDistanceFieldValue(shapeX, shapeY),
                mountainShape2.getDistanceFieldValue(shapeX, shapeY)
            );
            float adjustedMountainBias = mountainBias * 1.5f;
            float mountainHeight = calculateMountainNoise(x, y) *
                                 pow(ofClamp(adjustedMountainBias, 0.0f, 1.0f), 2.0);
            
            // Combine base terrain and mountains
            float combinedHeight = baseHeight + (mountainHeight * 0.8f);
            heightmap[y][x] = (adjustedBias > -0.1f) ? combinedHeight : 0.0f;
        }
    }
    return heightmap;
}

float TerrainGenerator::calculateNoise(float x, float y) {
    float totalNoise = 0.0;
    float amplitude = baseAmplitude * 0.3;
    float noiseScale = baseNoiseScale;
    
    // Add multiple octaves of noise for detail
    for (int octave = 0; octave < 6; octave++) {
        float noiseValue = ofNoise((x + offsetX) * noiseScale,
                                 (y + offsetY) * noiseScale) * amplitude;
        totalNoise += noiseValue;
        noiseScale *= 2.0;
        amplitude *= 0.5;
    }
    
    // Adjust noise and add center boost for island shape
    float noise = (totalNoise * 0.45f) + 0.35f;
    
    float centerX = ofGetWidth() / 2.0;
    float centerY = ofGetHeight() / 2.0;
    float dx = (x - centerX) / centerX;
    float dy = (y - centerY) / centerY;
    float distFromCenter = sqrt(dx*dx + dy*dy);
    float centerBoost = ofMap(distFromCenter, 0, 1, 0.15f, 0, true);
    
    return noise + centerBoost;
}

float TerrainGenerator::calculateMountainNoise(float x, float y) {
    float totalNoise = 0.0;
    float amplitude = baseAmplitude * 0.9;
    float noiseScale = baseNoiseScale;
    
    // Calculate gradient for ridge direction
    float dx = islandShape.getDistanceFieldValue(x + 1, y) - islandShape.getDistanceFieldValue(x - 1, y);
    float dy = islandShape.getDistanceFieldValue(x, y + 1) - islandShape.getDistanceFieldValue(x, y - 1);
    
    // Get perpendicular vector for ridge direction
    ofVec2f perpDir(-dy, dx);
    perpDir.normalize();
    
    // Create ridge pattern using gradient projection
    float ridgeVal = (x * perpDir.x + y * perpDir.y) * 0.5;
    float ridge = abs(sin(ridgeVal));
    
    // Add noise variation to the ridges
    for (int octave = 0; octave < 8; octave++) {
        float noiseValue = ofNoise(x * noiseScale, y * noiseScale) * amplitude;
        totalNoise += noiseValue;
        noiseScale *= 2.0;
        amplitude *= 0.5;
    }
    
    // Blend noise and ridges based on mountain proximity
    float mountainInfluence = max(
        mountainShape1.getDistanceFieldValue(x, y),
        mountainShape2.getDistanceFieldValue(x, y)
    );
    
    return totalNoise * (1.0 - mountainInfluence) + ridge * mountainInfluence * 2;
}

float TerrainGenerator::calculateBias(float x, float y, float centerX, float centerY) {
    return islandShape.getDistanceFieldValue(x, y);
}

void TerrainGenerator::drawTerrain(int width, int height) {
    // Update cached heightmap if needed
    if(needsUpdate) {
        cachedHeightmap = generateTerrain(width, height);
        needsUpdate = false;
    }
    
    // Draw heightmap as grayscale image
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float value = cachedHeightmap[y][x];
            if (value * 255 > 255) {
                ofSetColor(255);
            } else {
                ofSetColor(value * 255);
            }
            ofDrawRectangle(x, y, 1, 1);
        }
    }
}

void TerrainGenerator::drawDebug(int width, int height) {
    ofPushMatrix();
    ofTranslate(width/2, height/2);
    
    // Draw shapes for debugging
    ofSetColor(255);
    islandShape.draw();
    
    ofSetColor(255, 0, 0);
    mountainShape1.draw();
    mountainShape2.draw();
    
    ofPopMatrix();
}
