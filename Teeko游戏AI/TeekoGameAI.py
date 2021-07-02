import copy
import time

def isDropState(state):
    count = 0
    for i in range(5):
        for j in range(5):
            if state[i][j] != ' ':
                count = count + 1
    return False if count == 8 else True

def checkWin(state, color):
    # check horizontal wins
    for row in state:
        for i in range(2):
            if row[i] != ' ' and row[i] == row[i + 1] == row[i + 2] == row[i + 3]:
                return 1 if row[i] == color else -1

    # check vertical wins
    for col in range(5):
        for i in range(2):
            if state[i][col] != ' ' and state[i][col] == state[i + 1][col] == state[i + 2][
                col] == state[i + 3][col]:
                return 1 if state[i][col] == color else -1

    # check \ diagonal wins
    for row in [0, 1]:
        for col in [0, 1]:
            if state[row][col] != ' ' and state[row][col] == state[row + 1][col + 1] == \
                    state[row + 2][col + 2] == state[row + 3][col + 3]:
                return 1 if state[row][col] == color else -1

    # check / diagonal wins
    for row in [0, 1]:
        for col in [3, 4]:
            if state[row][col] != ' ' and state[row][col] == state[row + 1][col - 1] == \
                    state[row + 2][col - 2] == state[row + 3][col - 3]:
                return 1 if state[row][col] == color else -1

    # check 2x2 box wins
    for row in range(4):
        for col in range(4):
            if state[row][col] != ' ' and state[row][col] == state[row + 1][col + 1] == \
                    state[row + 1][col] == state[row][col + 1]:
                return 1 if state[row][col] == color else -1

    return 0  # no winner yet


class Board:
    state = [[' ' for j in range(5)] for i in range(5)]

    def print(self):
        for row in range(len(self.state)):
            line = str(row) + ': '
            for cell in self.state[row]:
                line += cell + ' '
            print(line)
        print('   A B C D E\n')

class AiPlayer:
    color = 'b'
    opp = 'r'
    endtime = None
    def __init__(self, color):
        if color == 'r':
            self.color, self.opp = 'r', 'b'

    def makeMove(self, board):
        gamevalue = -float('inf')
        nextstates = self.successors(board.state, self.color)
        self.endtime = time.time() + 10
        starttime = time.time()
        for succ in nextstates:
            value = self.minMax(succ, 4, -float('inf'), float('inf'))
            if gamevalue < value:
                gamevalue = value
                board.state = succ
        print(str(round(time.time() - starttime, 2)) + 's')

    def maxMin(self, state, depth, alpha, beta):
        win = checkWin(state, self.color)
        if win != 0:
            return win
        if self.endtime - time.time() <= 1 or depth == 0:
            return self.heuristicEvaluation(state)
        for succ in self.successors(state, self.color):
            alpha = max(alpha, self.minMax(succ, depth - 1, alpha, beta))
            if alpha >= beta:
                return beta
        return alpha

    def minMax(self, state, depth, alpha, beta):
        win = checkWin(state, self.color)
        if win != 0:
            return win
        if self.endtime - time.time() <= 1 or depth == 0:
            return self.heuristicEvaluation(state)
        for succ in self.successors(state, self.opp):
            beta = min(beta, self.maxMin(succ, depth - 1, alpha, beta))
            if alpha >= beta:
                return alpha
        return beta

    def successors(self, state, color):
        nextstates = []
        if isDropState(state):
            for (i, j) in [(2, 2), (2, 3), (1, 3), (1, 2), (1, 1), (2, 1), (3, 1), (3, 2), (3, 3), (2, 4), (1, 4), (0, 4), (0, 3), (0, 2), (0, 1), (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (4, 1), (4, 2), (4, 3), (4, 4), (3, 4)]:
                if state[i][j] == ' ':
                    tmp = copy.deepcopy(state)
                    tmp[i][j] = color
                    nextstates.append(tmp)
        else:
            for (i, j) in [(2, 2), (2, 3), (1, 3), (1, 2), (1, 1), (2, 1), (3, 1), (3, 2), (3, 3), (2, 4), (1, 4), (0, 4), (0, 3), (0, 2), (0, 1), (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (4, 1), (4, 2), (4, 3), (4, 4), (3, 4)]:
                if state[i][j] == color:
                    for offset_i in range(-1, 2):
                        for offset_j in range(-1, 2):
                            if 0 <= i + offset_i <= 4 and 0 <= j + offset_j <= 4:
                                if state[i + offset_i][j + offset_j] == ' ':
                                    tmp = copy.deepcopy(state)
                                    tmp[i][j] = ' '
                                    tmp[i + offset_i][j + offset_j] = color
                                    nextstates.append(tmp)
        return nextstates

    def heuristicEvaluation(self, state):
        mygamevalue = 1
        oppvalue = 1
        weights = [
            [1, 2, 1, 2, 1],
            [2, 3, 3, 3, 2],
            [1, 3, 4, 3, 1],
            [2, 3, 3, 3, 2],
            [1, 2, 1, 2, 1]
        ]
        for i in range(5):
            for j in range(5):
                if state[i][j] == self.color:
                    mygamevalue = mygamevalue + weights[i][j]
                if state[i][j] == self.opp:
                    oppvalue = oppvalue + weights[i][j]
        return (mygamevalue - oppvalue) / 10


class HumanPlayer:
    color = 'b'
    opp = 'r'

    def __init__(self, color):
        if color == 'r':
            self.color, self.opp = 'r', 'b'

    def makeMove(self, board):
        if isDropState(board.state):
            move_to = input('Move (e.g. B3): ')
            while move_to[0] not in 'ABCDE' and move_to[1] not in '01234' and len(move_to) == 2:
                move_to = input('Move (e.g. B3): ')
            board.state[int(move_to[1])][ord(move_to[0]) - ord('A')] = self.color
        else:
            move_from = input('Move from (e.g. B3): ')
            while move_from[0] not in 'ABCDE' and move_from[1] not in '01234' and len(move_from) == 2:
                move_from = input('Move from (e.g. B3): ')
            move_to = input('Move to (e.g. B3): ')
            while move_to[0] not in 'ABCDE' and move_to[1] not in '01234' and len(move_to) == 2:
                move_to = input('Move to (e.g. B3): ')
            board.state[int(move_from[1])][ord(move_from[0]) - ord('A')] = ' '
            board.state[int(move_to[1])][ord(move_to[0]) - ord('A')] = self.color


def test():
    b = Board()
    index = 1
    for (i, j) in [(2, 2), (2, 3), (1, 3), (1, 2), (1, 1), (2, 1), (3, 1), (3, 2), (3, 3), (2, 4), (1, 4), (0, 4), (0, 3), (0, 2), (0, 1), (0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (4, 1), (4, 2), (4, 3), (4, 4), (3, 4)]:
        if index < 10:
            b.state[i][j] = '0' + str(index)
        else:
            b.state[i][j] = str(index)
        index = index + 1
    b.print()

def main():
    board = Board()
    humancolor = input('Choose your color "b" or "r": ')
    while humancolor[0] not in 'rb' or len(humancolor) != 1:
        humancolor = input('Choose your color "b" or "r": ')
    if humancolor == 'b':
        human = HumanPlayer('b')
        AI = AiPlayer('r')
        board.print()
        while checkWin(board.state, 'r') == 0:
            human.makeMove(board)
            if checkWin(board.state, 'r') != 0:
                break
            board.print()
            AI.makeMove(board)
            board.print()
    else:
        human = HumanPlayer('r')
        AI = AiPlayer('b')
        board.print()
        while checkWin(board.state, 'r') == 0:
            AI.makeMove(board)
            board.print()
            if checkWin(board.state, 'r') != 0:
                break
            human.makeMove(board)
            board.print()
    if checkWin(board.state, AI.color) == 1:
        print('AI win!')
    else:
        print('You win!')


if __name__ == "__main__":
    main()