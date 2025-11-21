#include<iostream>
using namespace std;

// +, -, *, /, ==, <, >
class Frac {
    int n;
    int d;

    void normalize() {
        if (d < 0) {
            n = -n;
            d = -d;
        }
    }

public:
    int N () const { return this->n; }
    int D () const { return this->d; }
    void setN(int x) { this->n = x; }
    void setD(int x) { this->d = x; }

    Frac(const int n, const int d=1) : n(n), d(d) {
        normalize();
    }

    // Copy C-tor
    Frac (const Frac& orig) {
        this->n = orig.n;
        this->d = orig.d;
    }

    Frac& operator=(const Frac& orig) {
        if (this!=&orig) {
            this->n = orig.n;
            this->d = orig.d;
        }
        return *this;
    }

    void print() {
        cout << n << "/" << d;
    }

    // Aritmetichki operatori

    Frac operator+(const Frac& b) const {
        return Frac(n*b.d + d*b.n, d*b.d);
    }

    Frac operator-(const Frac& b) const {
        return Frac(n*b.d - d*b.n, d*b.d);
    }

    Frac operator*(const Frac& b) const {
        return Frac(n * b.n, d * b.d);
    }

    Frac operator/(const Frac& b) const {
        return Frac(n * b.d, d * b.n);
    }

    // Operatori za sporeduvanje

    bool operator==(const Frac& b) const {
        return (long long)n * b.d == (long long)b.n * d;
    }

    bool operator<(const Frac& b) const {
        return (long long)n * b.d < (long long)b.n * d;
    }

    bool operator>(const Frac& b) const {
        return (long long)n * b.d > (long long)b.n * d;
    }

    ~Frac() {}
};

ostream& operator<<(ostream& o, const Frac& f) {
    o << f.N() << "/" << f.D();
    return o;
}

int main() {
    Frac a(2,3);
    Frac b(4,5);

    cout << "a + b = " << (a + b) << endl;
    cout << "a - b = " << (a - b) << endl;
    cout << "a * b = " << (a * b) << endl;
    cout << "a / b = " << (a / b) << endl;

    cout << "a == b ? " << (a == b) << endl;
    cout << "a < b ?  " << (a < b) << endl;
    cout << "a > b ?  " << (a > b) << endl;

    return 0;
}
