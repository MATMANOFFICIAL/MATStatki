// Ship.h
#pragma once

class Ship {
public:
    Ship(int size) : size(size), hits(0) {}
    bool isSunk() const { return hits >= size; }
    void hit() { hits++; }

private:
    int size;
    int hits;
};
