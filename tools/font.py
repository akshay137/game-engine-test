#!/usr/bin/python3

from dataclasses import dataclass
import os
import struct
import argparse
import json
from typing import List
from PIL import Image

@dataclass
class ivec2:
	x: int
	y: int

@dataclass
class Glyph:
	code: int
	pos: ivec2
	size: ivec2
	bearing: ivec2
	advance: int

if '__main__' == __name__:
	parser = argparse.ArgumentParser(description='convert atlas json+png to .atlas')
	parser.add_argument('-o', type=str, default='out.atlas', help='out file')
	parser.add_argument('-i', type=str, help='input file')

	opts = parser.parse_args()

	if not opts.i:
		print('please provide input file')
		quit()
	
	with open(opts.i, 'rt') as atlasf:
		s = atlasf.read()
		atlas = json.loads(s)
	
	base = opts.i.replace(os.path.basename(opts.i), '')
	print(base)
	img = os.path.join(base, atlas['image'])
	print(img)
	image = Image.open(img)

	data: List[Glyph] = []
	for g in atlas['data']:
		d = Glyph(g['codepoint'],
			ivec2(g['pos'][0], g['pos'][1]),
			ivec2(g['size'][0], g['size'][1]),
			ivec2(g['bearing'][0], g['bearing'][1]),
			g['advance']
		)
		data.append(d)
		pass

	# sort data list by codepoint
	data = sorted(data, key=lambda g: g.code)

	# write atlas
	print('writing to', opts.o)
	with open(opts.o, 'wb') as out:
		out.write(struct.pack('<I', 0x00AABB00)) # identifier
		out.write(struct.pack('<I', len(data))) # number of code points
		out.write(struct.pack('<ii', *image.size)) # resolution of atlas image
		
		for code in data:
			out.write(struct.pack('<I', code.code))
			out.write(struct.pack('<ii', code.pos.x, code.pos.y))
			out.write(struct.pack('<ii', code.size.x, code.size.y))
			out.write(struct.pack('<ii', code.bearing.x, code.bearing.y))
			out.write(struct.pack('<I', code.advance // 64))
		
		# write image
		for y in range(image.size[1]):
			for x in range(image.size[0]):
				pix = image.getpixel((x, y))
				# print(pix)
				out.write(struct.pack('<B', pix))
			