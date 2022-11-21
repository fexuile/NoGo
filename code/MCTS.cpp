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
namespace MCTS{
    int total=0,num=0;
    int b[1000001][N][N];
    bool in(int x,int y){return x>=0&&x<9&&y>=0&&y<9;}
    int air(int id,int x,int y){
        vis_air[x][y]=1;
        int flag=0;
        for(int i=0;i<4;i++){
            int X=x+dx[i],Y=y+dy[i];
            if(!in(X,Y))continue;
            if(b[id][X][Y]==0)flag|=1;
            else if(b[id][X][Y]==b[id][x][y]&&!vis_air[X][Y])flag|=air(id,X,Y);
        }
        return flag;
    }
    bool ok(int id,int x,int y,int col){
        if(b[id][x][y])return false;
        b[id][x][y]=col;memset(vis_air,0,sizeof(vis_air));
        if(!air(id,x,y)){b[id][x][y]=0;return false;}
        for(int i=0;i<4;i++){
            int X=x+dx[i],Y=y+dy[i];
            if(!in(X,Y))continue;
            if(b[id][X][Y]&&!vis_air[X][Y]){
                if(!air(id,X,Y)){b[id][x][y]=0;return false;}
            }
        }
        b[id][x][y]=0;return true;
    }
    vector<int> sonlist(int id,int col){
        vector<int>son;son.clear();
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,col))son.push_back(i*9+j);
        return son;
    }
    bool end(int id,int col){//col将无处可下 - true ; 否则 - false
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,col))return false;
        return true;
    }
    vector<int>son[1000001];
    int vali[1000001],ni[1000001],put[1000001];
    void expand(int id,int col){
        vector<int>list=sonlist(id,col);
        for(int s:list){
            num++;son[num].clear();
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)b[num][i][j]=b[id][i][j];
            int x=s/9,y=s%9;
            b[num][x][y]=col;
            vali[num]=ni[num]=0;put[num]=s;
            son[id].push_back(num);
        }
    }
    int h(int id,int col){ //col=-1,h>0 ; col = 1, h < 0
        int s=0;
        if(end(id,-col))return -col*100;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,col))s++;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)
                if(ok(id,i,j,-col))s--;
        return s;
    }
    void rollout(int id,int col){
        int T=0,status=id,pre=num;
        while(T<4){
            T++;
            if(end(status,-col))break;
            ++num;vector<int>list=sonlist(status,col);
            int s=list[rand()%list.size()];
            for(int i=0;i<9;i++)
                for(int j=0;j<9;j++)b[num][i][j]=b[status][i][j];
            int x=s/9,y=s%9;
            b[num][x][y]=col;
            status=num;col=-col;
        }
        ni[id]=1;vali[id]=-col*h(status,col);num=pre;
    }
    bool cmp(int x,int y){return (double)2*sqrt((log2(total)*1./ni[x]))+1.*vali[x]/ni[x]>(double)2*sqrt((log2(total)*1./ni[y]))+1.*vali[y]/ni[y];}
    void update(int id){
        ni[id]++;vali[id]=0;
        for(auto s:son[id])vali[id]+=vali[s];
    }
    void mcts(int id,int col){
        if(end(id,-col)){vali[id]=col*(-100);ni[id]++;return;}
        if(son[id].size()==0){
            if(!ni[id]){
                rollout(id,col);
                return;
            }
            else{
                expand(id,col);
                mcts(son[id][0],-col);
                update(id);
            }
        }
        else{
            for(auto s:son[id])
                if(!ni[s]){mcts(s,-col);update(id);return;}
            sort(son[id].begin(),son[id].end(),cmp);
            mcts(son[id][0],-col);
            update(id);
        }
    }
    vector<int> work(){
        num=0;total=0;
        for(int i=0;i<9;i++)
            for(int j=0;j<9;j++)b[0][i][j]=board[i][j];
        double Time=clock();son[0].clear();ni[0]=0;vali[0]=0;
        expand(0,-1);
        while(clock()-Time<800.0 && num<5000){
            total++;
            mcts(0,-1);
        }
        vector<int>ans;ans.clear();double val=0;int anss=0;
        for(auto s:son[0])
            if(vali[s]*1./ni[s]>val)val=vali[s]*1./ni[s],anss=put[s];
        //if(son[0].empty())return Greedy::work();
        ans.push_back(anss);
        return ans;
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
    vector<int>ans = MCTS::work();
    output(ans);
    return 0;
}