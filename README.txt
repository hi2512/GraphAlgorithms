CS 377P - Assignment 2: Graph Algorithms
----------------------------------------

Eric Nguyen 
Ramsey Hashem

hw2 takes in one argument, the input DIMACS file.
The outputs are the DIMACS file (additional edges
between the same nodes are removed with only the
largest weight remaining following the homework
spec), the node number and label file, and a file
containing the histogram. 

To compile and run:
- Navigate to the GraphAlgorithms directory
- Use 'make' to compile
- To run, do ./hw2 <filename>
- For example, to run with the wiki.dimacs file, do:
  ./hw2 wiki.dimacs

The PageRank is output to standard output. 
The results of the histogram can be found in histogram.txt
The conversion from CSR to DIMACS is output to out.dimacs

The raw PageRank results can be viewed in:
wikipr.txt
road-NYpr.txt
rmat15pr.txt



