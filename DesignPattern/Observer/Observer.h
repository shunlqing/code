#include <iostream>
using namespace std;

class Observable; //前向声明

class Observer {
public:
    Observer();
    virtual ~Observer();
    virtual void update(Observable* ob) = 0;
};

class ConcreteObserver : public Observer {
public:
    ConcreteObserver(string name);
    virtual ~ConcreteObserver();
    virtual void update(Observable * ob);

private:
    string m_objName;
    int m_observerState;
};