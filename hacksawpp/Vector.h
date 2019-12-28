//
// Created by atuser on 12/28/19.
//

#ifndef AUTOGENTOO_VECTOR_H
#define AUTOGENTOO_VECTOR_H

#include <autogentoo/hacksaw/vector.h>

template<typename T>
class _Vector {
private:
	Vector* parent{};

public:
	explicit _Vector(bool ordered=true, bool remove=true);
	
	/* Vector operations */
	T get(int i);
	virtual int add(T element);
	virtual void extend(_Vector<T>* to_add);
	
	/* Attribute access */
	int length();
	
	/* Util */
	void foreach(void (*func)(T));
	void reset(int n);
	~_Vector();
};



#endif //AUTOGENTOO_VECTOR_H
