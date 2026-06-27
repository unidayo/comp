from typing import List
import sys
from math import erf,sqrt
from random import randint
import time
def norm(x):
    'Cumulative distribution function for the standard normal distribution'
    return (1.0 + erf(x / sqrt(2.0))) / 2.0

class Pos:
    def __init__(self, y: int, x: int):
        self.y = y
        self.x = x


class Judge:
    def set_temperature(self, temperature: List[List[int]]) -> None:
        for row in temperature:
            print(" ".join(map(str, row)))
        sys.stdout.flush()

    def measure(self, i: int, y: int, x: int) -> int:
        print(f"{i} {y} {x}", flush=True)
        v = int(input())
        if v == -1:
            print(f"something went wrong. i={i} y={y} x={x}", file=sys.stderr)
            sys.exit(1)
        return v

    def answer(self, estimate: List[int]) -> None:
        print("-1 -1 -1")
        for e in estimate:
            print(e)
        sys.stdout.flush()


class Solver:

    def __init__(self, L: int, N: int, S: int, landing_pos: List[Pos]):
        self.start=time.time()
        self.L = L
        self.N = N
        self.S = S
        self.used=[1]*N
        self.measure_count=0
        self.landing_pos = landing_pos
        self.judge = Judge()
        self.probability_table = self.create_probability_table(S)
        self.cumalative_table = self.create_cumalative_table(S)

        self.s_to_weight={
            1:2,
            4:3,
            9:3,
            16:4,
            25:4,
            36:5,
            49:6,
            64:7,
            81:8,
            100:9,
            121:10,
        }
        for i in range(12,31):
            self.s_to_weight[i*i]=i
        print('#',self.probability_table)
        print('#',self.cumalative_table)
        print('#',sum(self.probability_table))
    def create_cumalative_table(self, S: int)->List[float]:
        prob_table=[]
        for i in range(1001):
            prob_table.append(1-norm((i-0.5)/S))
        return prob_table
    
    def create_probability_table(self, S: int)->List[float]:
        prob_table=[]
        for i in range(1001):
            prob_table.append(norm((i+0.5)/S)-norm((i-0.5)/S))
        return prob_table

    def solve(self) -> None:
        temperature = self._create_temperature()
        self.judge.set_temperature(temperature)
        self.find_best_differences(temperature)
        estimate = self._predict(temperature)
        print('#',time.time()-self.start)
        self.judge.answer(estimate)


    def _create_temperature(self) -> List[List[int]]:
        temperature = [[0] * self.L for _ in range(self.L)]
        cost0=min(800+self.S,1000)
        cost1=min(650+self.S//2,1000)
        cost2=max(0,350-self.S//2)
        cost3=max(200-self.S,0)
        if self.S==36:
            cost1=620
            cost2=380
        elif self.S==49:
            cost1=640
            cost2=360
        elif self.S==64:
            cost1=650
            cost2=350
        elif self.S==81:
            cost1=670
            cost2=350
        elif self.S==100:
            cost1=690
            cost2=310
        elif self.S==25:
            cost1=600
            cost2=400
        elif self.S==16:
            cost1=580
            cost2=420
        # set the temperature to i * 10 for i-th position
        super_hot_set=set()
        hot_set=set()
        super_cold_set=set()
        cold_set=set()
        extra_connect=False
        extra_disconnect=False
        if self.L<=20 and self.L*19<=self.S:
            extra_connect=True
        elif self.L>=30 and self.S<=350:
            extra_disconnect=True
        for y in range(self.L):
            for x in range(self.L):
                if (self.L-1>y>self.L//2 or (self.S>36 and self.L-1==y)) and x>=(3*self.L)//4:
                    temperature[y][x]=cost1
                    hot_set.add((y,x))
                elif y<self.L//2 and x<=self.L//4:
                    if extra_disconnect and (y==self.L//2-1 and x==0 or y==0 and x==0):
                        continue
                    temperature[y][x]=cost2
                    cold_set.add((y,x))
                if extra_connect and (x==0 and y==self.L//2 or x==self.L-1 and y==self.L//2-1):
                    temperature[y][x]=cost2
                    cold_set.add((y,x))
        if self.S<=9:
            hot_set=set()
            cold_set=set()
            a_set=[]
            for i_out, pos in enumerate(self.landing_pos):
                a_set.append((min(pos.y,self.L-1-pos.y)+min(pos.x,self.L-1-pos.x),i_out,pos))
            for y in range(self.L):
                for x in range(self.L):
                    temperature[y][x]=0
            a_set.sort()
            count=0
            if self.S==1:
                k_val=1
            elif self.S==4:
                k_val=2
            elif self.S==9:
                k_val=3
            elif self.S==16:
                k_val=4
            else:
                k_val=4
            for _,i_out,pos in a_set:
                count+=1
                temperature[pos.y][pos.x]=300+k_val*count
                hot_set.add((pos.y,pos.x))
        else:
            if self.S<=121 and self.N+self.L>=115 and self.L>=24:
                super_cold_set.add((self.L//4,self.L//4))
                temperature[self.L//4][self.L//4]=cost3
                super_hot_set.add(((3*self.L)//4,(3*self.L)//4))
                temperature[(3*self.L)//4][(3*self.L)//4]=cost0
                
        while True:
            change_flag=False
            for y in range(self.L):
                for x in range(self.L):
                    if (y,x) in super_hot_set or (y,x) in super_cold_set or self.S<=9 and (y,x) in hot_set:
                        continue
                    new_temp=((temperature[(y-1)%self.L][x]+temperature[(y+1)%self.L][x]+temperature[y][(x-1)%self.L]+temperature[y][(x+1)%self.L])+2)//4
                    if self.S>=16:
                        if (y,x) in hot_set:
                            new_temp=max(new_temp,cost1)
                        if (y,x) in cold_set:
                            new_temp=min(new_temp,cost2)
                    if new_temp!=temperature[y][x]:
                        change_flag=True
                        temperature[y][x]=new_temp
            if change_flag==False:
                break
        best=10**18
        if self.S<16:
            return temperature
        
        for y in range(self.L):
            for x in range(self.L):
                tmp=0
                
                for i_out, pos in enumerate(self.landing_pos):
                    ypos=(pos.y-y)%self.L
                    xpos=(pos.x-x)%self.L
                    tmp+=min(ypos,self.L-ypos)+min(xpos,self.L-xpos)
                if tmp<best:
                    best=tmp
                    besty=y
                    bestx=x
        y=besty
        x=bestx
        temperature2=[[0]*self.L for _ in range(self.L)]
        for y2 in range(self.L):
            for x2 in range(self.L):
                temperature2[(y2+y)%self.L][(x2+x)%self.L]=temperature[y2][x2]

        return temperature2
    def find_best_differences(self, temperature: List[List[int]]):
        self.best_differences=[[[]] * self.N for _ in range(self.N)]
        sum_maxdiff=0
        for i_out1, pos1 in enumerate(self.landing_pos):
            for i_out2, pos2 in enumerate(self.landing_pos):
                
                if i_out1>=i_out2:
                    continue
                al=[]
                max_diff=0
                for x in range(self.L):
                    for y in range(self.L):
                        diff_val=abs(temperature[(pos1.y+y)%self.L][(pos1.x+x)%self.L]-temperature[(pos2.y+y)%self.L][(pos2.x+x)%self.L])
                        if diff_val>=5*self.S:
                            diff_val=10**8
                        tmp=(diff_val**1.5)/(10+min(x,self.L-x)+min(y,self.L-y))
                        if tmp>max_diff:
                            max_diff=tmp
                        if tmp>max_diff/1.2:
                            al.append((tmp,x,y))
                sum_maxdiff+=max_diff
                self.best_differences[i_out2][i_out1]=[]
                self.best_differences[i_out1][i_out2]=[]
                for val,xdiff,ydiff in al:
                    if val>max_diff/1.2:
                        self.best_differences[i_out1][i_out2].append(Pos(ydiff,xdiff))
                        self.best_differences[i_out2][i_out1].append(Pos(ydiff,xdiff))
        return sum_maxdiff
    def _predict(self, temperature: List[List[int]]) -> List[int]:
        estimate = [-1] * self.N
        probability_grid=[[1/self.N]*self.N for _ in range(self.N)]
        count=0
        preciseness=0.003
        prec2=5
        if self.S>=400:
            preciseness=0.002
            prec2=7
        while True:
            if time.time()-self.start>3.7:
                break
            breakflag=True
            override_in=-1
            if self.S>144:
                tmp=0
                for i in range(self.N):
                    largest=0
                    second_largest=0
                    for j in probability_grid[i]:
                        if j > largest:
                            largest, second_largest = j, largest
                        elif j > second_largest:
                            second_largest = j
                    if largest<=0.9:
                        breakflag=False
                    if second_largest>tmp:
                        tmp=second_largest
                        i_in=i
            else:
                tmp=1
                for i in range(self.N):
                    
                    
                    tmp2=max(probability_grid[i])
                    if tmp2<=0.9:
                        breakflag=False
                    if tmp2<tmp:
                        tmp=tmp2
                        i_in=i
            if breakflag==True:
                tmp=1
                min_val=0
                estimates=[0]*self.N
                for i in range(self.N):
                    tmp2=max(probability_grid[i])
                    for j in range(self.N):
                        if tmp2==probability_grid[i][j]:
                            estimates[i]=j
                            break
                for i in range(self.N):
                    for k in range(self.N):
                        if i==k:
                            continue
                        j=estimates[k]
                        val=probability_grid[i][j]
                        if val>preciseness and probability_grid[k][estimates[i]]> max((1-probability_grid[k][j])/prec2,preciseness):
                            breakflag=False
                            
                            if val>min_val:
                                min_val=val
                                override_in=i
                            break
            print('#',tmp)
            if breakflag:
                break
            if override_in!=-1:
                i_in=override_in
            
            prob_of_sequence=probability_grid[i_in]
            klist=[]
            for i_out, pos in enumerate(self.landing_pos):
                klist.append((prob_of_sequence[i_out],i_out))
            klist.sort(reverse=True)
            print('# Confidence',klist[0][0], klist[0][1])
            print('# Confidence',klist[1][0], klist[1][1])
            votes=[[0]*self.L for _ in range(self.L)]
            out1=klist[0][1]
            for (prob2,out2) in klist:
                if prob2==0:
                    continue
                if out1==out2:
                    continue

                for pos in self.best_differences[out1][out2]:
                    votes[pos.y][pos.x]+=prob2
            max_votes=0
            for y in range(self.L):
                for x in range(self.L):
                    if votes[y][x]>max_votes:
                        max_votes=votes[y][x]
                        ydiff=y
                        xdiff=x
                
                    
            if ydiff>self.L/2:
                ydiff-=self.L
            if xdiff>self.L/2:
                xdiff-=self.L
            if (self.S<=100 or self.L<=20) and count<self.N:
                count+=1

                xdiff=0
                ydiff=0
            print(f"# measure i={i_in} y={ydiff} x={xdiff}")
            measured_value = self.judge.measure(i_in, ydiff, xdiff)
            self.measure_count+=1
            # answer the position with the temperature closest to the measured value
            for i_out, pos in enumerate(self.landing_pos):
                diff = abs(temperature[(pos.y+ydiff)%self.L][(pos.x+xdiff)%self.L] - measured_value)
                if measured_value==0 or measured_value==1000:
                    prob_of_sequence[i_out]*=self.cumalative_table[diff]
                    # prob_of_sequence[i_out]*=self.probability_table[diff]
                else:
                    prob_of_sequence[i_out]*=self.probability_table[diff]
            sum_prob=sum(prob_of_sequence)
            # print('#',sum_prob)
            for i_out in range(self.N):
                prob_of_sequence[i_out]/=sum_prob
            for i_out in range(self.N):
                tmp1=0
                for i_in2 in range(self.N):
                    if i_in2==i_in:
                        continue
                    tmp1+=probability_grid[i_in2][i_out]
                if tmp1==0:
                    continue
                for i_in2 in range(self.N):
                    if i_in2==i_in:
                        continue
                    probability_grid[i_in2][i_out]*=(1-prob_of_sequence[i_out])/tmp1
            for i_in2 in range(self.N):
                
                sum_prob=sum(probability_grid[i_in2])
                # print('#',sum_prob)
                for i_out in range(self.N):
                    probability_grid[i_in2][i_out]/=sum_prob
                # print('#',probability_grid[i_in2])
            # print('#',prob_of_sequence)
        for i_in in range(self.N):
            k=max(probability_grid[i_in])
            for i_out in range(self.N):
                if probability_grid[i_in][i_out]==k:
                    # print('#',prob_of_sequence)
                    estimate[i_in] = i_out
                    self.used[i_out]=0
                    done=True
                    break
            
        return estimate


def main():
    L, N, S = [int(v) for v in input().split(" ")]
    landing_pos = []
    for _ in range(N):
        y, x = (int(v) for v in input().split(" "))
        landing_pos.append(Pos(y, x))

    solver = Solver(L, N, S, landing_pos)
    solver.solve()


if __name__ == "__main__":
    main()
