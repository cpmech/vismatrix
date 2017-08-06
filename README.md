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

Download vismatrix into **~/xpkg/vismatrix**

In Ubuntu/Debian:
```
sudo apt-get install freeglut3-dev libxmu-dev libxi-dev cmake-curses-gui

mkdir -p ~/xpkg/build_vismatrix
cd ~/xpkg/build_vismatrix
ccmake ~/xpkg/vismatrix
[c] [c] [g]
make
sudo make install
```
