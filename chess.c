#define _POSIX_C_SOURCE 199309L  
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

// 棋盘相关的常量定义
#define SIZE 15          // 棋盘大小
#define maxn 15
#define CHARSIZE 3       // 每个棋子字符的大小
#define WINDOW_WIDTH 600 // 窗口宽度
#define WINDOW_HEIGHT 600 // 窗口高度
#define GRID_SIZE 15     // 网格大小
#define GRID_maxn 15     // 最大网格数
#define CELL_SIZE 80     // 单元格大小

// 棋子状态定义
#define EMPTY 0  // 空位
#define BLACK 1  // 黑子
#define WHITE 2  // 白子

// 评分相关常量
#define whole 2147483647    // 整数最大值
#define inf 1000000000      // 无穷大值
#define block 1073741823    // 阻挡值
#define score_five 1000000  // 五连得分
#define score_livefour 4320 // 活四得分
#define score_rushfour 720  // 冲四得分
#define score_livethere 720 // 活三得分
#define score_rushthere 100 // 冲三得分
#define score_livetwo 120   // 活二得分
#define score_rushtwo 20    // 冲二得分
#define score_liveone 2     // 活一得分
#define ban -1000000        // 禁手得分
#define WIN_SCORE 10000     // 胜利得分
#define max(a,b) ((a)>(b)?(a):(b)) // 取最大值宏
#define min(a,b) ((a)>(b)?(b):(a)) // 取最小值宏
#define oppo(color) (((color)==1)?2:1) // 获取对手颜色宏
#define MAX_STACK_SIZE 100  // 最大堆栈大小
#define DEPTH 6  // 搜索深度
#define modifybi(n,i,k) do{bline[(n)]=(((1<<((i)<<1))-1)&bline[(n)])|((k)<<((i)<<1))|(bline[(n)]&(block^((1<<((i)+1<<1))-1)));}while(0) // 修改二进制位宏

char line[95][50];   
int global_board[GRID_SIZE][GRID_SIZE] = {EMPTY};
int global_bline[95];
int linelen[95];
int state;
char des[30];
int totalround;

// 数据结构定义
typedef struct {
    int x;
    int y;
} Position;  // 位置结构体

struct data {
    int longban;    // 长连禁手
    int five;       // 五连
    int livefour;   // 活四
    int rushfour;   // 冲四
    int livethree;  // 活三
    int rushthree;  // 冲三
    int livetwo;    // 活二
    int liveone;    // 活一
    int rushtwo;    // 冲二
};

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
int dx[]={-1,-1,1,1};
int dy[]={-1,1,-1,1};
typedef struct {
    Position pos;
    int score;
} ScoredPosition,*P;
int search_depth2[]={15,15,15,15,20,12,15,10,10};
int search_depth[]={14,7,7,7,7,10,10,12,15}; 
typedef struct {
    Position best_move;  // 最佳杀棋点
    int win;            // 是否必胜
} KillResult;

// 线程数据结构
typedef struct {
    int board[GRID_SIZE][GRID_SIZE];  // 棋盘副本
    int bline[95];                    // 线条数组副本
    Position pos;                     // 搜索位置
    int depth;                        // 搜索深度
    int color;                        // 当前颜色
    int score;                        // 评分
} ThreadData;

int getscore(struct data node,int color);
struct data bscore(int bline[95],int board[GRID_maxn][GRID_maxn],int x,int y,int color,int mode);
struct data killscore(int bline[95],int board[GRID_maxn][GRID_maxn],int x,int y,int color,int mode);
void bmodifyline(int bline[95],int x,int y,int k);
void substr(char* src, char* dest, int l, int r , int maxlen);
void processdirc(int x,int y,int dirca[],int dircb[]);
void init();    
int bsubstr(int src,int l,int r,int maxnlen);
Position get_ai_move(int board[GRID_SIZE][GRID_SIZE],int color);
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
KillResult kill_search(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int color, int maximizingPlayer, int alpha, int beta) ;
int evaluate_board(int bline[95], int board[GRID_SIZE][GRID_SIZE],int color) ;
inline int dfs(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color) ;
void* position_search(void* arg);
Position get_ai_move1(int bline[95], int board[GRID_SIZE][GRID_SIZE], int color) ;
int compare_scores(const void *a, const void *b);
int oneplay(int x,int y,int color);

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
        displayBoard();
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
                if(mode==1)
                {
                    if(totalround==2)
                    {
                        if(x==9&&y==9)
                        {
                            oneplay(7,9,1);
                            currentplayer=(currentplayer==1)?2:1;
                            continue;
                        }
                        if(x==7&&y==7)
                        {
                            oneplay(7,9,1);
                            currentplayer=(currentplayer==1)?2:1;
                            continue;
                        }
                        if(x==7&&y==9)
                        {
                            oneplay(9,9,1);
                            currentplayer=(currentplayer==1)?2:1;
                            continue;
                        }
                        if(x==9&&y==7)
                        {
                            oneplay(9,9,1);
                            currentplayer=(currentplayer==1)?2:1;
                            continue;
                        }
                    }
                }
                if(first&&mode==2)
                {
                    first=0;
                    for(int i=0;i<4;++i)
                    {
                        int xx=x+dx[i];
                        int yy=y+dy[i];
                        if(xx<1||yy<1||xx>SIZE||yy>SIZE) continue;
                        if(global_board[xx-1][yy-1]) continue;
                        oneplay(xx,yy,currentplayer);
                        break;
                    }
                    currentplayer=(currentplayer==1)?2:1;
                    continue;
                }
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
                struct timespec start, end;
                clock_gettime(CLOCK_MONOTONIC, &start);
                Position ans =get_ai_move1(global_bline,global_board,currentplayer);
                clock_gettime(CLOCK_MONOTONIC, &end);
                double time_s = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
                printf("AI思考时间: %.3f秒\n", time_s);

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
                 clock_gettime(CLOCK_MONOTONIC, &start);
                 ans =get_ai_move1(global_bline,global_board,currentplayer);
                clock_gettime(CLOCK_MONOTONIC, &end);
                 time_s = (double)(end.tv_sec - start.tv_sec) + (double)(end.tv_nsec - start.tv_nsec) / 1e9;
                printf("AI思考时间: %.3f秒\n", time_s);    
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

// 打印当前棋盘状态（用于调试）
void printboard() {
    for(int i=0;i<SIZE;++i,puts(""))
    for(int j=0;j<SIZE;++j)
        printf("%d ",arrayForInnerBoardLayout[i][j]);
}

// 判断棋盘是否有人获胜
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
// 获取用户输入的落子位置
// x,y: 输出参数，存储用户输入的坐标
int getchess(int *x,int *y) {
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



// 计算某个位置的得分
// node: 包含各种棋型数量的结构体
// color: 当前玩家颜色
inline int getscore(struct data node,int color) {
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

// 计算某个位置的得分
// bline: 棋盘线性表示
// board: 棋盘数组
// x,y: 当前落子位置
// color: 当前玩家颜色
// mode: 模式，0-不落子（计算被毁掉的组合情况），1-落子（计算新产生的组合情况）
struct data bscore(int bline[95],int board[GRID_SIZE][GRID_SIZE],int x,int y,int color,int mode) {

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
                flag=1;
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

//与bscore类似，但用于计算算杀点得分
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






// 从棋盘线性表示中提取子串
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

// 处理方向数组
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

// 初始化棋盘线性表示
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

// 修改棋盘线性表示
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



// 判断棋盘线性表示中是否包含某个子串
int bstrstr(int s,int x,int len)
{   
    int temp=((1<<(len<<1))-1);
  //  printf("%d %d ",s,x);
    for(  int i=0;i<15;++i,s>>=2)
        if((s&temp)==x) return 1;
    return 0;
}

// 落子
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


// 比较得分，用于排序
int compare_scores(const void *a, const void *b) {
    return ((ScoredPosition *)b)->score - ((ScoredPosition *)a)->score;
}


// 算杀函数，使用了alpha-beta剪枝

KillResult kill_search(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int color, int maximizingPlayer, int alpha, int beta) {
    KillResult result = {{-1, -1}, 0};
    
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
            
            KillResult child = kill_search(bline,board, depth - 1, color, 0, alpha, beta);
            
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
            
            KillResult child = kill_search(bline,board, depth - 1, color, 1, alpha, beta);
            
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
// 评估棋盘得分，用于dfs
int evaluate_board(int bline[95], int board[GRID_SIZE][GRID_SIZE],int color) {
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
    //printf(" %d %d\n",attack,defence);
    return attack-defence;
}



// dfs搜索,使用了alpha-beta剪枝,返回值为得分
inline int dfs(int bline[95], int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color) {
    if (depth == 0)
        return evaluate_board(bline, board, color);

    // 检查是否有一方已经获胜
    int state = bjudge(bline);
    if(state == color)
        return 1000000;  // 我方获胜，返回极大值
    if(state == (oppo(color)))
        return -10000000;  // 对方获胜，返回极小值

    // 用于存储每个位置的进攻分数和防守分数
    int scoreboard[15][15][2];
    // flag1: 对方有成五点 flag2: 自己有活四 flag3: 对方有活四
    int flag1, flag2, flag3;
    flag1 = flag2 = flag3 = 0;

    // self: 当前层轮到谁下子 p: 分数正负号(最大化层为1,最小化层为-1)
    int self = (maximizingPlayer) ? color : oppo(color);
    int p = (maximizingPlayer) ? 1 : -1;

    // 第一次扫描棋盘,计算所有空位的进攻分和防守分
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int attack = getscore(bscore(bline,board,i,j,self,1), self);
                int defence = getscore(bscore(bline,board,i,j,oppo(self),1), oppo(self));
                // 如果找到必胜点,直接返回
                if(attack >= score_five) return attack*p;
                scoreboard[i][j][0] = attack;
                scoreboard[i][j][1] = defence;
            }
            else
                scoreboard[i][j][0] = scoreboard[i][j][1] = ban;
        }
    }

    // 检查是否存在对方的成五点
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(scoreboard[i][j][1] >= score_five) {
                flag1 = 1;
                break;
            }
        }
    }

    // 如果存在对方成五点,必须阻挡
    if(flag1) {
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if(scoreboard[i][j][1] >= score_five) {
                    // 尝试在此处落子
                    board[i][j] = self;
                    bmodifyline(bline,i, j, self);

                    int value = dfs(bline,board, depth - 1, alpha, beta, maximizingPlayer^1, color);

                    // 回溯
                    board[i][j] = EMPTY;
                    bmodifyline(bline,i, j, 0);

                    // Alpha-Beta更新
                    if (maximizingPlayer) {
                        alpha = max(alpha, value);
                    } else {
                        beta = min(beta, value);
                    }

                    if (beta <= alpha) {
                        return (maximizingPlayer) ? alpha : beta;  // 剪枝
                    }
                }
            }
        }
        return (maximizingPlayer) ? alpha : beta;
    }

    // 检查是否存在活四点,有的话直接返回必胜
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if(scoreboard[i][j][0] >= score_livefour) {
                return scoreboard[i][j][0]*p;
            }
        }
    }

    // 收集所有可行的落子点
    ScoredPosition scored_positions[225];
    int count = 0;

    // 将空位按进攻分和防守分之和排序
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                int attack = scoreboard[i][j][0];
                int defence = scoreboard[i][j][1];
                // 黑方要避开禁手点
                if(self == 1 && attack == ban) continue;
                scored_positions[count++] = (ScoredPosition){{i, j}, attack+defence};
            }
        }
    }

    // 按分数从高到低排序
    qsort(scored_positions, count, sizeof(ScoredPosition), compare_scores);

    // 只搜索最有希望的几个点
    int search_limit = min(search_depth[depth], count);

    // 对每个候选点进行搜索
    for (int i = 0; i < search_limit; i++) {
        int x = scored_positions[i].pos.x;
        int y = scored_positions[i].pos.y;
        
        // 尝试落子
        board[x][y] = maximizingPlayer ? color : oppo(color);
        bmodifyline(bline,x, y, board[x][y]);

        // 递归搜索
        int value = dfs(bline,board, depth - 1, alpha, beta, maximizingPlayer^1, color);

        // 回溯
        board[x][y] = EMPTY;
        bmodifyline(bline,x, y, 0);

        // Alpha-Beta更新
        if (maximizingPlayer) {
            alpha = max(alpha, value);
        } else {
            beta = min(beta, value);
        }

        // Alpha-Beta剪枝
        if (beta <= alpha) {
            return (maximizingPlayer) ? alpha : beta;
        }
    }

    // 返回最终的估值
    return (maximizingPlayer) ? alpha : beta;
}
// 标准搜索函数，返回值为最佳落子位置
// 这是第一个版本，只考虑一步
Position get_ai_move_standard(int bline[95], int board[GRID_SIZE][GRID_SIZE],int color) {

    clock_t start_time = clock();
    
    Position move;
    struct data opans={0};
    struct data ans={0};
    int oppo=(color==1)?2:1;
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
    

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("AI思考时间: %.3f秒\n", time_spent);
    printf("point: %d %d atk:%d def:%d\n",move.x,move.y,mx1,mx2);
    //display(move.x,move.y,color,1);
    return move;
}

// 线程搜索函数，用于dfs
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

// 主搜索函数，返回值为最佳落子位置
Position get_ai_move1(int bline[95], int board[GRID_SIZE][GRID_SIZE], int color) {

    // 尝试找到五子连珠
    for(int x=0; x<maxn; x++)
        for(int y=0; y<maxn; y++)
        {
            if(board[x][y] == 0) // 如果当前位置为空
            {
                board[x][y] = color; // 假设在此位置落子
                bmodifyline(bline, x, y, color); // 更新行列信息
                
                struct data ans = {0}; // 初始化评估数据
                int dira[5], dirb[5];
                processdirc(x, y, dira, dirb); // 处理方向
                
                // 检查长连禁手
                if(color == 1)
                {
                    for(int i=0; i<4; ++i)
                        if(bstrstr(bline[dira[i]], 0b010101010101, 6) != 0)
                        {
                            ans.longban = 1;
                            break;
                        }       
                }
                if(color == 1 && ans.longban > 0) 
                { 
                    continue; // 如果是长连禁手，跳过
                }
                
                // 检查五子连珠
                if(color == 1)
                {
                    for(int i=0; i<4; ++i)
                        if(bstrstr(bline[dira[i]], 0b010101010101, 6) == 0 && bstrstr(bline[dira[i]], 0b0101010101, 5) != 0)
                        {
                            ans.five = 1; 
                            break;
                        }      
                }
                else
                {
                    for(int i=0; i<4; ++i)
                        if(bstrstr(bline[dira[i]], 0b1010101010, 5) != 0)
                        {
                            ans.five = 1;
                            break;
                        }       
                }
                if(ans.five)
                {
                    Position move = {x, y};
                    printf("找到成功五子连珠\n");
                    board[x][y] = 0; // 恢复棋盘状态
                    bmodifyline(bline, x, y, 0); // 恢复行列信息
                    return move; // 返回五子连珠的位置
                }
                board[x][y] = 0; // 恢复棋盘状态
                bmodifyline(bline, x, y, 0); // 恢复行列信息
            }
        }

    // 先尝试VCF算杀
    for(int depth = 5; depth <= 11; depth += 2) {
        KillResult kill_result = kill_search(bline, board, depth, color, 1, -inf, inf);
        if(kill_result.win == 1) {
            printf("找到必胜序列(VCF), 深度: %d\n", depth);
            return kill_result.best_move; // 返回必胜序列的最佳落子
        }
        
        // 检查时间限制
        // if(time_spent >= 8.0) break;  // 超过10秒则停止算杀
    }

    // 分配内存用于存储评分位置
    ScoredPosition *scored_positions = (ScoredPosition *)malloc(sizeof(ScoredPosition) * 225);
    ScoredPosition *scored_positions1 = (ScoredPosition *)malloc(sizeof(ScoredPosition) * 225);
    ScoredPosition *scored_positions2 = (ScoredPosition *)malloc(sizeof(ScoredPosition) * 225);
    ScoredPosition *scored_positions3 = (ScoredPosition *)malloc(sizeof(ScoredPosition) * 225);

    int count = 0;

    // 初始评估所有空位
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                if(color == 1 && getscore(bscore(bline, board, i, j, color, 1), color) == ban) continue;
                // 使用浅层搜索进行初始评估
                board[i][j] = color;
                bmodifyline(bline, i, j, color);
                int moveValue = dfs(bline, board, 2, -inf, inf, 0, color); // 初始深度为2
                board[i][j] = EMPTY;
                bmodifyline(bline, i, j, 0);
                scored_positions[count++] = (ScoredPosition){{i, j}, moveValue};
            }
        }
    }

    // 排序候选位置
    qsort(scored_positions, count, sizeof(ScoredPosition), compare_scores);
    Position bestMove = {-1, -1};
    int bestValue = -inf;
    Position currentBestMove = {-1, -1};
    int currentBestValue = -inf;
    pthread_t* threads = malloc(sizeof(pthread_t) * count);
    ThreadData* thread_data = malloc(sizeof(ThreadData) * count);

    // 迭代加深搜索
    int maxdepth;
    if(totalround <= 4) maxdepth = 6;
    else maxdepth = 8;
    for(int depth = 4; depth <= maxdepth; depth += 2) {  // 从深度4开始，每次加2
        currentBestValue = -inf;
        
        count = min(count, search_depth2[depth]);
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
            
            // printf("位置(%d,%d): %d\n", 
            //        scored_positions[i].pos.x, 
            //        scored_positions[i].pos.y, 
            //        scored_positions[i].score);
            
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
        
        // 重新排序候选位置
        qsort(scored_positions, search_depth2[depth], sizeof(ScoredPosition), compare_scores);
        if(depth == 4) memcpy(scored_positions1, scored_positions, sizeof(ScoredPosition) * search_depth2[depth]);
        if(depth == 6) memcpy(scored_positions2, scored_positions, sizeof(ScoredPosition) * search_depth2[depth]);
        if(depth == 8) memcpy(scored_positions3, scored_positions, sizeof(ScoredPosition) * search_depth2[depth]);
    }

search_end:
    printf("\n搜索完成:\n");
    printf("最终选择: %d %d 评分: %d\n", bestMove.x, bestMove.y, bestValue);
    putchar(bestMove.y + 'A');
    printf("%d\n", bestMove.x + 1);
    if(bestValue <= -4320)
    {
        bestMove = get_ai_move_standard(bline, board, color);
        free(scored_positions);
        return bestMove;
    }
    free(scored_positions);
    return bestMove;
}


