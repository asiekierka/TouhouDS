<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="xml" encoding="UTF-8" doctype-system="http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-strict.dtd" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"/>

  <xsl:template match="/">
    <head>
      <title>Type Reference</title>
	  <link id="main_style" rel="stylesheet" type="text/css" href="default.css"/>
    </head>
    <body>
      <table>
        <xsl:apply-templates select="*"/>
      </table>
    </body>
  </xsl:template>

  <xsl:template match="type">
	<tr>
	  <th bgcolor="#CCCCCC">
	    <a>
		  <xsl:attribute name="href">
			<xsl:value-of select="concat('#', @name)"/>
		  </xsl:attribute>
		  <xsl:value-of select="@name"/>
		</a>
	  </th>
	  <td>
        <table><tr>
          <xsl:choose>
            <xsl:when test="@baseType = 'int'">
              <td>min: <xsl:value-of select="@rangeMin"/></td>
              <td>max: <xsl:value-of select="@rangeMax"/></td>
            </xsl:when>
            <xsl:when test="@baseType = 'fixed'">
              <td>format: <xsl:value-of select="@format"/> fixed point</td>
            </xsl:when>
            <xsl:when test="@baseType = 'enum'">
              <td>values:
                <xsl:for-each select="values/value">
                  <span style="padding-right: 10px;"><xsl:value-of select="."/></span>
                </xsl:for-each>
              </td>
            </xsl:when>
            <xsl:when test="@baseType = 'reference'">
              <td>reference</td>
            </xsl:when>
            <xsl:otherwise>
              <td>...</td>
            </xsl:otherwise>
          </xsl:choose>
		</tr></table>
	  </td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
