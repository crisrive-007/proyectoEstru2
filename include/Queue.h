#ifndef QUEUE_H
#define QUEUE_H

#include <iostream>
#include <vector>
#include <string>

template <typename T>
struct Nodo {
    T data;
    Nodo* next;

    Nodo(T valor) : data(valor), next(nullptr) {

    }
};

template <typename T>
class Queue
{
    public:
        Queue();
        //void enqueue(const& T valor);
        T dequeue();
        bool estaVacio() const;
        virtual ~Queue();

    protected:

    private:
        Nodo<T>* cabeza;
        Nodo<T>* cola;
};

#endif // QUEUE_H
