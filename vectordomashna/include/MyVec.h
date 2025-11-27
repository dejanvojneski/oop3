#pragma once
#include <iostream>
#include <stdexcept>
using namespace std;

class MyVec {
private:
    int* data;
    int sz;
    int capacity;

public:
    MyVec();
    MyVec(int n, int value);
    MyVec(const MyVec& other);
    MyVec& operator=(const MyVec& other);
    ~MyVec();

    int size() const;
    int cap() const;

    int& at(int index);
    int& operator[](int index);
    int& front();
    int& back();
    int* dataPtr();

    void push(int value);
    void pop();
    void sort();

    MyVec operator+(const MyVec& other);

    void print() const;
};
