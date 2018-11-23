#generate coordinates for pe and se used in latex
import sys
edge_size = 0.25 
ledge_size = 2.5 
def get_coords(x, y, ins, col):#pair end reads
    global edge_size
    if ins <  2*edge_size:
        ins = edge_size*2 + 0.5
    # edge = x+shape_size/2+el, y -> x + 1 +  , y 
    # edge2 = x + ins - 0.125
    
    # n1 = [x, y]
    # n2 = [x + ins, y]
    e1 = [[x, y], [x+edge_size, y]]
    e2 = [[x+ ins, y], [x+ins - edge_size, y]]
    tikz_ecmd = '\draw [->, {2}] (\\xs + {0[0][0]}, \\ys + {0[0][1]}) -- (\\xs + {0[1][0]}, \\ys + {0[1][1]}); \draw [->, {2}] (\\xs + {1[0][0]}, \\ys + {1[0][1]}) -- (\\xs + {1[1][0]}, \\ys + {1[1][1]});'.format(e1, e2, col) 
    tikz_dot_cmd = '\draw [dotted, {2}] (\\xs + {0[1][0]}, \\ys + {0[1][1]}) -- (\\xs + {1[0][0]}, \\ys + {1[0][1]});'.format(e1, e2, col)
    return  tikz_ecmd + tikz_dot_cmd

def get_long_coords2(x, y, direct, col):
    global ledge_size
    # edge = x+shape_size/2+el, y -> x + 1 +  , y 
    # edge2 = x + ins - 0.125

    if direct:
        # n1 = [x, y]
        e1 = [[x, y], [x + ledge_size, y]]
    else:
        # n1 = [x, y]

        e1 = [[x, y], [x - ledge_size, y]]
    # tikz_ncmd = '\\node [{0},minimum size={1}cm,draw] at ({2[0]}, {2[1]}) {{}};'.format(shape, shape_size, n1) 
    tikz_ecmd = '\draw [->, {1}] (\\xs + {0[0][0]}, \\ys + {0[0][1]}) -- (\\xs + {0[1][0]}, \\ys + {0[1][1]});'.format(e1, col) 
    return tikz_ecmd 

def get_coords2(x, y, direct, col):
    global edge_size
    # edge = x+shape_size/2+el, y -> x + 1 +  , y 
    # edge2 = x + ins - 0.125

    if direct:
        # n1 = [x, y]
        e1 = [[x, y], [x + edge_size, y]]
    else:
        # n1 = [x, y]

        e1 = [[x, y], [x - edge_size, y]]
    # tikz_ncmd = '\\node [{0},minimum size={1}cm,draw] at ({2[0]}, {2[1]}) {{}};'.format(shape, shape_size, n1) 
    tikz_ecmd = '\draw [->, {1}] (\\xs + {0[0][0]}, \\ys + {0[0][1]}) -- (\\xs + {0[1][0]}, \\ys + {0[1][1]});'.format(e1, col) 
    return tikz_ecmd 




if __name__ == "__main__":
    if len(sys.argv) < 6:
        print ("get_cords <pe> <x> <y> <ins:direction> <color>")
        sys.exit(1)
    tp = sys.argv[1] 
    x = float(sys.argv[2])
    y = float(sys.argv[3])
    ins = float(sys.argv[4])
    col = sys.argv[5]
    if tp == "pe":
        cmd = get_coords(x, y, ins, col)
    elif tp == "se":
        cmd = get_coords2(x, y, ins, col)
    else:
        cmd = get_long_coords2(x, y , ins, col)
    print (cmd)



