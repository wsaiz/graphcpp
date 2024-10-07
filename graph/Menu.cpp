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
    int weight;

    while (true) {
        cout << "\n����:\n";
        cout << "1. �������� ������� \n";
        cout << "2. �������� ����� \n";
        cout << "3. ������� ������� \n";
        cout << "4. ������� ����� \n";
        cout << "5. �������� ������ ��������� \n";
        cout << "6. ��������� ���� � ���� \n";
        cout << "7. ��������� ���� �� ����� \n";
        cout << "8. ����� ����� ������� ��� ���� ������ \n";  

        if (graph.isDirected()) {
            cout << "9. ����� ����������� ������ ��� ������� \n";
            cout << "10. ��������� ��������� ����� \n";
        }

        cout << "11. ����� \n";
        cout << "�������� �����: ";
        while (!(cin >> option)) {
            cout << "������ �����! ����������, ������� ����� �����: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        switch (option) {
        case 1:
            cout << "������� �������� �������: ";
            cin >> name;
            graph.addVertex(name);
            break;

        case 2:
            cout << "������� ��������� �������: ";
            cin >> from;
            cout << "������� �������� �������: ";
            cin >> to;
            cout << "������� ��� �����: ";
            while (!(cin >> weight)) {
                cout << "������ �����! ������� ��� ����� (int): ";
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }
            graph.addEdge(from, to, weight);
            break;

        case 3:
            cout << "������� �������� ������� ��� ��������: ";
            cin >> name;
            graph.removeVertex(name);
            break;

        case 4:
            cout << "������� ��������� �������: ";
            cin >> from;
            cout << "������� �������� �������: ";
            cin >> to;
            graph.removeEdge(from, to);
            break;

        case 5:
            graph.printAdjList();
            break;

        case 6:
            cout << "������� ��� ����� ��� ����������: ";
            cin >> filename;
            graph.saveToFile(filename);
            break;

        case 7:
            cout << "������� ��� ����� ��� ��������: ";
            cin >> filename;
            graph = Graph(filename);
            break;

        case 8:
            cout << "������� ������ �������: ";
            cin >> from;
            cout << "������� ������ �������: ";
            cin >> to;
            graph.findCommonTarget(from, to);  
            break;

        case 9:
            if (graph.isDirected()) {
                cout << "������� �������� �������: ";
                cin >> name;
                graph.getOutDegree(name);
            }
            else {
                cout << "������� �������� ������ ��� ��������������� ������.\n";
            }
            break;

        case 10:
            if (graph.isDirected()) {
                Graph reversedGraph = graph.reverseGraph();
                cout << "��������� ����� ���������. ��� ������ ��������� ����������� �����:\n";
                reversedGraph.printAdjList();
            }
            else {
                cout << "������� �������� ������ ��� ��������������� ������.\n";
            }
            break;

        case 11:
            return;

        default:
            cout << "�������� �����, ���������� �����.\n";
        }
    }
}
