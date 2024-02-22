// Heuristic.h
#ifndef HEURISTIC_H
#define HEURISTIC_H
#include <bits/stdc++.h>
#include "Graph.h"
using namespace std;
class Heuristic {
private:
    Graph graph;
    vector<double> labelledSet;
    int n;

public:
    Heuristic(const Graph& g, int size): graph(g), n(size) {
        labelledSet.resize(size);
        for(int i=0;i<size;i++){
            labelledSet[i] = -1;
        }
    }

    void labelNeighbours(int node){
        vector<int> neigh = graph.neighbors(node);
        int n=neigh.size(),j=0;
        for(int i=j;i<n;i++){
            if(labelledSet[neigh[i]]==-1)
                    labelledSet[neigh[i]]=0;
        }    
    }
    bool FindLabelNegative(vector<double>& l){
        for(int i=0;i<n;i++){
            if(l[i]==-1){
                return true;
            }
        }
        return false;
    }
    int FindUnlabelledIndex(vector<double>& l,int r){
        int ind =-1,c=0;
        //count unlabelled Vertex 
        for(int i=0;i<n;i++){
            if(l[i]==-1){
                c++;
            }
        }
        //take mod of r(random value) from remaining unlabeled vertex
        r=(r%c)+1;
        c=0;
        for(int i=0;i<n;i++){
            if(l[i]==-1){
                c++;
            }
            if(c==r)
            return i;
        }
        return ind;
    }
    void findLabelledSet() {
        // srand(time(0)); 
        random_device rd;
        mt19937 gen(rd());
        while(FindLabelNegative(labelledSet)){
            uniform_int_distribution<> distribution(1, n);
            int r = distribution(gen);
            int currVertex = FindUnlabelledIndex(labelledSet,r);
            if(labelledSet[currVertex]==-1){
                labelledSet[currVertex] = 2;
                labelNeighbours(currVertex);   
            }    
        }
    }
    vector<double>& getLabelledSet(){
        return labelledSet;
    }
    void printSolution(vector<double>& l){
        double sum=0;
        for(int i=0;i<n;i++){
            sum+=l[i];
            cout<<l[i]<<" ";
        }
        cout<<"cost: "<<sum<<" ";
        cout<<endl;
    }

    ~ Heuristic(){
        // delete[] labelledSet;

    }

};

#endif // HEURISTIC2_H
