#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <fstream>
#include "MenuLink.h"
#include "Graph.h"
using namespace std;

void graphMenu(Graph& graph) {
    int option;
    string from, to, name, filename;
    int weight, k;

    while (true) {
        cout << "\nМеню:\n";
        cout << "1. Добавить вершину \n";
        cout << "2. Добавить ребро \n";
        cout << "3. Удалить вершину \n";
        cout << "4. Удалить ребро \n";
        cout << "5. Показать список смежности \n";
        cout << "6. Сохранить граф в файл \n";
        cout << "7. Загрузить граф из файла \n";
        cout << "8. Найти общую вершину для двух вершин \n";
        cout << "9. Определить, можно ли закрыть k рёбер, чтобы из вершины u нельзя было попасть в вершину v \n";

        if (graph.isDirected()) {
            cout << "10. Найти полустепень исхода для вершины \n";
            cout << "11. Построить обращение графа \n";
        }

        cout << "12. Найти цикломатическое число графа \n";

        cout << "13. Выйти \n";
        cout << "Выберите опцию: ";
        while (!(cin >> option)) {
            cout << "Ошибка ввода! Пожалуйста, введите номер опции: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (option) {
        case 1:
            cout << "Введите название вершины: ";
            cin >> name;
            graph.addVertex(name);
            break;

        case 2:
            cout << "Введите начальную вершину: ";
            cin >> from;
            cout << "Введите конечную вершину: ";
            cin >> to;
            cout << "Введите вес ребра: ";
            while (!(cin >> weight)) {
                cout << "Ошибка ввода! Введите вес ребра (int): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            graph.addEdge(from, to, weight);
            break;

        case 3:
            cout << "Введите название вершины для удаления: ";
            cin >> name;
            graph.removeVertex(name);
            break;

        case 4:
            cout << "Введите начальную вершину: ";
            cin >> from;
            cout << "Введите конечную вершина: ";
            cin >> to;
            graph.removeEdge(from, to);
            break;

        case 5:
            graph.printAdjList();
            break;

        case 6:
            cout << "Введите имя файла для сохранения: ";
            cin >> filename;
            graph.saveToFile(filename);
            break;

        case 7:
            cout << "Введите имя файла для загрузки: ";
            cin >> filename;
            graph = Graph(filename);
            break;

        case 8:
            cout << "Введите первую вершину: ";
            cin >> from;
            cout << "Введите вторую вершину: ";
            cin >> to;
            graph.findCommonTarget(from, to);
            break;

        case 9:
            cout << "Введите начальную вершину (u): ";
            cin >> from;
            cout << "Введите конечную вершину (v): ";
            cin >> to;
            cout << "Введите количество рёбер для удаления (k): ";
            while (!(cin >> k)) {
                cout << "Ошибка ввода! Введите количество рёбер (int): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            graph.canDisconnectWithKEdges(from, to, k);
            break;

        case 10:
            if (graph.isDirected()) {
                cout << "Введите название вершины: ";
                cin >> name;
                graph.getOutDegree(name);
            }
            else {
                cout << "Функция доступна только для ориентированных графов.\n";
            }
            break;

        case 11:
            if (graph.isDirected()) {
                Graph reversedGraph = graph.reverseGraph();
                cout << "Обращение графа построено. Вот список смежности обращённого графа:\n";
                reversedGraph.printAdjList();
            }
            else {
                cout << "Функция доступна только для ориентированных графов.\n";
            }
            break;

        case 12:
            cout << "Цикломатическое число графа: " << graph.findCyclomaticNumber() << endl;
            break;

        case 13:
            return;

        default:
            cout << "Неверная опция, попробуйте снова.\n";
        }
    }
}
