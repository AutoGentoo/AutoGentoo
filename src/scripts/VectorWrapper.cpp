//
// Created by atuser on 3/3/18.
//

#include <cstdlib>
#include "VectorWrapper.h"


template <typename T>
VectorWrapper<T>::VectorWrapper (size_t start_size) {
	this->ptr = malloc (sizeof (T) * start_size);
	this->length = start_size;
	this->size = 0;
	this->increment = 5;
}

template <typename T>
void VectorWrapper<T>::allocate () {
	this->length += this->increment;
	this->ptr = realloc (this->ptr, this->length);
}

template <typename T>
void VectorWrapper<T>::append (T item) {
	if (this->size + 1 >= this->length)
		this->allocate();
	this->ptr[this->size] = item;
	this->size++;
}

template <typename T>
T VectorWrapper<T>::operator[] (const int index) {
	return this->ptr[index];
}