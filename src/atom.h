#ifndef atom_h
#define atom_h

#include <iostream>
#include <string>

class Atom
{
    public :
        Atom(float ,float ,float ,float , std::string, std::string c,float ,float);
        ~Atom();
        float get_x();
        float get_y();
        float get_z();
        void move_x(float);
        void move_y(float);
        void move_z(float);
        float get_mass();
        void set_mass(float);
        std::string get_type();
        void set_type(std::string);
        float get_r();
        float get_g();
        float get_b();
        float get_ionic_radius();
        float get_covalent_radius();
    private :
        float posX,posY,posZ,mass,ionicRadius,covalentRadius;
        float R,G,B;
        std::string type;
};

#endif
