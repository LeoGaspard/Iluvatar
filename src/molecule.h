#ifndef molecule_h
#define molecule_h

#include <iostream>
#include "src/atom.h"
#include <eigen3/unsupported/Eigen/MatrixFunctions>
#include <eigen3/Eigen/Dense>
#include <vector>
#include <math.h>
#include <bits/stdc++.h>
#include <algorithm>


class Atom;

class Molecule
{
    public :
        Molecule();
        ~Molecule();
        void add_atom(Atom);
        void compute_inertia_tensor();
        void center();
        float get_inertia_eigenval(int);
        void find_point_group();
        void compute_distance_matrix();
        void compute_SEA();
        void compute_adjacency_matrix();
        void read_from_xyz(std::string fileName);
        int get_adjacency(unsigned int i, unsigned int j);
        unsigned int get_n_atom();
        Atom& get_atom(unsigned int i);
        void check_inversion();
        bool check_proper_rotation(unsigned int, Eigen::Vector3f);
        bool check_improper_rotation(unsigned int, Eigen::Vector3f);
        bool check_plane(Eigen::Vector3f);
        bool is_proper_rotation_found(unsigned int, Eigen::Vector3f);
        bool is_improper_rotation_found(unsigned int, Eigen::Vector3f);
        bool is_plane_found(Eigen::Vector3f);
        float get_mass(){return mass;}
        void symmetrize(float trsh);
        Eigen::Vector3f get_center_of_mass();
        Eigen::Vector3f get_inertia_eigenvec(unsigned int);
        std::string get_point_group();

    private :
        float symPrecision;
        Eigen::Matrix3f 	                            inertiaTensor;
        std::string                                     pointGroup;
        std::vector<Atom>                               atomList;
        std::vector<std::vector<float>>                 distanceMatrix;
        std::vector<std::vector<int>>                   adjacencyMatrix;
        std::vector<std::vector<unsigned int>>                   SEA; // Symmetry Equivalent Atoms
        unsigned int                                    nAtom;
        float                                           mass;
        float                                           IEigenVal[3];
        std::vector<Eigen::Vector3f>	     	        IEigenVec,planes;
        std::vector<std::vector<Eigen::Vector3f>>       properRotation,improperRotation;
        bool                                            inversion;
};

#endif
