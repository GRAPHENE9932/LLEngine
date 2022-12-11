import sys
import math
import argparse
from PIL import Image
from tqdm import trange
import numpy as np

MAX_COLOR_VALUE = 255.0

def len_of_vec(vec):
    return math.sqrt(vec[0] * vec[0] + vec[1] * vec[1] + vec[2] * vec[2])

def normalize(vec):
    vec_len = len_of_vec(vec)
    return [
        vec[0] / vec_len,
        vec[1] / vec_len,
        vec[2] / vec_len
    ]

def blend_pixel(prim, sec):
    return normalize([
        prim[0] + sec[0],
        prim[1] + sec[1],
        prim[2] * sec[2]
    ])

def main():
    arg_parser = argparse.ArgumentParser(description="""
    This script merges two normal maps. Primary (with
    relatively big changes of normals) and secondary
    (small details).

    Takes image formats that python pillow image library supports.
    """, formatter_class=argparse.RawDescriptionHelpFormatter
        )
    arg_parser.add_argument("--primary", "-p", metavar="<primary normal map file>", type=str, required=True,
                help="Input primary normal map image file.")
    arg_parser.add_argument("--secondary", "-s", metavar="<secondary normal map file>", type=str, required=True,
            help="Input secondary normal map image file.")
    arg_parser.add_argument("--output", "-o", metavar="<output file>", type=str, required=True,
            help="Where to store the result.")
    args = arg_parser.parse_args()

    # Load the primary image.
    print("Loading the primary normal map...")
    prim_image = Image.open(args.primary)
    prim_image_data = np.asarray(prim_image)
    prim_image_data = prim_image_data / MAX_COLOR_VALUE * 2.0 - 1.0
    print("Loading done.")

    if prim_image_data.shape[2] != 3:
        raise RuntimeError("Primary image must have only 3 channels. Have: " + str(prim_image_data.shape[2]))

    # Load the secondary image.
    print("Loading the secondary normal map...")
    sec_image = Image.open(args.secondary)
    sec_image_data = np.asarray(sec_image)
    sec_image_data = sec_image_data / MAX_COLOR_VALUE * 2.0 - 1.0
    print("Loading done.")

    if sec_image_data.shape[2] != 3:
        raise RuntimeError("Secondary image must have only 3 channels. Have: " + str(sec_image_data.shape[2]))

    if prim_image_data.shape != sec_image_data.shape:
        raise RuntimeError("Primary and secondary images have different shapes (resolutions).")

    # Blend.
    print("Blending...")
    result_data = np.zeros(shape=prim_image_data.shape, dtype=np.dtype("f"))
    for i in trange(prim_image_data.shape[0], unit="lines"):
        for j in range(prim_image_data.shape[1]):
            result_data[i][j] = blend_pixel(prim_image_data[i][j], sec_image_data[i][j])
    print("Blending done.")

    print("Saving...")
    result = Image.fromarray(np.uint8((result_data + 1.0) / 2.0 * MAX_COLOR_VALUE))
    result.save(args.output)
    print("Saving done.")

if __name__ == "__main__":
    main()
