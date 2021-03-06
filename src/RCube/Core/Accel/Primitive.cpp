#include "RCube/Core/Accel/Primitive.h"

namespace rcube
{

Point::Point(size_t id, const glm::vec3 &pos, float radius) : pos_(pos), id_(id), radius_(radius)
{
    radius_sq_ = radius * radius;
}

size_t Point::id() const
{
    return id_;
}

bool Point::rayIntersect(const Ray &ray, float &t) const
{
    const glm::vec3 L = pos_ - ray.origin();
    const float tca = glm::dot(L, ray.direction());
    const float d2 = glm::dot(L, L) - tca * tca;
    if (d2 > radius_sq_)
    {
        return false;
    }
    const float thc = std::sqrt(radius_sq_ - d2);
    const float t0 = tca - thc;
    const float t1 = tca + thc;
    t = std::min(t0, t1);
    return true;
}

glm::vec3 Point::position() const
{
    return pos_;
}

AABB Point::aabb() const
{
    const glm::vec3 radvec(radius_, radius_, radius_);
    return AABB{pos_ - radvec, pos_ + radvec};
}

Triangle::Triangle(size_t id, glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
    : id_(id), v0_(v0), v1_(v1), v2_(v2)
{
}

size_t Triangle::id() const
{
    return id_;
}

bool Triangle::rayIntersect(const Ray &ray, float &t) const
{
    const glm::vec3 v0v1 = v1_ - v0_;
    const glm::vec3 v0v2 = v2_ - v0_;
    const glm::vec3 pvec = glm::cross(ray.direction(), v0v2);
    const float det = glm::dot(v0v1, pvec);

    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (std::abs(det) < 1e-6)
    {
        return false;
    }

    float invDet = 1.f / det;
    const glm::vec3 tvec = ray.origin() - v0_;
    float u = glm::dot(tvec, pvec) * invDet;
    if (u < 0.0f || u > 1.0f)
    {
        return false;
    }

    const glm::vec3 qvec = glm::cross(tvec, v0v1);
    const float v = glm::dot(ray.direction(), qvec) * invDet;
    if (v < 0.0f || u + v > 1.0f)
    {
        return false;
    }

    const float tempt = glm::dot(v0v2, qvec) * invDet;
    if (tempt < ray.tmin() || tempt > ray.tmax())
    {
        return false;
    }
    t = tempt;
    return true;
}

glm::vec3 Triangle::position() const
{
    // This can be precomputed
    return (v0_ + v1_ + v2_) / 3.f;
}

AABB Triangle::aabb() const
{
    // This can be precomputed
    return AABB{glm::min(v0_, glm::min(v1_, v2_)), glm::max(v0_, glm::max(v1_, v2_))};
}

} // namespace rcube
