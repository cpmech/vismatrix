# Vismatrix &ndash; Visualization of sparse matrices

Vismatrix is a nice tool to visualize sparse matrices in (.smat) format

<div id="container">
<p><img src="figs/fig01.png" width="400"></p>
<p><img src="figs/fig02.png" width="400"></p>
</div>

## License

Unless otherwise noted, the source files are distributed under the BSD-style license found in the
LICENSE file.

See also LICENSE files in each one of the subdirectories 'boost', 'glui', and 'tclap'.

## Acknowledgements

To Professor David Gleich for this wonderful tool.

## Installation

**1** Install dependencies (tested on `Ubuntu 20.04.1 LTS`):
```
sudo apt-get install freeglut3-dev libxmu-dev libxi-dev libz-dev g++ make cmake
```

**2** Download the code into `/tmp/vismatrix`:
```
git clone --depth 1 https://github.com/cpmech/vismatrix.git /tmp/vismatrix
```

**3** Compile the code:
```
cd /tmp/vismatrix/src
cmake -Wno-dev .
make
```

**4** The executable file will be `/tmp/vismatrix/src/vismatrix` and you may install into `/usr/local/bin` by using:
```
sudo make install
```
