              Qt-LCoV Version 0.43
                   2006/07/01

1. About Qt-LCoV
-------------------

Qt-LCoV is a Component library editor for Qt-BSch3V based on Qt.

It needs Qt 4.1.2, please refer to Trolltech(http://www.trolltech.com) 
about Qt.


2. Files
---------

  readme.txt       This file
  readme-j.txt     Japanese readme
  qtlcov/          Source file
  COPYING          License

In the source file, the comment is written in Japanese language using 
character code EUC-JP. 


3. Executable environment
--------------------------

I checked compile and operation in the following environment;
  Windows XP pro SP2 + Qt 4.1.2 opensource edition
  Knoppix Linux 3.8.1 on HDD + Qt 4.1.2 opensource edition


4. How to compile
------------------

From command prompt, enter followings;
$cd qtlcov
$qmake
$make

* qtlcov.pro exists in qtlcov/.

5. Using
----------

Following is the flow of creating a symbol;
(1)Open the [Edit] menu and select the [New Component]
(2)Set the attribute of the component
  On this dialog-box, if you check the [Use pattern], you can create
  the symbol with pattern.
  The right side of window is the edit area.
  You can expand or reduce a display by using the "View" menu.

(3)Edit with the tools
 -Select
    Click to select singly.
    Button down and drag to select by area.
    You can select a pin only singly.

 -Pin
    1)Click arranges a pin as the selected state.
    2)In the selected state, you can move the pin, or change its property.
    3)Type ESC key or Click at the outside of selection area.

 -Line (Use pattern symbol only)
    1)Click to fix a starting point.
    2)Click to fix an end point. The line is placed as the selected state.
    3)In the selected state, you can move it, or change its property.
    4)Type ESC key or Click at the outside of selection area.

 -Ellipse (Use pattern symbol only)
    1)Click to fix a starting point of a bound rectangle.
    2)Click to fix a end point of a bound rectangle.
      The circle is placed as the selected state.
    3)In the selected state, you can move it, or change its property.
    4)Type ESC key or Click at the outside of selection area.

 -Polygon (Use pattern symbol only)
    1)Click to fix a first vertex of the polygon.
    2)Click to fix a second vertex of the polygon.
    3)  :
    4)At last vertex, use double click, and the vertex is placed as selected
      state.
    5)In the selected state, you can move it, or change its property.
    6)Type ESC key or Click at the outside of selection area.

    After you complete placing the polygon by double click, you can't add
    or delete a vertex.

 -Arc (Use pattern symbol only)
    1)Click to fix a center, and then move cursor horizontally.
    2)Click to fix a radius.
    3)Move cursor along the circumference of the temporary circle.
    4)Click to fix a starting point.
    5)Move cursor along the circumference of the temporary circle.
    6)Click to fix an end point, and the arc is placed as selected
      state.
    7)In the selected state, you can move it, or change its property.
    8)Type ESC key or Click at the outside of selection area.

 -Text  (Use pattern symbol only)
    1)Click at temporary place position.
	2)Setup text.
	
 -Move node
    When you select pattern singy, in move node mode, you can modify the shape
    of the pattern.


6. License
----------

This program is a free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


--------------------------------------------------------------------------------

  OKADA, Hitoshi
  URL    : http://www.suigyodo.com/online/e
  E-Mail : hitoshi@suigyodo.com
