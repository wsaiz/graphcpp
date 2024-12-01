﻿#ifndef GRAPH_H
#define GRAPH_H
#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>
#include <unordered_set>
#include <queue>

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
        // Используем std::getline для считывания типа графа
        if (!getline(file, type)) {
            throw runtime_error("Ошибка чтения типа графа из файла!");
        }

        // Удаляем лишние пробелы
        type.erase(remove_if(type.begin(), type.end(), ::isspace), type.end());

        // Проверяем тип графа
        if (type == "Directed") {
            directed = true;
        }
        else if (type == "Undirected") {
            directed = false;
        }
        else {
            throw runtime_error("Некорректный тип графа в файле: " + type);
        }

        string from, to;
        int weight;
        nameToIndex.clear();
        indexToName.clear();
        adjList.clear();
        numVertices = 0;

        // Читаем рёбра
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
        bool edgeExists = false;
        for (const Edge& edge : adjList[u]) {
            if (edge.to == v) {
                edgeExists = true;
                break;
            }
        }

        if (!edgeExists) {
            cout << "Ребро между " << from << " и " << to << " не существует." << endl;
            return;
        }
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

    //5-6 task
    //функция для проверки, можно ли отключить две вершины, используя не более k рёбер
    bool canDisconnectWithKEdges(const string& u, const string& v, int k, bool isDirected) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) {
            cout << "Одна или обе вершины не существуют!" << endl;
            return false;
        }

        int uIndex = nameToIndex[u];
        int vIndex = nameToIndex[v];

        if (!hasPath(uIndex, vIndex)) {
            cout << "Вершины " << u << " и " << v << " уже отключены." << endl;
            return true;
        }

        if (isDirected) {
            //для ориентированного графа — проверяем все исходящие рёбра из u
            vector<pair<int, int>> edgesToDisconnect;
            for (const Edge& edge : adjList[uIndex]) {
                int nextNode = edge.to;
                if (hasPath(nextNode, vIndex)) {
                    edgesToDisconnect.push_back({ uIndex, nextNode });
                }
            }

            if (edgesToDisconnect.size() <= k) {
                cout << "Можно отключить путь между " << u << " и " << v << " с помощью отключения "
                    << edgesToDisconnect.size() << " рёбер." << endl;
                cout << "Рёбра для отключения: ";
                for (const auto& edge : edgesToDisconnect) {
                    cout << "(" << indexToName[edge.first] << ", " << indexToName[edge.second] << ") ";
                }
                cout << endl;
                return true;
            }
            else {
                cout << "Невозможно отключить путь между " << u << " и " << v << " с помощью " << k
                    << " рёбер." << endl;
                return false;
            }
        }
        else {
            //для неориентированного графа подсчитываем степень вершин
            int degreeU = adjList[uIndex].size(); //количество рёбер, связанных с вершиной u
            int degreeV = adjList[vIndex].size(); //количество рёбер, связанных с вершиной v

            if (degreeU <= k || degreeV <= k) {
                cout << "Можно отключить путь между " << u << " и " << v << " удалив рёбра." << endl;
                cout << "Степень " << u << ": " << degreeU << ", степень " << v << ": " << degreeV << endl;
                return true;
            }
            else {
                cout << "Невозможно отключить путь между " << u << " и " << v << " с помощью " << k
                    << " рёбер." << endl;
                return false;
            }
        }
    }


    //функция проверки существования пути между двумя вершинами
    bool hasPath(int u, int v) {
        vector<bool> visited(numVertices, false);
        return dfs(u, v, visited);
    }

    // вспомогательная функция DFS для проверки пути
    bool dfs(int u, int v, vector<bool>& visited) {
        if (u == v) return true; // если достигли целевой вершины
        visited[u] = true;
        for (const Edge& edge : adjList[u]) {
            if (!visited[edge.to] && dfs(edge.to, v, visited)) return true;
        }
        return false;
    }
    //метод для нахождения цикломатического числа графа
    int findCyclomaticNumber() const {
        int edgeCount = 0;
        for (const auto& edges : adjList) { //подсчитываем кол-во ребер для каждой вершины
            edgeCount += edges.size();
        }
        
        if (!directed) { //для неориентированного графа (ребра делятся на два, т.к. они дважды записаны в список смежности)
            edgeCount /= 2;
        }

        int componentCount = countConnectedComponents();
        return edgeCount - numVertices + componentCount;
    }

    int countConnectedComponents() const { //метод для проверки посещенных вершин
        vector<bool> visited(numVertices, false);
        int componentCount = 0;

        for (int i = 0; i < numVertices; ++i) {
            if (!visited[i]) {
                ++componentCount;
                dfsForComponents(i, visited);
            }
        }

        return componentCount;
    }

    //dfs для обхода всех вершин, которые связаны с начальной
    void dfsForComponents(int vertex, vector<bool>& visited) const {
        visited[vertex] = true; 
        for (const Edge& edge : adjList[vertex]) {
            if (!visited[edge.to]) {
                dfsForComponents(edge.to, visited);
            }
        }
    }
    //метод для нахождения минимального остовного дерева с помощью алгоритма Прима
    void findMinimumSpanningTree() {
        if (directed) {
            cout << "Алгоритм Прима применим только к неориентированным графам." << endl;
            return;
        }

        vector<bool> inMST(numVertices, false);  //массив для отслеживания вершин в остовном дереве
        vector<int> minEdgeWeight(numVertices, INT_MAX);  //минимальный вес для добавления каждой вершины
        vector<int> parent(numVertices, -1);  //массив для хранения родителей в мин. ост. дереве

        //приоритетная очередь для выбора минимального веса рёбра
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

        int start = 0;  //начинаем с вершины с индексом 0
        minEdgeWeight[start] = 0;
        pq.push({ 0, start });  //помещаем начальную вершину в очередь

        while (!pq.empty()) {
            int u = pq.top().second;  //извлекаем вершину с минимальным весом
            pq.pop();

            if (inMST[u]) continue;  //пропускаем, если вершина уже в МОС

            inMST[u] = true;  //добавляем вершину в МОС

            //обходим все смежные вершины
            for (const Edge& edge : adjList[u]) {
                int v = edge.to;
                int weight = edge.weight;

                //если вершина v ещё не в МОС и вес ребра меньше известного
                if (!inMST[v] && weight < minEdgeWeight[v]) {
                    minEdgeWeight[v] = weight;
                    pq.push({ weight, v });
                    parent[v] = u;  //обновляем родителя вершины
                }
            }
        }

        //вывод минимального остовного дерева
        cout << "Минимальное остовное дерево:" << endl;
        int totalWeight = 0;
        for (int i = 1; i < numVertices; ++i) {
            if (parent[i] != -1) {
                cout << indexToName[parent[i]] << " - " << indexToName[i] << " (вес: " << minEdgeWeight[i] << ")" << endl;
                totalWeight += minEdgeWeight[i];
            }
        }
        cout << "Общий вес остовного дерева: " << totalWeight << endl;
    }

    //метод для поиска кратчайшего пути из вершины u до вершины v с использованием алгоритма Беллмана-Форда
    void findShortestPathBellmanFord(const string& u, const string& v) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) {
            cout << "Одна или обе вершины не существуют!" << endl;
            return;
        }

        int start = nameToIndex[u]; //индекс начальной вершины
        int end = nameToIndex[v];   //индекс конечной вершины

        vector<int> distance(numVertices, INT_MAX); //минимальные расстояния до каждой вершины
        vector<int> predecessor(numVertices, -1);   //предшественники для восстановления пути

        distance[start] = 0; //расстояние до начальной вершины равно 0

        //основной цикл алгоритма Беллмана-Форда
        for (int i = 0; i < numVertices - 1; ++i) {
            for (int u = 0; u < numVertices; ++u) {
                for (const Edge& edge : adjList[u]) {
                    int v = edge.to;
                    int weight = edge.weight;

                    //если найдено более короткое расстояние, обновляем его
                    if (distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
                        distance[v] = distance[u] + weight;
                        predecessor[v] = u;
                    }
                }
            }
        }

        //проверка на наличие отрицательного цикла
        for (int u = 0; u < numVertices; ++u) {
            for (const Edge& edge : adjList[u]) {
                int v = edge.to;
                int weight = edge.weight;
                if (distance[u] != INT_MAX && distance[u] + weight < distance[v]) {
                    cout << "Граф содержит отрицательный цикл!" << endl;
                    return;
                }
            }
        }

        //если конечная вершина недостижима
        if (distance[end] == INT_MAX) {
            cout << "Вершина " << v << " недостижима из " << u << "." << endl;
            return;
        }

        //восстанавливаем путь от u до v
        vector<string> path;
        for (int current = end; current != -1; current = predecessor[current]) {
            path.push_back(indexToName[current]);
        }
        reverse(path.begin(), path.end());

        //вывод результата
        cout << "Кратчайший путь из " << u << " в " << v << ": ";
        for (const string& vertex : path) {
            cout << vertex << " ";
        }
        cout << endl;
        cout << "Длина пути: " << distance[end] << endl;
    }
    //определить, существует ли путь длиной не более L между двумя заданными вершинами графа с помощью алгоритма Флойда — Уоршелла
    void findPathWithinL(const string& startName, const string& endName, int L) {
        //проверяем, существуют ли начальная и конечная вершины
        if (nameToIndex.find(startName) == nameToIndex.end() || nameToIndex.find(endName) == nameToIndex.end()) {
            cout << "Одна или обе вершины не существуют!" << endl;
            return; 
        }

        //индексы для вершин
        int start = nameToIndex[startName];
        int end = nameToIndex[endName];
        int n = adjList.size();
        const int INF = INT_MAX / 2; //бесконечность для недостижимых путей

        //инициализация матрицы расстояний и матрицы "предков" для восстановления пути
        vector<vector<int>> dist(n, vector<int>(n, INF));
        vector<vector<int>> next(n, vector<int>(n, -1)); //матрица для восстановления пути

        //список всех вершин
        vector<string> vertices;
        for (const auto& pair : nameToIndex) {
            vertices.push_back(pair.first); 
        }

        //заполнение списков расстояний и связей для прямых рёбер
        for (int i = 0; i < n; ++i) {
            dist[i][i] = 0; // Расстояние до себя = 0
            for (const Edge& edge : adjList[i]) {
                dist[i][edge.to] = edge.weight; //вес ребра
                next[i][edge.to] = edge.to;    //связь с другими ребрами ребро
            }
        }

        //алгоритм Флойда-Уоршелла для нахождения всех кратчайших путей
        for (int k = 0; k < n; ++k) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    if (dist[i][k] < INF && dist[k][j] < INF) {
                        if (dist[i][j] > dist[i][k] + dist[k][j]) {
                            dist[i][j] = dist[i][k] + dist[k][j];
                            next[i][j] = next[i][k]; 
                        }
                    }
                }
            }
        }

        //проверяем, что существует ли путь с длиной <= L
        if (dist[start][end] > L || dist[start][end] == INF) {
            cout << "Путь не существует или его длина больше, чем " << L << endl;
            return;
        }

        //восстанавливаем путь
        vector<string> path;
        int current = start;
        while (current != end) {
            if (current == -1) {
                cout << "Путь недостижим!" << endl;
                return; 
            }
            path.push_back(vertices[current]); //добавляем название вершины в путь
            current = next[current][end];
        }
        path.push_back(vertices[end]); //добавляем конечную вершину в путь

        
        cout << "Путь от " << startName << " до " << endName << " с длиной <= " << L << "\n";
        cout << "Минимальная длина пути : " << dist[start][end] << "\n";
        for (const string& vertex : path) {
            cout << vertex << " "; 
        }
        cout << endl;
    }
   
    //метод для проверки вершины, удовлетворяющей условию задачи
    vector<int> getVerticesWithPathsBelowN(int N) {
        vector<int> validVertices; //список для хранения вершин, которые удовлетворяют условию.

        for (int u = 0; u < numVertices; ++u) {
            vector<int> distance(numVertices, INT_MAX);
            distance[u] = 0;

            //алгоритм Беллмана-Форда
            for (int i = 0; i < numVertices - 1; ++i) {
                for (int v = 0; v < numVertices; ++v) {
                    for (const Edge& edge : adjList[v]) {
                        if (distance[v] != INT_MAX && distance[v] + edge.weight < distance[edge.to]) {
                            distance[edge.to] = distance[v] + edge.weight;
                        }
                    }
                }
            }
            //проверка на отрицательный цикл
            bool hasNegativeCycle = false;
            for (int v = 0; v < numVertices; ++v) {
                for (const Edge& edge : adjList[v]) {
                    if (distance[v] != INT_MAX && distance[v] + edge.weight < distance[edge.to]) {
                        hasNegativeCycle = true;
                        break;
                    }
                }
            }
            if (hasNegativeCycle) {
                cout << "Граф содержит отрицательный цикл. Проверка невозможна." << endl;
                return {};
            }
            //проверяем все минимальные расстояния от вершины u
            bool allBelowN = true;
            for (int v = 0; v < numVertices; ++v) {
                if (v != u && (distance[v] == INT_MAX || distance[v] > N)) {
                    allBelowN = false;
                    break;
                }
            }

            //если вершина удовлетворяет условию, добавляем ее в список
            if (allBelowN) {
                validVertices.push_back(u);
            }
        }
        if (!validVertices.empty()) {
            cout << "Вершины, удовлетворяющие условию (расстояния не превосходят " << N << "): ";
            for (int vertex : validVertices) {
                cout << indexToName[vertex] << " ";
            }
            cout << endl;
        }
        else {
            cout << "В графе нет вершин, удовлетворяющих условию." << endl;
        }

        return validVertices;
    }

    //вспомогательный метод для поиска пути в остаточной сети с использованием BFS
    bool bfs(vector<vector<int>>& residualGraph, int source, int sink, vector<int>& parent) {
        int n = residualGraph.size();
        vector<bool> visited(n, false);

        queue<int> q;
        q.push(source);
        visited[source] = true;
        parent[source] = -1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int v = 0; v < n; ++v) {
                if (!visited[v] && residualGraph[u][v] > 0) {
                    q.push(v);
                    parent[v] = u;
                    visited[v] = true;

                    if (v == sink) {
                        return true;
                    }
                }
            }
        }

        return false;
    }

    int fordFulkerson(const string& u, const string& v) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) {
            throw runtime_error("Начальная вершина или конечная вершина не найдена!");
        }

        int source = nameToIndex[u];
        int sink = nameToIndex[v];

        int n = numVertices;
        vector<vector<int>> residualGraph(n, vector<int>(n, 0)); //матрица остаточной сети nxn (n-кол-во вершин)

        //строим остаточную сеть из списка смежности графа
        for (int u = 0; u < n; ++u) {
            for (const Edge& edge : adjList[u]) {
                residualGraph[u][edge.to] = edge.weight;
            }
        }

        vector<int> parent(n);
        int maxFlow = 0;

        //пока существует путь от source до sink в остаточной сети
        while (bfs(residualGraph, source, sink, parent)) {
            //находим минимальную пропускную способность ребра на пути, найденном BFS
            int pathFlow = numeric_limits<int>::max();
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                pathFlow = min(pathFlow, residualGraph[u][v]);
            }

            //обновляем остаточную сеть
            for (int v = sink; v != source; v = parent[v]) {
                int u = parent[v];
                residualGraph[u][v] -= pathFlow;
                residualGraph[v][u] += pathFlow;
            }

            maxFlow += pathFlow;
        }

        return maxFlow;
    }
};

#endif  // GRAPH_H