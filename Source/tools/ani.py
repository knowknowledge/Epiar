#!/usr/bin/env python

##	Simple tool for dealing with Animation (.ani) files
#
#	The .ani file is unique to Epiar.  It's a simple png package that
#	includes a time delay instructing the Epiar engine how long each
#	frame lasts.  Since it's a Epiar-only format, we need this
#	specialized parser to create and edit these files.
#
#	@author Matt Zweig

import os
import sys
import struct
from optparse import OptionParser

##	The version value should be changed whenever the Animation format changes
__version__ = 1

USAGE = """
pass .ani files to unpack into folders:
	%prog [ANIMATION ...]
or pass folders fo construct .ani files:
	%prog [FOLDER ..]

Animation Folders should contain:
	*.png files
	delay.txt (Optional)
		A one line file with a integer number of milliseconds for each frame.
		If a delay.txt is not included, the user will be prompted for a time.
	order.txt (Optional)
		A list of the png files in the order they should be displayed.
		If a order.txt file is not included, files will be sorted alphabetically.
"""

## Parse command line options
def Parse():
	parser = OptionParser(USAGE)
	# Overrides
	parser.add_option("-n", "--name", metavar="NAME", help="Override output name.")
	parser.add_option("-d", "--delay",  help="Override the time delay (In Milliseconds).")
	# Printing
	parser.add_option("-p", "--packed", dest='format', action='store_const', const='file', help="Create packed .ani files")
	parser.add_option("-u", "--unpacked", dest='format', action='store_const', const='folder', help="Create unpacked Animation folders")
	# Printing
	parser.add_option("-v", "--verbose", default=False, action="store_true", help="Lots of output")
	parser.add_option("-q", "--quiet", dest='verbose', action="store_true", help="No output")
	# Output files
	parser.add_option("-f", "--force", action="store_true", help="Force file overwrites.")
	parser.add_option("-!", "--no-output", action="store_true", help="Do not create any files.")
	return parser.parse_args()

##	Removes a file or folder that conflicts with the animation destination

def forceRemove( somepath):
	""" Deletes a file or folder. """
	if os.path.isdir( somepath ):
		for innerpath in os.listdir( somepath ):
			forceRemove( os.path.join( somepath,innerpath ) )
		os.removedirs( somepath )
	else:
		os.remove( somepath )

##	This is an abstraction of an Animation
#
#	By using an intermediate class rather than to functions to turn
#	filesToFolders and foldersToFiles, we make it possible to add other
#	input or output methods.
#
#	The 'save' method is set to the most likely destination format.

class Animation:
	""" The Animation Class is used for converting to and from various formats. """
	##	Create an Animation from a generic source
	def __init__(self, source):
		if os.path.exists(source):
			if os.path.isdir(source):
				# Pack a Folder
				self.fromFolder(source)
				# into a .ani file (by default)
				self.save = self.toFile
			else:
				# Unpack a .ani file
				self.fromFile(source)
				# into a Folder (by default)
				self.save = self.toFolder
		else:
			print "ERROR: The path %s does not exist" % source
			sys.exit(1)

	##	Generate a string suitable for printing
	def __str__(self):
		ret = ""
		ret += "VERSION: %d\n" % self.version
		ret += "FRAMES:  %d\n" % self.count
		ret += "DELAY:   %d\n" % self.delay
		for i,framename in enumerate(self.order):
			frame = self.frames[framename]
			ret += "[%03d] %s %1.2f kb\n" % (i,framename,len(frame)/1024.0)
		return ret

	## 
	def sanitize(self):
		# Ensure that delay is valid
		try:
			while (self.delay <= 0) or (self.delay >=256):
				attempt = raw_input("Enter delay in milliseconds [1..255]:")
				if attempt.isdigit():
					self.delay = int(attempt)
		except KeyboardInterrupt:
			print "\nCanceled"
			sys.exit(0)

	##	Collect Animation data from a packed .ani file
	def fromFile(self, filename ):
		self.name = os.path.splitext( os.path.basename( filename ) )[0]
		if not os.path.exists(filename):
			print "ERROR: Folder %s does not exists." % filename
			sys.exit(2)
		file = open(filename,'rb')
		# Get header
		self.version = ord( file.read(1) )
		if self.version != __version__:
			print "WARNING: version %d is unknown!" % self.version
		self.count = ord( file.read(1) )
		self.delay = ord( file.read(1) )
		self.order = []
		self.frames = {}
		# Get each png
		for i in range(self.count):
			size = struct.unpack("I", file.read(4))[0]
			data = file.read(size)
			framename = "%s_%03d.png" % (self.name, i)
			self.order.append(framename)
			self.frames[framename] = data

	##	Collect Animation data from an unpacked folder
	def fromFolder(self, foldername ):
		if not os.path.exists(foldername):
			print "ERROR: Folder %s does not exists." % foldername
			sys.exit(3)
		self.name = foldername
		self.name = self.name.strip("/\\")
		self.name = os.path.split(self.name)[1]
		self.name = self.name.replace("_unpacked","")
		# Know Version
		self.version = __version__
		# Get Delay
		self.delay = 0
		delaypath =  os.path.join(foldername,"delay.txt") 
		if not os.path.exists(delaypath):
			print "File %s does not exist." % delaypath
		else:
			delayfile = open( delaypath, 'r' )
			firstline = delayfile.readline().strip()
			delayfile . close()
			if firstline.isdigit() and int(firstline)>0 and int(firstline)<256:
				self.delay = int(firstline)
			else:
				print "WARNING: Delay '%s' isn't a valid delay" % firstline
			
		# Get Order
		orderpath = os.path.join(foldername, "order.txt")
		if os.path.exists(orderpath):
			orderfile = open(orderpath)
			order = orderfile.readlines()
			orderfile.close()
			cleanup = lambda s: s.strip()
			order = map(cleanup, order)
		else: 
			print "WARNING: %s does not exist.  Frames will be ordered alphabetically." % orderpath
			order = os.listdir(foldername)
		ispng = lambda p: p.lower().endswith(".png")
		self.order = filter( ispng, order )
		# Get Frames
		self.frames = {}
		for filename in self.order:
			name, ext = os.path.splitext( filename.lower() )
			if not ext == ".png":
				print "Unknown file '%s'" % filename
				continue
			filepath = os.path.join(foldername,filename)
			file = open(filepath)
			data = file.read()
			file . close()
			self.frames[filename] = data
		# Get Count
		self.count = len( self.frames )

	##	Create a .ani file
	def toFile(self, verbose=False, force=False):
		""" Save an animation as a file """
		filename = self.name
		filename += ".ani"
		if os.path.exists( filename ):
			if force:
				forceRemove( filename )
			else:
				print "ERROR: File %s already exists. Use '--force' to overwrite." % filename
				sys.exit(4)
		if verbose:
			print "Creating Animation file: %s" % filename
		file = open( filename, "wb")
		header = struct.pack("BBB", self.version, self.count, self.delay )
		file . write( header )
		for framename in self.order:
			frame = self.frames[framename]
			file . write( struct.pack("I",len(frame))) 
			file . write( frame ) 
		file.close()

	##	Create an unpacked folder
	def toFolder(self, verbose=False, force=False):
		""" Save an animation as a folder """
		# Check for Folder
		foldername = self.name
		foldername += "_unpacked"
		if os.path.exists(foldername):
			if force:
				forceRemove( foldername )
			else:
				print "ERROR: Folder %s already exists. Use '--force' to overwrite." % foldername
				sys.exit(5)
		if verbose:
			print "Creating Animation folder: %s" % foldername
		# Create Folder
		if verbose:
			print "Creating folder '%s' to store the frames..." % foldername
		os.mkdir(foldername)
		# Create Delay
		delaypath = os.path.join( foldername, "delay.txt" )
		if verbose:
			print "Creating delay file: %s" % delaypath
		delayfile = open(delaypath, "w")
		delayfile . write( str(self.delay) )
		delayfile . close()
		# Create Order
		orderpath = os.path.join( foldername, "order.txt" )
		if verbose:
			print "Creating order file: %s" % orderpath
		orderfile = open(orderpath, "w")
		# Create Files
		if verbose:
			print "Creating Frames:"
		for i,framename in enumerate(self.order):
			frame = self.frames[framename]
			if self.name:
				pngname = self.name +"_%03d.png" % i
			else:
				pngname = framename
			pngpath = os.path.join(foldername,pngname)
			print >> orderfile, pngname
			if verbose:
				print "\t%s" % pngname
			png = open(pngpath, "wb")
			png . write( frame )
			png . close()
		orderfile . close()


##	The normal execution path of this script
#
#	This gathers user supplied paths and converts each one.
def main():
	(opts,args) = Parse() 
	
	for file in args:
		ani = Animation(file)
		# Override parameters?
		if opts.name:
			ani.name = opts.name
		if opts.delay:
			assert(opts.delay.isdigit())
			ani.delay = int(opts.delay)
		ani.sanitize()
		if opts.verbose:
			print ani
		# Output results
		if not opts.no_output:
			# File Formats
			if opts.format == 'file':
				if opts.verbose:
					print "Using the .ani file format..."
				ani.toFile(verbose=opts.verbose, force=opts.force)
			elif opts.format == 'folder':
				if opts.verbose:
					print "Using the Animation folder format..."
				ani.toFolder(verbose=opts.verbose, force=opts.force)
			else:
				ani.save(verbose=opts.verbose, force=opts.force)

# This is the 'pythonic' way of calling main
if __name__ == "__main__":
	main()
