// Implementation of Hopfield Network


#include <string.h>
#include <fstream>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <random>
#include <time.h>
#include <algorithm>
#include <iterator>
#include <functional>
#include <limits.h>
#include <iomanip>


using namespace std;
clock_t runStart;

//-------------------------------------------------
void readInputFile(ifstream &inStream, int &numNodes, int &numEdges , vector<vector<bool> > &adjacent, vector<int> &degree,
        vector< vector<int> > &adjList)
{
    //Reads a DIMACS format file and creates the corresponding degree array and adjacency matrix
    char c;
    char str[1000];
    int line=0,i,j;
    numEdges=0;
    int edges=-1;
    int blem=1;
	int multiple = 0;
    while(!inStream.eof())
    {
        line++;
        inStream.get(c);
        if (inStream.eof()) break;
        switch(c)
        {
        case 'p':
			inStream.get(c);
			inStream.getline(str,999,' ');
			if (strcmp(str,"edge") && strcmp(str,"edges")) {
				cerr << "Error reading 'p' line: no 'edge' keyword found.\n";
				cerr << "'" << str << "' found instead\n";
				exit(-1);
			}
			inStream >> numNodes;
			inStream >> numEdges;
			//Set up the 2d adjacency matrix
			adjacent.clear();
			adjacent.resize(numNodes, vector<bool>(numNodes));			
			for (i=0;i<numNodes;i++) for(j=0;j<numNodes;j++) {
				if(i==j)adjacent[i][j]=true; 
				else adjacent[i][j] = false;
			}
			blem=0;
			break;
        case 'n':
			if (blem) {
				cerr << "Found 'n' line before a 'p' line.\n";
				exit(-1);
			}
			int node;
			inStream >> node;
			if (node < 1 || node > numNodes) {
				cerr << "Node number " << node << " is out of range!\n";
				exit(-1);
			}
			node--;	
			cout << "Tags (n Lines) not implemented in g object\n";
			break;

        case 'e':
            int node1,node2;
            inStream >> node1 >>  node2;
            if (node1 < 1 || node1 > numNodes|| node2 < 1 || node2 > numNodes )
            {
                cerr << "Node " << node1 << " or " << node2 << " is out of range!\n";
				exit(-1);
            }
            node1--;
            node2--;
            if (!adjacent[node1][node2])
            {
                edges++;
            }
            else
            {
                multiple++;
				if (multiple<5) {
					cerr << "Warning: in graph file at line " << line << ": edge is defined more than once.\n";
					if (multiple == 4) {
						cerr << "  No more multiple edge warnings will be issued\n";
					}
				}
                
            }
            adjacent[node1][node2] = true;
            adjacent[node2][node1] = true;
            break;
        case 'd':
		case 'v':
		case 'x':
			cerr << "File line " << line << ":\n";
			cerr << "'" << c << "' lines are not implemented yet...\n";
			inStream.getline(str,999,'\n');
			break;
		case 'c':
			inStream.putback('c');
			inStream.get(str,999,'\n');
			break;
		default:
			cerr << "File line " << line << ":\n";
			cerr << "'" << c << "' is an unknown line code\n";
			exit(-1);
        }
        inStream.get(); // Kill the newline
    }
    inStream.close();
    if (multiple) {
		cerr << multiple << " multiple edges encountered\n";
	}
	//Now use the adjacency matrix to construct the degree array and adjacency list
	degree.resize(numNodes, 0);
	adjList.resize(numNodes, vector<int>());
	for(i=0; i<numNodes; i++)
    {
		for(j=0; j<numNodes; j++)
        {
			if(adjacent[i][j] && i!=j)
            {
				adjList[i].push_back(j);
				degree[i]++;
			}
		}
	}
}


//Return maximum in a cluster
inline
int maximum(vector<int > &clusterM )
{ int maxi=-999999999;
    for(int i=0;i<clusterM.size();i++)
    {
        maxi = max(maxi,clusterM[i]);
    }
    return maxi;
}

//Motion Equation Calculation
inline
void matrixMultiplication(vector< vector<bool> > &adjacent, vector< vector<int> > &outputMatrix, vector< vector<int> > &motionEqMatrix)
{   
    int r1=adjacent.size();
    int c1=adjacent[0].size();
    int c2=outputMatrix[0].size();
    for(int i=0;i<r1;i++)
    {    for(int j=0;j<c2;j++)
        {   motionEqMatrix[i][j]=0;
            for(int k=0;k<c1;k++)
            {
                motionEqMatrix[i][j] -= adjacent[i][k]*outputMatrix[k][j];
            }
        }
    }
}

//OutputMatrix Calculation
inline
void outputMatrixOperation(vector< vector<int> > &outputMatrix, vector< vector<int> > &internalMatrix,int m )
{

    int maxi = maximum(internalMatrix[m]);
    int index=0;
    for(int i=0;i<internalMatrix[0].size();i++)
    {
        if(maxi == internalMatrix[m][i])
         {   index = i;
            break;
         }
    }
    for(int i=0;i<internalMatrix[0].size();i++)
    {
        if(i == index)
            outputMatrix[m][i]=1;
        else
            outputMatrix[m][i]=0;
        
    }

}


//Check Equilibrium Condition for Mth Cluster
inline
bool isEquilibrium( vector<vector<int>> &outputMatrix,vector< vector<int>> &motionEqMatrix,int m)
{  //int maxi = maximum(motionEqMatrix[m]);
   for(int i=0;i<outputMatrix[0].size();i++)
   { 
       if(outputMatrix[m][i]==1 && motionEqMatrix[m][i] == maximum(motionEqMatrix[m]) && maximum(motionEqMatrix[m])>=0)
            return true;
   }
   return false;
}

inline
void update(vector<vector<int>> &internalMatrix,vector<vector<int>> &motionEqMatrix,int m)
{
    int i = rand()%internalMatrix[0].size();
    internalMatrix[m][i] += motionEqMatrix[m][i];
}

inline
void printMatrix(vector<vector<int> > &M)
{
    for(int i=0;i<M.size();i++)
    {
        for(int j=0;j<M[0].size();j++)
        {
            cout<<setw(5)<<M[i][j]<<" ";
        }
        cout<<endl;
    }
}

inline
void printMatrix(vector<vector<bool> > &M)
{
    for(int i=0;i<M.size();i++)
    {
        for(int j=0;j<M[0].size();j++)
        {
            cout<<setw(5)<<M[i][j]<<" ";
        }
        cout<<endl;
    }
}


inline
void solution(vector< vector<bool> > &adjacent,vector< vector<int> > &internalMatrix,vector< vector<int> > &outputMatrix,
        vector< vector<int> > &motionEqMatrix,ofstream &resultStream , char* str)
{

    //Step 2
    for(int i=0;i<outputMatrix.size();i++)
        outputMatrixOperation(outputMatrix,internalMatrix,i);
    //
    // cout<<"initial output"<<endl;
    // printMatrix(outputMatrix);


    //Step 3 
    matrixMultiplication(adjacent,outputMatrix,motionEqMatrix);

    //
    //  cout<<"intial motion"<<endl;
    // printMatrix(motionEqMatrix);

   //Step 4
   for(int i=0;i<outputMatrix.size();i++)
   {
       bool equilibriumM = false;
       while(equilibriumM==false)
       {
           if(isEquilibrium(outputMatrix,motionEqMatrix,i))
               { equilibriumM = true;} //cout<<"\n!!!!Equilibrium!!!! "<<i<<endl;}
            else
            {   
                 clock_t runFinish = clock();
                 int duration = (int)(((runFinish-runStart)/double(CLOCKS_PER_SEC))*1000);
                 if(duration >= 3600000)
                 { 
                     
                     resultStream<<"*******************************"<<str<<"*************************************************"<<"\n"; 
                     resultStream<<"\t\t Time Exceeded more than 3600000ms "<<endl<<endl;
                     resultStream.close();
                     exit(1);   
                 }
                //cout<<"*************"<<i<<"***************"<<endl; 
                update(internalMatrix,motionEqMatrix,i);
                // cout<<"Internal Matrix"<<endl;
                // printMatrix(internalMatrix);

                outputMatrixOperation(outputMatrix,internalMatrix,i);
                // cout<<"Output matrix"<<i<<endl;
                // printMatrix(outputMatrix);


                matrixMultiplication(adjacent,outputMatrix,motionEqMatrix);
                // cout<<"Motion Matrix"<<endl;
                // printMatrix(motionEqMatrix);
            }
            
       }
       //cout<<"Exit from while "<<i<<endl;

   }
   //cout<<"Exit from for loop"<<endl;

}


int main(int argc, char ** argv)
{
    if(argc<=1)
    {
        cout<<"Hopfield Network for Graph Colouring\n\n"
            <<"<InputFile> (DIMACS format file required)\n";
        exit(1);
    }
    else
    {
        cout<<"Graph colouring using Hopfield Network <"<<argv[1]<<">."<<endl;   
     }
   
    int i,numNodes,numEdges=0;
    vector<int> degree;
    vector< vector<int> > adjList;
    vector< vector<bool> > adjacent;

  

    ifstream inStream;
    inStream.open(argv[1]);
    readInputFile(inStream,numNodes,numEdges,adjacent,degree,adjList);

    for(int i=0;i<adjacent.size();i++)
    {
        adjacent[i][i]=false;
    }

  
    int k = maximum(degree)+1;
    //int k=atoi(argv[2]);
    vector< vector<int> > internalMatrix(numNodes,vector<int>(k)); //Internal Matrix initialized with random numbers
    vector< vector<int> > outputMatrix(numNodes,vector<int>(k,0)); //
    vector< vector<int> > motionEqMatrix(numNodes,vector<int>(k,0));
   

    
    //srand(time(NULL));

    for(int i=0;i<numNodes;i++)
    {
        for(int j=0;j<k;j++)
        {
            internalMatrix[i][j] = rand()%2000;
        }
    }

    //  cout<<"Initial internal"<<endl;
    //  printMatrix(internalMatrix);

     //Start the timer
    ofstream resultStream("hopfield.txt",ios::app);
    runStart = clock();

    solution(adjacent,internalMatrix,outputMatrix,motionEqMatrix,resultStream,argv[1]);

   // cout<<"Exit from solution function"<<endl;

    clock_t runFinish = clock();

    int duration = (int)(((runFinish-runStart)/double(CLOCKS_PER_SEC))*1000);

    int bestColor = 0;

    for(int j=0;j<outputMatrix[0].size();j++)
    {   int sum_of_jth_col = 0;
        for(int i=0;i<numNodes;i++)
        {
            sum_of_jth_col += outputMatrix[i][j];
        }
        if(sum_of_jth_col>=1)
            bestColor++;
    }

  
    resultStream<<"*******************************"<<argv[1]<<"*************************************************"<<"\n"; 
    resultStream<<" COLS     CPU-TIME(ms)"<<endl;
    resultStream<<setw(5)<<bestColor<<setw(11)<<duration<<"ms\t"<<endl<<endl<<endl;
    resultStream.close();

    adjacent.clear();
    internalMatrix.clear();
    outputMatrix.clear();
    motionEqMatrix.clear();
}

