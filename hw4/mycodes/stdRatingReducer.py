#!/usr/bin/python3

import sys 
import math
movie_genre = None
rating_ls = []
for line in sys.stdin:
    line = line.strip()
    genre, rating = line.split("\t", 2)

    try:
        if  len(rating_ls)>0 and  movie_genre != genre:
            # Find standard derivate of rating list of one genre
            mean_val = sum(rating_ls)/len(rating_ls)
            val_ls = [(val - mean_val)**2 for val in rating_ls]
            std = math.sqrt(sum(val_ls)/len(val_ls))
            print("%s\t%f" % ( movie_genre, std))
            # reset the list of ratings of current genre
            rating_ls.clear()
        rating = float(rating)
        rating_ls.append(rating)    
        movie_genre = genre
    except ValueError: 
        continue

#Check the last genre in list and print its standard devirative
if  len(rating_ls)>0:
    mean_val = sum(rating_ls)/len(rating_ls)
    val_ls = [(val - mean_val)**2 for val in rating_ls]
    std = math.sqrt(sum(val_ls)/len(val_ls))
    print("%s\t%f" % ( movie_genre, std))
    rating_ls.clear()
