# Maze with levers solver
Programm finds the closest path to the given coordinates. Levers apply xor operation on rows of the map.

## Input
Numbers n and k, n represents one side of the square map and k number of levers.
Then k rows which represent the position and configuration of each lever.
And then the maze itself.

## Output
If the end can not be reached then the output is -1.
On the next row is length of the closest path to the given coordinates.
Then the configuration of applied levers (Set of k numbers from {0, 1})
Then the path itself.

## Examples
### Input
```
5 1
3 00100
00001 00010 00100 01000 10000
5 5
```

### Output
```
14
1
[1;1],[1;2],[1;3],[2;3],[3;3],[4;3],[4;4],[4;5],[5;5]
```
### Input
```
5 0
01000 00000 00000 00000 00000
3 1
```

### Output
```
4
[1;1],[1;2],[2;2],[3;2],[3;1]
```