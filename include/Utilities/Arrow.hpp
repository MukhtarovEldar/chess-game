#pragma once

#include <string>
#include <vector>
#include <utility>
using namespace std;

typedef std::pair<int, int> coor2d;
inline constexpr int g_rotation[3][3] = {
        { 225, 180, 135 },
        { 270,   0,  90 },
        { 315,   0,  45 },
};
inline vector<coor2d> urCoords = { { 1,-2 }, { 2,1 }, { -1,2 }, { -2,-1 } };
inline vector<coor2d> ruCoords = { { 2,-1 }, { 1,2 }, { -2,1 }, { -1,-2 } };

class Arrow {
    coor2d m_origin; // origin absolute coordinate
    coor2d m_destination; // destination absolute coordinate
    string m_filename; // file name for the arrow
    int m_dx, m_dy; // tile differential coordinates
    int m_rotation; // multiples of 45 degrees
    int m_size; // size of the arrow (0 to 7)
    bool m_isLArrow = false;

    public:
    Arrow(): m_rotation(0), m_size(0){};
    Arrow(coor2d, coor2d, int, string);

    string getFilename() { return m_filename; }
    coor2d getOrigin() { return m_origin; }
    coor2d getFormattedOrigin();
    coor2d getDestination() { return m_destination; }
    int getRotation() { return m_rotation; }
    
    void setCoordinates(coor2d&,coor2d&);
    void setDestination(coor2d&);
    void setOrigin(coor2d&); 
    void checkKnightSquares();
    bool checkOutOfBounds();
    void updateArrow();
    void resetParameters();
    bool removeArrow(vector<Arrow>&);
    bool isDrawable();
    bool compare(Arrow&);
    bool isLArrow() { return m_isLArrow; }
};