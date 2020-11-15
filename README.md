<!--
 * @Author: your name
 * @Date: 2020-11-15 16:37:17
 * @LastEditTime: 2020-11-15 17:27:37
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RoboRTS_BehaviorTree/README.md
-->
# WestWorld行为树项目

![westworld](/assets/westworld.jpeg)

## 简介

这是一份基于[DJI RoboMaster](https://github.com/RoboMaster/RoboRTS)和[ICRA2020-JLU-TARS_GO-Decision](https://github.com/Junking1/ICRA2020-JLU-TARS_GO-Decision/tree/master/%E5%86%B3%E7%AD%96%E6%A0%91/roborts_decision)框架思路的练习demo。灵感来源于《游戏人工智能编程案例精粹》中的同名项目。该书第二章《状态驱动智能体设计》详细讲解了FSM有限状态机原理以及代码实现，然而并没有涉及行为树的相关知识。

RoboRTS虽然提供了一些行为节点类的定义，但是没有给出一个清晰的实例用于阐述行为树的构建过程。JLU的框架则非常好用，思路清晰，然而基于ROS系统的调试操作似乎并不没有那么直观形象。

本demo使用了下列节点类型，运行后你可以看到各种节点的函数调用逻辑，从而更为深入地理解行为树工作过程。

+ ActionNode 
+ SelectorNode
+ SequenceNode
+ ParallelNode
+ PreconditionNode


## 文件目录说明

```
├── action                          #动作节点包
│   ├── changePositionAction.h      #更换地点动作
│   ├── heatWaterAction.h           #烧水动作
│   ├── miningAction.h              #挖矿动作
│   └── restAction.h                #休息动作
├── behavior_tree                   #决策框架示例，行为树
│   ├── behavior_node.h             #行为树节点类定义
│   ├── behavior_tree.h             #行为树运行类定义
│   └── behavior_tree.pu            
├── blackboard                  
│   └── blackboard.h                #黑板定义（决策框架的输入）
├── CMakeLists.txt
├── images
│   ├── nodes.png
│   ├── uml.png
│   └── westworld.jpeg
├── main.cpp                        #行为树搭建
├── main.pu
└── README.md
```


## 行为树结构与事件顺序

![nodes](/assets/nodes.png)

+ **frame0**: 从Home前往Mine 
+ **frame1**: 到达Mine
+ **frame2**: 开始挖矿，精力充沛 
+ **frame3**: 矿石+1
+ **frame4**: 开始挖矿，力不从心
+ **frame5**: 矿石+1
+ **frame6**: 身心俱疲，从Mine前往Home
+ **frame7**: 想起要接孩子放学，前往School
+ **frame8**: 到达School，从School前往Home
+ **frame9**: 到达Home
+ **frame10**: 烧水+休息 
+ **frame11**: 水开了，继续休息
+ **frame12**: 体力恢复 



## 节点UML图
![uml](/assets/uml.png)
