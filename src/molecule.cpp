#include "src/molecule.h"

Molecule::Molecule()
{
    inertiaTensor << 0,0,0,0,0,0,0,0,0;
    nAtom = 0;
    symPrecision=0.001f;
}

Molecule::~Molecule()
{

}

void Molecule::add_atom(Atom newAtom)
{
    atomList.push_back(newAtom);
    mass += newAtom.get_mass();
    nAtom += 1;
}

void Molecule::compute_inertia_tensor()
{
    Eigen::EigenSolver<Eigen::MatrixXf> eig;

    // Moving the origin to the center of mass to compute
    // the inertia tensor
    this->center();
    //Computing the inertia tensor
    for(unsigned int i=0;i<nAtom;i++)
    {
        Atom& a=atomList[static_cast<unsigned long>(i)];
        inertiaTensor(0,0) += a.get_mass()*(a.get_y()*a.get_y()+a.get_z()*a.get_z());
        inertiaTensor(1,1) += a.get_mass()*(a.get_x()*a.get_x()+a.get_z()*a.get_z());
        inertiaTensor(2,2) += a.get_mass()*(a.get_x()*a.get_x()+a.get_y()*a.get_y());
        inertiaTensor(0,1) -= a.get_mass()*(a.get_x()*a.get_y());
        inertiaTensor(0,2) -= a.get_mass()*(a.get_x()*a.get_z());
        inertiaTensor(1,2) -= a.get_mass()*(a.get_y()*a.get_z());
        inertiaTensor(1,0)  = inertiaTensor(0,1);
        inertiaTensor(2,0)  = inertiaTensor(0,2);
        inertiaTensor(2,1)  = inertiaTensor(1,2);
    }
    // Compute the eigenvalues and eigenvectors of the
    // inertia tensor
    eig.compute(inertiaTensor);
    IEigenVec.resize(3);

    for(unsigned int i=0;i<3;i++)
    {
        IEigenVal[i] = static_cast<float>(eig.eigenvalues()(i).real());
        IEigenVec[i] = eig.eigenvectors().col(i).real();
    }
}

void Molecule::center()
{
    Eigen::Vector3f com;

    com = this->get_center_of_mass();

    //This moves the origin to the center of mass
    for(unsigned int i=0;i<nAtom;i++)
    {
        atomList[i].move_x(atomList[i].get_x()-static_cast<float>(com(0)));
        atomList[i].move_y(atomList[i].get_y()-static_cast<float>(com(1)));
        atomList[i].move_z(atomList[i].get_z()-static_cast<float>(com(2)));
    }
}

float Molecule::get_inertia_eigenval(int n)
{
    return IEigenVal[n];
}


// function molecule.find_point_group()
//
// Author: Leo Gaspard (leo.gaspard@outlook.fr)
// Date  : August 2020
//
// Function : Uses the procedure described in Beruski, O., Vidal, L. N.. J. Comput. Chem. 2014, 35, 290– 299.
// DOI: 10.1002/jcc.23493 to determine the point group of a the object molecule.
//
// Usage :
//
// 	Molecule *myMol = new Molecule();
// 	myMol->add_atom(Atom a);
// 	...
// 	myMol->find_point_group();
// 	...
// 	delete myMol;

void Molecule::find_point_group()
{
    float                        I[3];

    this->compute_inertia_tensor();
    this->compute_distance_matrix();
    this->compute_SEA();
    this->check_inversion();
    properRotation.clear();
    properRotation.resize(1);
    for(unsigned i=0;i<SEA.size();i++)
    {
        if(SEA[i].size()>1)
        {
            Molecule *seaMol = new Molecule();
            for(unsigned int j=0; j<SEA[i].size();j++)
            {
                seaMol->add_atom(atomList[static_cast<unsigned int>(SEA[i][j])]);
            }


            // Locating the reflexion planes

            if(seaMol->get_n_atom() > 1)
            {
                for(unsigned int j=0;j<seaMol->get_n_atom()-1;j++)
                {
                    if(planes.size()==15) // 15 is the maximum number of reflexion planes, in Ih
                    {
                        break;
                    }
                    Eigen::Vector3f rA = Eigen::Vector3f(seaMol->get_atom(j).get_x(),seaMol->get_atom(j).get_y(),seaMol->get_atom(j).get_z());
                    for(unsigned int k=j+1;k<seaMol->get_n_atom();k++)
                    {
                        if(planes.size()==15) // 15 is the maximum number of reflexion planes, in Ih
                        {
                            break;
                        }
                        Eigen::Vector3f rB = Eigen::Vector3f(seaMol->get_atom(k).get_x(),seaMol->get_atom(k).get_y(),seaMol->get_atom(k).get_z());

                        Eigen::Vector3f n = rA-rB;
                        n.normalize();

                        if(n.norm() > symPrecision && this->check_plane(n))
                        {
                            if(!this->is_plane_found(n))
                            {
                                planes.push_back(n);
                            }
                        }

                        n = rA.cross(rB);
                        n.normalize();
                        if(n.norm() > symPrecision && this->check_plane(n))
                        {
                            if(!this->is_plane_found(n))
                            {
                                planes.push_back(n);
                            }
                        }
                    }
                }
            }


            // Algorithm from Figure 3

            if(seaMol->get_n_atom() > 2)
            {
                unsigned int nMax(1);
                seaMol->compute_inertia_tensor();
                //Sort IA < IB < IC
                I[0] = seaMol->get_inertia_eigenval(0);
                I[1] = seaMol->get_inertia_eigenval(1);
                I[2] = seaMol->get_inertia_eigenval(2);
                std::sort(I,I+sizeof(I)/sizeof(I[0]));

                // Locating the possible C2 axis folloxing procedure of p.3
                for(unsigned int a=0;a<seaMol->get_n_atom()-1;a++)
                {
                    if(properRotation[0].size()==15) // 15 is the maximum number of C2, in Ih
                    {
                        break;
                    }
                    for(unsigned int b=a+1;b<seaMol->get_n_atom();b++)
                    {
                        if(properRotation[0].size()==15) // 15 is the maximum number of C2, in Ih
                        {
                            break;
                        }
                        Eigen::Vector3f c2m, c2a, c2b;

                        c2a << seaMol->get_atom(a).get_x(), seaMol->get_atom(a).get_y(), seaMol->get_atom(a).get_z();
                        c2b << seaMol->get_atom(b).get_x(), seaMol->get_atom(b).get_y(), seaMol->get_atom(b).get_z();
                        c2m = (c2a+c2b)/2;

                        if(c2a.norm() > symPrecision && this->check_proper_rotation(2,c2a))
                        {
                            if(properRotation.size() < 1)
                            {
                                properRotation.resize(1);
                            }
                            if(!(this->is_proper_rotation_found(2,c2a.normalized())))
                            {
                                properRotation[0].push_back(c2a.normalized());
                            }
                        }
                        if(c2b.norm() > symPrecision && this->check_proper_rotation(2,c2b))
                        {
                            if(properRotation.size() < 1)
                            {
                                properRotation.resize(1);
                            }
                            if(!(this->is_proper_rotation_found(2,c2b.normalized())))
                            {
                                properRotation[0].push_back(c2b.normalized());
                            }
                        }
                        if(c2m.norm() > symPrecision && this->check_proper_rotation(2,c2m))
                        {
                            if(properRotation.size() < 1)
                            {
                                properRotation.resize(1);
                            }
                            if(!(this->is_proper_rotation_found(2,c2m.normalized())))
                            {
                                properRotation[0].push_back(c2m.normalized());
                            }
                        }
                    }
                }

                // Locating highest order C
                if(abs(I[0]+I[1]-I[2]) <= symPrecision)
                {
                    // IA + IB = IC
                    nMax = seaMol->get_n_atom();

                    Eigen::Vector3f com, ra, rb, trialAxis;
                    com = seaMol->get_center_of_mass();

                    ra << seaMol->get_atom(0).get_x(), seaMol->get_atom(0).get_y(), seaMol->get_atom(0).get_z();
                    rb << seaMol->get_atom(1).get_x(), seaMol->get_atom(1).get_y(), seaMol->get_atom(1).get_z();

                    ra -= com;
                    rb -= com;

                    trialAxis = ra.cross(rb);
                    for(unsigned int n=nMax; n>1; n--)
                    {
                        if(nMax%n==0)
                        {
                            if(this->check_proper_rotation(n,trialAxis))
                            {
                                if(properRotation.size() < n-1)
                                {
                                    properRotation.resize(n-1);
                                }
                                if(!(this->is_proper_rotation_found)(n,trialAxis.normalized()))
                                {
                                    properRotation[n-2].push_back(trialAxis.normalized());
                                }
                            }
                        }
                    }
                    trialAxis = ra-rb;
                    for(unsigned int n=nMax; n>1; n--)
                    {
                        if(nMax%n==0)
                        {
                            if(this->check_proper_rotation(n,trialAxis))
                            {
                                if(properRotation.size() < n-1)
                                {
                                    properRotation.resize(n-1);
                                }
                                if(!(this->is_proper_rotation_found)(n,trialAxis.normalized()))
                                {
                                    properRotation[n-2].push_back(trialAxis.normalized());
                                }
                            }
                        }
                    }
                }
                else
                {
                    for(unsigned int t=0;t<3;t++)
                    {
                        Eigen::Vector3f trialAxis = seaMol->get_inertia_eigenvec(t);
                        unsigned int k = static_cast<unsigned int>(floor(static_cast<float>(seaMol->get_n_atom())/2));

                        for(unsigned int j=2;j<=k;j++)
                        {
                            if(this->check_proper_rotation(j,trialAxis.normalized()))
                            {
                                if(properRotation.size() < j-1)
                                {
                                    properRotation.resize(j-1);
                                }
                                if(!(this->is_proper_rotation_found(j,trialAxis.normalized())))
                                {
                                    properRotation[j-2].push_back(trialAxis.normalized());
                                }
                            }
                        }
                    }

                }
            }
            else if(seaMol->get_n_atom() == 2)
            {
                Atom a = seaMol->get_atom(0);
                Atom b = seaMol->get_atom(1);

                Eigen::Vector3f rA = Eigen::Vector3f(a.get_x(),a.get_y(),a.get_z());
                Eigen::Vector3f rB = Eigen::Vector3f(b.get_x(),b.get_y(),b.get_z());
                if(properRotation.size() < 1)
                {
                    properRotation.resize(1);
                }
                //C2 orthogonal to A-B
                Eigen::Vector3f trialC2 = (rA+rB)/2;
                if(trialC2.norm() > symPrecision && this->check_proper_rotation(2,trialC2.normalized()))
                {
                    if(!(this->is_proper_rotation_found(2,trialC2.normalized())))
                    {
                        properRotation[0].push_back(trialC2.normalized());
                    }
                }
                //Cn can exist through A-B

                //Checking for any other SEA of size 2 for C2 orthogonal to both SEA

                for(unsigned int j=i;j<SEA.size();j++)
                {
                    if(SEA[j].size() == 2)
                    {
                        Eigen::Vector3f rC = Eigen::Vector3f(atomList[SEA[j][0]].get_x(),atomList[SEA[j][0]].get_y(),atomList[SEA[j][0]].get_z());
                        Eigen::Vector3f rD = Eigen::Vector3f(atomList[SEA[j][1]].get_x(),atomList[SEA[j][1]].get_y(),atomList[SEA[j][1]].get_z());

                        Eigen::Vector3f rAB = rA-rB;
                        Eigen::Vector3f rCD = rC-rD;

                        Eigen::Vector3f C2 =  rAB.cross(rCD);

                        if(C2.norm() > symPrecision && this->check_proper_rotation(2,C2.normalized()))
                        {
                            if(!(this->is_proper_rotation_found(2,C2.normalized())))
                            {
                                properRotation[0].push_back(C2.normalized());
                            }
                        }

                    }
                }
            }
            delete seaMol;
        }
    }
    std::sort(IEigenVal,IEigenVal+sizeof(IEigenVal)/sizeof(IEigenVal[0]));
    // Case of spherical symmetry
    if(abs(IEigenVal[0]-IEigenVal[1]) <= symPrecision && abs(IEigenVal[1]-IEigenVal[2]) <= symPrecision)
    {
        switch(properRotation[0].size())
        {
            case 3:
                //Tetrahedral
                properRotation.resize(2);
                improperRotation.resize(5);

                if(planes.size()==6)
                {
                    //Td
                    pointGroup = "Td";
                }
                else if(planes.size()==3)
                {
                    //Th
                    pointGroup = "Th";
                }
                else
                {
                    //T
                    pointGroup = "T";
                }
                break;
            case 9:
                //Octahedral
                if(inversion)
                {
                    //Oh
                    pointGroup = "Oh";
                }
                else
                {
                    //O
                    pointGroup = "O";
                }
                break;
            case 15:
                //Isocahedral
                if(inversion)
                {
                    //Ih
                    pointGroup = "Ih";
                }
                else
                {
                    //I
                    pointGroup = "I";
                }
                break;
            default:
                std::cout << "Non identified spherical point group" << std::endl;

        }
    }
    else if(IEigenVal[0] <= symPrecision && abs(IEigenVal[1]-IEigenVal[2]) <= symPrecision)
    {
        if(inversion)
        {
            pointGroup = "Dooh";
        }
        else
        {
            pointGroup = "Coov";
        }
    }
    else
    {
        if(properRotation.size()==1 && properRotation[0].size() >= 1)
        {
            if(inversion)
            {
                if(planes.size()>1)
                {
                    pointGroup = "D2h";
                }
                else
                {
                    pointGroup = "C2h";
                }
            }
            else
            {
                if(planes.size()>0)
                {
                    if(properRotation[0].size() > 1)
                    {
                        pointGroup = "D2d";

                    }
                    else
                    {
                        pointGroup = "C2v";
                    }

                }
                else
                {
                    if(properRotation[0].size() > 1)
                    {
                        pointGroup = "D2";
                    }
                    else
                    {
                        pointGroup = "C2";
                    }
                }
            }
        }
        else if(properRotation.size()>1)
        {
            unsigned int nC2(0);
            unsigned long n(properRotation.size()+1);
            for(unsigned int i=0; i<properRotation[0].size(); i++)
            {
                if(abs(properRotation[0][i].dot(properRotation[n-2][0])) <= symPrecision)
                {
                    nC2++;
                }
            }
            bool sigmah = false;
            bool sigmav = false;
            for(unsigned int i=0;i<planes.size();i++)
            {
                if(abs(planes[i].dot(properRotation[n-2][0])-1) <= symPrecision)
                {
                    sigmah = true;
                }
                if(abs(planes[i].dot(properRotation[n-2][0])) <= symPrecision)
                {
                    sigmav = true;
                }
            }
            if(n == nC2 && sigmah)
            {
                pointGroup = "D"+std::to_string(n)+"h";
            }
            else if(n == nC2 && !sigmah && sigmav)
            {
                pointGroup = "D"+std::to_string(n)+"d";
            }
            else if(n==nC2 && !sigmah && !sigmav)
            {
                pointGroup = "D"+std::to_string(n);
            }
            else if(n!=nC2 && sigmah)
            {
                pointGroup = "C"+std::to_string(n)+"h";
            }
            else if(n!=nC2 && !sigmah && sigmav)
            {
                pointGroup = "C"+std::to_string(n)+"v";
            }
            else
            {
                pointGroup = "C"+std::to_string(n);
            }
        }
        else
        {
            if(planes.size()!=0)
            {
                pointGroup = "Cs";
            }
            else if(inversion)
            {
                pointGroup = "Ci";
            }
            else
            {
                pointGroup = "C1";
            }
        }
    }


    // Checking for the improper rotations, colinear to the proper rotations
    for(unsigned int i=0; i<properRotation.size();i++)
    {
        for(unsigned int j=0; j<properRotation[i].size();j++)
        {
            if(this->check_improper_rotation(i+2,properRotation[i][j]))
            {
                if(improperRotation.size() < i+1)
                {
                    improperRotation.resize(i+1);
                }
                if(!this->is_improper_rotation_found(i+2,properRotation[i][j]) && i>0)
                {
                    improperRotation[i].push_back(properRotation[i][j]);
                }
            }
            if(this->check_improper_rotation(2*(i+2),properRotation[i][j]))
            {
                if(improperRotation.size() < 2*(i+1)+1)
                {
                    improperRotation.resize(2*(i+1)+1);
                }
                if(!this->is_improper_rotation_found(2*(i+2),properRotation[i][j]))
                {
                    improperRotation[2*(i+1)].push_back(properRotation[i][j]);
                }
            }
        }
    }

}

void Molecule::compute_SEA()
{
    std::vector<int> treated;
    SEA.clear();

    for(unsigned int i=0;i<nAtom;i++)
    {
        sort(distanceMatrix[i].begin(),distanceMatrix[i].end());
    }
    for(unsigned int i=0;i<nAtom-1;i++)
    {
        if(std::find(treated.begin(),treated.end(),i)==treated.end())
        {
            treated.push_back(static_cast<int>(i));
            SEA.resize(SEA.size()+1);
            SEA[SEA.size()-1].push_back(i);
            for(unsigned int j=i+1;j<nAtom;j++)
            {
                bool seq(true);
                if(atomList[i].get_type() == atomList[j].get_type())
                {
                    for(unsigned int k=0;k<nAtom;k++)
                    {
                        if(abs(distanceMatrix[i][k]-distanceMatrix[j][k]) > symPrecision)
                        {
                            seq=false;
                        }
                    }
                }
                else
                {
                    seq = false;
                }
                if(seq)
                {
                    treated.push_back(static_cast<int>(j));
                    SEA[SEA.size()-1].push_back(j);
                }
            }
        }
    }
}

void Molecule::compute_distance_matrix()
{
    distanceMatrix.clear();
    distanceMatrix.resize(static_cast<unsigned long>(nAtom));
    for(unsigned int i=0;i<nAtom;i++)
    {
        distanceMatrix[i].resize(static_cast<unsigned long>(nAtom));
        for(unsigned int j=0;j<nAtom;j++)
        {
            distanceMatrix[i][j]=0;
        }
    }
    for(unsigned int i=0;i<nAtom-1;i++)
    {
        for(unsigned int j=i+1;j<nAtom;j++)
        {
            Atom& a=atomList[i];
            Atom& b=atomList[j];

            distanceMatrix[i][j] = static_cast<float>(sqrt(pow(static_cast<double>(a.get_x()-b.get_x()),2.0)+pow(static_cast<double>(a.get_y()-b.get_y()),2.0)+pow(static_cast<double>(a.get_z()-b.get_z()),2)));
            distanceMatrix[j][i] = distanceMatrix[i][j];
        }
    }
}

unsigned int Molecule::get_n_atom()
{
    return nAtom;
}

Atom& Molecule::get_atom(unsigned int i)
{
    return atomList[i];
}

bool Molecule::check_proper_rotation(unsigned int n, Eigen::Vector3f C)
{
    Eigen::MatrixXf posMat, rotPos;
    Eigen::Matrix3f rotMat;
    posMat.resize(nAtom,3);
    rotPos.resize(nAtom,3);
    float a = 2*3.14159265359f/static_cast<float>(n); //Angle for the rotation around C
    float c = std::cos(a);
    float s = std::sin(a);
    C.normalize();

    rotMat << C(0)*C(0)*(1-c)+c, C(0)*C(1)*(1-c)-C(2)*s, C(0)*C(2)*(1-c)+C(1)*s,
              C(0)*C(1)*(1-c)+C(2)*s, C(1)*C(1)*(1-c)+c, C(1)*C(2)*(1-c)-C(0)*s,
              C(0)*C(2)*(1-c)-C(1)*s, C(1)*C(2)*(1-c)+C(0)*s, C(2)*C(2)*(1-c)+c;

    for(unsigned int i=0; i<nAtom; i++)
    {
        posMat(i,0) = atomList[i].get_x();
        posMat(i,1) = atomList[i].get_y();
        posMat(i,2) = atomList[i].get_z();
    }

    rotPos = rotMat*posMat.transpose();

    float dx, dy, dz;

    for(unsigned int i=0; i<nAtom;i++)
    {
        bool isIn(false);
        for(unsigned int j=0; j<nAtom; j++)
        {
            dx = abs(posMat(i,0)-rotPos(0,j));
            dy = abs(posMat(i,1)-rotPos(1,j));
            dz = abs(posMat(i,2)-rotPos(2,j));
            if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
            {
                isIn = true;
            }
        }
        if(!isIn)
        {
            return false;
        }
    }
    return true;
}

void Molecule::compute_adjacency_matrix()
{
    adjacencyMatrix.clear();
    adjacencyMatrix.resize(nAtom);
    for(unsigned int i=0; i<nAtom; i++)
    {
        adjacencyMatrix[i].resize(nAtom);
    }
    this->compute_distance_matrix();
    for(unsigned int i=0; i<nAtom-1; i++)
    {
        for(unsigned int j=i+1; j<nAtom; j++)
        {
            float d = distanceMatrix[i][j];
            if(d < 0.8f)
            {
                std::cout << "Two atoms on top of each other" << std::endl;
            }
            else if (d < atomList[i].get_covalent_radius() + atomList[j].get_covalent_radius() + 0.2f)
            {
                adjacencyMatrix[i][j] = 1;
                adjacencyMatrix[j][i] = 1;
            }
        }
    }
}

void Molecule::check_inversion()
{
    Eigen::MatrixXf posMat, rotPos;
    Eigen::Matrix3f rotMat;
    posMat.resize(nAtom,3);
    rotPos.resize(nAtom,3);

    rotMat << -1,0,0,
              0,-1,0,
          0,0,-1;

    for(unsigned int i=0; i<nAtom; i++)
    {
        posMat(i,0) = atomList[i].get_x();
        posMat(i,1) = atomList[i].get_y();
        posMat(i,2) = atomList[i].get_z();
    }

    rotPos = rotMat*posMat.transpose();

    float dx, dy, dz;

    for(unsigned int i=0; i<nAtom;i++)
    {
        bool isIn(false);
        for(unsigned int j=0; j<nAtom; j++)
        {
            dx = abs(posMat(i,0)-rotPos(0,j));
            dy = abs(posMat(i,1)-rotPos(1,j));
            dz = abs(posMat(i,2)-rotPos(2,j));
            if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
            {
                isIn = true;
            }
        }
        if(!isIn)
        {
            inversion=false;
        }
    }
}

bool Molecule::check_plane(Eigen::Vector3f C)
{
    Eigen::MatrixXf posMat, rotPos;
    Eigen::Matrix3f rotMat;
    posMat.resize(nAtom,3);
    rotPos.resize(nAtom,3);
    C.normalize();

    rotMat << 1-2*C(0)*C(0), -2*C(0)*C(1), -2*C(0)*C(2),
              -2*C(0)*C(1), 1-2*C(1)*C(1), -2*C(1)*C(2),
          -2*C(0)*C(2), -2*C(1)*C(2), 1-2*C(2)*C(2);

    for(unsigned int i=0; i<nAtom; i++)
    {
        posMat(i,0) = atomList[i].get_x();
        posMat(i,1) = atomList[i].get_y();
        posMat(i,2) = atomList[i].get_z();
    }

    rotPos = rotMat*posMat.transpose();

    float dx, dy, dz;

    for(unsigned int i=0; i<nAtom;i++)
    {
        bool isIn(false);
        for(unsigned int j=0; j<nAtom; j++)
        {
            dx = abs(posMat(i,0)-rotPos(0,j));
            dy = abs(posMat(i,1)-rotPos(1,j));
            dz = abs(posMat(i,2)-rotPos(2,j));
            if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
            {
                isIn = true;
            }
        }
        if(!isIn)
        {
            return false;
        }
    }
    return true;
}

bool Molecule::is_proper_rotation_found(unsigned int n, Eigen::Vector3f C)
{
    for(unsigned int i=0; i<properRotation[n-2].size(); i++)
    {
        float dx = abs(properRotation[n-2][i](0) - C(0));
        float dy = abs(properRotation[n-2][i](1) - C(1));
        float dz = abs(properRotation[n-2][i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }

        dx = abs(-properRotation[n-2][i](0) - C(0));
        dy = abs(-properRotation[n-2][i](1) - C(1));
        dz = abs(-properRotation[n-2][i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }
    }
    return false;
}

bool Molecule::check_improper_rotation(unsigned int n, Eigen::Vector3f C)
{
    Eigen::MatrixXf posMat, rotPos;
    Eigen::Matrix3f rotMat;
    posMat.resize(nAtom,3);
    rotPos.resize(nAtom,3);
    float a = 2*3.14159265359f/static_cast<float>(n); //Angle for the rotation around C
    float c = std::cos(a);
    float s = std::sin(a);
    C.normalize();

    rotMat << c-C(0)*C(0)*(1+c), -C(0)*C(1)*(1+c)-C(2)*s, -C(0)*C(2)*(1+c)+C(1)*s,
              -C(0)*C(1)*(1+c)+C(2)*s, c-C(1)*C(1)*(1+c), -C(1)*C(2)*(1+c)-C(0)*s,
          -C(0)*C(2)*(1+c)-C(1)*s, -C(1)*C(2)*(1+c)+C(0)*s, c-C(2)*C(2)*(1+c);

    for(unsigned int i=0; i<nAtom; i++)
    {
        posMat(i,0) = atomList[i].get_x();
        posMat(i,1) = atomList[i].get_y();
        posMat(i,2) = atomList[i].get_z();
    }

    rotPos = rotMat*posMat.transpose();

    float dx, dy, dz;

    for(unsigned int i=0; i<nAtom;i++)
    {
        bool isIn(false);
        for(unsigned int j=0; j<nAtom; j++)
        {
            dx = abs(posMat(i,0)-rotPos(0,j));
            dy = abs(posMat(i,1)-rotPos(1,j));
            dz = abs(posMat(i,2)-rotPos(2,j));
            if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
            {
                isIn = true;
            }
        }
        if(!isIn)
        {
            return false;
        }
    }
    return true;
}

Eigen::Vector3f Molecule::get_center_of_mass()
{
    Eigen::Vector3f com(0.0,0.0,0.0);
    for(unsigned int i=0;i<nAtom;i++)
    {
        com(0) += atomList[i].get_x()*atomList[i].get_mass();
        com(1) += atomList[i].get_y()*atomList[i].get_mass();
        com(2) += atomList[i].get_z()*atomList[i].get_mass();
    }
    com(0) /= mass;
    com(1) /= mass;
    com(2) /= mass;

    return com;
}

bool Molecule::is_plane_found(Eigen::Vector3f C)
{
    for(unsigned int i=0; i<planes.size(); i++)
    {
        float dx = abs(planes[i](0) - C(0));
        float dy = abs(planes[i](1) - C(1));
        float dz = abs(planes[i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }

        dx = abs(-planes[i](0) - C(0));
        dy = abs(-planes[i](1) - C(1));
        dz = abs(-planes[i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }
    }
    return false;
}

bool Molecule::is_improper_rotation_found(unsigned int n, Eigen::Vector3f C)
{
    for(unsigned int i=0; i<improperRotation[n-2].size(); i++)
    {
        float dx = abs(improperRotation[n-2][i](0) - C(0));
        float dy = abs(improperRotation[n-2][i](1) - C(1));
        float dz = abs(improperRotation[n-2][i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }

        dx = abs(-improperRotation[n-2][i](0) - C(0));
        dy = abs(-improperRotation[n-2][i](1) - C(1));
        dz = abs(-improperRotation[n-2][i](2) - C(2));
        if(dx <= symPrecision && dy <= symPrecision && dz <= symPrecision)
        {
            return true;
        }
    }
    return false;
}

Eigen::Vector3f Molecule::get_inertia_eigenvec(unsigned int n)
{
    return IEigenVec[n];
}

std::string Molecule::get_point_group()
{
    return pointGroup;
}

int Molecule::get_adjacency(unsigned int i, unsigned int j)
{
    return adjacencyMatrix[i][j];
}

void Molecule::read_from_xyz(std::string fileName)
{
    std::string atomName("");
    std::string element[118], colors[118];
    int atomNumber(0),count(0);
    float filePosX(0.0),filePosY(0.0),filePosZ(0.0);
    float weight[118];
    float ionicRadius[118], covalentRadius[118];

    std::ifstream periodic("data/periodic.csv");
    if(periodic.is_open())
    {
        for(int i=0;i<118;i++)
        {
            periodic >> element[i];
            periodic >> weight[i];
            periodic >> colors[i];
            periodic >> ionicRadius[i];
            periodic >> covalentRadius[i];
        }
        periodic.close();
    }
    else
    {
        std::cout << "Error : Can not open the periodic.csv file" << std::endl;
    }
    std::ifstream file(fileName);

    if(file.is_open())
    {
        file >> atomNumber;
        while(count < atomNumber)
        {
            float m(0.0), iR(0.0), cR(0.0);
            std::string c;
            file >> atomName;
            file >> filePosX;
            file >> filePosY;
            file >> filePosZ;
            //The next line find the mass of the given atom
            auto pos(std::find(element,element+sizeof(element)/sizeof(element[0]),atomName));
            if(pos != std::end(element))
            {
                int p = static_cast<int>(distance(element,pos));
                m = weight[p];
                c = colors[p];
                iR = ionicRadius[p];
                cR = covalentRadius[p];
            }
            else
            {
                std::cout << "Error : no such element in the periodic table " << atomName << std::endl;
            }
            Atom a(filePosX,filePosY,filePosZ,m,atomName,c,iR,cR);
            this->add_atom(a);
            count++;
        }
        file.close();
    }
    else
    {
        std::cout << "Error can't open the file :  " << fileName << std::endl;
    }
}

void Molecule::symmetrize(float trsh)
{
    this->compute_distance_matrix();

    this->compute_SEA();
    std::cout << "Treshold : " << symPrecision << "Angstrom   nSEA : "  << SEA.size() << std::endl;

}
