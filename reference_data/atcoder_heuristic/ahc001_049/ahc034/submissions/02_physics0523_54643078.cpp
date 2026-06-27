#pragma GCC target("avx2")
#pragma GCC optimize("Ofast")
#pragma GCC optimize("unroll-loops")
#include <bits/stdc++.h>
#include <sys/time.h>
#include <atcoder/all>

using namespace std;
using namespace atcoder;

long long start_time;

void start_clock(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  start_time=(tv.tv_sec*1000000+tv.tv_usec);
}

long long current_clock(){
  struct timeval tv;
  gettimeofday(&tv, NULL);
  long long current_time=(tv.tv_sec*1000000+tv.tv_usec);
  // cout << current_time-start_time << "(us)\n";
  return current_time-start_time;
}

string udlr="UDLR";
int dx4[4]={-1,1,0,0};
int dy4[4]={0,0,-1,1};

int N;
array<array<int,20>,20> h;

void getInput(){
  cin >> N;
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      cin >> h[i][j];
    }
  }
}

bool fin(array<array<int,20>,20> &ch){
  for(auto &nx : ch){
    for(auto &ny : nx){
      if(ny!=0){return false;}
    }
  }
  return true;
}

array<array<int,400>,400> fmem;
string car(int x){
  if(x>=0){
    return "+"+to_string(x);
  }
  else{
    return to_string(x);
  }
}

vector<string> work(string &w){
  mcf_graph<int,int> g(N*N+2);
  int s=N*N,t=N*N+1;
  int need=0;
  for(int i=0;i<N;i++){
    for(int j=0;j<N;j++){
      if(h[i][j]>0){
        g.add_edge(s,i*N+j,h[i][j],1);
        need+=h[i][j];
      }
      if(h[i][j]<0){
        g.add_edge(i*N+j,t,-h[i][j],1);
      }
    }
  }

  int cx=0,cy=0;
  for(auto &dst : w){
    int nx=cx,ny=cy;
    if(dst=='U'){
      nx--;
    }
    else if(dst=='D'){
      nx++;
    }
    else if(dst=='L'){
      ny--;
    }
    else{
      ny++;
    }
    g.add_edge(cx*N+cy,nx*N+ny,1e5,1);
    cx=nx;
    cy=ny;
  }
  auto fres=g.flow(s,t);
  if(fres.first!=need){
    return {};
  }

  auto edges=g.edges();
  for(auto &e : edges){
    if(0<=e.from && e.from<N*N && 0<=e.to && e.to<N*N){
      fmem[e.from][e.to]+=e.flow;
    }
  }

  vector<string> res;
  cx=0;cy=0;
  int sd=0;
  array<array<int,20>,20> ch=h;
  for(auto &dst : w){
    int nx=cx,ny=cy;
    if(dst=='U'){
      nx--;
    }
    else if(dst=='D'){
      nx++;
    }
    else if(dst=='L'){
      ny--;
    }
    else{
      ny++;
    }
    int aim=fmem[cx*N+cy][nx*N+ny];
    int del=aim-sd;
    if(del!=0){
      res.push_back(car(del));
    }
    sd=aim;
    fmem[cx*N+cy][nx*N+ny]-=sd;
    cx=nx;
    cy=ny;

    string app="";
    app.push_back(dst);
    res.push_back(app);
  }
  if(sd!=0){
    res.push_back(car(-sd));
  }

  for(auto &e : edges){
    if(0<=e.from && e.from<N*N && 0<=e.to && e.to<N*N){
      fmem[e.from][e.to]=0;
    }
  }

  return res;
}

int eval(vector<string> &res){
  // trim
  while(true){
    if(res.back()[0]=='+'){break;}
    if(res.back()[0]=='-'){break;}
    res.pop_back();
  }
  int s=0;
  int pt=0;
  for(auto &nx : res){
    if(nx=="U"){pt+=(100+s);}
    else if(nx=="D"){pt+=(100+s);}
    else if(nx=="L"){pt+=(100+s);}
    else if(nx=="R"){pt+=(100+s);}
    else{
      int del=stoi(nx);
      pt+=abs(del);
      s+=del;
    }
  }
  return pt;
}

void oup(vector<string> &res){
  for(auto &nx : res){
    if(nx=="+0" || nx=="-0"){continue;} // fail safe
    cout << nx << "\n";
  }
}

int main(){
  start_clock();
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  getInput();

  vector<string> ws;
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<5;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<5;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<4;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<4;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<4;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<4;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<3;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<3;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<2;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<2;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
    }
    w.pop_back();
    for(int i=0;i<2;i++){
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
      w.push_back('R');
    }
    w.pop_back();
    ws.push_back(w);
  }
  {
    string w;
    for(int i=0;i<10;i++){
      for(int j=0;j<19;j++){w.push_back('D');}
      w.push_back('R');
      for(int j=0;j<19;j++){w.push_back('U');}
      w.push_back('R');
    }
    w.pop_back();
    for(int i=0;i<2;i++){
      for(int j=0;j<19;j++){w.push_back('L');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      for(int j=0;j<19;j++){w.push_back('R');}
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
      w.push_back('D');
    }
    w.pop_back();
    ws.push_back(w);
  }

  int pt=1e9;
  vector<string> res;
  while(current_clock()<1980000){
    if(ws[0].size()==0){break;}
    for(int i=0;i<ws.size();i++){
      vector<string> cres=work(ws[i]);
      if(cres.empty()){continue;}
      int cpt=eval(cres);
      if(pt>cpt){
        pt=cpt;
        res=cres;
      }
      ws[i].pop_back();
    }
  }
  oup(res);
  cerr << ws[0].size() << "\n";
  return 0;
}
