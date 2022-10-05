# Use this script on all glTF files that exported from
# external programs like Blender.
# Currently, it does these manipulations with glTF:
#	- Converts texture png/jpeg/webp files to ktx2.
# Blender doesn't support KTX yet, but LLShooter
# supports KTX with ASTC only.

# Why not just using tools like gltfpack, gltf-transform or
# gltf-pipeline?
# Because they can't make KTX textures with ASTC compression,
# only UASTC, ETC1S + BasisLZ. Both are slower to load and
# harder to implement.

import os
import sys
import base64
import argparse
import warnings
from subprocess import Popen, PIPE
from tempfile import TemporaryDirectory
from PIL import Image
from gltflib import GLTF

class ExecutionError(Exception):
	def get_message(self) -> str:
		return """Error occured when trying to execute this:\n{}
stderr gave this:\n{}\nReturn code: {}""".format(
			self.command_args, self.stderr, self.return_code
		)

	def __init__(self, command_args: list, stderr: str, return_code: int):
		self.command_args = command_args
		self.stderr = stderr
		self.return_code = return_code
		
		super().__init__(self.get_message())

def is_base64_uri(uri: str) -> bool:
	return uri.endswith("data:")

def read_base64_uri(uri: str) -> bytes:
	if not is_base64_uri(uri):
		raise ValueError("Provided URI is not in base64.")

	# Skip first 5 symbols because they are "data:"
	return base64.b64decode(uri[5:])

def create_base64_uri(data: bytes) -> str:
	return "data:" + base64.b64encode(data)

def read_buffer_view(gltf: GLTF, buf_view_index: int) -> bytes:
	buf_view = gltf.model.bufferViews[buf_view_index]
	buf = gltf.model.buffers[buf_view.buffer]

	if buf_view.byteStride != None:
		raise "byteStride is not supported for textures."
	start = buf_view.byteOffset or 0
	end = start + buf_view.byteLength

	if buf.uri == None:
		return gltf.get_glb_resource().data[start:end]
	else:
		return gltf.get_resource(buf.uri).data[start:end]

def rewrite_buffer_view(gltf: GLTF, buf_view_index: int, data: bytes):
	# Get buffer view and buffer.
	buf_view = gltf.model.bufferViews[buf_view_index]
	buf = gltf.model.buffers[buf_view.buffer]

	# Gather some info.
	if buf_view.byteStride != None:
		raise "byteStride is not supported for textures."
	start = buf_view.byteOffset or 0
	old_end = start + buf_view.byteLength

	# Set new data.
	if buf.uri == None:
		resource = gltf.get_glb_resource()
	else:
		resource = gltf.get_resource(buf.uri)
	full_data = resource.data
	full_data = full_data[:start] + data + full_data[old_end:]
	resource.data = full_data

	# Change buffer view and buffer info.
	difference = len(data) - buf_view.byteLength
	buf.byteLength += difference
	buf_view.byteLength = len(data)
	# Change affected offsets in other buffer views.
	for cur_buf_view in gltf.model.bufferViews:
		is_the_same_file = gltf.model.buffers[cur_buf_view.buffer].uri == buf.uri
		is_later_in_file = buf_view.byteOffset < cur_buf_view.byteOffset

		if is_the_same_file and is_later_in_file:
			cur_buf_view.byteOffset += difference

def main():
	arg_parser = argparse.ArgumentParser(description="""
	This script prepares and optimizes common glTF files
	exported from Blender to LLShooter.
	Currently, it does these manipulations with glTF:
	- Converts texture png/jpeg/webp files to ktx2.

	Use the TOKTX_BIN environment variable to specify path
	to toktx executable. Otherwise, it will try to execute
	"toktx" instead.
	toktx tool can be obtained here:
	https://github.com/KhronosGroup/KTX-Software
	""", formatter_class=argparse.RawDescriptionHelpFormatter
	)
	arg_parser.add_argument("--astc-block-size", "-b", metavar="<XxY>", type=str, required=False,
			help="ASTC block dimensions. Implies --astc_blk_d argument of toktx.")
	arg_parser.add_argument("--astc-quality", "-q", metavar="<0-100>", type=int, required=False,
			default=100, help="ASTC encoding quality. Implies --astc_quality argument of toktx.")
	arg_parser.add_argument("--input", "-i", metavar="<input file>", type=str, required=True,
			help="Input glTF file (exported from a program like Blender).")
	arg_parser.add_argument("--output", "-o", metavar="<output file>", type=str, required=True,
			help="Where to store the result.")
	args = arg_parser.parse_args()

	# Check if the file is really glTF.
	if not (args.input.endswith(".glb") or args.input.endswith(".gltf")):
		warnings.warn("Input file has invalid/missing extension.")

	# Load glTF.
	print("Loading glTF file...")
	gltf = GLTF.load(args.input)

	# Iterate through all images.
	for cur_image in gltf.model.images:
		# Change mime type.
		cur_image.mimeType = "image/ktx2"

		# Set some toktx arguments.
		toktx_args = [
			"toktx" if os.getenv("TOKTX_BIN") is None else os.getenv("TOKTX_BIN"),
			"--genmipmap",
			"--t2",
			"--encode", "astc",
			"--astc_quality", str(args.astc_quality),
			"--zcmp", "19"
		]
		if args.astc_block_size != None:
			toktx_args.extend(["--astc_blk_d", args.astc_block_size])

		if cur_image.uri != None:
			# Calculate input and output file pathes.
			new_uri = os.path.splitext(cur_image.uri)[0] + ".ktx2"
			input_file_name = os.path.join(os.path.split(args.input)[0], cur_image.uri)
			output_file_name = os.path.join(os.path.split(args.output)[0], new_uri)
			
			# Change glTF data.
			cur_image.uri = new_uri
		elif cur_image.bufferView != None:
			# Create temporary directory and files in it.
			temp_dir = TemporaryDirectory()

			input_file_name = os.path.join(temp_dir.name, "original_image")
			input_temp_file = open(input_file_name, "wb+")
			input_temp_file.write(read_buffer_view(gltf, cur_image.bufferView))

			output_file_name = os.path.join(temp_dir.name, "ktx_image.ktx2")
		else:
			sys.exit("Image has no source.")
		
		# Convert the image if it is not in PNG or JPEG format.
		# toktx accepts only these.
		image = Image.open(input_temp_file)
		if image.format != "PNG" and image.format != "JPEG":
			print("Converting image from {} format to PNG".format(image.format))
			image.save(input_temp_file, "PNG")

		# Add corresponding toktx arguments.
		toktx_args.extend([
			output_file_name,
			input_file_name
		])

		# Execute command.
		print("EXECUTING COMMAND:", *toktx_args, sep=" ")
		process = Popen(toktx_args, stdout=PIPE, stdin=PIPE, stderr=PIPE)
		stdout, stderr = process.communicate()
		if process.returncode != 0:
			raise ExecutionError(toktx_args, stderr, process.returncode)

		if cur_image.uri == None:
			# Read temporary files.
			output_temp_file = open(output_file_name, "rb")
			rewrite_buffer_view(gltf, cur_image.bufferView, output_temp_file.read())
	
	print("Saving glTF file...")
	gltf.export(args.output)

	print("Done!")

if __name__ == "__main__":
	try:
		main()
	except ExecutionError as err:
		sys.exit(err.get_message())