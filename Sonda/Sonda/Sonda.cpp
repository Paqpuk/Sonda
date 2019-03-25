#include "pch.h"
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>
#include <cmath>
#include <random>
#include <filesystem>
#include <ctime>
#include <iomanip>
#define N0 10000
#define Pi 3.14
#define e_mass 9.11e-31
#define e_charge 1.60217662e-19
using namespace std;

struct particle {
	double X;
	double Y;
	double Z;
	double Vx;
	double Vy;
	double Vz;
	int sonda;
};

struct medium {
	double Xmin;
	double Xmax;
	double Ymin;
	double Ymax;
	double Zmin;
	double Zmax;
};

particle e[N0];
medium space;

int Vupdate(double E, double B[3], double dt, int i);
double* CrossProduct(double v1[], double v2[]);
int sign();
string sim_time_out(double dt, double s);


int main()
{




	double MField[3];
		MField[0] = 0;
		MField[2] = 300e-4;
		MField[1] = 0;
	double EField = 0;
		space.Xmax = 10e-3;
		space.Xmin = 5e-3;
		space.Ymax = 10e-3;
		space.Ymin = 5e-3;
		space.Zmax = 1e-3;
		space.Zmin = 0.2e-3;

		double Temp = 1;
		double Kt = 1.38e-23;
		double f0 = 0.01;
		double dt = 1e-12;
		double totalTime = 200e-9;
		double Time_steps = totalTime / dt;
		int output_Rate = 1000;


		random_device rd{};
		mt19937 gen{ rd() }; 
		normal_distribution<double> distr(0.5, 0.5);
		double random = distr(gen);


		

		double Vmax = sqrt((2 * Pi*Kt*Temp / e_mass)*log(f0*sqrt(2 * Pi*Kt*Temp / e_mass) * 2 * Pi*Kt*Temp / e_mass));


	for (int i = 0; i < N0; i++)
		{
				random = distr(gen);
				while (random > 1 || random < 0)
				{
					random = distr(gen);
				}
			e[i].Vx = sign() * Vmax * random;
				random = distr(gen);
				while (random > 1 || random < 0)
				{
					random = distr(gen);
				}
			e[i].Vy = sign() * Vmax * random;
				random = distr(gen);
				while (random > 1 || random < 0)
				{
				random = distr(gen);
				}
			e[i].Vz = - Vmax * random;
			e[i].sonda = 0;
		}
	
	for (int i = 0; i < N0; i++)
		{
				random = distr(gen);
				while (random > 1 || random < 0)
				{
					random = distr(gen);
				}
			e[i].X = space.Xmax * random;
				random = distr(gen);
				while (random > 1 || random < 0)
				{
					random = distr(gen);
				}
			e[i].Y = space.Ymax * random;
			e[i].Z = space.Zmax;
		}
	

	ofstream output;
	string outputFileName = "step_";
	
	time_t t = time(0);
	tm now;
	localtime_s(&now, &t);
	string date_time = to_string(now.tm_mday) + "." + to_string(now.tm_mon+1) + "." + to_string(now.tm_year+1900) + "_";
	date_time += to_string(now.tm_hour) + ";" + to_string(now.tm_min) + ";" + to_string(now.tm_sec);

	std::experimental::filesystem::create_directory(date_time);

	int sonda_counter = 0;
	

	for (double s = 0; s < Time_steps; s++)
	{
		

		if (int(s)%output_Rate == 0)
			{
				
				
				
				cout << s << " / " << Time_steps << endl;
				outputFileName = date_time + "/step" + to_string(int(s)) + "_" + sim_time_out(dt, s) + ".txt";
				output.open(outputFileName);
				output << setprecision(10);
				output << "N			X			Y			Z" << endl;
			}

		for (int i = 0; i < N0; i++)
		{
			if (int(s)%output_Rate == 0)
				{
				
					output << left;
					output << setw(6)  << i << "| " << setw(25) << setprecision(20) << e[i].X << " | " << setw(25) << setprecision(20) << e[i].Y;
					output << " | " << setw(25) << setprecision(20) << e[i].Z << " |" << endl;
				}

			Vupdate(EField, MField, dt, i);
			e[i].X += e[i].Vx*dt;
			e[i].Y += e[i].Vy*dt;
			e[i].Z += e[i].Vz*dt;
			if ((e[i].X <= space.Xmin && e[i].Y <= space.Ymin) && e[i].Z < space.Zmin)
			{
				e[i].sonda = 1;
			}
		}

		if (int(s)%output_Rate == 0)
			output.close();
	}
	outputFileName = date_time + "/result.txt";
	output.open(outputFileName);
	for (int i = 0; i < N0; i++)
		{
			sonda_counter +=e[i].sonda;
		}
		
	output << sonda_counter << endl;
}

double* CrossProduct(double v1[], double v2[])
{
	double r[3];
	r[0] = v1[1] * v2[2] - v1[2] * v2[1];
	r[1] = -v1[0] * v2[2] + v1[2] * v2[0];
	r[2] = v1[0] * v2[1] - v1[1] * v2[0];
	return r;
}

int Vupdate(double E, double B[3], double dt, int i)
{
	double v_minus[3];
	double v_plus[3];
	double v_prime[3];
	double t[3];
	double s[3];
	double t_mag2;
	double electronV[3];
		electronV[0]=e[i].Vx;
		electronV[1]=e[i].Vy;
		electronV[2]=e[i].Vz;
	int dim; /*dimension*/
	
	for (dim = 0; dim < 3; dim++)
		t[dim] = e_charge / e_mass * B[dim]*0.5*dt;

	t_mag2 = t[0] * t[0] + t[1] * t[1] + t[2] * t[2];

	for (dim = 0; dim < 3; dim++)
	{
		s[dim] = 2 * t[dim] / (1 + t_mag2);
		v_minus[dim] = electronV[dim] + e_charge / e_mass * E*0.5*dt;
	}

	double *v_prime_cross_t = CrossProduct(v_minus, t);
	for (dim = 0; dim < 3; dim++)
		v_prime[dim] = v_minus[dim] + v_prime_cross_t[dim];

	double *v_prime_cross_s = CrossProduct(v_prime, s);
	for (dim = 0; dim < 3; dim++)
		v_plus[dim] = v_minus[dim] + v_prime_cross_s[dim];

	for (dim = 0; dim < 3; dim++)
	{
		electronV[dim]=v_plus[dim]+ e_charge / e_mass * E*0.5*dt;
	}

	e[i].Vx = electronV[0];
	e[i].Vy = electronV[1];
	e[i].Vz = electronV[2];
	return 0;
}

int sign()
{
	return 1 - 2*(rand() & 1);
}

string sim_time_out(double dt, double s)
{
	double a = dt * s;
	if (a >= 1)					return to_string(int(a)) + "s";
	if (a < 1 && a >= 1e-3)		return (to_string(int(a*1e+3))) + "ms";
	if (a < 1e-3 && a >= 1e-6)	return (to_string(int(a*1e+6))) + "\03B7" + "s";
	if (a < 1e-6 && a >= 1e-9)	return (to_string(int(a*1e+9))) + "ns";
	if (a < 1e-9 && a >= 1e-12) return (to_string(int(a*1e+12))) + "ps";
	if (a == 0) return "0s";
}
 