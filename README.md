# Vismatrix &ndash; Visualization of sparse matrices

<img src="figs/fig00a.png">

<img src="figs/fig00b.png">

![](figs/vismatrix01.gif)

Vismatrix is a nice tool to visualize sparse matrices in (.smat) format

## Installation (Ubuntu/Linux)

```bash
sudo apt install libgl1-mesa-dev libglut-dev
bash all.bash
```

## Acknowledgements

To [Professor David Gleich](https://github.com/dgleich) for this wonderful tool.

## License

```
------------------------------------------------------------
vismatrix 2.0 
------------------------------------------------------------
Copyright David Gleich, Leonid Zhukov, 2006-2007.
```

Unless otherwise noted, the source files are distributed under the BSD-style license found in the
LICENSE file.

See also LICENSE files in each one of the subdirectories boost, glui, and tclap.

## Installation

**1** Install dependencies (tested on **Ubuntu 23.04 LTS**):

```
sudo apt-get install freeglut3-dev libxmu-dev libxi-dev libz-dev g++ make cmake
```

**2** Download the code into /tmp/vismatrix:

```
git clone --depth 1 https://github.com/cpmech/vismatrix.git /tmp/vismatrix
```

**3** Compile the code (in /tmp/build-vismatrix):

```
./all.bash
```

**4** The executable file will be /tmp/build-vismatrix and you may install into /usr/local/bin by using:

```
sudo cp /tmp/build-vismatrix/vismatrix /usr/local/bin/
```

## Usage

The matrix input file is quite simple (0-index based):

```
m n nnz
i j x
...
i j x
```

where the first line has _m_ as the number of rows, _n_ as the number of columns, and _nnz_ as the number of non-zero values. The following lines contain the index of row _i_ and column _j_ of the non-zero entry _x_.

See [example.smat](https://github.com/cpmech/vismatrix/blob/master/example.smat) file.

Usage:

```
vismatrix example.smat
```

Some commands:

* right-click to Exit
* mouse move: pan
* shift + mouse: zoom
* control + click: inspect value
* right-click: options

![](figs/vismatrix02.gif)

<img src="figs/fig01.png" width="400">

<img src="figs/fig02.png" width="400">
