/*
Reference implementation of HRLHG.
Author: Yue Yin
*/
#pragma warning(disable : 4996)
#include<cstdio>
#include<math.h>
#include<vector>
#include<cstring>
#include<queue>
#include<map>
#include<set>
#include<iostream>
#include<time.h>
#include<stdlib.h>
using namespace std;

string dirpath="test"; // dir path of training data
string outputpath="output_im1.txt";

// ID of nodes, edge types and node types are 1-based
const double INF = 1e10;
const int MAXN = 300000; // max ID of node
const int PTYPE = 5; // max number of node types
const int ETYPE = 11; // max number of edge types
const int K = 2; // k-shortest paths
int n; // number of nodes
int m; // number of edges
int ptype; // node type
int etype; // edge type
int labelcount;
double p[PTYPE][ETYPE]; // p[i][j] means the probability of choosing edge j at node i
double increment[PTYPE][ETYPE]; // increment of weight according to k-shortest path
double decrease[K];

map<int, int> itertype; // <node id, node type>
vector<pair<int, int> > label;

vector<pair<int, int> > shortest[K]; // k shortest paths for each label pair, the pair is <node type, chosen edge type>

const int max_iter = 10;
const double iter_eps = 1e-4;
const double db_eps = 1e-8;

int increment_method; // 1-4 represent "raw count/normalized count/log decrease/normalized log decrease" respectively
double stop_ratio; // stop training when there is more than stop_ratio pairs share the same paths during the two iteration
double dumping_factor;

map<pair<int, int>, vector<vector<pair<int,int> > > > current_shortest; // memorize the current k-shortest paths for each label pair
map<pair<int, int>, vector<vector<pair<int,int> > > > last_shortest; // memorize the last k-shortest paths for each label pair for stopping training judge

int degree[MAXN]; // the degree for nodes

// The algorithm for k-shortest paths is Dijkstra with min-heap

struct qnode // for min heap
{
	int v;
	double c;
	qnode(int _v = 0, double _c = 0) :v(_v), c(_c){}
	bool operator <(const qnode &r)const
	{
		return c>r.c;
	}
};
struct Edge
{
	int v;
	double cost;
	double original_cost;
	int edgetype;
	Edge(int _v = 0, int _type = 0, double _cost = INF) :v(_v), edgetype(_type), cost(_cost), original_cost(_cost){}
};
vector<Edge>E[MAXN]; // graph storage with adjacency list

bool vis[MAXN];
double dist[K][MAXN];
pair<int, int> parent[K][MAXN]; // node u is the node before v in the k-th shortest path, parent[k][v] = <u, u's index in adjacency list of node v>

void init()
{
	itertype.clear();
	memset(p, 0, sizeof(p));
	label.clear();
	for(int i=0;i<K;i++)
        decrease[i]=1.0/log(2+i);
}

void addedge(int u, int v, int t, double c = INF)
{
	E[u].push_back(Edge(v, t, c));
}

void input()
{
	int u, v, st, ed, ty;
	double cost;

	string graphdata=dirpath+"/graph.dat";
	string edgedata=dirpath+"/edge.dat";
	string nodedata=dirpath+"/node.dat";
	string labeldata=dirpath+"/label.dat";

	freopen(graphdata.c_str(), "r", stdin);
	scanf("%d,%d", &n, &m);

	ptype = PTYPE - 1;
	etype = ETYPE - 1;

	freopen(edgedata.c_str(), "r", stdin);

	for (int i = 0; i<m; i++)
	{
	    double weight;
		scanf("%d,%d,%d,%lf", &u, &v, &ty, &weight); // start node, end node, edge type, edge weight
		if(weight>db_eps)
        {
            cost=1.0/weight;
            addedge(u, v, ty, cost);
            addedge(v, u, ty, cost);
        }
        else
        {
            addedge(u, v, ty, INF);
            addedge(v, u, ty, INF);
        }
	}

	// <node id, node type>
	freopen(nodedata.c_str(), "r", stdin);
	int temp1, temp2;
	for (int i = 0; i<n; i++)
	{
		scanf("%d,%d", &temp1, &temp2);
		itertype[temp1] = temp2;
	}

	freopen(labeldata.c_str(), "r", stdin);
	while (~scanf("%d,%d", &st, &ed))
	{
		label.push_back(make_pair(st,ed));
	}
}

void init_prob()
{
	for (int i = 1; i <= MAXN; i++)
		degree[i] = E[i].size();

	for (int i = 1; i <= MAXN; i++)
	{
		for (int j = 0; j<E[i].size(); j++)
		{
            if(p[itertype[i]][E[i][j].edgetype] < db_eps)
                p[itertype[i]][E[i][j].edgetype]++;
		}
	}

	for (int i = 1; i <= ptype; i++)
	{
		double pi = 0;
		for (int j = 1; j <= etype; j++)
			pi += p[i][j];

		if (fabs(pi)>db_eps)
		{
			for (int j = 1; j <= etype; j++)
			{
				if (fabs(p[i][j])>db_eps)
					p[i][j] = p[i][j] / pi;
			}
		}
	}
}

void getpath(int st, int ed, int k) // memorize the edges between start node and end node
{
	vector<pair<int, int> > temppoint;
	vector<pair<int, int> > point;

	temppoint.push_back(make_pair(ed, -1));

	pair<int, int> temp = parent[k][ed];
	while (temp.first != -1)
	{
		temppoint.push_back(temp);
		temp = parent[k][temp.first];
	}

	for (int i = temppoint.size() - 1; i >= 0; i--)
	{
		point.push_back(temppoint[i]);
	}

	int pre, cur;
	pre = cur = 0;
	if (point.size() >= 2) // otherwise there is no path
	{
		for (int i = 1; i<point.size(); i++)
		{
			pre = point[i - 1].first;
			cur = point[i].first;
			int j = point[i - 1].second;

			double shortestcost = E[pre][j].cost;
			int shortesttype = E[pre][j].edgetype;

			shortest[k].push_back(make_pair(itertype[pre], shortesttype));
		}
	}
}

void dijkstra(int st, int ed, int k)
{
	bool vis[MAXN];
	memset(vis, false, sizeof(vis));
	for (int j = 0; j < MAXN; j++)
	{
		dist[k][j] = INF;
		parent[k][j] = make_pair(-1, -1);
	}

	priority_queue<qnode>que;
	while (!que.empty())que.pop();

	dist[k][st] = 0.0;

	que.push(qnode(st, 0));
	qnode tmp;
	while (!que.empty())
	{
		tmp = que.top();
		que.pop();
		int u = tmp.v;
		if (vis[u])
			continue;
		vis[u] = true;
		for (int i = 0; i<E[u].size(); i++) // E[u]: all edges started at u
        {
			int v = E[u][i].v;
			double cost = E[u][i].cost + tmp.c;

			if (!vis[v] && (cost<dist[k][v] - db_eps))
			{
				dist[k][v] = cost;
				que.push(qnode(v, dist[k][v]));
				parent[k][v] = make_pair(u, i);
			}
		}
	}
}

void shortests(int st, int ed)
{
	dijkstra(st, ed, 0);
	getpath(st, ed, 0);

	// enumerate the deleted path in shortest k1 and try to get shortest k2

	int ru, rv, rj; // record the deleted path info
	ru = rv = rj = 0;
	double secondshort = INF;

	vector<pair<int, int> > temppoint;
	vector<pair<int, int> > point;
	temppoint.push_back(make_pair(ed, -1));
	pair<int, int> temp = parent[0][ed];

	while (temp.first != -1)
	{
		temppoint.push_back(temp);
		temp = parent[0][temp.first];
	}
	for (int i = temppoint.size() - 1; i >= 0; i--)
		point.push_back(temppoint[i]);

	int pre, cur, prj;
	pre = cur = prj = 0;
	if (point.size() >= 2) // otherwise there is no path
	{
		for (int i = 1; i<point.size(); i++)
		{
			pre = point[i - 1].first;
			cur = point[i].first;
			prj = point[i - 1].second;
			E[pre][prj].v = -1; // deleted

			dijkstra(st, ed, 1);

			if (dist[1][ed]<secondshort)
			{
				secondshort = dist[1][ed];
				ru = pre;
				rv = cur;
				rj = prj;
			}

			E[pre][prj].v = cur; // get back for next enumeration
        }
	}

    if(ru!=0 && rv!=0)
    {
        // delete the chosen path
        int v = E[ru][rj].v;
        int t = E[ru][rj].edgetype;
        double w = E[ru][rj].cost;

        E[ru][rj].v = -1;

        dijkstra(st, ed, 1);
        getpath(st, ed, 1);

        // get back
        E[ru][rj].v = rv;
    }
}

void getincrement(int c)
{
    if(c==1) //raw count
    {
        for (int k = 0; k<K; k++)
        {
            for (int i = 0; i<shortest[k].size(); i++)
            {
                pair<int, int> temp = shortest[k][i];
                increment[temp.first][temp.second] += 1;
            }
        }

    }
    else if(c==2) // normalize count
    {
        for (int k = 0; k<K; k++)
        {
            for (int i = 0; i<shortest[k].size(); i++)
            {
                pair<int, int> temp = shortest[k][i];
                increment[temp.first][temp.second] += 1;
            }
            if(shortest[k].size() > 0)
            {
                for (int i = 1; i <= ptype; i++)
                {
                    for (int j = 1; j <= etype; j++)
                    {
                        increment[i][j] = increment[i][j] / shortest[k].size();
                    }
                }
            }
        }
    }
    else if(c==3) //log decrease
    {
        for (int k = 0; k<K; k++)
        {
            for (int i = 0; i<shortest[k].size(); i++)
            {
                pair<int, int> temp = shortest[k][i];
                increment[temp.first][temp.second] += decrease[k];
            }
        }
    }
    else if(c==4) // normalize and log decrease
    {
        for (int k = 0; k<K; k++)
        {
            for (int i = 0; i<shortest[k].size(); i++)
            {
                pair<int, int> temp = shortest[k][i];
                increment[temp.first][temp.second] += decrease[k];
            }
            if(shortest[k].size() > 0)
            {
                for (int i = 1; i <= ptype; i++)
                {
                    for (int j = 1; j <= etype; j++)
                    {
                        increment[i][j] = increment[i][j] / shortest[k].size();
                    }
                }
            }
        }
    }
    else
    {
        printf("Increment chose error.");
    }
}

void update()
{
	for (int i = 1; i <= ptype; i++)
    {
        double eitype=0;
        for(int j=1;j<=etype;j++)
        {
            if(p[i][j]>db_eps)
                eitype+=1;
        }
        for (int j = 1; j <= etype; j++)
        {
            if(eitype>db_eps)
                increment[i][j] = increment[i][j] / eitype;
        }
    }

	for (int i = 1; i <= ptype; i++)
        for (int j = 1; j <= etype; j++)
            p[i][j] += increment[i][j];

	// normalization
	for (int i = 1; i <= ptype; i++)
	{
		double itmp = 0;
		for (int j = 1; j <= etype; j++)
			itmp += p[i][j];
		if (itmp>0)
		{
			for (int j = 1; j <= etype; j++)
				p[i][j] = p[i][j] / itmp;
		}
	}

    // dumping factor
	for (int i = 1; i <= ptype; i++)
	{
        double eitype=0;
        for(int j=1;j<=etype;j++)
        {
            if(p[i][j]>db_eps)
                eitype+=1;
        }
		if (eitype>0)
		{
            for (int j = 1; j <= etype; j++)
            {
                if(p[i][j]>db_eps)
                    p[i][j] = p[i][j]*dumping_factor + (1-dumping_factor)/eitype;
			}
		}
	}

    // normalization after dumping factor
	for (int i = 1; i <= ptype; i++)
	{
		double itmp = 0;
		for (int j = 1; j <= etype; j++)
			itmp += p[i][j];
		if (itmp>0)
		{
			for (int j = 1; j <= etype; j++)
				p[i][j] = p[i][j] / itmp;
		}
	}

	// updating weight
	for (int i = 1; i <= MAXN; i++)
	{
		for (int j = 0; j<E[i].size(); j++)
		{
			double ctemp = 0;
			ctemp = p[itertype[i]][E[i][j].edgetype];
			E[i][j].cost = E[i][j].original_cost / ctemp;
		}
	}
}

bool shortestequal(vector<vector<pair<int,int> > >last, vector<vector<pair<int,int> > > current)
{
    if(last.size()!=current.size())
        return false;
    if(last.size()!=K)
        return false;
    for(int k=0;k<K;k++)
    {
        vector<pair<int,int> > l=last[k];
        vector<pair<int,int> > c=current[k];
        if(l.size()!=c.size())
            return false;
        else
        {
            for(int i=0;i<l.size();i++)
            {
                if(l[i]!=c[i])
                    return false;
            }
        }
    }
    return true;
}

int main(int argc,char **argv)
{
    dirpath = string(argv[1]);
    string im = string(argv[2]);
    outputpath = string(argv[3]);
    string sr = string(argv[4]);
    string df = string(argv[5]);

    increment_method = atoi(im.c_str());
    stop_ratio = atof(sr.c_str());
    dumping_factor = atof(df.c_str());

	freopen(outputpath.c_str(),"w",stdout);
	init();
	input();

	init_prob();
	int itertime = 0;

	while (itertime<max_iter)
	{
		memset(increment, 0, sizeof(increment));

		int labcnt=0;

        int shortestequalcount=0;
		for (int iter = 0; iter < label.size(); iter++)
		{
			int st = label[iter].first;
			int ed = label[iter].second;

			pair<int,int> st_ed=make_pair(st,ed);

            for(int k=0;k<K;k++)
                shortest[k].clear();

            vector<vector<pair<int,int> > > cs;
            if(current_shortest.count(st_ed))
                cs=current_shortest[st_ed];
            last_shortest[st_ed]=cs;

			shortests(st, ed);
			vector<vector<pair<int,int> > > tempshortest;
			for(int k=0;k<K;k++)
                tempshortest.push_back(shortest[k]);

			current_shortest[st_ed]=tempshortest;

			getincrement(increment_method);

            if(last_shortest.count(st_ed)&&current_shortest.count(st_ed))
            {
                if(shortestequal(last_shortest[st_ed], current_shortest[st_ed]))
                {
                    shortestequalcount++;
                }
            }
		}
		double equalratio = (double)shortestequalcount/(double)label.size();

		update();

        if(equalratio>stop_ratio)
            break;

		itertime++;
	}
    for (int i = 1; i <= ptype; i++)
	{
		for (int j = 1; j <= etype; j++)
        {
			printf("%f ", p[i][j]);
        }
		printf("\n");
	}
	return 0;
}
