#ifndef PLATFORMERGAMECPP_OBSERVERS_HPP_
#define PLATFORMERGAMECPP_OBSERVERS_HPP_

namespace Platformer::observers {
class Observer {
public:
    virtual void update() = 0;
    virtual ~Observer() = default;
};
}  // namespace Platformer::observers

#endif  // PLATFORMERGAMECPP_OBSERVERS_HPP_
