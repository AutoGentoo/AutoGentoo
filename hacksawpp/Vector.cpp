//
// Created by atuser on 12/28/19.
//

#include "Vector.h"

template<typename T>
_Vector<T>::_Vector(bool ordered, bool remove) {
	int opts = 0;
	if (ordered)
		opts |= VECTOR_ORDERED;
	else
		opts |= VECTOR_UNORDERED;
	
	if (remove)
		opts |= VECTOR_REMOVE;
	else
		opts |= VECTOR_KEEP;
	
	
	this->parent = vector_new(static_cast<vector_opts>(opts));
}

template<typename T>
T _Vector<T>::get(int i) {
	return vector_get(this->parent, i);
}

template<typename T>
int _Vector<T>::add(T element) {
	int out = this->parent->n;
	vector_add(this->parent, element);
	
	return out;
}

template<typename T>
_Vector<T>::~_Vector() {
	vector_free(this->parent);
}

template<typename T>
int _Vector<T>::length() {
	return this->parent->n;
}

template<typename T>
void _Vector<T>::foreach(void (* func)(T)) {
	for (int i = 0; i < this->length(); i++)
		func(this->get(i));
}

template<typename T>
void _Vector<T>::reset(int n) {
	this->parent->n = n;
}

template<typename T>
void _Vector<T>::extend(_Vector<T>* to_add) {
	vector_extend(this->parent, to_add->parent);
}
