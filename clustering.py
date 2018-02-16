#!/bin/python3
"""Clusters a graph"""
import networkx as nx
import numpy as np
import itertools
from random import shuffle
from sklearn.cluster import KMeans
from sklearn.cluster import AgglomerativeClustering
from sklearn.cluster import SpectralClustering
from sklearn import metrics
import sys
np.random.seed(1)

if len(sys.argv) < 3:
    print("Usage: clustering.py <file> <output_file>")
    exit(1)

feature_list = list()
node_ids = list()
with open(sys.argv[1], "r") as file:
    for line in file:
        id, x ,y = line.split()
        feature_list.append([x,y])
        node_ids.append(id)

numpy_features = np.array(feature_list)

kmeans = KMeans(n_clusters=14, n_init=20, max_iter=300, n_jobs=-2)
#aggc = AgglomerativeClustering(14)
#spectral = SpectralClustering()

kmeans.fit(numpy_features)
#aggc.fit(numpy_features)
#spectral.fit(numpy_features)

labels = kmeans.labels_
#labels = spectral.labels_

with open(sys.argv[2], "w") as file:
    for i in range(len(node_ids)):
        file.write(node_ids[i] + " " + str(labels[i]) + "\n")
