// Population.cc - storage class for individual genomes

#if defined(USE_MPI)
#include <mpi.h>
#endif

#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <map>
#include <queue>

using namespace std;

#include "Genome.h"
#include "Population.h"
#include "Random.h"
#include "Objective.h"
#include "QuickSort.h"
#include "Debug.h"
#include "XMLConverter.h"

#ifdef USE_UDP
#include "UDP.h"
extern UDP gUDP;

#elif defined(USE_TCP)
#include <pthread.h>
#include "TCP.h"
extern TCP gTCP;
pthread_mutex_t gMutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t gMutex2 = PTHREAD_MUTEX_INITIALIZER;

#elif defined(USE_MPI)
#include "MPIStuff.h"
extern int gMPI_Comm_size;
extern int gMPI_Comm_rank;
#endif

extern int gUsleep;
extern int gRedundancyPercent;
extern XMLConverterGA *gXMLConverter;
extern int gTCPRetryCount;

// constructor
Population::Population()
{
    m_Population = 0;
    m_PopulationSize = 0;
    m_SelectionType = RankBasedSelection;
    m_MaxRunning = 1;
    m_CumulativeFitness = 0;
}

// destructor
Population::~Population()
{
    int i;
    if (m_Population) 
    {
        for (i = 0; i < m_PopulationSize; i++) delete m_Population[i];
        delete [] m_Population;
    }
    if (m_CumulativeFitness) delete [] m_CumulativeFitness;
}

// initialise the population 
void Population::InitialisePopulation(int populationSize,
                                      GenomeType genomeType, 
                                      int genomeLength,
                                      double lowBound,
                                      double highBound,
                                      bool randomFlag, 
                                      double value,
                                      double gaussianSD)
{
    int i;
    
    if (gDebug == Debug_Population)
        std::cerr << "Population::InitialisePopulation(" << populationSize
        << " " << genomeType
        << " " << genomeLength
        << " " << lowBound
        << " " << highBound
        << " " << randomFlag
        << " " << value
        << " " << gaussianSD << ")\n";
    
    m_PopulationSize = populationSize;
    
    m_CumulativeFitness = new double[m_PopulationSize];
    m_Population = new Genome *[m_PopulationSize];
    for (i = 0; i < m_PopulationSize; i++)
    {
        m_Population[i] = new Genome();
        m_Population[i]->InitialiseGenome(genomeType, genomeLength, 
                                          lowBound, highBound,
                                          randomFlag, value, gaussianSD);
    }
}

// choose a parent from a population
Genome * Population::ChooseParent(int *parentRank)
{
    double r;
    int l, u, p, d;
    
    // in this version we do uniform selection and just choose a parent
    // at random
    if (m_SelectionType == UniformSelection)
    {
        *parentRank = RandomInt(0, m_PopulationSize - 1);
        return m_Population[*parentRank];
    }
    
    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == RankBasedSelection)
    {
        *parentRank = RankBiasedRandomInt(1, m_PopulationSize) - 1;
        return m_Population[*parentRank];
    }
    
    // this type biases random choice to higher ranked individuals
    // this assumes a sorted genome
    if (m_SelectionType == SqrtBasedSelection)
    {
        *parentRank = SqrtBiasedRandomInt(0, m_PopulationSize - 1);
        return m_Population[*parentRank];
    }
    
    // this type biases random choice to higher ranked individuals
    // again assumes a sorted genome since m_CumulativeFitness is
    // allocated at sort time
    // MUST HAVE POSITIVE FITNESS
    if (m_SelectionType == RouletteWheelSelection)
    {
        // if fitness total is zero fall back on uniform selection
        if (m_CumulativeFitness[m_PopulationSize - 1] == 0)
        {
            *parentRank = RandomInt(0, m_PopulationSize - 1);
            return m_Population[*parentRank];
        }
        
        // first need to get a suitable random number
        r = RandomDouble(0, m_CumulativeFitness[m_PopulationSize - 1]);
        // now find the index where: 
        // m_CumulativeFitness[index - 1] < r <= m_CumulativeFitness[index]
        l = 0;
        u = m_PopulationSize - 1;
        p = (u - l) / 2;
        while (true)
        {
            if (m_CumulativeFitness[p - 1] < r && m_CumulativeFitness[p] >= r) break;
            if (r > m_CumulativeFitness[p]) 
            {
                l = p;
                d = (u - l) / 2;
                if (d == 0) d = 1;
                p += d;
            }
            else 
            {
                u = p;
                d = (u - l) / 2;
                if (d == 0) d = 1;
                p -= d;
            }
        }
        *parentRank = p;
        
        if (gDebug == Debug_Population)
            cerr << "Population::ChooseParent\tRouletteWheelSelection\tr\t" <<
            r << "\tparentRank\t" << *parentRank << "\tm_CumulativeFitness\t" <<
            m_CumulativeFitness[p] << "\n";
        
        return m_Population[*parentRank];
    }
    
    // should never get here
    return 0;
}

// sort by fitness and fill in the m_CumulativeFitness if we
// are using it
void Population::Sort()
{
    double total;
    double fitness;
    int i;
    
    QuickSort(m_Population, m_PopulationSize);
    
    if (m_SelectionType == RouletteWheelSelection)
    {
        // need to fill m_CumulativeFitness
        total = 0;
        for (i = 0; i < m_PopulationSize; i++)
        {
            fitness = m_Population[i]->GetFitness();
            if (fitness < 0) fitness = 0;
            m_CumulativeFitness[i] = total + fitness;
            total += fitness;
            
            if (gDebug == Debug_Population)
                cerr << "Population::Sort\tm_CumulativeFitness[\t" << i <<
                "\t]\t" << m_CumulativeFitness[i] << "\n";
        }
    }
}

// randomise a subset of the population
void Population::Randomise(int from, int to)
{
    int i;
    
    for (i = from; i < to; i++)
        m_Population[i]->Randomise();
}

// resize a population
void Population::Resize(int newSize)
{
    if (newSize == m_PopulationSize) return;
    
    Genome **newPopulation = new Genome *[newSize];
    int i;
    
    if (newSize < m_PopulationSize) // make smaller by deleting all the worse ones
    {
        int offset = m_PopulationSize - newSize;
        for (i = 0; i < newSize; i++)
        {
            newPopulation[i] = m_Population[i + offset];
        }
        for (i = 0; i < m_PopulationSize - newSize; i++) delete m_Population[i];
    }
    else // make bigger by filling the blanks with random new data
    {
        int offset = newSize - m_PopulationSize;
        for (i = 0; i < m_PopulationSize; i++)
        {
            newPopulation[i + offset] = m_Population[i];
        }
        for (i = 0; i < offset; i++)
        {
            newPopulation[i] = new Genome();
            *newPopulation[i] = *m_Population[0];
            newPopulation[i]->Randomise();
        }
    }

    delete [] m_Population;
    delete [] m_CumulativeFitness;
    m_CumulativeFitness = new double[newSize];
    m_Population = newPopulation;
    m_PopulationSize = newSize;
    
}


// output a subpopulation as a new population
void Population::OutputSubpopulation(const char *filename, int nBest)
{
    if (nBest <= 0 || nBest > m_PopulationSize) nBest = m_PopulationSize;
    
    ofstream outFile(filename);
    
    outFile << nBest << "\n";
    int index = m_PopulationSize - 1;
    for (int i = 0; i < nBest; i++)
    {
        outFile << *(m_Population[index]);
        index--;
    }
}

#if defined (USE_FS)

// calculate the fitness for a subset of the population
void Population::CalculateFitness(int from, int to, char *programName, time_t timeLimit,
                                  int restartOnError)
{
    int i, j;
    int nRunning = 0;
    
    if (m_MaxRunning == 1)
    {
        Objective objective;
        objective.SetSynchronousFlag(true);
        if (programName) objective.SetProgramName(programName);
        // loop through the population
        for (i = from; i < to; i++)
        {
            if (m_Population[i]->GetFitnessValid() == false)
            {
                objective.Run(i, m_Population[i]);
                objective.CheckFinished(); // needed anyway
                m_Population[i]->SetFitness(objective.GetScore());
                objective.CleanUp();
            }
        }
    }
    else
    {
        Objective *runList = new Objective[m_MaxRunning];
        if (programName)
        {
            for (i = 0; i < m_MaxRunning; i++)
            {
                runList[i].SetProgramName(programName);
            }
        }
        // loop through the population
        for (i = from; i < to; i++)
        {
            if (m_Population[i]->GetFitnessValid() == false)
            {
                while (nRunning >= m_MaxRunning) // need to look for some finished processes
                {
                    usleep(gUsleep); // sleep for short period - might need to tweak
                    // look for a finished process
                    for (j = 0; j < m_MaxRunning; j++)
                    {
                        if (runList[j].CheckFinished())
                        {
                            m_Population[runList[j].GetID()]->SetFitness(runList[j].GetScore());
                            runList[j].CleanUp();
                            nRunning--;
                        }
                    }
                }
                
                // must now be space to start up a new process
                for (j = 0; j < m_MaxRunning; j++)
                {
                    if (runList[j].Valid() == false) // find gap in list (there *must* be one)
                    {
                        runList[j].Run(i, m_Population[i]);
                        nRunning++;
                        break;
                    }
                }
            }
        }
        
        // there are probably still a few processes running
        while (nRunning)
        {
            for (j = 0; j < m_MaxRunning; j++)
            {
                if (runList[j].CheckFinished())
                {
                    m_Population[runList[j].GetID()]->SetFitness(runList[j].GetScore());
                    runList[j].CleanUp();
                    nRunning--;
                }
            }
        }      
        delete [] runList;
    }
}


#elif defined(USE_SOCKETS)

// calculate the fitness for a subset of the population    
void Population::CalculateFitness(int from, int to, 
                                  char *programName, time_t timeLimit, 
                                  int restartOnError)
{
    int i, j;
    bool stillWaiting;
    
    if (gDebug == Debug_Population)
        cerr << "Population::CalculateFitness\tfrom\t" << from << "\tto\t" << to
        << "\ttimeLimit\t" << timeLimit << "\n";
    
    
    int runListSize = to - from;
    Objective *runList = new Objective[runListSize];
    
    // loop through runList starting processes
    for (i = 0; i < runListSize; i++)
    {
        // set up the data
        runList[i].SetID(from + i);
        
        //if (m_Population[from + i]->GetFitnessValid() == true)
        //  cerr << "m_Population[" << from + i << "]->GetFitnessValid() is true\n";
        
        if (m_Population[from + i]->GetFitnessValid() == false)
        {
            runList[i].SetGenome(m_Population[from + i]);
            runList[i].SetTimeLimit(timeLimit);
            if (programName) runList[i].SetProgramName(programName);
            
            // and wait until the process starts up
            while (runList[i].Run() == false) 
            {
                // cerr << "runList[i].Run() " << i << "\n";
                for (j = 0; j < i; j++) runList[j].CheckRunning();
                usleep(gUsleep); // sleep for short period
            }
        }
    }
    
    // now loop until all the processes have finished
    do
    {
        stillWaiting = false;
        for (j = 0; j < runListSize; j++) 
        {
            if (runList[j].CheckRunning()) stillWaiting = true;
            // cerr << "CalculateFitness Objective::CheckRunning() stillWaiting " << stillWaiting << "\n";
        }
        //Objective::ReportBusy();
        //cerr << "CalculateFitness Objective::ReportBusy() runListSize " << runListSize << " stillWaiting " << stillWaiting << "\n";
    } while (stillWaiting);
    
    // all finished now so read off the fitness values
    for (i = 0; i < runListSize; i++)
    {
        //if (m_Population[runList[i].GetID()]->GetFitnessValid() == true)
        //  cerr << "m_Population[" << runList[i].GetID() << "]->GetFitnessValid() is true\n";
        
        if (m_Population[runList[i].GetID()]->GetFitnessValid() == false)
            m_Population[runList[i].GetID()]->SetFitness(runList[i].GetScore());
    }
    
    // and delete the run list  
    delete [] runList;
}


#elif defined(USE_UDP)

// calculate the fitness for a subset of the population    
void Population::CalculateFitness(int from, int to, 
                                  char *programName, time_t timeLimit, 
                                  int restartOnError)
{
    if (gDebug == Debug_Population)
        cerr << "Population::CalculateFitness\tfrom\t" << from << "\tto\t" << to
        << "\ttimeLimit\t" << timeLimit << "\n";
    
    // check which values need calculating
    std::map<int, UDPRunSpecifier *>toSendList;
    std::map<int, UDPRunSpecifier *>::iterator iter;
    int i;
    for (i = from; i < to; i++) 
    {
        if (m_Population[i]->GetFitnessValid() == false)
        {
            toSendList[i] = new UDPRunSpecifier();
            // memset(toSendList[i], 0, sizeof(RunSpecifier));
        }
    }
    
    // start server loop
    bool finished = false;
    UDPPacket *packet;
    int status;
    int index;
    std::map<int, UDPRunSpecifier *>toRunList;
    std::map<int, UDPRunSpecifier *>runningList;
    
    while (finished == false)
    {
        status = gUDP.ReceiveUDPPacket(sizeof(SendTextUDPPacket));
        packet = gUDP.GetUDPPacket();
        
        // std::cerr << "UDPPacket Type : " << packet->type << " size " << status << "\n";
        
        switch (packet->type)
        {
            case request_send_genome:
            {
                if (toSendList.empty() == false)
                {
                    iter = toSendList.begin();
                    gXMLConverter->ApplyGenome(m_Population[iter->first]);
                    int len;
                    char *buf = (char *)gXMLConverter->GetFormattedXML(&len);
                    len++; // to include trailing zero
                    struct sockaddr_in  *senderAddress = gUDP.GetSenderAddress();
                    senderAddress->sin_port = ((RequestSendGenomeUDPPacket *)packet)->port;
                    iter->second->packetID = ((RequestSendGenomeUDPPacket *)packet)->packetID;
                    iter->second->startTime = time(0);
                    if (gRedundancyPercent <= 0)
                    {
                        status = gUDP.SendText(senderAddress, iter->first, buf, len);
                    }
                    else
                    {
                        status = gUDP.SendFEC(senderAddress, iter->first, buf, len, gRedundancyPercent + 100);
                    }
                    toRunList[iter->first] = iter->second;
                    toSendList.erase(iter);
                }
                break;
            }
                
            case genome_received:
            {
                struct sockaddr_in  *senderAddress = gUDP.GetSenderAddress();
                senderAddress->sin_port = ((GenomeReceivedUDPPacket *)packet)->port;
                index = ((GenomeReceivedUDPPacket *)packet)->index;
                iter = toRunList.find(index);
                if  (iter != toRunList.end() && iter->second->packetID == ((GenomeReceivedUDPPacket *)packet)->packetID)
                {
                    memcpy(&iter->second->runningOn, senderAddress, sizeof(struct sockaddr_in));
                    runningList[index] = iter->second;
                    toRunList.erase(iter);
                }
                break;
            }
                
            case send_result:
            {
                // std::cerr << ((SendResultUDPPacket *)packet)->index << " " <<  ((SendResultUDPPacket *)packet)->result << "\n";
                // check it comes from the right machine
                iter = runningList.find(((SendResultUDPPacket *)packet)->index);
                if (iter != runningList.end())
                {
                    if (iter->second->runningOn.sin_addr.s_addr == gUDP.GetSenderAddress()->sin_addr.s_addr 
                        && iter->second->runningOn.sin_port == ((SendResultUDPPacket *)packet)->port
                        && iter->second->packetID == ((SendResultUDPPacket *)packet)->packetID)
                    {
                        delete iter->second;
                        runningList.erase(iter);
                        m_Population[((SendResultUDPPacket *)packet)->index]->SetFitness(((SendResultUDPPacket *)packet)->result);
                        // std::cerr << ((SendResultUDPPacket *)packet)->index << " " <<  ((SendResultUDPPacket *)packet)->result << "\n";
                    }
                }
                break;
            }
                
        }
        
        if (toSendList.empty())
        {
            if (toRunList.empty())
            {
                if (runningList.empty())
                {
                    finished = true;
                }
                else
                {
                    for (iter = runningList.begin(); iter != runningList.end(); iter++)
                    {
                        status = gUDP.CheckAlive(&iter->second->runningOn);
                        if (status == -1 || (time(0) - iter->second->startTime) > timeLimit)
                        {
                            toSendList[iter->first] = iter->second;
                            runningList.erase(iter);
                        }
                    }
                }
            }
            else
            {
                for (iter = toRunList.begin(); iter != toRunList.end(); iter++)
                {
                    status = gUDP.CheckAlive(&iter->second->runningOn);
                    if (status == -1 || (time(0) - iter->second->startTime) > 5) // should not take long to get a response from client
                    {
                        toSendList[iter->first] = iter->second;
                        toRunList.erase(iter);
                    }
                }
            }
        }
        
    }
    
}


#elif defined(USE_TCP)

struct TCPThreadRunSpecifier
{
    time_t timeLimit;
    unsigned long packetID;
    int status;
    double result;
    int index;
    int retry;
    Genome *genome;
    TCP *tcp;
    pthread_t thread;
};

//#define THREAD_JOINABLE
// calculate the fitness for a subset of the population    
void Population::CalculateFitness(int from, int to, 
                                  char *programName, time_t timeLimit, 
                                  int restartOnError)
{
    if (gDebug == Debug_Population)
        cerr << "Population::CalculateFitness\tfrom\t" << from << "\tto\t" << to
        << "\ttimeLimit\t" << timeLimit << "\n";
    
    // check which values need calculating
    std::map<int, TCPThreadRunSpecifier *>toSendList;
    std::map<int, TCPThreadRunSpecifier *>::iterator iter;
    int i;
    for (i = from; i < to; i++) 
    {
        if (m_Population[i]->GetFitnessValid() == false)
        {
            toSendList[i] = new TCPThreadRunSpecifier();
            toSendList[i]->retry = 0;
            // memset(toSendList[i], 0, sizeof(TCPRunSpecifier));
        }
    }
    
    if (toSendList.empty()) return;
    
    pthread_attr_t tattr;
    pthread_t tid;
    
    // start server loop
    bool finished = false;
    int status;
    std::map<int, TCPThreadRunSpecifier *>runningList;
    TCP *connection;
    
    while (finished == false)
    {
        if (toSendList.empty() == false)
        {
            // first check to see if there are new connections
            if (gTCP.CheckReceiver(1, 0) == 1)
            {
                connection = new TCP();
                if (connection->StartAcceptor(gTCP.GetSocket()) == -1) 
                {
                    delete connection;
                }
                else
                {
                    iter = toSendList.begin();
                    if (gDebug == Debug_Population)
                        std::cerr << "Population::CalculateFitness\tindex\t" << iter->first << "\n";
                    iter->second->index = iter->first;
                    iter->second->status = -1;
                    iter->second->genome = m_Population[iter->first];
                    iter->second->tcp = connection;
                    iter->second->timeLimit = timeLimit;
                    
                    pthread_attr_init(&tattr);
#ifdef THREAD_JOINABLE
                    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_JOINABLE);
#else
                    pthread_attr_setdetachstate(&tattr, PTHREAD_CREATE_DETACHED);
#endif
                    pthread_attr_setstacksize (&tattr, 1024 * 64); // 64k should be plenty (really!)
                    status = pthread_create(&tid, &tattr, ThreadedCommunication, (void *) iter->second);
                    pthread_attr_destroy(&tattr);
                    
                    if (status == 0)
                    {
                        iter->second->thread = tid;
                        runningList[iter->first] = iter->second;
                        toSendList.erase(iter);
                    }
                    else
                    {
                        if (gDebug == Debug_Population)
                            std::cerr << "Population::CalculateFitness\tpthread_create\t" << status << "\n";
                        connection->StopAcceptor();
                        delete connection;
                        sleep(1); // this is really an error condition - it might fix itself over time
                    }
                }
            }
        }
        else
        {
            //std::cerr << "runningList.size() " << runningList.size() << "\n";
            if (runningList.empty())
            {
                finished = true;
            }
            else
            {
                for (iter = runningList.begin(); iter != runningList.end(); iter++)
                {
                    pthread_mutex_lock( &gMutex2 );
                    status = iter->second->status;
                    pthread_mutex_unlock( &gMutex2 );
                    if (gDebug == Debug_Population)
                        std::cerr << "Population::CalculateFitness\tindex\t" << iter->first << "\tstatus\t" << iter->second->status << "\tresult\t" << iter->second->result << "\n";
                    if (status > 0)
                    {
#ifdef THREAD_JOINABLE
                        status = pthread_join(iter->second->thread, 0);
#endif
                        iter->second->retry++;
                        if (iter->second->retry < gTCPRetryCount)
                        {
                            toSendList[iter->first] = iter->second;
                            runningList.erase(iter);
                        }
                        else
                        {
                            std::cerr << "Warning: exceeded retry count on " << iter->first << "\n";
                            m_Population[iter->first]->SetFitness(0);
                            delete iter->second;
                            runningList.erase(iter);
                        }
                    }
                    else
                    {
                        if (status == 0)
                        {
#ifdef THREAD_JOINABLE
                            status = pthread_join(iter->second->thread, 0);
#endif
                            m_Population[iter->first]->SetFitness(iter->second->result);
                            delete iter->second;
                            runningList.erase(iter);
                        }
                    }
                }
                if (toSendList.empty()) usleep(gUsleep); // only sleep if there is really nothing to do but wait
            }
        }
    }
    
}

#elif defined(USE_MPI)

// calculate the fitness for a subset of the population    
void Population::CalculateFitness(int from, int to, 
                                  char *programName, time_t timeLimit, 
                                  int restartOnError)
{
    if (gDebug == Debug_Population)
        cerr << "Population::CalculateFitness\tfrom\t" << from << "\tto\t" << to
        << "\ttimeLimit\t" << timeLimit << "\n";
    
    // check which values need calculating
    std::map<int, MPIRunSpecifier *>toSendList;
    std::map<int, MPIRunSpecifier *>::iterator iter;
    int i;
    for (i = from; i < to; i++) 
    {
        if (m_Population[i]->GetFitnessValid() == false)
        {
            toSendList[i] = new MPIRunSpecifier();
        }
    }
    
    std::queue<int>freeList;
    for (i = 1; i < gMPI_Comm_size; i++)
    {
        freeList.push(i);
    }
    
    // start server loop
    int genomeLength = m_Population[0]->GetGenomeLength();
    bool finished = false;
    std::map<int, MPIRunSpecifier *>runningList;
    MPI_Status status;
    char buffer[256];
    int flag;
    int len = (genomeLength) * sizeof(double) + 3 * sizeof(int);
    char *buf = new char[len];
    while (finished == false)
    {
        if (freeList.empty() == false)
        {
            if (toSendList.empty() == false)
            {
                iter = toSendList.begin();
                int *iPtr = (int *)buf;
                iPtr[0] = MPI_MESSAGE_ID_SEND_GENOME_DATA;
                iPtr[1] = iter->first;
                iPtr[2] = genomeLength;
                memcpy((double *)(&iPtr[3]), m_Population[iter->first]->GetGenes(), genomeLength * sizeof(double));
                
                iter->second->startTime = time(0);
                iter->second->mpiSource = freeList.front();
                
                if (gDebug == Debug_Population)
                    cerr << "Population::CalculateFitness\tindex\t" << iter->first << "\tstartTime\t" << iter->second->startTime
                    << "\tMPI_SOURCE\t" << iter->second->mpiSource << "\n";
                
                
                MPI_Send(buf,                    /* message buffer */ 
                         len,                    /* 'len' data item */ 
                         MPI_BYTE,               /* data items are bytes */ 
                         iter->second->mpiSource,/* destination */ 
                         0,                      /* user chosen message tag */ 
                         MPI_COMM_WORLD);        /* default communicator */ 
                
                runningList[iter->first] = iter->second;
                toSendList.erase(iter);
                freeList.pop();
                continue; // this means that stuffing the clients takes priority
            }
        }
        
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        if (flag)
        {
            MPI_Recv(buffer,            /* message buffer */ 
                     sizeof(buffer),    /* max number of data items */ 
                     MPI_BYTE,          /* of type BYTE */ 
                     MPI_ANY_SOURCE,    /* receive from any sender */ 
                     MPI_ANY_TAG,       /* any type of message */ 
                     MPI_COMM_WORLD,    /* default communicator */ 
                     &status);          /* info about the received message */ 
            
            if (((int *)buffer)[0] == MPI_MESSAGE_ID_SEND_RESULTS)
            {
                int index = ((int *)buffer)[1];
                double result = *(double *)(&(((int *)buffer)[2]));
                if (gDebug == Debug_Population)
                    cerr << "Population::CalculateFitness\tindex\t" << index << "\tresult\t" << result << "\n";
                iter = runningList.find(index);
                if (iter != runningList.end())
                {
                    if (iter->second->mpiSource == status.MPI_SOURCE)
                    {
                        if (gDebug == Debug_Population)
                            cerr << "Population::CalculateFitness\tfitness\t" << result << "\n";
                        m_Population[index]->SetFitness(result);
                        delete iter->second;
                        runningList.erase(iter);
                        freeList.push(status.MPI_SOURCE);
                    }
                }
                
            }
        }
        
        if (toSendList.empty())
        {
            if (runningList.empty())
            {
                finished = true;
            }
            else
            {
                for (iter = runningList.begin(); iter != runningList.end(); iter++)
                {
                    if ((time(0) - iter->second->startTime) > timeLimit)
                    {
                        std::cerr << "Population::CalculateFitness Timeout on " << iter->second->mpiSource << "\n";
                        toSendList[iter->first] = iter->second;
                        runningList.erase(iter);
                    }
                }
            }
        }
        
    }
    
}

#endif

// Resort a previously sorted genome so that only unique fitness elements
// are represented (duplicates are put to the end of the list)
void Population::UniqueResort()
{
    int i, j;
    Genome *t;
    
    for (i = m_PopulationSize - 1; i >= 1; i--)
    {
        if (m_Population[i]->GetFitness() <= m_Population[i - 1]->GetFitness())
        {
            // got a match so search down list for first non-match
            for (j = i - 2; j >= 0; j--)
            {
                if (m_Population[i]->GetFitness() > m_Population[j]->GetFitness()) break;
            }
            if (j < 0) break; // we've done the best we can
            t = m_Population[i - 1];
            m_Population[i - 1] = m_Population[j];
            m_Population[j] = t;
        }
    }
}

ostream& operator<<(ostream &out, Population &g)
{
    int i;
    out << g.m_PopulationSize << "\n";
    for (i = g.m_PopulationSize - 1; i >= 0; i--)
        out << *(g.m_Population[i]);
    return out;
}

istream& operator>>(istream &in, Population &g)
{
    int i;
    int populationSize;
    
    in >> populationSize;
    if (gDebug == Debug_Population)
        std::cerr << "Population operator>> populationSize = " << populationSize << "\n";
    if (populationSize !=g.m_PopulationSize)
    {
        if (g.m_Population) 
        {
            for (i = 0; i < g.m_PopulationSize; i++)
                delete g.m_Population[i];
            delete [] g.m_Population;
        }
        if (g.m_CumulativeFitness) delete [] g.m_CumulativeFitness;
        
        g.m_PopulationSize = populationSize;
        g.m_CumulativeFitness = new double[g.m_PopulationSize];
        g.m_Population = new Genome *[g.m_PopulationSize];
        for (i = 0; i < g.m_PopulationSize; i++)
            g.m_Population[i] = new Genome();
    }
    
    for (i = g.m_PopulationSize - 1; i >= 0; i--)
    {
        in >> *(g.m_Population[i]);
        //std::cerr << g.m_Population[i]->GetFitness() << " " << g.m_Population[i]->GetFitnessValid() << "\n";
    }
    return in;
}  

#ifdef USE_TCP
void *ThreadedCommunication(void *threadArg)
{
#ifdef TCP_DEBUG
    std::cerr << "Creating new connection\n";
#endif
    
    TCPThreadRunSpecifier *runSpecifier = (TCPThreadRunSpecifier *)threadArg;
    int status;
    int len;
    char *ptr;
    char *buf = 0;
    char buffer[16];
    
    try
    {
        // get first command
        status = runSpecifier->tcp->ReceiveData(buffer, 16, 10, 0);
        if (status != 16) throw __LINE__;
        if (strcmp(buffer, "req_send_length") != 0) throw __LINE__;
        
        // convert the genome
        pthread_mutex_lock( &gMutex1 );
        gXMLConverter->ApplyGenome(runSpecifier->genome);
        ptr = (char *)gXMLConverter->GetFormattedXML(&len);
        len++; // to include trailing zero
        buf = new char[len];
        memcpy(buf, ptr, len);
        pthread_mutex_unlock( &gMutex1 );
        
        // send the length
        memcpy(buffer, &len, sizeof(len));
        status = runSpecifier->tcp->SendData(buffer, 16);
        if (status != 16) throw __LINE__;
        
        // get second command
        status = runSpecifier->tcp->ReceiveData(buffer, 16, 10, 0);
        if (status != 16) throw __LINE__;
        if (strcmp(buffer, "req_send_data") != 0) throw __LINE__;
        
        status = runSpecifier->tcp->SendData(buf, len);
        delete [] buf;
        buf = 0;
        if (status != len) throw __LINE__;
        
        // wait for the response
        if (runSpecifier->tcp->CheckReceiver(runSpecifier->timeLimit, 0) != 1) throw __LINE__;
        status = runSpecifier->tcp->ReceiveData(buffer, 16, 1, 0);
        if (status != 16) throw __LINE__;
        
        runSpecifier->result = *(double *)buffer;
        
        status = 0;
    }
    
    catch(int e)
    {
        status = e;
        if (buf) delete [] buf;
#ifdef TCP_DEBUG
        std::cerr << "ThreadedCommunication error on line " << status << "\n";
#endif
    }
    
    runSpecifier->tcp->StopAcceptor();
    delete runSpecifier->tcp;
    pthread_mutex_lock( &gMutex2 );
    runSpecifier->status = status;
    pthread_mutex_unlock( &gMutex2 );
    return 0;
    
}
#endif
