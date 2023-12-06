#include "Graph.h"

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <map>
#include <utility>
#include <random>
#include <cmath>
#include <stdlib.h>
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
using std::string;
using std::map;
using std::pair;
using std::default_random_engine;
using std::uniform_real_distribution;



vector<int> formatData(string filePath) {
	vector<int> edges;
	ifstream input(filePath);
	if (input) {
		string line;
		while (getline(input, line)) {
			auto from = split(line, "\t")[0];
			edges.push_back(stoi(from));
			auto to = split(line, "\t")[1];
			edges.push_back(stoi(to));
		}
	}
	return edges;
}

vector<string> split(const string &s, const string &delim) {
	vector<string> res;
	string::size_type front = 0;
	string::size_type last = s.find_first_of(delim, front);
	while (last != string::npos) {
		if (last > front) {
			string tmp = s.substr(front, last - front);
			res.push_back(tmp);
		}
		front = last + 1;
		last = s.find_first_of(delim, front);
	}
	if (last > front) {
		res.push_back(s.substr(front, last - front));
	}
	return res;
}

Graph graphNew(vector<int> edges) {
	Graph G;
	auto max = max_element(edges.begin(), edges.end());
	auto vertexNum = *max + 1;
	vector<Node> v(vertexNum);
	for (int i = 0; i < vertexNum; ++i) {
		v[i].data = i;
		G.InsertVertex(v[i]);
	}
	for (int i = 0, j = 1; j < edges.size(); i += 2, j += 2) {
		G.AddEdge(edges[i], edges[j]);
		G.AddEdge(edges[j], edges[i]);  
	}
	return G;
}

void testInfo(const string& info) {
	cout << "-----" << info << "-----" << endl;
}

template<typename T>
void print(const vector<T> &v) {
	for (auto i : v) {
		cout << i << " - ";
	}
	cout << endl;
}

void formatOutput(vector<pair<int, double>>& sorted_index, const string& filePath) {
	ofstream output(filePath);
	for (auto ele : sorted_index) {
		cout << ele.first << "\t" << ele.second << endl;
		output << ele.first << "\t" << ele.second << endl;
	}
}

template<typename T>
void formatOutput(vector<T>& data, const string& filePath){
    ofstream output(filePath);
    int size = data.size();
    for(int index = 0;  index < size; ++index){
        output << index << "\t" << data[index] << endl;
    }
}

template<typename T, typename N>
void formatOutput(vector<T>& data, vector<N>& other, const string& filePath) {
	ofstream output(filePath);
	int size = data.size();
	for (int index = 0; index < size; ++index) {
		output << index << "\t" << data[index] << "\t" << other[index] << endl;
	}
}

vector<int> initialState(int totalNum, int index) {
	vector<int> initial(totalNum, 0);
	initial[index] = 1;
	return initial;
}

vector<int> getStateFromGraph(Graph &graph) {
	vector<int> ret;
	for (int index = 0; index < graph.Vertex.size(); ++index) {
		ret.push_back(graph.Vertex[index].state);
	}
	return ret;
}

bool ensure(double ratio){
    static default_random_engine e;
    static uniform_real_distribution<double> u(0,1);
    double guess = u(e);
    if(guess <= ratio){
        return true;
    } else {
        return false;
    }
}

vector<int> recoveryProcess(double recovery_ratio, Graph& graph) {
	vector<int> ret;
	for (int index = 0; index < graph.GetVexNum(); ++index) {
		int node_state = graph.Vertex[index].state;
		if (node_state == 1) {    
			if (ensure(recovery_ratio)) {
				node_state = 2;
			}
		}
		ret.push_back(node_state);
	}
	return ret;
}

vector<int> infectedProcess(double infected_ratio, const Graph& graph) {
	vector<int> ret;
	int size = graph.Vertex.size();
	for (int index = 0; index < size; ++index) {
		int node_state = graph.Vertex[index].state;
		if (node_state == 0) {  
			auto neighbors = graph.Vertex[index].edges;
			}
            int infected_neighbor_num = count(neighbors.begin(), neighbors.end(), 1);
            double infected_probability = 1 - pow(1 - infected_ratio, infected_neighbor_num);
            if(ensure(infected_probability)) node_state = 1;
		}
		ret.push_back(node_state);
	}
	return ret;
}

void update(vector<int> &new_state, Graph& graph) {
	for (int index = 0; index < new_state.size(); ++index) {
		graph.Vertex[index].state = new_state[index];
	}
}


void update(vector<int> &initial_state, vector<int> &infected_state, vector<int> &recovery_state, Graph &graph) {
	for (int index = 0; index < initial_state.size(); ++index) {
		if (initial_state[index] == 0) {	
			graph.Vertex[index].state = infected_state[index];
		}
		else if (initial_state[index] == 1) {
			graph.Vertex[index].state = recovery_state[index];	
		}
		else {
			graph.Vertex[index].state = 2;		
		}
	}
}


bool isEnd(vector<int> &state) {
	for (auto s : state) {
		if (s == 1) {
			return false;
		}
	}
	return true;
}


vector<double> runSIR(double infected, double recovery_ratio, int iterator_num, Graph& graph) {
	int vertexNum = graph.GetVexNum();
	int edgesNum = graph.GetEdgeNum();

	vector<double> ret(vertexNum, 0);
	vector<vector<double>> scores;

	for (int iter = 0; iter < iterator_num; ++iter) {

		cout << endl;
		cout << "#iterator " << iter << endl;
		cout << endl;

							 
		vector<int> infected_step(vertexNum, 0);  
		for (int index = 0; index < vertexNum; ++index) {
			int infected_index = index; 
			auto initial_state = initialState(vertexNum, infected_index);
			update(initial_state, graph); 

			vector<int> recovery_state(vertexNum, 0);
			vector<int> infected_state(vertexNum, 0);
			int infectedNum = 0; 
			int recoveryNum = 0;
			int step = -1
			do {
				++step;
				initial_state = getStateFromGraph(graph);
				infected_state = infectedProcess(infected, graph);

				recovery_state = recoveryProcess(recovery_ratio, graph);

				update(initial_state, infected_state, recovery_state, graph);

				infectedNum = count(infected_state.begin(), infected_state.end(), 1); 
			} while (infectedNum != 0);
			infected_step.push_back(step);
			recoveryNum = count(recovery_state.begin(), recovery_state.end(), 2);
			score.push_back(recoveryNum);
			int degree = graph.Vertex[index].edges.size();
			cout << "#R " << recoveryNum << "   #degree " << degree << "   #step " << step << endl;
		}
        formatOutput(score,infected_step, "./results/score_" + std::to_string(iter));
		for (auto i : score) cout << i << " ";
		scores.push_back(score);  
	}

	for (int iter = 0; iter < iterator_num; ++iter) {
		for (int index = 0; index < vertexNum; ++index) {
			auto iter_socre = scores[iter][index];
			ret[index] += iter_socre;
		}
	}

	for (auto &s : ret) {  
		s /= iterator_num;
	}

	return ret;
}


struct compare {
	bool operator()(const pair<int, double> & lhs, const pair<int, double> & rhs)
	{
		return lhs.second > rhs.second;
	}
};

void ranking(vector<double>& index_value, vector<pair<int, double>> &sorted_index) {
	map<int, double> tmp;
	int nodeID = 0;
	for (auto value : index_value) {
		tmp.insert({ nodeID++, value });
	}
	for (auto i : tmp) {
		sorted_index.push_back(i);
	}
	sort(sorted_index.begin(), sorted_index.end(), compare());
}


int main() {
    cout << "Program start..." << endl;
	string filePath("data/test.txt");
	auto data = formatData(filePath);
	Graph graph = graphNew(data);
	double infected = 0.0105;
	double recovery_ratio = 1;
	int iterator_num = 50;

	auto result = runSIR(infected, recovery_ratio, iterator_num, graph);
	print(result);
	vector<pair<int, double>> rank;
	ranking(result, rank);
	formatOutput(rank, "./results/sir_score.txt");
	cout << "done !" << endl;
	return 0;
}
