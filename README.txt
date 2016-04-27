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
    PATHFINDING

Simulation Structure:
    -Car arrives
        - Check for room in lane
        - Update state for lane that car arrived in
        - Check if this is the destination
        - Statistics
        - Swap car direction variable WEST/EAST
        - Schedule departure
            - Traffic light
    -Car departs
        - Figure out the direction to go to get closer to destination
            - Seek x first then y
                -unless lane to seek is full
                    - Seek random direction
                -or would result in u-turn
                    - Seek other random direction
                    - unless last resort
        - Schedule arrival event
            - function of lane capacity
                - lane capacity * constant


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
