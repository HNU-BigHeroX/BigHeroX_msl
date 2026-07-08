# BigHeroX MSL Control Code

![BigHeroX](logo.jpg)

> Language: [Chinese](README.md) | **English**

This repository open-sources the control code and supporting debugging tools used by Team BigHeroX in the RoboCup Middle Size League.

This framework has been developed and tested in competition for about four years. It supported BigHeroX through multiple RoboCup seasons and contributed to the team's strong performance at RoboCup 2024. We are publishing the code, tools, and onboarding documents so that more teams, students, and RoboCup community members can study, reproduce, discuss, and improve the system.

## Repository Contents

```text
.
|-- source_code/   # Robot and coach-side runtime code
|-- tools/         # Debugging, calibration, and field-map tools
|-- manual/        # Onboarding documents and device usage guides
|-- logo.jpg       # BigHeroX logo
`-- README.en.md   # This document
```

### source_code

[source_code](source_code/Readme.en.md) contains the core runtime code:

- `MSLRobot`: field player robot program.
- `GoalKeeper`: goalkeeper robot program.
- `MSLFieldMonitor`: coach-side / field monitor program for receiving robot status, sending control commands, and assisting debugging.

These projects are mainly Windows MFC / C++ projects. The repository keeps the `.dsw` and `.dsp` project files, as well as some executable files, so the legacy framework can still be maintained or quickly inspected.

### tools

[tools](tools/Readme.en.md) contains supporting debugging tools:

- `MSLAgent`: robot communication and debugging related tool.
- `MSLFieldLine`: field-line and depth-map generation tool.
- `SCPlatform`: camera debugging tool.
- `Thresdhold`: vision threshold segmentation tool.

Some tools also appear under the robot program directories, such as `Config` or `Hardware_test_tools`, for deployment-time configuration, calibration, and hardware testing.

### manual

[manual](manual/Readme.en.md) contains onboarding and device usage documents:

- [MPU6050EXE usage guide](manual/MPU6050EXE/use.en.md)
- [GoalKeeper getting started guide](manual/GoalKeeper/use.en.md)

If this is your first time working with this codebase, start with `manual`, then move into `source_code` to read the implementation details.

## How to Use

### 1. Prepare the Environment

We recommend using this repository on Windows. The main codebase is built around an older MFC / C++ project structure. Prepare the following:

- Windows system.
- Visual C++ / Visual Studio with MFC components installed.
- A compatible development environment that can open `.dsw` and `.dsp` project files.
- Robot hardware, camera, MPU, and network communication environment if you want to deploy on real robots.

If you only want to read the code or study the framework, real robot hardware is not required.

### 2. Choose a Module

Enter the directory that matches your goal:

- Field player robot: `source_code/MSLRobot`
- Goalkeeper robot: `source_code/GoalKeeper`
- Coach-side / monitor program: `source_code/MSLFieldMonitor`
- Debugging tools: `tools`
- Onboarding documents: `manual`

### 3. Open a Project

Find the project file in the corresponding directory, for example:

- `source_code/MSLRobot/VoyVideo.dsw`
- `source_code/GoalKeeper/VoyVideo.dsw`
- `source_code/MSLFieldMonitor/MSLFieldMonitor.dsw`

Open it with a compatible Visual C++ / Visual Studio environment to read, debug, and compile the module.

The repository also keeps some `.exe` files, which can be useful for quickly launching or checking tool interfaces in an existing runtime environment.

### 4. Configure and Calibrate

Before deployment, configuration usually needs to be adjusted for the current robot hardware and field:

- Use `Thresdhold` to tune vision thresholds.
- Use `MSLFieldLine` to generate field-line, field-model, or depth-map related files.
- Use `SCPlatform` to inspect camera input.
- Follow the documents in `manual` to check the MPU, camera, communication, and goalkeeper program.

Configuration files are usually located in `Config`, `ConstantFile`, or `Hardware_test_tools` directories under each module.

### 5. Typical Runtime Flow

A typical startup flow is:

1. Start and check the camera, MPU, and other hardware.
2. Complete vision threshold and field-map configuration.
3. Run `MSLRobot` or `GoalKeeper` on the robot side.
4. Run `MSLFieldMonitor` on the host computer.
5. Confirm that network communication works and that the coach-side program can receive robot status and send control commands.
6. Continue tuning parameters based on debug windows, logs, and tool feedback.

Different years and hardware versions may require different configurations. Use the source-code comments together with the documents in `manual` when troubleshooting.

## Reading Guide

This codebase has a long development history. It contains competition-time debugging traces, Chinese comments, historical code, and utility programs. A practical reading order is:

1. Start with `manual` to understand the devices and basic runtime process.
2. Read `source_code/Readme.en.md` to understand the structure of the player robot, goalkeeper, and coach-side programs.
3. Study the system by module: communication, vision, localization, tactics, and motion control.
4. Use the `tools` directory to understand threshold tuning, field-map generation, and hardware debugging workflows.

## Community

We are open-sourcing this codebase so it can become a reusable engineering reference for the RoboCup MSL community. You are welcome to:

- Read and study the implementation of a RoboCup MSL control system.
- Open issues, improve documentation, or correct explanations.
- Improve the toolchain, build process, and runtime tutorials.
- Migrate the legacy MFC projects to a more modern project structure.
- Share reproduction experience and new debugging methods.

If this code helps your team, course project, or RoboCup research, feedback is welcome. It will help future maintainers make the documentation and tools more complete.

## Contributors

Thanks to the following team members for their contributions in code development, parameter debugging, hardware structure, and more:

- [程民华](https://github.com/chengminhua)
- [穆森源](https://github.com/M8888-sy)
- [李路畅](https://github.com/RordChang)
- [任庸祥](https://github.com/ryx-hub)
- [黄景岳](https://github.com/MiyakoMeow)
- [申鑫洋](https://github.com/sxy0828)
- [陈兆基](https://github.com/Reflectedmoon)
- [马梓博](https://github.com/MAZB-YZI)

> If there are any omissions or misattributions, please submit an issue or PR to help us correct them.
