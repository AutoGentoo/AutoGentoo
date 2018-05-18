//
// Created by atuser on 5/18/18.
//


#include <autogentoo/queue.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>

Queue* queue_new (queue_t type, char* template, ...) {
	va_list args;
	va_start (args, template);
}

Queue** queue_find_end (Queue* q) {
	Queue* end = q;
	while (end->last)
		end = end->last;
	return &(end->last);
}

Queue* queue_add (Queue* dest, Queue* src) {
	*queue_find_end(dest) = src;
	return src;
}

Queue* queue_shrink (Queue* q) {
	Queue* out = q->last;
	q->last = NULL;
	return out;
}

void prv_write_template (char* t, void* ptr, int fd) {
	size_t offset = 0;
	size_t len = 0;
	while (*t) {
		if (*t == 'i')
			write (fd, (char*)ptr + offset, len = sizeof (int));
		else if (*t == 's')
			write (fd, (char*)ptr + offset, len = strlen ((char*)ptr + offset));
		offset += len;
		t++;
	}
}

void queue_write (Queue* q, int fd) {
	while (q) {
		write (fd, &q->type, sizeof (int));
		write (fd, q->template, strlen (q->template));
		prv_write_template (q->template, q->args, fd);
		q = q->last;
	}
	
	int k = QUEUE_END;
	write (fd, &k, sizeof (int));
}

Queue* queue_free_single (Queue* q) {
	free (q->args);
	free (q->template);
	Queue* out = q->last;
	free (q);
	
	return out;
}
void queue_free (Queue* head) {
	while (head)
		head = queue_free_single (head);
}
