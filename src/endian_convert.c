//
// Created by atuser on 1/13/18.
//

#include <autogentoo/endian_convert.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <byteswap.h>

uint32_t htonl(uint32_t host) {
	uint32_t to_change = host;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	to_change = (uint32_t) bswap_32(to_change);
#endif
	return to_change;
}

uint32_t ntohl(uint32_t network) {
	uint32_t to_change = network;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	to_change = (uint32_t) bswap_32(to_change);
#endif
	return to_change;
}

uint64_t htonll(uint64_t host) {
	uint64_t to_change = host;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	to_change = (uint64_t) bswap_64(to_change);
#endif
	return to_change;
}

uint64_t ntohll(uint64_t network) {
	uint64_t to_change = network;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	to_change = (uint64_t) bswap_64(to_change);
#endif
	return to_change;
}

Endian* endian_new(void* src) {
	Endian* out = malloc(sizeof(Endian));
	out->nodes = vector_new(sizeof(EndianNode*), REMOVE | UNORDERED);
	
	return out;
}

void endian_add_handler(Endian* target, size_t offset, endian_convert_t type) {
	EndianNode* handler = malloc(sizeof(EndianNode));
	handler->type = type;
	handler->offset = offset;
	
	vector_add(target->nodes, &handler);
}

void endian_to_network(Endian* target, void* dest) {
	int i;
	for (i = 0; i != target->nodes->n; i++) {
		EndianNode* node = *(void**) vector_get(target->nodes, i);
		
		uint16_t _s;
		uint32_t _i;
		uint64_t _l;
		
		switch (node->offset) {
			case ENDIAN_CHAR:
				break;
			case ENDIAN_U16:
				_s = htons(*(uint16_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_s, sizeof(uint16_t));
			case ENDIAN_U32:
				_i = htonl(*(uint32_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_i, sizeof(uint32_t));
			case ENDIAN_U64:
				_l = htonll(*(uint64_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_l, sizeof(uint64_t));
			default:
				break;
		}
	}
}

void endian_to_host(Endian* target, void* dest) {
	int i;
	for (i = 0; i != target->nodes->n; i++) {
		EndianNode* node = *(void**) vector_get(target->nodes, i);
		
		uint16_t _s;
		uint32_t _i;
		uint64_t _l;
		
		switch (node->offset) {
			case ENDIAN_CHAR:
				break;
			case ENDIAN_U16:
				_s = ntohs(*(uint16_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_s, sizeof(uint16_t));
			case ENDIAN_U32:
				_i = ntohl(*(uint32_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_i, sizeof(uint32_t));
			case ENDIAN_U64:
				_l = ntohll(*(uint64_t*) (target->src + node->offset));
				memcpy(dest + node->offset, &_l, sizeof(uint64_t));
			default:
				break;
		}
	}
}

void endian_free(Endian* target) {
	vector_free(target->nodes);
	free(target);
}