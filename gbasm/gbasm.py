#!/usr/bin/env python3
import sys

# Dictionary to associate registers with opcodes for 8-bit-loads
opcodes_load_regs8 = {'A' : 0x3e, 'B' : 0x06, 'C' : 0x0e, 'D' : 0x17,
					  'E' : 0x1e, 'L' : 0x2e, 'H' : 0x26}

def do_load(tokens, output_handle):
	"""Handle load commands"""
	# Check if argument is register
	if tokens[1] in opcodes_load_regs8:
		output_handle.write(struct.pack('B', opcodes_load_regs8[tokens[1]]))
	# Check if second argument is immediate
	if tokens[2].isdigit():
		output_handle.write(struct.pack('B', int(tokens[2]))
		

def assembleGBA(input_file, output_file):
	"""Assembles given GBA assembly file into a ROM output
	file. Raises errors if opcodes not found."""
	input_handle = open(input_file, 'r')
	output_handle = open(output_file, 'wb')
	# Write 100 NULL bytes
	for i in range(0, 100):
		output_handle.write('\0'.encode('ascii'))
	lines = input_handle.readlines()
	for line in lines:
		# Split and strip whitespace
		tokens = line.split(' ')
		if tokens[0] == 'NOP':
			output_handle.write('\0'.encode('ascii'))
		elif tokens[0] == 'LD':
			do_load(tokens, output_handle)
			

# Entry point to function
if __name__ == "__main__":
	if len(sys.argv) != 3:
		print("Usage ./gbasm.py input_file output_file")
	else:
		try:
			assembleGBA(sys.argv[1], sys.argv[2])
		except FileNotFoundError:
			print("Input file not found")
		except PermissionError:
			print("Permission for opening file not granted")
		except IOError:
			print("File IO error")
		except:
			print("Unknown error")
