The literature adapted instances use the client data from Solomon (1986)'s VRPTW instances. They are available at: http://w.cba.neu.edu/~msolomon/problems.htm
In this folder you will find the clients localization, demand, time windows, service time and vehicle costs and capacity that should be used.

For clients localization, demand, time windows and service time, you can find one file for each group of instances (R1, R2, C1, C2, RC1 or RC2).
For vehicle costs and capacity, the Vehicles.txt file contains the data for all the groups.

The file is organised for each instance group.
Ex.:

C1                -> Instance group (R1, R2, C1, C2, RC1 or RC2)
3                 -> Number of types of vehicles (K)
5                 -> Number of distance Ranges   (F)
100 200 300       -> Vehicle Capacity for each type of vehicle (a_k)
0 40 70 100 130   -> start of each distance range (W_f)

-> Cost per distance range and type of vehicle (Ckf):
- Costs for C1a:
300 315 329 343 1 
800 816 831 845 1
1,350 1,366 1,382 1,397 1

- Costs for C1b:
60 75 89 103 1	
160 176 191 205 1
270 286 302 317 1

- Costs for C1c:
30 45 59 73 1
80 96 111 125 1	
135 151 167 182 1	
