# MAVSDK demos

The prototype is designed to work on a Linux Ubuntu box.

These instructions have been tried on Ubuntu 20.04 and 18.04 and can't be guaranteed to work in different environments.  It is still possible for the prototype to  be  used in MacOS and even Windows with some adjustments, but this is not recommended.

The purpose of the prototype is to develop a mission application. The main functional requirements are listed below:

-   Detect a quadcopter running in jmavsim and/or gazebo and connect to it
-   Open a mission .plan file, created with QGroundcontrol.
-   Extract mission segments and send segments to the vehicle for execution
-   Log telemetry information from the quadcopter in the mission app
-   Observe drone movements and access data from QGroundcontrol

Other requirements for the developed code are listed here:

-   Code shall leverage the MAVSDK library and will be based on fly\_qgc\_mission and offboard\_velocity examples
-   The code shall provide C wrapper function to hide the C++ implementation of  MAVSDK in order to interface easier with the code generated from the AADL model
-   The developed prototype will have a README.md file with instructions on how to run it and an overview of the design
-   The developed prototype shall be reusable in communicating with real drone in the future phases of the development

## Components

1.  GAZEBO - Gazebo is a physics simulator that needs to be installed additionally and works with PX4 and MAVSDK

https://dev.px4.io/v1.9.0/en/simulation/gazebo.html

2.  MAVSDK- This SDK allows for control of one or multiple vehicles, simulated or physical and provides C++ classes which make the task possible with less effort.

https://mavsdk.mavlink.io/develop/en/index.html

3.  QGROUNDCONTROL  - Allows for creation of mission files and monitoring of quadcopter in flight. Provides logs for flight analysis.

http://qgroundcontrol.com/

## Installation Procedure

### Prerequisites:

Make sure you have java 8 installed and selected as default java. On
Linux one can use the following command:

```
sudo update-alternatives --config java
```


1.  Install PX4 if you need jmavsim only. If you need the gazebo simulator, then skip to step 2. Follow the instructions on this page:

Getting PX4 from git can be done with the following command:

```
git clone https://github.com/PX4/Firmware.git
```

Building px4 and the jmavsim simulator is done with this command:

```
make px4\_sitl jmavsim
```

Full instructions are given here: https://dev.px4.io/v1.9.0/en/setup/building_px4.html

2.  Install Gazebo]

Download https://raw.githubusercontent.com/PX4/Devguide/v1.9.0/build_scripts/ubuntu_sim.sh

Run the script in a bash shell: `source ubuntu_sim.sh`

The instructions for Ubuntu are here:

If the command 'make px4\_sitl gazebo' does not succeed install the following library:

```
sudo apt-get install libgstreamer-plugins-base1.0-dev
```

3.  Install MAVSDK

Instructions for Linux are here:

https://mavsdk.mavlink.io/develop/en/contributing/build.html

Follow build and install sections for Linux.

4.  Install QGroundControl

Follow the instructions for Ubuntu Linux here:

https://docs.qgroundcontrol.com/en/getting_started/download_and_install.html

5.  Check installation

The easiest way to check your installation is to run the takeoff_land
example.

-   cd MAVSDK/examples/takeoff\_land
-   mkdir build
-   cd build
-   cmake ..
-   make
-   start a simulator in a separate shell by choosing one of the
    simulators jmavsim or gazebo:

```
-   make px4_sitl jmavsim
-   make px4_sitl gazebo

```

-   run the example:

```
./takeoff\_and\_land udp://:14540
```

-   For a more elaborate example follow the same steps and run fly\_mission example
-   Optionally start QGroundControl.AppImage and see it discovering the vehicle and monitoring the mission.

### Phases

#### Simulation

The simulation phase shall demonstrate that a mission application can
talk to the gazebo simulator as described in the installation procedure.
The simulator, the mission application and QGroundControl will be run as
separate processes:

-   make px4\_sitl gazebo
-   ./mission\_app udp://:14540
-   ./QGroundControl.AppImage

The .plan file is going to be created in advance by QGroundControl. Running QGroundControl is optional.

#### Drone implementation

The drone procedure is going to be very similar to the
simulation:

-   ./QGroundControl.AppImage
-   Start px4 on drone
-   ./mission\_app udp://:14540

Additional functionality for hardware and software implementation will be determined in the second phase of the project.
