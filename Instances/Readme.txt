The instances to evaluate methods for the FSMVRPTWSC are present in this folder, as well as the current known results.

***** INSTANCES: *****

There are three groups of Instances available:

- Literature Adapted Instances:
168 instances based on the instances proposed by Liu and Shen (1999) that are an adaptation of the instances proposed by Solomon (1986);
They can be found in the folder "Literature Adapted", with the original client data and our contribution with vehicle costs and capacity and distance ranges inside the folder.

- Small Sized Instances:
72 instances that are adapted from the Literature Adapted Instances;
The file "Small.txt" contains the data for each of these instances.

- Real Case Inspired Instances:
3 instances based on three days of deliveries of a construction material supplier in Sao Paulo (Brazil);
The file "Real.txt" contains the data of these instantes.

Each Instance file (in this folder - Small.txt and Real.txt) is organized as follows:

[Quantity of Instances in the file]

[Name of the instances]
[Quantity of clients - N] [Quantity of types of vehicles - K] [Quantity of distance ranges - F]

[Distances between clients - D_ij] -> It is a table format, where the first row and column refers to the deposit

[Transit Time between clients - T_ij] -> It is a table format, where the first row and column refers to the deposit

[Time Windows Start- e_i] [Time Windows End - l_i] [Service Time - s_i] [Demand - d_i] -> Each row refers to a client, where the first row is for the deposit

[Capacity of each vehicle Type - a_k] -> In one row
[Final distance of each distance range - w_f] -> In one row

[Cost of each vehicle type and distance range - C_kf] -> Each row is a type of vehicle and distance range is a column, where the last column is the incremental cost for each distance unit

***** RESULTS *****

The results are compiled in the Results.xls file, a MS Excel file.

- Literature Adapted Instances:
The results are presented in the sheet named "Literature Adapted".
The values are grouped by combinations of spatial distribution (R/C/RC), time windows width(1/2) and vehicle costs (a/b/c)
The "Avg." column present the average cost found by the constructive heuristic for each group of instances.
The "%Best" column present the percentage of best known results amont the constructive heuristics for each group of instances.
The "Local Search" present the average cost found by the constructive heuristic after the Relocate local search for each group of instances.
The "CPU Time" present the computing time to perform the local search.
The "%Improv" present the percentage improvement in cost reduction generated by the local search after the constructive heuristic.

- Small Sized Instances:
All instances whose optimal solution is known are presented in a list in the "Small" sheet.
For each instance, the optimial solution (cost) is presented in the column "Optimal".
The result found by each constructive method is present in the following column, as well as the GAP to optimal.
Optimal values are highlighted in bold font.

- Real Case Inspired Instances:
The results for the three instances are presented in a list in the "Real" sheet.
For every method the solution value is presented in a "Cost" column and the number of routes in the solution is under "#Routes"
The reported solution that the industry performed is presented in the column "Reported".