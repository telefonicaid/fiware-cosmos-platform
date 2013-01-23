<?xml version="1.0"?>
<!--
  ~ Telefónica Digital - Product Development and Innovation
  ~
  ~ THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
  ~ EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
  ~ WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
  ~
  ~ Copyright (c) Telefónica Investigación y Desarrollo S.A.U.
  ~ All rights reserved.
  -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:output method="html"/>
<xsl:template match="nutch-conf">
<html>
<body>
<table border="1">
<tr>
 <td>name</td>
 <td>value</td>
 <td>description</td>
</tr>
<xsl:for-each select="property">
<tr>
  <td><xsl:value-of select="name"/></td>
  <td><xsl:value-of select="value"/></td>
  <td><xsl:value-of select="description"/></td>
</tr>
</xsl:for-each>
</table>
</body>
</html>
</xsl:template>
</xsl:stylesheet>
