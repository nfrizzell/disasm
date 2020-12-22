#! /usr/bin/python3
import xml.etree.ElementTree as ET
import copy

path = "x86.xml"

tree = ET.parse(path)
root = tree.getroot()

all_unique_attrib_keys = [
		'direction', 'op_size', 'nr', 'address', 'attr', 'doc1632_ref', 'post',
		'ref', 'sug', 'mode', 'depend', 'type', 'cond', 'r', 'doc', 'sign-ext', 
		'tttn', 'mod', 'doc_ref', 'doc64_ref', 'ring', 'alias', 'mem_format', 
		'fpop', 'fpush', 'lat_step', 'part_alias', '{http://www.w3.org/XML/1998/namespace}id',
		'group', 'escape', 'lock', 'exclusive', 'version', 'value', 'displayed',
		'is_undoc', 'is_doc', 'particular', 'ring_ref', 'doc_part_alias_ref', 'id', "+r"
		]

all_unique_child_tags = [
		'pri_opcd', 'entry', 'syntax', 'mnem', 'dst', 'a', 't', 'src', 'grp1', 
		'grp2', 'grp3', 'modif_f', 'def_f', 'note', 'brief', 'proc_start', 
		'undef_f', 'f_vals', 'proc_end', 'test_f', 'instr_ext', 'opcd_ext', 
		'pref', 'modif_f_fpu', 'undef_f_fpu', 'sec_opcd', 'def_f_fpu', 'sub', 
		'sup', 'f_vals_fpu', 'det', 'twobyte'
		]

type_enum = 	{
		'a':0, 'b':1, 'c':2, 'd':3, 'dq':4, 'p':5, 'pd':6, 'pi':7, 'ps':8, 'q':9, 'qq':10,
		's':11, 'sd':12, 'ss':13, 'si':14, 'v':15, 'w':16, 'x':17, 'y':18, 'z':19, 

		'bcd':20, 'bs':21, 'bsq':22, 'bss':23, 'di':24, 'dqp':25, 'dr':26, 'e':27,
		'er':28, 'psq':29, 'ptp':30, 'qi':31, 'sr':32, 'st':33, 'stx':34, 'vds':35,
		'vqp':36, 'vs':37, 'wi':38, 'va':39, 'dqa':40, 'wa':41, 'wo':42, 'ws':43,
		'da':44, 'do':45, 'qa':46, 'qs':47, 'vq':48, 'qp':49,
		}

method_enum =	{
		'A':0, 'B':1, 'C':2, 'D':3, 'E':4, 'F':5, 'G':6, 'H':7, 'I':8, 'J':9, 'L':10,
		'M':11, 'N':12, 'O':13, 'P':14, 'Q':15, 'R':16, 'S':17, 'U':18, 'V':19, 'W':20,
		'X':21, 'Y':22, 'Z':23, 'BA':24, 'BB':25, 'BD':26, 'ES':27, 'EST':28, 'SC':29, 'T':30,

		'AH':101, 'AL':102, 'AX':103, 'EAX':104, 'RAX':105, 'BH':201, 'BL':202, 
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
		'XMM12':1412, 'XMM13':1413, 'XMM14':1414, 'XMM15':1415
		}

class Entry():
	def __init__(self):
		self.properties = {}
		self.syntax_elements = []
		self.syntaxes = []

		for key in all_unique_attrib_keys + all_unique_child_tags:
			self.properties[key] = []
	
class Syntax():
	def __init__(self):
		self.mnem = ""
		self.operands = []

class Operand():
	def __init__(self):
		self.type = ""
		self.a = ""
		self.t = ""
		self.displayed = True

def recursively_parse_entry_properties(entry, element):
	if element.tag == "syntax":
		entry.syntax_elements.append(element)

	else:
		for attrib_key in element.attrib:
			entry.properties[attrib_key].append(element.attrib[attrib_key])
		if element.text not in ["", None]:
			entry.properties[element.tag].append(element.text)
		for child in element:
			recursively_parse_entry_properties(entry, child)

def parse_syntax_properties(syntax, element):
	for subelement in element:
		if subelement.tag == 'mnem':
			syntax.mnem = subelement.text

		elif subelement.tag in ['src', 'dst']:
			operand = Operand()
			parse_operand_properties(operand, subelement)
			if operand.displayed:
				syntax.operands.append(operand)
				
def parse_operand_properties(operand, element):
	operand.type = element.tag

	if 'type' in element.attrib:
		operand.t = element.attrib['type']
		if element.text.upper() in method_enum:
			operand.a = element.text.upper().strip()
		elif 'address' in element.attrib:
			operand.a = element.attrib['address'].strip()

	for subelement in element:
		if subelement.tag == 't' and subelement.text != None:
			operand.t = subelement.text.strip()
		elif subelement.tag == 'a' and subelement.text != None:
			operand.a = subelement.text.strip()

	if 'displayed' in element.attrib:
		if element.attrib['displayed'] == 'no':
			operand.displayed = False
		else:
			operand.displayed = True

def convert_operand_values(syntax):
	
	for operand in syntax.operands:
		if operand.a != "":
			operand.a = method_enum[operand.a]	
		if operand.t != "":
			operand.t = type_enum[operand.t]	

def parse_all_unique_entries(root, twobyte, unique_entries):
	for pri_opcd in root:
		opcode = pri_opcd.attrib['value']

		for subelement1 in pri_opcd:
			if subelement1.tag == 'entry':
				entry = Entry()

				recursively_parse_entry_properties(entry, subelement1)

				for syntax in entry.syntax_elements:
					s = Syntax()
					parse_syntax_properties(s, syntax)
					convert_operand_values(s)
					if len(s.mnem) > 0:
						entry.syntaxes.append(s)

				entry.properties['pri_opcd'] = opcode
				entry.properties['twobyte'] = twobyte
				rplus_key = (twobyte, entry.properties['pri_opcd'])

				if len(entry.properties['grp1']) > 0 and entry.properties['grp1'][0] == 'prefix':
					pass

				# This section is used to find and duplicate weird instructions that have different opcodes based on 
				# what register they are supposed to access (this method of addressing goes by 'Z' in the reference).
				# These instructions are given the field tag "+r" in the web reference, but this does not exist
				# in the XML reference for some reason, so I figured this is the best way to make up for it.
				elif rplus_key in [(False, '40'), (False, '48'), (False, '50'), (False, '58'), (False, '90'), (False, 'B0'), (False, 'B8'), (True, 'C8')]:
					for i in range(8):
						cop = copy.deepcopy(entry)
						new_opcd = int(entry.properties['pri_opcd'], 16)+i
						cop.properties['pri_opcd'] = "{:02x}".format(new_opcd)
						unique_entries.append(cop)

				else:
					unique_entries.append(entry)

def write_csv(unique_entries):
	columns= ['mnem', 'pref', 'twobyte', 'pri_opcd', 'sec_opcd', 'op_size', 'sign-ext', 'direction', 
			'tttn', 'mem_format', 'opcd_ext', 'mode', 'ring', 'lock', 'fpush', 
			'fpop', 'alias', 'part_alias',  'instr_ext', 'grp1', 'grp2', 'grp3',
			'test_f', 'modif_f', 'def_f', 'undef_f', 'f_vals', 'exclusive', 'brief',
			'op1_m', 'op1_t', 'op2_m', 'op2_t', 'op3_m', 'op3_t', 'op4_m', 'op4_t' ]

			
	csv_file = open("x86.csv", "w")	
	csv_file.write(",".join(columns) + ",\n")

	for entry in unique_entries:
		for syntax in entry.syntaxes:
			csv_file.write(syntax.mnem + ",")
			csv_file.write("".join(entry.properties['pref']) + ",")
			csv_file.write(str(int(entry.properties['twobyte'])) + ",")
			csv_file.write("".join(entry.properties['pri_opcd']) + ",")
			csv_file.write("".join(entry.properties['sec_opcd']) + ",")
			csv_file.write("".join(entry.properties['op_size']) + ",")
			csv_file.write("".join(entry.properties['sign-ext']) + ",")
			csv_file.write("".join(entry.properties['direction']) + ",")
			csv_file.write("".join(entry.properties['tttn']) + ",")
			csv_file.write("".join(entry.properties['mem_format']) + ",")
			csv_file.write("".join(entry.properties['opcd_ext']) + ",")
			csv_file.write("".join(entry.properties['mode']) + ",")
			csv_file.write("".join(entry.properties['ring']) + ",")
			csv_file.write("".join(entry.properties['lock']) + ",")
			csv_file.write("".join(entry.properties['fpush']) + ",")
			csv_file.write("".join(entry.properties['fpop']) + ",")
			csv_file.write("".join(entry.properties['alias']) + ",")
			csv_file.write("".join(entry.properties['part_alias']) + ",")
			csv_file.write("".join(entry.properties['instr_ext']) + ",")
			csv_file.write("".join(entry.properties['grp1']) + ",")
			csv_file.write("".join(entry.properties['grp2']) + ",")
			csv_file.write("".join(entry.properties['grp3']) + ",")
			csv_file.write("".join(entry.properties['test_f']) + ",")
			csv_file.write("".join(entry.properties['modif_f']) + ",")
			csv_file.write("".join(entry.properties['def_f']) + ",")
			csv_file.write("".join(entry.properties['undef_f']) + ",")
			csv_file.write("".join(entry.properties['f_vals']) + ",")
			csv_file.write("".join(entry.properties['exclusive']) + ",")
			csv_file.write("".join(entry.properties['brief']).replace(",", "") + ",")

			for i in range(4):
				if i < len(syntax.operands) and len(syntax.operands) > 0:
					csv_file.write(str(syntax.operands[i].a) + "," + str(syntax.operands[i].t) + ",")

			csv_file.write('\n')

	csv_file.close()

unique_entries = []
parse_all_unique_entries(root[0], False, unique_entries)
parse_all_unique_entries(root[1], True, unique_entries)
write_csv(unique_entries)
