//
// Created by CGCL on 2022/5/17.
//

#include <cstring>
#include <map>
#include "Louvain.h"
#include <cassert>

void Louvain::readFromFile(const string& filename) {
    ifstream ifs;
    ifs.open(filename, ios::in);
    string s;
    int nodeNum, edgeNum;
    getline(ifs, s);
    getline(ifs, s);
    nodeNum = stoi(s);
    getline(ifs, s);
    edgeNum = stoi(s);
    printf("%d  %d\n", nodeNum, edgeNum);

    int *offsets, *neighbor;
    offsets = (int *) malloc((nodeNum + 1) * sizeof(int));
    neighbor = (int *) malloc(edgeNum * sizeof(int));
    for (int i = 0; i < nodeNum; ++i) {
        getline(ifs, s);
        offsets[i] = stoi(s);
    }
    offsets[nodeNum] = edgeNum;
    for (int i = 0; i < edgeNum; ++i) {
        getline(ifs, s);
        neighbor[i] = stoi(s);
    }
    ifs.close();

    initLouvain(nodeNum, edgeNum, offsets, neighbor);
}

void Louvain::initLouvain(int nodeNum, int edgeNum, const int *offsets, const int *neighbor) {
    clen = nodeNum;
    elen = edgeNum * 2;
    nlen = nodeNum;
    olen = edgeNum;
    cindex = (int *) calloc(clen, sizeof(int));
    nodes = (Node *) calloc(clen, sizeof(Node));
    edges = (Edge *) calloc(elen, sizeof(Edge));
    for (int i = 0; i < clen; i++) {
        nodes[i].eindex = -1;
        cindex[i] = i;
        nodes[i].count = 1;
        nodes[i].kin = 0;
        nodes[i].clskin = 0;
        nodes[i].clsid = i;
        nodes[i].prev = -1;
        nodes[i].first = -1;
        nodes[i].next = -1;
    }
    for (int i = 0; i < nodeNum; ++i) {
        for (int j = offsets[i]; j < offsets[i + 1]; ++j) {
            sumw++;
            nodes[i].kout++;
            nodes[i].clstot++;
            nodes[neighbor[j]].kout++;
            nodes[neighbor[j]].clstot++;

            edges[j*2].left = i;
            edges[j*2].right = neighbor[j];
            edges[j*2].weight = 1;
            edges[j*2].next = nodes[i].eindex;
            nodes[i].eindex = j*2;

            edges[j*2+1].left = neighbor[j];
            edges[j*2+1].right = i;
            edges[j*2+1].weight = 1;
            edges[j*2+1].next = nodes[neighbor[j]].eindex;
            nodes[neighbor[j]].eindex = j*2+1;
        }
    }
}

int Louvain::firstStage() {
    int i, j, ci, cid, ei, wi, wci, cct, idc, maxId, stage_two;
    int *ids = NULL;
    double kv, wei, cwei, maxInWei, deltaQ, maxDeltaQ;
    double *weight = NULL;
    ids = (int *) malloc(nlen * sizeof(int));
    weight = (double *) calloc(nlen, sizeof(double));
    memset(ids, -1, nlen * sizeof(int));
    stage_two = 0;
    while (true) {
        cct = 0;
        for (i = 0; i < clen; i++) {
            ci = cindex[i];
            kv = nodes[ci].kin + nodes[ci].kout;
            cid = nodes[ci].clsid;
            ei = nodes[ci].eindex;
            idc = 0;
            int a, b;
            while (-1 != ei) {
                wi = edges[ei].right;
                wei = edges[ei].weight;
                wci = nodes[wi].clsid;
                weight[wci] += wei;
                ids[idc++] = wci;
                ei = edges[ei].next;
            }
            maxInWei = cwei = maxDeltaQ = 0.0;
            maxId = -1;
            for (j = 0; j < idc; j++)
                if (weight[ids[j]] > 0.0) {
                    if (cid == ids[j]) {
                        deltaQ = weight[ids[j]] - kv * (nodes[ids[j]].clstot - kv) / sumw;
                        cwei = weight[ids[j]];
                    }
                    else
                        deltaQ = weight[ids[j]] - kv * nodes[ids[j]].clstot / sumw;
                    if (deltaQ > maxDeltaQ) {
                        maxDeltaQ = deltaQ;
                        maxId = ids[j];
                        maxInWei = weight[ids[j]];
                    }
                    weight[ids[j]] = 0.0;
                }
            if (maxDeltaQ > 0.0 && maxId != cid) {
                if (maxId == -1) {
//                    fprintf(stderr, "this can not be, something must be wrong\n");
                    return 0;
                }
                removeNodeFromComm(ci, cwei);
                addNodeToComm(ci, maxId, maxInWei);
                cct += 1;
                stage_two = 1;
            }
        }
//        fprintf(stderr, "    one iteration inner first stage, changed nodes : %d\n", cct);
        if (cct == 0) {
            break;
        }
    }
    free(ids);
    ids = NULL;
    free(weight);
    weight = NULL;
    return stage_two;
}

//新插入的点id接到社区cid的next头部
void Louvain::addNodeToComm(int id, int cid, double weight) {
    nodes[id].clsid = cid;
    nodes[id].next = nodes[cid].next;
    nodes[cid].next = id;
    nodes[id].prev = cid;
    if (nodes[id].next != -1) {
        nodes[nodes[id].next].prev = id;
    }
    nodes[cid].count += nodes[id].count;
    nodes[cid].clstot += nodes[id].clstot;
    nodes[cid].clskin += nodes[id].kin + 2 * weight;
}

void Louvain::removeNodeFromComm(int id, double weight) {
    int cid = nodes[id].clsid;
    int prev, next;
    if (cid != id) {
        prev = nodes[id].prev;
        next = nodes[id].next;
        nodes[prev].next = next;
        if (next != -1) {
            nodes[next].prev = prev;
        }
        nodes[cid].count -= nodes[id].count;
        nodes[cid].clstot -= nodes[id].clstot;
        nodes[cid].clskin -= nodes[id].kin + 2 * weight;
    } else {
        next = nodes[id].next; // the new center of the community
        cid = next;                // cid , new center
        if (-1 != next) {
            nodes[next].prev = -1;
            nodes[next].clsid = next;
            while (-1 != (next = nodes[next].next)) {
                nodes[cid].count += nodes[next].count;
                nodes[next].clsid = cid;
            }
            nodes[cid].clstot = nodes[id].clstot - nodes[id].kin - nodes[id].kout;
            nodes[cid].clskin = nodes[id].clskin - nodes[id].kin - 2 * weight;
            nodes[id].count -= nodes[cid].count;
            nodes[id].clskin = nodes[id].kin;
            nodes[id].clstot -= nodes[cid].clstot;
        }
    }
}

void Louvain::checkLengh(int cid)
{
    int first = nodes[cid].first;
    int fcnt = 1;
    while (first != -1) {
        fcnt++;
        first = nodes[first].next;
    }
    first = nodes[cid].next;
    int ncnt = 1;
    while (first != -1) {
        ncnt++;
        first = nodes[first].next;
    }
    printf("\ncheckLengh:  first: %d  next: %d  count: %d  clsid: %d\n", fcnt, ncnt, nodes[1672].count,
           nodes[1672].clsid);
}

void Louvain::secondStage() {
    int i, ci, next, first, tclen = 0;
    int l, r, telen = 0, lcid, rcid;
    double w;
    for (i = 0; i < clen; i++) {
        ci = cindex[i];
        if (nodes[ci].clsid == ci) {
            cindex[tclen++] = ci;
            //把next链 接到first链后面
            next = nodes[ci].next;
            first = nodes[ci].first;
            if (first != -1) {
                while (-1 != (nodes[first].next))
                    first = nodes[first].next;
                nodes[first].next = next;
            }
            else
                nodes[ci].first = next;
            if (next != -1)
                nodes[next].prev = first;
            nodes[ci].next = -1;
            nodes[ci].prev = -1;
        }
    }
    clen = tclen;
    for (i = 0; i < clen; i++) {
        ci = cindex[i];
        nodes[ci].kin = nodes[ci].clskin;
        nodes[ci].kout = nodes[ci].clstot - nodes[ci].kin;
        nodes[ci].eindex = -1;
    }
    for (i = 0; i < elen; i++) {
        l = edges[i].left;
        r = edges[i].right;
        w = edges[i].weight;
        lcid = nodes[l].clsid;
        rcid = nodes[r].clsid;
        if (lcid != rcid) {
            edges[telen].left = lcid;
            edges[telen].right = rcid;
            edges[telen].weight = w;
            edges[telen].next = nodes[lcid].eindex;
            nodes[lcid].eindex = telen++;
        }
    }
    elen = telen;
}

void Louvain::learnLouvain() {
    readFromFile("../datasets/Gnutella05.txt");
//    initGraph();
    while (firstStage()){
        secondStage();
        fprintf(stderr, "community count: %d after one pass\n", clen);
    }
    prtCom();
}

void Louvain::initGraph() {
    int offset[] = {0, 3, 4, 7, 9, 10, 11, 13, 14, 16, 17, 18, 19};
    int neighbor[] = {4, 8, 10, 7, 5, 9, 11, 0, 7, 3, 9, 2, 10, 11, 6, 10, 1, 3, 1};
//    int offset[] = {0, 0, 1, 2, 3, 4, 7, 7, 8, 8};
//    int neighbor[] = {4, 4, 4, 5, 6, 7, 0, 8};
    int nodeNum = 12;
    int edgeNum = 19;
//    int nodeNum = 9;
//    int edgeNum = 8;
    initLouvain(nodeNum, edgeNum, offset, neighbor);
}

void Louvain::prtCom() {
    for (int i = 0; i < nlen; ++i) {
        int clsid = nodes[i].clsid;
        while (nodes[clsid].clsid != clsid)
            clsid = nodes[clsid].clsid;
        nodes[i].clsid = clsid;
    }
    map<int, int> com_cnt;
    int sum = 0;
    for (int i = 0; i < clen; ++i) {
        com_cnt[nodes[cindex[i]].count]++;
    }
    for (auto ea:com_cnt) {
        sum += ea.first * ea.second;
        printf("%d : %d\n", ea.first, ea.second);
    }
    printf("%d\n", sum);
}

void run_louvain()
{
    Louvain louvain{};
    louvain.learnLouvain();
}