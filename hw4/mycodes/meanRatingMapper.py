#!/usr/bin/python3

#mapper
import sys
import csv

# read movies file
movieFile = "./movies.csv"
movieList = {}

with open(movieFile, mode = 'r') as infile:
    reader = csv.reader(infile)
    for row in reader:
        movieId = row[0]
        movieList[movieId] = {}
        movieList[movieId]["title"] = row[1]
        movieList[movieId]["genre"] = row[2]


for oneMovie in sys.stdin:
    oneMovie= oneMovie.strip()
    ratingInfo = oneMovie.split(",")
    try:
        userId = ratingInfo[0]
        movieId = ratingInfo[1]
        rating = float(ratingInfo[2])
        movieTitle = movieList[movieId]["title"]
        movieGenre = movieList[movieId]["genre"]
        # Split the complex genres into a list of different genres
        # then find the ratings of each genre 
        genres = movieGenre.split("|")
        for genre in genres:
            # print the genre-rating pairs
            print("%s\t%s"%(genre,rating))
        pass
    
    except  IndexError:
        continue
    except  ValueError:
        continue
        pass
    
