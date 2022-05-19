//
// Created by CGCL on 2022/5/17.
//

#ifndef THREAD_TEST_LOUVAIN_H
#define THREAD_TEST_LOUVAIN_H

#include <string>
#include <vector>
#include <fstream>

using namespace std;

struct Node {
    int count;        // node number of current cluster int    clsid;
    int clsid;        // the upper cluster id
    int next;         // the next node which belong to the same upper cluster
    int prev;         // the prev node which belong to the same upper cluster
    int first;        // the first child of current community
    int eindex;       // first neighbor index
    double kin;          // current node in weight
    double kout;         // current node out weight
    double clskin;       // the kin value for new community
    double clstot;       // nodes which belong to the same cluster have the same clstot;
};

struct Edge {
    int left;    // left <------ right
    int right;
    int next;    // next neighbor index for node left
    double weight;  // edge weight from right to left
};

class Louvain {
    int clen;
    int elen;
    int nlen;
    int olen;
    int *cindex;
    double sumw;
    Node *nodes;
    Edge *edges;

public:
    void initLouvain(int nodeNum, int edgeNum, const int *offsets, const int *neighbor);
    void readFromFile(const string& filename);
    void initGraph();
    void addNodeToComm(int id, int cid, double weight);
    void removeNodeFromComm(int id, double weight);
    void prtCom();
    int firstStage();
    void secondStage();
    void learnLouvain();
};

void run_louvain();

#endif //THREAD_TEST_LOUVAIN_H
