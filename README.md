# Reinforcement Learning Rocket Centering
A deterministic physics simulation (dependent on initial parameters) that uses reinforcement learning via genetic programming.


## The Simulation
An initial velocity and position is provided to a rocket, and the program subsequently determines an optimal thrust application in order to get the rocket to reach a certain position, and attain a zero velocity at that position.

### Running the Program
To run the program, execute the following lines in the terminal.
```
make
./ExecuteCentering
```

### Program Output
Two key outputs are produced by this program. First, the optimal solution algorithm is displayed, and then an animation plays which demonstrates the effect of the computed solution on the rocket. 

### Changing the program
If you want to view the effect of different initial parameters for the physics simulation, do the following.

Open:
```
main.cpp
```
Go to line 659:
```
mt19937 rng(42)
```
Change the rangom number generator to any desired seed, and recompile and run the program to view the changes to the simulation.


