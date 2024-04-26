import argparse
from .diagram_maker import make_diagram


def main():
    parser = argparse.ArgumentParser(description='Process Apogée XML to DrawIO diagram')
    parser.add_argument('-in', '--infile', type=str, help='Input apogée XML file path')
    parser.add_argument('-out', '--outfile', type=str, help='Output DrawIO file path')
    parser.add_argument('-s', '--show', type=str, default='code_apogee', 
                        choices=['code_apogee', 'description', 'both'], help='Information to show')

    args = parser.parse_args()

    if args.infile is None or args.outfile is None:
        parser.error("Please provide both input and output file paths.")
    else:
        make_diagram(args.infile, args.outfile, to_show=args.show)

if __name__ == "__main__":
    main()