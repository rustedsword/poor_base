# poor_base
Header only macro library for type-generic printing and advanced C array operations.

```c
#include <poor_array.h>
#include <poor_stdio.h>

#define do_something(data) _do_something(ARRAY_SIZE(data), &auto_arr(data))
int _do_something(size_t len, int (*d)[len]) {
    if(!d || ARRAY_SIZE(d) < 6)
        return printerrln("Too small array"), -1;

    make_arrview_first(d_first, 2, d);
    make_arrview_shrink(d_middle, 2, 3, d);
    make_arrview_last(d_last, 3, d);

    copy_array(d_first, (const int[]){-3, -4});

    fill_array(d_middle, 5);

    foreach_array_ref(d_last, ref)
        *ref = array_ref_index(d_last, ref) + 1;

    print_array(d);

    concat_vla(info,
            "d:", ARRAY_SIZE(d), " "
            "d_first:", ARRAY_SIZE(d_first), " "
            "d_middle:", ARRAY_SIZE(d_middle), " "
            "d_last:", ARRAY_SIZE(d_last));

    println(info);

    return 0;
}

int main(int argc, char **argv) {
    int (*data)[6 + argc] = malloc_array(data);
    if(!data)
        return printerrln("Failed to allocate memory"), EXIT_FAILURE;

    int rc = do_something(data);
    free(data);
    return rc;
}
```
Compile and run it:
```
# ./test 
[-3,-4,5,5,1,2,3]
d:7 d_first:2 d_middle:2 d_last:3

# ./test 1 1
[-3,-4,5,5,5,5,1,2,3]
d:9 d_first:2 d_middle:4 d_last:3

```
# <poor_stdio.h>
This header contains macros for various functions found in stdio.h

### print() family macros
Type-generic wrappers around standard printing functions, with formatting support(width, precision, hexademical integers).

These macros automatically generate string with format specifiers at compile time.

ln() versions append '\n' at the end of printed line.

wrapped function | macro                    | description
-----------------|--------------------------|------------------
printf()         | print(), println()       | Print to stdout
fprintf(stderr)  | printerr(), printerrln() | Print to stderr
fprintf()        | fprint(), fprintln()     | Print to FILE
sprintf()        | sprint(), sprintln()     | Print to buffer
snprintf()       | sprint_array()           | Print to array/pointer to array

Supported format modifiers:

format_modifier                | description
-------------------------------|--------------------------------
fmt_p(var, precision)          | prints variable with specified precision
fmt_w(var, width)              | prints variable with field width
fmt_zw(var, width)             | prints variable with field width, filled with zeros
fmt_wp(var, width, prcsn)      | prints variable with field width and precision
fmt_zwp(var, width, prcsn)     | prints floats with zero-field width and precision
fmt_hex(var)                   | prints integer as hexademical
fmt_hex_p(var)                 | prints integer as hexademical with specified precision

```c
println("int:", 3, " float:", 5.6f, " bool:", true);
//int:3 float:5.600000 bool:true

printerrln("0x", fmt_hex(0xae), " 0x", fmt_hex_p(300, 10));
//0xae 0x000000012c

fprintln(stderr, "Offset is ", fmt_wp(13.449, 10, 2));
//Offset is      13.45

char buf[5];
sprint_array(buf,1,2,3,4,5);
println(buf);
//1234
```

### concat()
Concatenation macro family. Supports same features as print() macro family.

macro                 | description
----------------------|---------------
concat()              | dynamically allocates memory for concatenated string
concat_malloc_array() | same as concat() but declares a pointer to an array with a new string
concat_vla()          | creates variable length array for concatenated string

```c
char *string = concat("First:", 5, " Second:", fmt_zw(10, 6));
if(string) {
    println(string); //First:5 Second:000010
    free(string);
}

concat_malloc_array(s, 1L, 2U, 3LL, "4", fmt_p(5.0, 0));
if(s) {
    println("result:", s, " size in bytes:", sizeof(*s)); //result:12345 size in bytes:6
    free(s);
}

concat_vla(vla_string, "Five plus Six:", 5 + 6);
println(vla_string); //Five plus Six:11
```

# <poor_array.h>
This header contains useful macros to work with arrays.
All macros here can operate on arrays or pointers to arrays by auto-dereferencing them using auto_arr() macro,
while checking that arguments are really arrays.

Before even considering to use this library you should completely understand how arrays work.

Arrays are not pointers. But in most operations they decay to pointer to it's first element.
```c
unit32_t a[5]; //Array of 5 uint32_t  
sizeof(a); // 5 * 4 = 20  //sizeof doesn't decay array to pointer
uint32_t *ptr = a;  //a decayed to pointer to first element 
```
Pointers to arrays are just like regular pointers, but they pointing to whole array.
```c
uint32_t (*b)[5]; //Pointer to array of 5 uint32_t  
sizeof(b);  //size of pointer itself  
sizeof(*b); //size of array where pointer points: 5 * 4 = 20  
```
You can't pass array by value into another function. If you declaring a function with array argument then that array decayed into pointer to first element, so it is not longer an array anymore.
```c
void fn(uint32_t a[5]); -> void fn(uint32_t *a);
void fn(uint32_t a[]);  -> void fn(uint32_t *a);
void fn(size_t len, uint32_t a[len]); -> void fn(size_t len, uint32_t *a);
```
If you declare a function with multi-dimensional array argument, then it is converted to pointer to it's first element too: pointer to subarray. So, you are losing dimension that way.
```c
void fn(uint32_t a[5][3]); -> void fn(uint32_t (*a)[3]);  
```
So, first thing is to consider that you should never declare functions with array arguments, you should always use pointers to arrays instead. Pointers to arrays do not decay. That approach allows to use C type system to warn the programmer if he passes pointer to array of wrong size or type. While inside function body you will keep all arrays' dimensions.
```c
void fn(uint32_t (*a)[5]) {  //Function accepts only pointer to array of 5 uint32_t  
    sizeof(*a); //20  
}  

void fn(uint32_t (*a)[4][6]) { //Function accepts only pointer to array of 4 arrays of 6 uint32_t  
    sizeof(*a); //96  
}  
```
When passing an array of size known only at run-time the traditional approach is to pass pointer to first element and array length. But this way has same problems as above: you no longer have an array inside function body: only two variables: pointer and length.  
```c
void fn(uint32_t *a, size_t len); --> WRONG  
```
So, when you want to pass an array of run-time length, you should use pointers to Variable Length Arrays(VLA)
```c
void fn(const size_t len, uint32_t (*a)[len]) {  
    sizeof(*a); //Returns length of array at runtime.  
}  
```
And now it is where this macro library comes into play. By using auto_arr() and ARRAY_SIZE() you can simplify such function calls.  
```c
#define fn(a) _fn(ARRAY_SIZE(a), &auto_arr(a))  
void _fn(const size_t len, uint32_t (*a)[len]) {  
    sizeof(*a); //Returns length of array at runtime.
}  

uint32_t w[6];  //static array
fn(w);  
  
size_t len = 9;  
uint32_t x[len];  //VLA
fn(x);  

uint32_t (*y)[6] = &w; //Pointer to static array
fn(y);

uint32_t (*z)[len] = &x; //Pointer to VLA
fn(z);
```

By keeping arrays' arrayness you can use all features of this library: array views, generic array iterators, information macros, and even treat arrays as static vectors.

## Generic Array Macros

### auto_arr(var) / arr(var)
Allows to work with pointers to arrays in the same way as with ordinary arrays

If argument is a pointer to an array then this macro will dereference it.  
If argument is an array, then this macro does nothing at all, returning array as is.  
If argument is not an array or a pointer to an array then compilation will be stopped.  

```c
int a[2] = {1,2};  
int (*b)[2] = &a;  
arr(a)[0] = 5;   
arr(b)[1] = 6; 
```
Used by other macros, so they can operate on arrays or pointers to arrays at the same time.

### ARRAY_SIZE(var)
Returns count of elements in the array.
```c
int a[5];   
int (*b)[2];  
ARRAY_SIZE(a); //5  
ARRAY_SIZE(b); //2  
```
### ARRAYS_SIZE(arr1, arr2, ..., arrn)
Returns sum of all arrays sizes;
```c
int a[4];
int b[3];
int c[6];

ARRAYS_SIZE(a, b, c); //13
```
### ARRAY_SIZE_BYTES(var)
Returns total array size in bytes
```c
uint16_t (*e)[20];
ARRAY_SIZE_BYTES(e); //40
```
### ARRAYS_SIZE_BYTES(arr1, arr2, ..., arrn)
Returns sum of all arrays sizes in bytes
```c
uint8_t a[4];
uint16_t b[3];
uint32_t c[6];

ARRAYS_SIZE_BYTES(a, b, c); //34
```
### ARRAY_ELEMENT_SIZE(var)
Returns size of one array element
```c
uint32_t x[5];
ARRAY_ELEMENT_SIZE(x); //4: sizeof(uint32_t)

uint16_t a[2][5];
ARRAY_ELEMENT_SIZE(a); //10: sizeof(uint16_t [5])
```
### PRINT_ARRAY_INFO(var)
Prints information about array
```c
int a[2];   
PRINT_ARRAY_INFO(a); //Array "a" at 0x0 has size:2 uses 8 bytes, while single element has size:4  

size_t len = 5;   
short b[len];   
PRINT_ARRAY_INFO(b); //VLA "b" at 0x0 has size:5 uses 10 bytes, while single element has size:2 
```
### malloc_array(var)
Allocates memory for pointer to array (var) by using malloc()
```c
size_t len = 5000;
int (*a)[len];
if(!malloc_array(a)) {
    /* Failed to allocate memory */
}

fill_array(a, 0);
...
```
### calloc_array(var)
Allocates memory for pointer to array (var) by using calloc()
```c
int (*a)[2048];
if(!calloc_array(a)) {
    /* Failed to allocate memory */
}

/* Use array a ... */
```
### fill_array(var, value)
Fills array (var) with (value).
```c
size_t len = 5;
int x[len];
fill_array(x, 50);
print_array(x); //Will print 5050505050
```
### copy_array(dst_arr, src_arr)
Copies data from array (src_arr) to array (dst_arr)  
Type of array elements may be compatible.  
If destination array is not long enough then only part of source array will be copied.  
Source and destination arrays should not overlap.
```c
unsigned char a[4] = {0,1,2,3};
int (*b)[4];
malloc_array(b);
copy_array(b, a); 
print_array(b); //0123
```
### copy_arrays(dst_arr, src_arr0, src_arr1, ..., src_arrn)
Copies data from all source arrays into destination array.
Type of all arrays' elements should be same.  
Destination array should be long enough to fit all data from source arrays.
```c
int a1[] = {0,1,2,3};
int a2[] = {4,5,6,7};

int b[ ARRAYS_SIZE(a1, a2) ];
copy_arrays(b, a1, a2);

print_array(b); //01234567
```
### make_merged_array(var_name, arr1, arr2, ..., arrn)
Creates an array with name (var_name) with size of all provided arrays and copies all these arrays' data into it.
All source arrays should have same type.
If at least one of source arrays is VLA or pointer to VLA, then merged array will be VLA too.
```c
make_merged_array(data,
                 ((int[]){0,1,2,3}),
                 ((int[]){4,5,6}),
                 ((int[]){7,8,9,10,11}));

print_array(data); //prints: 01234567891011
```
## Array Iterators
### foreach_array_ref(var, ref_name)
Iterate over an array (var) by declaring variable (ref_name) as a pointer to current array element.
```c
uint16_t test[] = {1, 2, 3, 4};
foreach_array_ref(test, val)
    print(*val);  //prints: 1234
```
### foreach_array_ref_bw(var, ref_name)
Iterate over an array (var) backwards by declaring variable (ref_name) as a pointer to current array element.
```c
uint16_t test[] = {1, 2, 3, 4};
foreach_array_ref_bw(test, val)
    print(*val);  //prints: 4321
```
## ArrayView
A view of an array. It is just a pointer to array pointing into some part of another array.   
Arguments are statically validated by default, prohibiting creating out-of-bounds views at compile time.  
it is somewhat similar to C++ std::span and rust slices.

### (make_)arrview(idx, len, var)
Creates a view of array var, starting from position (idx) with len (len)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview(data_slc, 2, 3, data); //Start index: 2 and size: 3  
print_array(data_slc); //prints: 234
```  
### (make_)arrview_first(len, var)
Create a view of the first (len) elements of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview_first(data_slc, 3, data);  
print_array(data_slc); //prints: 012
```
### (make_)arrview_last(len, var)
Create a view of the last (len) elements of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview(data_slc, 2, data);  
print_array(data_slc); //prints: 45
```
### (make_)arrview_shrink(start, end, var)
Create a view by skipping (start) elements from the begining and (end) elements from the end of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview_shrink(data_slc, 1, 2, data);  
print_array(data_slc); //prints: 123
```
### (make_)arrview_cfront(start, var)
Create a view by skipping (start) elements from the begining of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview_cfront(data_slc, 2, data);  
print_array(data_slc); //prints: 2345
```
### (make_)arrview_cback(end, var)
Create a view by skipping (end) elements from the end of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview_cback(data_slc, 2, data);  
print_array(data_slc); //prints: 0123
```
### (make_)arrview_full(var)
Create a full view of the array (var)
```c
uint8_t data[] = {0,1,2,3,4,5};  
make_arrview_full(data_slc, data);  
print_array(data_slc); //prints: 012345
```
### (make_)arrview_str(var)
Create a view of the array (var) without last element. Useful for null-terminated strings.
```c
make_arrview_str(hello, "Hello");
const char space[1] = {' '};
make_arrview_str(world, "World");
const char null[1] = {0};

make_merged_array(str, hello, space, world, null);
println(str); //Hello World
```
## Array Access helpers
### array_first_ref(var)
Returns a pointer to first array element
### array_last_ref(var)
Returns a pointer to last array element
### array_end_ref(var)
Returns a pointer to past the last array element. It is valid pointer, but it should not be dereferenced
### array_ref_index(var, ref)
Returns index of array element where (ref) points 
### is_first_array_ref(var, ref)
Returns true if (ref) points to the first element of the array (var)
### is_last_array_ref(var, ref)
Returns true if (ref) points to the last element of the array (var)
### is_end_array_ref(var, ref)
Returns true if (ref) points to the past the last element of the array (var)
