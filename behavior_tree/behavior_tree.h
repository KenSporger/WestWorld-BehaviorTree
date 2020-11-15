/*
 * @Author: your name
 * @Date: 2020-11-15 09:38:34
 * @LastEditTime: 2020-11-15 17:19:34
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RoboRTS_BehaviorTree/behavior_tree/behavior_tree.h
 */
#ifndef BEHAVIOR_TREE_H
#define BEHAVIOR_TREE_H

#include<behavior_node.h>
#include<chrono>
#include<thread>

class BehaviorTree
{
public:
    BehaviorTree(const BehaviorNode::Ptr &root_node, int cycle_duration, const BlackBoard::Ptr &blackboard):
        root_node_(root_node),
        blackboard_(blackboard),
        cycle_duration_(cycle_duration){}

    void Run()
    {
        
        while(blackboard_->frame <= 12)
        {
            std::cout << "---------------frame "+std::to_string(blackboard_->frame)
                        +"---------------" << std::endl;
            std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();
            root_node_->Run();
            std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
            std::chrono::milliseconds execution_duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            std::chrono::milliseconds sleep_time = cycle_duration_ - execution_duration;
            blackboard_->Info();
            if (sleep_time > std::chrono::milliseconds(0)) {
                std::cout << "tree sleeping!" << std::endl << std::endl;
                std::this_thread::sleep_for(sleep_time);
            }
            else
            {
                std::cout << "tree timeout!" << std::endl << std::endl;
            }
            blackboard_->frame++;
        }
    }
private:
    BehaviorNode::Ptr root_node_;
    BlackBoard::Ptr blackboard_;
    std::chrono::milliseconds cycle_duration_;
};

#endif
