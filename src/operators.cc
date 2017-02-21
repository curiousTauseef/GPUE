#include "../include/operators.h"

double sign(double x){
    if (x < 0){
        return -1.0;
    }
    else if (x == 0){
        return 0.0;
    }
    else{
        return 1.0;
    }
}

// Function to take the curl of Ax and Ay in 2d
// note: This is on the cpu, there should be a GPU version too.
double *curl2d(Grid &par, double *Ax, double *Ay){
    int xDim = par.ival("xDim");
    int yDim = par.ival("yDim");

    int size = sizeof(double) * xDim * yDim;
    double *curl;
    curl = (double *)malloc(size);

    int index;

    // Note: To take the curl, we need a change in x and y to create a dx or dy
    //       For this reason, we have added yDim to y and 1 to x
    for (int i = 0; i < xDim; i++){
        for (int j = 0; j < yDim-1; j++){
            index = j + yDim * i;
            curl[index] = (Ay[index] - Ay[index+yDim]) 
                          - (Ax[index] - Ax[index+1]);
        }
    }

    return curl;
}

// Function for simple 2d rotation with i and j as the interators
double rotation_K(Grid &par, Op &opr, int i, int j, int k){
    double *xp = par.dsval("xp");
    double *yp = par.dsval("yp");
    double mass = par.dval("mass");
    return (HBAR*HBAR/(2*mass))*(xp[i]*xp[i] + yp[j]*yp[j]);
}

// A simple 3d rotation with i, j, and k as integers
double rotation_K3d(Grid &par, Op &opr, int i, int j, int k){
    double *xp = par.dsval("xp");
    double *yp = par.dsval("yp");
    double *zp = par.dsval("zp");
    double mass = par.dval("mass");
    return (HBAR*HBAR/(2*mass))*(xp[i]*xp[i] + yp[j]*yp[j] + zp[k]*zp[k]);
}

// Function for simple 2d rotation with i and j as the interators dimensionless
double rotation_K_dimensionless(Grid &par, Op &opr, int i, int j, int k){
    double *xp = par.dsval("xp");
    double *yp = par.dsval("yp");
    return (xp[i]*xp[i] + yp[j]*yp[j])*0.5;
}


// Function for simple 2d rotation with i and j as the interators
double rotation_gauge_K(Grid &par, Op &opr, int i, int j, int k){
    double *xp = par.dsval("xp");
    double *yp = par.dsval("yp");
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double omega = par.dval("omega");
    double omegaX = par.dval("omegaX");
    double omega_0 = omega * omegaX;
    double mass = par.dval("mass");
    double p1 = HBAR*HBAR*(xp[i]*xp[i] + yp[j]*yp[j]);
    double p2 = mass*mass*omega_0*omega_0*(x[i]*x[i] + y[j]*y[j]);
    double p3 = 2*HBAR*mass*omega_0*(xp[i]*y[j] - yp[j]*x[i]);

    return (1/(2*mass))*(p1 + p2 + p3) *0.5;
}

// Function for simple 2d harmonic V with i and j as the iterators
double harmonic_V(Grid &par, Op &opr, int i , int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double omegaX = par.dval("omegaX");
    double omegaY = par.dval("omegaY");
    double gammaY = par.dval("gammaY");
    double yOffset = 0.0;
    double xOffset = 0.0;
    double mass = par.dval("mass");
    double V_x = omegaX*(x[i]+xOffset); 
    double V_y = gammaY*omegaY*(y[j]+yOffset);
    if (par.Afn != "file"){
        return 0.5 * mass * ( V_x * V_x + V_y * V_y) + 
               0.5 * mass * pow(opr.Ax_fn(par.Afn)(par, opr, i, j, k),2) + 
               0.5 * mass * pow(opr.Ay_fn(par.Afn)(par, opr, i, j, k),2);
    }
    else{
        double *Ax = opr.dsval("Ax");
        double *Ay = opr.dsval("Ay");
        int yDim = par.ival("yDim");
        int count = i*yDim + j; 
        return 0.5 * mass * ( V_x * V_x + V_y * V_y) + 
               0.5 * mass * pow(Ax[count],2) + 
               0.5 * mass * pow(Ay[count],2);
    }

}

// Function for simple 3d torus trapping potential
double torus_V(Grid &par, Op &opr, int i, int j, int k){
    double V;

    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double *z = par.dsval("z");

    double xMax = par.dval("xMax");
    double yMax = par.dval("yMax");

    double omegaX = par.dval("omegaX");
    double omegaY = par.dval("omegaY");
    double omegaZ = par.dval("omegaZ");
    double yOffset = par.dval("x0_shift");
    double xOffset = par.dval("y0_shift");
    double zOffset = par.dval("z0_shift");
    double mass = par.dval("mass");
    double fudge = par.dval("fudge");

    // Now we need to determine how we are representing V_xyz

    double angle = atan(x[i]/y[j]);
    if (y[j] <0){
        angle += M_PI;
    }

    // Creating a harmonic trap that stretches azimuthally in a toroidal shape
    // this is done by combining x and y into an r term and comparing this
    // to the large torus radius
    //double rMax = sqrt(xMax*xMax + yMax*yMax);
    double rMax = xMax;

    double rad = sqrt((x[i] - xOffset) * (x[i] - xOffset)
                      + (y[j] - yOffset) * (y[j] - yOffset)) - 0.5*rMax*fudge;
    double omegaR = sqrt(omegaX*omegaX + omegaY*omegaY);
    double V_r = omegaR*rad;
    V_r = V_r*V_r;

    double V_z = omegaR*(z[k]+zOffset);
    V_z = V_z * V_z;
    if (par.Afn != "file"){
        return 1 * 0.5 * mass * ( V_r + V_z) + 
               0.5 * mass * pow(opr.Ax_fn(par.Afn)(par, opr, i, j, k),2) + 
               0.5 * mass * pow(opr.Az_fn(par.Afn)(par, opr, i, j, k),2) + 
               0.5 * mass * pow(opr.Ay_fn(par.Afn)(par, opr, i, j, k),2);
    }
    else{
        double *Ax = opr.dsval("Ax");
        double *Ay = opr.dsval("Ay");
        double *Az = opr.dsval("Az");
        int yDim = par.ival("yDim");
        int zDim = par.ival("zDim");
        int count = i*yDim*zDim + j*zDim + k; 
        return 1 * 0.5 * mass * ( V_r + V_z) + 
               0.5 * mass * pow(Ax[count],2) + 
               0.5 * mass * pow(Az[count],2) + 
               0.5 * mass * pow(Ay[count],2);
    }

    return V_r + V_z;
}

// Function for simple 3d harmonic V with i and j as the iterators
double harmonic_V3d(Grid &par, Op &opr, int i , int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double *z = par.dsval("z");
    double omegaX = par.dval("omegaX");
    double omegaY = par.dval("omegaY");
    double omegaZ = par.dval("omegaZ");
    double gammaY = par.dval("gammaY");
    double yOffset = 0.0;
    double xOffset = 0.0;
    double zOffset = 0.0;
    double mass = par.dval("mass");
    double V_x = omegaX*(x[i]+xOffset); 
    double V_y = gammaY*omegaY*(y[j]+yOffset);
    double V_z = gammaY*omegaZ*(z[k]+zOffset);
    if (par.Afn != "file"){
        return 0.5 * mass * ( V_x * V_x + V_y * V_y + V_z*V_z) + 
               0.5 * mass * pow(opr.Ax_fn(par.Afn)(par, opr, i, j, k),2) + 
               0.5 * mass * pow(opr.Az_fn(par.Afn)(par, opr, i, j, k),2) + 
               0.5 * mass * pow(opr.Ay_fn(par.Afn)(par, opr, i, j, k),2);
    }
    else{
        double *Ax = opr.dsval("Ax");
        double *Ay = opr.dsval("Ay");
        double *Az = opr.dsval("Az");
        int yDim = par.ival("yDim");
        int zDim = par.ival("zDim");
        int count = i*yDim*zDim + j*zDim + k; 
        return 0.5 * mass * ( V_x * V_x + V_y * V_y + V_z * V_z) + 
               0.5 * mass * pow(Ax[count],2) + 
               0.5 * mass * pow(Az[count],2) + 
               0.5 * mass * pow(Ay[count],2);
    }

}

// Function for simple 2d harmonic V with i and j as iterators, dimensionless
double harmonic_V_dimensionless(Grid &par, Op &opr, int i , int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double omegaX = par.dval("omegaX");
    double omegaY = par.dval("omegaY");
    double gammaY = par.dval("gammaY");
    double yOffset = 0.0;
    double xOffset = 0.0;
    double V_x = omegaX*(x[i]+xOffset); 
    double V_y = gammaY*omegaY*(y[j]+yOffset);
    return 0.5*( V_x * V_x + V_y * V_y) + 
           0.5 * pow(opr.Ax_fn(par.Afn)(par, opr, i, j, k),2) + 
           0.5 * pow(opr.Ay_fn(par.Afn)(par, opr, i, j, k),2);

}


// Function for simple 2d harmonic V with i and j as iterators, gauge
double harmonic_gauge_V(Grid &par, Op &opr, int i , int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double omega = par.dval("omega");
    double omegaX = par.dval("omegaX");
    double omegaY = par.dval("omegaY");
    double gammaY = par.dval("gammaY");
    double omega_0 = omega * omegaX;
    double omega_1 = omega * omegaY;
    double yOffset = 0.0;
    double xOffset = 0.0;
    double mass = par.dval("mass");
    double ox = omegaX - omega_0;
    double oy = omegaY - omega_1;
    double v1 = ox * (x[i]+xOffset) * ox * (x[i]+xOffset) 
                + gammaY*oy*(y[j]+yOffset) * gammaY*oy*(y[j]+yOffset);
    return 0.5 * mass * (v1 );
    //return 0.5*mass*( pow(omegaX*(x[i]+xOffset) - omega_0,2) + 
    //       pow(gammaY*omegaY*(y[j]+yOffset) - omega_1,2) );

}

// Functions for pAx, y, z for rotation along the z axis
// note that pAx and pAy call upon the Ax and Ay functions
double rotation_pAx(Grid &par, Op &opr, int i, int j, int k){
    double *xp = par.dsval("xp");
    if (par.Afn != "file"){
        return opr.Ax_fn(par.Afn)(par, opr, i, j, k) * xp[i];
    }
    else{
        double *Ax = opr.dsval("Ax");
        int yDim = par.ival("yDim");
        int count = 0;
        if (par.ival("dimnum") == 2){
            count = i*yDim + j; 
        }
        else if (par.ival("dimnum") == 3){
            int zDim = par.ival("zDim");
            count = k + j*zDim + i*yDim*zDim;
        }
        return Ax[count] * xp[i];
    }
}

double rotation_pAy(Grid &par, Op &opr, int i, int j, int k){
    double *yp = par.dsval("yp");
    if (par.Afn != "file"){
        return opr.Ay_fn(par.Afn)(par, opr, i, j, k) * yp[j];
    }
    else{
        double *Ay = opr.dsval("Ay");
        int yDim = par.ival("yDim");
        int count = 0;
        if (par.ival("dimnum") == 2){
            count = i*yDim + j; 
        }
        else if (par.ival("dimnum") == 3){
            int zDim = par.ival("zDim");
            count = k + j*zDim + i*yDim*zDim;
        }
        return Ay[count] * yp[j];
    }
}

double rotation_pAz(Grid &par, Op &opr, int i, int j, int k){
    double *zp = par.dsval("zp");
    if (par.Afn != "file"){
        return opr.Az_fn(par.Afn)(par, opr, i, j, k) * zp[k];
    }
    else{
        double *Az = opr.dsval("Az");
        int yDim = par.ival("yDim");
        int count = 0;
        if (par.ival("dimnum") == 2){
            count = i*yDim + j; 
        }
        else if (par.ival("dimnum") == 3){
            int zDim = par.ival("zDim");
            count = k + j*zDim + i*yDim*zDim;
        }
        return Az[count] * zp[k];
    }
}

double rotation_Ax(Grid &par, Op &opr, int i, int j, int k){
    double *y = par.dsval("y");
    double omega = par.dval("omega");
    double omegaX = par.dval("omegaX");
    return -y[j] * omega * omegaX;
}

double rotation_Az(Grid &par, Op &opr, int i, int j, int k){
    return 0;
}

double rotation_Ay(Grid &par, Op &opr, int i, int j, int k){
    double *x = par.dsval("x");
    double omega = par.dval("omega");
    double omegaY = par.dval("omegaY");
    return x[i] * omega * omegaY;
}

// Functions for a simple vortex ring
double ring_Ax(Grid &par, Op &opr, int i, int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");

    //first, we need to determine the radial distance and angle
    double angle = atan(x[i] / y[j]);
    if (y[j] == 0 && x[i] == 0){
        angle = 0;
    }
    if (y[j] < 0){
        angle += M_PI;
    }

    double rad = sqrt(1000000000*x[i]*x[i] + 1000000000*y[j]*y[j]) * cos(angle);

    if (rad < 0){
        rad = -1/(rad*100000);
    }
    else{
        rad = 1/(rad*100000);
    }

    if (rad > 0.001){
        rad = 0.001;
    }

    return rad;
}

double ring_Ay(Grid &par, Op &opr, int i, int j, int k){
    double *x = par.dsval("x");
    double *y = par.dsval("y");

    //first, we need to determine the radial distance and angle
    double angle = atan(x[i] / y[j]);
    if (y[j] == 0 && x[i] == 0){
        angle = 0;
    }
    if (y[j] < 0){
        angle += M_PI;
    }

    double rad = sqrt(1000000000*x[i]*x[i] + 1000000000*y[j]*y[j])*sin(angle);

    //multiplying by an arbitrary constant
    if (rad < 0){
        rad = -1/(rad*100000);
    }
    else{
        rad = 1/(rad*100000);
    }

    if (rad > 0.001){
        rad = 0.001;
    }

    return rad;
}

// Function to return 0, this is for constant gauge field tests.
double constant_A(Grid &par, Op &opr, int i, int j, int k){
    return 0;
}

// Fuinctions for Ax, y, z for rotation along the z axis
double test_Ax(Grid &par, Op &opr, int i, int j, int k){
    double *y = par.dsval("y");
    //double *x = par.dsval("x");
    double omega = par.dval("omega");
    double omegaX = par.dval("omegaX");
    double yMax = par.dval("yMax");
    //double val = -y[j]*y[j];
    double val = (sin(y[j] * 10000)+1) * omegaX * yMax * omega;
    return val;
}

double test_Ay(Grid &par, Op &opr, int i, int j, int k){
    //double *x = par.dsval("x");
    //double omega = par.dval("omega");
    //double omegaY = par.dval("omegaY");
    //double xMax = par.dval("xMax");
    //double val = x[i]*x[i];
    double val = 0;
    return val;
}

// Functions for fiber -- BETA
// Note: because of the fiber axis we are working with here, we will be using
//       E_z and E_r and ignoring E_phi
//       E_r -> E_x
//       E_z -> E_y

// This is a Function to return Az, because there is no A_r or A_phi
double fiber2d_Ax(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;
    val = HBAR ; // Plus everything else. How to implement detuning?

    return val;
}

double fiber2d_Ay(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;

    return val;
}

/*
// Functions to determine Electric field at a provided point
// Note that we need to multiply this by the dipole moment, (d)^2
double LP01_E_squared(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;

    double r = par.dsval("x")[i];

    std::unordered_map<std::string, double>
        matlab_map = read_matlab_data(14);

    double beta1 = matlab_map["beta1"];
    double q = matlab_map["q"];
    double h = matlab_map["h"];
    double a = matlab_map["a"];
    double n1 = matlab_map["n1"];
    double n2 = matlab_map["n2"];
    double spar = matlab_map["spar"];
    double N1 = (beta1*beta1/(4*h*h))
                *(pow((1-spar),2)*(pow(jn(0,h*a),2)+pow(jn(1,h*a),2))
                  +pow(1+spar,2)
                   *(pow(jn(2,h*a),2)-jn(1,h*a)*jn(3,h*a)))
                +((0.5)*(((pow(jn(1,h*a),2))-(jn(0,h*a)*jn(2,h*a)))));


    double N2=(0.5)*(jn(1,h*a)/pow(boost::math::cyl_bessel_k(1,q*a),2))
               *(((beta1*beta1/(2*q*q))
               *(pow(1-spar,2)*(pow(boost::math::cyl_bessel_k(1,q*a),2)
                                -pow(boost::math::cyl_bessel_k(0,q*a),2))
               -pow(1+spar,2)*(pow(boost::math::cyl_bessel_k(2,q*a),2)
               -boost::math::cyl_bessel_k(1,q*a)
               *boost::math::cyl_bessel_k(3,q*a))))
               -pow(boost::math::cyl_bessel_k(1,q*a),2) 
               +boost::math::cyl_bessel_k(0,q*a)
                *boost::math::cyl_bessel_k(2,q*a));
    

    double AA = (beta1 / (2 * q)) * 
                (jn(1,h*a)/boost::math::cyl_bessel_k(1,q*a))
                / (2 * M_PI * a * a * (n1*n1*N1 + n2*n2*N2));

    val = 2 * AA * AA *((1-spar)*(1-spar)
                        *pow(boost::math::cyl_bessel_k(0,q*r),2)
                        + (1+spar)*(1+spar)
                          *pow(boost::math::cyl_bessel_k(2,q*r),2)
                        + (2*q*q / (beta1*beta1))
                          *pow(boost::math::cyl_bessel_k(1,q*r),2));

    return val;
}
*/

// Now we need a function to read in the data from matlab
// Note that this has already been parsed into a bunch of different files
//     in data/data... This may need to be changed...
// Ideally, we would fix the parser so that it takes the fiber option into 
//     account and reads in the appropriate index. 
// For now (due to lack of dev time), we will simply read in ii = 14.
// BETA
std::unordered_map<std::string, double> read_matlab_data(int index){

    std::cout << "doing stuff" << '\n';

    // Note that we need a std::unordered_map for all the variables
    std::unordered_map<std::string, double> matlab_variables;

    // Now we need to read in the file 
    std::string filename = "data/data" + std::to_string(index) + ".dat";
    std::ifstream fileID(filename);
    std::string item1, item2;

    std::string line;
    while (fileID >> line){
        item1 = line.substr(0,line.find(","));
        item2 = line.substr(line.find(",")+1,line.size());
        matlab_variables[item1] = std::stod(item2);
    }

    return matlab_variables;
}


// Functions for dynamic fields read in by string
double dynamic_Ax(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;
    std::string equation = par.sval("Axstring");
    parse_equation(par, equation, val, i, j, k);
    // For debugging
    // exit(0);
    return val;
}

double dynamic_Ay(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;
    std::string equation = par.sval("Aystring");
    parse_equation(par, equation, val, i, j, k);
    // For debugging
    //exit(0);
    return val;
}

double dynamic_Az(Grid &par, Op &opr, int i, int j, int k){
    double val = 0;
    std::string equation = par.sval("Aystring");
    parse_equation(par, equation, val, i, j, k);
    return val;
}

// Function to read Ax from file.
// Note that this comes with a special method in init...
void file_A(std::string filename, double *A){
    std::fstream infile(filename, std::ios_base::in);

    double inval;
    int count = 0;
    while (infile >> inval){
        A[count] = inval;
        count++;
    }
}

// Function to check whether a file exists
std::string filecheck(std::string filename){

    struct stat buffer = {0};
    if (stat(filename.c_str(), &buffer) == -1){
        std::cout << "File " << filename << " does not exist!" << '\n';
        std::cout << "Please select a new file:" << '\n'; 
        std::cin >> filename; 
        filename = filecheck(filename);
    } 

    return filename;
}

// This function will be used with the dynamic gauge fields for AX,y,z (above)
// BETA, mfunctions_map does not seem to work!
void parse_equation(Grid par, std::string &equation, double &val, 
                    int i, int j, int k){

    // boolean value iff first minus
    bool minus = false;

    //std::cout << equation << '\n';

    // Because this will be called recursively, we need to return if the string
    // length is 0
    if (equation.length() == 0){
        std::cout << "There's nothing here!" << '\n';
        //return;
    }

    // vector of all possibe mathematical operators (not including functions)
    std::vector<std::string> moperators(4);
    moperators = {
        "-", "/", "*", "+"
    };

    // And another vector for brackets of various types which indicate recursive
    // parsing of the equation
    std::vector<std::string> mbrackets;
    mbrackets = {
        "(", "[", "]", ")"
    };

    // vector of all possible mathematical functions... more to come
    std::vector<std::string> mfunctions(5);
    mfunctions = {
        "sin", "cos", "exp", "tan", "erf", "sqrt", "sign"
    };

    // We also need a specific map for the functions above
    typedef double (*functionPtr)(double);
    std::unordered_map<std::string, functionPtr> mfunctions_map;
    mfunctions_map["sin"] = sin;
    mfunctions_map["cos"] = cos;
    mfunctions_map["tan"] = tan;
    mfunctions_map["exp"] = exp;
    mfunctions_map["erf"] = erf;
    mfunctions_map["sqrt"] = sqrt;
    mfunctions_map["sign"] = sign;
    

    // Check for parentheses
    for (auto &mbra : mbrackets){
        //std::cout << equation.substr(0,1) << '\n';
        if (equation.substr(0,1) == mbra){
            if (mbra == ")" || mbra == "]"){
                //std::cout << "could not find matching " << mbra << "!\n";
                exit(0);
            }
            else if (mbra == "("){
                int brapos = equation.find(")");
                std::string new_eqn = equation.substr(1,brapos-1);
                parse_equation(par, new_eqn, val, i, j, k);
                equation = equation.substr(brapos+1, equation.size());
            }
            else if (mbra == "["){
                int brapos = equation.find("]");
                std::string new_eqn = equation.substr(1,brapos-1);
                parse_equation(par, new_eqn, val, i, j, k);
                equation = equation.substr(brapos, equation.size());
            }
        }
    }


    // We will have values and operators, but some operators will need to 
    // recursively call this function (think exp(), sin(), cos())...
    // We now need to do some silly sorting to figure out which operator 
    // comes first and where it is
    size_t index = equation.length();
    std::string currmop = "";
    size_t moppos;
    for (auto &mop : moperators){
        moppos = equation.find(mop);
        if (moppos < equation.length()){
            if (moppos < index){ // && moppos > 0){
                currmop = mop;
                index = moppos;
            }
/*
            else if(moppos == 0 && mop == "-"){
                minus = true;
                equation = equation.substr(1,equation.size());
            }
*/
        }
    }

    //std::cout << currmop << '\t' << index << '\n';

    // Now we do a similar thing for the mbrackets
    // Sharing moppos from above
    for (auto &mbra : mbrackets){
        moppos = equation.find(mbra);
        if (moppos < equation.length()){
            if (moppos < index){
                currmop = mbra;
                index = moppos;
            }
        }
    }

    // Now we need to get the string we are working with...
    std::string item = equation.substr(0,index);

    // now we need to find the string in either mfunctions or par
    // First, we'll check mfunctions

    // Now we need to check to see if the string is in mfunctions
    auto it = mfunctions_map.find(item);
    if (it != mfunctions_map.end()){
        int openbracket, closebracket;
        openbracket = index;
        closebracket = equation.find(equation[openbracket]);
        std::string ineqn = equation.substr(openbracket + 1, 
                                            closebracket - 1);
        double inval = 1;
        parse_equation(par, ineqn, inval, i, j, k);
        val = mfunctions_map[item](inval);

        // now we need to parse the rest of the string...
        ineqn = equation.substr(closebracket, equation.size());
        parse_equation(par, ineqn, val, i, j, k);
    }

    // Now we need to do a similar thing for all the maps in par.
    else if (par.is_double(item)){
        val = par.dval(item);
    }
    else if (par.is_dstar(item)){
        if (item == "x" || item == "px"){
            val = par.dsval(item)[i];
        }
        if (item == "y" || item == "py"){
            val = par.dsval(item)[j];
        }
        if (item == "z" || item == "pz"){
            val = par.dsval(item)[k];
        }
    }
    else if (item.find_first_not_of("0123456789.") > item.size() &&
             item.size() > 0){
        //std::cout << item << '\n';
        val = std::stod(item);
    }
    else if (item.size() > 0){
        std::cout << "could not find string " << item << "! please use one of "
                  << "the following variables:" << '\n';
        par.print_map();
    }

    if (minus){
        val *= -1;
    }

    //std::cout << item << '\t' << currmop << '\n';

    // Now to deal with the operator at the end
    if (currmop == "+"){
        double inval = 1;
        std::string new_eqn = equation.substr(index+1,equation.size());
        //std::cout << new_eqn << '\n';
        parse_equation(par, new_eqn, inval, i, j, k);
        val += inval;
    }
    if (currmop == "-"){
        double inval = 1;
        std::string new_eqn = equation.substr(index+1,equation.size());
        //std::cout << new_eqn << '\n';
        parse_equation(par, new_eqn, inval, i, j, k);
        val -= inval;
    }
    if (currmop == "*"){
        double inval = 1;
        std::string new_eqn = equation.substr(index+1,equation.size());
        //std::cout << new_eqn << '\n';
        parse_equation(par, new_eqn, inval, i, j, k);
        val *= inval;
    }
    if (currmop == "/"){
        double inval = 1;
        std::string new_eqn = equation.substr(index+1,equation.size());
        //std::cout << new_eqn << '\n';
        parse_equation(par, new_eqn, inval, i, j, k);
        val /= inval;
    }
}

// Function to compute the wfc for the standard 2d case
cufftDoubleComplex standard_wfc_2d(Grid &par, double Phi,
                                   int i, int j, int k){
    cufftDoubleComplex wfc;
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double Rxy = par.dval("Rxy");
    double a0y = par.dval("a0y");
    double a0x = par.dval("a0x");
    wfc.x = exp(-( pow((x[i])/(Rxy*a0x),2) + 
                   pow((y[j])/(Rxy*a0y),2) ) ) * cos(Phi);
    wfc.y = -exp(-( pow((x[i])/(Rxy*a0x),2) + 
                    pow((y[j])/(Rxy*a0y),2) ) ) * sin(Phi);

    return wfc;
}

// Function to compute the initial wavefunction for the standard 3d caase
cufftDoubleComplex standard_wfc_3d(Grid &par, double Phi,
                                   int i, int j, int k){
    cufftDoubleComplex wfc;
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double *z = par.dsval("z");
    double Rxy = par.dval("Rxy");
    double a0y = par.dval("a0y");
    double a0x = par.dval("a0x");
    double a0z = par.dval("a0z");
    wfc.x = exp(-( pow((x[i])/(Rxy*a0x),2) + 
                   pow((y[j])/(Rxy*a0y),2) +
                   pow((z[k])/(Rxy*a0z),2))) *
                   cos(Phi);
    wfc.y = -exp(-( pow((x[i])/(Rxy*a0x),2) + 
                    pow((y[j])/(Rxy*a0y),2) +
                    pow((z[k])/(Rxy*a0z),2))) *
                    sin(Phi);

    return wfc;
}

// Function to initialize a toroidal wfc
// note that we will need to specify the size of everything based on fiber
// size and such.
cufftDoubleComplex torus_wfc(Grid &par, double Phi,
                             int i, int j, int k){

    cufftDoubleComplex wfc;

    // Let's read in all the necessary parameters
    double *x = par.dsval("x");
    double *y = par.dsval("y");
    double *z = par.dsval("z");
    double xOffset = par.dval("x0_shift");
    double yOffset = par.dval("y0_shift");
    double fudge = par.dval("fudge");

    double xMax = par.dval("xMax");
    double yMax = par.dval("yMax");

    double Rxy = par.dval("Rxy");

    double a0x = par.dval("a0x");
    //double a0y = par.dval("a0y");
    double a0z = par.dval("a0z");

    double rMax = sqrt(xMax*xMax + yMax*yMax);

    // We will now create a 2d projection and extend it in a torus shape
    double rad = sqrt((x[i] - xOffset) * (x[i] - xOffset) 
                      + (y[j] - yOffset) * (y[j] - yOffset)) - 0.5*rMax*fudge;
    //double a0r = sqrt(a0x*a0x + a0y*a0y);

    wfc.x = exp(-( pow((rad)/(Rxy*a0x*0.5),2) + 
                   pow((z[k])/(Rxy*a0z*0.5),2) ) );
    wfc.y = -exp(-( pow((rad)/(Rxy*a0x*0.5),2) + 
                    pow((z[k])/(Rxy*a0z*0.5),2) ) );

    return wfc;

}
