# BigHeroX MSL 控制代码

![BigHeroX](logo.jpg)

> 语言：**中文** | [English](README.en.md)

这里开源的是 BigHeroX 战队在 RoboCup Middle Size League 使用的控制代码与配套调试工具。

这套框架经历了约四年的持续开发和赛场验证，陪伴 BigHeroX 完成了多届 RoboCup 的技术积累，并在 2024 RoboCup 中取得了相当不错的成绩。我们将代码、工具和入门文档公开出来，希望让更多队伍、同学和 RoboCup 社区成员能够参考、复现、学习和继续改进。

## 仓库内容

```text
.
├── source_code/   # 机器人与教练机运行代码
├── tools/         # 调试、标定、场地图生成等辅助工具
├── manual/        # 上手文档和设备使用说明
├── logo.jpg       # BigHeroX 标识
└── README.md      # 本说明文档
```

### source_code

[source_code](source_code/Readme.md) 保存核心运行代码：

- `MSLRobot`：场上球员机器人程序。
- `GoalKeeper`：守门员机器人程序。
- `MSLFieldMonitor`：教练机 / 场地监控程序，用于接收机器人状态、下发控制指令和辅助调试。

这些项目主要是 Windows 下的 MFC / C++ 工程，仓库中保留了 `.dsw`、`.dsp` 工程文件以及部分可执行文件，便于继续维护旧框架或快速查看运行效果。

### tools

[tools](tools/Readme.md) 保存辅助调试工具：

- `MSLAgent`：机器人通信与调试相关工具。
- `MSLFieldLine`：场地线与深度图生成工具。
- `SCPlatform`：相机相关调试工具。
- `Thresdhold`：视觉阈值分割工具。

部分工具也会在机器人程序的 `Config` 或 `Hardware_test_tools` 目录中出现，用于实际部署时的配置、标定和硬件测试。

### manual

[manual](manual/Readme.md) 保存入门和设备使用文档：

- [MPU6050EXE 使用说明](manual/MPU6050EXE/use.md)
- [GoalKeeper 上手教程](manual/GoalKeeper/use.md)

如果是第一次接触这套代码，建议先从 `manual` 开始，再进入 `source_code` 阅读具体实现。

## 如何使用

### 1. 准备环境

推荐在 Windows 环境下使用本仓库。代码主体基于较早期的 MFC / C++ 工程结构，建议准备：

- Windows 系统。
- Visual C++ / Visual Studio，并安装 MFC 相关组件。
- 能打开 `.dsw`、`.dsp` 工程文件的兼容开发环境。
- 机器人本体、相机、MPU、网络通信环境等硬件条件。

如果只是阅读代码或学习框架，不需要连接真实机器人。

### 2. 选择要运行或阅读的模块

根据目标进入对应目录：

- 球员机器人：`source_code/MSLRobot`
- 守门员机器人：`source_code/GoalKeeper`
- 教练机 / 监控端：`source_code/MSLFieldMonitor`
- 调试工具：`tools`
- 上手文档：`manual`

### 3. 打开工程

在对应目录中找到工程文件，例如：

- `source_code/MSLRobot/VoyVideo.dsw`
- `source_code/GoalKeeper/VoyVideo.dsw`
- `source_code/MSLFieldMonitor/MSLFieldMonitor.dsw`

使用兼容的 Visual C++ / Visual Studio 打开后，可以阅读、调试和编译对应模块。

仓库中也保留了部分 `.exe` 文件，适合在已有运行环境中快速启动或验证工具界面。

### 4. 配置与标定

实际部署前通常需要根据当前机器人和场地重新配置：

- 使用 `Thresdhold` 调整视觉阈值。
- 使用 `MSLFieldLine` 生成场地线、场地模型或深度图相关文件。
- 使用 `SCPlatform` 检查相机输入。
- 根据 `manual` 中的说明检查 MPU、相机、通信和守门员程序。

配置文件通常位于各模块的 `Config`、`ConstantFile` 或 `Hardware_test_tools` 目录中。

### 5. 运行流程参考

典型运行顺序如下：

1. 启动并检查相机、MPU 等硬件。
2. 完成视觉阈值和场地图配置。
3. 在机器人端运行 `MSLRobot` 或 `GoalKeeper`。
4. 在上位机运行 `MSLFieldMonitor`。
5. 确认网络通信正常，教练机能接收机器人状态并下发控制指令。
6. 根据调试窗口、日志和工具反馈继续调整参数。

不同年份、不同硬件版本的配置可能存在差异，建议结合源码注释和 `manual` 中的说明逐步排查。

## 阅读建议

这份代码经历了较长开发周期，包含大量比赛现场调试痕迹、中文注释、历史代码和工具程序。建议按下面顺序理解：

1. 先看 `manual`，了解设备和基本运行方式。
2. 再看 `source_code/Readme.md`，了解球员、守门员、教练机的代码结构。
3. 从通信、视觉、定位、策略、运动控制几个模块分块阅读。
4. 结合 `tools` 目录理解阈值、场地图和硬件调试流程。

## 社区共建

我们开源这份代码，是希望它能成为 RoboCup MSL 社区中可讨论、可学习、可复现的工程资料。欢迎基于本仓库：

- 阅读和学习 RoboCup MSL 控制系统实现。
- 提出 issue，补充文档或修正说明。
- 改进工具链、构建方式和运行教程。
- 迁移旧 MFC 工程到更现代的工程结构。
- 分享复现经验和新的调试方法。

如果这份代码对你的队伍、课程项目或 RoboCup 研究有帮助，也欢迎反馈使用经验，帮助后续维护者把文档和工具补得更完整。

## 贡献者

感谢以下队员在代码编写、调试参数、硬件结构等方面的贡献：

- [程民华](https://github.com/chengminhua)
- [穆森源](https://github.com/M8888-sy)
- [李路畅](https://github.com/RordChang)
- [任庸祥](https://github.com/ryx-hub)
- [黄景岳](https://github.com/MiyakoMeow)
- [申鑫洋](https://github.com/sxy0828)
- [陈兆基](https://github.com/Reflectedmoon)
- [马梓博](https://github.com/MAZB-YZI)

> 如有遗漏或归属有误，欢迎通过 issue 或 PR 补充修正。
