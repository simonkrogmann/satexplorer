#!/bin/python3
"""Clusters a graph"""
import networkx as nx
import numpy as np
import itertools
from sklearn.cluster import SpectralClustering
from sklearn import metrics
import sys
np.random.seed(1)

if len(sys.argv) < 2:
    print("Usage: clustering.py <file.gml>")
    exit(1)

# Get your mentioned graph
G = nx.read_gml(sys.argv[1])

# Get ground-truth: club-labels -> transform to 0/1 np-array
#     (possible overcomplicated networkx usage here)
# gt_dict = nx.get_node_attributes(G, 'club')
# gt = [gt_dict[i] for i in G.nodes()]
# gt = np.array([0 if i == 'Mr. Hi' else 1 for i in gt])

# Get adjacency-matrix as numpy-array
adj_mat = nx.to_numpy_matrix(G)

# print('ground truth')
# print(gt)

# Cluster
sc = SpectralClustering(2, affinity='precomputed', n_init=100)
sc.fit(adj_mat)

# Compare ground-truth and clustering-results
print('spectral clustering')
print(sc.labels_)
# print('just for better-visualization: invert clusters (permutation)')
# print(np.abs(sc.labels_ - 1))

# Calculate some clustering metrics
# print(metrics.adjusted_rand_score(gt, sc.labels_))
# print(metrics.adjusted_mutual_info_score(gt, sc.labels_))