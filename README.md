[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/Ut4Ye_b7)

# Homework 3

Dikshant Pratap Singh

12411787

dikshant@uchicago.edu

## Code Structure
The implementation consists of two files C files, `QTree.c` and `barneshut.c`. The `QTree.c` along with its header file `QTree.h` contains the implementation of the Quad Tree. There is also a `plot.py` file for plotting the positions and velocities of particles, and it outputs a GIF file into the folder `result_gifs.`

**`QTree.c`**: It contains the definition of the `Particle` and `QTNode` structs along with the following functions:
1. `create_node` - Creates a new QTNode with the given parent node. If the parent is `NULL`, i.e., an empty tree, it creates a root node with boundaries set using the `root_boundary` method
2. `root_boundary` - sets the four root node boundaries `lb,rb,db,ub` to the min and max, `x` and `y` positions of the particles
3. `which_child_contains` - returns the index of the child which contains the given particle (return `0`,`1`,`2`,`3` or `-1` if error)
4. `qtnode_print` - prints all the members of the given node
5. `is_leaf` - checks if the given node is a leaf, i.e., all children are `NULL`
6. `qTree_insert` - inserts the given node into the tree at its valid position (sub-tree) and simultaneously updates the center of mass of its ancestors
7. `remove_empty_nodes` - removes all empty cells (with 0 particles) from the tree. Invoked after insertion of all nodes into the tree
8. `preorder_traversal` - does preorder traversal of the tree and prints it
9. `height` - returns the height of the quadtree
10. `qTree_free` - frees all memory allocated to the quadtree


**`barneshut.c`**: It contains functionality to simulate particles' behavior for the given number of timesteps. It uses the `QTree` to implement the Barnes Hut Algorithm. It has the following methods:
1. `ran_init` - Initializes the values of positions and velocities to random values between `-5.0` to `5.0`
2. `lattice_init` - Initializes the values of positions to integer `x` and `y` coordinates in a rectangular area. All velocities are initialized to `zero` 
3. `circle_init` - Initializes the values of positions to integer `x` and `y` coordinates such that they lie on the diameter of a circle centered at the origin having radius = `1/sin(2*pi/nParticles)` so that each plot has an appropriate radius. All velocities are initialized to `zero.` 
4. `calc_force_on_particle` - for the given particle in node `n`, it calculates the force on it from all other nodes. Does a DFS search from root QTnode and checks `θ` condition. If it passes, then it calculates force using the Center of Mass, or else it goes into the subtree of the root node. 
5. `update_particles` - Traverses the QTree in a DFS manner, and for each node that contains only one particle, it selects that particle and calculates the force on it from the rest of the particles using the Barners Hut Algorithm by calling the `calc_force_on_particle` function
6. `simulate_timesteps` - Simulates the particle behavior for `nIters` timesteps. For each iteration, it creates a QTree, calculates forces using Barner Hut, and then updates the positions and velocities
7. `run_simulation` - opens an output file `particles.dat` to write. Runs the simulation and writes the particle details for each timestep into the output file. Calls `simulate_timesteps` as a subroutine with FILE pointer to opened result file
8. `run_tests` - Simulates theta values `{0.0, 0.01, 0.05, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1, 1.25, 1.5, 1.75, 2, 4}` and outputs the RMSE values for positions and velocities, along with the % reduction in number of force calculation due to Barnes Hut vs Naive Algo. The number of naive calculations is calculated mathematically.

## Makefile

The provided `Makefile` provides commands for compiling and cleaning all the provided files using the appropriate flags
1. `make` - compiles `QTree.c` and `barneshut.c` and produces the executable `barneshut` or `barneshut.exe` depending upon the OS
2. `make test` - runs the `run_tests` method of `barneshut.c` to give RMSE and % reduction in computation for varying theta values. In the `Makefile`, you can set the `TEST_nParticles` (default: 300) and `TEST_nIterations` (default: 200) to change the number of particles and the number of iterations done.
3. `make plot` - plot the latest output of the BH algorithm using `matplotlib` and store it as a GIF. **Note:** Not available when `test_flag` is set to 1
4. `make clean` - delete the executables and `.o` files from the directory

```console
$ make
Compiling QTree.c...
gcc -c -o QTree.o QTree.c -lm

Compiling barneshut.c...
gcc -c -o barneshut.o barneshut.c -lm

Compiling executable...
gcc -o barneshut QTree.o barneshut.o -lm
```

## Running The Executable

Once `make` command is run, use the executable to run the program.

```console
$ ./barneshut [nParticles] [nIterations] [theta] [init_type] [test_flag]
```

1. `nParticles`: Specify the number of particles (Default = `300`)
2. `nIterations`: Specify the number of iterations or timesteps (Default = `200`)
3. `theta`: Specify the `θ` value for the BH Algorithm
4. `init_type`: How do you want to initialize the points, `{1:'random',2:'lattice',3:'circle'}`. (Default: `1`)

5. `test_flag`: Set to `1` if you want to test RMSE score for varying theta, else set to `0`. (Default: `0`)

**Note:** To run test, **ALL** arguments need to be specified with `test_flag=1` and any value of `theta`.

## Plotting

Run `make plot` to produce the simulation of all the particles for the latest non-test run of `barneshut.c`. The simulation animations are also stored as a GIF in `result_gifs` folder.

**WARNING:** Plotting does not work on `SSH` due to the unavailability of display output. Run `plot.py` only on a local machine.

Sample output plots:

<img src="https://i.imgur.com/9tBRIy0.gif" height=400/>
<img src="https://i.imgur.com/rsAG6mG.gif" height=400/>
<img src="https://i.imgur.com/2KU0oIc.gif" height=400/>

## Testing

Run `make test` to run the test function of `barneshut.c`. For the default values, it gives the following output:

```console
$ make test
Compiling executable...
gcc -o barneshut QTree.o barneshut.o -lm

./barneshut 300 200 0.0 1 1
number of particles = 300
number of iterations = 200

For theta = 0.00:
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 17940000
Percentage reduction in claculations = 0.00 %

RMSE Errors for theta = 0.01:
x  = 198.727295 y  = 258.148399
vx = 142.138074 vy = 161.238347
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 17522132
Percentage reduction in claculations = 2.33 %

RMSE Errors for theta = 0.05:
x  = 66.525590  y  = 178.415361
vx = 48.887678  vy = 115.006712
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 16116347
Percentage reduction in claculations = 10.17 %

RMSE Errors for theta = 0.10:
x  = 96.931437  y  = 192.536411
vx = 64.008103  vy = 118.503850
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 14263151
Percentage reduction in claculations = 20.50 %

RMSE Errors for theta = 0.20:
x  = 547.950962 y  = 235.140129
vx = 328.849194 vy = 154.997294
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 11272375
Percentage reduction in claculations = 37.17 %

RMSE Errors for theta = 0.30:
x  = 2307.442324        y  = 3274.493415
vx = 1478.143146        vy = 2099.258357
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 8955686
Percentage reduction in claculations = 50.08 %

RMSE Errors for theta = 0.40:
x  = 164.419244 y  = 298.207395
vx = 92.861982  vy = 179.036525
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 7190655
Percentage reduction in claculations = 59.92 %

RMSE Errors for theta = 0.50:
x  = 686.478708 y  = 287.438422
vx = 405.680342 vy = 171.651475
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 6079340
Percentage reduction in claculations = 66.11 %

RMSE Errors for theta = 0.60:
x  = 240.108153 y  = 241.417989
vx = 138.067013 vy = 144.449376
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 5323951
Percentage reduction in claculations = 70.32 %

RMSE Errors for theta = 0.70:
x  = 3372.907319        y  = 2811.228493
vx = 2982.241300        vy = 2484.547404
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 4432038
Percentage reduction in claculations = 75.30 %

RMSE Errors for theta = 0.80:
x  = 159.402725 y  = 187.124956
vx = 89.497917  vy = 121.765926
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 3703049
Percentage reduction in claculations = 79.36 %

RMSE Errors for theta = 0.90:
x  = 161.737624 y  = 197.543356
vx = 94.272151  vy = 134.161536
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 3236775
Percentage reduction in claculations = 81.96 %

RMSE Errors for theta = 1.00:
x  = 228.543085 y  = 221.634439
vx = 154.273944 vy = 134.566751
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 2832422
Percentage reduction in claculations = 84.21 %

RMSE Errors for theta = 1.25:
x  = 160.339195 y  = 188.625026
vx = 95.917027  vy = 118.693132
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 2050680
Percentage reduction in claculations = 88.57 %

RMSE Errors for theta = 1.50:
x  = 165.178905 y  = 184.741707
vx = 91.942225  vy = 115.117620
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 1578147
Percentage reduction in claculations = 91.20 %

RMSE Errors for theta = 1.75:
x  = 203.076390 y  = 256.078160
vx = 121.678605 vy = 161.632005
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 1179025
Percentage reduction in claculations = 93.43 %

RMSE Errors for theta = 2.00:
x  = 156.246911 y  = 184.974871
vx = 86.603984  vy = 115.194638
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 628100
Percentage reduction in claculations = 96.50 %

RMSE Errors for theta = 4.00:
x  = 155.575084 y  = 184.264804
vx = 85.596463  vy = 114.456091
naive nbody: #force calculations = 17940000
barnes hut: #force calculations = 272380
Percentage reduction in claculations = 98.48 %


To change the number of particles and iterations, change TEST_nParticles and TEST_nIterations in Makefile.
```

## Correctness

**Qualitative:** From the above simulation plots, we can see that the particles follow the properties of gravitational forces. The stationary particles in a circle collapse into one, and the burst, gravitational slingshot, and other effects can be observed in the plots.

**Quantitative:** We can analyze the output of `make test` to see the impact of theta on the computation cost and error.

As we increase the theta value from `0.01` to `4`, the reduction in the number of computations goes from `0.00%` to `98.48%`, thereby massively improving the running time.

The RMSE scores for positions and velocities for varying theta are very random. In the above output, we see a slight increase in RMSE values as we increase theta. This is expected. However, we also see a sudden massive increase in RMSE for theta equal to `0.3` and `0.7`. This observation is not consistent across other values of `TEST_nParticles` and `TEST_nIterations` that I used to test. So, a better testing criteria and/or parameter is needed to verify correctness. But still, the algorithm's errors remain almost the same even when theta is increased to higher values and thus can be used to simulate gravitational forces satisfactorily.

## Valgrind Memory Leak Check

Running Valgrind on all the data types and executing all functions produced **No Memory Leak**. Below is the Valgrind output:

```console
$ valgrind --leak-check=full ./barneshut 30 200 0.5 3 0
==2486584== Memcheck, a memory error detector
==2486584== Copyright (C) 2002-2017, and GNU GPL'd, by Julian Seward et al.
==2486584== Using Valgrind-3.15.0 and LibVEX; rerun with -h for copyright info
==2486584== Command: ./barneshut 30 200 0.5 3 0
==2486584== 
naive nbody force calcs = 174000
barnes hut force calcs = 96574
==2486584== 
==2486584== HEAP SUMMARY:
==2486584==     in use at exit: 0 bytes in 0 blocks
==2486584==   total heap usage: 19,632 allocs, 19,632 frees, 2,208,984 bytes allocated
==2486584== 
==2486584== All heap blocks were freed -- no leaks are possible
==2486584== 
==2486584== For lists of detected and suppressed errors, rerun with: -s
==2486584== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)
```
## Problem Statement

Implement a serial, two-dimensional version of the Barnes-Hut n-body algorithm as described in recorded lecture and course notes. Note that even the serial, 2D version can be tricky to implement efficiently, so I have provided some pseudocode and guidance in the recorded lectures in addition to a high-level description of the algorithm. There are three main parts and each requires care to get the details right. Step 1 is to build/add particles to the quadtree; Step 2 is to "summarize" the leaf nodes -- i.e., compute the center of mass at each parent; Step 3 is to scan the quadtree depth first left to right using the distance threshold to calculate the forces.

## Submission Requirements

Include a README with your name, the assignment, a list of any references you used, and a discussion of any shortcomings your code may have. You should also provide a Makefile and instructions for compiling and running your code.
