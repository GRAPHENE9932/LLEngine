import argparse
import math

def calculate_threshold_x(threshold_y: float) -> float:
    return math.sqrt(-1.0 * math.log(threshold_y))

def compute_the_filter_values(threshold: float, samples: int) -> list:
    step = calculate_threshold_x(threshold) / samples
    values = [None] * samples
    for i in range(samples):
        x = i * step
        values[i] = math.exp(-1.0 * x * x)
    
    return values

def main():
    arg_parser = argparse.ArgumentParser(description="""
    This script computes the gaussian filter values from the center of the filter to its edge.
    +____________________+
    | ------======------ |
    | ---============--- |
    | -================- |
    | ================== |
    | ========O123456789A|
    | ================== |
    | -================- |
    | ---============--- |
    | ------======------ |
    +____________________+
    This script returns filter values on the 0123456789A points.
    """, formatter_class=argparse.RawDescriptionHelpFormatter
        )
    arg_parser.add_argument("--threshold", "-t", metavar="<filter cutoff threshold>", type=float, required=True, default=0.025,
            help="The filter value after which the values are considered too small and therefore insignificant enough to ignore them.")
    arg_parser.add_argument("--samples", "-s", metavar="<samples count>", type=int, required=True, default=256,
            help="The amount of samples to output, also known as filter resolution.")
    arg_parser.add_argument("--output-separator", "-o", metavar="<output separator>", type=str, required=False, default=", ",
            help="The string with which all the output values will be separated.")
    args = arg_parser.parse_args()

    values = compute_the_filter_values(args.threshold, args.samples)
    for value in values:
        print(value, end=args.output_separator)
    print()

if __name__ == "__main__":
    main()