#include "Graph.h"

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <map>
#include <utility>
using std::ifstream;
using std::ofstream;
using std::istream;
using std::ostream;
using std::string;
using std::map;
using std::pair;



vector<string> split(const string &s, const string &delim) {
	vector<string> res;
	string::size_type front = 0;
	string::size_type last = s.find_first_of(delim, front);
	while (last != string::npos) {
		if (last > front) {
			string temp = s.substr(front, last - front);
			res.push_back(temp);
		}
		front = last + 1;
		last = s.find_first_of(delim, front);
	}
	if (last > front) {
		res.push_back(s.substr(front, last - front));
	}
	return res;
}

auto formatData(string filePath) {
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

template<typename T>
void print(const vector<T> &v) {
	for (auto i : v) {
		cout << i << " - ";
	}
	cout << endl;
}
vector<int> initialState(int totalNum, int index) {
	vector<int> initial(totalNum, 0);
	initial[index] = 1;
	return initial;
}

bool ensure(double ratio) {
	int accuracy = 1000;
	int guess = rand() % accuracy;
	double guess_format = guess*1.0 / accuracy;

	if (guess_format <= ratio) {
	
		return true;
	}
	else {
	
		return false;
	}
}

vector<int> infectedfun(vector<int> &state, double infected_ratio, const Graph& graph) {  
	vector<int> ret;
	for (int index = 0; index < state.size(); ++index) {
		int node_state = state[index];
		if (!node_state) {
		
			auto neighbors = graph.Vertex[index].edges;
		for (auto neighbor : neighbors) {   
			if (state[neighbor] == 1) {      
				if (ensure(infected_ratio)) node_state = 1;  
				}
			}
		}
		ret.push_back(node_state);
	}
	return ret;
}

vector<int> recoveryfun(double recovery_ratio, Graph& graph) {
	vector<int> ret;
	for (int index = 0; index < graph.GetVexNum(); ++index) {
		int node_state = graph.Vertex[index].state;
	if (node_state) {     
			if (ensure(recovery_ratio)) {
				node_state = 0;
			} 
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
struct renametemp {
	bool operator()(const pair<int, double> & lhs, const pair<int, double> & rhs)
	{
		return lhs.second > rhs.second;
	}
};

vector<double> runSIS(double infected, double recovery_ratio, int iterator_num, int step_num, Graph& graph) {
	int vertexNum = graph.GetVexNum();
	int edgesNum = graph.GetEdgeNum();

vector<double> ret(vertexNum, 0); 
vector<vector<double>> scores; 

	for (int i = 0; i < iterator_num; ++i) {

		cout << endl;
		cout << i << endl;
		cout << endl;

	vector<double> score; 
							
		for (int index = 0; index < vertexNum; ++index) {
		int infected_index = index; 
			auto initial_state = initialState(vertexNum, infected_index);
			print(initial_state);
			update(initial_state, graph);

			vector<int> recovery_state(vertexNum, 0);
			vector<int> infected_state(vertexNum, 0);
		int infectedNum = 0; 
			for (int j = 0; j < step_num; ++j) {
				recovery_state = recoveryfun(recovery_ratio, graph);
				print(recovery_state);
				update(recovery_state, graph);
				infected_state = infectedfun(recovery_state, infected, graph);
				print(infected_state);
				update(infected_state, graph);
				infectedNum = count(infected_state.begin(), infected_state.end(), 1);
				cout << "Infected " << infectedNum << endl;
			}
			score.push_back(infectedNum);
		}
		for (auto i : score) cout << i << endl;
	scores.push_back(score);   
	}

	for (int iter = 0; iter < iterator_num; ++iter){
		for (int index = 0; index < vertexNum; ++index) {
			auto iter_socre = scores[iter][index];
			ret[index] += iter_socre;
			cout << iter_socre << " ";
		}
		cout << endl;
	}

for (auto &s : ret) {   
		s /= iterator_num;
	}
	return ret;
}

void ranking(vector<double>& index_value, vector<pair<int, double>> &sorted_index) {
	map<int, double> temp;
	int nodeID = 0;
	for (auto value : index_value) {
		temp.insert({ nodeID++, value });
	
	}
	for (auto i : temp) {
		sorted_index.push_back(i);
	}
	sort(sorted_index.begin(), sorted_index.end(), renametemp());
}

void formatOutput(vector<pair<int, double>>& sorted_index, const string& filePath) {
	ofstream output(filePath);
	for (auto ele : sorted_index) {
		cout << ele.first << "\t" << ele.second << endl;
		output << ele.first << "\t" << ele.second << endl;
	}
}

int main() {

	string filePath("data/test.txt");
	auto data = formatData(filePath);
	Graph graph = graphNew(data);
	double infected = 0.6;
	double recovered = 0.1;
	int iterator_num = 50; 
	int step_num = 5;
	auto result = runSIS(infected, recovered, iterator_num, step_num, graph);
	print(result);
	vector<pair<int, double>> rank;
	ranking(result, rank);
	formatOutput(rank, "./result/sis.txt");
	return 0;
}
