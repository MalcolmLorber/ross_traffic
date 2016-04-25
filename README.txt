ROSS traffic simulation


Contributors
-Ezra Dowd
-Alex Mohr
-Malcolm Lorber


Simulation state:
    Simualation info:
    -north in/out
    -south in/out
    -east in/out
    -west in/out

    Simulation stats:
    -num cars finished here
    -num cars arrived here
    -average waiting time

Simulation events:
    ARRIVAL - north/south/east/west
    DEPARTURE - north/south/east/west

Message:
    message type (simulation event type)
    destination

Task List:
    -Finish working out random stuff that isn't the simulation
        -Figure out run time arguments
    -Do event handler function
        -Work out routing logic
        -Work out scheduling logic
            -How far in the future to schedule departure events
    -Do reverse handler function
        -Need to do event handler first
    -Do finalization function
        -What stats to we want to aggregate
