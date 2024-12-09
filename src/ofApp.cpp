#include "ofApp.h"

void ofApp::setup() {
    ofSetWindowShape(1600, 1600);
    
    // Setup GUI
    gui.setup("Island Parameters");
    gui.setPosition(ofGetWidth() - 220, 20);
    gui.add(baseSize.setup("Base Size", 1200, 1000, 1600));
    gui.add(noiseScale.setup("Noise Scale", 0.007, 0.005, 0.01));
    gui.add(regenerateButton.setup("Regenerate"));
    regenerateButton.addListener(this, &ofApp::onRegeneratePressed);
    
    // Setup shader and preview buffer
    terrainShader.load("shader/shader");
    preview2D.allocate(1600, 1600, GL_RGBA);
    
    // Setup camera
    camera.setDistance(2000);
    camera.setNearClip(1);
    camera.setFarClip(10000);
    
    // Generate initial terrain
    regenerateTerrain();
}

void ofApp::regenerateTerrain() {
    // Calculate distortion based on island size (300:1600 ratio)
    float distortion = (baseSize/1600.0f) * 300.0f;
    
    // Generate new terrain
    terrainGen.offsetX = ofRandom(1000); // New random offset each time (noise function can't be seeded)
    terrainGen.offsetY = ofRandom(1000);
    terrainGen.setup(8, baseSize, distortion);
    terrainGen.baseNoiseScale = noiseScale;
    
    // Create 3D mesh and 2D preview
    vector<vector<float>> heightmap = terrainGen.generateTerrain(1600, 1600);
    generateMesh(heightmap);
    
    // Cache 2D preview
    preview2D.begin();
        ofClear(0, 0, 0, 255);
        terrainGen.drawTerrain(ofGetWidth(), ofGetHeight());
        terrainGen.drawDebug(ofGetWidth(), ofGetHeight());
    preview2D.end();
}

void ofApp::onRegeneratePressed() {
    regenerateTerrain();
}

void ofApp::update() {
}

void ofApp::draw() {
    if (show3DView) {
        ofEnableDepthTest();
        
        camera.begin();
        terrainShader.begin();
        terrainShader.setUniform3f("localUp", 0, 0, 1);
        terrainShader.setUniform1f("maxHeight", highestPoint);
        terrainShader.setUniform1f("minHeight", lowestPoint);
        
        terrainMesh.draw();
        terrainShader.end();
        camera.end();
        
        ofDisableDepthTest();
    } else {
        preview2D.draw(0, 0);
    }
    
    // Draw UI
    if (showGui) {
        gui.draw();
        ofSetColor(255);
        ofDrawBitmapString("space: Toggle 3D\ng: Toggle UI", 20, 30);
    }

}

void ofApp::keyPressed(int key) {
    if (key == ' ') {
        show3DView = !show3DView;
    } else if (key == 'g') {
        showGui = !showGui;
    }
}

void ofApp::generateMesh(const vector<vector<float>>& heightmap) {
    int gridHeight = heightmap.size();
    int gridWidth = heightmap[0].size();
    float terrainHeight = 80.0f;  // Vertical scale of terrain
    
    // Clear and setup mesh
    terrainMesh.clear();
    terrainMesh.setMode(OF_PRIMITIVE_TRIANGLES);
    
    // Dimensions for centering the terrain
    float halfWidth = gridWidth / 2.0f;
    float halfHeight = gridHeight / 2.0f;
    
    // Track height range for shader
    lowestPoint = FLT_MAX;
    highestPoint = FLT_MIN;
    
    // Generate terrain grid
    for (int row = 0; row < gridHeight - 1; row++) {
        for (int col = 0; col < gridWidth - 1; col++) {
            // Convert grid position to world space (-1 to 1 range)
            float worldX = (col / (float)gridWidth) * 2 - 1;
            float worldY = (row / (float)gridHeight) * 2 - 1;
            float nextWorldX = ((col + 1) / (float)gridWidth) * 2 - 1;
            float nextWorldY = ((row + 1) / (float)gridHeight) * 2 - 1;
            
            // Get heights for current quad (apply power curve)
            float heightTopLeft = pow(heightmap[row][col], 1.2f) * terrainHeight;
            float heightTopRight = pow(heightmap[row][col + 1], 1.2f) * terrainHeight;
            float heightBottomLeft = pow(heightmap[row + 1][col], 1.2f) * terrainHeight;
            float heightBottomRight = pow(heightmap[row + 1][col + 1], 1.2f) * terrainHeight;
            
            // Track height range
            lowestPoint = min(lowestPoint, min(min(heightTopLeft, heightTopRight),
                                             min(heightBottomLeft, heightBottomRight)));
            highestPoint = max(highestPoint, max(max(heightTopLeft, heightTopRight),
                                               max(heightBottomLeft, heightBottomRight)));
            
            // Create vertices for current quad
            ofVec3f topLeft(worldX * halfWidth, worldY * halfHeight, heightTopLeft);
            ofVec3f topRight(nextWorldX * halfWidth, worldY * halfHeight, heightTopRight);
            ofVec3f bottomLeft(worldX * halfWidth, nextWorldY * halfHeight, heightBottomLeft);
            ofVec3f bottomRight(nextWorldX * halfWidth, nextWorldY * halfHeight, heightBottomRight);
            
            // Calculate normals for lighting
            ofVec3f normal1 = (topRight - topLeft).cross(bottomLeft - topLeft);
            normal1.normalize();
            ofVec3f normal2 = (bottomRight - topRight).cross(bottomLeft - topRight);
            normal2.normalize();
            ofVec3f quadNormal = normal1 + normal2;
            quadNormal.normalize();
            
            // First triangle (top-left, top-right, bottom-left)
            terrainMesh.addVertex(topLeft);
            terrainMesh.addVertex(topRight);
            terrainMesh.addVertex(bottomLeft);
            
            terrainMesh.addNormal(quadNormal);
            terrainMesh.addNormal(quadNormal);
            terrainMesh.addNormal(quadNormal);
            
            // Second triangle (top-right, bottom-right, bottom-left)
            terrainMesh.addVertex(topRight);
            terrainMesh.addVertex(bottomRight);
            terrainMesh.addVertex(bottomLeft);
            
            terrainMesh.addNormal(quadNormal);
            terrainMesh.addNormal(quadNormal);
            terrainMesh.addNormal(quadNormal);
        }
    }
}
