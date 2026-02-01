def solve(S):
    n = len(S)
    # 各括弧のペアを見つける
    stack = []
    pair = {}
    for i, c in enumerate(S):
        if c == '(':
            stack.append(i)
        elif c == ')':
            j = stack.pop()
            pair[j] = i
            pair[i] = j
    
    result = []
    
    def process(l, r, depth):
        # S[l:r+1] を処理（括弧を含まない範囲、または括弧で囲まれた範囲）
        i = l
        segments = []
        while i <= r:
            if S[i] == '(':
                j = pair[i]
                segments.append(('bracket', i, j, depth + 1))
                i = j + 1
            elif S[i] == ')':
                i += 1
            else:
                segments.append(('char', S[i], depth))
                i += 1
        
        if depth % 2 == 1:
            segments = segments[::-1]
        
        for seg in segments:
            if seg[0] == 'char':
                c = seg[1]
                if depth % 2 == 1:
                    c = c.swapcase()
                result.append(c)
            else:
                _, li, ri, d = seg
                process(li + 1, ri - 1, d)
    
    process(0, n - 1, 0)
    return ''.join(result)

S = input()
print(solve(S))