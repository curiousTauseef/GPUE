/*
* parser.cc - GPUE: Split Operator based GPU solver for Nonlinear 
Schrodinger Equation, Copyright (C) 2011-2016, James Ryan Schloss
<jrs.schloss@gmail.com>, Tadhg Morgan, Neil Crowley. All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are 
met:

1. Redistributions of source code must retain the above copyright 
notice, this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright 
notice, this list of conditions and the following disclaimer in the 
documentation and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its 
contributors may be used to endorse or promote products derived from 
this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "../include/parser.h"
#include "../include/unit_test.h"

// note: read variables into appendData directly.

Grid parseArgs(int argc, char** argv){

    // Creates initial grid class for the parameters
    Grid par;
    int opt;

    // Setting default values
    par.store("xDim", 256);
    par.store("yDim", 256);
    par.store("zDim", 256);
    par.store("omega", 0.0);
    par.store("gammaY", 1.0);
    par.store("gsteps", 1e4);
    par.store("esteps", 1000.0);
    par.store("gdt", 1e-4);
    par.store("dt", 1e-4);
    par.store("device", 0);
    par.store("atoms", 1);
    par.store("read_wfc", false);
    par.store("printSteps" ,100);
    par.store("winding", 0.0);
    par.store("corotating", false);
    par.store("gpe", false);
    par.store("omegaZ", 0.0);
    par.store("int_scaling",0.0);
    par.store("laser_power",0.0);
    par.store("angle_sweep",0.0);
    par.store("kick_it", 0);
    par.store("write_it", false);
    par.store("x0_shift",0.0);
    par.store("y0_shift",0.0);
    par.store("sepMinEpsilon",0.0);
    par.store("graph", false);
    par.store("unit_test",false);
    par.store("omegaX", 6.283);
    par.store("omegaY", 6.283);
    par.store("data_dir", "");

    optind = 1;

    while ((opt = getopt (argc, argv, 
          "d:D:x:y:w:G:g:e:T:t:n:p:ro:L:lsi:P:X:Y:O:k:WU:V:S:ahz:H:u")) !=-1)
    {
        switch (opt)
        {
            case 'x':
            {
                int xDim = atoi(optarg);
                printf("Argument for x is given as %d\n",xDim);
                par.store("xDim",(int)xDim);
                break;
            }
            case 'y':
            {
                int yDim = atoi(optarg);
                printf("Argument for y is given as %d\n",yDim);
                par.store("yDim",(int)yDim);
                break;
            }
            case 'z':
            {
                int zDim = atoi(optarg);
                printf("Argument for z is given as %d\n",zDim);
                par.store("zDim",(int)zDim);
                break;
            }
            case 'w':
            {
                double omega = atof(optarg);
                printf("Argument for OmegaRotate is given as %E\n",omega);
                par.store("omega",(double)omega);
                break;
            }
            case 'G':
            {
                double gammaY = atof(optarg);
                printf("Argument for gamma is given as %E\n",gammaY);
                par.store("gammaY",(double)gammaY);
                break;
            }
            case 'g':
            {
                double gsteps = atof(optarg);
                printf("Argument for Groundsteps is given as %E\n",gsteps);
                par.store("gsteps",(int)gsteps);
                break;
            }
            case 'e':
            {
                double esteps = atof(optarg);
                printf("Argument for EvSteps is given as %E\n",esteps);
                par.store("esteps",(int)esteps);
                break;
            }
            case 'T':
            {
                double gdt = atof(optarg);
                printf("Argument for groundstate Timestep is given as %E\n",
                       gdt);
                par.store("gdt",(double)gdt);
                break;
            }
            case 't':
            {
                double dt = atof(optarg);
                printf("Argument for Timestep is given as %E\n",dt);
                par.store("dt",(double)dt);
                break;
            }
            case 'd':
            {
                int device = atoi(optarg);
                printf("Argument for device is given as %d\n",device);
                par.store("device",(int)device);
                break;
            }
            case 'n':
            {
                double atoms = atof(optarg);
                printf("Argument for atoms is given as %E\n",atoms);
                par.store("atoms",(int)atoms);
                break;
            }
            case 'r':
            {
                printf("Reading wavefunction from file\n");
                par.store("read_wfc",true);
                break;
            }
            case 'p':
            {
                int print = atoi(optarg);
                printf("Argument for Printout is given as %d\n",print);
                par.store("printSteps",(int)print);
                break;
            }
            case 'L':
            {
                double l = atof(optarg);
                printf("Vortex winding is given as : %E\n",l);
                par.store("winding",(double)l);
                break;
            }
            case 'l':
            {
                printf("Angular momentum mode engaged\n");
                par.store("corotating",true);
                break;
            }
            case 's':
            {
                printf("Non-linear mode engaged\n");
                par.store("gpe",true);
                break;
            }
            case 'o':
            {
                double omegaZ = atof(optarg);
                printf("Argument for OmegaZ is given as %E\n",omegaZ);
                par.store("omegaZ",(double)omegaZ);
                break;
            }
            case 'h':
            {
                std::string command = "src/print_help.sh ";
                system(command.c_str());
                exit(0);
                break;
            }
            case 'H':
            {
                std::string command = "src/print_help.sh ";
                command.append(optarg);
                system(command.c_str());
                exit(0);
                break;
            }
            case 'i':
            {
                double interaction = atof(optarg);
                printf("Argument for interaction scaling is %E\n",interaction);
                par.store("int_scaling",interaction);
                break;
            }
            case 'P':
            {
                double laser_power = atof(optarg);
                printf("Argument for laser power is %E\n",laser_power);
                par.store("laser_power",laser_power);
                break;
            }
            case 'X':
            {
                double omegaX = atof(optarg);
                printf("Argument for omegaX is %E\n",omegaX);
                par.store("omegaX",(double)omegaX);
                break;
            }
            case 'Y':
            {
                double omegaY = atof(optarg);
                printf("Argument for omegaY is %E\n",omegaY);
                par.store("omegaY",omegaY);
                break;
            }
            case 'O':
            {
                double angle_sweep = atof(optarg);
                printf("Argument for angle_sweep is %E\n",angle_sweep);
                par.store("angle_sweep",angle_sweep);
                break;
            }
            case 'k':
            {
                int kick_it = atoi(optarg);
                printf("Argument for kick_it is %i\n",kick_it);
                par.store("kick_it",kick_it);
                break;
            }
            case 'W':
            {
                printf("Writing out\n");
                par.store("write_it",true);
                break;
            }
            case 'D':
            {
                std::string data_dir = optarg;
                std::cout << "Data directory is: " << data_dir << '\n';
                par.store("data_dir", data_dir + "/");
                break;
            }
            case 'U':
            {
                double x0_shift = atof(optarg);
                printf("Argument for x0_shift is %lf\n",x0_shift);
                par.store("x0_shift",x0_shift);
                break;
            }
            case 'u':
            {
                std::cout << "performing all unit tests" << '\n';
                par.store("unit_test", true);
                test_all();
                exit(0);
            }
            case 'V':
            {
                double y0_shift = atof(optarg);
                printf("Argument for y0_shift is %lf\n",y0_shift);
                par.store("y0_shift",y0_shift);
                break;
            }
            case 'S':
            {
                double sepMinEpsilon = atof(optarg);
                printf("Argument for sepMinEpsilon is %lf\n",sepMinEpsilon);
                par.store("sepMinEpsilon",sepMinEpsilon);
                break;
            }
            case 'a':
            {
                printf("Graphing mode engaged\n");
                par.store("graph",true);
                break;
            }
            case '?':
            {
                if (optopt == 'c') {
                    fprintf (stderr, 
                             "Option -%c requires an argument.\n", optopt);
                } 
                else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                } 
                else {
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",optopt);
                }
                return par;
            default:
                abort ();
            }
        }
    }

    return par;
}