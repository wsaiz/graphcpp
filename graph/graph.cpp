#include <iostream>
#include <string>
#include "MenuLink.h"
#include "Graph.h"
using namespace std;

int main() {
    setlocale(0, "");
    int mainOption;
    string filename;
    Graph graph;  

    while (true) {
        cout << "0. Создать новый граф\n";
        cout << "1. Загрузить граф из файла\n";
        cout << "2. Выход\n";
        cout << "Выберите опцию: ";
        cin >> mainOption;
        if (cin.fail()) {
            cout << "Неверная опция, попробуйте снова\n";
            cin.clear();  
            cin.ignore(numeric_limits<streamsize>::max(), '\n');  
            continue;  
        }
        if (mainOption == 0) {
            bool isDirected;
            while (true) {
                cout << "Ориентированный граф? (1 - Да, 0 - Нет): ";
                cin >> isDirected;
                if (cin.fail() || (isDirected != 0 && isDirected != 1)) {
                    cout << "Неверный ввод. Введите 1 для ориентированного графа или 0 для неориентированного\n";
                    cin.clear();  
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');  
                }
                else {
                    break;  
                }
            }
            graph = Graph(isDirected);  
            graphMenu(graph);
        }
        else if (mainOption == 1) {
            cout << "Введите имя файла для загрузки: ";
            cin >> filename;

            
            try {
                graph = Graph(filename);  
                graphMenu(graph);  
            }
            catch (const runtime_error& e) {
                cout << "Ошибка загрузки графа " << e.what() << "\n";
                continue; 
            }
        }
        else if (mainOption == 2) {
            break;
        }
        else {
            cout << "Неверная опция, попробуйте снова\n";
        }
    }

}
