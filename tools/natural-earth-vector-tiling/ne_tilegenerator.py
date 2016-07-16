#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# This file is part of the Marble Virtual Globe.  
#
# This program is free software licensed under the GNU LGPL. You can
# find a copy of this license in LICENSE.txt in the top directory of
# the source code.
#
# Copyright 2016 Akshat Tandon <akshat.tandon@research.iiit.ac.in>
#
"""
Generates low level tiles for Marble using Natural Earth data
"""

import sys
import os
import math
import argparse
import urllib3
import zipfile
sys.path.append('../shp2osm')
sys.path.append('../vectortilecreator')
import polyshp2osm
import vectortilecreator

def unzip_file(filename, in_dir):
	print(in_dir)
	path_zip = os.path.join(in_dir, filename + '.zip')
	with zipfile.ZipFile(path_zip ,"r") as zip_ref:
		path_dir = os.path.join(in_dir, filename)
		os.mkdir(path_dir)
		zip_ref.extractall(path_dir)
		os.remove(path_zip)

def generate_url(filename):
	url = 'www.naturalearthdata.com/http//www.naturalearthdata.com/download/'
	cultural_tokens = ['admin', 'populated', 'roads', 'railroads', 'airports', 'ports', 'urban', 'parks', 'time', 'cultural']
	file_tokens = filename.split('_')
	url += file_tokens[1] + '/'
	data_type = 'physical'
	for token in file_tokens:
		if token in cultural_tokens:
			data_type = 'cultural'
			break
	url += data_type + '/'
	url += filename + '.zip'
	return url

def download(filename, in_dir):
	url = generate_url(filename)
	print('Url', url)
	http = urllib3.PoolManager()
	r = http.request('GET', url, preload_content=False)
	chunk_size = 8192
	file_size_dl = 0
	content_length = r.getheader("content-length")
	if content_length != None:
		fileSize = int(r.getheader("content-length"))
	else:
		fileSize = None
	with open(os.path.join(in_dir, filename + '.zip'), 'wb') as out:
		while True:
			data = r.read(chunk_size)
			if data is None or len(data) == 0:
				break
			file_size_dl += len(data)
			out.write(data)
			if fileSize != None:
				print ("Downloading %s: %.4f/%.4f Mb (%3.1f%%)\r" % (filename, file_size_dl / 1024.0 / 1024.0, fileSize / 1024.0 / 1024.0, file_size_dl * 100. / fileSize), end='')
			else:
				print ("Downloading %s: %.4f Mb downloaded (file size unknown)\r" % (filename, file_size_dl / 1024.0 / 1024.0), end='')
	r.release_conn()
	out.close()
	print ("Done")

def parse_file(filename, in_dir):
	level_info = {}
	path = os.path.join(in_dir, filename)
	f = open(path, 'rU')
	for line in f:
		line = line.rstrip()
		if len(line) == 0:
			continue
		if line.startswith('#'):
			continue
		if line[0] == '*':
			level = int(line[1:])
			level_info[level] = []
		else:
			level_info[level].append(line)
	return level_info

def check_existence(filename, in_dir):
	path = os.path.join(in_dir, filename)
	if not os.path.exists(path):
		download(filename, in_dir)
		unzip_file(filename, in_dir)

if __name__ == "__main__":
	parser = argparse.ArgumentParser(description='Generates low level tiles for Marble using Natural Earth data')
	parser.add_argument('file', help='a file with information containing natural datasets for specific levels.')
	parser.add_argument('-i', '--in_dir', help='directory to read/process data from', default='.')
	parser.add_argument('-o', '--out_dir', help='directory to write tiles to', default='.')
	parser.add_argument('-c', '--cache', help='directory to store intermediate files in', default='.')
	parser.add_argument('-r', '--refresh', type=int, default=-1, help='Re-download cached OSM base file if it is older than REFRESH days (-1: do not re-download)')
	parser.add_argument('-ow', '--overwrite', action='store_true', help='Create tiles even if they exist already')
	args = parser.parse_args()

	exception_names = ['ne_50m_admin_1_states_provinces_lines']

	level_info = parse_file(args.file, args.in_dir)
	for level in level_info:
		abs_file_paths = []
		for filename in level_info[level]:
			print('Checking - {}'.format(filename))
			check_existence(filename, args.in_dir)
			if filename == 'ne_10m_parks_and_protected_lands':
				path = os.path.join(args.in_dir, filename) + '/' + filename + '_area.shp'
			elif filename not in exception_names:
				path = os.path.join(args.in_dir, filename) + '/' + filename + '.shp'
			else:
				path = os.path.join(args.in_dir, filename) + '/' + filename + '_shp.shp'
			abs_file_paths.append(path)
		print('Level has following SHP datasets: ', abs_file_paths)
		polyshp2osm.run(abs_file_paths, 1, 5000000, 'tiny_planet_{}'.format(level))
		print('Tiny planetosm for Level = {} complete.'.format(level))
		f = open('bound_info_{}'.format(level), "w")
		print('tiny_planet_{}.1.osm;Level;-180.0;-86.0;180.0;86.0'.format(level), file=f)
		f.close()
		vectortilecreator.run(['bound_info_{}'.format(level)], args.cache, args.refresh, args.out_dir, args.overwrite, [level])
