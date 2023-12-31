# Hash Hash Hash
Siddhartha Mishra
305935705
This is a lab to test hash tables with different levels of locking to enable multithreaded use. 

## Building
Just simply run the make command in the root lab3 directory. 
```shell
make
```

## Running
Run the hash-table-tester script with the t option to specify number of threads and s option to specify number of insertions. 
```shell
./hash-table-tester -t 8 -s 50000
```

## First Implementation
In the `hash_table_v1_add_entry` function, I added a single mutex to lock the entire hash table so only one thread could only access it at a time. 

### Performance
Run the hash table tester script and then look at the second response. That specifies the response times for the single mutex approach. 
```shell
./hash-table-tester -t 8 -s 50000
```
This is a sample response for v1: 
```
Hash table v1: 2,573,719 usec
  - 0 missing
```
Version 1 is a little slower than the base version. As a result we went with v2: 

## Second Implementation
In the `hash_table_v2_add_entry` function, I used multiple locks for each bucket of the hash table to allow multiple threads to access the hash table at once as long as they are operating on different data. 

### Performance
Run the hash table tester script and then look at the third response. That specifies the response times for the multiple mutex approach. 
```shell
./hash-table-tester -t 8 -s 50000
```
This is a sample response for v1: 
```
Hash table v2: 298,294 usec
  - 0 missing
```
the v2 was a consistent 1-3x improvement on the baseline depending on the number of cores in use. In particular, I want to further analzye how the different cores on my machine impact the performance of the multiple mutex approach. 

## Cleaning up
Just run make clean to delete all the object files that are generated by make. 
```shell
make clean
```