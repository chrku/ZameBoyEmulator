#!/usr/bin/env python3
import sys
import struct
import re

# Dictionary to associate registers with opcodes for 8-bit-loads
opcodes_load_regs8 = {'A': 0x3e, 'B': 0x06, 'C': 0x0e, 'D': 0x16,
                      'E': 0x1e, 'L': 0x2e, 'H': 0x26}

reg_list = ['A', 'B', 'C', 'D', 'E', 'L', 'H']

reg_comb_list = ['BC', 'DE', 'HL', 'SP', 'AF']

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

reg_indirect_to_acc = {
    '(BC)': 0x0a, '(DE)': 0x1a
}

acc_to_reg_indirect = {
    '(BC)': 0x02, '(DE)': 0x12
}

add_reg_opcodes = {
    'A' : 0x87, 'B': 0x80, 'C': 0x81, 'D': 0x82, 'E': 0x83, 'H': 0x84, 'L': 0x85
}

adc_reg_opcodes = {
    'A' : 0x8f, 'B': 0x88, 'C': 0x89, 'D': 0x8a, 'E': 0x8b, 'H': 0x8c, 'L': 0x8d
}

sub_reg_opcodes = {
    'A' : 0x97, 'B': 0x90, 'C': 0x91, 'D': 0x92, 'E': 0x93, 'H': 0x94, 'L': 0x95
}

sbc_reg_opcodes = {
    'A' : 0x9f, 'B': 0x98, 'C': 0x99, 'D': 0x9a, 'E': 0x9b, 'H': 0x9c, 'L': 0x9d
}

def do_load(tokens, output_handle):
    """Handle load commands"""
    # Check if argument is register
    print(tokens)
    parens = re.compile("\(([0-9]|[A-F]|[a-f]){4}\)")
    parens1 = re.compile("\(([0-9]|[A-F]|[a-f]){2}\)")
    imm = re.compile("([0-9]|[A-F]|[a-f]){4}")
    if tokens[1] in reg_list and tokens[2] in reg_list:
        output_handle.write(struct.pack('B', reg_to_reg_opcodes[tokens[1] + tokens[2]]))
    elif tokens[1] == '(HL)' and tokens[2] in reg_list:
        output_handle.write(struct.pack('B', reg_to_reg_indirect_opcodes[tokens[2]]))
    elif tokens[1] in reg_list and tokens[2] == '(HL)':
        output_handle.write(struct.pack('B', reg_indirect_to_reg_opcodes[tokens[1]]))
    elif tokens[1] == 'A' and (tokens[2] == '(BC)' or tokens[2] == '(DE)'):
        output_handle.write(struct.pack('B', reg_indirect_to_acc[tokens[2]]))
    elif (tokens[1] == '(BC)' or tokens[1] == '(DE)') and tokens[2] == 'A':
        output_handle.write(struct.pack('B', acc_to_reg_indirect[tokens[1]]))
    elif tokens[1] == 'A' and tokens[2] == '(C)':
        output_handle.write(struct.pack('B', 0xf2))
    elif tokens[1] == '(C)' and tokens[2] == 'A':
        output_handle.write(struct.pack('B', 0xe2))
    elif (tokens[1] == 'A' and tokens[2] == '(HLD)') or tokens[1] == 'A' and tokens[2] == '(HL-)':
        output_handle.write(struct.pack('B', 0x3a))
    elif (tokens[2] == 'A' and tokens[1] == '(HLD)') or tokens[2] == 'A' and tokens[1] == '(HL-)':
        output_handle.write(struct.pack('B', 0x32))
    elif (tokens[1] == 'A' and tokens[2] == '(HLI)') or tokens[1] == 'A' and tokens[2] == '(HL+)':
        output_handle.write(struct.pack('B', 0x2a))
    elif (tokens[2] == 'A' and tokens[1] == '(HLI)') or tokens[2] == 'A' and tokens[1] == '(HL+)':
        output_handle.write(struct.pack('B', 0x22))
    elif tokens[1] in reg_comb_list and imm.match(tokens[2]):
        if tokens[1] == 'BC':
            output_handle.write(struct.pack('B', 0x01))
        elif tokens[1] == 'DE':
            output_handle.write(struct.pack('B', 0x11))
        elif tokens[1] == 'HL':
            output_handle.write(struct.pack('B', 0x21))
        elif tokens[1] == 'SP':
            output_handle.write(struct.pack('B', 0x31))
        output_handle.write(struct.pack('B', int(tokens[2][2:4], 16)))
        output_handle.write(struct.pack('B', int(tokens[2][0:2], 16)))
    elif tokens[1] == 'SP' and tokens[2] == 'HL':
        output_handle.write(struct.pack('B', 0xf9))
    elif parens1.match(tokens[1]) and tokens[2] == 'A':
        output_handle.write(struct.pack('B', 0xe0))
        output_handle.write(struct.pack('B', int(tokens[1][1:3], 16)))
    elif parens1.match(tokens[2]) and tokens[1] == 'A':
        output_handle.write(struct.pack('B', 0xf0))
        output_handle.write(struct.pack('B', int(tokens[1][1:3], 16)))
    elif tokens[1] == 'A' and parens.match(tokens[2]):
        # Write FA opcode
        output_handle.write(struct.pack('B', 0xfa))
        # Write bytes in little endian order
        output_handle.write(struct.pack('B', int(tokens[2][3:5], 16)))
        output_handle.write(struct.pack('B', int(tokens[2][1:3], 16)))
    elif parens.match(tokens[1]) and tokens[2] == 'A':
        # Write EA opcode
        output_handle.write(struct.pack('B', 0xea))
        # Write bytes in little endian order
        output_handle.write(struct.pack('B', int(tokens[1][3:5], 16)))
        output_handle.write(struct.pack('B', int(tokens[1][1:3], 16)))
    elif tokens[1] == '(HL)':
        output_handle.write(struct.pack('B', 0x36))
        # Second argument is immediate
        output_handle.write(struct.pack('B', int(tokens[2], 16)))
    elif tokens[1] in reg_list:
        output_handle.write(struct.pack('B', opcodes_load_regs8[tokens[1]]))
        # Second argument is immediate
        output_handle.write(struct.pack('B', int(tokens[2], 16)))
    elif parens.match(tokens[1]) and tokens[2] == 'SP':
        output_handle.write(struct.pack('B', 0x08))
        output_handle.write(struct.pack('B', int(tokens[1][3:5], 16)))
        output_handle.write(struct.pack('B', int(tokens[1][1:3], 16)))
    else:
        raise ValueError('Invalid instruction')


def do_ldhl(tokens, output_handle):
    parens = re.compile("([0-9]|[A-F]|[a-f]){2}")
    if tokens[1] == 'SP' and parens.match(tokens[2]):
        output_handle.write(struct.pack('B', 0xf8))
        output_handle.write(struct.pack('B', int(tokens[2], 16)))
    else:
        raise ValueError('Invalid instruction')


def do_add(tokens, output_handle):
    parens = re.compile("([0-9]|[A-F]|[a-f]){2}")
    if tokens[1] == 'A':
        if tokens[2] in reg_list:
            output_handle.write(struct.pack('B', add_reg_opcodes[tokens[2]]))
        elif tokens[2] == '(HL)':
            output_handle.write(struct.pack('B', 0x86))
        elif parens.match(tokens[2]):
            output_handle.write(struct.pack('B', 0xc6))
            output_handle.write(struct.pack('B', int(tokens[2], 16)))
        else:
            raise ValueError('Invalid instruction')
    else:
        raise ValueError('Invalid instruction')


def do_adc(tokens, output_handle):
    parens = re.compile("([0-9]|[A-F]|[a-f]){2}")
    if tokens[1] == 'A':
        if tokens[2] in reg_list:
            output_handle.write(struct.pack('B', adc_reg_opcodes[tokens[2]]))
        elif tokens[2] == '(HL)':
            output_handle.write(struct.pack('B', 0x8e))
        elif parens.match(tokens[2]):
            output_handle.write(struct.pack('B', 0xce))
            output_handle.write(struct.pack('B', int(tokens[2], 16)))
        else:
            raise ValueError('Invalid instruction')
    else:
        raise ValueError('Invalid instruction')


def do_sub(tokens, output_handle):
    parens = re.compile("([0-9]|[A-F]|[a-f]){2}")
    if tokens[1] == 'A':
        if tokens[2] in reg_list:
            output_handle.write(struct.pack('B', sub_reg_opcodes[tokens[2]]))
        elif tokens[2] == '(HL)':
            output_handle.write(struct.pack('B', 0x96))
        elif parens.match(tokens[2]):
            output_handle.write(struct.pack('B', 0xd6))
            output_handle.write(struct.pack('B', int(tokens[2], 16)))
        else:
            raise ValueError('Invalid instruction')
    else:
        raise ValueError('Invalid instruction')


def do_sbc(tokens, output_handle):
    if tokens[1] == 'A':
        if tokens[2] in reg_list:
            output_handle.write(struct.pack('B', sbc_reg_opcodes[tokens[2]]))
        elif tokens[2] == '(HL)':
            output_handle.write(struct.pack('B', 0x9e))
        else:
            raise ValueError('Invalid instruction')
    else:
        raise ValueError('Invalid instruction')


def do_push(tokens, output_handle):
    if tokens[1] == 'AF':
        output_handle.write(struct.pack('B', 0xf5))
    elif tokens[1] == 'BC':
        output_handle.write(struct.pack('B', 0xc5))
    elif tokens[1] == 'DE:':
        output_handle.write(struct.pack('B', 0xd5))
    elif tokens[1] == 'HL':
        output_handle.write(struct.pack('B', 0xe5))
    else:
        raise ValueError('Invalid instruction')


def do_pop(tokens, output_handle):
    if tokens[1] == 'AF':
        output_handle.write(struct.pack('B', 0xf1))
    elif tokens[1] == 'BC':
        output_handle.write(struct.pack('B', 0xc1))
    elif tokens[1] == 'DE:':
        output_handle.write(struct.pack('B', 0xd1))
    elif tokens[1] == 'HL':
        output_handle.write(struct.pack('B', 0xe1))
    else:
        raise ValueError('Invalid instruction')

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
            output_handle.write(struct.pack('B', 0x0))
        elif tokens[0] == 'LD':
            do_load(tokens, output_handle)
        elif tokens[0] == 'LDHL':
            do_ldhl(tokens, output_handle)
        elif tokens[0] == 'PUSH':
            do_push(tokens, output_handle)
        elif tokens[0] == 'POP':
            do_pop(tokens, output_handle)
        elif tokens[0] == 'ADD':
            do_add(tokens, output_handle)
        elif tokens[0] == 'ADC':
            do_add(tokens, output_handle)
        elif tokens[0] == 'SUB':
            do_sub(tokens, output_handle)
        elif tokens[0] == 'SBC':
            do_sbc(tokens, output_handle)
        # Encode HALT as 0x76
        elif tokens[0] == 'HALT':
            output_handle.write(struct.pack('B', 0x76))



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
