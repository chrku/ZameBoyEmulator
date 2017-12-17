#!/usr/bin/env python3
import sys
import struct
import re

# Dictionary to associate registers with opcodes for 8-bit-loads
opcodes_load_regs8 = {'A': 0x3e, 'B': 0x06, 'C': 0x0e, 'D': 0x16,
                      'E': 0x1e, 'L': 0x2e, 'H': 0x26}

reg_list = ['A', 'B', 'C', 'D', 'E', 'L', 'H']

reg_to_reg_opcodes = {
    'AA': 0x7f, 'AB': 0x78, 'AC': 0x79, 'AD': 0x7a, 'AE': 0x7b, 'AH': 0x7c, 'AL': 0x7d,
    'BA': 0x47, 'BB': 0x40, 'BC': 0x41, 'BD': 0x42, 'BE': 0x43, 'BH': 0x44, 'BL': 0x45,
    'CA': 0x4f, 'CB': 0x48, 'CC': 0x49, 'CD': 0x4a, 'CE': 0x4b, 'CH': 0x4c, 'CL': 0x4d,
    'DA': 0x57, 'DB': 0x50, 'DC': 0x51, 'DD': 0x52, 'DE': 0x53, 'DH': 0x54, 'DL': 0x55,
    'EA': 0x5f, 'EB': 0x58, 'EC': 0x59, 'ED': 0x5a, 'EE': 0x5b, 'EH': 0x5c, 'EL': 0x5d,
    'HA': 0x67, 'HB': 0x60, 'HC': 0x61, 'HD': 0x62, 'HE': 0x63, 'HH': 0x64, 'HL': 0x65,
    'LA': 0x6f, 'LB': 0x68, 'LC': 0x69, 'LD': 0x6a, 'LE': 0x6b, 'LH': 0x6c, 'LL': 0x6d
}

reg_to_reg_indirect_opcodes = {
    'A' : 0x77, 'B': 0x70, 'C': 0x71, 'D': 0x72, 'E': 0x73, 'H': 0x74, 'L': 0x75
}

reg_indirect_to_reg_opcodes = {
    'A': 0x7e, 'B': 0x46, 'C': 0x4e, 'D': 0x56, 'E': 0x5e, 'H': 0x66, 'L': 0x6e
}


def do_load(tokens, output_handle):
    """Handle load commands"""
    # Check if argument is register
    if tokens[1] in reg_list and tokens[2] in reg_list:
        output_handle.write(struct.pack('B', reg_to_reg_opcodes[tokens[1] + tokens[2]]))
    elif tokens[1] == '(HL)' and tokens[2] in reg_list:
        output_handle.write(struct.pack('B', reg_to_reg_indirect_opcodes[tokens[2]]))
    elif tokens[1] in reg_list and tokens[2] == '(HL)':
        output_handle.write(struct.pack('B', reg_indirect_to_reg_opcodes[tokens[1]]))
    elif tokens[1] in reg_list:
        output_handle.write(struct.pack('B', opcodes_load_regs8[tokens[1]]))
        # Check if second argument is immediate
        print(tokens[2])
        if tokens[2].isdigit():
            output_handle.write(struct.pack('B', int(tokens[2])))


def assemble_GBA(input_file, output_file):
    """Assembles given GBA assembly file into a ROM output
    file. Raises errors if opcodes not found."""
    input_handle = open(input_file, 'r')
    output_handle = open(output_file, 'wb')
    pattern = re.compile(r'\s+')
    # Write 100 NULL bytes
    for i in range(0, 0x100):
        output_handle.write('\0'.encode('ascii'))
    lines = input_handle.readlines()
    for line in lines:
        # Split and strip whitespace
        tokens = line.split(' ')
        tokens = [re.sub(pattern, '', x) for x in tokens]
        # NOP get encoded as a zero byte
        if tokens[0] == 'NOP':
            output_handle.write('\0'.encode('ascii'))
        elif tokens[0] == 'LD':
            do_load(tokens, output_handle)
        # Encode HALT as 0x76 ('v')
        elif tokens[0] == 'HALT':
            output_handle.write('v'.encode('ascii'))



# Entry point to function
if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage ./gbasm.py input_file output_file")
    else:
        try:
            assemble_GBA(sys.argv[1], sys.argv[2])
        except FileNotFoundError:
            print("Input file not found")
        except PermissionError:
            print("Permission for opening file not granted")
        except IOError:
            print("File IO error")
