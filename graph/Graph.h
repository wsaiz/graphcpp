#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
using namespace std;
struct Edge {
    int to;        //�������� �������
    int weight;    //��� �����
    Edge(int to, int weight) : to(to), weight(weight) {}
};
class Graph {
private:
    int numVertices;                             //���-�� ������
    bool directed;                               //���� ����������������/����������������� �����
    vector<vector<Edge>> adjList;                //������ ��������� � ������
    unordered_map<string, int> nameToIndex;      //���-������� ��� ������������� ����� ������� � ��������
    unordered_map<int, string> indexToName;      //���-������� ��� ������������� ������� ������� � ������

public:
    //����������� �� ���������, ������� ������� ������ ����
    Graph(bool directed = false) : numVertices(0), directed(directed) {}

    //����������� ��� �������� ����� �� �����
    Graph(const string& filename) {
        ifstream file(filename);
        if (!file) {
            throw runtime_error("������ �������� ����� ��� ������!");  // ����������� ����������
        }

        string type;
        file >> type;
        directed = (type == "Directed");

        string from, to;
        int weight;
        nameToIndex.clear();
        indexToName.clear();
        adjList.clear();
        numVertices = 0;

        while (file >> from >> to >> weight) {
            addEdge(from, to, weight);
        }

        file.close();
        cout << "���� �������� �� ����� " << filename << endl;
    }
    Graph(const Graph& copy) {
        numVertices = copy.numVertices;
        directed = copy.directed;
        adjList = copy.adjList;  
        nameToIndex = copy.nameToIndex;  
        indexToName = copy.indexToName;  
    }
    bool isDirected() const {
        return directed;
    }
    //����� ��� ���������� �������
    void addVertex(const string& name) {
        if (nameToIndex.find(name) == nameToIndex.end()) { //�������� �� ������������� ������� � ����� �� ������
            nameToIndex[name] = numVertices; //��������� � nameToIndex (������ � ������� ������ ������) ��� ������� � ������� ������� �� �� �����
            indexToName[numVertices] = name; //������ ����������� � ������ ������� 
            adjList.push_back(vector<Edge>()); //��������� ����� ������ ����� ��� �������
            ++numVertices; //����������� ����� ������
        }
    }

    //����� ��� ���������� �����
    void addEdge(const string& from, const string& to, int weight) {
        addVertex(from); //��������� ���������� �� �������, ����� �������� ����� ��������� �����
        addVertex(to);
        int u = nameToIndex[from]; //������� ������� ������ � ���-�������
        int v = nameToIndex[to];
        adjList[u].push_back(Edge(v, weight)); //��������� ����� � ������ ���������

        if (!directed) { //���� ���� �����������������, �� ��������� �������� �����
            adjList[v].push_back(Edge(u, weight));
        }
    }
    //����� ��� �������� �������
    void removeVertex(const string& name) { //�������� �� ������������� �������
        if (nameToIndex.find(name) == nameToIndex.end()) {
            cout << "������� �� �������."<<endl;
            return;
        }

        int index = nameToIndex[name]; //������ ��������� �������

        
        for (auto& edges : adjList) { //������� ��� �����, ������� ������ � ������� �� �������
            edges.erase(remove_if(edges.begin(), edges.end(),
                [index](const Edge& edge) { return edge.to == index; }),
                edges.end());
        }

        adjList.erase(adjList.begin() + index); //������� ������ ��������� ��� ������ �������
       
        for (auto& edges : adjList) { //������������ ��� �������, � ������� ������ ������, ��� � ��������� �������
            for (auto& edge : edges) {
                if (edge.to > index) {
                    --edge.to;  //������� ������ �� -1
                }
            }
        }

        
        unordered_map<string, int> newNameToIndex; 
        unordered_map<int, string> newIndexToName;

        int newIndex = 0;
        for (const auto& pair : indexToName) { //��������� ���-�������
            if (pair.first != index) {  
                newIndexToName[newIndex] = pair.second;
                newNameToIndex[pair.second] = newIndex;
                ++newIndex;
            }
        }

        nameToIndex = move(newNameToIndex); //�������� ������ ���-������� �� ����� 
        indexToName = move(newIndexToName);
        --numVertices;  
    }


    //����� ��� �������� ����� 
    void removeEdge(const string& from, const string& to) {
        if (nameToIndex.find(from) == nameToIndex.end() || nameToIndex.find(to) == nameToIndex.end()) { //�������� �� ������������� ��������� ������ ����� �������� ��������� �����
            cout << "���� �� ������ (��� ���) �� ����������." << endl;
            return;
        }
        int u = nameToIndex[from]; //�������� ������� ������
        int v = nameToIndex[to];

        adjList[u].erase(remove_if(adjList[u].begin(), adjList[u].end(), //�������� ����� �� ������ ��������� ������� u (from) 
            [v](const Edge& edge) { return edge.to == v; }),
            adjList[u].end());

        if (!directed) { //���� ���� �����������������, �� ������� �������� �����
            adjList[v].erase(remove_if(adjList[v].begin(), adjList[v].end(),
                [u](const Edge& edge) { return edge.to == u; }),
                adjList[v].end());
        }
    }

    //����� ��� ���������� ���� � ����
    void saveToFile(const string& filename) const {
        ofstream outFile(filename);

        if (!outFile) {
            cout << "������ �������� ����� ��� ������!" << endl;
            return;
        }
        if (directed) {
            outFile << "Directed\n";
        }
        else {
            outFile << "Undirected\n";
        }

        //���������� �����
        for (int u = 0; u < numVertices; ++u) {
            for (const Edge& edge : adjList[u]) {
                //���� ���� �����������������, ���������� ������ �������� ����
                if (directed || u < edge.to) {
                    outFile << indexToName.at(u) << " "
                        << indexToName.at(edge.to) << " "
                        << edge.weight << "\n";
                }
            }
        }

        outFile.close();
        cout << "���� ������� ������� � ���� " << filename << endl;
    }

    //����� ��� ������ ������ ���������
    void printAdjList() const {
        for (int u = 0; u < numVertices; ++u) {
            cout << indexToName.at(u) << ": ";
            for (const Edge& edge : adjList[u]) {
                cout << "(" << indexToName.at(edge.to) << ", ���: " << edge.weight << ") ";
            }
            cout << endl;
        }
    }


    void findCommonTarget(const string& u, const string& v) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) {
            cout << "���� ��� ��� ������� �� ����������!" << endl;
            return;
        }

        int uIndex = nameToIndex[u]; //��������� ������� ������ �� ���-�������
        int vIndex = nameToIndex[v]; 

        unordered_set<int> targetsFromU; //��������� ��� �������� �������� ������, � ������� ����� ���� �� u � v
        unordered_set<int> targetsFromV;

        
        for (const Edge& edge : adjList[uIndex]) { //�������� �� ������ � ���� �������� ������� ��� u 
            targetsFromU.insert(edge.to);
        }

     
        for (const Edge& edge : adjList[vIndex]) { //�������� �� ������ � ���� �������� ������� ��� v
            targetsFromV.insert(edge.to);
        }

        
        for (int target : targetsFromU) { //���� ����������� ���� ��������
            if (targetsFromV.find(target) != targetsFromV.end()) {
                cout << "����� �������: " << indexToName[target] << endl;
                return;
            }
        }

        cout << "��� ����� �������, � ������� ����� ���� ��� �� " << u << " � �� " << v << endl;
    }


    void getOutDegree(const string& vertexName) {
        if (nameToIndex.find(vertexName) == nameToIndex.end()) {
            cout << "������� " << vertexName << " �� �������!" << endl;
            return;
        }

        int vertexIndex = nameToIndex[vertexName]; //������ �������
        int outDegree = adjList[vertexIndex].size();  //���-�� �����, ��������� �� ������� 

        cout << "����������� ������ ������� " << vertexName << " : " << outDegree << endl;
    }

    Graph reverseGraph() const {
        Graph reversedGraph(true); //����� ���� ������ ���� ���������������

        for (int i = 0; i < numVertices; ++i) { //��������� �������� ����� � ����� ����
            for (const Edge& edge : adjList[i]) {
                reversedGraph.addEdge(indexToName.at(edge.to), indexToName.at(i), edge.weight);
            }
        }
        return reversedGraph;  
    }
};

#endif  // GRAPH_H