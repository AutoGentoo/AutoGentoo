cdef extern from "<autogentoo/api/dynamic_binary.h>":
	
	cdef:
		ctypedef enum dynamic_bin_t:
			DYNAMIC_BIN_OK,
			DYNAMIC_BIN_ETYPE,
			DYNAMIC_BIN_EBIN # /* Binary added using dynamic_binary_add */
		
		struct array_node:
			void* parent;
			char* template_start;
			int* size_ptr;
		
		struct DynamicType_binary_t:
			void* data
			size_t n
		
		ctypedef enum dynamic_binary_endian_t:
			DB_ENDIAN_TARGET_NETWORK = 0x1, # /* If true then we set to big_endian*/
			DB_ENDIAN_INPUT_NETWORK = 0x2 # /* Default set to host_endian */
		
		ctypedef union DynamicType:
			char* string;
			int integer;
			DynamicType_binary_t binary;
		
	ctypedef struct DynamicBinary:
			size_t template_str_size;
			size_t ptr_size;
			size_t used_size;
			size_t template_used_size;
			void* ptr;
			char* template;
			char* current_template;
			
			array_node* array_size;
			dynamic_binary_endian_t endian;
	
	cdef:
		DynamicBinary* dynamic_binary_new(dynamic_binary_endian_t endian);
		void dynamic_binary_array_start(DynamicBinary* db);
		void dynamic_binary_array_next(DynamicBinary* db);
		void dynamic_binary_array_end(DynamicBinary* db);
		dynamic_bin_t dynamic_binary_add(DynamicBinary* db, char type, void* data);
		dynamic_bin_t dynamic_binary_add_binary(DynamicBinary* db, size_t n, void* data);
		dynamic_bin_t dynamic_binary_add_quick(DynamicBinary* db, char* template, DynamicType* content);
		void* dynamic_binary_free(DynamicBinary* db);
