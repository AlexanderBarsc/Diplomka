syms lam  
A = [0 1; 2 -1];
B = [1; 0]
C = [1 0]
I = [1 0; 0 1];
P = [-2 -1];
K = place(A, B, P);

result = A - lam*I
d = det(result)

solution = roots([1 1 -2])

syscl = ss(A - B*K, B, C, [0]);

step(syscl);