#include "src/atom.h"

Atom::Atom(float x, float y, float z, float m, std::string t, std::string c, float iR, float cR)
{
    posX = x;
    posY = y;
    posZ = z;
    mass = m;
    type = t;
    ionicRadius = iR;
    covalentRadius = cR;

    R = static_cast<float>(std::stoi(c.substr(0,2),nullptr,16))/255.0f;
    G = static_cast<float>(std::stoi(c.substr(2,2),nullptr,16))/255.0f;
    B = static_cast<float>(std::stoi(c.substr(4,2),nullptr,16))/255.0f;
}

Atom::~Atom()
{
}

float Atom::get_x()
{
    return posX;
}

float Atom::get_y()
{
    return posY;
}

float Atom::get_z()
{
    return posZ;
}

void Atom::move_x(float newX)
{
    posX = newX;
}

void Atom::move_y(float newY)
{
    posY = newY;
}

void Atom::move_z(float newZ)
{
    posZ = newZ;
}

float Atom::get_mass()
{
    return mass;
}

void Atom::set_mass(float m)
{
    mass = m;
}

void Atom::set_type(std::string t)
{
    type = t;
}

std::string Atom::get_type()
{
    return type;
}

float Atom::get_r()
{
    return R;
}
float Atom::get_g()
{
    return G;
}
float Atom::get_b()
{
    return B;
}
float Atom::get_ionic_radius()
{
    return ionicRadius;
}
float Atom::get_covalent_radius()
{
    return covalentRadius;
}
