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
    int to;        //конечная вершина
    int weight;    //вес ребра
    Edge(int to, int weight) : to(to), weight(weight) {}
};
class Graph {
private:
    int numVertices;                             //кол-во вершин
    bool directed;                               //флаг ориентированного/неориетированного графа
    vector<vector<Edge>> adjList;                //список смежности с весами
    unordered_map<string, int> nameToIndex;      //хэш-таблица для сопоставления имени вершины с индексом
    unordered_map<int, string> indexToName;      //хэш-таблица для сопоставления индекса вершины с именем

public:
    //конструктор по умолчанию, который создает пустой граф
    Graph(bool directed = false) : numVertices(0), directed(directed) {}

    //конструктор для загрузки графа из файла
    Graph(const string& filename) {
        ifstream file(filename);
        if (!file) {
            throw runtime_error("Ошибка открытия файла для чтения!");  // Выбрасываем исключение
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
        cout << "Граф загружен из файла " << filename << endl;
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
    //метод для добавления вершины
    void addVertex(const string& name) {
        if (nameToIndex.find(name) == nameToIndex.end()) { //проверка на существование вершины с таким же именем
            nameToIndex[name] = numVertices; //добавляем в nameToIndex (связка с текущим числом вершин) для доступа к индексу вершины по ее имени
            indexToName[numVertices] = name; //индекс связывается с именем вершины 
            adjList.push_back(vector<Edge>()); //добавляем новый список ребер для вершины
            ++numVertices; //увеличиваем число вершин
        }
    }

    //метод для добавления ребра
    void addEdge(const string& from, const string& to, int weight) {
        addVertex(from); //проверяем существуют ли вершины, между которыми можно построить ребро
        addVertex(to);
        int u = nameToIndex[from]; //находим индексы вершин в хэш-таблице
        int v = nameToIndex[to];
        adjList[u].push_back(Edge(v, weight)); //добавляем ребро в список смежности

        if (!directed) { //если граф неориентированный, то добавляем обратное ребро
            adjList[v].push_back(Edge(u, weight));
        }
    }
    //метод для удаления вершины
    void removeVertex(const string& name) { //проверка на существование вершины
        if (nameToIndex.find(name) == nameToIndex.end()) {
            cout << "Вершина не найдена."<<endl;
            return;
        }

        int index = nameToIndex[name]; //индекс удаляемой вершины

        
        for (auto& edges : adjList) { //удаляем все ребра, который входят и выходят из вершины
            edges.erase(remove_if(edges.begin(), edges.end(),
                [index](const Edge& edge) { return edge.to == index; }),
                edges.end());
        }

        adjList.erase(adjList.begin() + index); //удаляем список смежности для данной вершины
       
        for (auto& edges : adjList) { //перенумеруем все вершины, у которых индекс больше, чем у удаленной вершины
            for (auto& edge : edges) {
                if (edge.to > index) {
                    --edge.to;  //смещаем индекс на -1
                }
            }
        }

        
        unordered_map<string, int> newNameToIndex; 
        unordered_map<int, string> newIndexToName;

        int newIndex = 0;
        for (const auto& pair : indexToName) { //обновляем хэш-таблицы
            if (pair.first != index) {  
                newIndexToName[newIndex] = pair.second;
                newNameToIndex[pair.second] = newIndex;
                ++newIndex;
            }
        }

        nameToIndex = move(newNameToIndex); //заменяем старые хэш-таблицы на новые 
        indexToName = move(newIndexToName);
        --numVertices;  
    }


    //метод для удаления ребра 
    void removeEdge(const string& from, const string& to) {
        if (nameToIndex.find(from) == nameToIndex.end() || nameToIndex.find(to) == nameToIndex.end()) { //проверка на существование указанных вершин между которыми удаляется ребро
            cout << "Одна из вершин (или обе) не существует." << endl;
            return;
        }
        int u = nameToIndex[from]; //получаем индексы вершин
        int v = nameToIndex[to];

        adjList[u].erase(remove_if(adjList[u].begin(), adjList[u].end(), //удаление ребра из списка смежности вершины u (from) 
            [v](const Edge& edge) { return edge.to == v; }),
            adjList[u].end());

        if (!directed) { //если граф неориентированный, то удаляем обратное ребро
            adjList[v].erase(remove_if(adjList[v].begin(), adjList[v].end(),
                [u](const Edge& edge) { return edge.to == u; }),
                adjList[v].end());
        }
    }

    //метод для сохранения граф в файл
    void saveToFile(const string& filename) const {
        ofstream outFile(filename);

        if (!outFile) {
            cout << "Ошибка открытия файла для записи!" << endl;
            return;
        }
        if (directed) {
            outFile << "Directed\n";
        }
        else {
            outFile << "Undirected\n";
        }

        //записываем ребра
        for (int u = 0; u < numVertices; ++u) {
            for (const Edge& edge : adjList[u]) {
                //если граф неориентированный, пропускаем записи обратных рёбер
                if (directed || u < edge.to) {
                    outFile << indexToName.at(u) << " "
                        << indexToName.at(edge.to) << " "
                        << edge.weight << "\n";
                }
            }
        }

        outFile.close();
        cout << "Граф успешно сохранён в файл " << filename << endl;
    }

    //метод для вывода списка смежности
    void printAdjList() const {
        for (int u = 0; u < numVertices; ++u) {
            cout << indexToName.at(u) << ": ";
            for (const Edge& edge : adjList[u]) {
                cout << "(" << indexToName.at(edge.to) << ", вес: " << edge.weight << ") ";
            }
            cout << endl;
        }
    }


    void findCommonTarget(const string& u, const string& v) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) {
            cout << "Одна или обе вершины не существуют!" << endl;
            return;
        }

        int uIndex = nameToIndex[u]; //извлекаем индексы вершин из хэш-таблицы
        int vIndex = nameToIndex[v]; 

        unordered_set<int> targetsFromU; //множества для хранения конечных вершин, в которые ведут рёбра из u и v
        unordered_set<int> targetsFromV;

        
        for (const Edge& edge : adjList[uIndex]) { //проходим по ребрам и ищем конечные вершины для u 
            targetsFromU.insert(edge.to);
        }

     
        for (const Edge& edge : adjList[vIndex]) { //проходим по ребрам и ищем конечные вершины для v
            targetsFromV.insert(edge.to);
        }

        
        for (int target : targetsFromU) { //ищем пересечение двух множеств
            if (targetsFromV.find(target) != targetsFromV.end()) {
                cout << "Общая вершина: " << indexToName[target] << endl;
                return;
            }
        }

        cout << "Нет общей вершины, в которую ведут дуги как из " << u << " и из " << v << endl;
    }


    void getOutDegree(const string& vertexName) {
        if (nameToIndex.find(vertexName) == nameToIndex.end()) {
            cout << "Вершина " << vertexName << " не найдена!" << endl;
            return;
        }

        int vertexIndex = nameToIndex[vertexName]; //индекс вершины
        int outDegree = adjList[vertexIndex].size();  //кол-во ребер, исходящих из вершины 

        cout << "Полустепень исхода вершины " << vertexName << " : " << outDegree << endl;
    }

    Graph reverseGraph() const {
        Graph reversedGraph(true); //новый граф должен быть ориентированным

        for (int i = 0; i < numVertices; ++i) { //добавляем обратные ребра в новый граф
            for (const Edge& edge : adjList[i]) {
                reversedGraph.addEdge(indexToName.at(edge.to), indexToName.at(i), edge.weight);
            }
        }
        return reversedGraph;  
    }
};

#endif  // GRAPH_H