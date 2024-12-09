#pragma once
#include "ofMain.h"
#include "TerrainGenerator.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
    void keyPressed(int key);
    void generateMesh(const vector<vector<float>>& heightmap);
    void regenerateTerrain();
    void onRegeneratePressed();
    
private:
    // Core components
    TerrainGenerator terrainGen;
    ofVboMesh terrainMesh;
    ofEasyCam camera;
    ofShader terrainShader;
    
    // GUI elements
    ofxPanel gui;
    ofxFloatSlider baseSize;
    ofxFloatSlider noiseScale;
    ofxButton regenerateButton;
    
    // Display
    ofFbo preview2D;
    bool show3DView = false;
    bool showGui = true;
    
    // Height range for shader
    float lowestPoint;
    float highestPoint;
};
