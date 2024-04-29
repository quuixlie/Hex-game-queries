#include "HexagonList.h"

Hexagon::Hexagon(int x, int y, Hexagon *next) {
    this->x = x;
    this->y = y;
    this->next = next;
}

int Hexagon::getX() {
    return x;
}

int Hexagon::getY() {
    return y;
}

Hexagon *Hexagon::getNext() {
    return next;
}

void Hexagon::setNext(Hexagon* next) {
    this->next = next;
}

Hexagon::~Hexagon() {
}

HexagonList::HexagonList() {
    head = nullptr;
    tail = nullptr;
    size = 0;
}

void HexagonList::append(int x, int y) {
    Hexagon* newHexagon = new Hexagon(x, y, nullptr);
    size++;
    if (head == nullptr) {
        head = newHexagon;
        tail = newHexagon;
    } else {
        tail->setNext(newHexagon);
        tail = newHexagon;
    }
}

void HexagonList::removeFirstNElements(int size) {
    for (int i = 0; i < size; i++) {
        Hexagon* next = head->getNext();
        delete head;
        head = next;
    }
    if (head == nullptr) {
        tail = nullptr;
    }
    this->size -= size;
}

int HexagonList::getSize() {
    return size;
}

Hexagon *HexagonList::getHead() {
    return head;
}

bool HexagonList::elementIsInList(int x, int y) {
    Hexagon* current = head;
    while (current != nullptr) {
        if (current->getX() == x && current->getY() == y) {
            return true;
        }
        current = current->getNext();
    }
    return false;
}

void HexagonList::clear() {
    Hexagon* current = head;
    while (current != nullptr) {
        Hexagon* next = current->getNext();
        delete current;
        current = next;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;
}

HexagonList &HexagonList::operator=(const HexagonList &other) {
    clear();
    Hexagon* current = other.head;
    while (current != nullptr) {
        append(current->getX(), current->getY());
        current = current->getNext();
    }
    return *this;
}

HexagonList::~HexagonList() {
    Hexagon* current = head;
    while (current != nullptr) {
        Hexagon* next = current->getNext();
        delete current;
        current = next;
    }
}
