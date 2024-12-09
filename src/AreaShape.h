#pragma once
#include "ofMain.h"

class AreaShape {
public:
    AreaShape();
    void setup(int numVertices, float baseRadius, float maxDistortion, ofVec2f offset = ofVec2f(0,0));
    float getDistanceFieldValue(float x, float y);
    void draw();
    ofVec2f getVertex(int index);
    ofVec2f getCenter() const { return offset; }

private:
    // attributes
    vector<ofVec2f> vertices;
    float minRadius;
    float maxRadius;
    ofVec2f offset;
    
    float getInterpolatedRadius(float angle);
    int findBracketingVertices(float angle, ofVec2f& v1, ofVec2f& v2);
};
