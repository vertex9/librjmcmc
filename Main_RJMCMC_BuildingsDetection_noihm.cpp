#include <iostream>
#include <string>
#include <strstream>

#include "ImageGradientEnergyPolicy.hpp"

#include "BuildingsDetectorParameters.hpp"
#include "RectangleNode.hpp"
#include "CircleNode.hpp"
#include "RJMCMC_Detector.hpp"
#include "RJMCMC_Sampler.hpp"

#include "cgal_types.h"

typedef RJMCMC_Detector<RectangleNode, RectanglePriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;
//typedef RJMCMC_Detector<CircleNode, CirclePriorEnergyPolicy, ImageGradientEnergyPolicy >	BuildingsDetector;

int main (int argc, char **argv)
{	
	if (!BuildingsDetectorParametersSingleton::Instance()->ParseCmdLine(argc, argv))
		return -1;
	
	BBox::PointType size , origin;
	size[0] = BuildingsDetectorParametersSingleton::Instance()->RunningWidth()-1;
	size[1] = BuildingsDetectorParametersSingleton::Instance()->RunningHeight()-1;
	origin[0] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginX();
	origin[1] = BuildingsDetectorParametersSingleton::Instance()->RunningOriginY();
	BuildingsDetector buildingsDetector(BBox(size, origin));
	Sampler< BuildingsDetector > sampler( BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() , BuildingsDetectorParametersSingleton::Instance()->DecreaseCoefficient(), BuildingsDetectorParametersSingleton::Instance()->CumulatedProbabilities() );
	std::cout << BuildingsDetectorParametersSingleton::Instance()->InitialTemperature() << std::endl;

	// Formattage du log sous forme de tableau, ca facilite la creation de graphiques ...
	std::ostringstream my_out_stream;
	my_out_stream << "\tIte\t";
	my_out_stream << "NbObjects\t";
	my_out_stream << "Time(s)\t";
	my_out_stream << "Temp\t";
	my_out_stream << "E_data\t";
	my_out_stream << "E_priori\t";
	my_out_stream << "E_total\t";
	std::cout << my_out_stream.str() << std::endl;

	clock_t clock_debut = clock(), clock_local = clock();
	for (unsigned int current_iter=0; current_iter <= BuildingsDetectorParametersSingleton::Instance()->NbIterations(); ++current_iter)
	{
		if (current_iter % BuildingsDetectorParametersSingleton::Instance()->NbIterationsDump() == 0)
		{
			my_out_stream.str("");
			my_out_stream << "\t" << current_iter;
			my_out_stream << "\t" << buildingsDetector.GetNbVertices();
			my_out_stream << "\t" << double(clock()- clock_local) / CLOCKS_PER_SEC;
			my_out_stream << "\t" << sampler.Temperature();
			my_out_stream << "\t" << buildingsDetector.DataEnergy();
			my_out_stream << "\t" << buildingsDetector.PriorEnergy();
			my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
			std::cout << my_out_stream.str();
			clock_local = clock();
		}
		sampler.Itere(buildingsDetector);
	}
	clock_t clock_fin = clock();

	my_out_stream.str("");
	my_out_stream << "Iterations finished" << std::endl;
	double elapsed = double(clock_fin - clock_debut) / CLOCKS_PER_SEC;
	my_out_stream << "Total elapsed time (s) :  " << elapsed << std::endl;
	my_out_stream << "Mean time per iteration (ms) :  " << elapsed * 1000 / BuildingsDetectorParametersSingleton::Instance()->NbIterations() << std::endl;
	my_out_stream << "\t" << buildingsDetector.GetNbVertices();
	my_out_stream << "\t" << sampler.Temperature();
	my_out_stream << "\t" << buildingsDetector.DataEnergy();
	my_out_stream << "\t" << buildingsDetector.PriorEnergy();
	my_out_stream << "\t" << buildingsDetector.DataEnergy() + buildingsDetector.PriorEnergy() << std::endl;
	my_out_stream << "Graph Data energy integrity : " << buildingsDetector.CheckGraphDataEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Prior energy integrity: " << buildingsDetector.CheckGraphPriorEnergyIntegrity() << std::endl;
	my_out_stream << "Graph Structure integrity : " << buildingsDetector.CheckGraphStructureIntegrity() << std::endl;
	buildingsDetector.Dump(my_out_stream);
	std::cout << my_out_stream.str();
	//buildingsDetector.InitExport();
	//BuildingsDetector::vertex_iterator it_v = vertices(buildingsDetector.GetGraph()).first, fin_v = vertices(buildingsDetector.GetGraph()).second;
	//for (; it_v != fin_v; ++it_v)
	//	buildingsDetector.ExportNode(buildingsDetector.GetGraph()[*it_v]);
	//buildingsDetector.EndExport("final_out.tif");

	return 0;
}