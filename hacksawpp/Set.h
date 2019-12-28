//
// Created by atuser on 12/28/19.
//

#ifndef AUTOGENTOO_SET_H
#define AUTOGENTOO_SET_H

#include <autogentoo/hacksaw/set.h>
#include "Vector.h"

template<typename T, int (*cmp)(T, T)>
class _Set : _Vector<T> {
private:
	int find(T element);
public:
	_Set();
	
	T remove(T element);
	int add(T element);
};

#endif //AUTOGENTOO_SET_H
