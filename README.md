# stress-test
A background program to stress a host - concurrency, memory, i/o 
Runs resource-hungry jobs to test a foreground process for concurrencly, resource issues, etc.

## extending the run time
As of 71409dae56b1b46372f198a43407dc2d3b549938 there is no way to specify the runtime of the fio tests using the command line, but you can extend (or shorten) the runtime by editing the files in fio-tests
