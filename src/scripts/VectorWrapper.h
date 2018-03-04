//
// Created by atuser on 3/3/18.
//

#ifndef AUTOGENTOO_INTERFACE_VECTORWRAPPER_H
#define AUTOGENTOO_INTERFACE_VECTORWRAPPER_H


#include <cstdio>

template <typename T>
class VectorWrapper {
private:
	T* ptr;
	size_t size;
	size_t length;
	
	void allocate ();

public:
	size_t increment;
	
	explicit VectorWrapper (size_t start_size=5);
	void append (T item);
	T operator[] (int index);
};


#endif //AUTOGENTOO_INTERFACE_VECTORWRAPPER_H
