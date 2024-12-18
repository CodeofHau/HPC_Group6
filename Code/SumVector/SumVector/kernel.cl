__kernel void addArray(__global int* array1, __global int* array2) {
    int id = get_global_id(0);
    array1[id] += array2[id];
}
