//
// Created by Karol on 08.09.2016.
//

#include "GameInterfaces.hpp"

IUpdateable::~IUpdateable() = default;
ICollider::~ICollider() = default;
void ICollider::on_collide() noexcept {}
IStaticCollider::~IStaticCollider() = default;
void IStaticCollider::updateCollision(double, std::vector<ICollider *>&) noexcept {}
IRenderable::~IRenderable() = default;