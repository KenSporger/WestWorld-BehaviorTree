#ifndef MININGACTION_H
#define MININGACTION_H

#include<behavior_node.h>
#include<blackboard.h>
#include<chrono>

class MinningAction : public ActionNode
{
public:
    MinningAction(const BlackBoard::Ptr &blackboard_ptr):
        ActionNode("MinningAction", blackboard_ptr){}

    virtual ~MinningAction(){}

private:
    virtual void OnInitialize()
    {
        start_time = std::chrono::steady_clock::now();
    }

    virtual BehaviorState Update()
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            (std::chrono::steady_clock::now()-start_time)
            ) >= std::chrono::milliseconds(500))
        {
            blackboard_ptr_->adjustEnergy(-40);
            return BehaviorState::SUCCESS;
        }
        return BehaviorState::RUNNING;
    }

    virtual void OnTerminate(BehaviorState state)
    {
        
    }

    std::chrono::steady_clock::time_point start_time;
};


#endif
