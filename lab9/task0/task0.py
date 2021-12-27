import sys
from Error_summary import Error_summary

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python task0.py <file.csv>")
        sys.exit()

    err_sum = Error_summary(sys.argv[1])
    err_sum.print_data()