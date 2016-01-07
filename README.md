# stress-test
A background program to stress a host - concurrency, memory, i/o 
Runs resource-hungry jobs to test a foreground process for concurrencly, resource issues, etc.

## running the tests
execute the run-tests script in the project root directory; it will execute the tests in the background.


## extending the run time
As of 71409dae56b1b46372f198a43407dc2d3b549938 there is no way to specify the runtime of the fio tests using the command line, but you can extend (or shorten) the runtime by editing the files in fio-tests

## prerequisites
1. kernel build tools (git, gmake, gcc, etc.)
2. fio benchmark 

fio is packaged for most distros, and also available at https://github.com/axboe/fio
