﻿#include "pch.h"
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
#include <time.h>
#include <iomanip>
#include <vector>
#define N0 1000
#define Pi 3.14159265359
#define e_mass 9.10938356e-31
#define e_charge 1.60217662e-19
#define stop_condition 5
using namespace std;

struct particle {
	double X;
	double Y;
	double Z;
	double Vx;
	double Vy;
	double Vz;
	bool sonda;
};

struct medium {
	double Xson_r;
	double Xmax;
	double Yson_r;
	double Ymax;
	double Zson_l;
	double Zmax;
	double XYson_center;
};

vector<particle> e;
medium space;

int Vupdate(double E, double B[3], double dt, int i);
double* CrossProduct(double v1[], double v2[]);
double sqr(double a);
int sign();
string sim_time_out(double dt, double s);
bool circle_check(double x, double y, double center, double r);


int main()
{
	double MField[3];
	MField[0] = 0;
	MField[1] = 0;
	MField[2] = 300e-4;
	double EField = 0;
	space.Xmax = 2e-3;
	space.Xson_r = 0.2e-3;
	space.Ymax = 2e-3;
	space.Yson_r = 0.2e-3;
	space.Zmax = 10e-3;
	space.Zson_l = 5e-3;
	space.XYson_center = 0;

	double Temp = 500;
	double Kb = 1.38e-23;
	double ePeriod = (2 * Pi * e_mass) / (e_charge*MField[2]);
	double dt = ePeriod/100;

	vector<double> cell_r; 
	double initial_cell_radius = space.Xson_r /10;
	cell_r.push_back(initial_cell_radius);

	random_device rd{};
	mt19937 genM{ rd() };
	normal_distribution<double> distrM(0.9639, 0.4220);
	double random = distrM(genM);

	default_random_engine genUni;
	uniform_real_distribution<double> distrUni(0.0, 1.0);

	

	double Vp = sqrt(2 * Kb*Temp / e_mass);

	cout << "vmax = " << Vp << "\n";
	cout << "dt = " << ePeriod/100 << "\n";

	int sim_condition = 0;
	int j = 1;
	int total_cells = 1;
	int electrons_recieved = 0;
	int d_electrons_recieved = 0;
	double theta = 0;

	clock_t exec_t;

	time_t t = time(0);
	tm now;
	localtime_s(&now, &t);
	string date_time = to_string(now.tm_mday) + "." + to_string(now.tm_mon + 1) + "." + to_string(now.tm_year + 1900) + "_";
	date_time += to_string(now.tm_hour) + ";" + to_string(now.tm_min) + ";" + to_string(now.tm_sec);
	string Mfieldname;
	double radius;



	
	for (int b = 0; b < 11; b++)
	{
		j = 1;
		sim_condition = 0;
		total_cells = 1;
		electrons_recieved = 0;
		d_electrons_recieved = 0;
		cell_r[0] = initial_cell_radius;

		MField[2] = 100e-4*b;
		Mfieldname = date_time + "_" + to_string(MField[2] * 10000) + "G";
		std::experimental::filesystem::create_directory(Mfieldname);

		string out_filename = Mfieldname + "/particles.txt";
		ofstream output;
		output.open(out_filename);


		exec_t = clock();
		for (int i = 0; i < N0; i++)
		{
				e.push_back(particle());
			theta = 2 * Pi * distrUni(genUni);
			radius = distrUni(genUni) * cell_r[0];
			e[i].X = radius * cos(theta);
			e[i].Y = radius * sin(theta);
			e[i].Z = space.Zmax;

			do
			{
				random = distrM(genM);
			} while (random > 2.5 || random < 0);
			e[i].Vx = sign() * Vp * random;
			random = distrM(genM);
			do
			{
				random = distrM(genM);
			} while (random > 2.5 || random < 0);
			e[i].Vy = sign() * Vp * random;
			random = distrM(genM);
			do
			{
				random = distrM(genM);
			} while (random > 2.5 || random < 0);
			e[i].Vz = -Vp * random;
			e[i].sonda = 0;
		}
		for (int i = 0; i < N0; i++)
		{

			while (e[i].Z > 0.0 && e[i].sonda == 0)
			{
				Vupdate(EField, MField, dt, i);
				e[i].X += e[i].Vx*dt;
				e[i].Y += e[i].Vy*dt;
				e[i].Z += e[i].Vz*dt;
				if (e[i].Z < space.Zson_l &&
					circle_check(e[i].X, e[i].Y, space.XYson_center, space.Xson_r))
				{
					e[i].sonda = 1;

				}
			}
			electrons_recieved += e[i].sonda;
		}
		exec_t = clock() - exec_t;
		cout << "First cell finished: " << electrons_recieved << " electrons reached the probe. Simulation took: " << float(exec_t) / CLOCKS_PER_SEC << " seconds." << endl;
		output << "N      r" << endl;
		output << electrons_recieved << " " << cell_r[0] / 2 << endl;

		while (sim_condition != stop_condition)
		{
			exec_t = clock();
			if (total_cells == 1)
			{
				cell_r.push_back(0);
				cell_r[j] = sqrt(2)*cell_r[0];
			}
			else
			{
				cell_r.push_back(0);
				cell_r[j] = sqrt(2 * sqr(cell_r[j - 1]) - sqr(cell_r[j - 2]));
			}
			cout << "cell " << total_cells << " r " << cell_r[j] << endl;
			d_electrons_recieved = electrons_recieved;

			for (int i = total_cells * N0; i < (total_cells + 1) * N0; i++)
			{

					e.push_back(particle());

				theta = 2 * Pi * distrUni(genUni);
				radius = (distrUni(genUni) * (cell_r[j] - cell_r[j - 1]) + cell_r[j - 1]);
				e[i].X = radius * cos(theta);
				e[i].Y = radius * sin(theta);
				e[i].Z = space.Zmax;

				do
				{
					random = distrM(genM);
				} while (random > 2.5 || random < 0);
				e[i].Vx = sign() * Vp * random;
				random = distrM(genM);
				do
				{
					random = distrM(genM);
				} while (random > 2.5 || random < 0);
				e[i].Vy = sign() * Vp * random;
				random = distrM(genM);
				do
				{
					random = distrM(genM);
				} while (random > 2.5 || random < 0);
				e[i].Vz = -Vp * random;
				e[i].sonda = 0;
			}

			for (int i = total_cells * N0; i < (total_cells + 1) * N0; i++)
			{
				while (e[i].Z > 0 && e[i].sonda == 0)
				{
					Vupdate(EField, MField, dt, i);
					e[i].X += e[i].Vx*dt;
					e[i].Y += e[i].Vy*dt;
					e[i].Z += e[i].Vz*dt;
					if (e[i].Z < space.Zson_l && circle_check(e[i].X, e[i].Y, space.XYson_center, space.Xson_r))
					{
						e[i].sonda = 1;
					}
				}
				electrons_recieved += e[i].sonda;
			}

			if (d_electrons_recieved == electrons_recieved)
				sim_condition++;

			total_cells++;

			exec_t = clock() - exec_t;
			cout << total_cells << " cell finished: " << electrons_recieved - d_electrons_recieved << " electrons reached the probe. Simulation took: " << float(exec_t) / CLOCKS_PER_SEC << " seconds." << endl;
			output << electrons_recieved - d_electrons_recieved << " " << cell_r[j] / 2 << endl;
			j++;


		}

		output.close();

	}

	


	/*ofstream output, gnuplot, gnuplot_trajectory;
	string outputFileName, gnuplotFileName, plotFolder, trajectoryFileName;

	time_t t = time(0);
	tm now;
	localtime_s(&now, &t);
	string date_time = to_string(now.tm_mday) + "." + to_string(now.tm_mon + 1) + "." + to_string(now.tm_year + 1900) + "_";
	date_time += to_string(now.tm_hour) + ";" + to_string(now.tm_min) + ";" + to_string(now.tm_sec);

	std::experimental::filesystem::create_directory(date_time);
	plotFolder = date_time + "/plot_data";
	std::experimental::filesystem::create_directory(plotFolder);

	int sonda_counter = 0;

	time_t first_step_time;
	time_t second_step_time;
	int ETA;

	trajectoryFileName = plotFolder + "/Atrajectory.txt";
	gnuplot_trajectory.open(trajectoryFileName);

	for (double s = 0; s < Time_steps; s++)
	{
		if (s == 0)
		{
			time(&first_step_time);
		}

		if (s == 1)
		{
			time(&second_step_time);
			ETA = difftime(second_step_time, first_step_time);
			cout << "ETA of " << Time_steps << " time steps is: " << ETA * Time_steps << " seconds\n";
		}

		if (int(s) % output_Rate == 0)
		{
			cout << s / output_Rate << " / " << Time_steps / output_Rate << endl;
			outputFileName = date_time + "/step" + to_string(int(s)) + "_" + sim_time_out(dt, s) + ".txt";
			gnuplotFileName = plotFolder + "/plot_" + to_string(int(s)) + ".txt";
			output.open(outputFileName);
			gnuplot.open(gnuplotFileName);
			output << setprecision(10);
			output << "N			X			Y			Z" << endl;
			gnuplot_trajectory << e[0].X << " " << e[0].Y << " " << e[0].Z << endl;
		}
		
		for (int i = 0; i < N0; i++)
		{
			if (int(s) % output_Rate == 0)
			{
				output << left;
				output << setw(6) << i << "| " << setw(25) << setprecision(20) << e[i].X << " | " << setw(25) << setprecision(20) << e[i].Y;
				output << " | " << setw(25) << setprecision(20) << e[i].Z << " |" << endl;
				gnuplot << setprecision(30) << e[i].X << " " << e[i].Y << " " << e[i].Z << endl;
			}
			*/



		
/*
		if (int(s) % output_Rate == 0)
			output.close();
		gnuplot.close();
	}
	gnuplot_trajectory.close();

	outputFileName = date_time + "/Aresult.txt";
	output.open(outputFileName);
	for (int i = 0; i < N0; i++)
	{
		sonda_counter += e[i].sonda;
	}
	output << sonda_counter << endl;
	output.close();*/
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
	electronV[0] = e[i].Vx;
	electronV[1] = e[i].Vy;
	electronV[2] = e[i].Vz;
	int dim; /*dimension*/

	for (dim = 0; dim < 3; dim++)
		t[dim] = e_charge / e_mass * B[dim] * 0.5*dt;

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
		electronV[dim] = v_plus[dim] + e_charge / e_mass * E*0.5*dt;
	}

	e[i].Vx = electronV[0];
	e[i].Vy = electronV[1];
	e[i].Vz = electronV[2];
	return 0;
}

int sign()
{
	return 1 - 2 * (rand() & 1);
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

double sqr(double a)
{
	return a * a;
}

bool circle_check(double x, double y, double center, double r)
{
	bool a = sqrt((x - center)*(x - center) + (y - center)*(y - center)) < (r);
	return a;
}
