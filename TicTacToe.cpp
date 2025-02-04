#include <iostream>
#include <vector>
#include <algorithm>

struct Move {
    int row, col;

    Move() {
        row = 0;
        col = 0;
    }

    Move(int r, int c) {
        row = r;
        col = c;
    }
};

struct Game {
    char board[3][3];
    char currentTurn;

    Game(char currentTurn) {
        this->currentTurn = currentTurn;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                board[i][j] = ' ';
            }
        }
    }

    bool isGameOver() const {
        return hasWinner('X') || hasWinner('O') || getAvailableMoves().empty();
    }

    bool hasWinner(char player) const {
        for (int i = 0; i < 3; ++i) {
            if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
                return true;
            }
        }
        for (int j = 0; j < 3; ++j) {
            if (board[0][j] == player && board[1][j] == player && board[2][j] == player) {
                return true;
            }
        }
        if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
            return true;
        }
        if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
            return true;
        }
        return false;
    }

    std::vector<Move> getAvailableMoves() const {
        std::vector<Move> availableMoves;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                if (board[i][j] == ' ') {
                    availableMoves.emplace_back(i, j);
                }
            }
        }
        return availableMoves;
    }

    Game getNextState(int row, int col) const {
        Game newState = *this;
        newState.board[row][col] = currentTurn;
        newState.currentTurn = (currentTurn == 'X') ? 'O' : 'X';
        return newState;
    }

    char getCurrentPlayer() const {
        return currentTurn;
    }

    void displayBoard() const {
        std::cout << std::endl;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::cout << board[i][j];
                if (j < 2) std::cout << " | ";
            }
            std::cout << std::endl;
            if (i < 2) std::cout << "--+---+--" << std::endl;
        }
        std::cout << std::endl;
    }
};

int evaluateBoard(const Game& game, char currentPlayer, char opponent, int depth) {
    if (game.hasWinner(currentPlayer)) {
        return 10 - depth;
    }
    if (game.hasWinner(opponent)) {
        return depth - 10;
    }
    return 0;
}

int minMaxAlgorithm(Game& game, int depth, char currentPlayer, char opponent, Move& bestMove, int alpha, int beta) {
    if (game.isGameOver()) {
        return evaluateBoard(game, currentPlayer, opponent, depth);
    }

    std::vector<Move> availableMoves = game.getAvailableMoves();
    int bestScore = (game.getCurrentPlayer() == currentPlayer) ? -1000 : 1000;

    for (const auto& move : availableMoves) {
        Game nextState = game.getNextState(move.row, move.col);
        Move tempBestMove;
        int score = minMaxAlgorithm(nextState, depth + 1, currentPlayer, opponent, tempBestMove, alpha, beta);

        if (game.getCurrentPlayer() == currentPlayer) {
            if (score > bestScore) {
                bestScore = score;
                bestMove = move;
            }
            alpha = std::max(alpha, bestScore);
        }
        else {
            if (score < bestScore) {
                bestScore = score;
                bestMove = move;
            }
            beta = std::min(beta, bestScore);
        }

        if (beta <= alpha) {
            break;
        }
    }

    return bestScore;
}

int main() {
    Game game('X');
    char playerChar = 'X', computerChar = 'O';
    int firstPlayer;

    std::cout << "Player(1) or Computer(2) is first?" << std::endl << std::endl;
    std::cin >> firstPlayer;
    if (firstPlayer == 2) {
        std::swap(playerChar, computerChar);
    }

    while (!game.isGameOver()) {
        game.displayBoard();

        if (game.getCurrentPlayer() == playerChar) {
            int row, col;
            std::cout << "Please enter your move:" << std::endl << std::endl;

            while (true) {
                std::cin >> row >> col;
                row--; col--;

                if (row < 0 || row >= 3 || col < 0 || col >= 3) {
                    std::cout << std::endl << "Invalid input. Please try again." << std::endl << std::endl;
                    continue;
                }

                if (game.board[row][col] != ' ') {
                    std::cout << std::endl << "This position is already taken. Please try again." << std::endl << std::endl;
                    continue;
                }

                break;
            }

            game = game.getNextState(row, col);
        }
        else {
            Move computerMove;
            minMaxAlgorithm(game, 0, computerChar, playerChar, computerMove, -1000, 1000);
            game = game.getNextState(computerMove.row, computerMove.col);
            std::cout << "Computer plays: [" << computerMove.row + 1 << ", " << computerMove.col + 1 << "]" << std::endl;
        }
    }

    game.displayBoard();
    if (game.hasWinner(playerChar)) {
        std::cout << "Player wins!" << std::endl;
    }
    else if (game.hasWinner(computerChar)) {
        std::cout << "Computer wins!" << std::endl;
    }
    else {
        std::cout << "It's a draw!" << std::endl;
    }

    return 0;
}
