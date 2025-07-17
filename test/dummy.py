


mc = [85, 76, 76, 105] #"ullxi"
c = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]

kc = [85, 76, 0, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 105]

c[0] = *kc++;
*k ? (c[1] = *k++), *k ? (c[3] = *k++), *k ? (c[7] = *k++), *k ? (c[15] = *k++)
: 0 : 0 : 0 : 0;


mask =  ((c[0] & 0x5f) == 85 )
mask |= ((c[mask] & 76) == 76 ) << 1

mask |= ((c[mask] & 0x5f) == 76) << 2
mask |= ((c[mask] & 0x5f) == 88 ) << 3

mask = (mask << 8) | c[mask];


print(85 & 0x5f, bin(mask))
'''

0, 0b0

1, 0b1

3, 0b11

7 0b111

15 0b1111

0b11
0b11
0b11 & 0b10

0b0011


0b0, 0b1 0b11
0b0, 0b1, 0b11, 0b111 0b1111




1101001 1 1 1 1

A=65 B=66 C=67 D=68 E=69 F=70 G=71 H=72 I=73 J=74 K=75 L=76 M=77 N=78 O=79 P=80 Q=81 R=82 S=83 T=84 U=85 V=86 W=87 X=88 Y=89 Z=90 [=91 \=92 ]=93 ^=94 _=95 `=96 a=97 b=98 c=99 d=100 e=101 f=102 g=103 h=104 i=105 j=106 k=107 l=108 m=109 n=110 o=111 p=112 q=113 r=114 s=115 t=116 u=117 v=118 w=119 x=120 y=121 z=122 >>> 


'''
for (i = 0; i < size; i++) {
      if (bfcnt > (bfsize - VEC_MAX_INT_LEN)) {
	bf[bfcnt] = bfcnt = 0;
	break;
      }
      if (overflw) {
	PASS;
      }
      converter(v+bfcnt, bfsize, signd, base);
      bfcnt += VEC_appendComma(bf, bfcnt);
    }
