
* Magic
* Num constants
* Num functions
* Function metadata (num constants, num closures, num arguments, num allocs, call size)
* Funcion bodies (allocs, call)
* Constants (also imports and constant closures)

Function 88 
        call count: 8
        imports:    []
        constants:  []
        closures:   2
        arguments:  2
        allocates:  3
                15 []
                89 [0, 1, 2, 3]
                90 [0, 1, 3]
        tail call:
                [4, 2, 6, 5]

# File formate

```

struct file_t {
	file_header_t header;
	function_header_t functions[]; // size header.num_functions
	function_body_t  bodies[]; // size header.num_functions
	
};

struct file_header_t {
	uint64_t magic;
	uint16_t num_constants;
	uint16_t num_functions;
};

struct function_header_t {
	uint16_t constants_size;
	uint16_t closure_size;
	uint16_t arguments_size;
	uint16_t allocs_size;
	uint16_t call_size;
};

struct function_body_t {
	function_alloc_t allocs[]; // size function_header_t.allocs_size
	function_call_t call;
}

struct function_alloc_t {
	uint16_t function_index;
	uint16_t closure[]; // Size functions[function_index].closure_size
};

struct function_call_t {
	uint16_t closure_arguments[]; // Size function_header_t.call_size
};

struct constant_t {
	uint16_t size;
};
```
