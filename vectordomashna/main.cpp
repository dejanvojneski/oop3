#include "include/MyVec.h"
#include <iostream>
using namespace std;

int main() {
    MyVec v(3, 123);
    v.print();

    v.push(999);
    v.push(777);
    v.pop();
    v.push(777);
    v.push(333);
    v.push(122);
    v.push(10112554);

    v.print();

    cout << v[6];
}
