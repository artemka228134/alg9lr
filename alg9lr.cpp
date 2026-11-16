#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <queue>
#include <chrono>

using namespace std;

struct Node {
    int vertex;
    struct Node* next;
};

struct Node* createNode(int v) {
    struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
    if (newNode != NULL) {
        newNode->vertex = v;
        newNode->next = NULL;
    }
    return newNode;
}

int** generate_adjacency_matrix(int size, int edge_percent) {
    int** G = (int**)malloc(size * sizeof(int*));
    for (int i = 0; i < size; i++) {
        G[i] = (int*)malloc(size * sizeof(int));
    }

    for (int i = 0; i < size; i++) {
        G[i][i] = 0;
        for (int j = i + 1; j < size; j++) {
            int val = (rand() % 100 < edge_percent) ? 1 : 0;
            G[i][j] = val;
            G[j][i] = val;
        }
    }
    return G;
}

void free_matrix(int** G, int size) {
    if (G == NULL) return;
    for (int i = 0; i < size; i++) {
        free(G[i]);
    }
    free(G);
}

void print_matrix(int** G, int size) {
    printf("\n--- Матрица смежности (%dx%d) ---\n     ", size, size);
    for (int j = 0; j < size; j++) {
        printf("v%-3d", j);
    }
    printf("\n");

    for (int i = 0; i < size; i++) {
        printf("v%-3d ", i);
        for (int j = 0; j < size; j++) {
            printf("%-4d", G[i][j]);
        }
        printf("\n");
    }
}
struct Node** create_adj_lists(int size, int** G) {
    struct Node** adjLists = (struct Node**)malloc(size * sizeof(struct Node*));
    for (int i = 0; i < size; i++) {
        adjLists[i] = NULL;
    }

    for (int i = 0; i < size; i++) {
        struct Node* tail = NULL;
        for (int j = 0; j < size; j++) {
            if (G[i][j] == 1) {
                struct Node* n = createNode(j);
                if (adjLists[i] == NULL) {
                    adjLists[i] = tail = n;
                } else {
                    tail->next = n;
                    tail = n;
                }
            }
        }
    }
    return adjLists;
}

void free_lists(struct Node** adjLists, int size) {
    if (adjLists == NULL) return;
    for (int i = 0; i < size; i++) {
        struct Node* temp = adjLists[i];
        while (temp != NULL) {
            struct Node* toDelete = temp;
            temp = temp->next;
            free(toDelete);
        }
    }
    free(adjLists);
}

void print_lists(struct Node** adjLists, int size) {
    printf("\n--- Списки смежности ---\n");
    for (int i = 0; i < size; i++) {
        printf("v%d: ", i);
        struct Node* temp = adjLists[i];
        if (temp == NULL) {
            printf("[нет смежных вершин]");
        }
        while (temp != NULL) {
            printf("-> v%d ", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}

void bfs_distances_matrix(int start, int size, int** G, int* dist) {
    for (int i = 0; i < size; i++) {
        dist[i] = -1;
    }

    queue<int> Q;
    Q.push(start);
    dist[start] = 0;
    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();
        for (int i = 0; i < size; i++) {
            if (G[v][i] == 1 && dist[i] == -1) {
                Q.push(i);
                dist[i] = dist[v] + 1;
            }
        }
    }
}

void bfs_distances_list(int start, int size, struct Node** adjLists, int* dist) {
    for (int i = 0; i < size; i++) {
        dist[i] = -1;
    }

    queue<int> Q;
    Q.push(start);
    dist[start] = 0;

    while (!Q.empty()) {
        int v = Q.front();
        Q.pop();

        struct Node* temp = adjLists[v];
        while (temp != NULL) {
            int to = temp->vertex;
            if (dist[to] == -1) {
                dist[to] = dist[v] + 1;
                Q.push(to);
            }
            temp = temp->next;
        }
    }
}

void dfs_helper_matrix(int v, int size, int** G, int* dist, int current_dist) {
    dist[v] = current_dist;

    for (int i = 0; i < size; i++) {
        if (G[v][i] == 1) {
            if (dist[i] == -1 || current_dist + 1 < dist[i]) {
                dfs_helper_matrix(i, size, G, dist, current_dist + 1);
            }
        }
    }
}

void dfs_distances_matrix(int start, int size, int** G, int* dist) {
    for (int i = 0; i < size; i++) {
        dist[i] = -1;
    }
    dfs_helper_matrix(start, size, G, dist, 0);
}

void dfs_helper_list(int v, struct Node** adjLists, int* dist, int current_dist) {
    dist[v] = current_dist;

    struct Node* temp = adjLists[v];
    while (temp != NULL) {
        int to = temp->vertex;
        if (dist[to] == -1 || current_dist + 1 < dist[to]) {
            dfs_helper_list(to, adjLists, dist, current_dist + 1);
        }
        temp = temp->next;
    }
}

void dfs_distances_list(int start, int size, struct Node** adjLists, int* dist) {
    for (int i = 0; i < size; i++) {
        dist[i] = -1;
    }
    dfs_helper_list(start, adjLists, dist, 0);
}

void print_distances(int start, int size, int* dist, const char* title) {
    printf("\n--- Вектор расстояний (%s) от вершины v%d ---\n", title, start);
    for (int i = 0; i < size; i++) {
        if (dist[i] == -1) {
            printf("  До вершины v%-2d: недостижима (нет пути)\n", i);
        } else {
            printf("  До вершины v%-2d: расстояние = %d\n", i, dist[i]);
        }
    }
}

double get_time() {
    return chrono::duration<double>(chrono::high_resolution_clock::now().time_since_epoch()).count();
}


void run_benchmarks() {

    printf(" ЗАМЕР ВРЕМЕНИ РАБОТЫ (Задание 2*)\n");


    int test_sizes[] = { 100, 300, 600, 1000 };
    int num_tests = sizeof(test_sizes) / sizeof(test_sizes[0]);
    int edge_percent = 25;

    printf("%-8s | %-16s | %-16s | %-16s | %-16s\n", 
           "Вершин", "BFS (Матрица)", "BFS (Списки)", "DFS (Матрица)", "DFS (Списки)");

    for (int t = 0; t < num_tests; t++) {
        int n = test_sizes[t];
        int** G = generate_adjacency_matrix(n, edge_percent);
        struct Node** adjLists = create_adj_lists(n, G);
        int* dist = (int*)malloc(n * sizeof(int));

        double t_bfs_m, t_bfs_l, t_dfs_m, t_dfs_l;
        double start_t;

        // BFS Матрица
        start_t = get_time();
        bfs_distances_matrix(0, n, G, dist);
        t_bfs_m = get_time() - start_t;

        // BFS Списки
        start_t = get_time();
        bfs_distances_list(0, n, adjLists, dist);
        t_bfs_l = get_time() - start_t;

        // DFS Матрица
        start_t = get_time();
        dfs_distances_matrix(0, n, G, dist);
        t_dfs_m = get_time() - start_t;

        // DFS Списки
        start_t = get_time();
        dfs_distances_list(0, n, adjLists, dist);
        t_dfs_l = get_time() - start_t;

        printf("%-8d | %14.6f с | %14.6f с | %14.6f с | %14.6f с\n", 
               n, t_bfs_m, t_bfs_l, t_dfs_m, t_dfs_l);

        free(dist);
        free_lists(adjLists, n);
        free_matrix(G, n);
    }
}

int main() {
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
    srand((unsigned int)time(NULL));

    while (1) {
        printf("1. Случайный граф (ввод размера и вероятности связей)\n");
        printf("2. Сравнение времени работы BFS и DFS (Задание 2*)\n");
        printf("0. Выход\n");
        printf("Выберите пункт меню: ");

        int choice;
        if (scanf("%d", &choice) != 1 || choice == 0) {
            break;
        }
        if (choice == 1) {
            int size, edge_percent;
            printf("\nВведите количество вершин графа (например, 6 или 10): ");
            if (scanf("%d", &size) != 1 || size <= 0) {
                printf("Некорректный ввод!\n");
                continue;
            }

            printf("Введите вероятность наличия ребра в %% (0..100, например 30): ");
            if (scanf("%d", &edge_percent) != 1 || edge_percent < 0 || edge_percent > 100) {
                printf("Некорректная вероятность!\n");
                continue;
            }

            int** G = generate_adjacency_matrix(size, edge_percent);
            struct Node** adjLists = create_adj_lists(size, G);

            if (size <= 25) {
                print_matrix(G, size);
                print_lists(adjLists, size);
            } else {
                printf("\nГраф слишком большой (%d вершин), подробный вывод матриц пропущен.\n", size);
            }

            int start;
            do {
                printf("\nВведите начальную вершину (0..%d): ", size - 1);
                scanf("%d", &start);
                if (start < 0 || start >= size) {
                    printf("Такой вершины нет\n");
                }
                else if (adjLists[start] == NULL) {
                    printf("Вершина v%d изолирована \n", start);
                }
            } while (start < 0 || start >= size || adjLists[start] == NULL);
                
            int* dist_bfs_m = (int*)malloc(size * sizeof(int));
            int* dist_bfs_l = (int*)malloc(size * sizeof(int));
            int* dist_dfs_m = (int*)malloc(size * sizeof(int));
            int* dist_dfs_l = (int*)malloc(size * sizeof(int));

            double t1, t2;

            t1 = get_time();
            bfs_distances_matrix(start, size, G, dist_bfs_m);
            t2 = get_time();
            double time_bfs_m = t2 - t1;

            t1 = get_time();
            bfs_distances_list(start, size, adjLists, dist_bfs_l);
            t2 = get_time();
            double time_bfs_l = t2 - t1;

            t1 = get_time();
            dfs_distances_matrix(start, size, G, dist_dfs_m);
            t2 = get_time();
            double time_dfs_m = t2 - t1;

            t1 = get_time();
            dfs_distances_list(start, size, adjLists, dist_dfs_l);
            t2 = get_time();
            double time_dfs_l = t2 - t1;

            if (size <= 30) {
                print_distances(start, size, dist_bfs_m, "BFS (Матрица смежности)");
                print_distances(start, size, dist_bfs_l, "BFS (Списки смежности)");
                print_distances(start, size, dist_dfs_m, "DFS (Матрица смежности)");
            }

            printf("\n--- Время выполнения для вершины v%d ---\n", start);
            printf("BFS (Матрица смежности):  %.6f сек.\n", time_bfs_m);
            printf("BFS (Списки смежности):   %.6f сек.\n", time_bfs_l);
            printf("DFS (Матрица смежности):  %.6f сек.\n", time_dfs_m);
            printf("DFS (Списки смежности):   %.6f сек.\n", time_dfs_l);

            free(dist_bfs_m);
            free(dist_bfs_l);
            free(dist_dfs_m);
            free(dist_dfs_l);
            free_lists(adjLists, size);
            free_matrix(G, size);
        } if (choice == 2) {
            run_benchmarks();
        }
    }

    printf("\nПрограмма завершена.\n");
    return 0;
}
//запретить старт с изолированной вершины