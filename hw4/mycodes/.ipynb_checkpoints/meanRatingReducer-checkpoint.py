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
            # Find the mean of rating of a genre 
            
            genre_mean = sum(rating_ls)/float(len(rating_ls))
            print("%s\t%f" % ( movie_genre, genre_mean))
            rating_ls.clear()
        rating = float(rating)
        rating_ls.append(rating)    
        movie_genre = genre
    except ValueError: 
        continue

#Check the last genre in list and print its median
if  len(rating_ls)>0:
    genre_mean = sum(rating_ls)/float(len(rating_ls))
    print("%s\t%f" % ( movie_genre, genre_mean))
