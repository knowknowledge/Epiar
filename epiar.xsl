<?xml version="1.0" encoding="ISO-8859-1"?>
<xsl:stylesheet version="1.0"
xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:template match="/">
  <html>
  <head>
	<link href="http://epiar.net/default.css" rel="stylesheet" type="text/css" /> 
  
	</head>
	<body>
	<div id="logo-wrap"> 
	<div id="logo"> 
		<h1><a href="http://epiar.net">Epiar.net </a></h1> 
		<h2> Open source space arcade, trading, adventure game</h2> 
	</div> 
	</div> 

	<div id="page">
		<div id="content">
			<div class="post">
			<h3><xsl:value-of select="debugSession/time"/></h3>
			<xsl:for-each select="debugSession/log">
				<h2 class="title"><a><xsl:value-of select="function"/></a></h2>
				<div class="entry"> 
					<p><xsl:value-of select="message"/> </p>
				</div> 
				<div class="meta"> 
					<p class="byline"><xsl:value-of select="type"/> - <xsl:value-of select="time"/></p> 
				</div> 
			</xsl:for-each>
			</div>
		</div>
	</div>
	</body>
	</html>
</xsl:template>
</xsl:stylesheet>
