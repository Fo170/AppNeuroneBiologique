#!/usr/bin/env python3
"""Generate 74LS181 truth table dataset + neural network architecture."""

import csv, os

OUT = os.path.dirname(__file__)

def logic_f(s, a, b):
    """Compute one bit of F for given function select S and bits A,B (M=1)."""
    if s == 0:   return (~a) & 1
    if s == 1:   return (~(a|b)) & 1
    if s == 2:   return ((~a) & b) & 1
    if s == 3:   return 0
    if s == 4:   return (~(a&b)) & 1
    if s == 5:   return (~b) & 1
    if s == 6:   return a ^ b
    if s == 7:   return (a & (~b)) & 1
    if s == 8:   return ((~a) | b) & 1
    if s == 9:   return (~(a ^ b)) & 1
    if s == 10:  return b
    if s == 11:  return a & b
    if s == 12:  return 1
    if s == 13:  return (a | (~b)) & 1
    if s == 14:  return a | b
    if s == 15:  return a

def bits8(v, n=4):
    return [(v>>i)&1 for i in range(n)]

# ── Dataset: 74LS181 logic mode (M=1) ──
# For each combo of A(0..15), B(0..15), S(0..15), compute F(0..15)
# Inputs: a0-a3, b0-b3, s0-s3, m, cn  (14 cols)
# Outputs: f0-f3 (4 cols)

header_inputs  = ["a0","a1","a2","a3","b0","b1","b2","b3","s0","s1","s2","s3","m","cn"]
header_outputs = ["f0","f1","f2","f3","a_eq_b"]
header = header_inputs + header_outputs

rows = []
for a in range(16):
    for b in range(16):
        for s in range(16):
            a_bits = bits8(a)
            b_bits = bits8(b)
            s_bits = bits8(s)
            f_bits = [logic_f(s, a_bits[i], b_bits[i]) for i in range(4)]
            a_eq_b = 1 if a == b else 0
            row = a_bits + b_bits + s_bits + [1, 0] + f_bits + [a_eq_b]
            rows.append(row)

dataset_path = os.path.join(OUT, "datasets", "74ls181.csv")
os.makedirs(os.path.dirname(dataset_path), exist_ok=True)
with open(dataset_path, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["# nb_entrees=14"])
    w.writerow(header)
    w.writerows(rows)
print(f"Dataset: {dataset_path}  ({len(rows)} rows)")

# ── Generate dataset samples for training (sampled subset for faster training) ──
# Sample: 25% of rows = 1024 rows
import random
random.seed(42)
sampled = random.sample(rows, min(1024, len(rows)))
dataset_sub_path = os.path.join(OUT, "datasets", "74ls181_1024.csv")
with open(dataset_sub_path, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["# nb_entrees=14"])
    w.writerow(["e1","e2","e3","e4","e5","e6","e7","e8","e9","e10","e11","e12","e13","e14",
                 "cible1","cible2","cible3","cible4","cible5"])
    for row in sampled:
        w.writerow(row)
print(f"Subset: {dataset_sub_path}  ({len(sampled)} rows)")

# ── Network architecture ──
# 14 inputs (a0-3, b0-3, s0-3, m, cn)
# Hidden layer: 24 neurons
# Hidden layer: 12 neurons
# Output: 5 neurons (f0-3, a_eq_b)
# Layout: 14 → 24 → 12 → 5

net_path = os.path.join(OUT, "reseaux", "74ls181.csv")
with open(net_path, "w") as f:
    f.write("[NEURONES]\n")
    f.write("id,nom,V_rest,tau,biais,refractaire_ms,eta,oubli,est_entree,x,y\n")

    # Input neurons: ids 0-13
    labels = ["a0","a1","a2","a3","b0","b1","b2","b3",
              "s0","s1","s2","s3","m","cn"]
    for i, lab in enumerate(labels):
        f.write(f"{i},{lab},0,10,0,2,0.01,0.001,1,{50+(i%7)*60},{50+(i//7)*250}\n")

    # Hidden layer 1: ids 14-37 (24 neurons)
    for i in range(24):
        nid = 14 + i
        x = 400 + (i//6)*60
        y = 30 + (i%6)*70
        f.write(f"{nid},h1_{i},0,10,0.2,2,0.02,0.001,0,{x},{y}\n")

    # Hidden layer 2: ids 38-49 (12 neurons)
    for i in range(12):
        nid = 38 + i
        x = 700 + (i//4)*60
        y = 80 + (i%4)*80
        f.write(f"{nid},h2_{i},0,10,0.2,2,0.02,0.001,0,{x},{y}\n")

    # Output: ids 50-54 (5 neurons)
    olabs = ["f0","f1","f2","f3","a_eq_b"]
    for i, lab in enumerate(olabs):
        nid = 50 + i
        x = 1050
        y = 60 + i*120
        f.write(f"{nid},{lab},0,10,0,2,0.02,0.001,0,{x},{y}\n")

    f.write("[SYNAPSES]\n")
    f.write("id,source,target,poids,type\n")

    sid = 0
    # Input → Hidden1: 14 * 24 = 336 synapses
    for src in range(14):
        for dst in range(14, 38):
            f.write(f"{sid},{src},{dst},0.3,AXO_DENDRITIQUE\n")
            sid += 1

    # Hidden1 → Hidden2: 24 * 12 = 288 synapses
    for src in range(14, 38):
        for dst in range(38, 50):
            f.write(f"{sid},{src},{dst},0.3,AXO_DENDRITIQUE\n")
            sid += 1

    # Hidden2 → Output: 12 * 5 = 60 synapses
    for src in range(38, 50):
        for dst in range(50, 55):
            f.write(f"{sid},{src},{dst},0.3,AXO_DENDRITIQUE\n")
            sid += 1

print(f"Network: {net_path}  ({38+5} neurons, {sid} synapses)")
print("Done!")
