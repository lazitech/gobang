#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h> // For user input
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define GRID_SIZE 15
#define CELL_SIZE 80
#define maxn 15
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define whole 2147483647
#define inf 1000000000
#define block 1073741823
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)>(b)?(b):(a))
#define oppo(color) (((color)==1)?2:1)
//#define modifybi(n,i,k) do{bline[n]=(((1<<(i<<1))-1)&bline[n])|(k<<(i<<1))|(bline[n]&(block^((1<<(i+1<<1))-1)));}while(false)
#define modifybi(n,i,k) do{bline[(n)]=(((1<<((i)<<1))-1)&bline[(n)])|((k)<<((i)<<1))|(bline[(n)]&(block^((1<<((i)+1<<1))-1)));}while(false)

int bline[95];
int board[GRID_SIZE][GRID_SIZE] = {EMPTY};
char line[95][50];   
int linelen[95];
struct data
{
    int longban,five,livefour,rushfour,livethree,rushthree,livetwo,liveone,rushtwo;
};
int what;
int dirca[5],dircb[5];
char des[30];

typedef struct {
    int x;
    int y;
} Position;
Position win_positions[5];
int getscore(struct data node,int color);
struct data score(int x,int y,int color,int mode);
struct data bscore(int x,int y,int color,int mode);
int bsubstr(int src,int l,int r,int maxnlen);
int is_forbidden_move(int x,int y);
void modifyline(int x,int y,int k);
void substr(char* src, char* dest, int l, int r , int maxlen);
void processdirc(int x,int y,int dirca[],int dircb[]);
void init();
void draw_circle(SDL_Renderer *renderer, int x, int y, int radius);
bool check_winner(int board[GRID_SIZE][GRID_SIZE], int player, Position win_positions[5]);
int is_ban_move(int x,int y);
void display(int x,int y,int color,int mode);
void bmodifyline(int x,int y,int k);
void printbi(int x)
{
    for(int k=0;k<=31;++k)
        putchar('0'+(((1u<<k)&x)?1:0));
    puts("");
}
int bstrstr(int s,int x,int len)
{   
    int temp=((1<<(len<<1))-1);
  //  printf("%d %d ",s,x);
    for(  int i=0;i<15;++i,s>>=2)
        if((s&temp)==x) return 1;
    return 0;
}
void draw_board(SDL_Renderer *renderer, int board[GRID_SIZE][GRID_SIZE], Position last_move, Position win_positions[5], bool game_over);

#define DEPTH 1 // Define the depth of the search

int evaluate_board(int board[GRID_SIZE][GRID_SIZE],int color) {
    // Use existing scoring functions to evaluate the board
    struct data sum = {0};
    for (int i = 0; i < maxn; i++) {
        for (int j = 0; j < maxn; j++) {
            if (board[i][j] != color) continue;
            struct data mine = bscore(i, j, color, 0);
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
            struct data mine = bscore(i, j, oppo, 0);
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

        node.five /=5;
        node.livefour /=4;
        node.livethree /=3;
        node.livetwo /=2;
        node.rushfour /=4;
        node.rushthree /=3;
        node.rushtwo /=2;
    int attack=getscore(sum,color);
    int defence=node.five*100000+node.livefour*8000+node.rushfour*5000+node.livethree*2000+node.rushthree*250+node.livetwo*80+node.rushtwo*40+node.liveone*15;
            //优先级是对方活四>对方冲四>己方活四>对方活3
    //printf(" %d %d\n",attack,defence);
    return attack-defence;
}

int dfs(int board[GRID_SIZE][GRID_SIZE], int depth, int alpha, int beta, int maximizingPlayer, int color) {
    if (depth == 0 || check_winner(board, color, win_positions)) {
        return evaluate_board(board,color);
    }
    int bestValue=0;
    if (maximizingPlayer) {
        bestValue=-inf;
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = color;
                    bmodifyline(i,j,color);
                    int value = dfs(board, depth - 1, alpha, beta, 0, color);
                    board[i][j] = EMPTY;
                    bmodifyline(i,j,0);
                    bestValue=max(bestValue,value);
                    alpha=max(alpha,value);
                    if (beta <= alpha) {
                        return alpha; // Beta cut-off
                    }
                }
            }
        }
    } else {
        bestValue=inf;
        int opponentColor = (color == BLACK) ? WHITE : BLACK;
        for (int i = 0; i < GRID_SIZE; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = opponentColor;
                    bmodifyline(i,j,opponentColor);
                    int value = dfs(board, depth - 1, alpha, beta, 1, color);
                    board[i][j] = EMPTY;
                    bmodifyline(i,j,0);
                    bestValue=min(bestValue,value);
                    beta = min(beta, value);
                    if (beta <= alpha) {
                        return beta; // Alpha cut-off
                    }
                }
            }
        }
    }
    return bestValue;
}
Position get_ai_move1(int board[GRID_SIZE][GRID_SIZE], int color) {
    clock_t start_time = clock();

    Position bestMove = {-1, -1};
    int bestValue = -inf;
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = color;
                bmodifyline(i,j,color);
                //printf("%d,%d ",i,j);
                int moveValue = dfs(board, DEPTH, -inf, inf, 0, color);
                board[i][j] = EMPTY;
                bmodifyline(i,j,0);
                if (moveValue > bestValue) {
                    bestValue = moveValue;
                    bestMove.x = i;
                    bestMove.y = j;
                }
            }
        }
    }

    clock_t end_time = clock();
    double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("AI思考时间: %.3f秒\n", time_spent);
    printf("point: %d %d value: %d\n", bestMove.x, bestMove.y, bestValue);
    display(bestMove.x, bestMove.y, color, 1);
    return bestMove;
}
int main() {
    init();
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL)); // Initialize random seed

    SDL_Window *window = SDL_CreateWindow("五子棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Position last_move = {-1, -1}; // Initialize with an invalid position
     // To store winning positions
    bool running = true;
    bool game_over = false; // New flag to indicate if the game is over

    //Ask the user if AI should go first
    char choice;
    int aichoice=2;
    printf("Do you want the AI to go first? (y/n): ");
    scanf(" %c", &choice);
    int current_player = BLACK;
    if(choice=='y'||choice=='Y')
    {
        aichoice=BLACK;
        board[7][7]=1;
        modifyline(7,7,current_player);
        bmodifyline(7,7,current_player);
        
        current_player = (current_player==BLACK)?WHITE:BLACK; 
    }
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN && !game_over &&current_player!=aichoice) {
                int x = event.button.x / CELL_SIZE;
                int y = event.button.y / CELL_SIZE;
                if (board[y][x] == EMPTY) {
                    board[y][x] = current_player;
                    modifyline(y,x,current_player);
                    bmodifyline(y,x,current_player);
                    last_move.x = x;
                    last_move.y = y;
                    draw_board(renderer, board, last_move, win_positions, game_over); // Update the board immediately
                    if (check_winner(board, current_player, win_positions)) {
                        printf("%s wins!\n", current_player == BLACK ? "Black" : "White");
                        game_over = true; // Set game over flag
                    }
                    current_player = (current_player==BLACK)?WHITE:BLACK; 
                }
        //                 puts("-----------");
        // for(  int i=0;i<=90;++i)
        //     linelen[i]=strlen(line[i]),printf("%d %s\n",i,line[i]),printbi(bline[i]);
             }
        }

        if (!game_over &&current_player==aichoice) {
            SDL_Delay(1500); // Wait for 1 second before AI makes a move
            Position ai_move = get_ai_move1(board,current_player);
            board[ai_move.x][ai_move.y] = current_player;
            printf("%c%d\n",'A'+ai_move.y,maxn-ai_move.x);
            modifyline(ai_move.x,ai_move.y,current_player);
            bmodifyline(ai_move.x,ai_move.y,current_player);
               struct data sum={0};
            for(int i=0;i<maxn;i++)
                for(int j=0;j<maxn;j++)
                {
                    if(board[i][j]!=current_player) continue;
                    struct data mine=bscore(i,j,current_player,0);     
                    sum.five+=mine.five,sum.livefour+=mine.livefour,sum.liveone+=mine.liveone,sum.livethree+=mine.livethree,sum.livetwo+=mine.livetwo,sum.longban+=mine.longban,sum.rushfour+=mine.rushfour,sum.rushthree+=mine.rushthree,sum.rushtwo+=mine.rushtwo;

                }
            puts("The whole board score:");
                    sum.five /=5;
            sum.livefour /=4;
            sum.livethree /=3;
            sum.livetwo /=2;
            sum.rushfour /=4;
            sum.rushthree /=3;
            sum.rushtwo /=2;
            int ans=evaluate_board(board,(current_player));
            printf("longban: %d \n",sum.longban);
            printf("five: %d \n",sum.five);
            printf("livefour: %d \n",sum.livefour);
            printf("rushfour: %d \n",sum.rushfour);
            printf("livethree: %d \n",sum.livethree);
            printf("rushthree: %d \n",sum.rushthree);
            printf("livetwo: %d \n",sum.livetwo);
            printf("rushtwo: %d \n",sum.rushtwo);
            printf("liveone: %d \n",sum.liveone);
            printf("score: %d\n",ans);

            last_move.x = ai_move.y;
            last_move.y = ai_move.x;
            
            if (check_winner(board, current_player, win_positions)) {
                printf("%s wins!\n", current_player == BLACK ? "Black" : "White");
                game_over = true; // Set game over flag
            }
            current_player = (current_player==BLACK)?WHITE:BLACK; 
         }
// draw_board(renderer, board, last_move, win_positions, game_over);
//         if (!game_over &&current_player!=aichoice) {
//             SDL_Delay(1500); // Wait for 1 second before AI makes a move
//             Position ai_move = get_ai_move3(board,current_player);
//             board[ai_move.x][ai_move.y] = current_player;
//             printf("%c%d\n",'A'+ai_move.y,maxn-ai_move.x);
//             modifyline(ai_move.x,ai_move.y,current_player);
//             bmodifyline(ai_move.x,ai_move.y,current_player);

//             last_move.x = ai_move.y;
//             last_move.y = ai_move.x;
            
//             if (check_winner(board, current_player, win_positions)) {
//                 printf("%s wins!\n", current_player == BLACK ? "Black" : "White");
//                 game_over = true; // Set game over flag
//             }
//             current_player = (current_player==BLACK)?WHITE:BLACK; 
//         }


        draw_board(renderer, board, last_move, win_positions, game_over);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


int getscore(struct data node,int color)
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



struct data score(int x,int y,int color,int mode) // mode  0:不落子 用于计算被毁掉的组合情况 1：落子 用于计算新产生的组合情况
{
    struct data ans={0};
    
    //ans.rushtwo=ans.five=ans.livefour=ans.liveone=ans.livethree=ans.livetwo=ans.longban=ans.rushfour=ans.rushthree=0;
    if(mode) board[x][y]=color;
    if(mode) modifyline(x,y,color);
    processdirc(x,y,dirca,dircb);
    // 1. longban
    if(color==1)
    {
        for( int i=0;i<4;++i)
            if(strstr(line[dirca[i]],"111111")!=NULL)
                ans.longban=1;       
    }
    if(color==1&&ans.longban>0) 
    {
        if(mode) board[x][y]=0;
        if(mode) modifyline(x,y,0);
        return ans;
    }
    // 2. five
    if(color==1)
    {
        for( int i=0;i<4;++i)
            if(strstr(line[dirca[i]],"111111")==NULL && strstr(line[dirca[i]],"11111")!=NULL)
                ans.five=1;       
        
    }
    else
    {
        for( int i=0;i<4;++i)
            if(strstr(line[dirca[i]],"22222")!=NULL)
                ans.five=1;       
    
    }
    if(ans.five>0) 
    {
        if(mode) board[x][y]=0;
        if(mode) modifyline(x,y,0);
        return ans;
    }
    // 3. livefour
    if(color==1)
    {
        for( int i=0;i<4;++i)
        {
            substr(line[dirca[i]],des,dircb[i]-5,dircb[i]+5,linelen[dirca[i]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"20111103")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"30111103")!=NULL)
                ans.livefour++;
        }
    }
    else
    {
        for( int i=0;i<4;++i)
        {
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"022220")!=NULL)
                ans.livefour++;
        }
    }
    if(color==1&&ans.livefour>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) modifyline(x,y,0);
        return ans;
    }
    processdirc(x,y,dirca,dircb);
    // 4. rushfour
    // 0 1  2  3 
    // 横 竖 副 主
    for( int i=x-4;i<=x+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[i][y]) continue;
        board[i][y]=color;
        modifyline(i,y,color);
        
        if(color==1)
        {
            substr(line[dirca[1]],des,dircb[1]-5,dircb[1]+5,linelen[1]);
            if(strstr(des,"111111")==NULL && strstr(des,"11111")!=NULL)
                ans.rushfour++;
        }
        else
        {
            substr(line[dirca[1]],des,dircb[1]-4,dircb[1]+4,linelen[1]);
            if(strstr(des,"22222")!=NULL)
                ans.rushfour++;
        }
        board[i][y]=0;
        modifyline(i,y,0);
    }
    
    for( int i=y-4;i<=y+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        board[x][i]=color;
        modifyline(x,i,color);
        
        if(color==1)
        {
            substr(line[dirca[0]],des,dircb[0]-5,dircb[0]+5,linelen[0]);
            if(strstr(des,"111111")==NULL && strstr(des,"11111")!=NULL)
                ans.rushfour++;
        }
        else
        {
            substr(line[dirca[0]],des,dircb[0]-4,dircb[0]+4,linelen[0]);
            if(strstr(des,"22222")!=NULL)
                ans.rushfour++;
        }
        board[x][i]=0;
        modifyline(x,i,0);
    }
    for( int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        modifyline(xx,yy,color);
        
        if(color==1)
        {
            substr(line[dirca[2]],des,dircb[2]-5,dircb[2]+5,linelen[2]);
            if(strstr(des,"111111")==NULL && strstr(des,"11111")!=NULL)
                ans.rushfour++;
        }
        else
        {
            substr(line[dirca[2]],des,dircb[2]-4,dircb[2]+4,linelen[2]);
            if(strstr(des,"22222")!=NULL)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        modifyline(xx,yy,0);
    }
    for( int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        modifyline(xx,yy,color);
        
        if(color==1)
        {
            substr(line[dirca[3]],des,dircb[3]-5,dircb[3]+5,linelen[3]);
            if(strstr(des,"111111")==NULL && strstr(des,"11111")!=NULL)
                ans.rushfour++;
        }
        else
        {
            substr(line[dirca[3]],des,dircb[3]-4,dircb[3]+4,linelen[3]);
            if(strstr(des,"22222")!=NULL)
                ans.rushfour++;
        }
        board[xx][yy]=0;
        modifyline(xx,yy,0);
    }
    ans.rushfour-=ans.livefour*2;
    ans.rushfour=max(0,ans.rushfour);
    if(color==1&&ans.livefour+ans.rushfour>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) modifyline(x,y,0);
        return ans;
    }
    // 5. livethree
    
    int flag=0;
    for( int i=x-3;i<=x+3&&flag==0;++i)
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
        modifyline(i,y,color);
        
        if(color==1)
        {
            substr(line[dirca[1]],des,dircb[1]-5,dircb[1]+5,linelen[dirca[1]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"20111103")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"30111103")!=NULL)
            {
                
                struct data node=score(i,y,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dirca,dircb);
            }
        }
        else
        {
            substr(line[dirca[1]],des,dircb[1]-4,dircb[1]+4,linelen[dirca[1]]);
            if(strstr(des,"022220")!=NULL)
            {
                flag=1;
            }
        }
        board[i][y]=0;
        modifyline(i,y,0);
    }
    ans.livethree+=flag;
    flag=0;
    for( int i=y-3;i<=y+3&&flag==0;++i)
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
        modifyline(x,i,color);
        if(color==1)
        {
            substr(line[dirca[0]],des,dircb[0]-5,dircb[0]+5,linelen[dirca[0]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"20111103")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"30111103")!=NULL)
            {
                struct data node=score(x,i,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dirca,dircb);
            }
        }
        else
        {
            substr(line[dirca[0]],des,dircb[0]-4,dircb[0]+4,linelen[dirca[0]]);
            if(strstr(des,"022220")!=NULL)
                flag=1;
        }
        board[x][i]=0;
        modifyline(x,i,0);
    }
    ans.livethree+=flag;
    flag=0;
    for( int i=-3;i<=3&&flag==0;++i)
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
        modifyline(xx,yy,color);
        
        if(color==1)
        {
            substr(line[dirca[2]],des,dircb[2]-5,dircb[2]+5,linelen[dirca[2]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"20111103")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"30111103")!=NULL)
            {
                struct data node=score(xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dirca,dircb);
            }
        }
        else
        {
            substr(line[dirca[2]],des,dircb[2]-4,dircb[2]+4,linelen[dirca[2]]);
            if(strstr(des,"022220")!=NULL)
                flag=1;
        }
        board[xx][yy]=0;
        modifyline(xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    for( int i=-3;i<=3&&flag==0;++i)
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
        modifyline(xx,yy,color);
        
        if(color==1)
        {
            substr(line[dirca[3]],des,dircb[3]-5,dircb[3]+5,linelen[dirca[3]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"20111103")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"30111103")!=NULL)
            {
                struct data node=score(xx,yy,color,0);
                int temp=getscore(node,color);
                if(temp!=-1) flag=1;
                processdirc(x,y,dirca,dircb);
            }
        }
        else
        {
            substr(line[dirca[3]],des,dircb[3]-4,dircb[3]+4,linelen[dirca[3]]);
            if(strstr(des,"022220")!=NULL)
                flag=1;
        }
        board[xx][yy]=0;
        modifyline(xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    if(color==1&&ans.livethree>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) modifyline(x,y,0);
        return ans;
    }
    
    // 6. rushthree
    for( int i=0;i<4;++i)
    {
        if(color==1)
        {
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"211100")!=NULL)  ans.rushthree++;
            if(strstr(des,"311100")!=NULL)  ans.rushthree++;
            if(strstr(des,"211010")!=NULL)  ans.rushthree++;
            if(strstr(des,"311010")!=NULL)  ans.rushthree++;
            if(strstr(des,"001112")!=NULL)  ans.rushthree++;
            if(strstr(des,"001113")!=NULL)  ans.rushthree++;
            if(strstr(des,"010112")!=NULL)  ans.rushthree++;
            if(strstr(des,"010113")!=NULL)  ans.rushthree++;
            if(strstr(line[dirca[i]],"11011100")!=NULL||strstr(line[dirca[i]],"111011010")!=NULL||strstr(line[dirca[i]],"00111011")!=NULL||strstr(line[dirca[i]],"010110111")!=NULL)  ans.rushthree++;
            
        }
        else
        {
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"122200")!=NULL)  ans.rushthree++;
            if(strstr(des,"322200")!=NULL)  ans.rushthree++;
            if(strstr(des,"122020")!=NULL)  ans.rushthree++;
            if(strstr(des,"322020")!=NULL)  ans.rushthree++;
            if(strstr(des,"002221")!=NULL)  ans.rushthree++;
            if(strstr(des,"002223")!=NULL)  ans.rushthree++;
            if(strstr(des,"020221")!=NULL)  ans.rushthree++;
            if(strstr(des,"020223")!=NULL)  ans.rushthree++;
        }
    }

    // 7. livetwo
    for( int i=0;i<4;++i)
    {
         if(color==1)
        {
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+1,linelen[dirca[i]]);
            if(strstr(des,"01010")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-1,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"01010")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-2,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"01100")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+2,linelen[dirca[i]]);
            if(strstr(des,"00110")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+1,linelen[dirca[i]]);
            if(strstr(des,"010010")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-1,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"010010")!=NULL)  ans.livetwo++;     
           
        }
        else
        {
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+1,linelen[dirca[i]]);
            if(strstr(des,"02020")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-1,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"02020")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+3,linelen[dirca[i]]);
            substr(line[dirca[i]],des,dircb[i]-2,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"02200")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+2,linelen[dirca[i]]);
            if(strstr(des,"00220")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+1,linelen[dirca[i]]);
            if(strstr(des,"020020")!=NULL)  ans.livetwo++;
            substr(line[dirca[i]],des,dircb[i]-1,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"020020")!=NULL)  ans.livetwo++;     
           
        }
    }
    // 8. rushtwo
    for( int i=0;i<4;++i)
    {
        if(color==1)
        {
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"211000")!=NULL)  ans.rushtwo++;
            if(strstr(des,"201100")!=NULL)  ans.rushtwo++;
            if(strstr(des,"210100")!=NULL)  ans.rushtwo++;
            if(strstr(des,"210010")!=NULL)  ans.rushtwo++;
            if(strstr(des,"201010")!=NULL)  ans.rushtwo++;
            if(strstr(des,"000112")!=NULL)  ans.rushtwo++;
            if(strstr(des,"001102")!=NULL)  ans.rushtwo++;
            if(strstr(des,"001012")!=NULL)  ans.rushtwo++;
            if(strstr(des,"010012")!=NULL)  ans.rushtwo++;
            if(strstr(des,"010102")!=NULL)  ans.rushtwo++;

        }
        else
        {
            substr(line[dirca[i]],des,dircb[i]-4,dircb[i]+4,linelen[dirca[i]]);
            if(strstr(des,"122000")!=NULL)  ans.rushtwo++;
            if(strstr(des,"102200")!=NULL)  ans.rushtwo++;
            if(strstr(des,"120200")!=NULL)  ans.rushtwo++;
            if(strstr(des,"120020")!=NULL)  ans.rushtwo++;
            if(strstr(des,"102020")!=NULL)  ans.rushtwo++;
            if(strstr(des,"000221")!=NULL)  ans.rushtwo++;
            if(strstr(des,"002201")!=NULL)  ans.rushtwo++;
            if(strstr(des,"002021")!=NULL)  ans.rushtwo++;
            if(strstr(des,"020021")!=NULL)  ans.rushtwo++;
            if(strstr(des,"020201")!=NULL)  ans.rushtwo++;
        }
    }
    // 9. liveone
        for( int i=0;i<4;++i)
    {
        if(color==1)
        {
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"000100")!=NULL)  ans.liveone++;
            if(strstr(des,"001000")!=NULL)  ans.liveone++;
        }
        else
        {
            substr(line[dirca[i]],des,dircb[i]-3,dircb[i]+3,linelen[dirca[i]]);
            if(strstr(des,"000200")!=NULL)  ans.liveone++;
            if(strstr(des,"002000")!=NULL)  ans.liveone++;
        }
    }
    if(mode) board[x][y]=0;
    if(mode) modifyline(x,y,0);
    return ans;
}

struct data bscore(int x,int y,int color,int mode) // mode  0:不落子 用于计算被毁掉的组合情况 1：落子 用于计算新产生的组合情况
{
    struct data ans={0};
    int dira[5],dirb[5];
    //ans.rushtwo=ans.five=ans.livefour=ans.liveone=ans.livethree=ans.livetwo=ans.longban=ans.rushfour=ans.rushthree=0;
    if(mode) board[x][y]=color;
    if(mode) bmodifyline(x,y,color);
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
        if(mode) bmodifyline(x,y,0);
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
        if(mode) bmodifyline(x,y,0);
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
        if(mode) bmodifyline(x,y,0);
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
        bmodifyline(i,y,color);
        
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
        bmodifyline(i,y,0);
    }
    
    for(  int i=y-4;i<=y+4;++i)
    {
        if(i<0||i>=maxn) continue;
        if(board[x][i]) continue;
        board[x][i]=color;
        bmodifyline(x,i,color);
        
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
        bmodifyline(x,i,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y-i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(xx,yy,color);
        
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
        bmodifyline(xx,yy,0);
    }
    for(  int i=-4;i<=4;++i)
    {
        int xx=x+i;
        int yy=y+i;
        if(xx<0||xx>=maxn||yy<0||yy>=maxn) continue;
        if(board[xx][yy]) continue;
        board[xx][yy]=color;
        bmodifyline(xx,yy,color);
        
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
        bmodifyline(xx,yy,0);
    }
    ans.rushfour-=ans.livefour*2;
    ans.rushfour=max(0,ans.rushfour);
    if(color==1&&ans.livefour+ans.rushfour>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(x,y,0);
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
            if(board[j][y]==color) exist=1;
        }
        if(exist==0) continue;
        board[i][y]=color;
        bmodifyline(i,y,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[1]],dirb[1]-5,dirb[1]+5,linelen[dira[1]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                
                struct data node=bscore(i,y,color,0);
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
        bmodifyline(i,y,0);
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
        bmodifyline(x,i,color);
        if(color==1)
        {
            int des=bsubstr(bline[dira[0]],dirb[0]-5,dirb[0]+5,linelen[dira[0]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(x,i,color,0);
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
        bmodifyline(x,i,0);
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
        bmodifyline(xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[2]],dirb[2]-5,dirb[2]+5,linelen[dira[2]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(xx,yy,color,0);
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
        bmodifyline(xx,yy,0);
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
        bmodifyline(xx,yy,color);
        
        if(color==1)
        {
            int des=bsubstr(bline[dira[3]],dirb[3]-5,dirb[3]+5,linelen[dira[3]]);
            if(bstrstr(des,0b0000010101010000,8)!=0||bstrstr(des,0b1000010101010000,8)!=0||bstrstr(des,0b1100010101010000,8)!=0||bstrstr(des,0b0000010101010010,8)!=0||bstrstr(des,0b1000010101010010,8)!=0||bstrstr(des,0b1100010101010010,8)!=0||bstrstr(des,0b0000010101010011,8)!=0||bstrstr(des,0b1000010101010011,8)!=0||bstrstr(des,0b1100010101010011,8)!=0)
            {
                struct data node=bscore(xx,yy,color,0);
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
        bmodifyline(xx,yy,0);
    }
    ans.livethree+=flag;
    flag=0;
    if(color==1&&ans.livethree>1) 
    {
        if(mode) board[x][y]=0;
        if(mode) bmodifyline(x,y,0);
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
    if(mode) bmodifyline(x,y,0);
    return ans;
}


int is_forbidden_move(int x,int y)
{
    //return 0;
    struct data ans=score(x,y,1,1);
    int temp=getscore(ans,1);
    if(temp==-1)   
    {
        // printf("There is a ban on: %d %d\n",x,y);
        // printf("longban: %d\n",ans.longban);
        // printf("five: %d\n",ans.five);
        // printf("livefour: %d\n",ans.livefour);
        // printf("rushfour: %d\n",ans.rushfour);
        return 1;
    }
    return 0;
}


 void substr(char* src, char* dest, int l, int r , int maxlen) {
    int top=0;
    for(  int i=l;i<=r;++i)
    {
        if(i==-1) dest[top++]='3'; 
        if(i==maxlen) dest[top++]='3';
        if(i<0||i>=maxlen) continue;
        dest[top++]=src[i];
    }
    dest[top]='\0';
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
        int *pp = &bline[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + board[i][j]), p++,*pp|=(board[i][j]<<(j+j));
        *p = '\0';
    }
    // 列
    for ( int i = 0; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &bline[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + board[j][i]), p++,*pp|=(board[j][i]<<(j+j));
        *p = '\0';
    }
    // 主对角线
    for ( int i = 0; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &bline[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + board[j][i + j]), p++, *pp|=(board[j][i + j]<<(j+j));
        *p = '\0';
    }
    for ( int i = 1; i < maxn; ++i) {
        char *p = line[top];
        int *pp = &bline[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + board[i + j][j]), p++,*pp|=(board[i + j][j]<<(j+j));
        *p = '\0';
    }
    // 副对角线
    for ( int i = 0; i < maxn + maxn - 1; ++i) {
        char *p = line[top];
        int *pp = &bline[top++];
        for (int j = 0; j <= i; ++j)
        {
            if(j<0||j>=maxn||i-j<0||i-j>=maxn) continue;
            *p = ('0' + board[j][i - j]), p++,*pp|=(board[j][i - j]<<(j+j));
        }
        *p = '\0';
    }
     for(  int i=0;i<=90;++i)
         linelen[i]=strlen(line[i]);//,printf("%d %s\n",i,line[i]),printbi(bline[i]);
}

// void modifybi(int n, int i,int k )
// {
//     bline[n]=(((1<<(i<<1)-1)&bline[n])|(k<<(i<<1))|(bline[n]&(block^((1<<(i+1<<1))-1)));
// }

 void modifyline(int x,int y,int k)
{

    line[x][y]='0'+k;
    line[maxn+y][x]='0'+k;
    if(x+y>14)
    {
        line[59+x+y][14-y]='0'+k;
    
    }
    else
    {
        line[59+x+y][x]='0'+k;
        
    }
    if(x==y)
    {
        line[30][x]='0'+k;
    
    }
    else
    {
        if(x>y)
        {
            line[44+x-y][y]='0'+k;
        }
        else
        {   
            line[30+y-x][x]='0'+k;
        }
    }
    return ;
}
 void bmodifyline(int x,int y,int k)
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


void draw_circle(SDL_Renderer *renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void draw_board(SDL_Renderer *renderer, int board[GRID_SIZE][GRID_SIZE], Position last_move, Position win_positions[5], bool game_over) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for ( int i = 0; i <= GRID_SIZE; ++i) {
        SDL_RenderDrawLine(renderer, i * CELL_SIZE, 0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE);
        SDL_RenderDrawLine(renderer, 0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE, i * CELL_SIZE);
    }

    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (board[y][x] != EMPTY) {
                SDL_SetRenderDrawColor(renderer, board[y][x] == BLACK ? 0 : 255, 0, 0, 255);
                int centerX = x * CELL_SIZE + CELL_SIZE / 2;
                int centerY = y * CELL_SIZE + CELL_SIZE / 2;
                draw_circle(renderer, centerX, centerY, CELL_SIZE / 3);

                // Highlight the last move
                if (y == last_move.y && x == last_move.x) {
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red border for last move
                    int border_thickness = 3; // Define the thickness of the border
                    for ( int i = 0; i < border_thickness; ++i) {
                        SDL_Rect rect = {x * CELL_SIZE + CELL_SIZE / 2 - CELL_SIZE / 3 - i, 
                                       y * CELL_SIZE + CELL_SIZE / 2 - CELL_SIZE / 3 - i, 
                                       2 * CELL_SIZE / 3 + 2 * i, 
                                       2 * CELL_SIZE / 3 + 2 * i};
                        SDL_RenderDrawRect(renderer, &rect);
                    }
                }
            }
        }
    }

    // Highlight the winning positions
    if (game_over) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for winning line
        for ( int i = 0; i < 5; ++i) {
            int centerX = win_positions[i].x * CELL_SIZE + CELL_SIZE / 2;
            int centerY = win_positions[i].y * CELL_SIZE + CELL_SIZE / 2;
            draw_circle(renderer, centerX, centerY, CELL_SIZE / 3);
        }
    }

    SDL_RenderPresent(renderer);
}

bool check_winner(int board[GRID_SIZE][GRID_SIZE], int player, Position win_positions[5]) {
    // 检查是否有五子连珠
    for (int y = 0; y < GRID_SIZE; ++y) {
        for (int x = 0; x < GRID_SIZE; ++x) {
            if (board[y][x] == player) {
                // 水平、垂直、对角线检查
                if (x <= GRID_SIZE - 5 && board[y][x+1] == player && board[y][x+2] == player && board[y][x+3] == player && board[y][x+4] == player) {
                    for ( int i = 0; i < 5; ++i) win_positions[i] = (Position){x+i, y};
                    return true;
                }
                if (y <= GRID_SIZE - 5 && board[y+1][x] == player && board[y+2][x] == player && board[y+3][x] == player && board[y+4][x] == player) {
                    for ( int i = 0; i < 5; ++i) win_positions[i] = (Position){x, y+i};
                    return true;
                }
                if (x <= GRID_SIZE - 5 && y <= GRID_SIZE - 5 && board[y+1][x+1] == player && board[y+2][x+2] == player && board[y+3][x+3] == player && board[y+4][x+4] == player) {
                    for ( int i = 0; i < 5; ++i) win_positions[i] = (Position){x+i, y+i};
                    return true;
                }
                if (x >= 4 && y <= GRID_SIZE - 5 && board[y+1][x-1] == player && board[y+2][x-2] == player && board[y+3][x-3] == player && board[y+4][x-4] == player) {
                    for ( int i = 0; i < 5; ++i) win_positions[i] = (Position){x-i, y+i};
                    return true;
                }
            }
        }
    }
    return false;
}


int is_ban_move(int x,int y)
{
    int temp=getscore(score(x,y,1,1),1);
    if(temp==-1) return 1;
    return 0;
}


void display(int x,int y,int color,int mode)
{
    struct data ans=score(x,y,color,mode);
    struct data bans=bscore(x,y,color,mode);
    printf("point: %d %d mode: %d\n",x,y,mode);
    printf("longban: %d \n",ans.longban);
    printf("five: %d \n",ans.five);
    printf("livefour: %d \n",ans.livefour);
    printf("rushfour: %d \n",ans.rushfour);
    printf("livethree: %d \n",ans.livethree);
    printf("rushthree: %d \n",ans.rushthree);
    printf("livetwo: %d \n",ans.livetwo);
    printf("rushtwo: %d \n",ans.rushtwo);
    printf("liveone: %d \n",ans.liveone);
    if(ans.five!=bans.five||ans.livefour!=bans.livefour||ans.liveone!=bans.liveone||ans.livethree!=bans.livethree||ans.livetwo!=bans.livetwo||ans.longban!=bans.longban||ans.rushfour!=bans.rushfour||ans.rushthree!=bans.rushthree||ans.rushtwo!=bans.rushtwo)
    {
        puts("No!!!");
        puts("No!!!");
        puts("No!!!");
        puts("No!!!");
        puts("No!!!");
        printf("point: %d %d mode: %d\n",x,y,mode);
    printf("longban: %d \n",bans.longban);
    printf("five: %d \n",bans.five);
    printf("livefour: %d \n",bans.livefour);
    printf("rushfour: %d \n",bans.rushfour);
    printf("livethree: %d \n",bans.livethree);
    printf("rushthree: %d \n",bans.rushthree);
    printf("livetwo: %d \n",bans.livetwo);
    printf("rushtwo: %d \n",bans.rushtwo);
    printf("liveone: %d \n",bans.liveone);
    }
    else
    {
        puts("Yes");
    }
    puts("-------------------");
}