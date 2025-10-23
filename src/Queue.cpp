#include "Queue.h"

/*Queue::Queue()
{
    this.cabeza = nullptr;
    this.cola = nullptr;
}

void Queue::enqueue(const T& val) {
    Node<T>* newNode = new Node<T>(val);
    if (rear == nullptr) { // Cola vacía
        front = rear = newNode;
    } else {
        rear->next = newNode;
        rear = newNode;
    }
}

T Queue::dequeue() {
    if (front == nullptr) {
        throw std::runtime_error("Queue is empty");
    }
    Node<T>* temp = front;
    T data = front->data;
    front = front->next;

    if (front == nullptr) {
        rear = nullptr; // La cola quedó vacía
    }
    delete temp;
    return data;
}

bool Queue::estaVacio() const {
    return front == nullptr;
}

Queue::~Queue()
{
    //dtor
}*/
