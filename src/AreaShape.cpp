#include "AreaShape.h"

AreaShape::AreaShape() : minRadius(0), maxRadius(0), offset(0,0) {}

void AreaShape::setup(int numVertices, float baseRadius, float maxDistortion, ofVec2f offset) {
    vertices.clear();
    this->offset = offset;
    minRadius = baseRadius;
    maxRadius = baseRadius;
    
    // Create vertices in a circle, applying random distortion to each
    for (int i = 0; i < numVertices; i++) {
        float angle = (TWO_PI * i) / numVertices;
        float distortion = ofRandom(-maxDistortion, maxDistortion);
        float radius = baseRadius + distortion;
        
        // Store vertex position relative to center
        float x = cos(angle) * radius;
        float y = sin(angle) * radius;
        vertices.push_back(ofVec2f(x, y));
        
        // Keep track of radius range
        minRadius = min(minRadius, radius);
        maxRadius = max(maxRadius, radius);
    }
    
    // Close the shape by duplicating first vertex
    vertices.push_back(vertices[0]);
}

ofVec2f AreaShape::getVertex(int index) {
    if(index >= 0 && index < vertices.size() - 1) {
        return vertices[index];
    }
    return ofVec2f(0,0);
}

float AreaShape::getDistanceFieldValue(float x, float y) {
    // Convert to local coordinates
    x -= offset.x;
    y -= offset.y;
    
    float angle = atan2(y, x);
    if (angle < 0) angle += TWO_PI;
    
    // Compare actual distance to expected distance at this angle
    float expectedRadius = getInterpolatedRadius(angle);
    float actualRadius = sqrt(x*x + y*y);
    
    float normalizedDist = 1.0 - (actualRadius / expectedRadius);
    return ofClamp(normalizedDist, 0.0f, 1.0f);
}

float AreaShape::getInterpolatedRadius(float angle) {
    ofVec2f v1, v2;
    findBracketingVertices(angle, v1, v2);
    
    // Get properties of the two vertices we're interpolating between
    float r1 = v1.length();
    float r2 = v2.length();
    
    float a1 = atan2(v1.y, v1.x);
    if (a1 < 0) a1 += TWO_PI;
    
    float a2 = atan2(v2.y, v2.x);
    if (a2 < 0) a2 += TWO_PI;
    
    // Handle wraparound at 2PI
    if (a2 < a1) a2 += TWO_PI;
    
    // Use smooth cosine interpolation
    float t = (angle - a1) / (a2 - a1);
    t = 0.5f - cos(t * PI) * 0.5f;
    
    return ofLerp(r1, r2, t);
}

int AreaShape::findBracketingVertices(float angle, ofVec2f& v1, ofVec2f& v2) {
    // Find which segment of the shape contains this angle
    float segmentAngle = TWO_PI / (vertices.size() - 1);
    int index = angle / segmentAngle;
    
    v1 = vertices[index];
    v2 = vertices[index + 1];
    
    return index;
}

void AreaShape::draw() {
    ofSetLineWidth(2);
    ofNoFill();
    
    // Draw shape outline
    ofBeginShape();
    for (const auto& vertex : vertices) {
        ofVertex(vertex.x + offset.x, vertex.y + offset.y);
    }
    ofEndShape();
    
    ofSetLineWidth(1);
}
