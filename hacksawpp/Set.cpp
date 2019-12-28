//
// Created by atuser on 12/28/19.
//

#include "Set.h"


template<typename T, int (*cmp)(T, T)>
_Set<T, cmp>::_Set() {
	_Vector<T>::_Vector(false, true);
}


template<typename T, int (*cmp)(T, T)>
T _Set<T, cmp>::remove(T element) {
	int index = this->find(element);
	
	if (index != -1)
		return this->remove(index);
	
	return NULL;
}

template<typename T, int (*cmp)(T, T)>
int _Set<T, cmp>::find(T element) {
	for (int i = 0; i < this->length(); i++) {
		if (cmp(this->get(i), element))
			return i;
	}
	
	return -1;
}

template<typename T, int (*cmp)(T, T)>
int _Set<T, cmp>::add(T element) {
	if (this->find(element) != -1)
		return 0;
	
	_Vector<T>::add(element);
	
	return 1;
}
