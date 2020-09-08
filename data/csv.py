#! /usr/bin/python3
import xml.etree.ElementTree as ET

path = "x86.xml"

tree = ET.parse(path)
root = tree.getroot()

all_unique_attrib_keys = ['direction', 'op_size', 'nr', 'address', 'attr', 'doc1632_ref', 'post', 'ref', 'sug', 'mode', 'depend', 'type', 'cond', 'r', 'doc', 'sign-ext', 'tttn', 'mod', 'doc_ref', 'doc64_ref', 'ring', 'alias', 'mem_format', 'fpop', 'fpush', 'lat_step', 'part_alias', '{http://www.w3.org/XML/1998/namespace}id', 'group', 'escape', 'lock', 'exclusive', 'version', 'value', 'displayed', 'is_undoc', 'is_doc', 'particular', 'ring_ref', 'doc_part_alias_ref', 'id']

all_unique_child_tags = ['pri_opcd', 'entry', 'syntax', 'mnem', 'dst', 'a', 't', 'src', 'grp1', 'grp2', 'grp3', 'modif_f', 'def_f', 'note', 'brief', 'proc_start', 'undef_f', 'f_vals', 'proc_end', 'test_f', 'instr_ext', 'opcd_ext', 'pref', 'modif_f_fpu', 'undef_f_fpu', 'sec_opcd', 'def_f_fpu', 'sub', 'sup', 'f_vals_fpu', 'det', 'twobyte']

def recursively_handle_elements(element):
	attrib_key_as_list = list(element.attrib.keys())

	if len(attrib_key_as_list) > 0:
		for key in attrib_key_as_list:
			if key not in all_unique_attrib_keys:
				all_unique_attrib_keys.append(key)

	for child in element:
		recursively_handle_elements(child)

def recursively_get_all_child_tags(element):
	for child in element:
		if child.tag not in all_unique_child_tags:
			all_unique_child_tags.append(child.tag)
		recursively_get_all_child_tags(child)
	
class Operand():
	def __init__(self):
		self.properties = {}

		for key in all_unique_attrib_keys + all_unique_child_tags:
			self.properties[key] = []

class UniqueInstruction():
	def __init__(self):
		self.properties = {}
		self.operands = []

		for key in all_unique_attrib_keys + all_unique_child_tags:
			self.properties[key] = []

unique_instructions = []
def recursively_parse_instruction_properties(current_instruction, element):
	# if the element signals an operand, which have to be handled specially
	if element.tag in ['src', 'dst']:
		operand = Operand()

		for attrib_key in element.attrib:
			operand.properties[attrib_key].append(element.attrib[attrib_key])
		if element.text not in ["", None]:
			operand.properties[element.tag].append(element.text)
		for child in element:
			recursively_parse_instruction_properties(operand, child)

		current_instruction.operands.append(operand)

	else:
		for attrib_key in element.attrib:
			current_instruction.properties[attrib_key].append(element.attrib[attrib_key])
		if element.text not in ["", None]:
			current_instruction.properties[element.tag].append(element.text)
		for child in element:
			recursively_parse_instruction_properties(current_instruction, child)

def retrieve_unique_instructions(category, twobyte):
	for pri_opcd in category:
		for entry in pri_opcd:
			instr = UniqueInstruction()

			if twobyte:
				instr.properties['twobyte'].append('1')
			else:
				instr.properties['twobyte'].append('0')

			instr.properties['pri_opcd'].append(pri_opcd.attrib['value'])

			recursively_parse_instruction_properties(instr, entry)
			unique_instructions.append(instr)

retrieve_unique_instructions(root[0], False)
retrieve_unique_instructions(root[1], True)

def convert_operand_values(operands):
	literals = {'AH':101, 'AL':102, 'AX':103, 'EAX':104, 'RAX':105, 'BH':201, 'BL':202, 
			'BX':203, 'EBX':204, 'RBX':205, 'CH':301, 'CL':302, 'CX':303, 'ECX':304,
			'RCX':305, 'DH':401, 'DL':402, 'DX':403, 'EDX':404, 'RDX':405, 'SI':501,
			'ESI':502, 'RSI':503, 'DI':601, 'EDI':602, 'RDI':603, 'BP':701, 'EBP':702,
			'RBP':703, 'SP':801, 'ESP':802, 'RSP':803, 'SS':901, 'CS':902, 'DS':903, 
			'ES':904, 'FS':905, 'GS':906, 'IP':1001, 'EIP':1002, 'EFLAGS':1101, 'ST':
			1200, 'ST1':1201, 'ST2':1202, 'ST3':1203, 'ST4':1204, 'ST5':1205, 'ST6':
			1206, 'ST7':1207, 'MMX0':1300, 'MMX1':1301, 'MMX2':1302, 'MMX3':1303,
			'MMX4':1304, 'MMX5':1305, 'MMX6':1306, 'MMX7':1307, 'XMM0':1400, 'XMM1':
			1401, 'XMM2':1402, 'XMM3':1403, 'XMM4':1404, 'XMM5':1405, 'XMM6':1406,
			'XMM7':1407, 'XMM8':1408, 'XMM9':1409, 'XMM10':1410, 'XMM11':1411,
			'XMM12':1412, 'XMM13':1413, 'XMM14':1414, 'XMM15':1415}

	for operand in operands:
	
	return []

def write_csv():
	columns= ['pref', 'twobyte', 'pri_opcd', 'sec_opcd', 'op_size', 'sign-ext', 'direction', 
			'tttn', 'mem_format', 'opcd_ext', 'mode', 'ring', 'lock', 'fpush', 'fpop'
			'mnem', 'alias', 'part_alias', 'op1_m', 'op1_t', 'op2_m', 'op2_t',
			'op3_m', 'op3_t', 'op4_m', 'op4_t', 'instr_ext', 'grp1', 'grp2', 'grp3',
			'test_f', 'modif_f', 'def_f', 'undef_f', 'f_vals']

	for instr in unique_instructions:
		operands = convert_operand_values(instr.operands)

		prop = instr.properties

		values_to_write = {}
		
		for column in columns:
			values_to_write[column] = ""

write_csv()
