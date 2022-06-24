#!/usr/bin/python3

import struct
from dataclasses import dataclass
import os
import argparse
import json
from PIL import Image

parser = argparse.ArgumentParser(description='Converts tiled json map to binary format')
parser.add_argument('-ts', type=str, help='tileset file')
parser.add_argument('-tm', type=str, help='tilemap file')
parser.add_argument('-o', type=str, default='a.map', help='output file')

@dataclass
class Tile:
	id: int
	collect: bool
	is_static: bool
	interact: bool
	pass

class TileSet:
	def __init__(self, name, imgpath, size, columns):
		self.name = name
		self.img = Image.open(imgpath).convert('RGBA')
		self.size = size
		self.columns = columns
		self.tiles = []
		pass

	def get_source(self, id: int):
		x = id % self.columns
		y = id // self.columns
		src = (x * self.size, y * self.size,
			x * self.size + self.size, y * self.size + self.size
		)
		return src

	def __str__(self):
		return f'name: {self.name}, size: {self.size}, columns: {self.columns}, img: {self.img.size}'
	
	@classmethod
	def from_json(cls, js, root):
		tileset = TileSet(
			name=js['name'],
			imgpath=os.path.join(root, js['image']),
			size=js['tileheight'],
			columns=js['columns']
		)
		jtiles = js['tiles']
		for jtile in jtiles:
			tile = Tile(0, 0, False, False)
			tile.id = jtile.get('id', 0)
			props = jtile.get('properties', None)
			if props:
				for prop in props:
					if 'collect' == prop['name']:
						tile.collect = prop['value']
					elif 'interact' == prop['name']:
						tile.interact = prop['value']
					elif 'is_static' == prop['name']:
						tile.is_static = prop['value']
				pass
			tileset.tiles.append(tile)
		return tileset
		pass

class TileMap:
	def __init__(self, tileset, map, w=0, h=0):
		self.tileset = tileset
		self.map = map
		self.width = w
		self.height = h
		pass

	def __str__(self):
		return f'[{self.width}x{self.height}] | {len(self.map)}'
		pass

	@classmethod
	def from_json(cls, tileset, js):
		layer0 = js['layers'][0]
		width = js['width']
		height = js['height']
		map = []
		for id in layer0['data']:
			map.append(id - 1)
			pass
		tilemap = TileMap(tileset, map, width, height)
		return tilemap
		pass
	pass

def write_map_to_file(tm: TileMap, out: str):
	print('writing to', out)
	with open(out, 'wb') as file:
		file.write(struct.pack('<I', 0x00AABB00)) # id
		# tilemap
		tfilter = filter(lambda tile: -1 != tile[1], zip(range(len(tm.map)), tm.map))
		tlist = list(tfilter)
		print('map', tm.width, tm.height)
		print('actual tile count in map:', len(tlist))
		file.write(struct.pack('<ii', tm.width, tm.height))
		file.write(struct.pack('<i', len(tlist)))
		w = tm.width * tm.tileset.size
		h = tm.height * tm.tileset.size
		tmp = Image.new('RGBA', (w, h))
		for tile in tlist:
			index = tile[0]
			id = tile[1]
			x = (index % tm.width)
			y = int(index / tm.width)
			file.write(struct.pack('<iii', id, x, y))
			src = tm.tileset.get_source(id)
			dst = (x * tm.tileset.size, y * tm.tileset.size)
			tmp.paste(tm.tileset.img.crop(src), dst)
			# print(f'tile #{index}, {id}[{x}x{y}]')
			pass
		tmp.save('map.png')
		# tileset
		ts: TileSet = tm.tileset
		file.write(struct.pack('<i', ts.size))
		file.write(struct.pack('<i', ts.columns))
		file.write(struct.pack('<i', len(ts.tiles)))
		for tile in ts.tiles:
			flags = 0
			if tile.is_static:
				flags |= 1
			if tile.interact:
				flags |= (1 << 1)
			if tile.collect:
				flags |= (1 << 2)
			file.write(struct.pack('<I', flags))
		# image
		print(ts.img.mode)
		file.write(struct.pack('<ii', *ts.img.size))
		for y in range(ts.img.size[1]):
			for x in range(ts.img.size[0]):
				p = ts.img.getpixel((x, y))
				file.write(struct.pack('<BBBB', *p))
		pass
	pass

if '__main__' == __name__:
	opts = parser.parse_args()
	print(opts)
	if not opts.ts or not opts.tm:
		print('please provide both tileset and tilemap files')
		quit()
	
	ip = opts.ts
	root = ip.replace(os.path.basename(ip), '')
	print(root)
	with open(ip, 'r') as file:
		tsjson = json.load(file)
		tileset = TileSet.from_json(tsjson, root)
	tmap = opts.tm
	with open(tmap, 'r') as file:
		tmjson = json.load(file)
		tilemap = TileMap.from_json(tileset, tmjson)
		print(tilemap)
	
	op = opts.o
	write_map_to_file(tilemap, op)