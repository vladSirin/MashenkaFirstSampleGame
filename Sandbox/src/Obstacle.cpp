﻿#pragma once

#include "Obstacle.h"

void Obstacle::Init()
{
    LoadAsset();
}

void Obstacle::LoadAsset()
{
    // Load the triangle
    switch (m_Type)
    {
    case Type::Triangle:
        m_Texture = Mashenka::Texture2D::Create("assets/textures/Triangle.png");
        break;
    case Type::Rectangle:
        break;
    case Type::Circle:
        break;
    }
}

bool Obstacle::CheckPointCollision(const glm::vec2& point)
{
    switch (m_Type)
    {
        case Type::Triangle:
            return PointInTriangle(point, m_Vertices[0], m_Vertices[1], m_Vertices[2]);
        default:
            return false;
    }
}

void Obstacle::Render() const
{
    // Render the triangle
    Mashenka::Renderer2D::DrawRotatedQuad(m_Position, m_Scale, m_Rotation, m_Texture);
}

void Obstacle::Update(Mashenka::TimeStep ts)
{
}

/**
 * @brief Checks if a point is inside a triangle.
 * 
 * This function uses the barycentric coordinate method to determine if a point is inside a triangle.
 * The barycentric coordinate method involves calculating the areas of three triangles and comparing them.
 * 
 * @param p The point to check.
 * @param a One vertex of the triangle.
 * @param b Another vertex of the triangle.
 * @param c The third vertex of the triangle.
 * @return True if the point is inside the triangle, false otherwise.
 */
bool PointInTriangle(const glm::vec2& p, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    // Calculate the area of the original triangle
    float areaOrig = glm::abs((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));

    // Calculate the areas of three triangles formed by the point and the vertices of the triangle
    float area1 = glm::abs((a.x - p.x) * (b.y - p.y) - (a.y - p.y) * (b.x - p.x));
    float area2 = glm::abs((b.x - p.x) * (c.y - p.y) - (b.y - p.y) * (c.x - p.x));
    float area3 = glm::abs((c.x - p.x) * (a.y - p.y) - (c.y - p.y) * (a.x - p.x));

    // If the sum of the areas of the three triangles is equal to the area of the original triangle, the point is inside the triangle
    return (area1 + area2 + area3 == areaOrig);
}
