
# Arduino-Automation-car


The handout for the project is:

Design and build an automatic robot hardware that
- explores the walled maze;
- stops at the treasure inside the maze and takes picture(s)
of the treasure;
- leaves the maze and returns to the starting position;
- records the exploration and stores information for other
parts of the system;

Extra functions:
- explores a more complicated maze
(e.g. larger maze, with multiple splits/branches).
- self-designed other functions.
## Hardware

#### Power

- Four wheels each with a motor
- Every two motors on one side is
controlled by one l298n
- Each l298n connects to a same independent power source

#### Sensor

- Three kinds of sensors: ultrasonic sensor, Collision sensor, Color sensor
- Mainly depend on three Ultrasonic
sensors
- The motherboard supply their power
Uniformly
- Bluetooth module can send data from motherboard to computer

## Algorithm

#### Exploration Strategy

Set a hyperparameter TOLERANCE representing the minimum distance to the wall:

    if distanceForward < TOLERANCE (the robot’s head is too close to wall)
        Move(BACKWARD)
        If distanceLeft > distanceRight (the robot’s right is too close to wall)
            Move(LEFT)
        Else
            Move(RIGHT)


