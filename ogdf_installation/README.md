# How to install OGDF

## Method 1: Use `PKGBUILD` in Arch Linux

1. Execute the `generate_PKGBUILD_patch.sh` script. This will generate the `ogdf-git.patch` file.
2. Make a new folder for building the package, e.g. build
3. Copy `PKGBUILD` and `ogdf-git.patch` to the new folder
4. `cd` into the folder and run `makepkg -si`

## Method 2: Use the install script

1. Clone the latest ogdf snapshot from the git repository `git@github.com:ogdf/ogdf.git`
    (we used commit `8836cfd4bb4d3702b753178690711b0de1c1baf2`)
2. Copy the following files into the cloned ogdf git repository:
    - `install`
    - `RemoveAsserts.patch`
    - `CompressSVG.patch`
3. Execute the install script inside the ogdf folder. It installs the patched ogdf library to the system, so it will ask for a root password.
