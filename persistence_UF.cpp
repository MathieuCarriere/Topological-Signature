#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <math.h>
#include <string>

#include "union_find.h"

using namespace std;

struct vertex {
    unsigned int index;
    double value;
};

pair<vector<pair<int,int> >, vector<pair<double,double> > >
compute_PD_with_UF(const vector<double> & f, const vector<vector<int> > & neighb){

    int num_points = neighb.size();
    vector<int> neighbors, neighborsp;
    vector<pair<int,int> > pd_ind;
    vector<pair<double,double> > pd_val;
    vector<vertex> verts(num_points);
    vector<pair<int,int> > entries;
    vector<int> v_vect;
    vector<int> parents(num_points);

    for (int j = 0; j < num_points; j++){
        verts[j].index = j; verts[j].value = f[j];}

    sort(verts.begin(), verts.end(), [=](const vertex & a, const vertex & b){return a.value > b.value;});

    for (int j = 0; j < num_points; j++){

            int id = verts[j].index;
            int max_neighb = id;
            parents[id] = id;
            double value = verts[j].value;
            neighbors.clear(); neighborsp.clear();

            // Find neighbors of current node.
            // *******************************

            int num_neighb = neighb[id].size();

            for (int k = 0; k < num_neighb; k++){

                int n = neighb[id][k];
                if (f[n] > f[id])
                    neighbors.push_back(n);

            }

            // If current node is not a peak, attach it to peak with highest value.
            // ********************************************************************

            if (neighbors.size() >= 1){

                int num_neighbors = neighbors.size();

                for (int k = 0; k < num_neighbors; k++){

                    int n = UF_find(neighbors[k], &parents);
                    int pn;

                    for (int k = 0; k < entries.size(); k++)
                        if (entries[k].first == n){
                            pn = v_vect[k];
                            neighborsp.push_back(pn);
                            break;
                        }

                    if (  (pn != max_neighb && f[pn] > f[max_neighb])  ){
                        max_neighb = pn;
                        parents[id] = n;
                    }

                }

                int v = max_neighb;
                int vp = UF_find(v, &parents);

                for (int k = 0; k < entries.size(); k++)
                    if (entries[k].first == vp){
                        entries[k].second += 1;
                        break;
                    }


                // and merge all other cc.
                // ***********************

                for (int k = 0; k < num_neighbors; k++){

                    int n = neighbors[k];
                    int indn, indv;
                    int np = UF_find(n, &parents);
                    int vp = UF_find(v, &parents);

                    for (int k = 0; k < entries.size(); k++)
                        if (entries[k].first == vp){
                            indv = k;
                            break;
                        }

                    for (int k = 0; k < entries.size(); k++)
                        if (entries[k].first == np){
                            indn = k;
                            break;
                        }

                    if (np != vp){

                        pair<int,int> pt1(v_vect[indn],id);
                        pair<double,double> pt2(f[v_vect[indn]],value);
                        pd_ind.push_back(pt1);
                        pd_val.push_back(pt2);

                        UF_union(n, v, v, &entries, &parents, &v_vect);

                    }

                }

            }

            // If current node is a peak, create new cc.
            // *****************************************

            else{

                pair<int,int> new_cc(id,1);
                entries.push_back(new_cc);
                v_vect.push_back(id);
                parents[id] = id;

            }

            if (j == (num_points-1)){
                pair<int,int> pt1(v_vect[0],id);
                pair<double,double> pt2(f[v_vect[0]],value);
                pd_ind.push_back(pt1);
                pd_val.push_back(pt2);
            }

    }

    sort(pd_ind.begin(), pd_ind.end(), [=](const pair<int,int> & a, const pair<int,int> & b){return abs(f[a.first]-f[a.second]) > abs(f[b.first]-f[b.second]);});
    sort(pd_val.begin(), pd_val.end(), [=](const pair<double,double> & a, const pair<double,double> & b){return abs(a.first-a.second) > abs(b.first-b.second);});

    pair<vector<pair<int,int> >, vector<pair<double,double> > > PD;
    PD.first = pd_ind; PD.second = pd_val;
    return PD;

}

int main (int argc, char** argv) {

    vector<double> function; vector<vector<int> > neighbors;

    int tok = atoi(argv[1]);

    int num_points;

    double x; int k = 0;
    string line; getline(cin,line); stringstream streamf(line);
    while(streamf >> x){
        function.push_back(x);
        k++;
    }

    cin >> ws;
    num_points = k;

    for (int i = 0; i < num_points; i++) neighbors.emplace_back();

    k = 0;

    while(getline(cin,line) && k < num_points){
      int n;
      stringstream streamn(line);
      while(streamn >> n)
          neighbors[k].push_back(n);
      cin >> ws;
      k++;
    }

    pair<vector<pair<int,int> >, vector<pair<double,double> > > PD = compute_PD_with_UF(function,neighbors);

    if(tok){
      for(int i = 0; i < PD.second.size(); i++)
        cout << PD.first[i].second << " " << PD.first[i].first << endl;
      }
    else{
      for(int i = 0; i < PD.second.size(); i++){
        if(i==0){
          cout << PD.second[i].second << " inf 0" << endl;
          cout << PD.second[i].first << " inf 2" << endl;
        }
        else
          cout << PD.second[i].second << " " << PD.second[i].first << " 1" << endl;
      }
    }


    return 0;
}
