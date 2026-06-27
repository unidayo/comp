#define NDEBUG
//#undef NDEBUG
#pragma GCC optimize("O3","unroll-loops","omit-frame-pointer","inline") //Optimization flags
#pragma GCC option("arch=native","tune=native","no-zero-upper") //Enable AVX
#pragma GCC target("avx")  //Enable AVX
#ifdef __linux
#include <x86intrin.h> //AVX/SSE Extensions
#endif
#define OUTPUT_INPUT
#define OUTPUT_EVAL_NUM
#pragma warning(1:4456)
#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES
#define ALL(a) (a).begin(),(a).end()
#define MEM2LOC(type,name) type name=(this->name);
#define MEM2LOC_V(type,name) type name=((this->name).data());
#define LOC2MEM(name) (this->name)=(name);
#include<iostream>
#include<array>
#include<vector>
#include<list>
#include<stack>
#include<queue>
#include<set>
#include<map>
#include<unordered_set>
#include<unordered_map>
#include<algorithm>
#include<numeric>
#include<iomanip>
#include<math.h>
#include<fstream>
#include<string>
#include<sstream>
#include<cmath>
#include<memory>
#include<cassert>
#include<functional>
#include<chrono>
#include<bitset>
#ifdef __linux
#include<sys/resource.h>
#endif
using namespace std;
using namespace chrono;
constexpr int W=20;
constexpr int H=20;
constexpr int WH=W*H;
constexpr int MAX_W=20;
constexpr int MAX_H=20;
constexpr int MAX_WH=MAX_W*MAX_H;
constexpr double DIST_INF=1e100;
constexpr int DX4[]={0,-1,0,1};
constexpr int DY4[]={-1,0,1,0};
constexpr int DX8[]={0,-1,-1,-1,0,1,1,1};
constexpr int DY8[]={-1,-1,0,1,1,1,0,-1};
constexpr int DX5[]={DX4[0],DX4[1],DX4[2],DX4[3],0};
constexpr int DY5[]={DY4[0],DY4[1],DY4[2],DY4[3],0};
constexpr int DX9[]={DX8[0],DX8[1],DX8[2],DX8[3],DX8[4],DX8[5],DX8[6],DX8[7],0};
constexpr int DY9[]={DY8[0],DY8[1],DY8[2],DY8[3],DY8[4],DY8[5],DY8[6],DY8[7],0};
typedef unsigned int uint;
typedef long long ll;
typedef unsigned long long ull;
template<class T,size_t A>using array1=array<T,A>;
template<class T,size_t A,size_t B>using array2=array<array<T,B>,A>;
template<class T,size_t A,size_t B,size_t C>using array3=array<array<array<T,C>,B>,A>;
template<class T,size_t A,size_t B,size_t C,size_t D>using array4=array<array<array<array<T,D>,C>,B>,A>;
template<class T,size_t A,size_t B,size_t C,size_t D,size_t E>using array5=array<array<array<array<array<T,E>,D>,C>,B>,A>;
template<class T>using vector1=vector<T>;
template<class T>using vector2=vector<vector<T>>;
template<class T>using vector3=vector<vector<vector<T>>>;
template<class T>using vector4=vector<vector<vector<vector<T>>>>;
template<class T>using vector5=vector<vector<vector<vector<vector<T>>>>>;
template<typename Key,typename Value>using umap=std::unordered_map<Key,Value>;
template<typename T>using uset=std::unordered_set<T>;
template<typename T>using sptr=typename std::shared_ptr<T>;
template<class _Iter>using iter_cat_type=typename std::iterator_traits<_Iter>::iterator_category;
template<class _Ty,class=void>constexpr bool _is_iterator=false;
template<class _Ty>constexpr bool _is_iterator<_Ty,std::void_t<iter_cat_type<_Ty>>> = true;
template<typename T>void fill(vector<T>&a,const T&v){std::fill(ALL(a),v);}
template<typename T>void fill(vector<vector<T>>&a,const T&v){for(vector<T>&t:a)std::fill(ALL(t),v);}
template<typename T>void resize(vector<T>&a,int sz1){a.resize(sz1);}
template<typename T>void resize(vector<T>&a,int sz1,const T&v){a.resize(sz1,v);}
template<typename T>void resize(vector<vector<T>>&a,int sz1,int sz2){a.resize(sz1);for(vector<T>&t:a)t.resize(sz2);}
template<typename T>void resize(vector<vector<T>>&a,int sz1,int sz2,const T&v){a.resize(sz1);for(vector<T>&t:a)t.resize(sz2,v);}
template<typename T>void assign(vector<T>&a,int sz1,const T&v){a.assign(sz1,v);}
template<typename T>void assign(vector<vector<T>>&a,int sz1,int sz2,const T&v){a.resize(sz1);for(vector<T>&t:a)t.assign(sz2,v);}
template<typename T,size_t SZ>void fill(array<T,SZ>&a,const T&v){std::fill(ALL(a),v);}
template<typename T,size_t SZ1,size_t SZ2>void fill(array2<T,SZ1,SZ2>&a,const T&v){for(array<T,SZ2>&t:a)std::fill(ALL(t),v);}
template<typename T,typename U>ostream&operator<<(ostream&os,const pair<T,U>&pr){os<<"{"<<pr.first<<","<<pr.second<<"}";return os;}
template<typename T>ostream&operator<<(ostream&os,vector<T>&a){os<<"[";for(int i=0;i<a.size();i++){if(i!=0){os<<",";}os<<a[i];}os<<"]";return os;}
template<typename T,size_t SZ>ostream&operator<<(ostream&os,const array<T,SZ>&a){os<<"[";for(int i=0;i<SZ;i++){if(i!=0){os<<",";}os<<a[i];}os<<"]";return os;}
template<typename T,typename U>ostream&operator<<(ostream&os,const map<T,U>&m){os<<"{";auto b=m.begin();auto e=m.end();for(auto p=b;p!=e;++p){if(p!=b){os<<",";}os<<p->first<<":"<<p->second;}os<<"}";return os;}
template<typename T>ostream&operator<<(ostream&os,const set<T>&s){os<<"{";auto b=s.begin();auto e=s.end();for(auto p=b;p!=e;++p){if(p!=b){os<<",";}os<<*p;}os<<"}";return os;}
template<typename T,typename U>ostream&operator<<(ostream&os,const unordered_map<T,U>&m){os<<"{";auto b=m.begin();auto e=m.end();for(auto p=b;p!=e;++p){if(p!=b){os<<",";}os<<p->first<<":"<<p->second;}os<<"}";return os;}
template<typename T>ostream&operator<<(ostream&os,const unordered_set<T>&s){os<<"{";auto b=s.begin();auto e=s.end();for(auto p=b;p!=e;++p){if(p!=b){os<<",";}os<<*p;}os<<"}";return os;}
constexpr int intPow(int x,int n){assert(x>=0);assert(n>=0);if(x==0)if(n==0)return 1;else return 0;int r=1,z=x;while(1){if(n&1)r*=z;n>>=1;if(n<=0)break;z*=z;}return r;}
bool same(double a,double b){return abs(a-b)<1e-5;}
bool same(int a,int b){return a==b;}
ull xor128(){static ull x=123456789,y=362436069,z=521288629,w=88675123;ull t=(x^(x<<11));x=y;y=z;z=w;return w=(w^(w>>19))^(t^(t>>8));}
string strConcat(const vector<int>&strList,const string&separator){stringstream s;for(int i=0;i<(int)strList.size();++i){if(i>0){s<<separator;}s<<strList[i];}return move(s.str());}
string trimLeft(const string&str,char ch){int len=(int)str.length();for(int i=0;i<len;++i){if(str[i]!=ch){return str.substr(i);}}return "";}
string trimRight(const string&str,char ch){int len=(int)str.length();for(int i=len-1;i>=0;--i){if(str[i]!=ch){return str.substr(0,i+1);}}return "";}
string planeDouble(double val,int prec){assert(prec>=0);stringstream ss;ss<<fixed<<setprecision(prec)<<val;return move(ss.str());}
string fillZero(ull num,int digit){int zeroNum=max(0,(int)(digit-to_string(num).length()));return move(move(string(zeroNum,'0'))+move(to_string(num)));}
#ifdef __linux
void setStackSizeFull(){const rlim_t s=4294967295;struct rlimit l;int r;r=getrlimit(RLIMIT_STACK,&l);if(r==0){if(l.rlim_cur<s){l.rlim_cur=s;r=setrlimit(RLIMIT_STACK,&l);}}}
#endif

template<typename T,size_t CAPACITY>class ArrayVector{public:ArrayVector():mSize(0){}ArrayVector(size_t size):mSize(0){resize(size);}ArrayVector(size_t size,const T&initVal):mSize(0){resize(size,initVal);}ArrayVector(const ArrayVector<T,
CAPACITY>&other){copyInit(other);}ArrayVector&operator=(const ArrayVector<T,CAPACITY>&other){return copyInit(other);}ArrayVector(ArrayVector<T,CAPACITY>&&other){moveInit(std::move(other));}ArrayVector&operator=(ArrayVector<T,CAPACITY>&&other
){return moveInit(std::move(other));}private:inline ArrayVector&copyInit(const ArrayVector<T,CAPACITY>&other){T*p=begin();for(const T*op=other.begin();op!=other.end();++p,++op){*p=*op;}mSize=other.mSize;return *this;}inline ArrayVector&
moveInit(ArrayVector<T,CAPACITY>&&other){T*p=begin();for(const T*op=other.begin();op!=other.end();++p,++op){*p=std::move(*op);}mSize=other.mSize;return *this;}public:inline void clear(){mSize=0;}inline void resize(size_t size){assert(
size<=CAPACITY);mSize=size;}inline void resize(size_t size,const T&initVal){assert(size<=CAPACITY);if(size>mSize){T*e=&mData[size];for(T*p=&mData[mSize];p!=e;++p){*p=initVal;}}mSize=size;}inline void assign(size_t size,const T&initVal){assert
(size<=CAPACITY);std::fill_n(mData,size,initVal);mSize=size;}inline void add_size(size_t amount){assert(mSize+amount<=CAPACITY&&mSize+amount>=mSize);resize(mSize+amount);}inline void reduce_size(size_t amount){assert(mSize>=amount);resize(
mSize-amount);}constexpr size_t size()const noexcept{return mSize;}constexpr size_t capacity()const noexcept{return CAPACITY;}constexpr bool empty()const noexcept{return !mSize;}constexpr bool full()const noexcept{return mSize==CAPACITY;}
template<typename... _Valty>T&emplace_back(_Valty&&... args){assert(mSize!=CAPACITY);T*target=&mData[mSize++];::new(target) T(std::forward<_Valty>(args)...);return *target;}inline void push_back(const T&val){assert(mSize!=CAPACITY);mData[
mSize++]=val;}inline void push_back(T&&val){assert(mSize!=CAPACITY);mData[mSize++]=std::move(val);}inline void pop_back(){assert(mSize!=0);--mSize;}T*erase(T*_Where){for(T*p=_Where+1,*e=end();p!=e;++p){*(p-1)=*p;}--mSize;return end();}T*
erase(T*_First,T*_Last){auto dist=_Last-_First;for(T*p=_Last,*e=end();p!=e;++p){*(p-dist)=*p;}mSize-=dist;return end();}inline T&operator[](size_t index){assert(index<mSize);return mData[index];}constexpr const T&operator[](size_t index)
const{assert(index<mSize);return mData[index];}inline T&at(size_t index){if(index>=mSize){throw out_of_range("ArrayVector: out of range.");}return mData[index];}constexpr const T&at(size_t index)const{if(index>=mSize){throw out_of_range(
"ArrayVector: out of range.");}return mData[index];}inline T&front(){assert(mSize!=0);return mData[0];}constexpr const T&front()const{assert(mSize!=0);return mData[0];}inline T&back(){assert(mSize!=0);return mData[mSize-1];}constexpr
const T&back()const{assert(mSize!=0);return mData[mSize-1];}inline T*data() noexcept{return mData;}constexpr const T*data()const noexcept{return mData;}inline T*begin() noexcept{return mData;}constexpr const T*begin()const noexcept{return
mData;}inline T*end() noexcept{return &mData[mSize];}constexpr const T*end()const noexcept{return &mData[mSize];}template <typename... _Valty>inline T*emplace(const T*_Where,_Valty&&... args){assert(mSize!=CAPACITY);assert(_Where>=begin());
assert(_Where<=end());++mSize;for(T*p=end()-1;p!=_Where;--p){*p=*(p-1);}::new(_Where) T(std::forward<_Valty>(args)...);return _Where;}inline T*insert(T*_Where,const T&_Val){assert(mSize!=CAPACITY);assert(_Where>=begin());assert(_Where<=end());
++mSize;for(T*p=end()-1;p!=_Where;--p){*p=*(p-1);}*_Where=_Val;return _Where;}inline T*insert(T*_Where,T&&_Val){assert(mSize!=CAPACITY);assert(_Where>=begin());assert(_Where<=end());++mSize;for(T*p=end()-1;p!=_Where;--p){*p=*(p-1);}*_Where=
std::move(_Val);return _Where;}inline T*insert(T*_Where,const size_t _Count,const T&_Val){assert(mSize+_Count<=CAPACITY&&mSize+_Count>=mSize);assert(_Where>=begin());assert(_Where<=end());mSize+=_Count;for(T*p=end()-1,*e=_Where+_Count-1;p!=e;
--p){*p=*(p-_Count);}for(T*p=_Where,*e=_Where+_Count;p!=e;++p){*p=_Val;}return _Where;}private:size_t mSize;T mData[CAPACITY];};template<typename T,size_t CAPACITY>ostream&operator<<(ostream&os,ArrayVector<T,CAPACITY>&a){os<<"[";for(int i=0;
i<a.size();i++){if(i!=0){os<<",";}os<<a[i];}os<<"]";return os;}
template<typename T,size_t SZ>void fill(ArrayVector<T,SZ>&a,const T&v){std::fill(ALL(a),v);}
template<typename T,size_t SZ1,size_t SZ2>void fill(ArrayVector<ArrayVector<T,SZ2>,SZ1>&a,const T&v){for(ArrayVector<T,SZ2>&t:a)std::fill(ALL(t),v);}
template<typename T,size_t SZ>void resize(ArrayVector<T,SZ>&a,int sz1){a.resize(sz1);}
template<typename T,size_t SZ>void resize(ArrayVector<T,SZ>&a,int sz1,const T&v){a.resize(sz1,v);}
template<typename T,size_t SZ1,size_t SZ2>void resize(ArrayVector<ArrayVector<T,SZ2>,SZ1>&a,int sz1,int sz2){a.resize(sz1);for(ArrayVector<T,SZ2>&t:a)t.resize(sz2);}
template<typename T,size_t SZ1,size_t SZ2>void resize(ArrayVector<ArrayVector<T,SZ2>,SZ1>&a,int sz1,int sz2,const T&v){a.resize(sz1);for(ArrayVector<T,SZ2>&t:a)t.resize(sz2,v);}
template<typename T,size_t SZ>void assign(ArrayVector<T,SZ>&a,int sz1,const T&v){a.assign(sz1,v);}
template<typename T,size_t SZ1,size_t SZ2>void assign(ArrayVector<ArrayVector<T,SZ2>,SZ1>&a,int sz1,int sz2,const T&v){a.resize(sz1);for(ArrayVector<T,SZ2>&t:a)t.assign(sz2,v);}

template <typename _Ty,size_t CAPACITY>class ArrayQueue{public:ArrayQueue():mLeft(mData),mRight(mData),mSize(0){}ArrayQueue(const ArrayQueue&other){copyInit(other);}ArrayQueue&operator=(const ArrayQueue&other){return copyInit(other);}
ArrayQueue(ArrayQueue&&other){moveInit(std::move(other));}ArrayQueue&operator=(ArrayQueue&&other){return moveInit(std::move(other));}private:inline ArrayQueue&copyInit(const ArrayQueue&other){mLeft=mData;_Ty*p=mLeft;const _Ty*op=other.mLeft;
size_t count=other.mSize;const _Ty*opCap=other.mData+CAPACITY-1;for(;count;++p,((op==opCap)?op=mLeft:++op),--count){*p=*op;}mRight=(p==mData+CAPACITY)?mLeft:p;mSize=other.mSize;return *this;}inline ArrayQueue&moveInit(ArrayQueue&&other){mLeft=
mData;_Ty*p=mLeft;const _Ty*op=other.mLeft;size_t count=other.mSize;const _Ty*opCap=other.mData+CAPACITY-1;for(;count;++p,((op==opCap)?op=mLeft:++op),--count){*p=std::move(*op);}mRight=(p==mData+CAPACITY)?mLeft:p;mSize=other.mSize;return *this
;}public:template<typename... _Valty>void emplace(_Valty&&... args){assert(mSize!=CAPACITY);::new(mRight) _Ty(std::forward<_Valty>(args)...);if(++mRight==mData+CAPACITY){mRight=mData;}++mSize;}void push(const _Ty&_Val){assert(mSize!=CAPACITY);
*mRight=_Val;if(++mRight==mData+CAPACITY){mRight=mData;}++mSize;}void pop(){assert(!this->empty());if(++mLeft==mData+CAPACITY){mLeft=mData;}--mSize;}const _Ty&front()const{assert(!this->empty());return *mLeft;}const _Ty&back()const{assert(!
this->empty());return (mRight==mData)?mData[CAPACITY-1]:*(mRight-1);}size_t capacity()const{return CAPACITY;}size_t size()const{return mSize;}bool empty()const{return mSize==0;}void clear(){mLeft=mData;mRight=mData;mSize=0;}const _Ty&
operator[](size_t index)const{assert(index<mSize);if(mRight>mLeft){return *(mLeft+index);}size_t idx=((mLeft-mData)+index)%CAPACITY;return mData[idx];}void dump(ostream&os)const{size_t count=mSize;const _Ty*e=mData+CAPACITY-1;for(const _Ty*p=
mLeft;count;((p==e)?p=mData:++p),--count){if(count!=mSize){os << ",";}os << *p;}}private:_Ty*mLeft;_Ty*mRight;_Ty mData[CAPACITY];size_t mSize;};template <typename _Ty,size_t CAPACITY>using aqueue=ArrayQueue<_Ty,CAPACITY>;template<typename T,
size_t CAPACITY>ostream&operator<<(ostream&os,const ArrayQueue<T,CAPACITY>&a){os<<"{";a.dump(os);os<<"}";return os;}

constexpr int APQ_SMALL_THRESH = 170;template <typename _Ty,int MAX_SIZE,class _Pr=less<_Ty>,bool SMALL=(sizeof(_Ty)<APQ_SMALL_THRESH)>class ArrayPriorityQueue{public:ArrayPriorityQueue():mSize(0){
#ifndef NDEBUG
mUnsafe=false;
#endif
}ArrayPriorityQueue(const ArrayPriorityQueue&other){copyInit(other);}ArrayPriorityQueue&operator=(const ArrayPriorityQueue&other){return copyInit(other);}ArrayPriorityQueue(ArrayPriorityQueue&&other){moveInit(std::move(other));}
ArrayPriorityQueue&operator=(ArrayPriorityQueue&&other){return moveInit(std::move(other));}private:inline ArrayPriorityQueue&copyInit(const ArrayPriorityQueue&other){_Ty*p=mHeap;const _Ty*op=other.mHeap;for(const _Ty*opEnd=op+other.mSize;op!=
opEnd;++p,++op){*p=*op;}mSize=other.mSize;
#ifndef NDEBUG
mUnsafe=other.mUnsafe;
#endif
return *this;}inline ArrayPriorityQueue&moveInit(ArrayPriorityQueue&&other){_Ty*p=mHeap;const _Ty*op=other.mHeap;for(const _Ty*opEnd=op+other.mSize; op!=opEnd; ++p,++op){*p=std::move(*op);}mSize=other.mSize;
#ifndef NDEBUG
mUnsafe=other.mUnsafe;
#endif
return *this;}public:template<typename... _Valty>void emplace(_Valty&&... args){assert(mSize!=MAX_SIZE);assert(!mUnsafe);MEM2LOC(size_t,mSize);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);_Ty obj(std::forward<_Valty>(args)...);size_t index=mSize;
size_t parent=(index-1)>>1;while(index &&mCompare(mHeap[parent],obj)){mHeap[index]=mHeap[parent];index=parent;parent=(index-1)>>1;}mHeap[index]=obj;++mSize;LOC2MEM(mSize);}void push(const _Ty&val){assert(mSize!=MAX_SIZE);assert(!mUnsafe);
MEM2LOC(size_t,mSize);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);size_t index=mSize;size_t parent=(index-1)>>1;while(index &&mCompare(mHeap[parent],val)){mHeap[index]=mHeap[parent];index=parent;parent=(index-1)>>1;}mHeap[index]=val;++mSize;
LOC2MEM(mSize);}void pop(){assert(!this->empty());assert(!mUnsafe);MEM2LOC(size_t,mSize);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);--mSize;if(1<mSize){size_t index=0;size_t childLeft=1;size_t greaterChildIndex=(childLeft==mSize-1||mCompare(
mHeap[childLeft+1],mHeap[childLeft]))?childLeft:childLeft+1;while(mCompare(mHeap[mSize],mHeap[greaterChildIndex])){mHeap[index]=mHeap[greaterChildIndex];index=greaterChildIndex;childLeft=(index<<1)+1;if(childLeft>=mSize){break;}
greaterChildIndex=(childLeft==mSize-1 || mCompare(mHeap[childLeft+1],mHeap[childLeft]))?childLeft:childLeft+1;}mHeap[index]=mHeap[mSize];}else if(mSize==1){mHeap[0]=mHeap[1];}LOC2MEM(mSize);}const _Ty&top()const{assert(!this->empty());return
mHeap[0];}int capacity()const{return MAX_SIZE;}size_t size()const{return mSize;}bool empty()const{return !mSize;}void clear(){mSize=0;
#ifndef NDEBUG
mUnsafe=false;
#endif
}const _Ty&operator[](size_t index)const{assert(index<mSize);return mHeap[index];}void doAll(function<void(_Ty&,size_t)> func){MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t,mSize);size_t count=0;const _Ty*e=mHeap+MAX_SIZE-1;for(const _Ty*p=mHeap;count!=
mSize;((p==e)?p=mHeap:++p),++count){func(*p,count);}}void doAll(function<void(const _Ty&,size_t)>func)const{MEM2LOC(const _Ty*,mHeap);MEM2LOC(size_t,mSize);size_t count=0;const _Ty*e=mHeap+MAX_SIZE-1;for(const _Ty*p=mHeap;count!=mSize;((p==e)?p
=mHeap:++p),++count){func(*p,count);}}void dump(ostream&os)const{doAll([&os](const _Ty&a,size_t idx){if(idx!=0){os<<",";}os<<a;});}_Ty*_unsafe_add_size(){assert(mSize!=MAX_SIZE);assert(!mUnsafe);++mSize;
#ifndef NDEBUG
mUnsafe=true;
#endif
return &mHeap[mSize-1];}_Ty*_unsafe_put(const _Ty&val){assert(mSize!=MAX_SIZE);assert(!mUnsafe);mHeap[mSize++]=val;
#ifndef NDEBUG
mUnsafe=true;
#endif
return &mHeap[mSize-1];}template<typename... _Valty>_Ty*_unsafe_emplace_put(_Valty&&... args){assert(mSize!=MAX_SIZE);assert(!mUnsafe);::new(&mHeap[mSize++])_Ty(std::forward<_Valty>(args)...);
#ifndef NDEBUG
mUnsafe=true;
#endif
return &mHeap[mSize-1];}void _unsafe_insert(){assert(!this->empty());assert(mUnsafe);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);_Ty val=mHeap[mSize-1];size_t index=mSize-1;size_t parent=(index-1)>>1;while(index &&mCompare(mHeap[parent],val)){
mHeap[index]=mHeap[parent];index=parent;parent=(index-1)>>1;}mHeap[index]=val;
#ifndef NDEBUG
mUnsafe=false;
#endif
}void _unsafe_remove(){assert(!this->empty());assert(mUnsafe);--mSize;
#ifndef NDEBUG
mUnsafe=false;
#endif
}private:size_t mSize;_Ty mHeap[MAX_SIZE];_Pr mCompare;
#ifndef NDEBUG
bool mUnsafe;
#endif
};template<typename _Ty,int MAX_SIZE,class _Pr>class ArrayPriorityQueue<_Ty,MAX_SIZE,_Pr,false>{public:ArrayPriorityQueue():mSize(0),mBlankListSize(0){
#ifndef NDEBUG
mUnsafe=false;
#endif
}ArrayPriorityQueue(const ArrayPriorityQueue&other){copyInit(other);}ArrayPriorityQueue&operator=(const ArrayPriorityQueue&other){return copyInit(other);}ArrayPriorityQueue(ArrayPriorityQueue&&other){moveInit(std::move(other));}
ArrayPriorityQueue&operator=(ArrayPriorityQueue&&other){return moveInit(std::move(other));}private:inline ArrayPriorityQueue&copyInit(const ArrayPriorityQueue&other){_Ty*p=mHeap;size_t*rp=mHeapRef;const size_t*orp=other.mHeapRef;const _Ty*
otherHeap=other.mHeap;size_t count=0;for(const size_t*orpEnd=orp+other.mSize;orp!=orpEnd;++p,++rp,++orp){*p=otherHeap[*orp];*rp=count++;}mSize=other.mSize;mBlankListSize=0;
#ifndef NDEBUG
mUnsafe=other.mUnsafe;
#endif
return *this;}inline ArrayPriorityQueue&moveInit(ArrayPriorityQueue&&other){_Ty*p=mHeap;size_t*rp=mHeapRef;const size_t*orp=other.mHeapRef;const _Ty*otherHeap=other.mHeap;size_t count=0;for(const size_t*orpEnd=orp+other.mSize;orp!=orpEnd;++p,
++rp,++orp){*p=std::move(otherHeap[*orp]);*rp=count++;}mSize=other.mSize;mBlankListSize=0;
#ifndef NDEBUG
mUnsafe=other.mUnsafe;
#endif
return *this;}public:template<typename... _Valty>void emplace(_Valty&&... args){assert(mSize!=MAX_SIZE);assert(!mUnsafe);MEM2LOC(size_t,mSize);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t*,mHeapRef);size_t heapIndex;if(
mBlankListSize!=0){heapIndex=mBlankList[mBlankListSize-1];--mBlankListSize;}else{heapIndex=mSize;}::new(&mHeap[heapIndex]) _Ty(std::forward<_Valty>(args)...);size_t index=mSize;size_t parent=(index-1)>>1;while(index &&mCompare(mHeap[mHeapRef[
parent]],mHeap[heapIndex])){mHeapRef[index]=mHeapRef[parent];index=parent;parent=(index-1)>>1;}mHeapRef[index]=heapIndex;++mSize;LOC2MEM(mSize);}void push(const _Ty&val){assert(mSize!=MAX_SIZE);assert(!mUnsafe);MEM2LOC(size_t,mSize);MEM2LOC(_Pr
,mCompare);MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t*,mHeapRef);size_t heapIndex;if(mBlankListSize!=0){heapIndex=mBlankList[mBlankListSize-1];--mBlankListSize;}else{heapIndex=mSize;}mHeap[heapIndex]=val;size_t index=mSize;size_t parent=(index-1)>>1;
while(index &&mCompare(mHeap[mHeapRef[parent]],val)){mHeapRef[index]=mHeapRef[parent];index=parent;parent=(index-1)>>1;}mHeapRef[index]=heapIndex;++mSize;LOC2MEM(mSize);}void pop(){assert(!this->empty());assert(!mUnsafe);MEM2LOC(size_t,mSize);
MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t*,mHeapRef);mBlankList[mBlankListSize++]=mHeapRef[0];--mSize;if(1<mSize){size_t index=0;size_t childLeft=1;size_t greaterChildIndex=(childLeft==mSize-1||mCompare(mHeap[mHeapRef[childLeft+1
]],mHeap[mHeapRef[childLeft]]))?childLeft:childLeft+1;while(mCompare(mHeap[mHeapRef[mSize]],mHeap[mHeapRef[greaterChildIndex]])){mHeapRef[index]=mHeapRef[greaterChildIndex];index=greaterChildIndex;childLeft=(index<<1)+1;if(childLeft>=mSize){
break;}greaterChildIndex=(childLeft==mSize-1||mCompare(mHeap[mHeapRef[childLeft+1]],mHeap[mHeapRef[childLeft]]))?childLeft:childLeft+1;}mHeapRef[index]=mHeapRef[mSize];}else if(mSize==1){mHeapRef[0]=mHeapRef[1];}LOC2MEM(mSize);}const _Ty&top()
const{assert(!this->empty());return mHeap[mHeapRef[0]];}int capacity()const{return MAX_SIZE;}size_t size()const{return mSize;}bool empty()const{return !mSize;}void clear(){mSize=0;mBlankListSize=0;
#ifndef NDEBUG
mUnsafe=false;
#endif
}const _Ty&operator[](size_t index)const{assert(index<mSize);return mHeap[mHeapRef[index]];}void doAll(function<void(_Ty&,size_t)>func){MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t*,mHeapRef);MEM2LOC(size_t,mSize);size_t count=0;const size_t*e=mHeapRef+
MAX_SIZE-1;for(const size_t*p=mHeapRef; count!=mSize; ((p==e) ? p=mHeapRef : ++p),++count){func(mHeap[*p],count);}}void doAll(function<void(const _Ty&,size_t)>func)const{MEM2LOC(const _Ty*,mHeap);MEM2LOC(const size_t*,mHeapRef);MEM2LOC(size_t,
mSize);size_t count=0;const size_t*e=mHeapRef+MAX_SIZE-1;for(const size_t*p=mHeapRef;count!=mSize;((p==e)?p=mHeapRef:++p),++count){func(mHeap[*p],count);}}void dump(ostream&os)const{doAll([&os](const _Ty&a,size_t idx){if(idx!=0){os<<",";}os<<a;
});}_Ty*_unsafe_add_size(){assert(mSize!=MAX_SIZE);assert(!mUnsafe);size_t heapIndex;if(mBlankListSize!=0){heapIndex=mBlankList[mBlankListSize-1];--mBlankListSize;}else{heapIndex=mSize;}mHeapRef[mSize]=heapIndex;++mSize;
#ifndef NDEBUG
mUnsafe=true;
#endif
return &mHeap[heapIndex];}_Ty*_unsafe_put(const _Ty&val){assert(mSize!=MAX_SIZE);assert(!mUnsafe);_Ty*p=_unsafe_add_size();*p=val;
#ifndef NDEBUG
mUnsafe=true;
#endif
return p;}template<typename... _Valty>_Ty*_unsafe_emplace_put(_Valty&&... args){assert(mSize!=MAX_SIZE);assert(!mUnsafe);_Ty*p=_unsafe_add_size();::new(p) _Ty(std::forward<_Valty>(args)...);
#ifndef NDEBUG
mUnsafe=true;
#endif
return p;}void _unsafe_insert(){assert(!this->empty());assert(mUnsafe);MEM2LOC(size_t,mSize);MEM2LOC(_Pr,mCompare);MEM2LOC(_Ty*,mHeap);MEM2LOC(size_t*,mHeapRef);size_t heapIndex=mHeapRef[mSize-1];size_t index=mSize-1;size_t parent=(index-1)>>1;
while(index &&mCompare(mHeap[mHeapRef[parent]],mHeap[heapIndex])){mHeapRef[index]=mHeapRef[parent];index=parent;parent=(index-1)>>1;}mHeapRef[index]=heapIndex;
#ifndef NDEBUG
mUnsafe=false;
#endif
}void _unsafe_remove(){assert(!this->empty());assert(mUnsafe);--mSize;mBlankList[mBlankListSize++]=mHeapRef[mSize];
#ifndef NDEBUG
mUnsafe=false;
#endif
}private:size_t mSize;_Ty mHeap[MAX_SIZE];size_t mHeapRef[MAX_SIZE];_Pr mCompare;size_t mBlankList[MAX_SIZE];size_t mBlankListSize;
#ifndef NDEBUG
bool mUnsafe;
#endif
};template <typename _Ty,int MAX_SIZE,class _Pr=less<_Ty>,bool SMALL=(sizeof(_Ty)<APQ_SMALL_THRESH)>using apq=ArrayPriorityQueue<_Ty,MAX_SIZE,_Pr,SMALL>;template<typename T,size_t CAPACITY>ostream&operator<<(ostream&os,const ArrayPriorityQueue<
T,CAPACITY>&a){os<<"{";a.dump(os);os<<"}";return os;}


template<typename T,size_t SZ>class CTInitArray{public:CTInitArray()=default;CTInitArray(const T&initVal):mCurrentNum(1),mInitVal(initVal){std::fill(mNums,mNums+SZ,0);}inline void setInitVal(const T&initVal) noexcept{mInitVal=initVal;}inline
void init()noexcept{++mCurrentNum;}inline const T&read(size_t index)const{assert(index<SZ);return(mNums[index]==mCurrentNum)?mData[index]:mInitVal;}inline T&write(size_t index,const T&val){assert(index<SZ);mNums[index]=mCurrentNum;return
mData[index]=val;}constexpr size_t size()const noexcept{return SZ;}private:int mCurrentNum;T mInitVal;T mData[SZ];int mNums[SZ];};template<typename T,size_t SZ1,size_t SZ2>class CTInitArray2D{public:CTInitArray2D()=default;CTInitArray2D(const
T&initVal):mCurrentNum(1),mInitVal(initVal){for(array<int,SZ2>&t1:mNums)std::fill(t1.begin(),t1.end(),0);}inline void setInitVal(const T&initVal){mInitVal=initVal;}inline void init(){++mCurrentNum;}inline const T&read(size_t index1,size_t
index2)const{assert(index1<SZ1);assert(index2<SZ2);return(mNums[index1][index2]==mCurrentNum)?mData[index1][index2]:mInitVal;}inline T&write(size_t index1,size_t index2,const T&val){assert(index1<SZ1);assert(index2<SZ2);mNums[index1][index2]=
mCurrentNum;return mData[index1][index2]=val;}constexpr size_t size1()const{return SZ1;}constexpr size_t size2()const{return SZ2;}private:int mCurrentNum;T mInitVal;array<array<T,SZ2>,SZ1>mData;array<array<int,SZ2>,SZ1>mNums;};template<
typename T,size_t SZ1,size_t SZ2,size_t SZ3>class CTInitArray3D{public:CTInitArray3D()=default;CTInitArray3D(const T&initVal):mCurrentNum(1),mInitVal(initVal){for(array<array<int,SZ3>,SZ2>&t1:mNums)for(array<int,SZ3>&t2:t1)std::fill(t2.begin(),
t2.end(),0);}inline void setInitVal(const T&initVal){mInitVal=initVal;}inline void init(){++mCurrentNum;}inline const T&read(size_t index1,size_t index2,size_t index3)const{assert(index1<SZ1);assert(index2<SZ2);assert(index3<SZ3);return(mNums[
index1][index2][index3]==mCurrentNum)?mData[index1][index2][index3]:mInitVal;}inline T&write(size_t index1,size_t index2,size_t index3,const T&val){assert(index1<SZ1);assert(index2<SZ2);assert(index3<SZ3);mNums[index1][index2][index3]=
mCurrentNum;return mData[index1][index2][index3]=val;}constexpr size_t size1()const{return SZ1;}constexpr size_t size2()const{return SZ2;}constexpr size_t size3()const{return SZ3;}private:int mCurrentNum;T mInitVal;array<array<array<T,SZ3>,SZ2>
,SZ1>mData;array<array<array<int,SZ3>,SZ2>,SZ1>mNums;};template<typename T,size_t SZ>ostream&operator<<(ostream&os,const CTInitArray<T,SZ>&a){os<<"[";for(int i=0;i<SZ;i++){if(i!=0){os<<",";}os<<a.read(i);}os<<"]";return os;}


void debugErrSub(){cerr<<endl;}
template<class Head,class...Tail>void debugErrSub(Head&&head,Tail&&...tail){cerr<<" "<<head;debugErrSub(std::forward<Tail>(tail)...);}
template<class Head,class...Tail>void debugErr(Head&&head,Tail&&...tail){cerr<<head;debugErrSub(std::forward<Tail>(tail)...);}


ll getTime() {
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto epoch = now_ms.time_since_epoch();
	return epoch.count();
}

#ifdef CLOCK_MONOTONIC_RAW
using TimeType = timespec;
#else
using TimeType = system_clock::time_point;
#endif

class SimpleTimer {
public:

#ifdef CLOCK_MONOTONIC_RAW
	static timespec getTime() {
		timespec ts;
		clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
		return ts;
	}
	static ll getDurationNano(const timespec& end, const timespec& start) {
		return diff(start, end);
	}
	static ll getDurationMilli(const timespec& end, const timespec& start) {
		return diff(start, end) / 1000000;
	}
	static ll diff(const timespec& start, const timespec& end)	{
		ll d = 0ll;
		if ((end.tv_nsec-start.tv_nsec)<0) {
			d += (end.tv_sec-start.tv_sec-1)*1000000000;
			d += 1000000000+end.tv_nsec-start.tv_nsec;
		} else {
			d += (end.tv_sec-start.tv_sec)*1000000000;
			d += end.tv_nsec-start.tv_nsec;
		}
		return d;
	}
#else
	static TimeType getTime() { return system_clock::now(); }
	static ll getDurationNano(const TimeType& end, const TimeType& start) {
		return duration_cast<nanoseconds>(end - start).count();
	}
	static ll getDurationMilli(const TimeType& end, const TimeType& start) {
		return duration_cast<milliseconds>(end - start).count();
	}
#endif
};

class Point {
public:

	constexpr Point():x(-1),y(-1){}
	constexpr inline Point(int x,int y):x(x),y(y){}

	// x,yそれぞれの距離
	static Point distance(const Point&p1,const Point&p2) {
		return Point(abs(p1.x-p2.x),abs(p1.y-p2.y));
	}
	// マンハッタン距離
	static int manhattanDist(const Point&p1,const Point&p2) {
		return abs(p1.x-p2.x)+abs(p1.y-p2.y);
	}
	// ユークリッド距離の２乗
	static int euclidDist2(const Point&p1,const Point&p2) {
		return(p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y);
	}
	// ユークリッド距離
	static double euclidDist(const Point&p1,const Point&p2) {
		return sqrt((p1.x-p2.x)*(p1.x-p2.x)+(p1.y-p2.y)*(p1.y-p2.y));
	}

	void set(int x,int y){this->x=x;this->y=y;}
	void set(const Point&p){this->x=p.x;this->y=p.y;}

	Point operator+(int val)const{return Point(x+val,y+val);}
	void operator+=(int val){x+=val;y+=val;}
	Point operator-(int val)const{return Point(x-val,y-val);}
	void operator-=(int val){x-=val;y-=val;}
	Point operator*(int val)const{return Point(x*val,y*val);}
	void operator*=(int val){x*=val;y*=val;}
	Point operator/(int val)const{return Point(x/val,y/val);}
	void operator/=(int val){x/=val;y/=val;}

	Point operator+(const Point&p)const{return Point(x + p.x, y + p.y);}
	Point operator-(const Point&p)const{return Point(x - p.x, y - p.y);}
	void operator+=(const Point&p){x+=p.x;y+=p.y;}
	void operator-=(const Point&p){x-=p.x;y-=p.y;}
	bool operator==(const Point&p)const{return x==p.x&&y==p.y;}
	bool operator!=(const Point&p)const{return x!=p.x||y!=p.y;}
	//bool operator<(const Point&p)const{return x*x+y*y<p.x*p.x+p.y*p.y;}
	const Point&getPos()const{return*this;}
	int x,y;
};

constexpr Point DP4[]={Point(DX4[0],DY4[0]),Point(DX4[1],DY4[1]),Point(DX4[2],DY4[2]),Point(DX4[3],DY4[3])};
constexpr Point DP5[]={Point(DX5[0],DY5[0]),Point(DX5[1],DY5[1]),Point(DX5[2],DY5[2]),Point(DX5[3],DY5[3]),Point(DX5[4],DY5[4])};
constexpr Point DP8[]={Point(DX8[0],DY8[0]),Point(DX8[1],DY8[1]),Point(DX8[2],DY8[2]),Point(DX8[3],DY8[3]),Point(DX8[4],DY8[4]),Point(DX8[5],DY8[5]),Point(DX8[6],DY8[6]),Point(DX8[7],DY8[7])};
constexpr Point DP9[]={Point(DX9[0],DY9[0]),Point(DX9[1],DY9[1]),Point(DX9[2],DY9[2]),Point(DX9[3],DY9[3]),Point(DX9[4],DY9[4]),Point(DX9[5],DY9[5]),Point(DX9[6],DY9[6]),Point(DX9[7],DY9[7]),Point(DX9[8],DY9[8])};


ostream&operator<<(ostream&os, const Point&p){os<<"{"<<p.x<<","<<p.y<<"}";return os;}

typedef pair<int, int> pii;
typedef pair<int, Point> pip;

constexpr int MAX_TURN = 200;
constexpr ll THINK_TIME = 1800ll * 1000000ll; // nano sec

int gEvalNum = 0;
int gEvalNumEnd = 0;
#if defined(INPUT_EVAL_NUM)
int gEvalNumInput = 0;
#endif

array3<bool, 20, 20, 4> gWalls;
int SY, SX, TY, TX;
Point START, GOAL;
double PROB;

array4<int, H, W, H, W> DISTS;

bool isOverScreen(const Point& p) {
	return p.x < 0 || p.y < 0 || p.x >= W || p.y >= H;
}

int getDir(const Point& from, const Point& to) {
	if (to.y > from.y) {
		return 2;
	} else if (to.y < from.y) {
		return 0;
	} else if (to.x > from.x) {
		return 3;
	} else if (to.x < from.x) {
		return 1;
	}
	return 4;
}

string convToULDR(const vector<int>& path) {
	string s = "";
	for (int dir : path) {
		if (dir >= 0 && dir < 4) {
			s += "ULDR"[dir];
		}
	}
	return s;
}

bool canMove(const Point& from, int dir) {
	return !gWalls[from.y][from.x][dir];
}

int bfsDist(const Point& from, const Point& to) {

	static array2<bool, H, W> visit;
	fill(visit, false);

	static aqueue<pip, WH> q;
	q.clear();

	q.push(pip(0, from));
	visit[from.y][from.x] = true;

	while (!q.empty()) {
		pip pr = q.front();
		q.pop();
		int dist = pr.first;
		Point pos = pr.second;

		if (pos == to) {
			return dist;
		}

		for (int dir = 0; dir < 4; ++dir) {
			if (!canMove(pos, dir)) {
				continue;
			}
			const Point np = pos + DP4[dir];
			if (visit[np.y][np.x]) {
				continue;
			}
			visit[np.y][np.x] = true;

			q.push(pip(dist + 1, np));
		}
	}

	return -1;
}

pair<int, vector<int>> bfsDistPath(const Point& from, const Point& to) {

	if (from == to) {
		return make_pair(0, vector<int>());
	}

	static array2<bool, H, W> visit;
	fill(visit, false);

	static array2<Point, H, W> prevs;

	static aqueue<pip, WH> q;
	q.clear();

	q.push(pip(0, from));
	visit[from.y][from.x] = true;

	int ansDist = -1;
	while (!q.empty()) {
		pip pr = q.front();
		q.pop();
		int dist = pr.first;
		Point pos = pr.second;

		if (pos == to) {
			ansDist = dist;
			break;
		}

		for (int dir = 0; dir < 4; ++dir) {
			if (!canMove(pos, dir)) {
				continue;
			}
			const Point np = pos + DP4[dir];
			if (visit[np.y][np.x]) {
				continue;
			}
			visit[np.y][np.x] = true;

			q.push(pip(dist + 1, np));
			prevs[np.y][np.x] = pos;
		}
	}

	if (ansDist == -1) {
		return make_pair(-1, vector<int>());
	}

	vector<int> path;
	Point cur = to;
	while (cur != from) {
		int dir = getDir(prevs[cur.y][cur.x], cur);
		path.push_back(dir);
		cur = prevs[cur.y][cur.x];
	}
	std::reverse(ALL(path));

	return std::move(make_pair(ansDist, std::move(path)));
}

void input() {
	cin >> SY >> SX >> TY >> TX >> PROB; cin.ignore();
	START = Point(SX, SY);
	GOAL = Point(TX, TY);
	for (int y = 0; y < H; ++y) {
		fill(gWalls[y], false);
	}
	for (int y = 0; y < H; ++y) {
		string s;
		getline(cin, s);
		for (int x = 0; x < W - 1; ++x) {
			if (s[x] == '1') {
				gWalls[y][x][3] = true;
				gWalls[y][x + 1][1] = true;
			}
		}
	}
	for (int y = 0; y < H - 1; ++y) {
		string s;
		getline(cin, s);
		for (int x = 0; x < W; ++x) {
			if (s[x] == '1') {
				gWalls[y][x][2] = true;
				gWalls[y + 1][x][0] = true;
			}
		}
	}
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			for (int dir = 0; dir < 4; ++dir) {
				Point np = Point(x, y) + DP4[dir];
				if (isOverScreen(np)) {
					gWalls[y][x][dir] = true;
				}
			}
		}
	}
}

vector<int> makeInitPath1() {

	vector<int> initPath;
	for (int i = 0; i < 6; ++i) {
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(0);

		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(1);

		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(0);
		initPath.push_back(0);

		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(3);
		initPath.push_back(2);
		initPath.push_back(1);
		initPath.push_back(1);
	}

	auto pr = bfsDistPath(Point(W - 1, H - 1), GOAL);
	for (int dir : pr.second) {
		initPath.push_back(dir);
	}

	while (initPath.size() < MAX_TURN) {
		initPath.push_back(xor128() % 4);
	}
	
	initPath.resize(MAX_TURN);
	return initPath;
}

double calcEval(int turn, array2<double, H, W>& probs) {
	double eval = 0.0;
	double maxProb = 0.0;
	Point maxPos;
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			eval += sqrt(DISTS[y][x][TY][TX]) * probs[y][x];
			if (x == TX && y == TY) {
				eval -= 1.0 * probs[y][x];
			}
			if (probs[y][x] > maxProb) {
				maxProb = probs[y][x];
				maxPos = Point(x, y);
			}
		}
	}
	//eval = DISTS[maxPos.y][maxPos.x][TY][TX] * 1.0;
	return -eval;
}

vector<int> makeInitPath2() {

	vector<int> ret;

	array2<double, H, W> probs;
	fill(probs, 0.0);

	probs[SY][SX] = 1.0;

	for (int turn = 0; turn < MAX_TURN; ++turn) {
		//if (turn == 100) {
		//	cerr <<"a";
		//}
		int bestDir = -1;
		double bestEval = -99999999;
		array2<double, H, W> bestProbs;
		for (int dir = 0; dir < 4; ++dir) {
			array2<double, H, W> nextProbs;
			fill(nextProbs, 0.0);
			for (int y = 0; y < H; ++y) {
				for (int x = 0; x < W; ++x) {
					if (probs[y][x] <= 1e-100) {
						continue;
					}
					if (x == TX && y == TY) {
						nextProbs[y][x] += probs[y][x];
						continue;
					}
					if (canMove(Point(x, y), dir)) {
						Point np = Point(x, y) + DP4[dir];
						nextProbs[np.y][np.x] += probs[y][x] * (1.0 - PROB);
						nextProbs[y][x] += probs[y][x] * PROB;
					} else {
						nextProbs[y][x] += probs[y][x];
					}
				}
			}
			double eval = calcEval(turn, nextProbs);
			if (eval > bestEval) {
				bestEval = eval;
				bestDir = dir;
				bestProbs = nextProbs;
			}
		}
		ret.push_back(bestDir);
		probs = bestProbs;
	}
	
	return ret;
}

typedef vector<int> Solution;
typedef double Score;

Score simulate(Solution& solution) {
	static array<double, WH> probs;
	fill(probs, 0.0);
	probs[SY * W + SX] = 1.0;

	double score = 0.0;

	for (int turn = 0; turn < MAX_TURN; ++turn) {
		if ((int)solution.size() < turn) {
			solution.push_back(xor128() % 4);
		}
		int dir = solution[turn];

		static array<double, WH> nextProbs;
		fill(nextProbs, 0.0);
		for (int y = 0; y < H; ++y) {
			for (int x = 0; x < W; ++x) {
				if (x == TX && y == TY) {
					nextProbs[y * W + x] += probs[y * W + x];
					continue;
				}
				//if (canMove(Point(x, y), dir)) {
				if (!gWalls[y][x][dir]) {
					int nx = x + DX4[dir];
					int ny = y + DY4[dir];
					nextProbs[ny * W + nx] += probs[y * W + x] * (1.0 - PROB);
					nextProbs[y * W + x] += probs[y * W + x] * PROB;
					if (nx == TX && ny == TY) {
						score += (401 - turn) * probs[y * W + x];
					}
				} else {
					nextProbs[y * W + x] += probs[y * W + x];
				}
			}
		}
		probs = nextProbs;
	}

	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			//if (PROB > 0.3) {
			//	score -= 50 * DISTS[y][x][TY][TX] * probs[y * W + x];
			//} else {
				score -= sqrt(DISTS[y][x][TY][TX]) * probs[y * W + x];
			//}
		}
	}

	return score;
}

pair<Score, Solution> doSA(const Solution& initSolution, const ll msec) {

	const ll m_startTime = getTime(); // 開始時刻

	Solution curSolution = initSolution;
	Score simResult = simulate(curSolution);
	Solution bestSolution = curSolution;        // ベストの状態
	Score bestScore = simResult;      // ベストスコア。getScoreは、状態からスコアを求める関数
	Score currentScore = bestScore;    // 現在のスコア

	//cerr << "InitScore:" << bestScore << endl;

	const ll saTimeStart = getTime();            // 焼きなまし開始時刻。getTimeは、時間を返す関数
	const ll saTimeEnd = m_startTime + msec;     // 焼きなまし終了時刻。m_startTimeはこのプログラム自体の開始時間
	ll saTimeCurrent = saTimeStart;          // 現在の時刻

#if DEBUG_TIME == 1
	int LOOP_NUM = 1000;
	for (int cnt = 0; cnt < LOOP_NUM; ++cnt) {
		saTimeCurrent = saTimeStart + (ll)cnt * msec / (ll)LOOP_NUM;
#elif DEBUG_TIME == 2
	while ((saTimeCurrent = getTime()) || true) { // 無限ループ
#elif defined(INPUT_EVAL_NUM)
	while (gEvalNum < gEvalNumInput) { // 入力で受け取った回数までループ
#else
	while ((saTimeCurrent = getTime()) < saTimeEnd) { // 焼きなまし終了時刻までループ
#endif
		const ll T = saTimeEnd - saTimeStart;         // 焼きなましにかける時間
		const ll t = saTimeCurrent - saTimeStart;     // 焼きなまし法開始からの時間
		Solution tempSolution = curSolution;

		int CHANGE_NUM_MAX = 3;

		//int changeNum = (xor128() % CHANGE_NUM_MAX) + 1;
		int changeNum = (int)(CHANGE_NUM_MAX * (double)t / T);
		changeNum = CHANGE_NUM_MAX - changeNum;
		//cerr << changeNum << endl;
		int changeType, idx1, idx2, v1;
		changeType = xor128() % 5;
		if (changeType == 0) {
			// insert
			if (xor128() % 10 <= 7) {
				idx1 = xor128() % MAX_TURN;
				v1 = xor128() % 4;
				for (int i = 0; i < changeNum; ++i) {
					tempSolution.insert(tempSolution.begin() + idx1, v1);
				}
				tempSolution.resize(min((int)tempSolution.size(), MAX_TURN));
			} else {
				idx1 = xor128() % MAX_TURN;
				for (int i = 0; i < changeNum; ++i) {
					v1 = xor128() % 4;
					tempSolution.insert(tempSolution.begin() + idx1, v1);
				}
				tempSolution.resize(min((int)tempSolution.size(), MAX_TURN));
			}
		} else if (changeType == 1) {
			// erase
			idx1 = xor128() % MAX_TURN;
			for (int i = 0; i < changeNum; ++i) {
				tempSolution.erase(tempSolution.begin() + idx1);
				while (tempSolution.size() < MAX_TURN) {
					tempSolution.push_back(xor128() % 4);
				}
			}
		} else if (changeType == 2) {
			// swap
			idx1 = xor128() % MAX_TURN;
			idx2 = xor128() % (MAX_TURN - 1);
			if (idx1 == idx2) {
				idx2 = MAX_TURN - 1;
			}
			std::swap(tempSolution[idx1], tempSolution[idx2]);
		} else if (changeType == 3) {
			// move
			idx1 = xor128() % MAX_TURN;
			idx2 = xor128() % (MAX_TURN - 1);
			if (idx1 == idx2) {
				idx2 = MAX_TURN - 1;
			}
			for (int i = 0; i < changeNum; ++i) {
				int v = tempSolution[idx1];
				tempSolution.erase(tempSolution.begin() + idx1);
				tempSolution.insert(tempSolution.begin() + idx2, v);
			}
			//tempSolution.resize(min((int)tempSolution.size(), MAX_TURN));
		//} else if (changeType == 4) {
		//	// pattern insert
		//	idx1 = xor128() % MAX_TURN;
		//	if (xor128() % 2 == 0) {
		//		int a = 0, b = 3, c = 2;
		//		tempSolution.insert(tempSolution.begin() + idx1, c);
		//		tempSolution.insert(tempSolution.begin() + idx1, b);
		//		tempSolution.insert(tempSolution.begin() + idx1, a);
		//	} else {
		//		int a = 1, b = 2, c = 3;
		//		tempSolution.insert(tempSolution.begin() + idx1, c);
		//		tempSolution.insert(tempSolution.begin() + idx1, b);
		//		tempSolution.insert(tempSolution.begin() + idx1, a);
		//	}
		//	tempSolution.resize(min((int)tempSolution.size(), MAX_TURN));
		} else if (changeType == 4) {
			// change
			if (xor128() % 10 <= 7) {
				idx1 = xor128() % MAX_TURN;
				v1 = xor128() % 4;
				for (int i = 0; i < changeNum; ++i) {
					tempSolution[(idx1 + i) % MAX_TURN] = v1;
				}
			} else {
				idx1 = xor128() % MAX_TURN;
				for (int i = 0; i < changeNum; ++i) {
					v1 = xor128() % 4;
					tempSolution[(idx1 + i) % MAX_TURN] = v1;
				}
			}
		}

		// 次の状態
		simResult = simulate(tempSolution);
		const Score nextScore = simResult;

		// 最初t=0のときは、スコアが良くなろうが悪くなろうが、常にnextを使用
		// 最後t=Tのときは、スコアが改善したときのみ、nextを使用
		const ll R = 10000;
		// スコアが悪くなったときでも、次の状態に移動する遷移する場合はtrue。１次関数を使用。
		//const bool FORCE_NEXT = R * (T - t) > T*(ll)(xor128() % R);

		constexpr double START_TEMP = 3;
		constexpr double END_TEMP = 0.01;
		//constexpr double START_TEMP = 1;
		//constexpr double END_TEMP = 0.0001;
		double temp = START_TEMP + (END_TEMP - START_TEMP) * t / T;
		//temp += (START_TEMP - END_TEMP) * 0.1 * sin(100 * 2 * M_PI * (double)t/T);
		double probability = exp((nextScore - currentScore) / temp);  // 最大化なら分子はnextScore - currentScore
		bool FORCE_NEXT = probability > (double)(xor128() % R) / R;
		//FORCE_NEXT = false;

		// スコアが良くなった or 悪くなっても強制遷移
		if (nextScore >= currentScore || FORCE_NEXT)
		{
			currentScore = nextScore;
			curSolution = tempSolution;
			//fprintf(stderr, "current Score=%.8f time=%lld\n", currentScore, t);
		}

		// ベストスコアは保存しておく。
		if (currentScore > bestScore)
		{
			bestScore = currentScore;
			bestSolution = curSolution;
			//fprintf(stderr, "New best Score=%f time=%lld\n", bestScore, t);
		}

		gEvalNum++;
	}

#ifdef OUTPUT_EVAL_NUM
	cerr << "EvalNum:" << gEvalNum << endl;
#endif

	return pair<Score, Solution>(bestScore, bestSolution);
}

Solution think(const Solution& initSolution, ll thinkTime) {

	Solution solution = initSolution;

	solution = doSA(solution, thinkTime).second;

	return solution;
}

void solve1() {
	//auto pr = bfsDistPath(START, GOAL);
	//int dist = pr.first;
	//vector<int> initPath = std::move(pr.second);

	//while (initPath.size() < MAX_TURN) {
	//	initPath.push_back(xor128() % 4);
	//}


	//vector<int> initPath = makeInitPath1();
	vector<int> initPath = makeInitPath2();

	//Score bestScore = -99999999;
	//vector<int> bestPath;
	//vector<int> path = initPath;
	//constexpr ll LOOP_NUM = 1;
	//for (int i = 0; i < LOOP_NUM; ++i) {
	//	path = think(path, THINK_TIME / LOOP_NUM);
	//	Score score = simulate(path);
	//	if (score > bestScore) {
	//		bestScore = score;
	//		bestPath = path;
	//	}
	//}

	//string s = convToULDR(bestPath);

	vector<int> path = think(initPath, THINK_TIME);
	string s = convToULDR(path);
	cout << s << endl;
}

void calcAllDists() {
	for (int y = 0; y < H; ++y) {
		for (int x = 0; x < W; ++x) {
			static array2<bool, H, W> visit;
			fill(visit, false);

			static aqueue<pip, WH> q;
			q.clear();

			Point from(x, y);
			q.push(pip(0, from));
			visit[from.y][from.x] = true;

			while (!q.empty()) {
				pip pr = q.front();
				q.pop();
				int dist = pr.first;
				Point pos = pr.second;

				DISTS[y][x][pos.y][pos.x] = dist;

				for (int dir = 0; dir < 4; ++dir) {
					if (!canMove(pos, dir)) {
						continue;
					}
					const Point np = pos + DP4[dir];
					if (visit[np.y][np.x]) {
						continue;
					}
					visit[np.y][np.x] = true;

					q.push(pip(dist + 1, np));
				}
			}
		}
	}
}

void solve() {
	std::ios::sync_with_stdio(false);

#ifdef __linux
	//setStackSize();
#endif

#ifdef INPUT_FROM_FILE
	ifstream ifs("example_01.txt");
	cin.rdbuf(ifs.rdbuf());
#endif
#ifdef OUTPUT_TO_FILE
	ofstream ofs("output.txt");
	cout.rdbuf(ofs.rdbuf());
#endif
#ifdef CERR_TO_FILE
	ofstream ofserr("debug.txt");
	cerr.rdbuf(ofserr.rdbuf());
#endif

	//TimeType startTime0 = SimpleTimer::getTime();

	input();
	calcAllDists();
	solve1();

#if defined(OUTPUT_EVAL_NUM)
	cerr << "EvalNum:" << gEvalNum << ", EvalNumEnd:" << gEvalNumEnd << endl;
#elif defined(OUTPUT_SINGLE_EVAL_NUM)
	cerr << gEvalNum << endl;
#endif

	getchar();

}

int main() {
	solve();
}
