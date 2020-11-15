# poor_base
Header-only library for C with macros for type-generic printing and advanced array operations.
1. [Basic Example](#basic-example)
2. [Headers]
   1. [poor_stdio.h](#poor-stdio-h)
   2. [poor_array.h](#poor-array-h)
4. [Arrays in C Language](#arrays-in-c-language)


# Basic Example
```c
#include <poor_array.h>
#include <poor_stdio.h>

#define do_something(data) _do_something(ARRAY_SIZE(data), &auto_arr(data))
static int _do_something(size_t len, int (*d)[len]) {
    if(!d || ARRAY_SIZE(d) < 6)
        return printerrln("Too small array"), -1;

    make_arrview_first(d_first, 2, d);
    make_arrview_shrink(d_middle, 2, 3, d);
    make_arrview_last(d_last, 3, d);

    copy_array(d_first, (const int[]){-3, -4});
    fill_array(d_middle, 0);
    foreach_array_ref(d_last, ref)
        *ref = array_ref_index(d, ref) + 1;

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
Copy paste this example into test.c and compile
```
git clone http://192.168.180.151:3000/rustedsword/poor_base.git
gcc test.c -o test -I poor_base/include/
```
```
# ./test 
[-3,-4,0,0,5,6,7]
d:7 d_first:2 d_middle:2 d_last:3

# ./test 1 1
[-3,-4,0,0,0,0,7,8,9]
d:9 d_first:2 d_middle:4 d_last:3

```
# poor_stdio.h
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

# poor_array.h
This header contains useful macros to work with arrays.
All macros here can operate on arrays or pointers to arrays by auto-dereferencing them using auto_arr() macro,
while checking that arguments are really arrays.


### auto_arr(arrm) / arr(arrm) macro
Allows to work with pointers to arrays in the same way as with ordinary arrays

If argument is a pointer to an array then this macro will dereference it.  
If argument is an array, then this macro does nothing at all, returning array as is.  
If argument is not an array or a pointer to an array then compilation will be stopped.  

```c
int a[2] = {1,2};  
int (*b)[2] = &a;  
auto_arr(a)[0] = 5;   
auto_arr(b)[1] = 6; 
print_array(a); //[5,6]
print_array(b); //[5,6]
```
### Array informational macros

macro                        | description
-----------------------------|-------------------------------------------
ARRAY_SIZE(arrm)             | returns number of elements in the single array
ARRAYS_SIZE(arrm, ...)       | returns total number of elements in all arrays
ARRAY_SIZE_BYTES(arrm)       | returns number of bytes in the array
ARRAYS_SIZE_BYTES(arrm, ...) | returns total number of bytes in all arrays
ARRAY_ELEMENT_SIZE(arrm)     | returns number of bytes in a single array element
ARRAY_ELEMENT_TYPE(arrm)     | returns a type of array element
PRINT_ARRAY_INFO(arrm)       | prints information about array
print_array(arrm)            | prints array in form of [1,2,3,4]

```c
int32_t a[5] = {1,2,3,4,5};
println(ARRAY_SIZE(a)); //5

int32_t (*b)[5] = &a;
println(ARRAY_SIZE_BYTES(b)); //20

long c[(size_t){3}];
println(ARRAYS_SIZE(b, c)); //8

PRINT_ARRAY_INFO(c); //VLA "c" at 0x7ffe9a90b4a0 has size:3 uses 12 bytes, while single element uses 4 bytes

print_array(b); //[1,2,3,4,5]
```

### Array allocation

macro                  | description
-----------------------|-------
make_array_ptr(name, ptr, size) | declares a pointer to array by using a pointer to single element and size
malloc_array(arrp)     | allocates memory for a pointer to an array
calloc_array(arrp)     | allocates zero initialized memory for a pointer to an array
memset_array(arrm, sym)| fills entire array with specified symbol
fill_array(arrm, val)  | fills entire array with specified value

```c
short (*data)[3] = malloc_array(data);
if(data) {
    fill_array(data, -1);
    print_array(data); //[-1,-1,-1]
    free(data);
}

const char *n = "string";
make_array_ptr(n_arr, n, 6);
print_array(n_arr); //[s,t,r,i,n,g]
```

### Array iterators and accessors

macro                                | description
-------------------------------------|-----------------------
foreach_array_ref(arrm, ref_name)    | array iterator
foreach_array_ref_bw(arrm, ref_name) | array reverse iterator
array_first_ref(arrm)                | returns a pointer to the first array element
array_last_ref(arrm)                 | returns a pointer to the last array element
array_end_ref(arrm)                  | returns a pointer to the one past-the-last array element
is_first_array_ref(arrm, ref)        | returns true if ref points to the first array element
is_last_array_ref(arrm, ref)         | returns true if ref points to the last array element
is_end_array_ref(arrm, ref)          | returns true if ref points to the one past-the-last array element
array_ref_index(arrm, ref)           | returns index of array element

```c
int x[] = {1,2,3};
*array_first_ref(x) = 9;
*array_last_ref(x) = 6;

foreach_array_ref(x, x_ref) {
    println("index:", array_ref_index(x, x_ref), " value:", *x_ref);
    //index:0 value:9
    //index:1 value:2
    //index:2 value:6
}
```

### Array copy

macro                                | description
-------------------------------------|-----------------------
copy_array(arrm_dst, arrm_src)       | non-overflowing array copy
copy_arrays(arrm_dst, arrm_src, ...) | copies data from multiple arrays into a single one, but may overflow

```c
int a1[3] = {1,2,3};
int a2[2] = {5,6};

copy_array(a1, a2);
print_array(a1); //[5,6,3]

int aa[5];
copy_arrays(aa, a1, a2);
print_array(aa); //[5,6,3,5,6]
```

### Array View

macro                                        | description
---------------------------------------------|-----------------------
make_arrview(name, idx, size, arrm)          | creates an arrview by using index and size
make_arrview_first(name, size, arrm)         | creates an arrview of the first n elements in the array
make_arrview_last(name, size, arrm)          | creates an arrview of the last n elements in the array
make_arrview_shrink(name, skip_start, skip_end, arrm)  | creates an arrview without n first and n last elements of the array
make_arrview_cfront(name, skip_start, arrm)  | creates an arrview without n first elements of the array
make_arrview_cback(name, skip_end, arrm)     | creates an arrview without n last elements of the array
make_arrview_dim(name, size, arrm)           | creates an arrview by splitting top dimentsion into two dimensions
make_arrview_flat(name, arrm)                | creates an arview by merging two top dimensions into one dimension

```c
    const int e[] = {10,20,30,40,50};
    make_arrview(e_view, 1, 3, e);
    make_arrview_first(e_view_first, 2, e);
    make_arrview_last(e_view_last, 2, e);
    make_arrview_shrink(e_view_shrink, 2, 2, e);
    make_arrview_cfront(e_view_cut_front, 2, e);
    make_arrview_cback(e_view_cut_back, 2, e);

    print_array(e_view); //[20,30,40]
    print_array(e_view_first); //[10,20]
    print_array(e_view_last); //[40,50]
    print_array(e_view_shrink); //[30]
    print_array(e_view_cut_front); //[30,40,50]
    print_array(e_view_cut_back); //[10,20,30]
```

### Experimental

These macros are unstable and can be changed anytime

macro                                   | description
----------------------------------------|-----------------------
make_merged_array(name, arrm_src, ...)  | creates a new array and copies data from multiple arrays into it 

```c
int t1[] = {1,2};
int t2[] = {3,4};
make_merged_array(tall, t1, t2);
print_array(tall) //[1,2,3,4]
```

Vector-like array macros

macro                                   | description
----------------------------------------|-----------------------
array_insert(arrm, ref, val)            | inserts value into array
array_insert_array(arrm, ref, arrm_src) | inserts array into array
array_remove_ref(arrm, ref)             | removes single element from array
array_remove_ref_fill(arrm, ref, val)   | removes single element from array and fills free space with value
array_remove_view(arrm, view)           | removes view from array
array_remove_view_fill(arrm, view, val) | removes view from array, and fills free space with value

### Arrays in C Language

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
