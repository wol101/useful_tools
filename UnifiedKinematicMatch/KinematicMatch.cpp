// This is the wrapper to call GeneralGA to solve the kinematic match problem

// the basic outline is as follows

// 1. Set up the required population files and config files
// 2. Run the GA
// 3. Repeat as required.

// for each repeat the population file needs to have the correct randomisation and fixation applied

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <alloca.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>

#include "../GeneralGA/src/DataFile.h"
#include "../GeneralGA/src/Random.h"

struct Genome
{
    std::vector<double> value;
    std::vector<double> low;
    std::vector<double> high;
    std::vector<double> sd;
    double extra[5];
};    

void Fix(std::vector<Genome *> *population, int startLocus, int endLocus);
void Randomise(std::vector<Genome *> *population, int startLocus, int endLocus);
int GAMain(int argc, char *argv[]);

// returns zero on no erro, or the line number of an error
int KinematicMatch(int argc, char *argv[])
{
    char rangeControlFind[512];
    char rangeControlReplace[512];
    double *rangeControlList;
    int *randomiseLocusList;
    int *fixLocusList;
    int nRuns;
    int i, j, k;
    
    const char *modelConfigFile = "modelConfigFile.xml";
    const char *workingConfigFile = "workingConfigFile.xml";
    const char *workingPopulationFile = "workingPopulationFile.txt";
    const char *workingParameterFile = "modelParameterFile.txt"; // currently the parameter file is not changed
    DataFile file;
    char buffer[512];
    char dirname[512];
    char *pServerArgs[256];
    int nServerArgs;
    
    std::vector<Genome *> population;
    Genome *genome;
    double v;
    int populationSize, magic, genomeSize;
    std::ofstream *out;
    
    DIR *dir;
    struct dirent* entry;
    std::vector<std::string> fileList;
    
    RANDOM_SEED_TYPE randomSeed = (RANDOM_SEED_TYPE)time(0) * (RANDOM_SEED_TYPE)getpid();
    RandomSeed(randomSeed);
    
    const char *configFile = "config.txt";
    char lastPopulationFile[512];
    char modelPopulationFile[512];
    
    file.ReadFile(configFile);
    file.RetrieveParameter("modelPopulationFile", modelPopulationFile, sizeof(modelPopulationFile) - 1, true);
    file.RetrieveParameter("nRuns", &nRuns, true);
    rangeControlList = (double *)alloca(nRuns * sizeof(double));
    randomiseLocusList = (int *)alloca(nRuns * sizeof(int));
    fixLocusList = (int *)alloca(nRuns * sizeof(int));
    file.RetrieveParameter("rangeControlList", nRuns, rangeControlList, true);
    file.RetrieveParameter("randomiseLocusList", nRuns, randomiseLocusList, true);
    file.RetrieveParameter("fixLocusList", nRuns, fixLocusList, true);
    file.RetrieveParameter("rangeControlFind", rangeControlFind, sizeof(rangeControlFind) - 1, true);
    file.RetrieveParameter("rangeControlReplace", rangeControlReplace, sizeof(rangeControlReplace) - 1, true);
    
    strcpy(lastPopulationFile, modelPopulationFile);
    for (i = 0; i < nRuns; i++)
    {
        // prepare the workingConfigFile
        std::cerr << "Reading modelConfigFile: " << modelConfigFile << "\n";
        file.ReadFile(modelConfigFile);
        sprintf(buffer, rangeControlReplace, rangeControlList[i]);
        std::cerr << "Replacing " << rangeControlFind << " with " << buffer << "\n";
        file.Replace(rangeControlFind, buffer);
        std::cerr << "Writing workingConfigFile: " << workingConfigFile << "\n";
        file.WriteFile(workingConfigFile);
        
        // prepare the populationFile
        std::cerr << "Reading lastPopulationFile: " << lastPopulationFile << "\n";
        file.ReadFile(lastPopulationFile);
        file.ReadNext(&populationSize);
        for (j = 0; j < populationSize; j++)
        {
            file.ReadNext(&magic);
            if (magic != -1) return __LINE__;
            file.ReadNext(&genomeSize);
            genome = new Genome();
            for (k = 0; k < genomeSize; k++)
            {
                file.ReadNext(&v);
                genome->value.push_back(v);
                file.ReadNext(&v);
                genome->low.push_back(v);
                file.ReadNext(&v);
                genome->high.push_back(v);
                file.ReadNext(&v);
                genome->sd.push_back(v);
            }
            for (k = 0; k < 5; k++)
            {
                file.ReadNext(&v);
                genome->extra[k] = v;
            }
            population.push_back(genome);
        }
        
        Fix(&population, 0, fixLocusList[i]);
        Randomise(&population, randomiseLocusList[i], INT_MAX);
        
        std::cerr << "Writing workingPopulationFile: " << workingPopulationFile << "\n";
        out = new std::ofstream(workingPopulationFile);
        (*out) << populationSize << "\n";
        for (j = 0; j < populationSize; j++)
        {
            genome = population[j];
            (*out) << "-1 " << genome->value.size();
            for (k = 0; k < genome->value.size(); k++)
                (*out) << " " << genome->value[k] << " " << genome->low[k] << " " << genome->high[k] << " " << genome->sd[k];
            for (k = 0; k < 5; k++)
                (*out) << " " << genome->extra[k];
            (*out) << "\n";
        }
        out->close();
        delete out;
        for (j = 0; j < population.size(); j++) delete population[j];
        population.clear();
        
        // create the output folder
        time_t theTime = time(0);
        struct tm *theLocalTime = localtime(&theTime);
        sprintf(dirname, "%f_Run_%04d-%02d-%02d_%02d.%02d.%02d", 
                        rangeControlList[i],
                        theLocalTime->tm_year + 1900, 
                        theLocalTime->tm_mon + 1, 
                        theLocalTime->tm_mday,
                        theLocalTime->tm_hour, theLocalTime->tm_min,
                        theLocalTime->tm_sec);
        std::cerr << "Creating folder " << dirname << "\n";
        mkdir(dirname, 0777);
        
        sprintf(buffer, "ga -p %s -o %s", workingParameterFile, dirname);
        for (j = 1; j < argc; j++)
        {
            strcat(buffer, " ");
            strcat(buffer, argv[j]);
        }
        std::cerr << "Executing command:\n";
        std::cerr << buffer << "\n";
        nServerArgs = DataFile::ReturnTokens(buffer, pServerArgs, 256);
        GAMain(nServerArgs, pServerArgs);
        std::cerr << "GAMain finished\n";
        
        dir = opendir(dirname);
        fileList.clear();
        while( (entry = readdir(dir))) fileList.push_back(entry->d_name);
        closedir(dir);
        std::sort(fileList.begin(), fileList.end());
        
        for (j = fileList.size() - 1; j >= 0; j--)
        {
            if (fileList[j].find("Population") == 0) break;
        }
        if (j < 0) return __LINE__;
        
        sprintf(lastPopulationFile, "%s/%s", dirname, fileList[j].c_str());
        std::cerr << "New lastPopulationFile is now " << lastPopulationFile << "\n";
        
    }

    return 0;
}

// fix the values at the loci
// numbers are inclusive so should range from 0 to size-1
void Fix(std::vector<Genome *> *population, int startLocus, int endLocus)
{
    std::cerr << "Fix " << startLocus << " to " << endLocus << "\n";
    int i, j;
    Genome *g;
    for (i = 0; i < population->size(); i++)
    {
        g = (*population)[i];
        if (startLocus >= g->value.size()) return;
        if (endLocus < 0) return;
        if (startLocus < 0) startLocus = 0;
        if (endLocus >= g->value.size()) endLocus = g->value.size() - 1;
    
        for (j = startLocus; j <= endLocus; j++) g->sd[j] = 0;
    }
}

void Randomise(std::vector<Genome *> *population, int startLocus, int endLocus)
{
    std::cerr << "Randomise " << startLocus << " to " << endLocus << "\n";
    int i, j;
    Genome *g;
    for (i = 0; i < population->size(); i++)
    {
        g = (*population)[i];
        if (startLocus >= g->value.size()) return;
        if (endLocus < 0) return;
        if (startLocus < 0) startLocus = 0;
        if (endLocus >= g->value.size()) endLocus = g->value.size() - 1;
    
        for (j = startLocus; j <= endLocus; j++) g->value[j] = RandomDouble(g->low[j], g->high[j]);
    }
}

