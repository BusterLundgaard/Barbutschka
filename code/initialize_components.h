#include "component.h"
#include "components.h"

inline void initialize_components() {
    _Sprite::initialize();
    _Transform::initialize();
    _Velocity::initialize();
    _Collider::initialize();
    _Level::initialize();
    _Player::initialize();
    _Oscillator::initialize();
    _Animation_player::initialize();
}