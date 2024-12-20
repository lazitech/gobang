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
#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)>(b)?(b):(a))


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

int getscore(struct data node,int color);
struct data score(int x,int y,int color,int mode);
int is_forbidden_move(int x,int y);
void modifyline(int x,int y,int k);
void substr(char* src, char* dest, int l, int r , int maxlen);
void processdirc(int x,int y,int dirca[],int dircb[]);
void init();
void draw_circle(SDL_Renderer *renderer, int x, int y, int radius);
bool check_winner(int board[GRID_SIZE][GRID_SIZE], int player, Position win_positions[5]);
int is_ban_move(int x,int y);
void display(int x,int y,int color,int mode);
void draw_board(SDL_Renderer *renderer, int board[GRID_SIZE][GRID_SIZE], Position last_move, Position win_positions[5], bool game_over);
Position get_ai_move(int board[GRID_SIZE][GRID_SIZE],int color) {
    // 添加计时开始
    clock_t start_time = clock();
    
    Position move;
    struct data opans={0};
    struct data ans={0};
    int oppo=(color==1)?2:1;
    //优先级是对方活四>对方冲四>己方活四>对方活3
    int mx1=0,mx2=0; 
     for(int pp=1;pp<=5000;++pp)
        for(register int i=0;i<maxn;i++)
        for(int j=0;j<maxn;j++)
        {
            if(board[i][j]) continue;
            int attack=getscore(score(i,j,color,1),color);
            int defence=0;
            if(color==1&&attack==-1) continue;

            struct data node={0};
            node=score(i,j,oppo,1);
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
    display(move.x,move.y,color,1);
    return move;
}

int main() {
    init();
    SDL_Init(SDL_INIT_VIDEO);
    srand(time(NULL)); // Initialize random seed

    SDL_Window *window = SDL_CreateWindow("五子棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    Position last_move = {-1, -1}; // Initialize with an invalid position
    Position win_positions[5]; // To store winning positions
    bool running = true;
    bool game_over = false; // New flag to indicate if the game is over

    // Ask the user if AI should go first
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
                    last_move.x = x;
                    last_move.y = y;
                    draw_board(renderer, board, last_move, win_positions, game_over); // Update the board immediately
                    if (check_winner(board, current_player, win_positions)) {
                        printf("%s wins!\n", current_player == BLACK ? "Black" : "White");
                        game_over = true; // Set game over flag
                    }
                    current_player = (current_player==BLACK)?WHITE:BLACK; 
                }
            }
        }

        if (!game_over &&current_player==aichoice) {
            SDL_Delay(300); // Wait for 1 second before AI makes a move
            Position ai_move = get_ai_move(board,current_player);
            board[ai_move.x][ai_move.y] = current_player;
            modifyline(ai_move.x,ai_move.y,current_player);
            last_move.x = ai_move.y;
            last_move.y = ai_move.x;
            
            if (check_winner(board, current_player, win_positions)) {
                printf("%s wins!\n", current_player == BLACK ? "Black" : "White");
                game_over = true; // Set game over flag
            }
            current_player = (current_player==BLACK)?WHITE:BLACK; 
        }

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
        for(register int i=0;i<4;++i)
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
        for(register int i=0;i<4;++i)
            if(strstr(line[dirca[i]],"111111")==NULL && strstr(line[dirca[i]],"11111")!=NULL)
                ans.five=1;       
        
    }
    else
    {
        for(register int i=0;i<4;++i)
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
        for(register int i=0;i<4;++i)
        {
            substr(line[dirca[i]],des,dircb[i]-5,dircb[i]+5,linelen[dirca[i]]);
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111103")!=NULL)
                ans.livefour++;
        }
    }
    else
    {
        for(register int i=0;i<4;++i)
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
    for(register int i=x-4;i<=x+4;++i)
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
    
    for(register int i=y-4;i<=y+4;++i)
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
    for(register int i=-4;i<=4;++i)
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
    for(register int i=-4;i<=4;++i)
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
    for(register int i=x-3;i<=x+3&&flag==0;++i)
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
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111103")!=NULL)
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
    for(register int i=y-3;i<=y+3&&flag==0;++i)
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
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111103")!=NULL)
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
    for(register int i=-3;i<=3&&flag==0;++i)
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
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111103")!=NULL)
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
    for(register int i=-3;i<=3&&flag==0;++i)
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
            if(strstr(des,"00111100")!=NULL||strstr(des,"00111102")!=NULL||strstr(des,"20111100")!=NULL||strstr(des,"20111102")!=NULL||strstr(des,"30111100")!=NULL||strstr(des,"30111102")!=NULL||strstr(des,"00111103")!=NULL||strstr(des,"20111103")!=NULL)
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
    for(register int i=0;i<4;++i)
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
    for(register int i=0;i<4;++i)
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
    for(register int i=0;i<4;++i)
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
        for(register int i=0;i<4;++i)
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
    for(register int i=l;i<=r;++i)
    {
        if(i==-1) dest[top++]='3'; 
        if(i==maxlen) dest[top++]='3';
        if(i<0||i>=maxlen) continue;
        dest[top++]=src[i];
    }
    dest[top]='\0';
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
    for (int i = 0; i < maxn; ++i) {
        char *p = line[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + board[i][j]), p++;
        *p = '\0';
    }
    // 列
    for (int i = 0; i < maxn; ++i) {
        char *p = line[top++];
        for (int j = 0; j < maxn; ++j)
            *p = ('0' + board[j][i]), p++;
        *p = '\0';
    }
    // 主对角线
    for (int i = 0; i < maxn; ++i) {
        char *p = line[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + board[j][i + j]), p++;
        *p = '\0';
    }
    for (int i = 1; i < maxn; ++i) {
        char *p = line[top++];
        for (int j = 0; i + j < maxn; ++j)
            *p = ('0' + board[i + j][j]), p++;
        *p = '\0';
    }
    // 副对角线
    for (int i = 0; i < maxn + maxn - 1; ++i) {
        char *p = line[top++];
        for (int j = 0; j <= i; ++j)
        {
            if(j<0||j>=maxn||i-j<0||i-j>=maxn) continue;
            *p = ('0' + board[j][i - j]), p++;
        }
        *p = '\0';
    }
    for(register int i=0;i<=90;++i)
        linelen[i]=strlen(line[i]),printf("%d %s\n",i,line[i]);
}

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
    for (int i = 0; i <= GRID_SIZE; ++i) {
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
                    for (int i = 0; i < border_thickness; ++i) {
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
        for (int i = 0; i < 5; ++i) {
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
                    for (int i = 0; i < 5; ++i) win_positions[i] = (Position){x+i, y};
                    return true;
                }
                if (y <= GRID_SIZE - 5 && board[y+1][x] == player && board[y+2][x] == player && board[y+3][x] == player && board[y+4][x] == player) {
                    for (int i = 0; i < 5; ++i) win_positions[i] = (Position){x, y+i};
                    return true;
                }
                if (x <= GRID_SIZE - 5 && y <= GRID_SIZE - 5 && board[y+1][x+1] == player && board[y+2][x+2] == player && board[y+3][x+3] == player && board[y+4][x+4] == player) {
                    for (int i = 0; i < 5; ++i) win_positions[i] = (Position){x+i, y+i};
                    return true;
                }
                if (x >= 4 && y <= GRID_SIZE - 5 && board[y+1][x-1] == player && board[y+2][x-2] == player && board[y+3][x-3] == player && board[y+4][x-4] == player) {
                    for (int i = 0; i < 5; ++i) win_positions[i] = (Position){x-i, y+i};
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
    printf("point: %d %d mode: %d\n",x,y,mode);
    printf("longban: %d\n",ans.longban);
    printf("five: %d\n",ans.five);
    printf("livefour: %d\n",ans.livefour);
    printf("rushfour: %d\n",ans.rushfour);
    printf("livethree: %d\n",ans.livethree);
    printf("rushthree: %d\n",ans.rushthree);
    printf("livetwo: %d\n",ans.livetwo);
    printf("rushtwo: %d\n",ans.rushtwo);
    printf("liveone: %d\n",ans.liveone);
}
