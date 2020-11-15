#ifndef CHANGEPOSITION_H
#define CHANGEPOSITION_H

#include<behavior_node.h>
#include<blackboard.h>
#include<chrono>

class ChangePositionAction : public ActionNode
{
public:
    ChangePositionAction(std::string name, const BlackBoard::Ptr &blackboard_ptr, const Position &destination):
        ActionNode(name, blackboard_ptr), 
        destination_(destination){}

    virtual ~ChangePositionAction(){}

private:
    virtual void OnInitialize()
    {
        start_time = std::chrono::steady_clock::now();
        blackboard_ptr_->setDestination(destination_);
    }

    virtual BehaviorState Update()
    {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(
            (std::chrono::steady_clock::now()-start_time)
            ) >= std::chrono::milliseconds(500))
        {
            blackboard_ptr_->setPosition(destination_);
            return BehaviorState::SUCCESS;
        }
        return BehaviorState::RUNNING;
    }

    virtual void OnTerminate(BehaviorState state)
    {
        
    }

    Position destination_;
    std::chrono::steady_clock::time_point start_time;
};


#endif
