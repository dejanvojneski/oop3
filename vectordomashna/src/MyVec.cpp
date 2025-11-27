#include "../include/MyVec.h"

MyVec::MyVec() {
    sz = 0;
    capacity = 1;
    data = new int[capacity];
}

MyVec::MyVec(int n, int value) {
    sz = n;
    capacity = (n > 0 ? n : 1);
    data = new int[capacity];
    for (int i = 0; i < n; i++) data[i] = value;
}

MyVec::MyVec(const MyVec& other) {
    sz = other.sz;
    capacity = other.capacity;
    data = new int[capacity];
    for (int i = 0; i < sz; i++) data[i] = other.data[i];
}

MyVec& MyVec::operator=(const MyVec& other) {
    if (this != &other) {
        delete[] data;
        sz = other.sz;
        capacity = other.capacity;
        data = new int[capacity];
        for (int i = 0; i < sz; i++) data[i] = other.data[i];
    }
    return *this;
}

MyVec::~MyVec() {
    delete[] data;
}

int MyVec::size() const { return sz; }
int MyVec::cap() const { return capacity; }

int& MyVec::at(int index) {
    if (index < 0 || index >= sz)
        throw out_of_range("Index out of range");
    return data[index];
}

int& MyVec::operator[](int index) { return data[index]; }

int& MyVec::front() { return data[0]; }
int& MyVec::back() { return data[sz - 1]; }
int* MyVec::dataPtr() { return data; }

void MyVec::push(int value) {
    if (sz == capacity) {
        capacity *= 2;
        int* newData = new int[capacity];
        for (int i = 0; i < sz; i++) newData[i] = data[i];
        delete[] data;
        data = newData;
    }
    data[sz++] = value;
}

void MyVec::pop() {
    if (sz == 0) return;
    sz--;

    if (sz > 0 && sz <= capacity / 4) {
        int newCap = capacity / 2;
        if (newCap < 1) newCap = 1;

        int* newData = new int[newCap];
        for (int i = 0; i < sz; i++)
            newData[i] = data[i];

        delete[] data;
        data = newData;
        capacity = newCap;
    }
}

void MyVec::sort() {
    for (int i = 0; i < sz - 1; i++)
        for (int j = i + 1; j < sz; j++)
            if (data[j] < data[i])
                swap(data[i], data[j]);
}

MyVec MyVec::operator+(const MyVec& other) {
    MyVec result(sz + other.sz);

    for (int i = 0; i < sz; i++)
        result.data[i] = data[i];

    for (int i = 0; i < other.sz; i++)
        result.data[sz + i] = other.data[i];

    result.sz = sz + other.sz;
    return result;
}

void MyVec::print() const {
    cout << "MyVec (size=" << sz << ", capacity=" << capacity << ") : ";
    for (int i = 0; i < sz; i++) cout << data[i] << " ";
    cout << endl;
}
