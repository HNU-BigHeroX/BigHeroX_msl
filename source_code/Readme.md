# Readme

> 语言：**中文** | [English](Readme.en.md)

运行代码

## 文件说明

- GoalKeeper:守门员代码
- MSLFieldMonitor:教练机代码
- MSLRobot:球员代码

## 更新说明

### MSLFieldMonitor

#### CToAgent() 教练机对球员操作类

##### 23.8.29 by ShitT

* CtrlCmd() 控制指令

m_sendbuf[99]为控制指令，从这里出发，看球员的UDPAgent()类，跟踪指令ATTACK 即数字1

#### CCoachUDP() 通信类

##### 23.8.26 by ShitT

* m_ParseFrame(*inBuf,inLen) 接收数据帧解析方法

传入参数inBuf 和 inLen，即为数据包以及长度，先说数据包结构，而后谈及该方法具体涉及什么内容

![inBufParse](Readme.assets\inBufParse.png)

数据包是Unsigned Char 数组指针类型（应该这么叫），然后其中每一位都是一字节

其中`?`代表没在代码中提及，估计是一个数据比较大占用了几位，或者是为了校验调整整体数值？（后续再看看）

传入数据帧以及长度后，进行校验，其中inBuf的最后一位就是校验位，而m_calsum就是简单的把整个inBuf求和，若这两者相等则表示校验通过

```cpp
unsigned char chk_sum = m_calsum(inBuf,inLen-1);
unsigned char rec_sum = inBuf[inLen-1];
if (chk_sum != rec_sum)
{
	printf("校验和错误！");
	return;
}
```

如果`m_recvAgent[m_curID-1]`即目前数据帧的发送队员不在线的话，则通过该队员对象的方法设置对应的ip和端口，并将其在线状态翻转，对其发送停止指令

接下来辨别接受数据类型的switch语句，估计是调试的时候可能有其他类型，但是目前就只有一个类型，即`MSL_INFO`队员信息

进入case，首先检测该队员是否找到球，并且更新该队员的各种信息，为该队员的接受计数器加一

```cpp
//更新队员信息
m_recvAgent[m_curID - 1].ID = m_curID;
m_recvAgent[m_curID - 1].x = m_Piece2int(&inBuf[5]);
m_recvAgent[m_curID - 1].y = m_Piece2int(&inBuf[7]);
m_recvAgent[m_curID - 1].angle = m_Piece2int(&inBuf[9]);
m_recvAgent[m_curID - 1].V = m_Piece2int(&inBuf[16]);
m_recvAgent[m_curID - 1].V_Angle = m_Piece2int(&inBuf[18]);
m_recvAgent[m_curID - 1].status = (0x7f & inBuf[11]);
m_recvAgent[m_curID - 1].foundball = m_bFoundBall;
m_recvCnt[m_curID - 1] ++;
```

而后所有操作都有注释，不细说

这里`m_recvAgent`是`uAgent`类型，可以在类视图中查到，是一个结构体，记录了各种信息

`m_ToAgent`是`CToAgent`类型，也可以在类视图中查到，是一个类，其中方法就是教练机对球员的控制

另外值得一提的是UDP传输的数据是二进制，则通过`m_Piece2int()`和`m_Piece2float()`两个函数转化为整型和浮点型

名字前面带C的是类，有m的是方法

#### CTac_Counter_KickOff

##### 2023.8.23 BY Hcm

>针对CTac_Counter_KickOff类进行了代码剖析，分析了其执行流程，完善了注释

![image-20230823213129487](Readme.assets/image-20230823213129487.png)

> 一点发现：
>
> 1.Counter指的是对方为主动时我方的应对策略

#### CTac_Counter_FreeKick

##### 2023.8.24 BY Hcm

> 这是应对敌方自由球的策略，注解已完成一半左右，此文件有较多的实际的策略变动，比较奇怪，需要更多时间看看。

![image-20230824145104033](Readme.assets/image-20230824145104033.png)

#### CTac_KickOff

##### 2023.8.25 BY Hcm

> 这个是我方发球的基本策略，做了一些基本的注释添加，同时发现其实各个策略的代码框架差不多。

#### CTac_Off

##### 2023.8.5 BY Hcm

> 这个类里面的东西基本没有，估计就是终止（暂停）比赛时会调用的类。调用完这个类以后就可以手动控制了。

#### CTac_ParkIn

##### 2023.8.5 BY Hcm

> 这个类是机器人入场时会调用的类，就是设置了一下5个机器人分别的站位，没有额外的东西。

#### MSL_Structs.h

##### 2023.8.24 BY Hcm

> 此文件定义了整个代码的公共的会调用的结构体，参数等等，有不清楚代码中定义的参数的属性的基本可在这里面找到。

![image-20230823213615063](Readme.assets/image-20230823213615063.png)

#### ITactic.h

##### 2023.8.24 BY Hcm

> ITactic这个类中包含了球员的一些策略对应的标志位，其中还定义了在策略中会调用的函数，具体实现也可查看到。

![image-20230824094611356](Readme.assets/image-20230824094611356.png)

#### ITactic.cpp

##### 2023.8.24 BY Hcm

> 这个文件中实现了策略分配中需要使用到的计算等，是一个比较具体的苦，与其他的文件耦合性相比之下比较低，若后面有需求可直接在这个文件里加计算的方法就行。本次更新完善了1/3的函数的用处说明（原来没有任何注释），另外的等在策略中看到调用了再持续更新。

![image-20230824135720437](Readme.assets/image-20230824135720437.png)

### MSLRobot

#### CAgentUDP() 

##### 23.8.29 by ShitT

* Received() 接收到数据后做的处理

这个函数基本上就是单纯的检查IP地址，做一些简单的校验罢了，而后一个循环逐字节解析数据包

* ParseByte() 逐字节解析数据

其实就是一字节一字节把数据拼成m_ParseBuf[]这个数组，然后获取一些校验信息，比如长度啥的，然后交由ParseFrame()解析

* ParseFrame() 解析数据帧

首先校验，而后获取数据中的队友信息，障碍物信息，初始位置坐标，球坐标，然后根据rec_ctrl控制指令的不同决定需不需要获取其他的信息，用switch case语句

查找rec_ctrl和re_ballx，发现这两个变量都是CAgentUDP类的变量，出现的地方只有数据帧解析，但是同时找到了net.rec_ctrl和net.re_ballx

发现net这个实例是在CGetImage()类中实例化的一个CAgentUDP类，**后续再看**

发现net.rec_ctrl的部分（`net.toS_status = net.rec_ctrl`）也出现了一个switch case语句，当指令是ATTACK时，变量xingwei被定义为1

于是查找xingwei==1这个语句，唯一一个存在这个语句的部分中，和ATTACK相关的部分有一条 

> temp_x=y.quanbx

而这里引出了y.quanbx，temp_x，temp_a

temp_x等变量在被定义的时候被赋值为net.rec_CFtargetx等，估计是接受的目标的坐标等

* temp_x最终引导到track_black_OBJ_map_X[]这个数组，跟踪找到了f2l_radius()这个函数，**后续再看**

这个函数的返回值被赋给min_dis_OBJ_dis，这些OBJ应该就是障碍物的意思，这里可能跟障碍物有关，继续看下去发现在m_find.pField这个类中的SetBall函数中被传入，m_find是一个UPRColor类的实例，pField是一个CUPFullField类的一个指针，setBall这个函数大约是画图用的...

* y.quanbx中，y是Yuzhi这个结构体，quanbux是一个参数，但是y这个名称有很多，这个应该是最可能的一个

顺着这里查貌似也只有绘图的功能？

<img src="Readme.assets/y_quanbx.png" alt="y_quanbx" style="zoom: 50%;" />

* temp_a被设置为0后没有被调用

####  UPRColor()

##### 23.8.30 by ShitT

* pipei2_5() 有被使用的场线匹配函数

先处理一下changdi结构体中的benjix和benjiy，防止越界，而后转换到l_x和l_y，这两个变量随后被用于最速下降法，算法后续细看，发现在这里居然有显示每次下降路线什么的，回头上机器人里看看

而后就是绘图部分，最后是代码接口，zuixiaox1，zuixiaox2，zuixiaojiao是被转回的坐标，估计后面就是直接引用的

这个匹配2.5稍微写了一点注释，还需要细看

##### 23.8.28 by ShitT

* pipei() 匹配当前图像和模型图

即球员的定位函数，这个函数藏得很深，首先在CGetImage()类中的AfterUpdateOverlook()函数中的很下面发现了一个m_find实例，进一步看到这个是UPRColor()类，在这个类下找到pipei()函数，但是又找到了一个dingwei()函数

dingwei()函数中，粗看是用敌方球门和己方球门（通过手动绑定？）的大小来判断机器人在哪一个半场的，则这样就理应可以结合射线扫描到的边界准确的分辨机器人在场地的哪个角（机器人在场地边角的时候容易抽风，看这些也是想解决这些问题），问题就变为有没有针对球门大小的判断

还有可能pipei函数在机器人里没有用到2或者2.5的版本导致抽风，发现只有pipei2.5有被使用到，其他的版本都没有用到，主要看pipei2_5

还还有可能AfterUpdateOverlook()里的根本不是定位，而是其他用途...

#### CRadarScan()

##### 23.8.25 by ShitT

* CRadarScan()变量初始化&~CRadarScan()清除变量

```CRadarScan()```:

初始化了一些变量，作用在后续的```Scan()```方法（即具体的扫描方法）中会讲到

初始化射线点阵表```rayx```和```rayy```，将一个圆，2pi分为144份，每根射线长度240

用for循环嵌套将每一条射线的坐标信息写入，比如第0根线的x坐标就储存在```rayx```，y坐标就储存在```rayy```，

```cpp
//初始化
//这里的x2与y2不代表直角坐标系下的横横纵
//x2指将一个圆分成几份 y2指圆的半径...
for(int x2=-72;x2<72;x2++)
for(int y2=0;y2<240;y2++)
{
	rayx[(x2+72)*240+y2]=sin(x2*2.5/57.4)*y2+320;
	rayy[(x2+72)*240+y2]=cos(x2*2.5/57.4)*y2+240;
}
```

```~CRadarScan()```:释放扫描所需变量的内存空间

* Scan()径向扫描函数

传参：

```UCHAR *inmap,int inWidth, ``` 

```int inHeight,UCHAR inVal,int inSpan```

分别是传入的图片，传入图片的宽度，传入图片的高度，传入需要扫描的值，步长

两个for循环嵌套，按照每隔```inSpan```条射线的频率，从半径20开始离心查找，如果```inmap```即图像中的某个像素点为所需的值，则计数器自增1，当计数器大于3时，则认为当前该点为要找的点，将该点坐标储存到```resultx```和```resulty```数组中，距离中心的距离也储存到```distance```数组中，跳出该循环，即此根线查找完成

如果查找到半径为239时还未跳出循环，说明这根射线不存在所需的值，向result写入-1，distance写入240

```cpp
m_pMap = inmap;
m_nWidth = inWidth;
m_nHeight = inHeight;
for (int i=0;i<144;i+=inSpan)//射线迭代
{
	count = 0;
	for (int j=20;j<240;j++)//距离迭代
	{
		index = i*240+j;
		if (m_pMap[(rayy[index])*m_nWidth+rayx[index]] ==inVal)
		{
			count ++;//计数器自增
			if (count > 3)//写入点信息
			{
				resultx[i] = rayx[index];
				resulty[i] = rayy[index];
				distance[i] = j;
				break;
			}
		}
		if (j == 239)
		{
			//扫到射线末端还没跳出循环，说明没扫描到
			resultx[i] = -1;
			resulty[i] = -1;
			distance[i] = 240;
		}
```

##### 23.8.26 by ShitT

* Line() 检索线

传参：

`inBuf`,`inIndex`,`red`,`green`,`blue`

inIndex表示要检索的线的索引，这个范围是0到144，rgb三个参数代表要将该线上什么颜色，inBuf是输入缓冲区，最后上色的图像信息将储存在这里

来自GPT：

>  首先，它检查给定索引处的距离（distance[inIndex]）是否小于0。如果是，则返回FALSE。 然后，它通过一个循环，迭代从0到distance[inIndex]的值（即雷达扫描的距离）。在每次迭代中，它根据索引（inIndex）和当前迭代次数（index）计算得出的要在输入缓冲区中修改的位置（loc）。最后，它将输入缓冲区中的指定位置（loc）的像素值设置为指定的颜色（蓝色、绿色和红色）。函数执行完毕后，返回TRUE表示成功执行。

```cpp
BOOL CRadarScan::Line(UCHAR* inBuf,int inIndex, UCHAR red, UCHAR green, UCHAR blue)
{
	if (distance[inIndex] < 0)
	{
		return FALSE;
	}
	int loc;

	for (index=0;index<distance[inIndex];index++)//逐像素点扫描射线
	{
		loc = rayy[inIndex*240+index]*m_nWidth*3 + rayx[inIndex*240+index]*3;
		inBuf[loc] = blue;
		inBuf[loc + 1] = green;
		inBuf[loc + 2] = red;
	}
	return TRUE;
}
```

其中，loc的计算方法详解如下：

loc = rayy[inIndex*240+index]\*m_nWidth\*3 + rayx[inIndex*240+index]*3;

rayy和rayx是两个数组，它们存储了雷达扫描的每个点的y坐标和x坐标。在这个计算中，通过将索引（inIndex）乘以240（假设每条雷达扫描线上有240个点），然后加上当前迭代次数（index），可以得到当前点在rayy和rayx数组中的索引。

接下来，将rayy[inIndex*240+index]乘以m_nWidth*3，可以得到该点在输入缓冲区中的行位置（假设输入缓冲区的宽度为m_nWidth，每个像素占用3个字节，rgb三个通道）。

最后，将rayx[inIndex*240+index]乘以3，可以得到该点在输入缓冲区中的列位置。

将行和列位置相加，就得到了该点在输入缓冲区中的像素位置（loc）。

* GetPoint() 获取点坐标

传参：`inIndex,inDistance,*inPnt`

分别代表射线扫描下线的序号，距中心距离以及输入点的结构体指针

实际上是在做类似极坐标到直角坐标系的变换过程

* Sector() 扇区区分连通域

该方法迭代查找每条射线，寻找统一颜色的连通域

首先，函数初始化了一些变量，包括linesum（用于累计有效射线的索引总和）、num（用于计数有效射线数量）和m_nNumofSector（用于记录扇区数量）等。

然后，设置了要扫描的初始范围，即mintoscan和maxtoscan的初始值为0和143。

接下来，代码开始扫描零号线附近的区域。如果零号线（索引0）的距离不等于240（表示有效射线），则进入循环。

在循环中，首先从零号线右侧开始扫描，遍历144个索引（0到143）。如果该索引对应的距离小于240，则累计linesum和num，并将该索引添加到anticlockwise数组中。

```cpp
		//先扫零号线右侧
		for (int i=0;i<144;i++)
		{
			if (distance[i] <240)
			{
				linesum += i;
				num ++;
				anticlockwise[m_nNumofSector] =i;
			}
			else
			{
				break;
			}
		}
```

然后，继续扫描零号线左侧，从143开始递减到anticlockwise[m_nNumofSector]。如果该索引对应的距离小于240，则累计linesum和num，并将该索引更新为clockwise[m_nNumofSector]。（代码类似就不放上来了）

如果num大于0，则计算扇区的midline（中线）和sumoflines（有效射线数量），并将m_nNumofSector递增。

最后，更新剩余的扫描区域，将mintoscan设置为anticlockwise[0]，将maxtoscan设置为clockwise[0]。

```cpp
		if (num >0 )
		{
			midline[m_nNumofSector] = anticlockwise[m_nNumofSector] - (144-clockwise[m_nNumofSector]);
			if (midline[m_nNumofSector] > 0)
			{
				midline[m_nNumofSector] = midline[m_nNumofSector]/2;
			}
			else
			{
				midline[m_nNumofSector] = 143 + (midline[m_nNumofSector]/2);
			}
			sumoflines[m_nNumofSector] = num;
			m_nNumofSector ++;

			//更新剩下的扫描区域
			mintoscan = anticlockwise[0];
			maxtoscan = clockwise[0];
		}
```

![循环初始](Readme.assets\Line0.jpg)

接下来，代码进入一个while循环，用于扫描剩余的区域。循环条件是maxtoscan大于mintoscan。

在循环中，初始化linesum和num，并设置一个标记flag为false。然后从mintoscan开始遍历到maxtoscan。

如果当前索引对应的距离小于240，则判断flag是否为false。如果是，则将该索引设置为anticlockwise[m_nNumofSector]，并将flag设置为true。

然后，累计linesum和num。

如果当前索引对应的距离等于240，并且flag为true，则表示离开了当前连通域。将当前索引减1设置为clockwise[m_nNumofSector]，并计算扇区的midline和sumoflines。

最后，更新mintoscan为当前索引，然后跳出当前循环，进行下一个while循环。

![第二次循环](Readme.assets\Line1.jpg)

在while循环结束后，代码进入一个冒泡排序的过程，对扇区进行排序。排序的依据是sumoflines的值，即有效射线数量。

最后，函数返回TRUE表示成功执行。



总体上，这段代码的作用是将雷达扫描的数据分割成不同的扇区，并对这些扇区进行排序。每个扇区由相邻的有效射线组成，其中包括中线的索引、顺时针和逆时针边界的索引以及有效射线的数量。

* CalAngle(int inx, int iny)计算角度 

针对整个640x480的画面，原始坐标系下，画面左上角为坐标原点，该方法首先将坐标原点平移至中心，```inx```和```iny```的坐标从而变换为```dx```和``` dy```从而转换为中心坐标（该坐标系，向右为x正方向，向下为y正方向）

而后计算角度，总的目的就是获取输入的```(x,y)```坐标所对应的角度信息，即以画面中心为原点作直角坐标系，以y轴正半轴为角度0点，逆时针为正方向的角度信息，例如，点(1,-1)处在135°这根线上



#### StdAfx.h

##### 2023.8.23 BY Hcm

> 发现调整场地大小比例以及控制设备最高运动速度的参数

![image-20230823214538829](Readme.assets/image-20230823214538829.png)

> 注:有些文件若不显示在这个左边的文件列表里，可找到Header Files/Source Files右键添加进去。

#### Config/MSLFieldLine.exe(生成景深图的工具)

##### 2023.8.27 BY Hcm

![image-20230827213745914](Readme.assets/image-20230827213745914.png)

如图，该软件打开长这样。根据比赛场地的具体大小填入尺寸，其中禁区尺寸指的是大禁区的地方，球门区尺寸指的是小禁区的尺寸，这边没有填球门大小的地方，因为**球门是固定大小的！**不论场地的大小比例是怎么样的。

点击`生成图片`,可以在软件的统计目录下生成一个`field.bmp`文件，同时会在软件的左上角显示出来，让操作者看看填数据时有没有犯一些低级错误，比如多打一个0啥的，这样在图片上看的会非常明显。

点击`生成深度图`会在软件同等目录下生成一个`fieldmap`，该图片可以通过点击`显示`来展示在软件的右边，如下图所示。(生成时间会比较长，需要耐心等待！)

![image-20230827214354750](Readme.assets/image-20230827214354750.png)

#### Config/TxtWR.exe(线性插入数据工具)

##### 2023.8.27 BY Hcm

![image-20230827214854224](Readme.assets/image-20230827214854224.png)

工具打开后界面如图所示，该工具只能读取文档，不能写入，随便打开一个文件，如下图，其会在左边那一竖的基础上线性生成数据，如中间的表格所示。

![image-20230827215100483](Readme.assets/image-20230827215100483.png)

### MFC框架大解析

#### 2023.9.4 BY xbx & Hcm

包括教练机、球员、守门员的三个程序都是在MFC框架下写的，这是一个Windows下的可以提供窗口交互功能的框架，类似于先在的QT，但是比较古老，是20年前的东西了。

- StdAfx.h

![image-20230904214312283](Readme.assets/image-20230904214312283.png)

这是MFC的预编译头文件，可以加快编译速度，不能被任何.h头文件所包含，但在所有的.cpp文件中都要包含。这个文件里有另外加的中文注释，可以看看。

- MSLFieldMonitorView.h

这个文件是在初始化一个MFC界面时根据项目的名称自动生成的，还有MSLFieldMonitorDoc.h、MSLFieldMonitor.h（包括相应的.cpp文件），其中整个大程序的入口文件是MSLFieldMonitor.h

![image-20230904215416167](Readme.assets/image-20230904215416167.png)

- MSLFieldMonitorView.cpp

这个文件比较关键，包含了所有按钮的回调以及一些情况的设定。

~~~cpp
//[3]传递指针
m_wndField.m_cGreenField.pDataSummary = (m_cDataCenter.m_dataSummary);
m_wndField.m_cGreenField.pCoachUDP = &m_cCoachUDP;
m_wndField.m_cGreenField.pDataCenter = &m_cDataCenter;
m_cDataCenter.SetCoachUDP(&m_cCoachUDP);
m_cDataCenter.pInfoList = &(m_wndReferee.m_cDataInfoList);
m_wndMatchPanel.pDataCenter = &m_cDataCenter;
m_wndMatchPanel.pRefClient = &m_cRefClient;
m_wndMatchPanel.pFiled = &(m_wndField.m_cGreenField);
m_cRefClient.pDataCenter = &m_cDataCenter;
m_wndStatus.m_disRobotStatus.pField = &(m_wndField.m_cGreenField);
m_wndStatus.m_disRobotStatus.pCoachUDP = &m_cCoachUDP;
m_wndStatus.m_disRoleStatus.pField = &(m_wndField.m_cGreenField);
m_wndStatus.m_disRoleStatus.pTac = m_cDataCenter.m_stTactics;
m_wndStatus.pTac_TC = &(m_cDataCenter.m_tac_TechChallenge);
m_wndMatchPanel.pKey = &(m_cDataCenter.m_dataSummary->key);
m_wndReferee.pKey = &(m_cDataCenter.m_dataSummary->key);
m_wndReferee.pRefClient = &m_cRefClient;
m_cRefClient.m_pShowList = &(m_wndReferee.m_cRefInfoList);
m_wndField.m_cGreenField.pObstFusion = &(m_cDataCenter.m_ObstF_Zwj);
m_wndStatus.pObstFusion = &(m_cDataCenter.m_ObstF_Zwj);
m_wndReferee.m_dlgAgentIPList.pCoachUDP = &(m_cCoachUDP);
m_cCoachUDP.pInfoList = &m_wndMatchPanel.m_cTacInfoList;
~~~

~~~cpp
void CMSLFieldMonitorView::checkKeyPressed()
{
	//shift 按下
	if (m_cDataCenter.m_dataSummary->key.bShiftPressed == true)
	{
		m_cDataCenter.m_dataSummary->key.bShiftPressed = false;
		m_wndMatchPanel.OnRefStop();
	}
	
	//ctrl 按下
	if (m_cDataCenter.m_dataSummary->key.bCtrlPressed == true)
	{
		m_cDataCenter.m_dataSummary->key.bCtrlPressed = false;
		m_wndField.m_cGreenField.m_bSetPlayerIn = true;
	}
	
	//A 按下
	if (m_cDataCenter.m_dataSummary->key.bKeyAPressed == true)
	{
		m_cDataCenter.m_dataSummary->key.bKeyAPressed = false;
		//AfxMessageBox(L"A Pressed");
		//[1]获取所选队员的ID号
		int nSelID = m_wndField.m_cGreenField.m_nSelAgentID;
		
		if (nSelID > 0 || nSelID <= AG_NUM)
		{
			//选择队员ID存在，继续进行后面操作
			//[2]切换到手动控制模式
			m_cDataCenter.ChangeMatchModeTo(MATCH_OFF);
			
			//[3]将所选的队员切换到进攻
			m_cCoachUDP.m_ToAgent[nSelID - 1].CtrlCmd(CTRL_ATTACK, m_cDataCenter.m_dataSummary->cGlobalBall.x, m_cDataCenter.m_dataSummary->cGlobalBall.y ,0,0,true);
		}
	}
	
	//S 按下
	if (m_cDataCenter.m_dataSummary->key.bKeySPressed == true)
	{
		m_cDataCenter.m_dataSummary->key.bKeySPressed = false;
		m_wndMatchPanel.OnRefStop();
	}
}
~~~

在这个文件里也做了比较多的注释，可查看！
