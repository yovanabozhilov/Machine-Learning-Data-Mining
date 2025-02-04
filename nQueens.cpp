#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <vector>
#include <iomanip>

typedef std::vector<int> vec;

vec initializeQueens(int N) {
    vec queens(N);
    for (int i = 0; i < N; i++) {
        queens[i] = i;
    }
    for (int i = N - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        std::swap(queens[i], queens[j]);
    }
    return queens;
}

vec buildConflicts(const vec& queens, int N) {
    vec conflicts(N, 0);
    std::vector<int> rows(N, 0), rdiags(2 * N, 0), ldiags(2 * N, 0);

    for (int col = 0; col < N; col++) {
        int row = queens[col];
        rows[row]++;
        rdiags[row - col + N]++;
        ldiags[row + col]++;
    }

    for (int col = 0; col < N; col++) {
        int row = queens[col];
        conflicts[col] = (rows[row] - 1) + (rdiags[row - col + N] - 1) + (ldiags[row + col] - 1);
    }

    return conflicts;
}

int absoluteValue(int N) {
    return (N < 0) ? -N : N;
}

void rowConflicts(const vec& queens, std::vector<int>& row_confs, int N, int col) {
    std::fill(row_confs.begin(), row_confs.end(), 0);

    for (int other_col = 0; other_col < N; other_col++) {
        if (other_col == col) continue;
        int other_row = queens[other_col];

        row_confs[other_row]++;

        int rd = other_row + absoluteValue(other_col - col);
        int ld = other_row - absoluteValue(other_col - col);

        if (rd >= 0 && rd < N) row_confs[rd]++;
        if (ld >= 0 && ld < N) row_confs[ld]++;
    }
}

int pickPosition(const std::vector<int>& row_confs, int N, int min_val) {
    std::vector<int> candidates;
    for (int i = 0; i < N; i++) {
        if (row_confs[i] == min_val) {
            candidates.push_back(i);
        }
    }
    return candidates[rand() % candidates.size()];
}

std::pair<vec, int> minimumConflicts(int N) {
    const int MAX_STEPS = 1000000;

    vec queens = initializeQueens(N);
    vec conflicts = buildConflicts(queens, N);
    std::vector<int> row_confs(N);

    int total_conflicts = 0;
    for (int i = 0; i < N; i++) {
        total_conflicts += conflicts[i];
    }

    for (int step = 0; step < MAX_STEPS; step++) {
        if (total_conflicts == 0) {
            return { queens, step };
        }

        std::vector<int> conflict_cols;
        for (int col = 0; col < N; col++) {
            if (conflicts[col] > 0) {
                conflict_cols.push_back(col);
            }
        }

        int col = conflict_cols[rand() % conflict_cols.size()];

        rowConflicts(queens, row_confs, N, col);
        int min_conflict = *std::min_element(row_confs.begin(), row_confs.end());
        int new_row = pickPosition(row_confs, N, min_conflict);

        total_conflicts -= conflicts[col];
        queens[col] = new_row;
        conflicts = buildConflicts(queens, N);
        total_conflicts = 0;
        for (int i = 0; i < N; i++) {
            total_conflicts += conflicts[i];
        }
    }

    return { {}, -1 };
}

void printBoard(const vec& queens, int N) {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            std::cout << (queens[col] == row ? "*" : "_") << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void printSolution(const vec& queens, int N) {
    std::cout << "[";
    for (int i = 0; i < N; i++) {
        std::cout << queens[i];
        if (i < N - 1) std::cout << ", ";
    }
    std::cout << "]\n";
}

int main() {
    srand(time(0));
    int N;
    std::cin >> N;

    auto start = std::chrono::high_resolution_clock::now();

    auto result = minimumConflicts(N);

    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count();

    if (result.second == -1) {
        std::cout << "-1\n";
    }

    if (N > 100) {
        std::cout << std::fixed << std::setprecision(2) << elapsed << "\n";
    }
    else {
        //printSolution(result.first, N);
        printBoard(result.first, N);
    }

    return 0;
}
