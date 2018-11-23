#calculate kmers and their occurences
import sys
from collections import Counter
def counter(seq_lst, k):
    kmers = []
    for seq in seq_lst:
        print (seq)
        if len(seq) >= k:
            for i in range(len(seq) - k + 1):
                kmers.append(seq[i:i+k])
                
    return Counter(kmers) 



if __name__ == "__main__":
    cnt_dict = counter(sys.argv[1].split(","), int(sys.argv[2]))
    print (len(cnt_dict))

