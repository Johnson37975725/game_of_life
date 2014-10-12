import random,time
COL,ROW=30,30
init=lambda:[[random.randint(0,1) for j in range(ROW)] for i in range(COL)]
neig=lambda c,x,y:sum([c[(x+i+COL)%COL][(y+j+ROW)%ROW] for j in [0,1,-1] for i in [0,1,-1]][1:])
nexT=lambda c:[[1 if neig(c,i,j)==3 else (c[i][j] if neig(c,i,j)==2 else 0) for j in range(ROW)] for i in range(COL)]
prin=lambda c:print('\n'.join([''.join(['[]' if c[i][j]==1 else '  ' for j in range(ROW)]) for i in range(COL)]))
life=lambda c=init():prin(c) or print(''.join(['--' for i in range(ROW)])) or time.sleep(1) or life(nexT(c))
life()
