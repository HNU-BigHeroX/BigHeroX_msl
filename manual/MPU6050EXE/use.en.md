> Author: Hcm
>
> Date: August 27, 2023

# MPU6050 0.1.exe Usage Guide

> Language: [中文](use.md) | **English**

Where the software is located:

![image-20230827220716321](use.assets/image-20230827220716321.png)

![image-20230827220730107](use.assets/image-20230827220730107.png)

![image-20230827220745375](use.assets/image-20230827220745375.png)

What the software looks like:

![image-20230827215726617](use.assets/image-20230827215726617.png)

![image-20230827215732122](use.assets/image-20230827215732122.png)

This software connects to the MPU module on the robot and can read information such as the robot's attitude. Normally, first check **Device Manager on the current computer**. Here, "current computer" means the onboard computer.

![image-20230827215909681](use.assets/image-20230827215909681.png)

![image-20230827220104836](use.assets/image-20230827220104836.png)

Usually there are two USB devices. In this example, they are `COM3` and `COM4`. Enter either COM port in the software interface. When you see the display shown below, it means the connection is correct. The other serial port is the one used to communicate with the robot chassis.

![image-20230827220257559](use.assets/image-20230827220257559.png)

This software is very simple to use. It is mainly used to test which of the two ports transmits MPU data and which one communicates with the chassis.
