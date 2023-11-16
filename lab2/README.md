# You Spin Me Round Robin

UID: 305935705

This is a project in simulating a Round Robin scheduler. 

## Building

Run `make` to build the exectuable. 

## Running

To run the executable simply run 
```
./rr process.txt [quantum length]
```
 where processes.txt contains the
defintion of all the processes in when they arrive and how long they run for. The quantum length is a number or
can be specified to be the median which calculates it based on the previous processes. 

The results will show up as 
```
Average wait time: 0.75
Average response time: 0.75
```
where the wait time is average wait time each process had before it was finsihed processing and where the average response time is the time each process had to wait before it was first processed.  

## Cleaning up

```shell
make clean
```
