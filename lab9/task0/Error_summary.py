from plumbum import local
awk = local["awk"]
sed = local["sed"]
sort = local["sort"]
uniq = local["uniq"]
wc = local["wc"]

class Error_summary:

    def __init__(self, file):
        self.file = file
        self.student_list = self.get_student_list()
        self.error_code_list = self.get_error_code_list()

    def get_student_list(self):
        student_list = awk['{print $1}'][self.file]().split()
        return student_list

    def get_student_amt(self):
        student_amt = len(self.student_list)
        return student_amt

    def get_error_code_list(self):
        error_code_line_list = awk['{print $2}'][self.file]().split()

        dup_error_code_list_with_num = []
        for error_code_line in error_code_line_list:
            dup_error_code_list_with_num += error_code_line.split("|")

        dup_error_code_list = []
        for error_code_line in dup_error_code_list_with_num:
            dup_error_code_list.append(error_code_line.split(":")[0])

        error_code_list = []
        error_code_set = set(dup_error_code_list)
        for error in error_code_set:
            count = 0
            for dup_error in dup_error_code_list:
                if dup_error == error:
                    count += 1
            error_code_list.append([error, count])
        return error_code_list

    def print_data(self):
        print("List of of all students:")
        print(self.student_list)
        print("\nStudent amount:")
        print(len(self.student_list))
        print("\nError list:")
        print(self.error_code_list)
        print("\nUnique error amount:")
        count = 0
        for error in self.error_code_list:
            if error[1] == 1:
                count += 1
        print(count)