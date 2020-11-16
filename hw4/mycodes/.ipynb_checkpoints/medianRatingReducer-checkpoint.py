#!/usr/bin/python3

#Note that before using reducer, we need to sort/shuffle data
import sys 

movie_genre = None
rating_ls = []
for line in sys.stdin:
    line = line.strip()
    genre, rating = line.split("\t", 2)

    try:
        if  len(rating_ls)>0 and  movie_genre != genre:
            # Find the median of rating of a genre by sorting and finding the middel value of list
            # Note: sort must be applied in hdfs before using medianRatingReducer, so that
            # value in the middle position = median
            index = len(rating_ls)//2
            genre_median = rating_ls[index]
            print("%s\t%f" % ( movie_genre, genre_median))
            rating_ls.clear()
        rating = float(rating)
        rating_ls.append(rating)    
        movie_genre = genre
    except ValueError: 
        continue

#Check the last genre in list and print its median
if  len(rating_ls)>0:
    index = len(rating_ls)//2
    genre_median = rating_ls[index]
    print("%s\t%f" % ( movie_genre, genre_median))
    
