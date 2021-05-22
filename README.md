## Installation
Open your terminal then paste the following command to clone this repository.  Upon cloning, you may now import and simulate our source code into **Android Studio** and test it on the simulator.
```bash
git clone git@github.com:DIT112-V21/group-07.git
```

## What
Pathfinder is a remote-operated bus that is controllable via an Android app. The bus features two different screens, depending on the role of the user. 

The passenger screen features buttons that send stop and accessibility requests to the driver. These buttons may be toggled on and off, to allow the passenger to cancel their request as desired.

The driver is first introduced to a log-in screen to check their credentials.  Upon authorization, only then are they allowed access to the driver dashboard which features the controls for the bus. Initially, the driver is greeted with a live video feed streaming from the bus in order to grant them a real-time view of their course. 

A joystick is present to maneuver the bus along with basic obstacle avoidance features that will steer clear of any obstructions that may be present on the road. A brake is available in order to quickly put the bus to a halt. A speedometer and odometer is shown to keep the driver inform as they navigate their path. 

The driver screen also features cruise control, which lets the bus drive at the desired speed; the speed of the bus may also be limited as specified on the seek bar on the screen. The bus may be set to park, which would not allow the bus to drive when it is toggled on. There is also a next stop button, which lets the driver indicate that they are approaching the next stop in the route they are traversing. Another feature that isn’t as obvious, is that the bus stops when it loses connection to the car — connectivity loss to the app will render the bus to pull over and stop by the curb. Finally, the driver may also log out of the app to better secure the state of the bus. 

On both the passenger and driver screens, the traversed bus route is displayed, along with its associated stops. These stops are updated as instructed by the driver, in order to inform all passengers the next stop they are approaching are along the route.

## Why?
The COVID-19 pandemic has had a devastating impact on life, as we know it.  Social distancing measures have been taken to minimize its spread, encouraging all but essential workers to work remotely from home.  

The Pathfinder app allows bus drivers to execute their work from home. Equipped with a live video stream and functionalities that control the bus from afar, drivers may now perform their duties without risking their health and those of others.  

Furthermore, passengers may utilize the stop button controls available on the app which are traditionally present and shared by all bus passengers; with the buttons existing on passengers’ own phones, the spread of germs can be significantly minimized to avoid contraction of COVID-19.

## How?
1. Have a page that allows the user to select their role as either a driver or a passenger
2. Authenticate drivers prior to redirecting them to the driver dashboard
3. Have as many controls available on a physical car on the app, to drive the car as easily and seamlessly as possible
4. Display all stop requests for every stop that is shown
5. Update all stops as instructed by the driver
6. Passengers will have buttons available to them that will indicate if the want to stop, or if the need more amenities available to them.
