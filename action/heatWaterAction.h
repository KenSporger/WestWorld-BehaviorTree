#ifndef HEATWATERACTION_H
#define HEATWATERACTION_H

#include<behavior_node.h>
#include<blackboard.h>
#include<chrono>

class HeatWaterAction : public ActionNode
{
public:
    HeatWaterAction(const BlackBoard::Ptr &blackboard_ptr):
        ActionNode("HeatWaterAction", blackboard_ptr){}

    virtual ~HeatWaterAction(){}

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
