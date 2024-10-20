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
    bool edgeExists(const string& from, const string& to) {
        if (nameToIndex.find(from) == nameToIndex.end() || nameToIndex.find(to) == nameToIndex.end()) {
            return false; //если одна из вершин не существует, ребро не может существовать
        }

        int u = nameToIndex[from]; 
        int v = nameToIndex[to];   

        //если ребро есть в списке смежности
        for (const Edge& edge : adjList[u]) {
            if (edge.to == v) {
                return true; //ребро существует
            }
        }

        return false; //ребро не существует
    }
    bool canDisconnectWithKEdges(const string& u, const string& v, int k) {
        if (nameToIndex.find(u) == nameToIndex.end() || nameToIndex.find(v) == nameToIndex.end()) { //проверка на существование вершин
            cout << "Одна или обе вершины не существуют!" << endl;
            return false;
        }
        if (!edgeExists(u, v)) {
            cout << "Ребро между " << u << " и " << v << " не существует." << endl;
            return false;
        }
        int uIndex = nameToIndex[u];  //индексы вершины u и v
        int vIndex = nameToIndex[v];  

        //если не существует пути из u в v, то ничего искать не нужно
        if (!hasPath(uIndex, vIndex)) {
            cout << "Вершины " << u << " и " << v << " уже отключены." << endl;
            return true;
        }

        //поиск критических ребер, которые разрывают связь между u и v 
        vector<pair<int, int>> criticalEdges = findCriticalEdges(uIndex, vIndex);

        //если количество критических рёбер меньше или равно k, можем разорвать путь
        if (criticalEdges.size() == k) {
            cout << "Можно отключить вершины " << u << " и " << v << " с помощью " << k
                << " рёбер." << endl;
            return true;
        }
        else {
            cout << "Невозможно отключить вершины " << u << " и " << v << " с помощью " << k
                << " рёбер." << endl;
            return false;
        }
    }

    //вспомогательная функция для поиска всех путей
    bool hasPath(int u, int v) {
        vector<bool> visited(numVertices, false); //вектор для отслеживания посещенных вершин
        return dfs(u, v, visited);
    }

    //dfs для проверки существования пути
    bool dfs(int u, int v, vector<bool>& visited) {
        if (u == v) { //если текущая вершина целевая вершина
            return true;
        }
        visited[u] = true; //отмечаем вершину, как посещенную

        for (const Edge& edge : adjList[u]) { //перебираем всех соседей вершины
            if (!visited[edge.to]) {
                if (dfs(edge.to, v, visited)) { //рекурсивно вызываем метод для след. вершины
                    return true;
                }
            }
        }

        return false;
    }

    //поиск критических рёбер, удаление которых разорвёт путь
    vector<pair<int, int>> findCriticalEdges(int u, int v) {
        vector<pair<int, int>> criticalEdges; //список критических ребер

        //применяю DFS для поиска всех путей и выявления критических рёбер
        vector<bool> visited(numVertices, false);
        dfsFindCriticalEdges(u, v, visited, criticalEdges);

        return criticalEdges;
    }
    //dfs для критических ребер
    void dfsFindCriticalEdges(int u, int v, vector<bool>& visited, vector<pair<int, int>>& criticalEdges) {
        if (u == v) {
            return;
        }

        visited[u] = true; //отмечаем, что вершина посещена

        for (const Edge& edge : adjList[u]) {
            if (!visited[edge.to]) {
                
                int to = edge.to;
                visited[to] = true;

                
                if (!hasPathWithoutEdge(u, to, v)) { //проверяем является ли текущее ребро критическим
                    criticalEdges.push_back(make_pair(u, to));
                }

                
                dfsFindCriticalEdges(to, v, visited, criticalEdges); //продолжжаем обход в глубину

                
                visited[to] = false; //сбрасываем статус посещенной вершины
            }
        }

        visited[u] = false; 
    }


    bool hasPathWithoutEdge(int from, int to, int v) {
        vector<bool> visited(numVertices, false); //вектор для отслеживания посещенных вершин
        visited[to] = true; //исключаем ребро с помощью метки
        return dfs(from, v, visited); //запускаем dfs игнорируя это ребро
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
};

#endif  // GRAPH_H