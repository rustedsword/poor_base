# poor_base
Header only macro library

# <poor_array.h>
This header contains useful macros to work with arrays.
All macros here can operate on arrays or pointers to arrays by auto-dereferencing them using auto_arr()
while checking that arguments are really arrays.

## Generic Array Macros

### auto_arr(var) / arr(var)
Allows to work with pointers to arrays in the same way as with ordinary arrays

If argument is a pointer to an array then this macro will dereference it.  
If argument is an array, then this macro does nothing at all, returning array as is.  
If argument is not an array or a pointer to an array then compilation will be stopped.  

> int a[2] = {1,2};  
> int (*b)[2] = &a;  
> arr(a)[0] = 5;   
> arr(b)[1] = 6; 

Used by other macros, so they can operate on arrays or pointers to arrays at the same time.

### ARRAY_SIZE(var)
Returns count of elements in the array.

> int a[5];   
> int (*b)[2];  
> ARRAY_SIZE(a); //5  
> ARRAY_SIZE(b); //2  

### ARRAY_SIZE_BYTES(var)
Returns total array size

### ARRAY_ELEMENT_SIZE(var)
Returns size of array element

### PRINT_ARRAY_INFO(var)
Prints information about array

> int a[2];   
> PRINT_ARRAY_INFO(a); //Array "a" at 0x0 has size:2 uses 8 bytes, while single element has size:4  
> 
> size_t len = 5;   
> short b[len];   
> PRINT_ARRAY_INFO(b); //VLA "b" at 0x0 has size:5 uses 10 bytes, while single element has size:2 

### copy_array(dst_arr, src_arr)
Copies data from array (src_arr) to array (dst_arr)  
Type of array elements may be compatible.  
If destination array is not long enough then only part of source array will be copied.  

### copy_arrays(dst_arr, src_arr0, src_arr1, ..., src_arrn)
Copies data from all source arrays into destination array.
Type of all arrays' elements should be same.  
Destination array should be long enough to fit all data from source arrays.

## ArrayView
A view of an array. It is just a pointer to array pointing into some part of another array.  
You can create a view from an array or from another view. View can be created from VLA or from a pointer to VLA.  
Arguments are statically validated by default, prohibiting creating out-of-bounds views at compile time.  
Optional run-time arguments validation.  

### (make_)arrview(idx, len, var)
Creates a view of array var, starting from position (idx) with len (len)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview(data_slc, 2, 3, data); //Start index: 2 and size: 3  
>  print_array(data_slc); //prints: 234
  
### (make_)arrview_first(len, var)
Create a view of the first (len) elements of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview_first(data_slc, 3, data);  
>  print_array(data_slc); //prints: 012

### (make_)arrview_last(len, var)
Create a view of the last (len) elements of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview(data_slc, 2, data);  
>  print_array(data_slc); //prints: 45

### (make_)arrview_shrink(start, end, var)
Create a view by skipping (start) elements from the begining and (end) elements from the end of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview_shrink(data_slc, 1, 2, data);  
>  print_array(data_slc); //prints: 123

### (make_)arrview_cfront(start, var)
Create a view by skipping (start) elements from the begining of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview_cfront(data_slc, 2, data);  
>  print_array(data_slc); //prints: 2345

### (make_)arrview_cback(end, var)
Create a view by skipping (end) elements from the end of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview_cback(data_slc, 2, data);  
>  print_array(data_slc); //prints: 0123

### (make_)arrview_full(var)
Create a full view of the array (var)

>  uint8_t data[] = {0,1,2,3,4,5};  
>  make_arrview_full(data_slc, data);  
>  print_array(data_slc); //prints: 012345

### (make_)arrview_str(var)
Create a view of the array (var) without last elements. Useful for null-terminated strings.
