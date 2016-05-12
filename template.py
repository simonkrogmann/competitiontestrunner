# pylint: disable=missing-docstring
import sys


def problem(case):
    return case


def main():
    if len(sys.argv) == 1:
        pass
    name = sys.argv[1]
    with open(name, 'r') as input_file:
        content = input_file.read()
    lines = content.splitlines()
    num = lines[0]
    result = ''
    for run in range(int(num)):
        case = lines[1 + run]
        result += 'Case #{}: {}\n'.format(1 + run, problem(case))
    with open(name.replace('in', 'sol'), 'w') as result_file:
        result_file.write(result)

if __name__ == '__main__':
    main()
