#include<bits/stdc++.h>
#include"jsoncpp/json.h"
using namespace std;
typedef pair<int,int> pii;
const int N=10;
int board[N][N],dx[4]={0,1,0,-1},dy[4]={1,0,-1,0};
int vis_air[N][N];
void read(){
    string str;
	int x,y;
	getline(cin,str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input); 
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++) 
	{
		x=input["requests"][i]["x"].asInt(), y=input["requests"][i]["y"].asInt();
		if (x!=-1) board[x][y]=1;
		x=input["responses"][i]["x"].asInt(), y=input["responses"][i]["y"].asInt();
		if (x!=-1) board[x][y]=-1;
	}
	x=input["requests"][turnID]["x"].asInt(), y=input["requests"][turnID]["y"].asInt();
	if (x!=-1) board[x][y]=1;
}
    namespace Greedy{
        int val[N][N],dis[N][N];
        bool in(int x,int y){return x>=0&&x<9&&y>=0&&y<9;}
        int air(int x,int y){
            vis_air[x][y]=1;
            int flag=0;
            for(int i=0;i<4;i++){
                int X=x+dx[i],Y=y+dy[i];
                if(!in(X,Y))continue;
                if(board[X][Y]==0)flag|=1;
                else if(board[X][Y]==board[x][y]&&!vis_air[X][Y])flag|=air(X,Y);
            }
            return flag;
        }
        bool ok(int x,int y,int col){
            if(board[x][y])return false;
            board[x][y]=col;memset(vis_air,0,sizeof(vis_air));
            if(!air(x,y)){board[x][y]=0;return false;}
            for(int i=0;i<4;i++){
                int X=x+dx[i],Y=y+dy[i];
                if(!in(X,Y))continue;
                if(board[X][Y]&&!vis_air[X][Y]){
                    if(!air(X,Y)){board[x][y]=0;return false;}
                }
            }
            board[x][y]=0;return true;
        }
        int h(int x,int y,int col){
            board[x][y]=col;int s=0;
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    if(ok(i,j,col))s++;
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    if(ok(i,j,-col))s--;
            board[x][y]=0;
            return s;
        }
        int Dis(int x1,int y1,int x2,int y2){return abs(x1-x2)+abs(y1-y2);}
        vector<int> work(){
            vector<int>maybe;
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    val[i][j]=-100,dis[i][j]=100;
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    if(!board[i][j])
                        for(int k=0;k<9;k++)
                            for(int l=0;l<9;l++)
                                if(board[k][l])dis[i][j]=min(dis[i][j],Dis(i,j,k,l));
            maybe.clear();

            int mx=-100,distant=-100;
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    if(ok(i,j,-1)){
                        val[i][j]=h(i,j,-1);
                        if(val[i][j]>mx){
                            mx=max(mx,val[i][j]);
                            distant=dis[i][j];
                        }
                        else if(val[i][j]==mx)distant=max(distant,dis[i][j]);
                    }
                
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)
                    if(val[i][j]==mx&&dis[i][j]==distant)maybe.push_back(i*9+j);
            return maybe;
        }
    }
void output(vector<int>ans){
    int result=ans[rand()%ans.size()];
	Json::Value ret;
	Json::Value action;
    action["x"]=result/9; action["y"]=result%9;
	ret["response"]=action;
	Json::FastWriter writer;
	cout<<writer.write(ret)<<endl;
}

int main(){
    srand((unsigned)time(0));
    read();
    vector<int>ans = Greedy::work();
    output(ans);
    return 0;
}