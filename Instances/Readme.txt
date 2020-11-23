The instances to evaluate methods to solve the FSMVRPTWSC are present in this folder.

There are three groups of Instances available:

- Literature Adapted Instances:
168 instances based on the instances proposed by Liu and Shen (1999) that are an adaptation of the instances proposed by Solomon (1986);
They can be found in the folder "Literature Adapted" with instructions inside the folder.

- Small Sized Instances:
72 instances that are adapted from the Literature Adapted Instances;
The file "Small.txt" contains the data for each of these instances.

- Real Case Inspired Instances:
3 instances based on three days of deliveries of a construction material supplier in Sao Paulo (Brazil);
The file "Real.txt" contains the data of these instantes.

Each Instance file (in this folder) is organized as follows:

[Quantity of Instances in the file]

[Name of the instances]
[Quantity of clients - N] [Quantity of types of vehicles - K] [Quantity of distance ranges - F]

[Distances between clients - D_ij] -> It is a table format, where the first row and column refers to the deposit

[Transit Time between clients - T_ij] -> It is a table format, where the first row and column refers to the deposit

[X-coord - X_i] [Y-coord - Y_i] [Service Time - s_i] [Demand - d_i] -> Each row refers to a client, where the first row is for the deposit

[Capacity of each vehicle Type - a_k] -> In one row
[Final distance of each distance range - w_f] -> In one row

[Cost of each vehicle type and distance range - C_kf] -> Each row is a type of vehicle and distance range is a column, where the last column is the incremental cost for each distance unit