import csv
import matplotlib.pyplot as plt

def read_countries_dictionary():
    countries_dictionary = {}
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            countries = str(row[5]).split(', ')
            for country in countries:
                if country not in countries_dictionary:
                    countries_dictionary[country] = 1
                else:
                    countries_dictionary[country] += 1
    return countries_dictionary

def write_countries_dictionary(countries_dictionary: dict):
    with open('movies.stats', "w") as output_file:
        for country, movies_amount in countries_dictionary.items():
            output_file.write(f"{country}|{str(movies_amount)}\n")


def num_of_movies_per_country_after_year(country: str, min_year: int):
    count = 0
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            if int(row[2]) >= min_year and country in str(row[5]).split(', '):
                count += 1
    return count

def num_of_movies_per_year(year: int):
    count = 0
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            if int(row[2]) == year:
                count += 1
    return count

def show_histogram():
    years = []
    with open('IMDB.csv', "r") as data_file:
        line_list = list(csv.reader(data_file))[1:]
        for row in line_list:
            years.append(int(row[2]))
    plt.hist(years, max(years)-min(years), facecolor='blue', alpha=0.5)
    plt.show()

if __name__ == "__main__":

    # task1a
    countries_dictionary = read_countries_dictionary()
    write_countries_dictionary(countries_dictionary)

    # task1b
    for country in countries_dictionary:
        print(f"Number of movies in {country} after {1950}: {num_of_movies_per_country_after_year(country, 1950)}")

    # task1c
    show_histogram()
    