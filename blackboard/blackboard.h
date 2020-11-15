#ifndef BLACKBOARD_H
#define BLACKBOARD_H

enum class Position
{
    HOME,
    MINE,
    // BANK,
    SCHOOL
};

class BlackBoard
{
    public:
        typedef std::shared_ptr<BlackBoard> Ptr;
        BlackBoard():
            position_(Position::HOME), energy_(100){}

        bool isHome(){return position_ == Position::HOME;}
        bool isMine(){return position_ == Position::MINE;}
        bool isSchoolOver(){return (position_==Position::MINE && destination_ == Position::HOME) 
                || (position_==Position::MINE && destination_ == Position::SCHOOL)
                || position_==Position::SCHOOL;}

        bool isEnergyLow(){return energy_ <= 20;}

        void setPosition(Position postion){position_ = postion;}
        void setDestination(Position destination){destination_ = destination;}
        void adjustEnergy(int v)
        {
            energy_ += v;
            if (energy_ > 100) energy_ = 100;
            if (energy_ < 0) energy_ = 0;
        }

        void Info()
        {
            // std::cout << "money_: " << money_ << " energy_: " <<  energy_ << " weight_: " << weight_ << std::endl;
            std::cout << " energy_: " <<  energy_  << std::endl;
        }

        unsigned int frame = 0;

    private:
        // 位置
        Position position_;
        Position destination_;
        // 精力值0~100
        unsigned int energy_;
        
};

#endif
