#include <iostream>
#include <chrono>

using namespace std;

const int FOUND = -1;
const int MAX_INT_SIZE = 2147483647;

int* tiles;
char** path;
int boardSize;
int goalIndexOfZero;
int currentTileIndexOfZero;
int pathSize = 0;
int pathCapacity = 100;

int absoluteValue(int x) {
    return (x < 0) ? -x : x;
}

double squareRoot(double x) {
    if (x < 0) return -1;
    if (x == 0 || x == 1) return x;

    double approximation = x / 2;
    double epsilon = 0.00001;

    while (absoluteValue(approximation * approximation - x) >= epsilon) {
        approximation = (approximation + x / approximation) / 2;
    }
    return approximation;
}

int manhattanDist() {
    int sum = 0;
    int positionsBeforeZeroLeft = goalIndexOfZero;
    int currentTile;
    int x;
    for (int i = 0; i < boardSize; i++) {
        for (int j = 0; j < boardSize; j++) {
            x = j + i * boardSize;
            currentTile = tiles[x];
            if (currentTile == 0) {
                continue;
            }

            if (positionsBeforeZeroLeft > 0) {
                sum += absoluteValue((currentTile - 1) / boardSize - i) + absoluteValue((currentTile - 1) % boardSize - j);
                --positionsBeforeZeroLeft;
            }
            else {
                sum += absoluteValue(currentTile / boardSize - i) + absoluteValue(currentTile % boardSize - j);
            }
        }
    }
    return sum;
}

bool isGoal() {
    return manhattanDist() == 0;
}

void swapTiles(int first, int second) {
    swap(tiles[first], tiles[second]);
}

bool moveUp() {
    if (currentTileIndexOfZero / boardSize == boardSize - 1) {
        return false;
    }
    swapTiles(currentTileIndexOfZero, currentTileIndexOfZero + boardSize);
    currentTileIndexOfZero += boardSize;
    return true;
}

bool moveDown() {
    if (currentTileIndexOfZero / boardSize == 0) {
        return false;
    }
    swapTiles(currentTileIndexOfZero, currentTileIndexOfZero - boardSize);
    currentTileIndexOfZero -= boardSize;
    return true;
}

bool moveRight() {
    if (currentTileIndexOfZero % boardSize == 0) {
        return false;
    }
    swapTiles(currentTileIndexOfZero, currentTileIndexOfZero - 1);
    currentTileIndexOfZero -= 1;
    return true;
}

bool moveLeft() {
    if (currentTileIndexOfZero % boardSize == boardSize - 1) {
        return false;
    }
    swapTiles(currentTileIndexOfZero, currentTileIndexOfZero + 1);
    currentTileIndexOfZero += 1;
    return true;
}

int posOfZero() {
    for (int i = 0; i < boardSize * boardSize; i++) {
        if (tiles[i] == 0) {
            return i;
        }
    }
    return -1;
}

void resizePath() {
    char** newPath = new char* [pathCapacity * 2];
    for (int i = 0; i < pathSize; i++) {
        newPath[i] = path[i];
    }
    delete[] path;
    path = newPath;
    pathCapacity *= 2;
}

void addToPath(const char* direction) {
    if (pathSize == pathCapacity) {
        resizePath();
    }
    path[pathSize] = new char[10];
    int i = 0;
    while (direction[i] != '\0') {
        path[pathSize][i] = direction[i];
        i++;
    }
    path[pathSize][i] = '\0';
    pathSize++;
}

void removeFromPath() {
    if (pathSize > 0) {
        delete[] path[pathSize - 1];
        pathSize--;
    }
}

int search(int g, int threshold) {
    int f = g + manhattanDist();
    if (f > threshold) {
        return f;
    }
    if (isGoal()) {
        cout << pathSize - 1 << endl;
        for (int i = 1; i < pathSize; i++) {
            cout << path[i] << endl;
        }
        return FOUND;
    }

    int min = MAX_INT_SIZE;
    int temp;

    if ((pathSize == 0 || strcmp(path[pathSize - 1], "left") != 0) && moveRight()) {
        addToPath("right");
        temp = search(g + 1, threshold);
        if (temp == FOUND) {
            return FOUND;
        }
        if (temp < min) {
            min = temp;
        }
        removeFromPath();
        moveLeft();
    }
    if ((pathSize == 0 || strcmp(path[pathSize - 1], "right") != 0) && moveLeft()) {
        addToPath("left");
        temp = search(g + 1, threshold);
        if (temp == FOUND) {
            return FOUND;
        }
        if (temp < min) {
            min = temp;
        }
        removeFromPath();
        moveRight();
    }
    if ((pathSize == 0 || strcmp(path[pathSize - 1], "down") != 0) && moveUp()) {
        addToPath("up");
        temp = search(g + 1, threshold);
        if (temp == FOUND) {
            return FOUND;
        }
        if (temp < min) {
            min = temp;
        }
        removeFromPath();
        moveDown();
    }
    if ((pathSize == 0 || strcmp(path[pathSize - 1], "up") != 0) && moveDown()) {
        addToPath("down");
        temp = search(g + 1, threshold);
        if (temp == FOUND) {
            return FOUND;
        }
        if (temp < min) {
            min = temp;
        }
        removeFromPath();
        moveUp();
    }

    return min;
}

void idaStar() {
    int threshold = manhattanDist();
    path = new char* [pathCapacity];
    addToPath("init");
    int temp;

    while (true) {
        temp = search(0, threshold);
        if (temp == FOUND) {
            break;
        }
        threshold = temp;
    }
}

bool isSolvable() {
    int inversions = 0;
    for (int i = 0; i < boardSize * boardSize - 1; i++) {
        if (tiles[i] == 0) continue;
        for (int j = i + 1; j < boardSize * boardSize; j++) {
            if (tiles[j] == 0) continue;
            if (tiles[i] > tiles[j]) {
                inversions++;
            }
        }
    }
    if (boardSize % 2 != 0) {
        return (inversions % 2 == 0);
    }
    else {
        int rowOfZero = currentTileIndexOfZero / boardSize;
        return (inversions + rowOfZero) % 2 != 0;
    }
}

int main() {
    int n, index;
    cin >> n >> index;
    tiles = new int[n + 1];
    int tile;
    for (int i = 0; i <= n; i++) {
        cin >> tile;
        tiles[i] = tile;
    }

    boardSize = squareRoot(n + 1);

    if (index == -1) {
        goalIndexOfZero = boardSize * boardSize - 1;
    }
    else {
        goalIndexOfZero = index;
    }

    currentTileIndexOfZero = posOfZero();

    if (!isSolvable()) {
        cout << -1 << endl;
        delete[] tiles;
        return 0;
    }

    auto start = chrono::high_resolution_clock::now();

    idaStar();

    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;

    cout << duration.count() << endl;

    delete[] tiles;
    for (int i = 0; i < pathSize; i++) {
        delete[] path[i];
    }
    delete[] path;
    return 0;
}