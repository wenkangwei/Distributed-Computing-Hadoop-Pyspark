#!/usr/bin/python3

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

user_count = {}
for oneMovie in sys.stdin:
    oneMovie= oneMovie.strip()
    ratingInfo = oneMovie.split(",")
    try:
        #parse the userId, movieId, title, genres from dataset
        userId = ratingInfo[0]
        movieId = ratingInfo[1]
        movieTitle = movieList[movieId]["title"]
        movieGenre = movieList[movieId]["genre"]
        # Split  genres into different genre
        # and Find each userID-movieGenre pair and the counts that this user watch this genre of movie
        genres = movieGenre.split("|")
        rating = float(ratingInfo[2])
        for genre in genres:
            key =userId + '|'+ genre
            # store the count of user-movieGenre into dictionary
            if key not in user_count.keys():
                user_count[key] =0
            user_count[key] += 1
            
    except  IndexError:
        continue
    except  ValueError:
        continue
        pass
    
for Id in user_count.keys():
    # Need to put User Id first, Since I want to sort by user Id to
    # find all information of one user first in reducer
    userId, genre = Id.split("|")
    print("%s\t%s\t%d"%(userId, genre, user_count[Id]))
    
