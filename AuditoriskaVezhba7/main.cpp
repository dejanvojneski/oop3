#include<cstring>
#include<iostream>

using namespace std;

class User {
    char *username;
    char *companyName;
    int position;

    void copy(const User &u) {
        this->username = new char[strlen(u.username) + 1];
        strcpy(this->username, u.username);
        this->companyName = new char[strlen(u.companyName) + 1];
        strcpy(this->companyName, u.companyName);
        this->position = u.position;
    }

public:
    User(char *username = "", char *companyName = "", int position = 0) {
        this->username = new char[strlen(username) + 1];
        strcpy(this->username, username);
        this->companyName = new char[strlen(companyName) + 1];
        strcpy(this->companyName, companyName);
        this->position = position;
    }

    User(const User &u) {
        copy(u);
    }

    User &operator=(const User &u) {
        if (this != &u) {
            delete[] username;
            delete[] companyName;
            copy(u);
        }
        return *this;
    }

    ~User() {
        delete[] username;
        delete[] companyName;
    }

    char *getUsername() const {
        return username;
    }

    void setUsername(char *username) {
        this->username = new char[strlen(username) + 1];
        strcpy(this->username, username);
    }

    char *getCompanyName() const {
        return companyName;
    }

    void setCompanyName(char *companyName) {
        this->companyName = new char[strlen(companyName) + 1];
        strcpy(this->companyName, companyName);
    }

    int getPosition() const {
        return position;
    }

    void setPosition(int position) {
        this->position = position;
    }


    friend ostream &operator<<(ostream &os, const User &user) {
        return os << "Username: " << user.username
                  << " Company name: " << user.companyName
                  << " Position: " << user.position;
    }

    friend istream &operator>>(istream &in, User &user) {
        return in >> user.username >> user.companyName >> user.position;
    }

    bool operator==(User &u) {
        return strcmp(this->username,u.username)==0;
    }
};

class Group {
protected:
    User *users;
    int n;
    string name;

    void copy(const Group &g) {
        this->n = g.n;
        this->name = g.name;
        users = new User[n];
        for (int i = 0; i < n; i++)
            users[i] = g.users[i];
    }

public:
    Group(string name = "") {
        this->name = name;
        users = new User[0];
        n = 0;
    }

    Group(const Group &g) {
        copy(g);
    }

    Group &operator=(const Group &g) {
        if (this != &g) {
            delete[] users;
            copy(g);
        }
        return *this;
    }

    ~Group() {
        delete[] users;
    }

    virtual void addMember(User &u) {
        for (int i = 0; i < n; i++) {
            if (users[i] == u)
                return;
        }
        User *tmp = new User[n + 1];
        for (int i = 0; i < n; i++)
            tmp[i] = users[i];
        tmp[n++] = u;
        delete[] users;
        users = tmp;
    }

    double getAverageLevel() const {
        if (n == 0)
            return 0;
        int sum = 0;
        for (int i = 0; i < n; i++)
            sum += users[i].getPosition();
        return double(sum) / n;
    }

    virtual double rating() const {
        return (10 - getAverageLevel()) * n / 100.0;
    }

    friend ostream &operator<<(ostream &out, const Group &g) {
        out << "Group: " << g.name << endl;
        out << "Members: " << g.n << endl;
        out << "Rating: " << g.rating() << endl;
        out << "Members list: " << endl;
        if (g.n == 0) {
            out << "EMPTY" << endl;
        } else {
            for (int i = 0; i < g.n; i++) {
                out << i + 1 << ". " << g.users[i] << endl;
            }
        }

        return out;
    }

};

class PrivateGroup : public Group {
public:
    static int CAPACITY;
    static const float COEFFICIENT;

    PrivateGroup(string name) : Group(name) {};

    PrivateGroup(const PrivateGroup &pg) : Group(pg) {}

    PrivateGroup &operator=(const PrivateGroup &pr) {
        if (this != &pr) {
            Group::operator=(pr);
        }
        return *this;
    }

    void addMember(User &u) {
        if (this->n == CAPACITY) {
            return;
        }
        Group::addMember(u);
    }

    double rating() const {
        return (10 - getAverageLevel()) * (double(n) / CAPACITY) * COEFFICIENT;
    }

    static void setCapacity(int capacity) {
        CAPACITY = capacity;
    }

    static int getCapacity() {
        return CAPACITY;
    }

    friend ostream &operator<<(ostream &out, const PrivateGroup &g) {
        out << "Private ";
        Group group(g);
        out << group;
        return out;
    }

};

int PrivateGroup::CAPACITY = 10;
const float PrivateGroup::COEFFICIENT = 0.8f;

int main() {

    cout << "INTEGRATION TEST" << endl;
    char name[50];
    int nGroups;
    cin >> nGroups;
    Group **groups = new Group *[nGroups];
    for (int i = 0; i < nGroups; i++) {
        int type;
        cin >> type;
        cin >> name;
        if (type == 1) { //normal group
            groups[i] = new Group(name);
        } else { //private group
            groups[i] = new PrivateGroup(name);
        }
        int nUsers;
        cin >> nUsers;
        for (int j = 0; j < nUsers; j++) {
            User u;
            cin >> u;

            groups[i]->addMember(u);

        }
    }

    cout << "BEFORE CHANGE OF PRIVATE GROUP COEFFICIENT" << endl;
    for (int i = 0; i < nGroups; ++i) {
        cout << *groups[i];
    }
    PrivateGroup::setCapacity(15);
    cout << "AFTER CHANGE OF PRIVATE GROUP COEFFICIENT" << endl;
    for (int i = 0; i < nGroups; ++i) {
        cout << *groups[i];
    }

}