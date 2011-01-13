#!/bin/sh

if ! which font2svg 2>&1 >/dev/null; then
	echo this script needs font2svg
	exit 1
fi

if [ ! -p pipe ]; then mkfifo pipe; fi

FONT=FreeSans
FONTPATH=../Fonts/${FONT}.ttf

if [ ! -f $FONTPATH ]; then
	echo could not find $FONTPATH
	exit 1
fi

font2svg $FONT $FONTPATH pipe 1>&- 2>&- &

perl -ne '
BEGIN { @w = (); }
if(/glyph .*unicode="&#([0-9]+);" .*horiz-adv-x="([0-9\.]+)"/){
	my ($dec, $width) = ($1, int($2));
	if($dec >= 32 and $dec <= 126){
		my $character = chr($dec);
		#print "$character has width $width\n";
		$w[$dec] = $width;
	}
}
END {
	print "glyphWidths = {\n\t";
	for(0..126){
		next if(chr($_) !~ /^[0-9A-Za-z ,\.!\-\(\)]$/);
		printf("[%s%s%s] = %s, ", chr(39), chr($_), chr(39), (defined($w[$_]) ? $w[$_] : "nil"));
		$col++;
		if($col > 8){ $col = 0; print "\n\t"; }
	}
	print "\n}\n";
}
' < pipe

rm -f pipe
