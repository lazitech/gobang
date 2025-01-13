#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define SIZE 15
#define CHARSIZE 3
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 15
#define GRID_maxn 15
#define CELL_SIZE 80
#define maxn 15
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define whole 2147483647
#define inf 1000000000
#define block 1073741823
#define score_five 1000000
#define score_livefour 4320
#define score_rushfour 720
#define score_livethere 720
#define score_rushthere 100
#define score_livetwo 120
#define score_rushtwo 20
#define score_liveone 2
#define ban -1000000
#define WIN_SCORE 10000
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)>(b)?(b):(a))
#define oppo(color) (((color)==1)?2:1)
#define MAX_STACK_SIZE 100
#define KILL_VCF 1  // 仅冲四
#define KILL_VCT 2  // 活三和冲四
// 棋盘使用的是GBK编码，每一个中文字符占用3个字节。
// 空棋盘模板
#define modifybi(n,i,k) do{bline[(n)]=(((1<<((i)<<1))-1)&bline[(n)])|((k)<<((i)<<1))|(bline[(n)]&(block^((1<<((i)+1<<1))-1)));}while(0)
int maxdepth4;
int maxdepth6;
int maxdepth8;
char line[95][50];   
int global_board[GRID_SIZE][GRID_SIZE] = {EMPTY};
int global_bline[95];
int linelen[95];
struct data
{
    int longban,five,livefour,rushfour,livethree,rushthree,livetwo,liveone,rushtwo;
};
int what;

char des[30];
int totalround;
typedef struct {
    int x;
    int y;
} Position;

int getscore(struct data node,int color);
struct data bscore(int bline[95],int board[GRID_maxn][GRID_maxn],int x,int y,int color,int mode);
struct data killscore(int bline[95],int board[GRID_maxn][GRID_maxn],int x,int y,int color,int mode);

void bmodifyline(int bline[95],int x,int y,int k);
void substr(char* src, char* dest, int l, int r , int maxlen);
void processdirc(int x,int y,int dirca[],int dircb[]);
void init();

int bstrstr(int s,int x,int len)
{   
    int temp=((1<<(len<<1))-1);
  //  printf("%d %d ",s,x);
    for(  int i=0;i<15;++i,s>>=2)
        if((s&temp)==x) return 1;
    return 0;
}
int bsubstr(int src,int l,int r,int maxnlen);
Position get_ai_move(int board[GRID_SIZE][GRID_SIZE],int color);
char arrayForEmptyBoard[SIZE][SIZE * CHARSIZE + 1] =
    {
        "┏┯┯┯┯┯┯┯┯┯┯┯┯┯┓",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┠┼┼┼┼┼┼┼┼┼┼┼┼┼┨",
        "┗┷┷┷┷┷┷┷┷┷┷┷┷┷┛"};
// 此数组存储用于显示的棋盘
char arrayForDisplayBoard[SIZE][SIZE * CHARSIZE + 1];
char play1Pic[] = "●"; // 黑棋子;
char play1CurrentPic[] = "▲";
char play2Pic[] = "◎"; // 白棋子;
char play2CurrentPic[] = "△";
int currentplayer=1;
// 此数组用���记录当前的棋盘的格局
int arrayForInnerBoardLayout[SIZE][SIZE];
int x,y,op,gamemode;
struct node
{
    int x,y;
};


typedef struct {
    Position pos;
    int score;
} ScoredPosition,*P;

//int search_depth[]={25,25,25,25,25,25,25,25,15};
int search_depth2[]={15,15,15,15,15,12,12,10,3};
int search_depth3[]={14,7,7,10,10,12,15}; 

int search_depth[]={14,7,7,7,7,10,10,12,15}; 
//int search_depth[]={20,20,20,20,20,15,15,10,10,10};
void initRecordBorard(void);
void innerLayoutToDisplayArray(int x,int y);
void displayBoard(void);
void printboard();
int getchess(int *x,int *y);
int bjudge(int bline[95]);
int dfscount;
int dfs(int bline[95],int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color);
int dfs2(int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color);
int dfs3(int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color);

int oneplay(int x,int y,int color)
{
    
    totalround++;
    printf("The %d move for %s\n",totalround,(color==1)?"black":"white");
    global_board[x-1][y-1]=color;
    bmodifyline(global_bline,x-1,y-1,color);
    arrayForInnerBoardLayout[SIZE-x][y-1] = color;
    int state=bjudge(global_bline);
    innerLayoutToDisplayArray(SIZE-x,y-1);
    displayBoard();
    return state;
}
struct node ai_play()
{
    struct node ans;
    int x,y;
    x=rand()%SIZE+1,y=rand()%SIZE+1;
    while(arrayForInnerBoardLayout[x][y]) x=rand()%SIZE+1,y=rand()%SIZE+1;
    ans.x=x,ans.y=y;
    printf("ai_move: %d %d\n",ans.x,ans.y);
    return ans;
}

#define DEPTH 6 // Define the depth of the search
int is_killing_point(int x, int y, int color, int kill_type) {
    struct data score = bscore(global_bline,global_board,x, y, color, 1);
    
    if(kill_type == KILL_VCF) {
        // VCF只考虑冲四和活四
        return (score.five>0|| score.livefour > 0 || score.rushfour > 0);
    } else {
        // VCT考虑活三、冲四和活四
        return (score.five>0||score.livefour > 0 || score.rushfour > 0 || score.livethree > 0);
    }
}
// Comparison function for qsort
int compare_scores(const void *a, const void *b) {
    return ((ScoredPosition *)b)->score - ((ScoredPosition *)a)->score;
}
typedef struct {
    Position best_move;  // 最佳杀棋点
    int win;            // 是否必胜
} KillResult;
int killcount=0;
// 统一的算杀函数
KillResult kill_search(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int color, int maximizingPlayer, int kill_type, int alpha, int beta) {
    KillResult result = {{-1, -1}, 0};
    
    killcount++;

    // 检查是否已经连五
    int state = bjudge(bline);
    if(state == color) {
        result.win = 1;
        return result;
    }
    if(state == oppo(color)) {
        result.win = -1;
        return result;
    }
    if(depth == 0) {
        result.win = 0;
        return result;
    }
    // 收集所有可能的杀棋点
    ScoredPosition moves[225];
    int move_count = 0;

    for(int i = 0; i < GRID_SIZE; i++) {
        for(int j = 0; j < GRID_SIZE; j++) {
            if(board[i][j] == EMPTY) {
                    if(maximizingPlayer) {
                    struct data score = killscore(bline,board,i, j, color, 0);
                    if(color == BLACK)
                    {
                        if(score.longban>0) continue;
                        if(score.livefour+score.rushfour>1) continue;
                        if(score.livethree+score.rushthree>1) continue;
                    }
                    if(score.five>0||score.livefour > 0 ||score.rushfour > 0||score.livethree>0) {
                        
                        moves[move_count++] = (ScoredPosition){{i, j}, score.five*score_five+score.livefour*score_livefour+score.rushfour*2000+score.livethree*score_livethere};
                        
                    }
                } else {
                    // 防守方考虑所有空点
                    struct data score1 = killscore(bline,board,i, j, color, 0);
                    struct data score2 = killscore(bline,board,i, j, oppo(color), 0);
                    if(color == BLACK)
                    {
                        if(score1.longban>0) continue;
                        if(score1.livefour+score1.rushfour>1) continue;
                        if(score1.livethree+score1.rushthree>1) continue;
                    }
                    else
                    {
                        if(score2.longban>0) continue;
                        if(score2.livefour+score2.rushfour>1) continue;
                        if(score2.livethree+score2.rushthree>1) continue;
                        
                    }
                    if(score1.five>0||score2.five>0||score1.livefour > 0 ||score2.livefour>0||score2.rushfour>0) {
                        moves[move_count++] = (ScoredPosition){{i, j}, getscore(score1, color)+getscore(score2, oppo(color))};
                    }
                
                }
            }
        }
    }
    qsort(moves, move_count, sizeof(ScoredPosition), compare_scores);
    // 如果没有杀棋点，尝试切换算杀类型
    if(move_count == 0) {
        result.win = 0;
        return result;
    }
    move_count = min(move_count, 7);  // 限制最大搜索数量
    // MAX层(进攻方)
    if(maximizingPlayer) {
        result.win = -1;
        for(int i = 0; i < move_count; i++) {
            int x = moves[i].pos.x;
            int y = moves[i].pos.y;
            
            board[x][y] = color;
            bmodifyline(bline,x, y, color);
            
            KillResult child = kill_search(bline,board, depth - 1, color, 0, kill_type, alpha, beta);
            
            board[x][y] = EMPTY;
            bmodifyline(bline,x, y, 0);
            
            if(child.win == 1) {
                result.win = 1;
                result.best_move = moves[i].pos;
                return result;  // 找到必胜点，立即返回
            }
            
            alpha = max(alpha, child.win);
            if(beta <= alpha) break;  // Alpha-Beta剪枝
        }
    }
    // MIN层(防守方)
    else {
        result.win = 1;
        for(int i = 0; i < move_count; i++) {
            int x = moves[i].pos.x;
            int y = moves[i].pos.y;
            
            board[x][y] = oppo(color);
            bmodifyline(bline,x, y, oppo(color));
            
            KillResult child = kill_search(bline,board, depth - 1, color, 1, kill_type, alpha, beta);
            
            board[x][y] = EMPTY;
            bmodifyline(bline,x, y, 0);
            
            if(child.win != 1) {
                result.win = 0;
                result.best_move = moves[i].pos;
                return result;  // 找到防守点，立即返回
            }
            
            beta = min(beta, child.win);
            if(beta <= alpha) break;  // Alpha-Beta剪枝
        }
    }

    return result;
}
int evaluate_board(int bline[95], int board[GRID_SIZE][GRID_SIZE],int color) {
    // Use existing scoring functions to evaluate the board
    struct data sum = {0};
    for (int i = 0; i < maxn; i++) {
        for (int j = 0; j < maxn; j++) {
            if (board[i][j] != color) continue; 
            struct data mine = bscore(bline,board,i, j, color, 0);
            sum.five += mine.five;
            sum.livefour += mine.livefour;
            sum.liveone += mine.liveone;
            sum.livethree += mine.livethree;
            sum.livetwo += mine.livetwo;
            sum.longban += mine.longban;
            sum.rushfour += mine.rushfour;
            sum.rushthree += mine.rushthree;
            sum.rushtwo += mine.rushtwo;
        }
    }
    int oppo=oppo(color);
    struct data node = {0};
    for (int i = 0; i < maxn; i++) {
        for (int j = 0; j < maxn; j++) {
            if (board[i][j] != oppo) continue;
            struct data mine = bscore(bline,board,i, j, oppo, 0);
            node.five += mine.five;
            node.livefour += mine.livefour;
            node.liveone += mine.liveone;
            node.livethree += mine.livethree;
            node.livetwo += mine.livetwo;
            node.longban += mine.longban;
            node.rushfour += mine.rushfour;
            node.rushthree += mine.rushthree;
            node.rushtwo += mine.rushtwo;
        }
    }
        sum.five /=5;
        sum.livefour /=4;
        sum.livethree /=3;
        sum.livetwo /=2;
        sum.rushfour /=4;
        sum.rushthree /=3;
        sum.rushtwo /=2;
        sum.livetwo/=2;
        node.five /=5;
        node.livefour /=4;
        node.livethree /=3;
        node.livetwo /=2;
        node.rushfour /=4;
        node.rushthree /=3;
        node.rushtwo /=2;
        node.livetwo/=2;
    int attack=sum.five*1000000+sum.livefour*100000+sum.rushfour*720+sum.livethree*720+sum.rushthree*480+sum.livetwo*480+sum.rushtwo*20+sum.liveone*2;

    int defence=node.five*100000000+node.livefour*10000000+node.rushfour*1000000+node.livethree*50000+node.rushthree*720+node.livetwo*480+node.rushtwo*100+node.liveone*10;
            //优先级是对方活四>对方冲四>己方活四>对方活3
    //printf(" %d %d\n",attack,defence);
    return attack-defence;
}



int evacount;
inline int dfs(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color) {
    if (depth == 0 )//|| check_winner(board, color, win_positions)||check_winner(board, oppo(color), win_positions)) {
    {
        evacount++;
        return evaluate_board(bline,board, color);

    }
    dfscount++;
    int state=bjudge(bline);
    if(state==color)
        return 1000000;
    if(state==(oppo(color)))
        return -10000000;
    int scoreboard[15][15][2];
    int flag1,flag2,flag3;//1: 对方成五 2：自己活四 3：对方活四
    flag1=flag2=flag3=0;
    // Evaluate each empty position
        int self,p;
    self=(maximizingPlayer)?color:oppo(color);
    p=(maximizingPlayer)?1:-1;  
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
            int attack=getscore(bscore(bline,board,i,j,self,1),self);
            int defence=getscore(bscore(bline,board,i,j,oppo(self),1),oppo(self));
            if(attack>=score_five) return attack*p;
            scoreboard[i][j][0]=attack;
            scoreboard[i][j][1]=defence; 
            }
            else
                scoreboard[i][j][0]=scoreboard[i][j][1]=ban;
        }
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(scoreboard[i][j][1]>=score_five)
            {
                flag1=1;
                break;
            }
        }
    }
    if(flag1)
    {
        for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(scoreboard[i][j][1]>=score_five)
            {
                board[i][j] = self;
                bmodifyline(bline,i, j, self);

                int value = dfs(bline,board, depth - 1, alpha, beta, maximizingPlayer^1, color);

                board[i][j] = EMPTY;
                bmodifyline(bline,i, j, 0);

                if (maximizingPlayer) {
                    alpha = max(alpha, value);
                } else {
                    beta = min(beta, value);
                }

                if (beta <= alpha) {

                    return (maximizingPlayer)?alpha:beta; // Alpha-Beta pruning
                }
            }

            }
        }
        return (maximizingPlayer)?alpha:beta;
    }

    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(scoreboard[i][j][0]>=score_livefour)
            {
                return scoreboard[i][j][0]*p;
            }
        }
    }
    // for (int i = 0; i < GRID_SIZE; i++) {
    //     for (int j = 0; j < GRID_SIZE; j++) {
    //         if(scoreboard[i][j][1]>=score_livefour)
    //         {
    //             flag3=1;
    //             break;
    //         }
    //     }
    // }
    // if(flag3)
    // {
    //     for (int i = 0; i < GRID_SIZE; i++) {
    //     for (int j = 0; j < GRID_SIZE; j++) {
    //         if(scoreboard[i][j][1]>=score_livefour)
    //         {
    //             board[i][j] = maximizingPlayer ? color : oppo(color);
    //             bmodifyline(bline,i, j, board[i][j]);

    //             int value = dfs(bline,board, depth - 1, alpha, beta, maximizingPlayer^1, color);

    //             board[i][j] = EMPTY;
    //             bmodifyline(bline,i, j, 0);

    //             if (maximizingPlayer) {
    //                 alpha = max(alpha, value);
    //             } else {
    //                 beta = min(beta, value);
    //             }

    //             if (beta <= alpha) {

    //                 return (maximizingPlayer)?alpha:beta; // Alpha-Beta pruning
    //             }
    //         }

    //         }
    //     }
    //     return (maximizingPlayer)?alpha:beta;
    // }
    
        ScoredPosition scored_positions[225];
    int count = 0;
    
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
            int attack=scoreboard[i][j][0];
            int defence=scoreboard[i][j][1];
            if(self==1&&attack==ban) continue;
            scored_positions[count++] = (ScoredPosition){{i, j}, attack+defence};
            }
        }
    }

    // Sort positions by score
    qsort(scored_positions, count, sizeof(ScoredPosition), compare_scores);

    int search_limit = min(search_depth[depth], count);

    for (int i = 0; i < search_limit; i++) {
        int x = scored_positions[i].pos.x;
        int y = scored_positions[i].pos.y;
        board[x][y] = maximizingPlayer ? color : oppo(color);
        bmodifyline(bline,x, y, board[x][y]);

        int value = dfs(bline,board, depth - 1, alpha, beta, maximizingPlayer^1, color);

        board[x][y] = EMPTY;
        bmodifyline(bline,x, y, 0);

        if (maximizingPlayer) {
            alpha = max(alpha, value);
        } else {
            beta = min(beta, value);
        }

        if (beta <= alpha) {
           
            return (maximizingPlayer)?alpha:beta; // Alpha-Beta pruning
        }
    }
   
    return (maximizingPlayer)?alpha:beta;
}

Position get_ai_move_standard(int bline[95], int board[GRID_SIZE][GRID_SIZE],int color) {
    // 添加计时开始
    clock_t start_time = clock();
    
    Position move;
    struct data opans={0};
    struct data ans={0};
    int oppo=(color==1)?2:1;
    //优先级是对方活四>对方冲四>己方活四>对方活3
    int mx1=0,mx2=0; 
        for(register int i=0;i<maxn;i++)
        for(int j=0;j<maxn;j++)
        {
            if(board[i][j]) continue;
            int attack=getscore(bscore(bline,board,i,j,color,1),color);
            int defence=0;
            if(color==1&&attack==ban) continue;

            struct data node={0};
            node=bscore(bline,board,i,j,oppo,1);
            defence=node.five*50000+node.livefour*2000+node.rushfour*300+node.livethree*300+node.rushthree*50+node.livetwo*30+node.rushtwo*10+node.liveone*5;
            if(attack+defence>mx1+mx2)
            {
                mx1=attack;
                mx2=defence;
                move.x=i;
                move.y=j;
            }
        }
    
    // 计算并显示耗时
    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("AI思考时间: %.3f秒\n", time_spent);
    printf("point: %d %d atk:%d def:%d\n",move.x,move.y,mx1,mx2);
    //display(move.x,move.y,color,1);
    return move;
}

// 线程数据结构
typedef struct {
    int board[GRID_SIZE][GRID_SIZE];  // 每个线程独立的棋盘副本
    int bline[95];                    // 每个线程独立的bline副本
    Position pos;                     // 这个线程要搜索的位置
    int depth;                        // 搜索深度
    int color;                        // 当前颜色
    int score;                        // 该位置的评分
} ThreadData;

// 线程搜索函数
void* position_search(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    
    // 在棋盘副本上进行搜索
    data->board[data->pos.x][data->pos.y] = data->color;
    bmodifyline(data->bline, data->pos.x, data->pos.y, data->color);
    
    data->score = dfs(data->bline, data->board, data->depth, -inf, inf, 0, data->color);
    
    data->board[data->pos.x][data->pos.y] = EMPTY;
    bmodifyline(data->bline, data->pos.x, data->pos.y, 0);
    
    return NULL;
}

// 修改后的get_ai_move1函数
Position get_ai_move1_pre(int board[GRID_SIZE][GRID_SIZE], int color) {
    
    const int NUM_THREADS = 5;  // 每个候选点一个线程
    
    // 收集和评估候选点
    ScoredPosition* scored_positions = malloc(sizeof(ScoredPosition) * 225);
    if (!scored_positions) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }


    int count = 0;

    // 初始评估所有空位
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                if(color==1 && getscore(bscore(global_bline,board,i,j,color,1),color)==ban) continue;
                // 使用浅层搜索进行初始评估
                board[i][j] = color;
                bmodifyline(global_bline,i, j, color);
                int moveValue = dfs(global_bline,board, 2, -inf, inf, 0, color); // 初始深度为2
                board[i][j] = EMPTY;
                bmodifyline(global_bline,i, j, 0);
                scored_positions[count++] = (ScoredPosition){{i, j}, moveValue};
            }
        }
    }

    // 排序候选位置
    qsort(scored_positions, count, sizeof(ScoredPosition), compare_scores);
    dfscount=0;
    evacount=0;
    Position bestMove = {-1, -1};
    int bestValue = -inf;
    Position currentBestMove = {-1, -1};
    int currentBestValue = -inf;
    
    // 创建线程和线程数据
        count=min(count,15);
    pthread_t* threads = malloc(sizeof(pthread_t) * count);
    ThreadData* thread_data = malloc(sizeof(ThreadData) * count);

    // 为每个候选点创建一个线程
    for(int i = 0; i < count; i++) {
        // 初始化线程数据
        memcpy(thread_data[i].board, board, sizeof(int) * GRID_SIZE * GRID_SIZE);
        memcpy(thread_data[i].bline, global_bline, sizeof(global_bline));
        thread_data[i].pos = scored_positions[i].pos;
        thread_data[i].depth = 8;  // 搜索深度
        thread_data[i].color = color;
        thread_data[i].score = 0;
        
        // 创建线程
        if (pthread_create(&threads[i], NULL, position_search, &thread_data[i]) != 0) {
            fprintf(stderr, "Error creating thread %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
    
    // 等待所有线程完成并收集结果

    for(int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
        scored_positions[i].score = thread_data[i].score;
        
        printf("位置(%d,%d): %d\n", 
               scored_positions[i].pos.x, 
               scored_positions[i].pos.y, 
               scored_positions[i].score);
        
        if(scored_positions[i].score > bestValue) {
            bestValue = scored_positions[i].score;
            bestMove = scored_positions[i].pos;
        }
        
        // 如果找到必胜点
        if(scored_positions[i].score >= WIN_SCORE) {
            bestMove = scored_positions[i].pos;
            bestValue = scored_positions[i].score;
            break;
        }
    }
    
    // 清理线程资源
    free(threads);
    free(thread_data);
    free(scored_positions);

    return bestMove;
}




Position get_ai_move1(int bline[95], int board[GRID_SIZE][GRID_SIZE], int color) {
    clock_t start_time = clock();
    killcount=0;
    const double TIME_LIMIT = 150.0; // 设置10秒的时间限制
    for(int x=0;x<maxn;x++)
        for(int y=0;y<maxn;y++)
        {
            if(board[x][y]==0)
            {
                board[x][y]=color;
                bmodifyline(bline,x,y,color);
                
                struct data ans={0};
                int dira[5],dirb[5];
                processdirc(x,y,dira,dirb);
                // 1. longban
                if(color==1)
                {
                    for(  int i=0;i<4;++i)
                        if(bstrstr(bline[dira[i]],0b010101010101,6)!=0)
                        {
                            ans.longban=1;
                            break;
                        }       
                }
                if(color==1&&ans.longban>0) 
                { 
                    continue;
                }
                // 2. five
                if(color==1)
                {
                    for(  int i=0;i<4;++i)
                        if(bstrstr(bline[dira[i]],0b010101010101,6)==0 && bstrstr(bline[dira[i]],0b0101010101,5)!=0)
                        {
                            ans.five=1; 
                            break;
                        }      
                    
                }
                else
                {
                    for(  int i=0;i<4;++i)
                        if(bstrstr(bline[dira[i]],0b1010101010,5)!=0)
                        {
                            ans.five=1;
                            break;
                        }       
                
                }
                if(ans.five)
                {
                    Position move={x,y};
                    printf("找到成功五子连珠\n");
                    return move;
                }
                board[x][y]=0;
                bmodifyline(bline,x,y,0);

            }
        }
    // 先尝试VCF算杀
    for(int depth = 5; depth <= 11; depth += 2) {
        KillResult kill_result = kill_search(bline,board, depth, color, 1, KILL_VCF, -inf, inf);
        if(kill_result.win == 1) {
                double time_spent = (double)(clock() - start_time) / CLOCKS_PER_SEC;
            printf("找到必胜序列(VCF), 深度: %d,用时%.3f秒\n", depth, time_spent);
            return kill_result.best_move;
        }
        
        // 检查时间限制
       
      //  if(time_spent >= 8.0) break;  // 超过10秒则停止算杀
    }
    double time_spent = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("未找到必胜策略，用时%.3f秒\n", time_spent);
    printf("killcount:%d\n",killcount);

    ScoredPosition *scored_positions;

    scored_positions = (ScoredPosition *)malloc(sizeof(ScoredPosition) * 225);

    int count = 0;

    // 初始评估所有空位
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                if(color==1 && getscore(bscore(bline,board,i,j,color,1),color)==ban) continue;
                // 使用浅层搜索进行初始评估
                board[i][j] = color;
                bmodifyline(bline,i, j, color);
                int moveValue = dfs(bline,board, 2, -inf, inf, 0, color); // 初始深度为2
                board[i][j] = EMPTY;
                bmodifyline(bline,i, j, 0);
                scored_positions[count++] = (ScoredPosition){{i, j}, moveValue};
            }
        }
    }

    // 排序候选位置
    qsort(scored_positions, count, sizeof(ScoredPosition), compare_scores);
    dfscount=0;
    evacount=0;
    Position bestMove = {-1, -1};
    int bestValue = -inf;
    Position currentBestMove = {-1, -1};
    int currentBestValue = -inf;
               pthread_t* threads = malloc(sizeof(pthread_t) * count);
    ThreadData* thread_data = malloc(sizeof(ThreadData) * count);
    // 迭代加深搜索
    for(int depth = 4; depth <= 8; depth += 2) {  // 从深度4开始，每次加2
        currentBestValue = -inf;
        
        // 检查时间
     //   clock_t current_time = clock();
    //    double elapsed_time = (double)(current_time - start_time) / CLOCKS_PER_SEC;
       // if(elapsed_time >= TIME_LIMIT) {
      //      printf("达到时间限制，使用深度%d的最佳结果\n", depth-2);
     //       break;
    //    }
        
        printf("正在搜索深度: %d\n", depth);
        count=min(count,search_depth2[depth]);
        for(int i = 0; i < count; i++) {
            // 初始化线程数据
            memcpy(thread_data[i].board, board, sizeof(int) * GRID_SIZE * GRID_SIZE);
            memcpy(thread_data[i].bline, global_bline, sizeof(global_bline));
            thread_data[i].pos = scored_positions[i].pos;
            thread_data[i].depth = depth;  // 搜索深度
            thread_data[i].color = color;
            thread_data[i].score = 0;
            
            // 创建线程
            if (pthread_create(&threads[i], NULL, position_search, &thread_data[i]) != 0) {
                fprintf(stderr, "Error creating thread %d\n", i);
                exit(EXIT_FAILURE);
            }
        }

            for(int i = 0; i < count; i++) {
        pthread_join(threads[i], NULL);
        scored_positions[i].score = thread_data[i].score;
        
        printf("位置(%d,%d): %d\n", 
               scored_positions[i].pos.x, 
               scored_positions[i].pos.y, 
               scored_positions[i].score);
        
        if(scored_positions[i].score > currentBestValue) {
            currentBestValue = scored_positions[i].score;
            currentBestMove = scored_positions[i].pos;
        }
        
        // 如果找到必胜点
        if(scored_positions[i].score >= WIN_SCORE) {
            bestMove = currentBestMove;
            bestValue = currentBestValue;
           goto search_end;
        }
    }
     
        bestMove = currentBestMove;
        bestValue = currentBestValue;
        

        qsort(scored_positions, search_depth2[depth], sizeof(ScoredPosition), compare_scores);

    }

search_end:
    clock_t end_time = clock();
     time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    printf("\n搜索完成:\n");
    printf("AI思考时间: %.3f秒\n", time_spent);
    printf("最终选择: %d %d 评分: %d\n", bestMove.x, bestMove.y, bestValue);
    putchar(bestMove.y+'A');
    printf("%d\n", bestMove.x+1);
    if(bestValue<=-4320)
    {
        bestMove=get_ai_move_standard(bline,board,color);
        return bestMove;
    }
    printf("evacount: %d\n",evacount);
    printf("dfscount: %d\n",dfscount);
    free(scored_positions);
    return bestMove;
}



int main()
{
    initRecordBorard(); // 初始化一个空棋盘
    innerLayoutToDisplayArray(SIZE,SIZE);
    displayBoard();
    init();
    puts("work of 吕安哲");
    puts("Please choose game mode:");
    puts("1. Human vs Human");
    puts("2. Human vs Computer");
    puts("3. Computer vs Computer");
    scanf("%d",&gamemode);
    while(gamemode!=1&&gamemode!=2&&gamemode!=3)
    {
        puts("Please choose 1 or 2 or 3.");
        scanf("%d",&gamemode);
    }
    if(gamemode==1)
    {
        while(1)
        {
            printf("It's player%d's turn:\n",currentplayer);
            op=getchess(&x,&y);
            while(1)
            {

                
                if(op==0)
                {
                    op=getchess(&x,&y);
                    continue;
                }
                if(x<1||y<1||x>SIZE||y>SIZE)
                {
                    //printf("%d %d\n",x,y);
                    puts("Out of the boundary. Please try again.");
                }
                else 
                if(arrayForInnerBoardLayout[SIZE-x][y-1])
                    puts("There already have been a chess. Please try again.");
                else break;  
                op=getchess(&x,&y);
            }
            int state=oneplay(x,y,currentplayer);
            if(state)
            {
                printf("player %d wins!\n",state);
                return 0;
            }
            //printboard();
            currentplayer=(currentplayer==1)?2:1;
        }
    }
    if(gamemode==2)
    {
        int mode;
        puts("Please choose who is the first player:");
        puts("1. Computer");
        puts("2. Human");
        scanf("%d",&mode);
        while(mode!=1&&mode!=2)
        {
            puts("Please choose 1 or 2.");
            scanf("%d",&mode);
        }
        if(mode==1)
        {
            oneplay(8,8,1);
            currentplayer=(currentplayer==1)?2:1;
        }
        int first=1;
        while(1)
        {
            puts("Let's get started!");
            if(currentplayer==mode)
            {
                if(first&&mode==2)
                {
                    first=0;
                    oneplay(9,9,2);
                    currentplayer=(currentplayer==1)?2:1;
                    continue;
                }
                //struct node ans=ai_play();
                Position ans =get_ai_move1(global_bline,global_board,currentplayer);
                global_board[ans.x][ans.y]=currentplayer;
                bmodifyline(global_bline,ans.x,ans.y,currentplayer);
                int state=oneplay(ans.x+1,ans.y+1,currentplayer);
                if(state)
                {
                    printf("player %d wins!\n",state);
                    return 0;
                }
                currentplayer=(currentplayer==1)?2:1;
                continue;
            }
            printf("It's player%d's turn:\n",currentplayer);
            op=getchess(&x,&y);
            while(1)
            {
                 
                if(op==-1) return 0;
                if(op==0)
                {
                    op=getchess(&x,&y);
                    continue;
                }
                if(x<1||y<1||x>SIZE||y>SIZE)
                {
                    //printf("%d %d\n",x,y);
                    puts("Out of the boundary. Please try again.");
                }
                else 
                if(arrayForInnerBoardLayout[SIZE-x][y-1])
                    puts("There already have been a chess. Please try again.");
                else break;  
                op=getchess(&x,&y);
            }
            printf("%d,%d\n",x,y);
            if(currentplayer==1)
            {
                int score=getscore(bscore(global_bline,global_board,x-1,y-1,currentplayer,1),currentplayer);
                if(score==ban)
                {
                    puts("There is a ban move!!!");
                    return 0;
                }
            }

            int state=oneplay(x,y,currentplayer);
            global_board[x-1][y-1]=currentplayer;
            bmodifyline(global_bline,x-1,y-1,currentplayer);
            if(state)
            {
                printf("player %d wins!\n",state);
                return 0;
            }
            currentplayer=(currentplayer==1)?2:1;
        }
    }
    if(gamemode==3)
    {

        oneplay(8,8,1);
                        currentplayer=(currentplayer==1)?2:1;

         oneplay(9,9,2);
                         currentplayer=(currentplayer==1)?2:1;
        
        while(1)
        {
    clock_t start_time = clock();

                Position ans =get_ai_move1(global_bline,global_board,currentplayer);
                    double time_spent = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    printf("AI思考时间: %.3f秒\n", time_spent);
                int state=oneplay(ans.x+1,ans.y+1,currentplayer);
                    if(totalround==225)
                {
                    puts("There have been a draw!!!");
                    return 0;
                }
                if(state)
                {
                    printf("player %d wins!\n",state);
                    return 0;
                }
                currentplayer=(currentplayer==1)?2:1;
                ans = get_ai_move_standard(global_bline,global_board,currentplayer);
                state=oneplay(ans.x+1,ans.y+1,currentplayer);
                if(totalround==225)
                {
                    puts("There have been a draw!!!");
                    return 0;
                }
                if(state)
                {
                    printf("player %d wins!\n",state);
                    return 0;
                }
                currentplayer=(currentplayer==1)?2:1;
               
        }
    }
    return 0;
}
// 初始化一个空棋盘格局
void initRecordBorard(void)
{
    // 通过双重循环，将arrayForInnerBoardLayout清0
    for(int i=0;i<SIZE;++i)
        for(int j=0;j<SIZE;++j)
        arrayForInnerBoardLayout[i][j]=0;
}

// 将arrayForInnerBoardLayout中记录的棋子位置，转化到arrayForDisplayBoard中
void innerLayoutToDisplayArray(int x,int y)
{
    // 第一步：将arrayForEmptyBoard中记录的空棋盘，复制到arrayForDisplayBoard中
    for(int i=0;i<SIZE;++i)
        for(int j=0;j<SIZE * CHARSIZE + 1;++j)
        arrayForDisplayBoard[i][j]=arrayForEmptyBoard[i][j];

        // 第二步：扫描arrayForInnerBoardLayout，当遇到非0的元素，将●或者◎复制到arrayForDisplayBoard的相应位置上
        for(int i=0;i<SIZE;++i)
            for(int j=0;j<SIZE;++j)
                if(i==x&&j==y)
                switch(currentplayer)
                {
                    case 1:
                        arrayForDisplayBoard[i][j*CHARSIZE]=play1CurrentPic[0];
                        arrayForDisplayBoard[i][j*CHARSIZE+1]=play1CurrentPic[1];
                        arrayForDisplayBoard[i][j*CHARSIZE+2]=play1CurrentPic[2];
                        break;
                    case 2:
                        arrayForDisplayBoard[i][j*CHARSIZE]=play2CurrentPic[0];
                        arrayForDisplayBoard[i][j*CHARSIZE+1]=play2CurrentPic[1];
                        arrayForDisplayBoard[i][j*CHARSIZE+2]=play2CurrentPic[2];
                        break;
                        
                }
                else
                switch(arrayForInnerBoardLayout[i][j])
                {
                    case 1:
                        arrayForDisplayBoard[i][j*CHARSIZE]=play1Pic[0];
                        arrayForDisplayBoard[i][j*CHARSIZE+1]=play1Pic[1];
                        arrayForDisplayBoard[i][j*CHARSIZE+2]=play1Pic[2];
                        break;
                    case 2:
                        arrayForDisplayBoard[i][j*CHARSIZE]=play2Pic[0];
                        arrayForDisplayBoard[i][j*CHARSIZE+1]=play2Pic[1];
                        arrayForDisplayBoard[i][j*CHARSIZE+2]=play2Pic[2];
                        break;
                        
                }
                
        // 注意：arrayForDisplayBoard所记录的字符是中文字符，每个字符占2个字节。●和◎也是中文字符，每个也占2个字节。
}
// 显示棋盘格局
void displayBoard(void)
{
    int i;
    // 第一步：清屏
    //system("clear"); // 清屏
    // 第二步：将arrayForDisplayBoard输出到屏幕上
    for(int i=0;i<SIZE;++i,puts(""))
    {
        printf("%2d",SIZE-i);
        for(int j=0;j<SIZE * CHARSIZE + 1;++j)
            putchar(arrayForDisplayBoard[i][j]);
        

    }
        // 第三步：输出最下面的一行字母A B ....
    putchar(' ');
    putchar(' ');
    putchar(' ');
    for(int i=0;i<SIZE;++i)
        printf("%c ",'A'+i); 
    puts("");
}
int mygetline(char *a,int len)
{
    int i=0;
    char c;
    while(i<len&&(c=getchar())!='\n'&&c!=EOF)  a[i++]=c;
    a[i]='\0';
    return i;
}
void printboard()
{
    for(int i=0;i<SIZE;++i,puts(""))
    for(int j=0;j<SIZE;++j)
        printf("%d ",arrayForInnerBoardLayout[i][j]);
}
inline int bjudge(int bline[95])
{
    for(int i=0;i<=90;++i)
    {
        if(linelen[i]==0) continue;
        if(bstrstr(bline[i],0b0101010101,5)) return 1;
        if(bstrstr(bline[i],0b1010101010,5)) return 2;
    }

    return 0;
}
// int judge() {
//     clock_t start = clock(); // Start timing

//     char line[50];
//     // 行
//     for (int i = 0; i < SIZE; ++i) {
//         char *p = line;
//         for (int j = 0; j < SIZE; ++j)
//             *p = ('0' + board[i][j]), p++;
//         *p = '\0';
//         if (strstr(line, "11111") != NULL) return 1;
//         if (strstr(line, "22222") != NULL) return 2;
//     }
//     // 列
//     for (int i = 0; i < SIZE; ++i) {
//         char *p = line;
//         for (int j = 0; j < SIZE; ++j)
//             *p = ('0' + board[j][i]), p++;
//         *p = '\0';
//         if (strstr(line, "11111") != NULL) return 1;
//         if (strstr(line, "22222") != NULL) return 2;
//     }
//     // 主对角线
//     for (int i = 0; i < SIZE; ++i) {
//         char *p = line;
//         for (int j = 0; i + j < SIZE; ++j)
//             *p = ('0' + board[j][i + j]), p++;
//         *p = '\0';
//         if (strstr(line, "11111") != NULL) return 1;
//         if (strstr(line, "22222") != NULL) return 2;
//     }
//     for (int i = 0; i < SIZE; ++i) {
//         char *p = line;
//         for (int j = 0; i + j < SIZE; ++j)
//             *p = ('0' + board[i + j][j]), p++;
//         *p = '\0';
//         if (strstr(line, "11111") != NULL) return 1;
//         if (strstr(line, "22222") != NULL) return 2;
//     }
//     // 副对角线
//     for (int i = 0; i < SIZE + SIZE - 1; ++i) {
//         char *p = line;
//         for (int j = 0; j <= i; ++j)
//             *p = ('0' + board[j][i - j]), p++;
//         *p = '\0';
//         if (strstr(line, "11111") != NULL) return 1;
//         if (strstr(line, "22222") != NULL) return 2;
//     }


//     return 0;
// }
int getchess(int *x,int *y)
{
    *x=*y=0;
    int xx=0;
    int yy=0;
    char line[1000];
    int res=mygetline(line,1000);
   // if(res==0) res=mygetline(line,1000);
    if(res==0)   return 0;
    char *p=line;
    char *pp=line;
    while(*pp==' '||*pp=='\t') pp++;
    char ss[]="quit";
    char *s=ss;
    while(*pp&&*pp==*s) pp++,s++;
    if(*pp=='\0') return -1;
    while(*p)
    {
        if(*p>='a'&&*p<='o')
        {
            if(yy==0) yy=(*p-'a'+1);
            //else return -1;
        } 
        if(*p>='A'&&*p<='O')
        {
            if(yy==0) yy=(*p-'A'+1);
            //else return -1;
        } 
        if(*p>='0'&&*p<='9')
        {
            if(*x==0) xx*=10,xx+=(*p-'0');
        } 
        else
            if(xx) *x=xx;
        p++;
    }
    *x=xx;
    *y=yy;
    return 1;
}



int ggetscore(struct data node,int color)
{
    if(color==2)     
    {
        if(node.five) return 100000;
        return node.livefour*4320+node.rushfour*720+node.livethree*720+node.rushthree*120+node.livetwo*60+node.rushtwo*20+node.liveone*10;
    }
    if(node.five)   return 100000;
    if(node.longban)    return -1;
    if(node.livefour+node.rushfour>1) return -1;
    if(node.livethree>1) return -1; 
    return node.livefour*4320+node.rushfour*720+node.livethree*720+node.rushthree*120+node.livetwo*60+node.rushtwo*20+node.liveone*10;
}
inline int getscore(struct data node,int color)
{
    if(color==2)     
    {
        if(node.five) return score_five;
        return node.livefour*score_livefour+node.rushfour*score_rushfour+node.livethree*score_livethere+node.rushthree*score_rushthere+node.livetwo*score_livetwo/2+node.rushtwo*score_rushtwo+node.liveone*score_liveone;
    }
    if(node.five)   return score_five;
    if(node.longban)    return ban;
    if(node.livefour+node.rushfour>1) return ban;
    if(node.livethree>1) return ban; 
    return node.livefour*score_livefour+node.rushfour*score_rushfour+node.livethree*score_livethere+node.rushthree*score_rushthere+node.livetwo*score_livetwo/2+node.rushtwo*score_rushtwo+node.liveone*score_liveone;
}

struct data bscore(int bline[95],int board[GRID_SIZE][GRID_SIZE],int x,int y,int color,int mode) // mode  0:不落子 用于计算被毁掉的组合情况 1：落子 用于计算新产生的组合情况
{

    struct data ans={0};
    int dira[5],dirb[5];
    //ans.rushtwo=ans.five=ans.livefour=ans.liveone=ans.livethree=ans.livetwo=ans.longban=ans.rushfour=ans.rushthree=0;
    if(mode) board[x][y]=color;
    if(mode) bmodifyline(bline,x,y,color);
    processdirc(x,y,dira,dirb);
    // 1. longban
    if(color==1)
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b010101010101,6)!=0)
            {
                ans.longban=1;
                break;
            }       
    }
    if(color==1&&ans.longban>0) 
    { 
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(bline,x,y,0);
        return ans;
    }
    // 2. five
    if(color==1)
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b010101010101,6)==0 && bstrstr(bline[dira[i]],0b0101010101,5)!=0)
            {
                 ans.five=1; 
                 break;
            }      
        
    }
    else
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b1010101010,5)!=0)
            {
                ans.five=1;
                break;
            }       
    
    }
    if(ans.five>0) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(bline,x,y,0);
        return ans;
    }
    // 3. livefour
    if(color==1)
    {
        for(  int i=0;i<4;++i)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-5,dirb[i]+5,linelen[dira[i]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                ans.livefour++;
                if(ans.livefour>1) break;
            }
        }
    }
    else
    {
        for(  int i=0;i<4;++i)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            
            if(bstrstr(des,0b001010101000,6)!=0)
                ans.livefour++;
        }
    }
    if(color==1&&ans.livefour>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(bline,x,y,0);
        return ans;
    }
    // 4. rushfour
    // 0 1  2  3 
    // 横 竖 副 主
    for(  int i=x-4;i<=x+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[i][y]) continue;
        board[i][y]=color;
        bmodifyline(bline,i,y,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-5,dirb[1]+5,linelen[dira[1]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-4,dirb[1]+4,linelen[dira[1]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[i][y]=0;
        bmodifyline(bline,i,y,0);
    }
    
    for(  int i=y-4;i<=y+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        board[x][i]=color;
        bmodifyline(bline,x,i,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-5,dirb[0]+5,linelen[dira[0]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-4,dirb[0]+4,linelen[dira[0]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[x][i]=0;
        bmodifyline(bline,x,i,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-5,dirb[2]+5,linelen[dira[2]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-4,dirb[2]+4,linelen[dira[2]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-5,dirb[3]+5,linelen[dira[3]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-4,dirb[3]+4,linelen[dira[3]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.rushfour-=ans.livefour*2;
    ans.rushfour=max(0,ans.rushfour);
    if(color==1&&ans.livefour+ans.rushfour>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(bline,x,y,0);
        return ans;
    }
    // 5. livethree
    
    int flag=0;
    for(  int i=x-3;i<=x+3&&flag==0;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[i][y]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<x-3||j>x+3) continue;
            if(j<0||j>=maxn) continue;
            if(board[j][y]==color) exist=1;
        }
        if(exist==0) continue;
        board[i][y]=color;
        bmodifyline(bline,i,y,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-5,dirb[1]+5,linelen[dira[1]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                
                struct data node=bscore(bline,board,i,y,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;

                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-4,dirb[1]+4,linelen[dira[1]]);
            if(bstrstr(des,0b001010101000,6)!=0)
            {
                flag=1;
            }
        }
        board[i][y]=0;
        bmodifyline(bline,i,y,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=y-3;i<=y+3&&flag==0;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<y-3||j>y+3) continue;
            if(x<0||x>=maxn||j<0||j>=maxn) continue;
            if(board[x][j]==color) exist=1;
        }
        if(exist==0) continue;
        board[x][i]=color;
        bmodifyline(bline,x,i,color);
        if(color==1)
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-5,dirb[0]+5,linelen[dira[0]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,x,i,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-4,dirb[0]+4,linelen[dira[0]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[x][i]=0;
        bmodifyline(bline,x,i,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=-3;i<=3&&flag==0;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<-3||j>3) continue;
            if(x+j<0||x+j>=maxn||y-j<0||y-j>=maxn) continue;
            if(board[x+j][y-j]==color) exist=1;
        }
        if(exist==0) continue;        
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-5,dirb[2]+5,linelen[dira[2]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-4,dirb[2]+4,linelen[dira[2]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=-3;i<=3&&flag==0;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<-3||j>3) continue;
            if(x+j<0||x+j>=maxn||y+j<0||y+j>=maxn) continue;
            if(board[x+j][y+j]==color) exist=1;
        }
        if(exist==0) continue;   
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-5,dirb[3]+5,linelen[dira[3]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-4,dirb[3]+4,linelen[dira[3]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    if(color==1&&ans.livethree>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(bline,x,y,0);
        return ans;
    }
    
    // 6. rushthree
    for(  int i=0;i<4;++i)
    {
        if(color==1)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b000001010110,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b000001010111,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b000100010110,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b000100010111,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b100101010000,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b110101010000,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b100101000100,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b110101000100,6)!=0)  ans.rushthree++;
            if(bstrstr(bline[dira[i]],0b0000010101000101,8)!=0||bstrstr(bline[dira[i]],0b000100010100010101,9)!=0||bstrstr(bline[dira[i]],0b0101000101010000,8)!=0||bstrstr(bline[dira[i]],0b010101000101000100,9)!=0)  ans.rushthree++;
            
        }
        else
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b000010101001,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b000010101011,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b001000101001,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b001000101011,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b011010100000,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b111010100000,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b011010001000,6)!=0)  ans.rushthree++;
            if(bstrstr(des,0b111010001000,6)!=0)  ans.rushthree++;
        }
    }

    // 7. livetwo
    for(  int i=0;i<4;++i)
    {
         if(color==1)
        {
            int des;
            des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+1,linelen[dira[i]]);
            if(bstrstr(des,0b0001000100,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-1,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b0001000100,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-2,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b0000010100,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+2,linelen[dira[i]]);
            if(bstrstr(des,0b0001010000,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+1,linelen[dira[i]]);
            if(bstrstr(des,0b000100000100,6)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-1,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b000100000100,6)!=0)  ans.livetwo++;     
           
        }
        else
        {
            int des;
            des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+1,linelen[dira[i]]);
            if(bstrstr(des,0b0010001000,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-1,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b0010001000,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+3,linelen[dira[i]]);
            des=bsubstr(bline[dira[i]],dirb[i]-2,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b0000101000,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+2,linelen[dira[i]]);
            if(bstrstr(des,0b0010100000,5)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+1,linelen[dira[i]]);
            if(bstrstr(des,0b001000001000,6)!=0)  ans.livetwo++;
            des=bsubstr(bline[dira[i]],dirb[i]-1,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b001000001000,6)!=0)  ans.livetwo++;     
           
        }
    }
    // 8. rushtwo
    for(  int i=0;i<4;++i)
    {
        if(color==1)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b000000010110,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000001010010,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000001000110,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000100000110,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000100010010,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b100101000000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b100001010000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b100100010000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b100100000100,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b100001000100,6)!=0)  ans.rushtwo++;

        }
        else
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            if(bstrstr(des,0b000000101001,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000010100001,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b000010001001,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b001000001001,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b001000100001,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b011010000000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b010010100000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b011000100000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b011000001000,6)!=0)  ans.rushtwo++;
            if(bstrstr(des,0b010010001000,6)!=0)  ans.rushtwo++;
        }
    }
    // 9. liveone
        for(  int i=0;i<4;++i)
    {
        if(color==1)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b000001000000,6)!=0)  ans.liveone++;
            if(bstrstr(des,0b000000010000,6)!=0)  ans.liveone++;
        }
        else
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-3,dirb[i]+3,linelen[dira[i]]);
            if(bstrstr(des,0b000010000000,6)!=0)  ans.liveone++;
            if(bstrstr(des,0b000000100000,6)!=0)  ans.liveone++;
        }
    }
    if(mode) board[x][y]=0;
    if(mode) bmodifyline(bline,x,y,0);
    return ans;
}


struct data killscore(int bline[95],int board[maxn][maxn],int x,int y,int color,int mode) // mode  0:不落子 用于计算被毁掉的组合情况 1：落子 用于计算新产生的组合情况
{

    struct data ans={0};
    int dira[5],dirb[5];
    //ans.rushtwo=ans.five=ans.livefour=ans.liveone=ans.livethree=ans.livetwo=ans.longban=ans.rushfour=ans.rushthree=0;
     board[x][y]=color;
     bmodifyline(bline,x,y,color);
    processdirc(x,y,dira,dirb);
    // 1. longban
    if(color==1)
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b010101010101,6)!=0)
            {
                ans.longban=1;
                break;
            }       
    }
    if(color==1&&ans.longban>0) 
    { 
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }
    // 2. five
    if(color==1)
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b010101010101,6)==0 && bstrstr(bline[dira[i]],0b0101010101,5)!=0)
            {
                 ans.five=1; 
                 break;
            }      
        
    }
    else
    {
        for(  int i=0;i<4;++i)
            if(bstrstr(bline[dira[i]],0b1010101010,5)!=0)
            {
                ans.five=1;
                break;
            }       
    
    }
    if(ans.five>0) 
    {
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }
    // 3. livefour
    if(color==1)
    {
        for(  int i=0;i<4;++i)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-5,dirb[i]+5,linelen[dira[i]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                ans.livefour++;
                if(ans.livefour>1) break;
            }
        }
    }
    else
    {
        for(  int i=0;i<4;++i)
        {
            int des=bsubstr(bline[dira[i]],dirb[i]-4,dirb[i]+4,linelen[dira[i]]);
            
            if(bstrstr(des,0b001010101000,6)!=0)
                ans.livefour++;
        }
    }
    if(ans.livefour>0) 
    {
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }

    // 4. rushfour
    // 0 1  2  3 
    // 横 竖 副 主
    for(  int i=x-4;i<=x+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[i][y]) continue;
        board[i][y]=color;
        bmodifyline(bline,i,y,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-5,dirb[1]+5,linelen[dira[1]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-4,dirb[1]+4,linelen[dira[1]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[i][y]=0;
        bmodifyline(bline,i,y,0);
    }
    
    for(  int i=y-4;i<=y+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        board[x][i]=color;
        bmodifyline(bline,x,i,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-5,dirb[0]+5,linelen[dira[0]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-4,dirb[0]+4,linelen[dira[0]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[x][i]=0;
        bmodifyline(bline,x,i,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-5,dirb[2]+5,linelen[dira[2]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-4,dirb[2]+4,linelen[dira[2]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-5,dirb[3]+5,linelen[dira[3]]);
            if(bstrstr(des,0b010101010101,6)==0 && bstrstr(des,0b0101010101,5)!=0)
                ans.rushfour++;
        }
        else
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-4,dirb[3]+4,linelen[dira[3]]);
            if(bstrstr(des,0b1010101010,5)!=0)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.rushfour-=ans.livefour*2;
    ans.rushfour=max(0,ans.rushfour);
    if(ans.rushfour>0) 
    {
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }
    // 5. livethree
    if(mode) return ans;
    int flag=0;
    for(  int i=x-3;i<=x+3&&flag==0;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[i][y]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<x-3||j>x+3) continue;
            if(board[j][y]==color) exist=1;
        }
        if(exist==0) continue;
        board[i][y]=color;
        bmodifyline(bline,i,y,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-5,dirb[1]+5,linelen[dira[1]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                
                struct data node=bscore(bline,board,i,y,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;

                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-4,dirb[1]+4,linelen[dira[1]]);
            if(bstrstr(des,0b001010101000,6)!=0)
            {
                flag=1;
            }
        }
        board[i][y]=0;
        bmodifyline(bline,i,y,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=y-3;i<=y+3&&flag==0;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<y-3||j>y+3) continue;
            if(board[x][j]==color) exist=1;
        }
        if(exist==0) continue;
        board[x][i]=color;
        bmodifyline(bline,x,i,color);
        if(color==1)
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-5,dirb[0]+5,linelen[dira[0]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,x,i,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-4,dirb[0]+4,linelen[dira[0]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[x][i]=0;
        bmodifyline(bline,x,i,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=-3;i<=3&&flag==0;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<-3||j>3) continue;
            if(board[x+j][y-j]==color) exist=1;
        }
        if(exist==0) continue;        
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-5,dirb[2]+5,linelen[dira[2]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-4,dirb[2]+4,linelen[dira[2]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    for(  int i=-3;i<=3&&flag==0;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        int exist=0;
        for(int j=i-1;j<=i+1;++j)
        {
            if(j<-3||j>3) continue;
            if(board[x+j][y+j]==color) exist=1;
        }
        if(exist==0) continue;   
        board[xx][yy]=color;
        bmodifyline(bline,xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-5,dirb[3]+5,linelen[dira[3]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(bline,board,xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dira,dirb);
            }
        }
        else
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-4,dirb[3]+4,linelen[dira[3]]);
            if(bstrstr(des,0b001010101000,6)!=0)
                flag=1;
        }
        board[xx][yy]=0;
        bmodifyline(bline,xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    if(color==1&&ans.livethree>1) 
    {
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }
        if(ans.livethree>0) 
    {
         board[x][y]=0;
         bmodifyline(bline,x,y,0);
        return ans;
    }

     board[x][y]=0;
     bmodifyline(bline,x,y,0);
    return ans;
}







 int bsubstr(int src,int l,int r,int maxnlen)
{
    int dest=0;
    int top=0;
    if(l<0) dest|=3,top+=2;
    l=max(0,l);
    int temp=((1<<((min(r+1,maxnlen))<<1))-1);
    if(l>0) temp^=((1<<(l<<1))-1);
    src=temp&src;
    if(l>0) src>>=(l<<1);
    dest|=((src)<<top);// need to be modified
    top+=((min(r+1,maxnlen)-l)*2);
    if(r>=maxnlen) dest|=(3<<top),top+=2;
    int laz=(whole)^((1<<top)-1);
    dest|=laz;
    return dest;
}
void processdirc(int x,int y,int dirca[],int dircb[])
{
    // 0 1  2  3 
    // 横 竖 副 主
    dirca[0]=x;
    dircb[0]=y;
    
    dirca[1]=maxn+y;
    dircb[1]=x;
    
    
    if(x+y>14)
    {

        dirca[2]=59+x+y;
        dircb[2]=14-y;
        
    }
    else
    {
        dirca[2]=59+x+y;
        dircb[2]=x; 
    }
    if(x==y)
    {
        dirca[3]=30;
        dircb[3]=x;
    }
    else
    {
        if(x>y)
        {
            dirca[3]=44+x-y;
            dircb[3]=y;
        }
        else
        {   
            dirca[3]=30+y-x;
            dircb[3]=x;
        }
    }
    return ;
}


void init()
{

    // 行
    int top=0;
    for ( int i = 0; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &global_bline[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + global_board[i][j]), p++,*pp|=(global_board[i][j]<<(j+j));
        *p = '\0';
    }
    // 列
    for ( int i = 0; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &global_bline[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + global_board[j][i]), p++,*pp|=(global_board[j][i]<<(j+j));
        *p = '\0';
    }
    // 主对角线
    for ( int i = 0; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &global_bline[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + global_board[j][i + j]), p++, *pp|=(global_board[j][i + j]<<(j+j));
        *p = '\0';
    }
    for ( int i = 1; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &global_bline[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + global_board[i + j][j]), p++,*pp|=(global_board[i + j][j]<<(j+j));
        *p = '\0';
    }
    // 副对角线
    for ( int i = 0; i < maxn + maxn - 1; ++i) {
        char *p = line[top];
        int *pp = &global_bline[top++];
        for (int j = 0; j <= i; ++j)
        {
            if(j<0||j>=maxn||i-j<0||i-j>=maxn) continue;
            *p = ('0' + global_board[j][i - j]), p++,*pp|=(global_board[j][i - j]<<(j+j));
        }
        *p = '\0';
    }
     for(  int i=0;i<=90;++i)
         linelen[i]=strlen(line[i]);//,printf("%d %s\n",i,line[i]),printbi(bline[i]);
}

 void bmodifyline(int bline[95],int x,int y,int k)
{
    
    modifybi(x,y,k);
    modifybi(maxn+y,x,k);
    if(x+y>14)
    {
        modifybi(59+x+y,14-y,k);
    
    }
    else
    {
        modifybi(59+x+y,x,k);
        
    }
    if(x==y)
    {
        modifybi(30,x,k);
    
    }
    else
    {
        if(x>y)
        {
            modifybi(44+x-y,y,k);
        }
        else
        {   
            modifybi(30+y-x,x,k);
        }
    }
    return ;
}



