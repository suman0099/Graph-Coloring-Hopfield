# Graph-Coloring-Hopfield
Implementation of Hopfield Network for k-Coloring Problem in Graph Theory

<b> How to use</b>
My program can takes inputs in DIMACS format from the commandline as an argument.<br>

Just copy the DIMACS col file into the same directory.

Compilation<br>
g++ *.cpp -O3 -o hop

<br>Example
<br>For Linux Systems
<br>./hop k4.col

<br>It will create a file hopfield.txt which will contain the number of colours used and time taken to colour the graph in millisecond
