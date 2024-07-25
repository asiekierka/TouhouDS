#ifndef TH_LINKED_LIST_H
#define TH_LINKED_LIST_H

#include <stdlib.h>

template<class T> class THLinkedList;

template<class T>
class LL_Node {
    private:

    public:
    	THLinkedList<T>* list;
        LL_Node<T>* prev;
        LL_Node<T>* next;
        T* value;

        LL_Node();
        ~LL_Node();

        void Prepend(LL_Node<T>* node);
        void Append(LL_Node<T>* node);
        void Remove();
};

template<class T>
class THLinkedList {
    private:

    public:
        LL_Node<T>* first;
        LL_Node<T>* last;

        THLinkedList();
        ~THLinkedList();

        void Prepend(LL_Node<T>* node);
        void Append(LL_Node<T>* node);
};













template<class T>
LL_Node<T>::LL_Node() {
    list = NULL;
    prev = next = NULL;
    value = NULL;
}

template<class T>
LL_Node<T>::~LL_Node() {
    Remove();
}

template<class T>
void LL_Node<T>::Prepend(LL_Node<T>* node) {
    LL_Node<T>* left = prev;
    if (left) {
        left->next = node;
    }
    node->prev = left;

    node->next = this;
    this->prev = node;

    node->list = list;
    if (list && list->first == this) {
        list->first = node;
    }
}

template<class T>
ITCM_CODE
void LL_Node<T>::Append(LL_Node<T>* node) {
    LL_Node<T>* right = next;
    if (right) {
        right->prev = node;
    }
    node->next = right;

    node->prev = this;
    this->next = node;

    node->list = list;
    if (list && list->last == this) {
        list->last = node;
    }
}

template<class T>
void LL_Node<T>::Remove() {
    if (prev) prev->next = next;
    if (next) next->prev = prev;

    if (list) {
        if (list->first == this) {
            list->first = (prev ? prev : next);
        }
        if (list->last == this) {
            list->last = (next ? next : prev);
        }
    }

    list = NULL;
    prev = next = NULL;
}

template<class T>
THLinkedList<T>::THLinkedList() {
    first = last = NULL;
}

template<class T>
THLinkedList<T>::~THLinkedList() {
    LL_Node<T>* node = first;
    while (node) {
        LL_Node<T>* next = node->next;
        node->Remove();
        node = next;
    }
    first = last = NULL;
}

template<class T>
void THLinkedList<T>::Append(LL_Node<T>* node) {
    node->list = this;

    if (last) {
        last->Append(node);
        //last = node; <- done by append function
    } else {
        first = last = node;
    }
}

template<class T>
void THLinkedList<T>::Prepend(LL_Node<T>* node) {
    node->list = this;

    if (first) {
        first->Prepend(node);
        //first = node; <- done by prepend function
    } else {
        first = last = node;
    }
}

#endif
