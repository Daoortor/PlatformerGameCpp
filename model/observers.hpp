#ifndef PLATFORMERGAMECPP_OBSERVERS_HPP_
#define PLATFORMERGAMECPP_OBSERVERS_HPP_

#include "player_fwd.hpp"

namespace Platformer::observers {
    class Observer {
    public:
        virtual void update() {};
    };

    class PhysicsObserver : public Observer {
        Player *player;
        void updateCollision();
        void updateGravity();
        void updateMovement();
    public:
        explicit PhysicsObserver(Player *player_);
        void update() override;
    };

    class PoseObserver : public Observer {
        Player *player;
    public:
        explicit PoseObserver(Player *player_);
        void update() override;
    };
}

#endif  // PLATFORMERGAMECPP_OBSERVERS_HPP_
