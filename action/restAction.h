#ifndef RESTACTION_H
#define RESTACTION_H

#include<behavior_node.h>
#include<blackboard.h>
#include<chrono>

class RestAction : public ActionNode
{
public:
    RestAction(const BlackBoard::Ptr &blackboard_ptr):
        ActionNode("RestAction", blackboard_ptr){}

    virtual ~RestAction(){}

private:
    virtual void OnInitialize()
    {
        start_time = std::chrono::steady_clock::now();
    }

    virtual BehaviorState Update()
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            (std::chrono::steady_clock::now()-start_time)
            ) >= std::chrono::milliseconds(1000))
        {
            blackboard_ptr_->adjustEnergy(100);
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
