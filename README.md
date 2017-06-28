# MPI_rebalance_application_cpp
Author : Yu Zhang

# 1. Expected user case
This application simulate how to balance the work dynamicly among different MPI processes where each process is assigned unbalanced work initially. In detail, the load balancer form a "master and slave" pattern. And the master dynamicly find the free slaves to assign the tasks to them. A sleep function is added to simulate that different severs may have differnt performance. So some slaves may finish many tasks according to their ability, which is much better than static assignment. 

# 2. Compile environment
## 1) Linux and Windows
The application is developed and tested in Linux. Related header file is also included in the source file. To run on a Windows system, please comment "#define LINUX" and uncomment the "#define WINDOWS". Then rebuild the project and run.
 
## 2) Eclipse and command line
The program could be compiled with both Eclipse and command line. Please make sure your computer has install the MPI related library. 
As for Eclipse, use "build all".
As for command line, use "mpic++".

# 3. Runtime settings
## 1) Run with Eclipse
To run with Eclipse, you need to install MPICH and set the library and path correctly. Then in "run configuration", set the application to "your_related_path/mpirun". In "Arguments", write "-np 4 ${workspace_loc:MPI_rebalance_application_cpp}/Debug/MPI_rebalance_application_cpp". This represent the program will run with 4 processes. As we form a "master-slave" pattern in this program, at least 2 processes should be created. 

## 2) Run with command line
To run with command line, you could use the following command if you are in the "MPI_rebalance_application_cpp" folder.
mpirun -np 4 Debug/MPI_rebalance_application_cpp
Or if you compile the program in other places, use the similar command.
