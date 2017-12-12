#!/bin/bash

OGDF_GIT='https://github.com/ogdf/ogdf.git'
OGDF_DIR='ogdf'
PATCH_NAME='ogdf-git.patch'

git clone $OGDF_GIT $OGDF_DIR
cd $OGDF_DIR
git apply ../RemoveAsserts.patch
git apply ../CompressSVG.patch
git diff > $PATCH_NAME
mv $PATCH_NAME ../$PATCH_NAME
cd ..
rm -rf $OGDF_DIR
