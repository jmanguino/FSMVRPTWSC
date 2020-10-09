The literature adpted instances use the client data from Solomon (1986)'s VRPTW instances. It is available at: http://w.cba.neu.edu/~msolomon/problems.htm
In this folder you will find the vehicle data that should be used.

The file is organized as presented bellow for each instance group.

C1                -> Instance group (R1, R2, C1, C2, RC1 or RC2)
3                 -> Number of types of vehicles (K)
5                 -> Number of distance Ranges   (F)
100 200 300       -> Vehicle Capacity for each type of vehicle (a_k)
0 40 70 100 130   -> start of each distance range (W_f)
                  -> vehicles of cost group "a", meaning these are the costs for C1a
300 315 329 343 1 -> Cost per distance range and type of vehicle (C_{kf})
800 816 831 845 1
1,350 1,366 1,382 1,397 1
                  -> vehicles of cost group "b", meaning these are the costs for C1b
60 75 89 103 1	
160 176 191 205 1
270 286 302 317 1
                  -> vehicles of cost group "c", meaning these are the costs for C1c
30 45 59 73 1
80 96 111 125 1	
135 151 167 182 1	
