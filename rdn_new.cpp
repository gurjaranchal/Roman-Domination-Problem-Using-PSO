#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <random>
#include <algorithm>
#include "Heuristic.h"
#include "Graph.h" // Assuming you have a Graph class defined

using namespace std;

class Particle {
public:
    vector<double> position;
    vector<double> velocity;
    vector<double> localBest;
    vector<double> localBestV;

    void init(int size) {
        position.resize(size);
        velocity.resize(size);
        localBest.resize(size);
        localBestV.resize(size);
    }
};

class Swarm {
    Graph graph;
    vector<double> globalBestSolution;
    int globalBestValue;
    vector<Particle> p;
    int t, minIndex, popsize, dim;

public:
    Swarm(int n, int d, const Graph& g) : graph(g) {
        popsize = n;
        dim = d;
        globalBestSolution.resize(d);
        minIndex = -1;
        p.resize(n);
        t = 0;

        for (int i = 0; i < n; i++) {
            p[i].init(d);
        }
    }

    void GenerateInitialSolution() {

        for(int i=0;i<popsize;i++){
            Heuristic H(graph,dim);
            H.findLabelledSet();
            p[i].position = H.getLabelledSet();
        }
        int minObj = INT_MAX;
        for (int i = 0; i < popsize; ++i) {
            p[i].localBest = p[i].position;
            int cost = ObjectiveValue(p[i].position);
            for(int k=0;k<dim;k++)cout<<p[i].position[k]<<" ";cout<<" cost: "<<cost<<endl;
            if(cost<minObj){
                minObj = cost;
                minIndex = i;
            }
        }
        globalBestValue =minObj;
        globalBestSolution = p[minIndex].position;
    }
    int ObjectiveValue(vector<double>& p){
            int sum=0;
            for(int i=0;i<dim;i++){
                sum+= static_cast<int>(p[i]);
            }
            return sum;
    }
     void mainLoop(int iter){
            random_device rd;
            mt19937 generator(rd());
            int gind = minIndex;
            double c1=0.8, c2=0.8, w=0.4;
            for(int it=1;it<iter;it++){
                cout<<"Solution After "<<it<<" iteration\n"<<endl;
                for(int i=0;i<popsize;i++){
                    double r1 = 0.2;
                    double r2 = 0.5;
                    for(int j=0;j<dim;j++){
                            double a=(w * p[i].velocity[j]) + (c1 * r1 * (p[i].localBest[j] - p[i].position[j])) +
                                (c2 * r2 * (globalBestSolution[j] - p[i].position[j]));
                            p[i].velocity[j] = a;
                            p[i].position[j] = p[i].position[j] + (a);
                            p[i].position[j] = SetThreshold(p[i].position[j]);
                        }
                        // cout<<"S"<<i<<": ";for(int k=1;k<=dim;k++)cout<<p[i].position[k]<<" ";cout<<"--- \n";
                        vector<double> l1 = p[i].position,l2=p[i].position;

                        if(!CheckFeasible(p[i].position)){
                            // make fesiable with label 1
                            MakeFeasible1(l1);
                            // make feasible with label 2
                            MakeFeasible2(l2);
                        }
                        // cout<<"l1: ";for(int k=1;k<=dim;k++)cout<<l1[i]<<" ";cout<<endl;
                        // cout<<"l2: ";for(int k=1;k<=dim;k++)cout<<l2[i]<<" ";cout<<endl;
                        int fitnessOld = ObjectiveValue(p[i].localBest);
                        int fitnessNew = fitnessOld;
                        int fitnessl1 = ObjectiveValue(l1),fitnessl2 = ObjectiveValue(l2);

                        if(fitnessl1<=fitnessl2 && fitnessl1<fitnessOld){
                            p[i].position = l1;
                            fitnessNew = fitnessl1;
                        }else if(fitnessl2<fitnessl1 && fitnessl2<fitnessOld){
                            p[i].position = l2;
                            fitnessNew = fitnessl2;
                        }


                        if(fitnessNew < fitnessOld){
                            p[i].localBest = p[i].position;
                            p[i].localBestV = p[i].velocity;
                        }else{
                            p[i].position = p[i].localBest;
                            p[i].velocity = p[i].localBestV;
                        }   
                        
                        cout<<"S"<<i<<": ";
                        for(int k=0;k<dim;k++)cout<<p[i].position[k]<<" ";cout<<" cost: "<<fitnessNew<<"-------- ";

                        cout<<"V"<<i<<": ";
                        for(int k=0;k<dim;k++)cout<<p[i].velocity[k]<<" ";cout<<endl;
                        if(globalBestValue > fitnessNew){
                            cout<<"gbest = "<<fitnessNew<<endl;
                            cout<<"globalBest "<<globalBestValue<<endl;
                            globalBestSolution = p[i].position;
                            globalBestValue = fitnessNew;
                        }
                    }
                    cout<<"---------------------------------------------------------------------------\n";
                }
                cout<<"Optimal Solution After "<<iter<<": ";
                for(int k=0;k<dim;k++)cout<<globalBestSolution[k]<<" ";
                cout<<endl;
                cout<<"gBest = "<<globalBestValue<<endl;
                
        }
        double SetThreshold(double x){
            if(x<1.0) return 0;
            else if(x<2.0)  return 1;
            return 2;
        }
        bool HasNeighbourwithLabel2(int l,vector<double>& p){
            vector<int> getNeigh = graph.neighbors(l);
            for(auto it:getNeigh){
                if(p[it-1]==2)return true;
            }
            return false;
        }
        bool CheckFeasible(vector<double> &p){
            for(int i=0;i<dim;i++){
                if(p[i]==0){
                    bool neigh2 = HasNeighbourwithLabel2(i+1,p);
                    if(!neigh2)return 0; 
                }
            }
            
            return 1;
        }
        void MakeFeasible1(vector<double> &p){
            for(int i=0;i<dim;i++){    
                if(p[i]==0){
                    bool neigh2 = HasNeighbourwithLabel2(i+1,p);
                    if(neigh2)continue;
                    p[i] = 1;
                }    
            }
            if(!CheckFeasible(p)){
                MakeFeasible2(p);
            }      
        }
        void MakeFeasible2(vector<double>& p){
            for(int i=0;i<dim;i++){    
                if(p[i]==0){
                    bool neigh2 = HasNeighbourwithLabel2(i+1,p);
                    if(neigh2)continue;
                    p[i] =2;
                }    
            }      
        }
};

int main() {
    ifstream inputFile("./test.txt");
    if(!inputFile.is_open()){
        cerr << "Error opening the file!" << std::endl;
        return 1; // Return an error code
    }
    int value,t=0,node,edge;
     while (inputFile >> value) {
        t++;
        if(t==1)node = value;
        else if(t==2){
            edge = value; break;
        }
    }
    Graph G;
    while(inputFile >> value){
        int v1 = value;
        inputFile >> value;
        int v2 = value;
        G.addEdge(v1,v2);
    }
    inputFile.close();
    Swarm s(15,node,G);
    s.GenerateInitialSolution();
    s.mainLoop(10);
    return 0;
}



