#ifndef HEXGAME_HEXAGONLIST_H
#define HEXGAME_HEXAGONLIST_H

class Hexagon {
public:
    Hexagon(int x, int y, Hexagon* next);
    int getX();
    int getY();
    Hexagon* getNext();
    void setNext(Hexagon* next);
    ~Hexagon();
private:
    int x;
    int y;
    Hexagon* next;
};

class HexagonList {
public:
    HexagonList();
    void append(int x, int y);
    void removeFirstNElements(int size);
    int getSize();
    Hexagon* getHead();
    bool elementIsInList(int x, int y);
    void clear();
    HexagonList& operator=(const HexagonList& other);
    ~HexagonList();
private:
    Hexagon* head;
    Hexagon* tail;
    int size;
};


#endif //HEXGAME_HEXAGONLIST_H
