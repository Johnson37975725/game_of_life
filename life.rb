COL,ROW=20,20
def init;Array.new(COL){Array.new(ROW){rand(2)}}end
def neig(c,x,y);[0,1,-1].product([0,1,-1]).drop(1).collect{|i,j|c[(x+i+COL)%COL][(y+j+ROW)%ROW]}.inject(:+)end
def nexT(c);Array.new(COL){|i|Array.new(ROW){|j|neig(c,i,j)==3 ? 1 : (neig(c,i,j)==2 ? c[i][j] : 0)}}end
def prin(c);puts c.collect{|v|v.collect{|x|x==1 ? "[]" : "  "}.join("")}.join("\n")end
def life(c=init);prin(c);puts Array.new(ROW){"--"}.join("");sleep 1;life(nexT(c))end
life
