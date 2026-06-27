import random,time
start=time.time()
n,m=map(int,input().split())
origwords=[]
bestgrid=[]

bestgrid=['.']*(20*20)
bestnumwordsfound=0
for i in range(m):
  s=input()
  origwords.append((s,len(s)))
origwords.sort(key=lambda thing: thing[1]) 
origwords.reverse()

confidparam=4

words2=[]
words=[]
for word,length in origwords:
  flag=0
  for bigword in words2:
    compword=bigword[0]
    smolflag=0
    for i in range(len(compword)-length+1):
      smolsmolflag=0
      for j in range(length):
        if compword[i+j]!=word[j]:
          smolsmolflag=1
          break
      if smolsmolflag==0:
        smolflag=1
        break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
bigflag=0
while True:
  bigflag=0
  words3=[]
  checks=[0]*len(words2)
  for i in range(len(words2)):
    if checks[i]:
      continue
    for j in range(len(words2)):
      if checks[i] or checks[j]:
        continue
      if i==j:
        continue
      word1=words2[i]
      word2=words2[j]
      len1=len(word1)
      len2=len(word2)
      if len1>confidparam and  len2>confidparam:
        for common in range(min(len1,len2)-1,confidparam-1,-1):
          if word1[-common:]==word2[:common]:
            if len1+len2-common<=n:
              tmp=[word1]
              tmp.append(word2[common:])
              words3.append(''.join(tmp))
              checks[i]=1
              checks[j]=1
              bigflag=1
              break
            else:
              innerflag=0
              for k in range(max(0,n-len1+common),len2):
                if word2[k]!=word1[(len1-common+k)%n]:
                  innerflag=1
                  break
              if innerflag==0:
                tmp=[word1,word2[common:]]
                words3.append(''.join(tmp)[:n])
                checks[i]=1
                checks[j]=1
                bigflag=1
                break
  for i in range(len(words2)):
    if not checks[i]:
      words3.append(words2[i])
  words2=words3.copy()
  if not bigflag:
    break
words3=[]
for word in words2:
  words3.append((word,len(word)))
words3.sort(key=lambda thing: thing[1]) 
words3.reverse()
words2=[]
for word,length in words3:
  flag=0
  for compword in words2:
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
for word in words2:
  words.append([word,0])
for word,length in origwords:
  for bigword in words:
    compword=bigword[0]
    smolflag=0
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
repcount=0
while True:
  repcount+=1
  curr=time.time()
  if curr-start>1.4:
    break
  already=set()
  numwordsfound=0
  random.shuffle(words)
  words.sort(key=lambda thing: thing[1])
  words.reverse()
  grid=['.']*(n*n)
  for q in words:
    word=q[0]
    leng=len(word)
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
    already.add(word)
  for word,leng in origwords:
    if word in already:
      numwordsfound+=1
      continue
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    numwordsfound+=1
    already.add(word)
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
  if numwordsfound>bestnumwordsfound:
    bestnumwordsfound=numwordsfound
    bestgrid=grid

confidparam=3

words2=[]
words=[]
for word,length in origwords:
  flag=0
  for bigword in words2:
    compword=bigword[0]
    smolflag=0
    for i in range(len(compword)-length+1):
      smolsmolflag=0
      for j in range(length):
        if compword[i+j]!=word[j]:
          smolsmolflag=1
          break
      if smolsmolflag==0:
        smolflag=1
        break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
bigflag=0
while True:
  bigflag=0
  words3=[]
  checks=[0]*len(words2)
  for i in range(len(words2)):
    if checks[i]:
      continue
    for j in range(len(words2)):
      if checks[i] or checks[j]:
        continue
      if i==j:
        continue
      word1=words2[i]
      word2=words2[j]
      len1=len(word1)
      len2=len(word2)
      if len1>confidparam and  len2>confidparam:
        for common in range(min(len1,len2)-1,confidparam-1,-1):
          if word1[-common:]==word2[:common]:
            if len1+len2-common<=n:
              tmp=[word1]
              tmp.append(word2[common:])
              words3.append(''.join(tmp))
              checks[i]=1
              checks[j]=1
              bigflag=1
              break
            else:
              innerflag=0
              for k in range(max(0,n-len1+common),len2):
                if word2[k]!=word1[(len1-common+k)%n]:
                  innerflag=1
                  break
              if innerflag==0:
                tmp=[word1,word2[common:]]
                words3.append(''.join(tmp)[:n])
                checks[i]=1
                checks[j]=1
                bigflag=1
                break
  for i in range(len(words2)):
    if not checks[i]:
      words3.append(words2[i])
  words2=words3.copy()
  if not bigflag:
    break
words3=[]
for word in words2:
  words3.append((word,len(word)))
words3.sort(key=lambda thing: thing[1]) 
words3.reverse()
words2=[]
for word,length in words3:
  flag=0
  for compword in words2:
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
for word in words2:
  words.append([word,0])
for word,length in origwords:
  for bigword in words:
    compword=bigword[0]
    smolflag=0
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
repcount=0
while True:
  repcount+=1
  curr=time.time()
  if curr-start>1.6:
    break
  already=set()
  numwordsfound=0
  random.shuffle(words)
  words.sort(key=lambda thing: thing[1])
  words.reverse()
  grid=['.']*(n*n)
  for q in words:
    word=q[0]
    leng=len(word)
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
    already.add(word)
  for word,leng in origwords:
    if word in already:
      numwordsfound+=1
      continue
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    numwordsfound+=1
    already.add(word)
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
  if numwordsfound>bestnumwordsfound:
    bestnumwordsfound=numwordsfound
    bestgrid=grid
    
confidparam=6

words2=[]
words=[]
for word,length in origwords:
  flag=0
  for bigword in words2:
    compword=bigword[0]
    smolflag=0
    for i in range(len(compword)-length+1):
      smolsmolflag=0
      for j in range(length):
        if compword[i+j]!=word[j]:
          smolsmolflag=1
          break
      if smolsmolflag==0:
        smolflag=1
        break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
bigflag=0
while True:
  bigflag=0
  words3=[]
  checks=[0]*len(words2)
  for i in range(len(words2)):
    if checks[i]:
      continue
    for j in range(len(words2)):
      if checks[i] or checks[j]:
        continue
      if i==j:
        continue
      word1=words2[i]
      word2=words2[j]
      len1=len(word1)
      len2=len(word2)
      if len1>confidparam and  len2>confidparam:
        for common in range(min(len1,len2)-1,confidparam-1,-1):
          if word1[-common:]==word2[:common]:
            if len1+len2-common<=n:
              tmp=[word1]
              tmp.append(word2[common:])
              words3.append(''.join(tmp))
              checks[i]=1
              checks[j]=1
              bigflag=1
              break
            else:
              innerflag=0
              for k in range(max(0,n-len1+common),len2):
                if word2[k]!=word1[(len1-common+k)%n]:
                  innerflag=1
                  break
              if innerflag==0:
                tmp=[word1,word2[common:]]
                words3.append(''.join(tmp)[:n])
                checks[i]=1
                checks[j]=1
                bigflag=1
                break
  for i in range(len(words2)):
    if not checks[i]:
      words3.append(words2[i])
  words2=words3.copy()
  if not bigflag:
    break
words3=[]
for word in words2:
  words3.append((word,len(word)))
words3.sort(key=lambda thing: thing[1]) 
words3.reverse()
words2=[]
for word,length in words3:
  flag=0
  for compword in words2:
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
for word in words2:
  words.append([word,0])
for word,length in origwords:
  for bigword in words:
    compword=bigword[0]
    smolflag=0
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
repcount=0
while True:
  repcount+=1
  curr=time.time()
  if curr-start>1.8:
    break
  already=set()
  numwordsfound=0
  random.shuffle(words)
  words.sort(key=lambda thing: thing[1])
  words.reverse()
  grid=['.']*(n*n)
  for q in words:
    word=q[0]
    leng=len(word)
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
    already.add(word)
  for word,leng in origwords:
    if word in already:
      numwordsfound+=1
      continue
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    numwordsfound+=1
    already.add(word)
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
  if numwordsfound>bestnumwordsfound:
    bestnumwordsfound=numwordsfound
    bestgrid=grid
      
confidparam=5

words2=[]
words=[]
for word,length in origwords:
  flag=0
  for bigword in words2:
    compword=bigword[0]
    smolflag=0
    for i in range(len(compword)-length+1):
      smolsmolflag=0
      for j in range(length):
        if compword[i+j]!=word[j]:
          smolsmolflag=1
          break
      if smolsmolflag==0:
        smolflag=1
        break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
bigflag=0
while True:
  bigflag=0
  words3=[]
  checks=[0]*len(words2)
  for i in range(len(words2)):
    if checks[i]:
      continue
    for j in range(len(words2)):
      if checks[i] or checks[j]:
        continue
      if i==j:
        continue
      word1=words2[i]
      word2=words2[j]
      len1=len(word1)
      len2=len(word2)
      if len1>confidparam and  len2>confidparam:
        for common in range(min(len1,len2)-1,confidparam-1,-1):
          if word1[-common:]==word2[:common]:
            if len1+len2-common<=n:
              tmp=[word1]
              tmp.append(word2[common:])
              words3.append(''.join(tmp))
              checks[i]=1
              checks[j]=1
              bigflag=1
              break
            else:
              innerflag=0
              for k in range(max(0,n-len1+common),len2):
                if word2[k]!=word1[(len1-common+k)%n]:
                  innerflag=1
                  break
              if innerflag==0:
                tmp=[word1,word2[common:]]
                words3.append(''.join(tmp)[:n])
                checks[i]=1
                checks[j]=1
                bigflag=1
                break
  for i in range(len(words2)):
    if not checks[i]:
      words3.append(words2[i])
  words2=words3.copy()
  if not bigflag:
    break
words3=[]
for word in words2:
  words3.append((word,len(word)))
words3.sort(key=lambda thing: thing[1]) 
words3.reverse()
words2=[]
for word,length in words3:
  flag=0
  for compword in words2:
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
    if smolflag:
      flag=1
      break
  if not flag:
    words2.append(word)
for word in words2:
  words.append([word,0])
for word,length in origwords:
  for bigword in words:
    compword=bigword[0]
    smolflag=0
    if len(compword)!=20:
      smolflag=0
      for i in range(len(compword)-length+1):
        smolsmolflag=0
        for j in range(length):
          if compword[i+j]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
    else:
      smolflag=0
      for i in range(len(compword)):
        smolsmolflag=0
        for j in range(length):
          if compword[(i+j)%n]!=word[j]:
            smolsmolflag=1
            break
        if smolsmolflag==0:
          smolflag=1
          break
      if smolflag:
        bigword[1]+=1
repcount=0
while True:
  repcount+=1
  curr=time.time()
  if curr-start>2.85:
    break
  already=set()
  numwordsfound=0
  random.shuffle(words)
  words.sort(key=lambda thing: thing[1])
  words.reverse()
  grid=['.']*(n*n)
  for q in words:
    word=q[0]
    leng=len(word)
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
    already.add(word)
  for word,leng in origwords:
    if word in already:
      numwordsfound+=1
      continue
    bestcount=21
    for i in range(n):
      if bestcount==0:
        break
      for j in range(n):
        if bestcount==0:
          break
        # horizontal
        flag=0
        count=0
        for delta in range(leng):
          if grid[i*n+(j+delta)%n]=='.':
            count+=1
          elif grid[i*n+(j+delta)%n]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=0
          bestcount=count
        # vertical
        flag=0
        count=0
        for delta in range(leng):
          if grid[((i+delta)%n)*n+j]=='.':
            count+=1
          elif grid[((i+delta)%n)*n+j]!=word[delta]:
            flag=1
            break
        if flag==0 and count<bestcount:
          besti=i
          bestj=j
          direct=1
          bestcount=count
    if bestcount==21:
      continue
    numwordsfound+=1
    already.add(word)
    if direct==0:
      for delta in range(leng):
        grid[besti*n+(bestj+delta)%n]=word[delta]
    else:
      for delta in range(leng):
        grid[((besti+delta)%n)*n+bestj]=word[delta]
  if numwordsfound>bestnumwordsfound:
    bestnumwordsfound=numwordsfound
    bestgrid=grid

for i in range(20):
  print(''.join(bestgrid[i*n:(i+1)*n]))