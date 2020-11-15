#ifndef BEHAVIOR_NODE_H
#define BEHAVIOR_NODE_H

#include<iostream>
#include<memory>
#include<vector>
#include<string>
#include<functional>
#include<blackboard.h>
#include<algorithm>

// 为了enum转string
#define FOREACH_BEHAVIORSTATE(FUNC) \
        FUNC(IDLE) \
        FUNC(RUNNING) \
        FUNC(SUCCESS) \
        FUNC(FAILURE) \

#define FOREACH_BEHAVIORTYPE(FUNC) \
        FUNC(PARALLEL) \
        FUNC(SELECTOR) \
        FUNC(SEQUENCE) \
        FUNC(ACTION) \
        FUNC(PRECONDITION) \

#define TO_ENUM(x) x,
#define TO_STRING(x) #x,

// enum是类型不安全的，class申明了作用域
enum class BehaviorState {
  FOREACH_BEHAVIORSTATE(TO_ENUM)
};

enum class BehaviorType {
  FOREACH_BEHAVIORTYPE(TO_ENUM)
};

/**
 * @brif Abort behavior_type of behavior tree precondition node
 * @details For more information refer to https://docs.unrealengine.com/en-us/Engine/AI/BehaviorTrees/NodeReference/Decorators
 * https://zhuanlan.zhihu.com/p/143298443
 * 用于表示precondition节点的终止类型
 * NONE : 前提条件失效，直接返回true（用于调试）
 * SELF : 当前提条件为true时，允许执行子节点；当前提条件为false时，禁止子节点执行（若子节点
 *        处于RUNNING状态，则立即终止）
 * LOW_PRIORITY ： 当前提条件为true时，立即终止右侧低优先级节点，转而执行子节点；当前提条件
 *        为flase时，禁止子节点执行（但若子节点处于RUNNING状态，会允许其继续执行）
 * BOTH ： SELF+LOW_PRIORITY；当前提条件为true时，立即终止右侧低优先级节点，转而执行子节点；
 *        当前提条件为false，禁止子节点执行（若子节点处于RUNNING状态，则立即终止）
 */
enum class AbortType {
  NONE,           ///<Do not abort anything
  SELF,           ///<Abort self, and any sub-trees running under this node
  LOW_PRIORITY,   ///<Abort any nodes to the right of this node
  BOTH            ///<Abort self, any sub-trees running under me, and any nodes to the right of this node
};


/**
 * enable_shared_from_this是一个模板类，用于在类成员函数里创建一个指向当前类对象的share_ptr
 * 例如DecoratorNode中的setChild方法就需要为子节点设置父节点（即对象本身）
 * 更多关于enable_shared_from_this参考： https://blog.csdn.net/caoshangpa/article/details/79392878
 */
class BehaviorNode : public std::enable_shared_from_this<BehaviorNode>
{
  public:
    typedef std::shared_ptr<BehaviorNode> Ptr;
    /**
     * @brief 节点构造器
     * @param name 节点名称标识
     * @param behavior_type 节点行为类型
     */   
    BehaviorNode(std::string name, BehaviorType behavior_type, const BlackBoard::Ptr &blackboard_ptr) :
      name_(name),
      behavior_state_(BehaviorState::IDLE),
      behavior_type_(behavior_type),
      parent_node_ptr_(nullptr),
      blackboard_ptr_(blackboard_ptr){}

    virtual ~BehaviorNode(){}

    BehaviorState Run()
    {
      NodeInfo("MethodInto:Run");
      // 连续性假设：行为通常会持续多次遍历周期，因此节点处于RUNNING状态是常见的
      // 当重新遍历行为树时，如果遇到了RUNNING的节点，就不再需要初始化操作了
      if (behavior_state_ != BehaviorState::RUNNING)
      {
        NodeInfo("MethodInto:OnInitialize");
        OnInitialize();
        NodeInfo("MethodOut:OnInitialize");
      }

      // Update中又会调用子节点的Run,因此状态的返回、节点的终止都是由内而外
      NodeInfo("MethodInto:Update");
      behavior_state_ = Update();
      NodeInfo("MethodOut:Update");

      if (behavior_state_ != BehaviorState::RUNNING)
      {
        NodeInfo("MethodInto:OnTerminate");
        OnTerminate(behavior_state_);
        NodeInfo("MethodOut:OnTerminate");
      }

      NodeInfo("MethodOut:Run");
      return behavior_state_;
    }
  
    void SetParent(BehaviorNode::Ptr parent_node_ptr)
    {
      parent_node_ptr_ = parent_node_ptr;
    }
    
    BehaviorNode::Ptr GetParent()
    {
      return parent_node_ptr_;
    }
    // 派生节点一定会有父节点，但是不是所有的节点都是需要有子节点的，
    // 因此基类中定义了parent_node_ptr_，而没有定义child_node_ptr_
    virtual BehaviorNode::Ptr GetChild(){
      return nullptr;
    }

    BehaviorType GetBehaviorType()
    {
      return behavior_type_;
    }

    BehaviorState GetBehaviorState(){
      return behavior_state_;
    }

    // 用于在行为运行时提前终止节点（异常终止）
    // Reset里调用了当前节点的OnTerminate，
    // 部分类型的节点OnTerminate中又会调用子节点的Reset方法
    virtual void Reset(){
      NodeInfo("MethodInto:Reset");
      if (behavior_state_ == BehaviorState::RUNNING){
        behavior_state_ = BehaviorState::IDLE;
        NodeInfo("MethodInto:OnTerminate");
        OnTerminate(BehaviorState::IDLE);
        NodeInfo("MethodOut:OnTerminate");
      }
      NodeInfo("MethodOut:Reset");
    }
  protected:
    void NodeInfo(std::string msg)
    {
      static std::string behavior_types[] = {FOREACH_BEHAVIORTYPE(TO_STRING)};
      static std::string behavior_states[] = {FOREACH_BEHAVIORSTATE(TO_STRING)};

      std::cout << "Node:"+name_+" behavior_type:"+behavior_types[(int)behavior_type_]+
            " State:"+behavior_states[(int)behavior_state_]+" " +msg <<std::endl; 
    }

    virtual void OnInitialize() = 0;
    virtual BehaviorState Update() = 0;
    // OnTerminate在节点状态为SUCCESS、FAILURE、IDLE下正常调用
    // 当然也会在提前终止的情况下调用（一般在Update函数中），此时一般会设置state为IDLE，并且
    // 依次调用子节点的OnTerminate。
    // 但在正常调用的情况下（一般是在Run函数），节点应以其Update结果状态终止。
    virtual void OnTerminate(BehaviorState state) = 0;

    std::string name_;
    BehaviorState behavior_state_;
    BehaviorType behavior_type_;
    BehaviorNode::Ptr parent_node_ptr_;
    BlackBoard::Ptr blackboard_ptr_;

};

class ActionNode : public BehaviorNode
{
  public:
    /**
     * @brief 节点构造器
     * @param name 节点名称标识
     * @param behavior_type 节点行为类型
     */   
    ActionNode(std::string name, const BlackBoard::Ptr &blackboard_ptr) : 
      BehaviorNode::BehaviorNode(name, BehaviorType::ACTION, blackboard_ptr){}
  
    virtual ~ActionNode(){}

  protected:
    virtual void OnInitialize() = 0;
    virtual BehaviorState Update() = 0;
    virtual void OnTerminate(BehaviorState state) = 0;    
  
};


class DecoratorNode : public BehaviorNode
{
  public:
    DecoratorNode(std::string name, BehaviorType behavior_type, const BlackBoard::Ptr &blackboard_ptr) :
      BehaviorNode::BehaviorNode(name, behavior_type, blackboard_ptr){}

    void SetChild(const BehaviorNode::Ptr &child_node_ptr)
    {
      child_node_ptr_ = child_node_ptr;
      child_node_ptr->SetParent(shared_from_this());
    }

    virtual ~DecoratorNode(){}

  protected:
    virtual void OnInitialize() = 0;
    virtual BehaviorState Update() = 0;
    virtual void OnTerminate(BehaviorState state) = 0;   
    BehaviorNode::Ptr child_node_ptr_;
};


class PreconditionNode : public DecoratorNode
{
  public:
    PreconditionNode(std::string name, AbortType abort_type, const BlackBoard::Ptr &blackboard_ptr,
      std::function<bool()> precondition_function=std::function<bool()>()):
      DecoratorNode::DecoratorNode(name, BehaviorType::PRECONDITION, blackboard_ptr),
      precondition_function_(precondition_function),
      abort_type_(abort_type){}

    virtual ~PreconditionNode(){}

    AbortType GetAbortType()
    {
      return abort_type_;
    }

  protected:
    virtual bool Precondition()
    {
      if (precondition_function_)
      {
        return precondition_function_();
      }
      return false;
    }

    virtual void OnInitialize(){}

    virtual BehaviorState Update()
    {
      // 运行准入条件节点,其结果决定是否运行子行为节点
      if (Reevaluation())
      {
        BehaviorState state = child_node_ptr_->Run();
        return state;
      }
      // 不满足准入条件则返回失败状态
      return BehaviorState::FAILURE;
    }

    virtual void OnTerminate(BehaviorState state) {
      switch (state){
        case BehaviorState::IDLE:
          child_node_ptr_->Reset();
          break;
        case BehaviorState::SUCCESS:
          break;
        case BehaviorState::FAILURE:
          child_node_ptr_->Reset();
          break;
        default:
          return;
      }
  }

    virtual bool Reevaluation();

    // std::function实现了函数指针的功能，这里表示函数返回类型是bool，且函数无参数、
    // 我们可以传入lambda函数，利用捕获列表捕获外部变量，这样就使得参数列表为空
    // 注意，类普通土成员函数不能直接传入，因为它默认带有this参数
    std::function<bool()> precondition_function_;
    AbortType abort_type_;
};



class CompositeNode : public BehaviorNode
{
  public:
    /**
     * @brief 节点构造器
     * @param name 节点名称标识
     * @param behavior_type 节点行为类型
     */   
    CompositeNode(std::string name, BehaviorType behavior_type, const BlackBoard::Ptr &blackboard_ptr) : 
      BehaviorNode::BehaviorNode(name, behavior_type, blackboard_ptr),
      children_node_index_(0){}  

    virtual ~CompositeNode(){}

    virtual void AddChildren(const BehaviorNode::Ptr &children_node_ptr)
    {
      children_node_ptr_.push_back(children_node_ptr);
      children_node_ptr->SetParent(shared_from_this());
    }

    std::vector<BehaviorNode::Ptr>& GetChildren()
    {
      return children_node_ptr_;
    }

    unsigned int GetChildrenIndex()
    {
      return children_node_index_;
    }

  protected:
    virtual void OnInitialize() = 0;
    virtual BehaviorState Update() = 0;
    virtual void OnTerminate(BehaviorState state) = 0;    
    std::vector<BehaviorNode::Ptr> children_node_ptr_;
    // 记录当前正在或待运行运行的节点编号
    unsigned int children_node_index_;

};

class SelectorNode : public CompositeNode
{
  public:
    SelectorNode(std::string name, const BlackBoard::Ptr &blackboard_ptr) :
      CompositeNode::CompositeNode(name, BehaviorType::SELECTOR, blackboard_ptr){}

    virtual ~SelectorNode(){}

    virtual void AddChildren(const BehaviorNode::Ptr &children_node_ptr)
    {
      CompositeNode::AddChildren(children_node_ptr);
      children_node_reevaluation_.push_back(
        children_node_ptr->GetBehaviorType()==BehaviorType::PRECONDITION 
        && (std::dynamic_pointer_cast<PreconditionNode>(children_node_ptr)->GetAbortType()==AbortType::LOW_PRIORITY
        || std::dynamic_pointer_cast<PreconditionNode>(children_node_ptr)->GetAbortType()==AbortType::BOTH)
      );
    }

    void SetChildrenIndex(unsigned int children_node_index)
    {
      children_node_index_ = children_node_index;
    }

  protected:
    virtual void OnInitialize()
    {
      children_node_index_ = 0;
    }

    virtual BehaviorState Update()
    {
      if (children_node_ptr_.size() == 0) {
        return BehaviorState::SUCCESS;
      }

      // 查看是否需要终止低优先级节点（即当前运行节点children_node_index_）
      for (unsigned int index=0; index<children_node_index_; index++)
      {
        if (children_node_reevaluation_.at(index))
        {
          BehaviorState state = children_node_ptr_.at(index)->Run();
          // Run时如果低优先级被成功终止了（即高优先级节点的准入条件得到满足），
          //则children_node_index_会被设置为index
          if (index == children_node_index_)
          {
            // 同下面的while
            if (state != BehaviorState::FAILURE)
            {
              return state;
            }
            // 如果高优先级节点没有运行成功，则while循环从下一个节点开始遍历
            ++children_node_index_;
            break;
          }
        }
      }

      while (true)
      {
        // 运行子节点
        BehaviorState state = children_node_ptr_.at(children_node_index_)->Run();

        if (state != BehaviorState::FAILURE)
        {
          // 每次行为树遍历只运行一个节点
          // RUNNING： 下次继续从该节点开始访问
          // SUCCESS： 在Run()中调用Terminate
          return state;
        }

        // 当所有子节点运行失败时，终止自己（返回FAILURE状态）
        if (++children_node_index_ == children_node_ptr_.size())
        {
          children_node_index_ = 0;
          return BehaviorState::FAILURE;
        }
      }
    }
    // 复位当前运行或待运行的子节点
    virtual void OnTerminate(BehaviorState state){
      switch (state){
        case BehaviorState::IDLE:
          children_node_ptr_.at(children_node_index_)->Reset();
          break;
        case BehaviorState::SUCCESS:
          break;
        case BehaviorState::FAILURE:
          break;
        default:
          return;
      }
  }
    // 标记是否需要终止低优先级节点，LOW_PRIORITY和BOTH为true
    std::vector<bool> children_node_reevaluation_;

};


class SequenceNode : public CompositeNode
{
  public:
    SequenceNode(std::string name, const BlackBoard::Ptr &blackboard_ptr) :
      CompositeNode::CompositeNode(name, BehaviorType::SEQUENCE, blackboard_ptr){}

    virtual ~SequenceNode(){}

  protected:
    virtual void OnInitialize()
    {
      children_node_index_ = 0;
    }

    // 类似于SelectorNode
    virtual BehaviorState Update()
    {
      if (children_node_ptr_.size() == 0) {
        return BehaviorState::SUCCESS;
      }

      while (true)
      {
        // 运行子节点
        BehaviorState state = children_node_ptr_.at(children_node_index_)->Run();

        if (state != BehaviorState::SUCCESS)
        {
          // 要么运行失败导致序列节点也失败，要么处于RUNNING
          return state;
        }

        // 当所有子节点都运行成功时，终止自己（返回SUCCESS状态）
        if (++children_node_index_ == children_node_ptr_.size())
        {
          children_node_index_ = 0;
          return BehaviorState::SUCCESS;
        }
      }
    }    

    virtual void OnTerminate(BehaviorState state){
      switch (state){
        case BehaviorState::IDLE:
          children_node_ptr_.at(children_node_index_)->Reset();
          break;
        case BehaviorState::SUCCESS:
          break;
        case BehaviorState::FAILURE:
          break;
        default:
          return;
      }
  }
};


class ParallelNode : public CompositeNode
{
  public:
    ParallelNode(std::string name, const BlackBoard::Ptr &blackboard_ptr, unsigned int threshold) :
      CompositeNode::CompositeNode(name, BehaviorType::PARALLEL, blackboard_ptr),
      threshold_(threshold), success_count_(0), failure_count_(0){}

    virtual ~ParallelNode(){}

  protected:
    virtual void OnInitialize()
    {
      success_count_ = 0;
      failure_count_ = 0;
      children_node_done_.clear();
      // 节点默认标记为false，表未曾访问
      children_node_done_.resize(children_node_ptr_.size(), false);
    }

    virtual BehaviorState Update()
    {
      if (children_node_ptr_.size() == 0) {
        return BehaviorState::SUCCESS;
      }

      for (unsigned int index=0; index<children_node_ptr_.size(); index++)
      {
        // 仅运行未曾执行完毕的节点
        if (children_node_done_.at(index)==false)
        {
          // 运行子节点
          BehaviorState state = children_node_ptr_.at(index)->Run();

          if (state == BehaviorState::SUCCESS)
          {
            //标记已运行
            children_node_done_.at(index) = true;
            // 当成功次数超过设定值时，并行节点终止自己（返回SUCCESS状态）
            if (++success_count_ >= threshold_)
            {
              return BehaviorState::SUCCESS;
            }
          }
          else if (state == BehaviorState::FAILURE)
          {
            //标记已运行 
            children_node_done_.at(index) = true;
            // 运行失败的节点数量超过一定数量（也就是说即使剩下来的节点全部执行完都达不到SUCCESS要求）
            if (++failure_count_ >= children_node_ptr_.size()-threshold_)
            {
              return BehaviorState::FAILURE;
            }
          }
        }
      }

      // 有多个节点还处于RUNNING状态，等待下一次行为树遍历继续执行
        return BehaviorState::RUNNING;
    }

    virtual void OnTerminate(BehaviorState state){
      switch (state){
        case BehaviorState::IDLE:
          break;
        case BehaviorState::SUCCESS:
          break;
        case BehaviorState::FAILURE:
          break;
        default:
          return;
      }
      for (unsigned int index = 0; index<children_node_ptr_.size(); index++) {
        children_node_ptr_.at(index)->Reset();
      }
  }

    // 终止并行节点所需的SUCCESS节点数
    unsigned int threshold_;
    unsigned int success_count_;
    unsigned int failure_count_;
    // 表示是否访问过并执行完毕（即SUCCESS或FAILURE）
    // 存在连续多帧访问并行节点的情况，而每个子节点得到更新结果所需的时间不同，
    // 所谓的并行，就是在一帧内启动所有节点，可以想像，在这一帧内大多的节点都
    // 会返回RUNNING状态，在后续的几帧中，会有陆续节点转换到SUCCESS或FALURE
    std::vector<bool> children_node_done_;
};


// 重新评估选择节点，查看左侧高优先级节点的条件是否满足
bool PreconditionNode::Reevaluation()
{
  if (parent_node_ptr_ != nullptr && parent_node_ptr_->GetBehaviorType() == BehaviorType::SELECTOR
      && (abort_type_ == AbortType::LOW_PRIORITY || abort_type_ ==  AbortType::BOTH))
  {
    //BehaviorNode 没有GetChildren等派生类的方法，因此不能用多态指针
    auto parent_selector_node_ptr = std::dynamic_pointer_cast<SelectorNode>(parent_node_ptr_);
    auto parent_children = parent_selector_node_ptr->GetChildren();
    auto iter_in_parent = std::find(parent_children.begin(), parent_children.end(), shared_from_this());
    // 意外，没找到
    if (iter_in_parent == parent_children.end()) {
      std::cerr << "Can't find current node in parent!" << std::endl;
      return false;
    }
    // 计算当前选择节点在parent_children中的位置
    unsigned int index_in_parent = iter_in_parent - parent_children.begin();
    // 当前节点优先级高于运行节点（在运行节点左侧）,不包含当前前提节点是正在运行节点的情况
    if (index_in_parent < parent_selector_node_ptr->GetChildrenIndex())
    {
      // 符合准入条件，则将待运行节点设置为当前节点
      if(Precondition())
      {
        // 终止掉低优先级节点
        parent_children.at(parent_selector_node_ptr->GetChildrenIndex())->Reset();
        parent_selector_node_ptr->SetChildrenIndex(index_in_parent);
        // 还没有终止子节点
        return true;
      }
      else
      {
        return false;
      }
    }
  }

  // 如果准入条件是false，则终止自己
  if(abort_type_== AbortType::SELF || abort_type_== AbortType::BOTH
      //这个条件是考虑了什么情况呢？ 
      || child_node_ptr_->GetBehaviorState() != BehaviorState::RUNNING){
    if(!Precondition()){
      return false;
    }
  }

  // abort_type = NONE
  return true;
}

#endif
