#include <iostream>
#include <algorithm>
#include <cstring>
#include "HexagonList.h"


#define MAX_QUERY_SIZE 210
#define NUMBER_OF_EDGES_OF_HEXAGON 6
#define STARTING_EDGE 0

typedef enum Player {
    BLUE = -1,
    NONE = 0,
    RED = 1
} Player;

typedef struct GameStatus {
    Player** board;
    int boardSize;
    int numberOfBoardTags; // '---' tag
    int numberOfRedPawns;
    int numberOfBluePawns;
    Player playerTurn;
    HexagonList emptyHexagons;
    bool hexagonsHasBeenReduced;
} GameStatus;


void initializeGameStatus(GameStatus &gameStatus);
void handleQuery(char *query, GameStatus &gameStatus);
void handleAllocatingAndDeallocatingBoard(char *query, GameStatus &gameStatus);
void allocateBoard(GameStatus &gameStatus);
void deallocateBoard(GameStatus &gameStatus);
void initializeBoard(GameStatus &gameStatus);
int getBoardSize(char *query);
short getNumberOfSpacesToChar(char *query, char delimiter);
void handleCountingPawnsInQuery(char *query, GameStatus &gameStatus);
void setPlayerTurn(GameStatus &gameStatus);
void handlePlacingPawns(char *query, GameStatus &gameStatus);
void placePawnsOnDiagonal(char *query, int startPositionX, int startPositionY, GameStatus &gameStatus);
Player charToPlayer(char c);
void handleBoardSizeQuery(char *query, GameStatus &gameStatus);
void handlePawnsNumberQuery(char *query, GameStatus &gameStatus);
int getPawnsNumber(GameStatus &gameStatus);
void handleIsBoardCorrectQuery(char *query, GameStatus &gameStatus);
bool checkIfBoardIsCorrect(GameStatus &gameStatus);
void handleGameOverQuery(char *query, GameStatus &gameStatus);
Player getWinnerOfGame(GameStatus &gameStatus);
bool** allocateVisitedHexagons(GameStatus &gameStatus);
void deallocateVisitedHexagons(GameStatus &gameStatus, bool** visitedHexagons);
void initializeVisitedHexagons(GameStatus &gameStatus, bool** visitedHexagons);
bool DFS(int rootX, int rootY, Player pawnType, bool** visitedHexagons, int startingEdge, GameStatus &gameStatus);
bool checkIfPositionIsOppositeEdgeForPawn(int x, int y, int startingEdge, Player pawnType, GameStatus &gameStatus);
bool checkIfPositionIsValidForDFS(int x, int y, bool** visitedHexagons, Player pawnType, GameStatus &gameStatus);
void handleIsBoardPossibleQuery(char *query, GameStatus &gameStatus);
bool checkIfBoardIsPossible(GameStatus &gameStatus);
void handleCanPlayerWinInOneMoveWithNaiveOpponentQuery(char *query, Player player, GameStatus &gameStatus);
bool checkIfGameCanBeContinued(GameStatus &gameStatus, Player currentPlayer, int inMoves);
int calculateDepthOfCalculations(Player player, int moves, GameStatus &gameStatus);
bool checkIfPlayerHasEnoughPawnsOnBoardToWinInNMoves(Player player, int nMoves, GameStatus &gameStatus);
bool checkIfThereIsEnoughEmptyHexagonsOnBoardToWinInNMoves(int depth, GameStatus &gameStatus);
bool canPlayerWinInOneMoveWithNaiveOpponent(Player player, GameStatus &gameStatus);
bool pawnIsConnectedWithTwoEdges(int x, int y, Player player, GameStatus &gameStatus);
void handleCanPlayerWinInTwoMovesWithNaiveOpponentQuery(char *query, Player player, GameStatus &gameStatus);
bool canPlayerWinInTwoMovesWithNaiveOpponent(Player player, GameStatus &gameStatus);
void handleCanPlayerWinInNMovesWithPerfectOpponentQuery(char *query, Player player, int moves, GameStatus &gameStatus);
void reduceUselessEmptyHexagonsForMinimax(GameStatus &gameStatus, Player player);
bool findAndAppendAllOneWinningMovesOfPlayer(GameStatus &gameStatus, Player player, HexagonList &oneWinningMoves);
void findAndAppendAllTwoWinningMovesOfPlayer(GameStatus &gameStatus, Player player, HexagonList &twoWinningMoves);
void findAndAppendOneEmptyHexagonNotNeededForWinning(GameStatus &gameStatus, HexagonList &winningHexagons);
int minimax(GameStatus &gameStatus, Player player, Player expectedWInner, int depth, int alpha, int beta, bool isMaximizing);
int evaluateBoardForMinimax(GameStatus &gameStatus, Player player);
bool playerHasWon(GameStatus &gameStatus, Player player);


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    GameStatus gameStatus;
    char query[MAX_QUERY_SIZE];
    initializeGameStatus(gameStatus);

    while (!std::cin.getline(query, MAX_QUERY_SIZE).eof()) {
        handleQuery(query, gameStatus);
    }
    deallocateBoard(gameStatus);
    return 0;
}

void initializeGameStatus(GameStatus &gameStatus) {
    gameStatus.board = nullptr;
    gameStatus.boardSize = 0;
    gameStatus.numberOfBoardTags = 0;
    gameStatus.numberOfRedPawns = 0;
    gameStatus.numberOfBluePawns = 0;
    gameStatus.playerTurn = NONE;
    gameStatus.emptyHexagons.clear();
    gameStatus.hexagonsHasBeenReduced = false;
}

void handleQuery(char *query, GameStatus &gameStatus) {
    if (strstr(query, "---") != nullptr) {
        handleAllocatingAndDeallocatingBoard(query, gameStatus);
    } else if (strstr(query, "<") != nullptr) {
        handleCountingPawnsInQuery(query, gameStatus);
        handlePlacingPawns(query, gameStatus);
    } else if (strcmp(query, "BOARD_SIZE") == 0) {
        handleBoardSizeQuery(query, gameStatus);
    } else if (strcmp(query, "PAWNS_NUMBER") == 0) {
        handlePawnsNumberQuery(query, gameStatus);
    } else if (strcmp(query, "IS_BOARD_CORRECT") == 0) {
        handleIsBoardCorrectQuery(query, gameStatus);
    } else if (strcmp(query, "IS_GAME_OVER") == 0) {
        handleGameOverQuery(query, gameStatus);
    } else if (strcmp(query, "IS_BOARD_POSSIBLE") == 0) {
        handleIsBoardPossibleQuery(query, gameStatus);
    } else if (strcmp(query, "CAN_RED_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT") == 0) {
        handleCanPlayerWinInOneMoveWithNaiveOpponentQuery(query, RED, gameStatus);
    } else if (strcmp(query, "CAN_BLUE_WIN_IN_1_MOVE_WITH_NAIVE_OPPONENT") == 0) {
        handleCanPlayerWinInOneMoveWithNaiveOpponentQuery(query, BLUE, gameStatus);
    } else if (strcmp(query, "CAN_RED_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT") == 0) {
        handleCanPlayerWinInTwoMovesWithNaiveOpponentQuery(query, RED, gameStatus);
    } else if (strcmp(query, "CAN_BLUE_WIN_IN_2_MOVES_WITH_NAIVE_OPPONENT") == 0) {
        handleCanPlayerWinInTwoMovesWithNaiveOpponentQuery(query, BLUE, gameStatus);
    } else if (strcmp(query, "CAN_RED_WIN_IN_1_MOVE_WITH_PERFECT_OPPONENT") == 0) {
        handleCanPlayerWinInNMovesWithPerfectOpponentQuery(query, RED, 1, gameStatus);
    } else if (strcmp(query, "CAN_BLUE_WIN_IN_1_MOVE_WITH_PERFECT_OPPONENT") == 0) {
        handleCanPlayerWinInNMovesWithPerfectOpponentQuery(query, BLUE, 1, gameStatus);
    } else if (strcmp(query, "CAN_RED_WIN_IN_2_MOVES_WITH_PERFECT_OPPONENT") == 0) {
        handleCanPlayerWinInNMovesWithPerfectOpponentQuery(query, RED, 2, gameStatus);
    } else if (strcmp(query, "CAN_BLUE_WIN_IN_2_MOVES_WITH_PERFECT_OPPONENT") == 0) {
        handleCanPlayerWinInNMovesWithPerfectOpponentQuery(query, BLUE, 2, gameStatus);
        printf("\n");
    }
}

void handleAllocatingAndDeallocatingBoard(char *query, GameStatus &gameStatus) {
    if (gameStatus.numberOfBoardTags < 3) {
        gameStatus.numberOfBoardTags++;
    }
    if (gameStatus.numberOfBoardTags == 3) { // Opening tag of board in the next board
        deallocateBoard(gameStatus);
        initializeGameStatus(gameStatus);
        gameStatus.numberOfBoardTags++;
    }
    if (gameStatus.numberOfBoardTags == 1) { // Opening tag of board
        gameStatus.boardSize = getBoardSize(query);
        allocateBoard(gameStatus);
        initializeBoard(gameStatus);
    }
    if (gameStatus.numberOfBoardTags == 2) { // Closing tag of board
        setPlayerTurn(gameStatus);
    }
}

void allocateBoard(GameStatus &gameStatus) {
    gameStatus.board = new Player*[gameStatus.boardSize];
    for (int i = 0; i < gameStatus.boardSize; i++) {
        gameStatus.board[i] = new Player[gameStatus.boardSize];
    }
}

void deallocateBoard(GameStatus &gameStatus) {
    for (int i = 0; i < gameStatus.boardSize; i++) {
        delete[] gameStatus.board[i];
    }
    delete[] gameStatus.board;
}

void initializeBoard(GameStatus &gameStatus) {
    for (int i = 0; i < gameStatus.boardSize; i++) {
        for (int j = 0; j < gameStatus.boardSize; j++) {
            gameStatus.board[i][j] = NONE;
        }
    }
}

int getBoardSize(char *query) {
    int numberOfSpaces = getNumberOfSpacesToChar(query, '-');
    int boardSize = numberOfSpaces >= 1 ? 1 : 0;
    boardSize += (numberOfSpaces - 1) / 3;

    return boardSize;
}

short getNumberOfSpacesToChar(char *query, char delimiter) {
    short numberOfSpaces = 0;
    for (int i = 0; i < strlen(query); i++) {
        if (query[i] != delimiter) {
            numberOfSpaces++;
        }
    }

    return numberOfSpaces;
}

void handleCountingPawnsInQuery(char *query, GameStatus &gameStatus) {
    for (int i = 0; i < strlen(query); i++) {
        if (query[i] == 'r') {
            gameStatus.numberOfRedPawns++;
        }
        else if (query[i] == 'b') {
            gameStatus.numberOfBluePawns++;
        }
    }
}

void setPlayerTurn(GameStatus &gameStatus) {
    if (gameStatus.numberOfRedPawns == gameStatus.numberOfBluePawns) {
        gameStatus.playerTurn = RED;
    }
    else if (gameStatus.numberOfRedPawns == gameStatus.numberOfBluePawns + 1) {
        gameStatus.playerTurn = BLUE;
    }
    else {
        gameStatus.playerTurn = NONE;
    }
}

// Read in zigzag order
void handlePlacingPawns(char *query, GameStatus &gameStatus) {
    static int startPositionX = -1; // Start from -1 to start from 0
    static int startPositionY = 0;

    if (startPositionX < gameStatus.boardSize - 1) {
        startPositionX++;
    }
    else if ((startPositionX == gameStatus.boardSize - 1) && (startPositionY < gameStatus.boardSize - 1)) {
        startPositionY++;
    }

    placePawnsOnDiagonal(query, startPositionX, startPositionY, gameStatus);

    if (startPositionY == gameStatus.boardSize - 1) {
        startPositionX = -1;
        startPositionY = 0;
    }
}

void placePawnsOnDiagonal(char *query, int startPositionX, int startPositionY, GameStatus &gameStatus) {
    int offsetX = 1;
    int offsetY = 1;
    int distanceBetweenOpenSquareBracketAndPawn = 2;

    do {
        query = strstr(query, "<") + distanceBetweenOpenSquareBracketAndPawn;
        gameStatus.board[startPositionX][startPositionY] = charToPlayer(query[0]);

        if (query[0] == ' ') // Add empty hexagon to the list.
            gameStatus.emptyHexagons.append(startPositionX, startPositionY);

        startPositionX -= offsetX;
        startPositionY += offsetY;

        query = strstr(query, "<");
    } while (query != nullptr);
}

Player charToPlayer(char c) {
    if (c == 'r') {
        return RED;
    }
    else if (c == 'b') {
        return BLUE;
    }
    else {
        return NONE;
    }
}

void handleBoardSizeQuery(char *query, GameStatus &gameStatus) {
    printf("%d\n", gameStatus.boardSize);
}

void handlePawnsNumberQuery(char *query, GameStatus &gameStatus) {
    printf("%d\n", getPawnsNumber(gameStatus));
}

int getPawnsNumber(GameStatus &gameStatus) {
    int pawnsNumber = 0;
    for (int i = 0; i < gameStatus.boardSize; i++) {
        for (int j = 0; j < gameStatus.boardSize; j++) {
            if (gameStatus.board[i][j] != NONE) {
                pawnsNumber++;
            }
        }
    }

    return pawnsNumber;
}

void handleIsBoardCorrectQuery(char *query, GameStatus &gameStatus) {
    if (checkIfBoardIsCorrect(gameStatus)) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

bool checkIfBoardIsCorrect(GameStatus &gameStatus) {
    return gameStatus.numberOfRedPawns >= gameStatus.numberOfBluePawns && (abs(gameStatus.numberOfRedPawns - gameStatus.numberOfBluePawns) <= 1);
}

void handleGameOverQuery(char *query, GameStatus &gameStatus) {
    bool isBoardCorrect = checkIfBoardIsCorrect(gameStatus);
    if (!isBoardCorrect) {
        printf("NO\n\n");
        return;
    }

    Player winner = getWinnerOfGame(gameStatus);
    if (winner == NONE) {
        printf("NO\n");
    }
    else {
        printf("YES %s\n", winner == RED ? "RED" : "BLUE");
    }
}

Player getWinnerOfGame(GameStatus &gameStatus) {
    bool** visitedHexagons = allocateVisitedHexagons(gameStatus);

    // DFS for red pawns on edge starting from STARTING_EDGE
    for (int i = 0; i < gameStatus.boardSize; i++) {
        initializeVisitedHexagons(gameStatus, visitedHexagons);
        if (gameStatus.board[i][STARTING_EDGE] == RED) {
            if (DFS(i, STARTING_EDGE, RED, visitedHexagons, STARTING_EDGE, gameStatus)) {
                deallocateVisitedHexagons(gameStatus, visitedHexagons);
                return RED;
            }
        }
    }

    // DFS for blue pawns on edge starting from STARTING_EDGE
    for (int i = 0; i < gameStatus.boardSize; i++) {
        initializeVisitedHexagons(gameStatus, visitedHexagons);
        if (gameStatus.board[STARTING_EDGE][i] == BLUE) {
            if (DFS(STARTING_EDGE, i, BLUE, visitedHexagons, STARTING_EDGE, gameStatus)) {
                deallocateVisitedHexagons(gameStatus, visitedHexagons);
                return BLUE;
            }
        }
    }

    deallocateVisitedHexagons(gameStatus, visitedHexagons);
    return NONE;
}

bool** allocateVisitedHexagons(GameStatus &gameStatus) {
    bool** visitedHexagons = new bool*[gameStatus.boardSize];
    for (int i = 0; i < gameStatus.boardSize; i++) {
        visitedHexagons[i] = new bool[gameStatus.boardSize];
    }

    return visitedHexagons;
}

void deallocateVisitedHexagons(GameStatus &gameStatus, bool** visitedHexagons) {
    for (int i = 0; i < gameStatus.boardSize; i++) {
        delete[] visitedHexagons[i];
    }
    delete[] visitedHexagons;
}

void initializeVisitedHexagons(GameStatus &gameStatus, bool** visitedHexagons) {
    for (int i = 0; i < gameStatus.boardSize; i++) {
        for (int j = 0; j < gameStatus.boardSize; j++) {
            visitedHexagons[i][j] = false;
        }
    }
}

bool DFS(int rootX, int rootY, Player pawnType, bool** visitedHexagons, int startingEdge, GameStatus &gameStatus) {
    visitedHexagons[rootX][rootY] = true;

    if (checkIfPositionIsOppositeEdgeForPawn(rootX, rootY, startingEdge, pawnType, gameStatus)) {
        return true;
    }

    // Next hexagons to visit
    int rowOffset[] = {0, -1, -1, 0, 1, 1};
    int colOffset[] = {-1, -1, 0, 1, 1, 0};

    for (int i = 0; i < NUMBER_OF_EDGES_OF_HEXAGON; i++) {
        int newRootX = rootX + rowOffset[i];
        int newRootY = rootY + colOffset[i];
        if (checkIfPositionIsValidForDFS(newRootX, newRootY, visitedHexagons, pawnType, gameStatus)) {
            if (DFS(newRootX, newRootY, pawnType, visitedHexagons, startingEdge, gameStatus)) {
                return true;
            }
        }
    }

    return false;
}

bool checkIfPositionIsOppositeEdgeForPawn(int x, int y, int startingEdge, Player pawnType, GameStatus &gameStatus) {
    int oppositeEdge = startingEdge == 0 ? gameStatus.boardSize - 1 : 0;
    if (pawnType == RED && y == oppositeEdge) {
        return true;
    }
    else if (pawnType == BLUE && x == oppositeEdge) {
        return true;
    }
    else {
        return false;
    }
}

bool checkIfPositionIsValidForDFS(int x, int y, bool** visitedHexagons, Player pawnType, GameStatus &gameStatus) {
    return (x >= 0 && x < gameStatus.boardSize && y >= 0 && y < gameStatus.boardSize && !visitedHexagons[x][y] && gameStatus.board[x][y] == pawnType);
}

void handleIsBoardPossibleQuery(char *query, GameStatus &gameStatus) {
    bool isBoardCorrect = checkIfBoardIsCorrect(gameStatus);
    if (!isBoardCorrect) {
        printf("NO\n");
        return;
    }

    bool isBoardPossible = checkIfBoardIsPossible(gameStatus);
    if (isBoardPossible) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

bool checkIfBoardIsPossible(GameStatus &gameStatus) {
    Player winner = getWinnerOfGame(gameStatus);
    if (winner == RED && gameStatus.numberOfRedPawns != gameStatus.numberOfBluePawns + 1)
        return false;
    else if (winner == BLUE && gameStatus.numberOfBluePawns != gameStatus.numberOfRedPawns)
        return false;
    else if (winner == NONE)
        return true;

    for (int i = 0; i < gameStatus.boardSize; i++) {
        for (int j = 0; j < gameStatus.boardSize; j++) {
            if (gameStatus.board[i][j] == winner) {
                gameStatus.board[i][j] = NONE;
                if (getWinnerOfGame(gameStatus) == NONE) {
                    gameStatus.board[i][j] = winner;
                    return true;
                }
                gameStatus.board[i][j] = winner;
            }
        }
    }

    return false;
}

void handleCanPlayerWinInOneMoveWithNaiveOpponentQuery(char *query, Player player, GameStatus &gameStatus) {
    if (!checkIfGameCanBeContinued(gameStatus, player, 1)) {
        printf("NO\n");
        return;
    }

    bool canPlayerWinInOneMove = canPlayerWinInOneMoveWithNaiveOpponent(player, gameStatus);
    if (canPlayerWinInOneMove) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

bool checkIfGameCanBeContinued(GameStatus &gameStatus, Player currentPlayer, int inMoves) {
    if (!checkIfPlayerHasEnoughPawnsOnBoardToWinInNMoves(currentPlayer, inMoves, gameStatus))
        return false;
    if (!checkIfThereIsEnoughEmptyHexagonsOnBoardToWinInNMoves(calculateDepthOfCalculations(currentPlayer, inMoves, gameStatus), gameStatus))
        return false;
    if (!checkIfBoardIsCorrect(gameStatus))
        return false;

    bool isSomeoneWinner = getWinnerOfGame(gameStatus) != NONE;
    if (isSomeoneWinner)
        return false;

    return true;
}

int calculateDepthOfCalculations(Player player, int moves, GameStatus &gameStatus) {
    if (gameStatus.playerTurn == player && moves == 1)
        return 1;
    else if (gameStatus.playerTurn == player && moves == 2)
        return 3;
    else if (gameStatus.playerTurn != player && moves == 1)
        return 2;
    else if (gameStatus.playerTurn != player && moves == 2)
        return 4;
    else
        return 0;
}

bool checkIfPlayerHasEnoughPawnsOnBoardToWinInNMoves(Player player, int nMoves, GameStatus &gameStatus) {
    if (player == RED && gameStatus.numberOfRedPawns + nMoves < gameStatus.boardSize)
        return false;
    else if (player == BLUE && gameStatus.numberOfBluePawns + nMoves < gameStatus.boardSize)
        return false;
    else
        return true;
}

bool checkIfThereIsEnoughEmptyHexagonsOnBoardToWinInNMoves(int depth, GameStatus &gameStatus) {
    if (gameStatus.emptyHexagons.getSize() < depth) {
        return false;
    }
    else
        return true;
}

bool canPlayerWinInOneMoveWithNaiveOpponent(Player player, GameStatus &gameStatus) {
    Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
    while (currentHexagon != nullptr) {
        int x = currentHexagon->getX();
        int y = currentHexagon->getY();
        gameStatus.board[x][y] = player;
        if (pawnIsConnectedWithTwoEdges(x, y, player, gameStatus)) {
            gameStatus.board[x][y] = NONE;
            return true;
        }
        gameStatus.board[x][y] = NONE;
        currentHexagon = currentHexagon->getNext();
    }
    return false;
}

bool pawnIsConnectedWithTwoEdges(int x, int y, Player player, GameStatus &gameStatus) {
    bool** visitedHexagons = allocateVisitedHexagons(gameStatus);
    initializeVisitedHexagons(gameStatus, visitedHexagons);
    bool isConnectedWithEndEdge = DFS(x, y, player, visitedHexagons, STARTING_EDGE, gameStatus);
    initializeVisitedHexagons(gameStatus, visitedHexagons);
    bool isConnectedWithStartEdge = DFS(x, y, player, visitedHexagons, gameStatus.boardSize - 1, gameStatus);
    deallocateVisitedHexagons(gameStatus, visitedHexagons);

    return isConnectedWithEndEdge && isConnectedWithStartEdge;
}

void handleCanPlayerWinInTwoMovesWithNaiveOpponentQuery(char *query, Player player, GameStatus &gameStatus) {
    if (!checkIfGameCanBeContinued(gameStatus, player, 2)) {
        printf("NO\n");
        return;
    }

    bool canPlayerWinInTwoMoves = canPlayerWinInTwoMovesWithNaiveOpponent(player, gameStatus);
    if (canPlayerWinInTwoMoves) {
        printf("YES\n");
    }
    else {
        printf("NO\n");
    }
}

bool canPlayerWinInTwoMovesWithNaiveOpponent(Player player, GameStatus &gameStatus) {
    Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
    while (currentHexagon != nullptr) {
        int x = currentHexagon->getX();
        int y = currentHexagon->getY();
        gameStatus.board[x][y] = player;
        if (pawnIsConnectedWithTwoEdges(x, y, player, gameStatus)) { // We care about winning in two moves
            gameStatus.board[x][y] = NONE;
            currentHexagon = currentHexagon->getNext();
            continue;
        }
        if (canPlayerWinInOneMoveWithNaiveOpponent(player, gameStatus)) {
            gameStatus.board[x][y] = NONE;
            return true;
        }
        gameStatus.board[x][y] = NONE;
        currentHexagon = currentHexagon->getNext();
    }

    return false;
}

void handleCanPlayerWinInNMovesWithPerfectOpponentQuery(char *query, Player player, int moves, GameStatus &gameStatus) {
    if (!checkIfGameCanBeContinued(gameStatus, player, moves)) {
        printf("NO\n");
        return;
    }

    int depth = calculateDepthOfCalculations(player, moves, gameStatus);

    // Reducing empty hexagons for minimax is O(n^3) operation, so we do it only for depth > 2 only once.
    if (depth > 2 && !gameStatus.hexagonsHasBeenReduced) {
        reduceUselessEmptyHexagonsForMinimax(gameStatus, player);
        gameStatus.hexagonsHasBeenReduced = true;
    }

    if (gameStatus.emptyHexagons.getSize() < depth) { // No enough significant empty hexagons. Player can't win in n moves.
        printf("NO\n");
        return;
    }

    int alpha = -1;
    int beta = 1;
    if (depth == 1 || depth == 3) { // Player starts
        int minimaxValue = minimax(gameStatus, player, player, depth, alpha, beta, true);
        if (minimaxValue > 0) {
            printf("YES\n");
            return;
        }
    } else if (depth == 2 || depth == 4) { // Enemy starts
        Player opponent = player == RED ? BLUE : RED;
        int minimaxValue = minimax(gameStatus, opponent, player, depth, alpha, beta, false);
        if (minimaxValue > 0) {
            printf("YES\n");
            return;
        }
    }

    printf("NO\n");
}

void reduceUselessEmptyHexagonsForMinimax(GameStatus &gameStatus, Player player) {
    Player enemy = player == RED ? BLUE : RED;

    HexagonList newEmptyHexagons;
    findAndAppendAllOneWinningMovesOfPlayer(gameStatus, player, newEmptyHexagons);
    findAndAppendAllTwoWinningMovesOfPlayer(gameStatus, enemy, newEmptyHexagons);
    findAndAppendAllOneWinningMovesOfPlayer(gameStatus, enemy, newEmptyHexagons);
    findAndAppendAllTwoWinningMovesOfPlayer(gameStatus, player, newEmptyHexagons);
    findAndAppendOneEmptyHexagonNotNeededForWinning(gameStatus, newEmptyHexagons); // In case if someone doesn't want to win in this move =)

    gameStatus.emptyHexagons = newEmptyHexagons;
}

bool findAndAppendAllOneWinningMovesOfPlayer(GameStatus &gameStatus, Player player, HexagonList &oneWinningMoves) {
    bool found = false;

    Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
    while (currentHexagon != nullptr) {
        if (gameStatus.board[currentHexagon->getX()][currentHexagon->getY()] == NONE) {
            int x = currentHexagon->getX();
            int y = currentHexagon->getY();
            gameStatus.board[x][y] = player;
            if (!oneWinningMoves.elementIsInList(x, y) && pawnIsConnectedWithTwoEdges(x, y, player, gameStatus)) {
                oneWinningMoves.append(x, y);
                found = true;
            }
            gameStatus.board[x][y] = NONE;
        }
        currentHexagon = currentHexagon->getNext();
    }

    return found;
}

void findAndAppendAllTwoWinningMovesOfPlayer(GameStatus &gameStatus, Player player, HexagonList &twoWinningMoves) {
    Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
    while (currentHexagon != nullptr) {
        if (gameStatus.board[currentHexagon->getX()][currentHexagon->getY()] == NONE) {
            int x = currentHexagon->getX();
            int y = currentHexagon->getY();
            gameStatus.board[x][y] = player;
            if(findAndAppendAllOneWinningMovesOfPlayer(gameStatus, player, twoWinningMoves)) {
                if (!twoWinningMoves.elementIsInList(x, y)) {
                    twoWinningMoves.append(x, y);
                }
            }
            gameStatus.board[x][y] = NONE;
        }
        currentHexagon = currentHexagon->getNext();
    }
}

void findAndAppendOneEmptyHexagonNotNeededForWinning(GameStatus &gameStatus, HexagonList &winningHexagons) {
    Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
    while (currentHexagon != nullptr) {
        if (!winningHexagons.elementIsInList(currentHexagon->getX(), currentHexagon->getY())) {
            winningHexagons.append(currentHexagon->getX(), currentHexagon->getY());
            break;
        }
        currentHexagon = currentHexagon->getNext();
    }
}

int minimax(GameStatus &gameStatus, Player player, Player expectedWInner, int depth, int alpha, int beta, bool isMaximizing) {
    Player enemy = player == RED ? BLUE : RED;
    if (depth == 0)
        return evaluateBoardForMinimax(gameStatus, expectedWInner);

    if (isMaximizing) {
        int best = -1;
        Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
        while (currentHexagon != nullptr) {
            int i = currentHexagon->getX();
            int j = currentHexagon->getY();
            if (gameStatus.board[i][j] == NONE) {
                gameStatus.board[i][j] = player;
                if (depth > 2 && pawnIsConnectedWithTwoEdges(i, j, player, gameStatus)) {
                    gameStatus.board[i][j] = NONE;
                    currentHexagon = currentHexagon->getNext();
                    continue;
                }
                int c = std::max(best, minimax(gameStatus, enemy, expectedWInner, depth - 1, alpha, beta, !isMaximizing));
                best = std::max(best, c);
                gameStatus.board[i][j] = NONE;
                alpha = std::max(alpha, best);
                if (beta <= alpha)
                    break;
            }
            currentHexagon = currentHexagon->getNext();
        }
        return best;
    } else {
        int best = 1;
        Hexagon* currentHexagon = gameStatus.emptyHexagons.getHead();
        while (currentHexagon != nullptr) {
            int i = currentHexagon->getX();
            int j = currentHexagon->getY();
            if (gameStatus.board[i][j] == NONE) {
                gameStatus.board[i][j] = player;
                int c = std::min(best, minimax(gameStatus, enemy, expectedWInner, depth - 1, alpha, beta, !isMaximizing));
                gameStatus.board[i][j] = NONE;
                best = std::min(best, c);
                beta = std::min(beta, best);
                if (beta <= alpha)
                    break;
            }
            currentHexagon = currentHexagon->getNext();
        }
        return best;
    }
}

int evaluateBoardForMinimax(GameStatus &gameStatus, Player player) {
    Player opponent = player == RED ? BLUE : RED;
    if (playerHasWon(gameStatus, player)) {
        return 1;
    } else if (playerHasWon(gameStatus, opponent)) {
        return -1;
    }
    else {
        return 0;
    }
}

bool playerHasWon(GameStatus &gameStatus, Player player) {
    bool** visitedHexagons = allocateVisitedHexagons(gameStatus);
    if (player == RED) {
        // DFS for red pawns on edge starting from STARTING_EDGE
        for (int i = 0; i < gameStatus.boardSize; i++) {
            initializeVisitedHexagons(gameStatus, visitedHexagons);
            if (gameStatus.board[i][STARTING_EDGE] == RED) {
                if (DFS(i, STARTING_EDGE, RED, visitedHexagons, STARTING_EDGE, gameStatus)) {
                    deallocateVisitedHexagons(gameStatus, visitedHexagons);
                    return RED;
                }
            }
        }
    } else if (player == BLUE) {
        // DFS for blue pawns on edge starting from STARTING_EDGE
        for (int i = 0; i < gameStatus.boardSize; i++) {
            initializeVisitedHexagons(gameStatus, visitedHexagons);
            if (gameStatus.board[STARTING_EDGE][i] == BLUE) {
                if (DFS(STARTING_EDGE, i, BLUE, visitedHexagons, STARTING_EDGE, gameStatus)) {
                    deallocateVisitedHexagons(gameStatus, visitedHexagons);
                    return BLUE;
                }
            }
        }
    }
    deallocateVisitedHexagons(gameStatus, visitedHexagons);
    return false;
}