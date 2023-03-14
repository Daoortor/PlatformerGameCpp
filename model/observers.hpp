#ifndef PLATFORMERGAMECPP_OBSERVERS_HPP_
#define PLATFORMERGAMECPP_OBSERVERS_HPP_

namespace Platformer::observers {
class Observer {
public:
    virtual void update() = 0;
};
}  // namespace Platformer::observers

#endif  // PLATFORMERGAMECPP_OBSERVERS_HPP_
