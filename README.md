# gobang
中国科学院大学 C语言程序设计课程 五子棋大作业

chess.c为最终版本，其余文件均为中间版本产生。

## Feature
- 使用min-max搜索，alpha-beta剪枝，IDS，VCX等算法
- 精细实现了禁手的判断
- 使用位运算优化棋型匹配
- 借助pthread进行并行化搜索

---

# Gobang (Five in a Row)

Course: C Language Programming, University of Chinese Academy of Sciences (UCAS)

*Note: chess.c is the final version; all other files are intermediate versions generated during development.*

## Features
- **Advanced AI Algorithms**: Implements Minimax search, Alpha-beta pruning, Iterative Deepening Search (IDS), and Victory of Continuous Check (VCX).

- **Forbidden Moves**: Features a precise implementation of Forbidden Move (Kinshute) rules (typical for Renju/professional Gomoku).

- **Bitwise Optimization**: Uses Bitwise operations to optimize pattern matching and board evaluation for high-speed performance.

- **Parallel Computing**: Leverages pthread to implement parallelized searching, significantly improving decision-making efficiency.
