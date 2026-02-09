## Easy PID Motor Controller (EPMC) C++ Library
This library helps communicate with the **`Easy PID Motor Controller Module`** (i.e **`EPMC MODULE`**) in your PC or microcomputer linux c++ robotic project (as it depends on the libserial-dev package), with the [epmc_setup_application](https://github.com/robocre8/epmc_setup_application).

> you can use it in your microcomputer robotics project **running on linux** (e.g Raspberry Pi, PC, etc.)

A simple way to get started is simply to try out and follow the example code in the example folder


## How to Use the Library (build from source)
- install the libserial-dev package
  > sudo apt-get update
  >
  > sudo apt install libserial-dev

- Ensure you have the **`EPMC MODULE`** interfaced with your preferred motors, setup the encoder and PID parameters with the **`epmc_setup_application`**.

- Download (by clicking on the green Code button above) or clone the repo into your PC using **`git clone`**
> [!NOTE]  
> you can use this command if you want to clone the repo:
> 
> ```git clone https://github.com/robocre8/epmc_serial_cpp.git```

- check the serial port the driver is connected to:
  ```shell
  ls /dev/ttyU*
  ```
  > you should see /dev/ttyUSB0 or /dev/ttyUSB1 and so on

- A simple way to get started is simply to try out and follow the example `motor_control.cpp` code.

- make, build and run the example code.
  > cd into the root directory
  >
  > mkdir build (i.e create a folder named build)
  >
  > enter the following command in the terminal in the root folder:
    ````shell
    cmake -B ./build/
    ````
    ````shell
    cmake --build ./build/
    ````
    ````shell
    ./build/motor_control
    ````

- You can follow the pattern used in the example `motor_control.cpp` in your own code.

#

## Basic Library functions and usage (Four Motor Support Control)

- connect to epmc_driver shield module
  > epmc_serial::EPMCSerialClient controller
  >
  > controller.connect("port_name or port_path")

- clear speed, position, e.t.c data buffer on the EPMC module
  > controller.clearDataBuffer() // returns bool -> success

- send target angular velocity command
  > controller.writeSpeed(motor0_TargetVel, motor1_TargetVel, motor2_TargetVel, motor3_TargetVel)

- send PWM command
  > controller.writePWM(motor0_PWM, motor1_PWM, motor2_PWM, motor3_PWM)

- set motor command timeout
  > controller.setCmdTimeout(timeout_ms)

- get motor command timeout
  > controller.getCmdTimeout() // returns std::tuple -> bool, float

- read motors angular position and angular velocity
  > controller.readMotorData() // returns std::tuple -> bool, std::vector<float-size8> (pos0, pos1, pos2, pos3, vel0, vel1, vel2, vel3)

- read motors angular position
  > controller.readPos() // returns std::tuple -> bool, std::vector<float-size4> (pos0, pos1, pos2, pos3)

- read motors angular velocity
  > controller.readVel() // returns std::tuple -> bool, std::vector<float-size4> (vel0, vel1, vel2, vel3)

- read motor maximum commandable angular velocity
  > controller.getMaxVel(motor_no) // returns std::tuple -> bool, float
  > maxVel0 or maxVel1 based on the specified motor number

- while these function above help communicate with the already configure EPMC module, more examples of advanced funtions usage for parameter tuning can be found in the [epmc_setup_application](https://github.com/robocre8/epmc_setup_application) source code