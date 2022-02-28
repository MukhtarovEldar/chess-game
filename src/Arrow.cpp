#include <utility>
#include <algorithm> 
#include "../include/Arrow.hpp"
#include "../include/GameThread.hpp"

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

Arrow::Arrow(coor2d origin, coor2d destination) : m_origin(origin), m_destination(destination) {};

void Arrow::setOrigin(coor2d& origin) {
    // need to format the origin to be at the center of the tile
    m_origin.first = (origin.first / CELL_SIZE) * CELL_SIZE + CELL_SIZE/2;
    m_origin.second = (origin.second / CELL_SIZE) * CELL_SIZE + MENUBAR_HEIGHT + CELL_SIZE/2;
}
void Arrow::setCoordinates(coor2d& origin, coor2d& destination) {
    m_origin = origin; m_destination = destination; 
}

void Arrow::updateArrow() { 
    // absolute coordinates
    int dx = m_destination.first - m_origin.first;
    int dy = m_destination.second - m_origin.second;

    // tile coordinates 
    int dxc = (m_destination.first - m_origin.first)/(int)CELL_SIZE;
    int dyc = (m_destination.second - m_origin.second)/(int)CELL_SIZE;

    // check if arrow is feasible 
    if((dxc == 0 && dyc == 0) || (dxc != 0 && abs(dyc) > 1) || (dyc != 0 && abs(dxc) > 1)) return;

    // TODO check if out of window bounds ? 

    if (dxc != 0 || dyc != 0)  // if (!(x == 0 && y == 0))
        m_rotation = rotation[1 + sign(dxc)][1 + sign(dyc)];
    
    m_size = abs(std::max(dxc, dyc));
}

void Arrow::resetParameters() {
     m_origin.first = 0 , m_origin.second = 0;
     m_destination.first = 0 , m_destination.second = 0;
}

bool Arrow::isDrawable() {
    int dx = abs(m_destination.first - m_origin.first)/CELL_SIZE;
    int dy = abs(m_destination.second - m_origin.second)/CELL_SIZE;
    // cout << dx << "," << dy << endl;
    return dx > 0 || dy > 0;
}

