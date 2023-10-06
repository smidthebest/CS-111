# A Kernel Seedling
Lab 0: A Kernel method which returns to the user the number of processes which are running on the machine currently. 

## Building
```shell
make
sudo insmod proc_count.ko 
```
To run the kernel, we first need to build the necessary modules and components first. The makefile does this first through the 
make command. Next, we need to insert the module we have created into the kernel so that it can run all the code that we have 
written. 

## Running
```shell
cat /proc/count
```
This command simply returns the output of the kernel code which we have run. It dumps the contents of the /proc/count file 
into standard output so that the user can read the number of processes running on the machine. 

## Cleaning Up
```shell
sudo rmmod proc_count
make clean
```
The first command simply removed the proc_count module from the kernel so that we can re add the proc_count kernel again with new 
code and update the process. The second command simply removes all the components and modules which were built by the original 
make command and allows the user to rebuild the kernel code again. 

## Testing
```python
python -m unittest
```

Report which kernel release version you tested your module on
(hint: use `uname`, check for options with `man uname`).
It should match release numbers as seen on https://www.kernel.org/.

```shell
uname -r -s -v
```
TODO: kernel ver?