<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" xmlns="http://www.w3.org/1999/xhtml">

<xsl:output method="xml" encoding="UTF-8" doctype-system="http://www.w3.org/TR/2000/REC-xhtml1-20000126/DTD/xhtml1-strict.dtd" doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"/>

  <xsl:template match="/">
    <head>
      <title>Command Reference</title>
	  <link id="main_style" rel="stylesheet" type="text/css" href="default.css"/>
    </head>
    <body>
      <div style="float: left; margin-right: 50px; padding: 10px; background-color: #DDDDDD;">
        <strong>List of commands</strong>
        <br/><br/>
        <table>
          <xsl:for-each select="commands/command">
            <xsl:sort select="@name"/>
            <tr>
              <th>
                <a>
                  <xsl:attribute name="href">
                    <xsl:value-of select="concat('#', @name)"/>
                  </xsl:attribute>
                  <xsl:value-of select="@name"/>
                </a>
              </th>
            </tr>
          </xsl:for-each>
        </table>
      </div>
      <div style="float: left;  padding: 10px; width: 650px;">
        <strong>Command Descriptions</strong>
        <br/><br/>
        <table>
          <xsl:apply-templates select="commands/command">
            <xsl:sort select="@name"/>
          </xsl:apply-templates>
        </table>
      </div>
    </body>
  </xsl:template>

  <xsl:template match="command">
	<tr>
	  <th bgcolor="#CCCCCC">
        <a>
          <xsl:attribute name="name">
            <xsl:value-of select="@name"/>
          </xsl:attribute>
          <xsl:value-of select="@name"/>
        </a>
      </th>
	  <td>
	    <table><tr>
          <xsl:for-each select="params/param">
	        <td>
			  <xsl:value-of select="@name"/>:<a>
			    <xsl:attribute name="href">
				  <xsl:value-of select="concat('types.xml#', @type)"/>
				</xsl:attribute>

			    <xsl:value-of select="@type"/>
			  </a>
			</td>
	      </xsl:for-each>
		</tr></table>
	  </td>
    </tr>
    <tr>
      <td></td>
      <td>
        <p>
          <xsl:value-of select="desc"/>
        </p>
      </td>
    </tr>
    <tr><td colspan="2"><hr/></td></tr>

  </xsl:template>

</xsl:stylesheet>
