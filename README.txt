ROSS traffic simulation

A simulation of traffic congesting using the ROSS parallel discrete event simulator.

Contributors
-Ezra Dowd
-Alex Mohr
-Malcolm Lorber

Setup Instructions:

First setup ROSS according to the instructions at https://github.com/carothersc/ROSS/wiki/Installation, including submodules.
Place the contents of this folder in ROSS/models/ROSS-Models/ross_traffic/
Re-run the cmake -DROSS_BUILD_MODELS=ON ../ROSS command in the ross-build directory
Run make in ross-build/models/ROSS-Models/ross_traffic (NOTE THATTHIS IS IN THE BUILD DIRECTORY)

Running instructions:

ross_traffic has many runtime options, to view them run ./ross_traffic --help
Sample run commands:
mpirun -np 4 ./ross_traffic --synch=3 --grid_size=128 --initial_cars_per_intersection=2
mpirun -np 8 ./ross_traffic --synch=3 --grid_size=256 --initial_cars_per_intersection=4 --memory=15000
