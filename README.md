
<p align="center"><img src="https://i.postimg.cc/tRKJ5w9C/Pathfinder.jpg" alt="Pathfinder.jpg" width="480" height="121"></p>
<p align="center">Project for autonomous public transportation</p>

[![Arduino CI](https://github.com/DIT112-V21/group-07/actions/workflows/Arduino-Build&Test.yml/badge.svg?branch=master&event=push)](https://github.com/DIT112-V21/group-07/actions/workflows/Arduino-Build&Test.yml)
[![Arduino CI](https://github.com/DIT112-V21/group-07/actions/workflows/Arduino-Build&Test.yml/badge.svg?branch=master&event=pull_request)](https://github.com/DIT112-V21/group-07/actions/workflows/Arduino-Build&Test.yml)

***

### Contents
- [Introduction](#introduction)
- [App Installation](#app-installation)
- [Dependencies](#dependencies)
- [Testing](#testing)
- [What?](#what)
- [Why?](#why)
- [How?](#how)
- [Developers](#developers)
- [Credits](#credits)
- [License](#license)

***

### Introduction
The purpose of the PathFinder smart vehicle is to offer a digitalized experience to public transportation companies and their customers. We aim to do so by offering:

to drivers the possibility to control the vehicle from their phone and remotely
to passenger's interaction with the bus directly through their phone.

Please refer below pages  for a complete tutorial of the system; 
- [User manual](https://github.com/DIT112-V21/group-07/wiki/User-Manual)
- [Project charter](https://github.com/DIT112-V21/group-07/wiki/Project-Charter)
- [Wiki pages](https://github.com/DIT112-V21/group-07/wiki)

***

### App Installation

Installation guidance are provided in detail on the Wiki pages below.

- To install the application, please proceed to [user installation](https://github.com/DIT112-V21/group-07/wiki/User-installation-&-setup-guidance).

- To install the application, please proceed to [developer installation](https://github.com/DIT112-V21/group-07/wiki/Developer-Installation).

***

### Dependencies

- [Arduino IDE](https://www.arduino.cc/en/software)
- [SMCE-gd](https://github.com/ItJustWorksTM/smce-gd)

***

### SMCE Installation
**SMCE** : is an external virtual environment software that provides the possibility to run your software on, for more details on how to install and run it in different operating systems, please visit this link [SMCE](https://github.com/ItJustWorksTM/smce-gd/wiki).

***

### Smartcar_Shield

The smart-car functionalities in Pathfinder utilizes the open source library [smartcar_shield](https://github.com/platisd/smartcar_shield).

***

### Testing

To run the unit tests for the back-end, please refer the link below;
 - [Arduino unit testing](https://github.com/DIT112-V21/group-07/wiki/Guidance-for-unit-testing)

***

### What?
Pathfinder is a remote-operated bus that is controllable via an Android app. The bus features two different screens, depending on the role of the user. 

The passenger screen features buttons that send stop and accessibility requests to the driver. These buttons may be toggled on and off, to allow the passenger to cancel their request as desired.

The driver is first introduced to a log-in screen to check their credentials.  Upon authorization, only then are they allowed access to the driver dashboard which features the controls for the bus. Initially, the driver is greeted with a live video feed streaming from the bus in order to grant them a real-time view of their course. 

A joystick is present to maneuver the bus along with basic obstacle avoidance features that will steer clear of any obstructions that may be present on the road. A brake is available in order to quickly put the bus to a halt. A speedometer and odometer is shown to keep the driver inform as they navigate their path. 

The driver screen also features cruise control, which lets the bus drive at the desired speed; the speed of the bus may also be limited as specified on the seek bar on the screen. The bus may be set to park, which would not allow the bus to drive when it is toggled on. There is also a next stop button, which lets the driver indicate that they are approaching the next stop in the route they are traversing. Another feature that isn’t as obvious, is that the bus stops when it loses connection to the car — connectivity loss to the app will render the bus to pull over and stop by the curb. Finally, the driver may also log out of the app to better secure the state of the bus. 

On both the passenger and driver screens, the traversed bus route is displayed, along with its associated stops. These stops are updated as instructed by the driver, in order to inform all passengers the next stop they are approaching are along the route.

***

### Why?
The COVID-19 pandemic has had a devastating impact on life, as we know it.  Social distancing measures have been taken to minimize its spread, encouraging all but essential workers to work remotely from home.  

The Pathfinder app allows bus drivers to execute their work from home. Equipped with a live video stream and functionalities that control the bus from afar, drivers may now perform their duties without risking their health and those of others.  

Furthermore, passengers may utilize the stop button controls available on the app which are traditionally present and shared by all bus passengers; with the buttons existing on passengers’ own phones, the spread of germs can be significantly minimized to avoid contraction of COVID-19.

***

### How?
1. Have a page that allows the user to select their role as either a driver or a passenger
2. Authenticate drivers prior to redirecting them to the driver dashboard
3. Have as many controls available on a physical car on the app, to drive the car as easily and seamlessly as possible
4. Display all stop requests for every stop that is shown
5. Update all stops as instructed by the driver
6. Passengers will have buttons available to them that will indicate if the want to stop, or if the need more amenities available to them.

***
### Developers
* [Alexandre Rancati-Palmer](https://github.com/alrapal)
* [Bassam Eldesouki](https://github.com/bassamEldesouki)
* [Christopher Axt](https://github.com/gusaxtcha)
* [Ediz Genc](https://github.com/edizgenc2021)
* [Gianmarco Iachella](https://github.com/iachella)
* [Julia Van Kirk](https://github.com/juliavankirk)

***

### Credits
We would like to thank everyone who had even a small contribution in this project. We appericiate all support we have had along the way. 
Please follow the [credits](https://github.com/DIT112-V21/group-07/wiki/Credits) for our gratitude. In case we forgot to thank anyone, please forgive us. :)

***

### License
MIT © Group-07

The source code for the repository is licensed under the MIT license, refer to [LICENSE](https://github.com/DIT112-V21/group-07/wiki/License) file in the repository.

***