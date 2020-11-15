#include<behavior_node.h>
#include<behavior_tree.h>
#include<blackboard.h>
#include<changePositionAction.h>
#include<miningAction.h>
#include<restAction.h>
#include<heatWaterAction.h>

using namespace std;

int main()
{
    auto blackboard_ptr_ = std::make_shared<BlackBoard>();
    
    auto rest_action = std::make_shared<RestAction>(blackboard_ptr_);
    auto minning_action = std::make_shared<MinningAction>(blackboard_ptr_);
    auto heat_water_action = std::make_shared<HeatWaterAction>(blackboard_ptr_);
    auto go_school_action = std::make_shared<ChangePositionAction>("go_school_action",blackboard_ptr_, Position::SCHOOL);
    auto go_home_from_school_action = std::make_shared<ChangePositionAction>("go_home_from_school_action",blackboard_ptr_, Position::HOME);
    auto go_home_from_mine_action = std::make_shared<ChangePositionAction>("go_home_from_mine_action",blackboard_ptr_, Position::HOME);
    auto go_mine_from_home_action = std::make_shared<ChangePositionAction>("go_mine_from_home_action",blackboard_ptr_, Position::MINE);

    auto position_selector = std::make_shared<SelectorNode>("position_selector", blackboard_ptr_);
    auto is_schoolover_condition = std::make_shared<PreconditionNode>("is_schoolover_condition", AbortType::BOTH,
        blackboard_ptr_, [&](){return blackboard_ptr_->isSchoolOver();});
    auto is_mine_condition = std::make_shared<PreconditionNode>("is_mine_condition", AbortType::SELF,
        blackboard_ptr_, [&](){return blackboard_ptr_->isMine();});
    auto home_selector = std::make_shared<SelectorNode>("home_selector", blackboard_ptr_);
    auto home_parallel = std::make_shared<ParallelNode>("home_parallel", blackboard_ptr_, 2);
    auto school_sequence = std::make_shared<SequenceNode>("school_sequence", blackboard_ptr_);
    auto mine_selector = std::make_shared<SelectorNode>("mine_selector", blackboard_ptr_);
    auto is_rich_or_energy_low_condition = std::make_shared<PreconditionNode>("is_rich_or_energy_low_condition", AbortType::SELF, 
        blackboard_ptr_, [&](){return blackboard_ptr_->isEnergyLow();});
    auto is_energy_low_condition = std::make_shared<PreconditionNode>("is_energy_low_condition", AbortType::SELF, 
        blackboard_ptr_, [&](){return blackboard_ptr_->isEnergyLow();});

    position_selector->AddChildren(is_schoolover_condition);
    position_selector->AddChildren(is_mine_condition);
    position_selector->AddChildren(home_selector);
    is_schoolover_condition->SetChild(school_sequence);
    is_mine_condition->SetChild(mine_selector);
    school_sequence->AddChildren(go_school_action);
    school_sequence->AddChildren(go_home_from_school_action);
    mine_selector->AddChildren(is_rich_or_energy_low_condition);
    mine_selector->AddChildren(minning_action);
    home_selector->AddChildren(is_energy_low_condition);
    home_selector->AddChildren(go_mine_from_home_action);
    is_rich_or_energy_low_condition->SetChild(go_home_from_mine_action);
    is_energy_low_condition->SetChild(home_parallel);
    home_parallel->AddChildren(heat_water_action);
    home_parallel->AddChildren(rest_action);


    BehaviorTree root_(position_selector, 500, blackboard_ptr_);
    root_.Run();
    
    
}