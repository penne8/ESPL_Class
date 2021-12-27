import csv

def read_genres_dictionary():
    genres_dictionary = {}
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            genres = str(row[3]).split(", ")
            duration = int(row[4])
            for genre in genres:
                if genre not in genres_dictionary:
                    genres_dictionary[genre] = [duration, 1]
                else:
                    genres_dictionary[genre] = [genres_dictionary[genre][0] + duration, genres_dictionary[genre][1] + 1]
    return genres_dictionary

def write_genres_dictionary(genres_dictionary: dict):
    with open('genre.stats', "w") as output_file:
        for genre, genre_data in genres_dictionary.items():
            output_file.write(f"{genre}|{str(float(genre_data[0])/float(genre_data[1]))}\n")

def genres_counts_per_country():
    countries_dictionary = {}
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            genres = str(row[3]).split(", ")
            countries = str(row[5]).split(", ")
            for country in countries:
                if country not in countries_dictionary:
                    countries_dictionary[country] = {}
                for genre in genres:
                    if genre not in countries_dictionary[country]:
                        countries_dictionary[country][genre] = 1
                    else:
                        countries_dictionary[country][genre] += 1
    return countries_dictionary

if __name__ == "__main__":

    # task2a
    genres_dictionary = read_genres_dictionary()
    write_genres_dictionary(genres_dictionary)

    # task2b
    print(genres_counts_per_country())
    